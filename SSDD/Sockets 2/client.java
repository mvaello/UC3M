import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import gnu.getopt.Getopt;

class client {

	private static boolean _debug = false;
	private static String _server = null;
	private static int _port = -1;
	private static DataOutputStream _out;
	private static DataInputStream _in;

	private static int SOCK_BUFFER = 1100;
	private static int SWAP_BUFFER = 1024;
	private static int HASH_BUFFER = 1024;
	private static int CHECK_BUFFER = 1024;

	private static byte OP_QUIT = 1;
	private static byte OP_PING = 2;
	private static byte OP_SWAP = 3;
	private static byte OP_SWAP_ITER = 4;
	private static byte OP_HASH = 5;
	private static byte OP_HASH_ITER_PET = 6;
	private static byte OP_HASH_ITER_RES = 7;
	private static byte OP_CHECK = 8;
	private static byte OP_CHECK_ITER_PET = 9;
	private static byte OP_CHECK_ITER_RES = 10;
	private static byte OP_STAT = 11;
	private static int STAT_CODES = 5;

	private static byte CHECK_OK = 0;
	private static byte CHECK_FAIL = 1;
	private static String CHECK_STRING_OK = "OK";
	private static String CHECK_STRING_FAIL = "FAIL";

	private static int HASH_MOD = 1000000;

	private static int STRUCT_SWAP_SIZE = 9;
	private static int STRUCT_SWAP_ITER_SIZE = 17 + SWAP_BUFFER;
	private static int STRUCT_HASH_SIZE = 9;
	private static int STRUCT_HASH_ITER_PET_SIZE = 9 + HASH_BUFFER;
	private static int STRUCT_HASH_ITER_RES_SIZE = 13;
	private static int STRUCT_CHECK_SIZE = 13;
	private static int STRUCT_CHECK_ITER_PET_SIZE = 9 + CHECK_BUFFER;
	private static int STRUCT_CHECK_ITER_RES_SIZE = 14;
	private static int STRUCT_STAT_SIZE = 21;
	
	static void f_ping(){
		if (_debug)
			System.out.println("PING");

		//Guardamos el tiempo de sistema en nanosegundos.
		long nano_start_time = System.nanoTime();

		//Mandamos la petición de PING.
		try {
			_out.write(OP_PING);
			_out.flush();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al enviar datos al servidor.");
			return;
		}

		//Esperamos una respuesta y guardamos el tiempo justo después.
		byte response;
		try {
			response = _in.readByte();
		}
		catch (Exception e) {
			System.out.println("[ERROR] Fallo al recibir datos del servidor.");
			return;
		}
		long nano_end_time = System.nanoTime();

		//Calculamos la latencia o tiempo de respuesta.
		long latency = (nano_end_time - nano_start_time) / (2 * 1000);
		long micro = 1000000;
		long latency_seconds = latency / micro;
		long latency_microseconds = latency % micro;

		//Si la respuesta se corresponde con una de tipo PING mostramos la latencia.
		if(response == OP_PING) {
			System.out.printf("%d.%06d\n", latency_seconds, latency_microseconds);
		}
	}

