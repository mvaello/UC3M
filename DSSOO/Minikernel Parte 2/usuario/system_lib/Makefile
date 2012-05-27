#
# usuario/lib/Makefile
#	Makefile de la biblioteca
#

# FLAGS DE CONFIGURACION
INCLUDEDIR=../include
INCLUDEDIR2=../../minikernel/include
CC=gcc
CFLAGS=-Wall -g -I$(INCLUDEDIR) -I$(INCLUDEDIR2)

#regla principal: llama a crear la libreria de sistema
all: libserv.a

# regla para compilar los .c en .o
%.o :: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

#regla para crear la libreria de sistema
libserv.a: serv.o misc.o
	ar rv libserv.a serv.o misc.o
	ranlib libserv.a

#regla para limpiar los ficheros de compilacion
clean:
	rm -f serv.o libserv.a
