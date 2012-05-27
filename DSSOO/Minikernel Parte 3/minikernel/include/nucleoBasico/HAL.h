/*
 *  minikernel/include/HAL.h
 *  Minikernel. Versión 1.0
 *  Fernando Pérez Costoya
 *
 * Fichero de cabecera que contiene los prototipos de las funciones
 * proporcionadas por el módulo HAL.
 *
 * 	NO SE DEBE MODIFICAR
 *
 */

#ifndef _HAL_H
#define _HAL_H

/***************************************************************
 * INCLUDES Y TIPOS DE DATOS DEPENDIENTES DE LA PLATAFORMA
 ***************************************************************/
 
/* La version actual no usa en Linux las primitivas de gestion de contexto
   disponibles en algunos UNIX ("makecontext", "setcontext", ...), ya que,
   que yo sepa, por ahora no estan implementadas. En su lugar recurre
   a una version mas "cutre" basada en "setjmp". n una próxima versión
   deberian desaparecer. */

#ifndef __linux__
	/* Para sistemas con "makecontext" como Digital UNIX */
	#include <ucontext.h>
#else /* linux */
	#include <linux/version.h>

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,1)
		/* Para Linux modernos que también lo tienen */		
		#include <ucontext.h>

	#else
		/* Para Linux antiguos */
		#include <setjmp.h>
		#include <signal.h>

		typedef struct st_contexto {
			stack_t uc_stack;
			char * args;
			sigjmp_buf contexto;
			void * uc_link;
			sigset_t uc_sigmask;	
		} ucontext_t;

	#endif /* linux2.2 */
#endif /* linux */


/***************************************************************
 * CONSTANTES 
 ***************************************************************/
/*
 * defincion de los elementos de hardware
 */
#define NUM_HARDWARE 4

#define ID_TECLADO	0
#define ID_PANTALLA 1
#define ID_CDROM 2
#define ID_DISCO 3

/* 
 * definicion del tama–o de sector
 */
#define DEVICE_SECTOR_SIZE 256
 
/* 
 * Registros generales del procesador 
 */
#define NREGS 6

/* 
 * evita uso de printf de bilioteca estandar 
 */
#define printf printk 

/***************************************************************
 * TIPOS DE DATOS 
 ***************************************************************/
 
/* 
 * Contexto "hardware" de un proceso 
 */
typedef struct {
	ucontext_t ctxt;
	long registros[NREGS];
} contexto_t;

/***************************************************************
 * EXPORTAR FUNCIONES PUBLICAS DECLARADAS EN EL FICHERO C
 ***************************************************************/

/* ------------------------------------------------------------------*
 * Operaciones relacionadas con los dispositivos y las interrupciones.
 * ------------------------------------------------------------------*/

/* 
 * obtiene hora del rejoj CMOS
 */
unsigned long long int leer_reloj_CMOS(); 

/* 
 * iniciar controlador de reloj 
 */
void iniciar_cont_reloj(int ticks_por_seg); 

/* 
 * iniciar controlador de interrupciones. 
 */
void iniciar_cont_int();  

/* 
 * instala un manejador 
 */
void instal_man_int(int nvector, void (*manej)()); 

/* 
 * fija nivel de interrupcion del procesador 
 * devolviendo el previo 
 */
int fijar_nivel_int(int nivel);

/* 
 * Devuelve verdadero si el modo previo de ejecucion
 * del procesador era usuario 
 */ 
int viene_de_modo_usuario(); 

/* 
 * activa la interrupcion SW 
 */
void activar_int_SW();  


/* -------------------------------------------------------------------------*/

/*
 * Operacion de salvaguarda y recuperacion de contexto hardware del proceso.
 * Rutina que realiza el cambio de contexto. Si (contexto_a_salvar==NULL)
 * no salva contexto, solo restaura
 */
void cambio_contexto(contexto_t *contexto_a_salvar, contexto_t *contexto_a_restaurar);


/* ---------------------------------------------------------------*
 * Operaciones relacionadas con mapa de memoria del proceso y pila
 * ---------------------------------------------------------------*/

/* 
 * crea mapa de memoria a partir de ejecutable "prog" devolviendo un
 * descriptor de dicho mapa y la dirección del punto de arranque del programa 
 */
void * crear_imagen(char *prog, void **dir_ini); 

/* 
 * crea la pila del proceso 
 */
void * crear_pila(int tam); 

/* 
 * crea el contexto inicial del proceso 
*/
void fijar_contexto_ini(void *mem, void *p_pila, int tam_pila,
						void * pc_inicial, contexto_t *contexto_ini);

/* 
 * libera el mapa de memoria 
 */
void liberar_imagen(void *mem);		

/* 
 * libera la pila del proceso 
 */
void liberar_pila(void *pila);		



/* ------------------------------------------*
 * Operaciones de gestion del hardware de I/O
 * ------------------------------------------*/

/* 
 * Conectar y configurar incialmente un  
 * hardware especificado, si es un dispositivo de bloques
 * asocia el fichero con el y devuelve el numero de sectores.
 */
int plug_hardware(int id_hardware, char *nombre_fichero);

/* 
 * Desconectar un hardware especificado  
 */
int unplug_hardware(int id_hardware);

/* 
 * Devuelve si el hardware tiene o no su interrupcion activada
 * en caso afirmativo, la desactiva
 */
int test_and_set_interrupcion(int id_hardware);

/* 
 * lee un caracter del puerto asignado al 
 * hardware especificado  
 */
char leer_puerto(int id_hardware); 

/* 
 * escribir un caracter del puerto asignado al 
 * hardware especificado  
 */
int escribir_puerto(int id_hardware, char caracter); 

/* 
 * Configurar una operacion de DMA de lectura  
 */
int cfg_lectura_DMA(int id_hardware, int sector_inicial, 
					char *buffer, int num_sectores);
					
/* 
 * Configurar una operacion de DMA de escritura  
 */
int cfg_escritura_DMA(int id_hardware, int sector_inicial, 
				  	  char *buffer, int num_sectores);
					  
					  
/* --------------------------------*
 * Operaciones miscelaneas 
 * --------------------------------*/

/* 
 * leer un registro 
 */
long leer_registro(int nreg); 

/* 
 * escribir un registro 
 */
int escribir_registro(int nreg, long valor); 

/* 
 * Ejecuta una instrucción HALT para parar UCP 
 */
void halt();	

/* 
 * muestra mensaje y termina SO 
 */
void panico(char *mens); 

/* 
 * escribe en pantalla 
 */
void escribir_ker(char *buffer, unsigned int longi); 

/* 
 * escribe en pantalla con formato 
 */
int printk(const char *formato, ...); 

/* 
 * concatena en un buffer existente un string con formato hasta un tama–o m‡ximo
 * devuelve el numero de bytes que quedan libres
 */
int sprintk_concat(char * buffer, int bytes_libres, const char *formato, ...);

/* 
 * Cargar un nuevo modulo del kernel y ejecuta su funcion
 * de inicializacion "cargarModulo_<nombre>" 
 */
int cargar_modulo (char * nombre_modulo);

#endif /* _HAL_H */


