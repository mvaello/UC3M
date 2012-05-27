#
# usuario/Makefile
#	Makefile de programas de usuario
#
# NOTA: EN LA VERSIÓN INICIAL SÓLO COMPILARÁN CORRECTAMENTE LOS PROGRAMAS
# init, excep_arit, excep_mem y simplon, YA QUE LOS DEMÁS USAN LLAMADAS QUE
# QUE AUN NO HA IMPLEMENTADO EL ALUMNO 
#


# FLAGS DE CONFIGURACION
MAKEFLAGS=-k
INCLUDEDIR=../include
LIBDIR=../system_lib
CC=gcc
CFLAGS=-Wall -Werror -g -I$(INCLUDEDIR)
LDFLAGS=-shared -L$(LIBDIR) -lserv

# PROGRAMAS A COMPILAR y LINKAR
# NOTA: Anyadir en la variable PROGRAMAS aquellos programas de usuario 
# que se quieran compilar (comentando y descomentando)
# Pendientes: yosoy dormilon prueba_dormir
PROGRAMAS=init simplon excep_arit excep_mem prueba_RR mostrar_disco mostrar_fichero copiar_disco copiar_fichero yosoy prueba_dormir dormilon prueba_alta_prioridad yosoy_duerme prueba_avanzada_dormir leer_disco escribir_disco leer_disco_bloque escribir_disco_bloque

#regla principal: llama a crear la biblioteca de sistema y los programas de usuario
all: $(PROGRAMAS)

# regla para crear cada uno de los programas de usuario
% :: %.c $(INCLUDEDIR)/servicios.h $(LIBRARYS)
	$(CC) $(CFLAGS) -c -o $@.o $<
	$(CC) -o $@ $@.o $(LDFLAGS)

#regla para limpiar los ficheros de la compilacion
clean:
	rm -f *.o $(PROGRAMAS)

