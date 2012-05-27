#
# minikernel/kernel/Makefile
#	Makefile para crear la libreria basica del kernel 
#


# FLAGS DE CONFIGURACION
INCLUDEDIR=../include
CC=gcc
CFLAGS=-g -Wall -I$(INCLUDEDIR)

# FICHEROS A COMPILAR
OBJECTS=Interrupciones_Excepciones.o LlamadasSistema.o ListaBCP_Planificador.o HAL.o cargarModulos.o

SOURCES=$(shell echo ../modulosCargables/modulo_*.c)
NAMES=$(notdir $(SOURCES))
NAME_MODULES=$(basename $(NAMES))

# regla principal: llama a crear libnucleoBasico.a
all: $(OBJECTS)
	@rm -f cargarModulos.c

# regla para crear HAL.o
HAL.o: HAL.o.bak
	@ln -sf HAL.o.bak HAL.o

# regla para crear cargarModulos.c
cargarModulos.c: cargarModulos.sh
	@./cargarModulos.sh $(NAME_MODULES)

# regla para compilar los .c en .o
%.o :: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

# regla para limpiar los ficheros de la compilacion
clean:
	rm -f $(OBJECTS) cargarModulos.c
