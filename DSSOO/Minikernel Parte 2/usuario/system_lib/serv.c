/*
 *  usuario/lib/serv.c
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 *
 * Fichero que contiene las definiciones de las funciones de interfaz
 * a las llamadas al sistema. Usa la funcion de apoyo llamsis
 *
 *      SE DEBE MODIFICAR AL INCLUIR NUEVAS LLAMADAS
 *
 */

#include "nucleoBasico/Numeros_Llamadas.h"
#include "servicios.h"

/* Función del módulo "misc" que prepara el código de la llamada
   (en el registro 0), los parámetros (en registros 1, 2, ...), realiza la
   instruccion de llamada al sistema  y devuelve el resultado 
   (que obtiene del registro 0) */

int llamsis(int llamada, int nargs, ... /* args */);


/*
 *
 * Funciones interfaz a las llamadas al sistema
 *
 */


int crear_proceso(char *prog){
	return llamsis(CREAR_PROCESO, 1, (long)prog);
}

int crear_proceso_alta_prioridad(char *prog){
	return llamsis(CREAR_PROCESO_ALTA_PRIORIDAD, 1, (long)prog);
}

int terminar_proceso(){
	return llamsis(TERMINAR_PROCESO, 0);
}
int abrir (char *nombre, int flags){
	return llamsis(ABRIR, 2, (long)nombre, (long)flags);
}
int leer (int fd, char * buffer, int tamanyo){
	return llamsis(LEER, 3, (long)fd, (long)buffer, (long)tamanyo);
}
int escribir (int fd, char * buffer, int tamanyo){
	return llamsis(ESCRIBIR, 3, (long)fd, (long)buffer, (long)tamanyo);
}
int buscar (int fd, int offset){
	return llamsis(BUSCAR, 2, (long)fd, (long)offset);
}
int cerrar (int fd){
	return llamsis(CERRAR, 1, (long)fd);
}
int montar (char *nombreDispositivo, char *nombreMontaje, char *nombreFS){
	return llamsis(MONTAR, 3, (long)nombreDispositivo, (long)nombreMontaje, (long)nombreFS);
}
int sincronizar (char *nombreMontaje){
	return llamsis(SINCRONIZAR, 1, (long)nombreMontaje);
}
int desmontar (char *nombreMontaje){
	return llamsis(DESMONTAR, 1, (long)nombreMontaje);
}
int obtener_id_proceso() {
	return llamsis(PID, 0);
}
int obtener_tiempo() {
	return llamsis(TIME, 0);
}
int dormir(unsigned int segundos) {
	return llamsis(SLEEP, 1, (long) segundos);
}

