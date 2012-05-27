#
# usuario/Makefile
#	Makefile de programas de usuario y libreria de systema


# regla principal: llama a crear la biblioteca de sistema y los programas de usuario
all: biblioteca programas

# regla para crear la biblioteca de sistema
biblioteca:
	make -C system_lib

# regla para crear los programas
programas:
	make -C user_programs

#regla para limpiar los ficheros de la compilacion
clean:
	make -C system_lib clean
	make -C user_programs clean
