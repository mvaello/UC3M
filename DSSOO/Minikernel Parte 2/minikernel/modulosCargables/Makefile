#
# minikernel/filesystem/Makefile
#	Makefile para compilar los modulos cargables del kernel 
#


# FLAGS DE CONFIGURACION
INCLUDEDIR=../include
CC=gcc
CFLAGS=-g -Wall -I$(INCLUDEDIR)

# obtener la lista de modulos a compilar
SOURCES=$(shell echo modulo_*.c)
BASENAMES=$(basename $(SOURCES))
OBJECTS=$(addsuffix .drv, $(BASENAMES)) 

# regla principal: llama a compilar todos los modulos
all: $(OBJECTS)

# regla para compilar los .c en .drv
%.drv :: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<
	
# regla para limpiar los ficheros de la compilacion
clean:
	rm -f $(OBJECTS)