	static void f_swap(String src, String dst) {
		if (_debug)
			System.out.println("SWAP <SRC=" + src + "> <DST=" + dst +">");

		//Descriptores de fichero.
		File fdin = new File(src);
		File fdout = new File(dst);

		//Comprobamos si existe el fichero de entrada y si está vacío.
		if(!fdin.exists()) {
			System.out.println("[ERROR] Error al crear el descriptor del fichero de entrada.");
			return;
		}
		int fdin_length = (int) fdin.length();
		if(fdin_length == 0) {
			System.out.println("[ERROR] El fichero de entrada está vacío.\n");
			return;
		}

		//Borramos el fichero de salida si existe.
		if(fdout.exists()) {
			if(!fdout.delete()) {
				System.err.println("[ERROR] Error al vaciar el fichero de salida.");
				return;
			}
		}

		//Calcular el número de iteraciones a realizar.
		int iter = (int) ((fdin_length / SWAP_BUFFER) + 1);

		//Creamos un ByteBuffer para poder leer tipos de datos en otro formato (BIG_ENDIAN).
		byte[] buffer = new byte[SOCK_BUFFER];
		ByteBuffer bb = ByteBuffer.wrap(buffer);
		bb.order(ByteOrder.BIG_ENDIAN);

		//Mandamos una petición de SWAP al servidor.
		bb.put(OP_SWAP);
		bb.putInt(iter);
		bb.putInt(fdin_length);
		try {
			_out.write(buffer, 0, STRUCT_SWAP_SIZE);
			_out.flush();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al enviar datos al servidor.");
			return;
		}

		//Creamos un stream para leer del fichero de entrada y otro para el fichero de salida.
		byte[] text = new byte[SWAP_BUFFER];
		InputStream is = null;
		OutputStream os = null;
		try {
			is = new FileInputStream(fdin);
			os = new FileOutputStream(fdout);
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al crear streams para lectura/escritura de ficheros.");
			return;
		}

		//Mandamos una petición por cada bloque de texto.
		int bytes_read = 0;
		int swapped_global_calc = 0, swapped_global = 0;
		for(int i = 0; i < iter; i++) {
			//Leer del fichero de entrada un máximo de SWAP_BUFFER.
			try {
				bytes_read = is.read(text);
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al leer datos del fichero de entrada.");
				return;
			}

			//Mandamos la iteración de SWAP.
			bb.clear();
			bb.put(OP_SWAP_ITER);
			bb.putInt(i);
			bb.putInt(0);
			bb.putInt(0);
			bb.putInt(bytes_read);
			bb.put(text);
			try {
				_out.write(buffer, 0, STRUCT_SWAP_ITER_SIZE);
				_out.flush();
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al enviar datos al servidor.");
				return;
			}

			//Esperamos la respuesta del servidor.
			byte iter_res_op = 0;
			int iter_res_id = 0, iter_res_swapped = 0, iter_res_swapped_global = 0, iter_res_text_length;
			try {
				bb.clear();
				_in.read(buffer, 0, STRUCT_SWAP_ITER_SIZE);
				iter_res_op = bb.get();
				iter_res_id = bb.getInt();
				iter_res_swapped = bb.getInt();
				iter_res_swapped_global = bb.getInt();
				iter_res_text_length = bb.getInt();
				bb.get(text, 0, iter_res_text_length);
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al recibir datos del servidor.");
				return;
			}

			//Comprobamos que el mensaje de respuesta sea del tipo SWAP_ITER.
			if(iter_res_op != OP_SWAP_ITER) {
				System.err.println("[ERROR] Identificador de operación incorrecto.");
				return;
			}

			//Comprobar si la id es correcta.
			if(i != iter_res_id) {
				System.err.println("[ERROR] Identificador de swap incorrecto: id[" + i + "] != " + iter_res_id);
				return;
			}

			//Actualizamos los caracteres intercambiados.
			swapped_global_calc += iter_res_swapped;
			swapped_global = iter_res_swapped_global;

			//(DEBUG) Información sobre la iteración del swap.
			if(_debug) {
				System.out.println("swap iter[" + i +"] " + iter_res_swapped + "/" + iter_res_text_length);
			}

			//Escribimos los bytes intercambiados al fichero de salida.
			try {
				os.write(text, 0, iter_res_text_length);
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al escribir datos en el fichero de salida.");
				return;
			}
		}

		//Cerramos los streams de lectura/escritura.
		try {
			is.close();
			os.close();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al cerrar los streams de lectura/escritura.");
			return;
		}

		//Mostramos los swaps realizados.
		if(swapped_global_calc == swapped_global) {
			System.out.println(swapped_global);
		}
		else {
			System.out.println("client[" + swapped_global_calc + "] vs server[" + swapped_global + "]");
		}
	}

