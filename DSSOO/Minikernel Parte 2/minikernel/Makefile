
# minikernel/Makefile
#	Makefile del sistema 
#

# FLAGS DE CONFIGURACION
INCLUDEDIR=include
CC=gcc
CFLAGS=-g -Wall -I$(INCLUDEDIR)

# FICHEROS A COMPILAR
OBJECTS=minikernel.o 

# BIBLIOTECAS A LINKAR
LIBRARYS=-ldl -lrt 

# OBJETOS NUCLEO BASICO 
OBJECTS_NB=$(shell echo nucleoBasico/*.o)

# OBJETOS TABLAS DINAMICAS
OBJECTS_TD=$(shell echo tablasDinamicas/*.o)

# MODULOS A CARGAR
MODULES=$(shell echo modulosCargables/modulo_*.drv) 

# FICHEROS RESULTADO
EXEC=minikernel


#regla principal: llama a crear el ejecutable del minikernel
all: minikernel

# llama al makefile para crear la libreria libnucleoBasico.a
make_nucleoBasico:
	make -C ./nucleoBasico

# llama al makefile para crear la libreria libtablasDinamicas.a
make_tablasDinamicas: 
	make -C ./tablasDinamicas

# llama al makefile para generar los modulos cargables .drv
make_modulosCargables: 
	make -C ./modulosCargables

# regla para compilar los .c en .o
%.o :: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

libauxiliar.a: make_nucleoBasico make_tablasDinamicas make_modulosCargables
	ar rv libnucleo.a $(OBJECTS_NB) $(OBJECTS_TD) $(MODULES)
	ranlib libnucleo.a

# regla para crear el ejecutable del minikernel
minikernel: libauxiliar.a $(OBJECTS)
	$(CC) -shared -o $@  $(OBJECTS) libnucleo.a $(LIBRARYS)

# regla para limpiar los ficheros de la compilacion
clean:
	make -C ./nucleoBasico clean
	make -C ./tablasDinamicas clean
	make -C ./modulosCargables clean
	rm -f $(OBJECTS) $(EXEC) libnucleo.a
