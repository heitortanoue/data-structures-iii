#ifndef __BUSCA_H__
#define __BUSCA_H__

#include "registro.h"
#include <stdio.h>
#include <stdlib.h>

int criaFiltro(Busca *filtros, int qtd);
int destroiFiltro(Busca *filtros);
int trataFiltros(Busca *filtros, int i);
int testaRegistro (Registro reg, Busca *filtro, int numFiltro);

#endif