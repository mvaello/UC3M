#
# Makefile
# 	Makefile global del sistema
#

# regla principal: llama a crear la biblioteca de sistema y los programas de usuario
all: sistema programas

# regla para crear el kernel del SSOO
sistema:
	make -C minikernel

# regla para crear la biblioteca de sistema y los programas de usuario
programas:
	make -C usuario

#regla para limpiar los ficheros de la compilacion
clean:
	make -C minikernel clean
	make -C usuario clean
