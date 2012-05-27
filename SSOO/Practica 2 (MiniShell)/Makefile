 #
 # Makefile
 # Copyright (c) 1993-1999, Francisco Rosales <frosal@fi.upm.es>
 #
 # Minishell "make" source
 # Regenerate msh recompiling only modified sources
 #
 # DO NOT MODIFY THIS FILE
 #

CC	= gcc
CFLAGS	= -Wall -g
YFLAGS	= -d
LFLAGS	=

OBJS	= parser.o scanner.o y.o main.o

all: msh

msh: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

clean:
	rm -f *.tab.? *.o *.bak *~ parser.c scanner.c core msh

cleanall: clean
	rm -f :* freefds* nofiles* killmyself* sigdfl*

depend:
	makedepend main.c parser.y scanner.l
