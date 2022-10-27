#include "../headers/cabecalho.h"
#include "../headers/orgarquivos.h"
#include <math.h>

// Atualiza o cabeçalho com as informações iniciais
int atualizarCabecalhoPadrao (Cabecalho *c ) {
	c->status = 0;
    c->topo = -1;
    c->proxRRN = 0;
    c->nroRegRem = 0;
    c->nroPagDisco = 0;
    c->qttCompacta = 0;

    return SUCESSO;
}

// Atualiza o status do cabeçalho com o parametro 'status'
int atualizarStatusCabecalho (Cabecalho *c, char status) {
    c->status = status;
    return SUCESSO;
}

int calculaNumPagDisco ( int numRegistros ) {
    return (ceil((double) numRegistros * TAM_REGISTRO / TAM_PG_DISCO)) + 1;
}

// Atualiza o numero de pagina de disco do arquivo com o parametro 'numRegistros'
int atualizarNumPagDiscoCabecalho (Cabecalho *c, int numRegistros) {
    // +1 por conta do cabeçalho que ocupa 1 pg de disco
    c->nroPagDisco = calculaNumPagDisco(numRegistros);
    return SUCESSO;
}

// Lê o cabeçalho do arquivo 'arq' e armazena no Cabecalho 'c'
int lerCabecalhoArquivo (FILE *arq, Cabecalho *c) {
    fread(&c->status, sizeof(char), 1, arq);
    fread(&c->topo, sizeof(int), 1, arq);
    fread(&c->proxRRN, sizeof(int), 1, arq);
    fread(&c->nroRegRem, sizeof(int), 1, arq);
    fread(&c->nroPagDisco, sizeof(int), 1, arq);
    fread(&c->qttCompacta, sizeof(int), 1, arq);
    // passa por todo o lixo e vai diretamente para a posicao do primeiro registro
    fseek(arq, TAM_PG_DISCO, SEEK_SET);

    return SUCESSO;
}

// Imprime o cabeçalho 'c'
void imprimeCabecalho (Cabecalho *c) {
    printf("\n======  Cabecalho  ======\n");
    printf("status: %c\n", c->status);
    printf("topo: %d\n", c->topo);
    printf("proxRRN: %d\n", c->proxRRN);
    printf("nroRegRem: %d\n", c->nroRegRem);
    printf("nroPagDisco: %d\n", c->nroPagDisco);
    printf("qttCompacta: %d\n", c->qttCompacta);
    printf("=========================\n\n");
}