/*
 *  usuario/include/servicios.h
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */

/*
 *
 * Fichero de cabecera que contiene los prototipos de funciones de
 * biblioteca que proporcionan la interfaz de llamadas al sistema.
 *
 *      SE DEBE MODIFICAR AL INCLUIR NUEVAS LLAMADAS
 *
 */

#ifndef SERVICIOS_H
#define SERVICIOS_H

#include <stdio.h>

/* Evita el uso del printf y scanf de la bilioteca estándar */
#define printf escribirf
#define scanf leerf

/* constantes de los flags de la llamada abrir */
#define T_DIRECTORIO 0
#define T_FICHERO 1

/* Funcion de biblioteca */
int escribirf(const char *formato, ...);
int leerf(const char *formato, ...);

/* Llamadas al sistema proporcionadas */
int crear_proceso(char *prog);
int crear_proceso_alta_prioridad(char *prog);
int terminar_proceso();
int abrir (char *nombre, int flags);
int leer (int fd, char * buffer, int tamanyo);
int escribir (int fd, char * buffer, int tamanyo);
int buscar (int fd, int offset);
int cerrar (int fd);
int montar (char *nombreDispositivo, char *nombreMontaje, char *nombreFS);
int sincronizar (char *nombreMontaje);
int desmontar (char *nombreMontaje);
int obtener_id_proceso ();
int obtener_tiempo ();
int dormir(unsigned int segundos);

#endif /* SERVICIOS_H */

