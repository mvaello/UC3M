#
# minikernel/drivers/Makefile
#	Makefile para crear la libreria de tablas dinamicas del kernel 
#


# FLAGS DE CONFIGURACION
INCLUDEDIR=../include
CC=gcc
CFLAGS=-g -Wall -I$(INCLUDEDIR)

# FICHEROS A COMPILAR
OBJECTS= bufferCaracteres.o listaPeticiones.o tablaDrivers.o tablaDispositivos.o tablaFS.o tablaMontajes.o tablaInodos.o  tablaDescFicheros.o


# regla principal: llama a crear libtablasDinamicas.a
all: $(OBJECTS)

# regla para compilar los .c en .o
%.o :: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# regla para limpiar los ficheros de la compilacion
clean:
	rm -f $(OBJECTS) 
