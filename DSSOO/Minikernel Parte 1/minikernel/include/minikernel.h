/*
 *  minikernel/include/kernel.h
 *
 *  Minikernel. Versión 1.0
 *
 *  Fernando Pérez Costoya
 *
 */


#ifndef _KERNEL_H
#define _KERNEL_H

/*
 * Incluir las cabeceras b‡sicas del nucleo
 */
#include "nucleoBasico/const.h"
#include "nucleoBasico/HAL.h"
#include "nucleoBasico/Interrupciones_Excepciones.h"
#include "nucleoBasico/LlamadasSistema.h"
#include "nucleoBasico/Numeros_Llamadas.h"
#include "nucleoBasico/ListaBCP_Planificador.h"

/*
 * Incluir las cabeceras de las tablas dinamicas
 */
#include "tablasDinamicas/bufferCaracteres.h"
#include "tablasDinamicas/listaPeticiones.h"
#include "tablasDinamicas/tablaDescFicheros.h"
#include "tablasDinamicas/tablaDispositivos.h"
#include "tablasDinamicas/tablaDrivers.h"
#include "tablasDinamicas/tablaFS.h"
#include "tablasDinamicas/tablaInodos.h"
#include "tablasDinamicas/tablaMontajes.h"


#endif /* _KERNEL_H */


