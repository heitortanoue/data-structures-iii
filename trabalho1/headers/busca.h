#ifndef __BUSCA_H__
#define __BUSCA_H__

#include "registro.h"
#include <stdio.h>

int trataFiltros(Busca *filtros, char *campo, char **criterios);
int testaRegistro (Registro reg, Busca *filtro, int numFiltro);

#endif