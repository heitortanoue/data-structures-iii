#ifndef __CABECALHO_H__
#define __CABECALHO_H__

#include <stdio.h>

// Quantidade de bytes do cabeçalho
#define TAM_CABECALHO 21

// Tamanho fixo de 21 bytes mas alocar em uma pagina de disco inteira (960 bytes)
typedef struct {
    char status;
    int topo;
    int proxRRN;
    int nroRegRem;
    int nroPagDisco;
    int qttCompacta;
} Cabecalho;

int atualizarCabecalhoPadrao (Cabecalho *c);
int atualizarStatusCabecalho (Cabecalho *c, char status);
int atualizarNumPagDiscoCabecalho (Cabecalho *c, int numRegistros);
int lerCabecalhoArquivo (FILE *arq, Cabecalho *c);
void imprimeCabecalho (Cabecalho *c);
int calculaNumPagDisco ( int numRegistros );

#endif