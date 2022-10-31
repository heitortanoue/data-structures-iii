#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/arvoreB.h"

No* criaNo () {
    No *novo = (No*) malloc(sizeof(No));
    novo->chaves = (Indice*) malloc(sizeof(Indice) * ORDEM);
    for (int i = 0; i < ORDEM; i++) {
        novo->chaves[i].descendente = -1;
        novo->chaves[i].chave = -1;
        novo->chaves[i].referencia = -1;
    }

    novo->folha = '1';
    novo->nroChavesNo = 0;
    novo->alturaNo = 1;
    novo->RRNdoNo = -1;

    return novo;
}

void escreveNo (No *no, FILE *arquivo) {
    fseek(arquivo, TAM_CABECALHO + (no->RRNdoNo * TAM_PG_DISCO), SEEK_SET);
    fwrite(&no->folha, sizeof(char), 1, arquivo);
    fwrite(&no->nroChavesNo, sizeof(int), 1, arquivo);
    fwrite(&no->alturaNo, sizeof(int), 1, arquivo);
    fwrite(&no->RRNdoNo, sizeof(int), 1, arquivo);
    for (int i = 0; i < ORDEM; i++) {
        fwrite(&no->chaves[i].descendente, sizeof(int), 1, arquivo);
        fwrite(&no->chaves[i].chave, sizeof(int), 1, arquivo);
        fwrite(&no->chaves[i].referencia, sizeof(int), 1, arquivo);
    }
}