	static void f_hash(String src){
		if (_debug)
			System.out.println("HASH <SRC=" + src + ">");
	
		//Descriptores de fichero.
		File fdin = new File(src);

		//Comprobamos si existe el fichero de entrada y si está vacío.
		if(!fdin.exists()) {
			System.out.println("[ERROR] Error al crear el descriptor del fichero de entrada.");
			return;
		}
		int fdin_length = (int) fdin.length();
		if(fdin_length == 0) {
			System.out.println("[ERROR] El fichero de entrada está vacío.\n");
			return;
		}

		//Calcular el número de iteraciones a realizar.
		int iter = (int) ((fdin_length / HASH_BUFFER) + 1);

		//Creamos un ByteBuffer para poder leer tipos de datos en otro formato (BIG_ENDIAN).
		byte[] buffer = new byte[SOCK_BUFFER];
		ByteBuffer bb = ByteBuffer.wrap(buffer);
		bb.order(ByteOrder.BIG_ENDIAN);

		//Mandar la petición de hash al servidor.
		bb.put(OP_HASH);
		bb.putInt(iter);
		bb.putInt(fdin_length);
		try {
			_out.write(buffer, 0, STRUCT_HASH_SIZE);
			_out.flush();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al enviar datos al servidor.");
			return;
		}

		//Creamos un stream para leer del fichero de entrada y otro para el fichero de salida.
		byte[] text = new byte[HASH_BUFFER];
		InputStream is = null;
		try {
			is = new FileInputStream(fdin);
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al crear streams para lectura/escritura de ficheros.");
			return;
		}

		//Mandamos una petición por cada bloque de texto.
		int bytes_read = 0;
		int hash_global_calc = 0, hash_global = 0;
		for(int i = 0; i < iter; i++) {
			//Leer del fichero de entrada un máximo de SWAP_BUFFER.
			try {
				bytes_read = is.read(text);
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al leer datos del fichero de entrada.");
				return;
			}

			//Mandar la petición de hash al servidor.
			bb.clear();
			bb.put(OP_HASH_ITER_PET);
			bb.putInt(i);
			bb.putInt(bytes_read);
			bb.put(text);
			try {
				_out.write(buffer, 0, STRUCT_HASH_ITER_PET_SIZE);
				_out.flush();
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al enviar datos al servidor.");
				return;
			}

			//Esperamos la respuesta del servidor.
			byte iter_res_op = 0;
			int iter_res_id = 0, iter_res_hash = 0, iter_res_hash_global = 0;
			try {
				bb.clear();
				_in.read(buffer, 0, STRUCT_HASH_ITER_RES_SIZE);

				iter_res_op = bb.get();
				iter_res_id = bb.getInt();
				iter_res_hash = bb.getInt();
				iter_res_hash_global = bb.getInt();
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al recibir datos del servidor.");
				return;
			}

			//Comprobamos que el mensaje de respuesta sea del tipo HASH_ITER_RES.
			if(iter_res_op != OP_HASH_ITER_RES) {
				System.err.println("[ERROR] Identificador de operación incorrecto.");
				return;
			}

			//Comprobar si la id es correcta.
			if(i != iter_res_id) {
				System.err.println("[ERROR] Identificador de hash incorrecto: id[" + i + "] != " + iter_res_id);
				return;
			}

			//(DEBUG) Información sobre la iteración del hash..
			if(_debug) {
				System.out.println("hash iter[" + i +"] " + iter_res_hash + " (" + bytes_read + ")");
			}

			//Actualizamos el hash.
			hash_global_calc = (hash_global_calc + iter_res_hash) % HASH_MOD;
			hash_global = iter_res_hash_global;
		}

		//Cerramos el stream de lectura.
		try {
			is.close();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al cerrar los streams de lectura/escritura.");
			return;
		}

		//Mostramos el hash obtenido.
		if(hash_global_calc == hash_global) {
			System.out.println(hash_global);
		}
		else {
			System.out.println("client[" + hash_global_calc + "] vs server[" + hash_global + "]");
		}
	}

