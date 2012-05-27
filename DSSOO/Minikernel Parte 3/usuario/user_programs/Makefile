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
PROGRAMAS=init simplon excep_arit excep_mem prueba_RR mostrar_disco mostrar_fichero copiar_disco copiar_fichero yosoy prueba_dormir dormilon prueba_alta_prioridad yosoy_duerme prueba_avanzada_dormir leer_disco escribir_disco leer_disco_bloque escribir_disco_bloque test_e2_1_abrir_disco test_e2_2_leer_bloque test_e2_3_leer_disco test_e2_4_escribir_bloque test_e2_5_escribir_disco test_e2_6_leer_varios_disco test_e2_7_leer_varios_bloque test_e2_8_escribir_varios_disco test_e2_9_escribir_varios_bloque montar_vol abrir_fichero leer_fichero escribir_fichero sincronizar_vol lectura_escritura_varios_tamanyos leer_fichero_mul leer_fichero_mul_child escribir_fichero_mul escribir_fichero_mul_child leer_fichero_mul_varfic leer_fichero_mul_varfic_child escribir_fichero_mul_varfic escribir_fichero_mul_varfic_child abrir_fichero_exista_noexista

#regla principal: llama a crear la biblioteca de sistema y los programas de usuario
all: $(PROGRAMAS)

# regla para crear cada uno de los programas de usuario
% :: %.c $(INCLUDEDIR)/servicios.h $(LIBRARYS)
	$(CC) $(CFLAGS) -c -o $@.o $<
	$(CC) -o $@ $@.o $(LDFLAGS)

#regla para limpiar los ficheros de la compilacion
clean:
	rm -f *.o $(PROGRAMAS)

