#include "../headers/cabecalho.h"
#include "../headers/orgarquivos.h"
#include "../headers/arvoreB.h"
#include <math.h>

// Atualiza o cabeçalho com as informações default
int atualizarCabecalhoPadrao (Cabecalho *c ) {
	c->status = '0';
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

int testaStatusCabecalho (Cabecalho *c) {
    if (c->status == '0') {
        printf("Falha na execução da funcionalidade.\n");
        return ERRO;
    }
    return SUCESSO;
}

// Calcula quantas paginas de disco uma quantidade de regs. ocupam contando com a pag. do cabecalho
int calculaNumPagDisco ( int numRegistros ) {
    float numBytes = numRegistros * TAM_REGISTRO;
    float numBytesTotal = numBytes + TAM_PG_DISCO;
    float numPagDisco = numBytesTotal / TAM_PG_DISCO;
    int pagDisco = (int) ceil(numPagDisco);

    return pagDisco;
}

// Atualiza o numero de pagina de disco do arquivo com o parametro 'numRegistros'
int atualizarNumPagDiscoCabecalho (Cabecalho *c, int numRegistros) {
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

    if (testaStatusCabecalho(c) == ERRO) {
        exit(ERRO);
    }

    // passa por todo o lixo e vai diretamente para a posicao do primeiro registro
    fseek(arq, TAM_PG_DISCO, SEEK_SET);

    return SUCESSO;
}

// Imprime o cabeçalho 'c' para visualizacao
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


// Cria um cabecalho de indice com dados padroes
void criaCabecalhoIndice (CabecalhoIndice* cabecalho) {
    cabecalho->status = '0';
    cabecalho->noRaiz = -1;
    cabecalho->RRNproxNo = 0;
    cabecalho->alturaArvore = 0;
    cabecalho->nroChavesTotal = 0;
}

// Escreve o conteudo de um cabecalho de indice no arquivo de indice
void escreveCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo) {
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->noRaiz, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroChavesTotal, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->alturaArvore, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->RRNproxNo, sizeof(int), 1, arquivo);

    escreverLixo(arquivo, TAM_PG_DISCO_INDICE - TAM_CABECALHO_INDICE);
}

// Le o conteudo do arquivo de indice e coloca em um cabecalho de indice
void leCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo) {
    fseek(arquivo, 0, SEEK_SET);
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->noRaiz, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroChavesTotal, sizeof(int), 1, arquivo);
    fread(&cabecalho->alturaArvore, sizeof(int), 1, arquivo);
    fread(&cabecalho->RRNproxNo, sizeof(int), 1, arquivo);
}