	static void f_check(String src, int hash){
		if (_debug)
			System.out.println("CHECK <SRC=" + src + "> <HASH=" + hash + ">");
	
		//Descriptores de fichero.
		File fdin = new File(src);

		//Comprobamos si existe el fichero de entrada y si está vacío.
		if(!fdin.exists()) {
			System.out.println("[ERROR] Error al crear el descriptor del fichero de entrada.");
			return;
		}
		int fdin_length = (int) fdin.length();
		if(fdin_length == 0) {
			System.out.println("[ERROR] El fichero de entrada está vacío.\n");
			return;
		}

		//Calcular el número de iteraciones a realizar.
		int iter = (int) ((fdin_length / HASH_BUFFER) + 1);

		//Creamos un ByteBuffer para poder leer tipos de datos en otro formato (BIG_ENDIAN).
		byte[] buffer = new byte[SOCK_BUFFER];
		ByteBuffer bb = ByteBuffer.wrap(buffer);
		bb.order(ByteOrder.BIG_ENDIAN);

		//Mandar la petición de check al servidor.
		bb.put(OP_CHECK);
		bb.putInt(iter);
		bb.putInt(fdin_length);
		bb.putInt(hash);
		try {
			_out.write(buffer, 0, STRUCT_CHECK_SIZE);
			_out.flush();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al enviar datos al servidor.");
			return;
		}

		//Creamos un stream para leer del fichero de entrada y otro para el fichero de salida.
		byte[] text = new byte[CHECK_BUFFER];
		InputStream is = null;
		try {
			is = new FileInputStream(fdin);
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al crear streams para lectura/escritura de ficheros.");
			return;
		}

		//Mandamos una petición por cada bloque de texto.
		int bytes_read = 0;
		int hash_global_calc = 0, hash_global = 0;
		byte check = CHECK_FAIL;
		for(int i = 0; i < iter; i++) {
			//Leer del fichero de entrada un bloque de máximo CHECK_BUFFER.
			try {
				bytes_read = is.read(text);
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al leer datos del fichero de entrada.");
				return;
			}

			//Mandar la petición de check al servidor.
			bb.clear();
			bb.put(OP_CHECK_ITER_PET);
			bb.putInt(i);
			bb.putInt(bytes_read);
			bb.put(text);
			try {
				_out.write(buffer, 0, STRUCT_CHECK_ITER_PET_SIZE);
				_out.flush();
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al enviar datos al servidor.");
				return;
			}

			//Esperamos la respuesta del servidor.
			byte iter_res_op = 0, iter_res_check = 0;
			int iter_res_id = 0, iter_res_hash = 0, iter_res_hash_global = 0;
			try {
				bb.clear();
				_in.read(buffer, 0, STRUCT_CHECK_ITER_RES_SIZE);
				iter_res_op = bb.get();
				iter_res_check = bb.get();
				iter_res_id = bb.getInt();
				iter_res_hash = bb.getInt();
				iter_res_hash_global = bb.getInt();
			}
			catch(Exception e) {
				System.err.println("[ERROR] Fallo al recibir datos del servidor.");
				return;
			}

			//Comprobamos que el mensaje de respuesta sea del tipo HASH_ITER_RES.
			if(iter_res_op != OP_CHECK_ITER_RES) {
				System.err.println("[ERROR] Identificador de operación incorrecto.");
				return;
			}

			//Comprobar si la id es correcta.
			if(i != iter_res_id) {
				System.err.println("[ERROR] Identificador de check incorrecto: id[" + i + "] != " + iter_res_id);
				return;
			}

			//(DEBUG) Información sobre la iteración del check.
			if(_debug) {
				System.out.println("check iter[" + i +"] " + iter_res_hash + " (" + bytes_read + ")");
			}

			//Actualizamos el hash.
			hash_global_calc = (hash_global_calc + iter_res_hash) % HASH_MOD;
			hash_global = iter_res_hash_global;

			//Guardamos el check del servidor.
			check = iter_res_check;
		}

		//Cerramos el stream de lectura.
		try {
			is.close();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al cerrar los streams de lectura/escritura.");
			return;
		}

		//Comprobamos si ha habido errores en el hash.
		if(hash_global_calc != hash_global) {
			System.err.println("client[" + hash_global_calc + "] vs server[" + hash_global + "]");
			System.err.println("[ERROR] El hash obtenido del servidor no coincide.");
		}

		//Imprimimos la comprobación del servidor.
		String check_string = "";
		if(check == CHECK_OK) {
			check_string = CHECK_STRING_OK;
		}
		else if(check == CHECK_FAIL) {
			check_string = CHECK_STRING_FAIL;
		}
		System.out.println(check_string);

		//Comprobamos que el check es correcto.
		if(hash_global != hash_global_calc) {
			System.err.println("[ERROR] El check obtenido del servidor no coincide.");
		}
	}

	static void f_stat(){
		if (_debug)
			System.out.println("STAT");

		//Solicitar estadísticas al servidor.
		try {
			_out.write(OP_STAT);
			_out.flush();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al enviar datos al servidor.");
			return;
		}

		//Creamos un ByteBuffer para poder leer tipos de datos en otro formato (BIG_ENDIAN).
		byte[] buffer = new byte[SOCK_BUFFER];
		ByteBuffer bb = ByteBuffer.wrap(buffer);
		bb.order(ByteOrder.BIG_ENDIAN);

		//Recibir estadísticas del servidor.
		try {
			_in.read(buffer, 0, STRUCT_STAT_SIZE);
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al recibir datos del servidor.");
			return;
		}

		//Comprobamos que el mensaje de respuesta sea del tipo STAT.
		byte op = bb.get();
		if(op == OP_STAT) {
			int[] stats = new int[STAT_CODES];
			stats[0] = bb.getInt();
			stats[1] = bb.getInt();
			stats[2] = bb.getInt();
			stats[3] = bb.getInt();
			stats[4] = bb.getInt();

			//Mostramos las estadísticas.
			System.out.println("ping " + stats[0]);
			System.out.println("swap " + stats[1]);
			System.out.println("hash " + stats[2]);
			System.out.println("check " + stats[3]);
			System.out.println("stat " + stats[4]);
		}
	}

