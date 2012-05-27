#!/bin/bash

FILE=$0
FILE=${FILE%.sh}.c
LISTA=""

for I in $*; do
    LISTA="$LISTA ${I#modulo_}"
done

echo > $FILE

for I in $LISTA; do
    echo "int cargarModulo_${I}();" >> $FILE
done

echo >> $FILE

echo "struct modulos {char servicio[256]; int (*funServicio)();} arrayModulos[] = {" >> $FILE

for I in $LISTA; do
    echo "{\"cargarModulo_${I}\",cargarModulo_${I}}," >> $FILE
done

echo "{\"\",0}};" >> $FILE