	static void f_quit(){
		if (_debug)
			System.out.println("QUIT");

		//Quit server.
		try {
			_out.write(OP_QUIT);
			_out.flush();
		}
		catch(Exception e) {
			System.err.println("[ERROR] Fallo al enviar datos al servidor.");
			return;
		}
	}

	static void usage() {
		System.out.println("Usage: java -cp . client [-d] -s <server> -p <port>");
	}
	
	static boolean parseArguments(String [] argv) {
		Getopt g = new Getopt("client", argv, "ds:p:");

		int c;
		String arg;

		while ((c = g.getopt()) != -1) {
			switch(c) {
				case 'd':
					_debug = true;
					break;
				case 's':
					_server = g.getOptarg();
					break;
				case 'p':
					arg = g.getOptarg();
					_port = Integer.parseInt(arg);
					break;
				case '?':
					System.out.print("getopt() returned " + c + "\n");
					break; // getopt() already printed an error
				default:
					System.out.print("getopt() returned " + c + "\n");
			}
		}
		
		if (_server == null)
			return false;
		
		if ((_port < 1024) || (_port > 65535)) {
			System.out.println("Error: Port must be in the range 1024 <= port <= 65535");
			return false;
		}

		return true;
	}
	
	static void shell() {
		boolean exit = false;
		String [] line;
		BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

		while (!exit) {
			try {
				System.out.print("c> ");
				line = in.readLine().split("\\s");

				if (line.length > 0) {
					if (line[0].equals("ping")) {
						if  (line.length == 1)
							f_ping();
						else
							System.out.println("Syntax error. Use: ping");
					} else if (line[0].equals("swap")) {
						if  (line.length == 3)
							f_swap(line[1], line[2]);
						else
							System.out.println("Syntax error. Use: swap <source_file> <destination_file>");
					} else if (line[0].equals("hash")) {
						if  (line.length == 2)
							f_hash(line[1]);
						else
							System.out.println("Syntax error. Use: hash <source_file>");
					} else if (line[0].equals("check")) {
						if  (line.length == 3)
							try {
								f_check(line[1], Integer.parseInt(line[2]));
							} catch (java.lang.NumberFormatException e) {
								System.out.println("Syntax error: second argument must be numeric");
								System.out.println("         Use: check <source_file> <hash>");
							}
						else
							System.out.println("Syntax error. Use: check <source_file> <hash>");
					} else if (line[0].equals("stat")) {
						if  (line.length == 1)
							f_stat();
						else
							System.out.println("Syntax error. Use: stat");
					} else if (line[0].equals("quit")) {
						if  (line.length == 1) {
							f_quit();
							exit = true;
						} else {
							System.out.println("Syntax error. Use: quit");
						}
					} else {
						System.out.println("Error: command '" + line[0] + "' not valid.");
					}
				}
			} catch (java.io.IOException e) {
				System.out.println("Exception: " + e);
				e.printStackTrace();
			}
		}
	}
	
	public static void main(String[] argv) {
		if(!parseArguments(argv)) {
			usage();
			return;
		}
		
		if (_debug)
			System.out.println("SERVER: " + _server + " PORT: " + _port);

		try {
			//Creamos el socket para la dirección y puerto pasados por parámetros.
			Socket _socket = new Socket(_server, _port);

			//Creamos los streams para lectura/escritura con el servidor.
			_out = new DataOutputStream(_socket.getOutputStream());
			_in = new DataInputStream(_socket.getInputStream());

			shell();

			//Cerramos el socket y los streams.
			_out.close();
			_in.close();
			_socket.close();
		}
		catch(Exception e) {
			System.err.println("c> Error en la conexión con el servidor " + _server + ":" + _port);
		}
	}
}
