#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/arvoreB.h"
#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/busca.h"
#include "../headers/removeinsere.h"

No* criaNo () {
    No *novo = (No*) malloc(sizeof(No));
    novo->dados = (Indice*) malloc(sizeof(Indice) * (ORDEM - 1));
    novo->descendentes = (int*) malloc(sizeof(int) * ORDEM);

    for (int i = 0; i < ORDEM - 1; i++) {
        novo->dados[i].chave = -1;
        novo->dados[i].referencia = -1;
    }
    for (int i = 0; i < ORDEM; i++) {
        novo->descendentes[i] = -1;
    }

    novo->folha = '1';
    novo->nroChavesNo = 0;
    novo->alturaNo = 1;
    novo->RRNdoNo = -1;

    return novo;
}

int calculaByteOffsetArvoreB (int RRN) {
    return (TAM_PG_DISCO_INDICE + (RRN * TAM_NO));
}

void escreveNo (No *no, FILE *arquivo) {
    fseek(arquivo, calculaByteOffsetArvoreB(no->RRNdoNo), SEEK_SET);
    fwrite(&no->folha, sizeof(char), 1, arquivo);
    fwrite(&no->nroChavesNo, sizeof(int), 1, arquivo);
    fwrite(&no->alturaNo, sizeof(int), 1, arquivo);
    fwrite(&no->RRNdoNo, sizeof(int), 1, arquivo);

    for (int i = 0; i < ORDEM - 1; i++) {
        fwrite(&no->descendentes[i], sizeof(int), 1, arquivo);
        fwrite(&no->dados[i].chave, sizeof(int), 1, arquivo);
        fwrite(&no->dados[i].referencia, sizeof(int), 1, arquivo);
    }
    fwrite(&no->descendentes[ORDEM - 1], sizeof(int), 1, arquivo);
}

No* leNo (int RRN, FILE *arquivo) {
    No *novo = criaNo();

    fseek(arquivo, calculaByteOffsetArvoreB(RRN), SEEK_SET);
    fread(&novo->folha, sizeof(char), 1, arquivo);
    fread(&novo->nroChavesNo, sizeof(int), 1, arquivo);
    fread(&novo->alturaNo, sizeof(int), 1, arquivo);
    fread(&novo->RRNdoNo, sizeof(int), 1, arquivo);

    for (int i = 0; i < ORDEM - 1; i++) {
        fread(&novo->descendentes[i], sizeof(int), 1, arquivo);
        fread(&novo->dados[i].chave, sizeof(int), 1, arquivo);
        fread(&novo->dados[i].referencia, sizeof(int), 1, arquivo);
    }
    fread(&novo->descendentes[ORDEM - 1], sizeof(int), 1, arquivo);

    return novo;
}

void criaCabecalhoIndice (CabecalhoIndice* cabecalho) {
    cabecalho->status = '0';
    cabecalho->noRaiz = -1;
    cabecalho->RRNproxNo = 0;
    cabecalho->alturaArvore = 0;
    cabecalho->nroChavesTotal = 0;
}

void escreveCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo) {
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->noRaiz, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroChavesTotal, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->alturaArvore, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->RRNproxNo, sizeof(int), 1, arquivo);

    escreverLixo(arquivo, TAM_PG_DISCO_INDICE - TAM_CABECALHO_INDICE);
}

void leCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo) {
    fseek(arquivo, 0, SEEK_SET);
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->noRaiz, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroChavesTotal, sizeof(int), 1, arquivo);
    fread(&cabecalho->alturaArvore, sizeof(int), 1, arquivo);
    fread(&cabecalho->RRNproxNo, sizeof(int), 1, arquivo);
}

No* buscaChaveArvoreB (int chave, int RRN, FILE *arquivo) {
    No *no = leNo(RRN, arquivo);
    int i = 0;
    // Enquanto a chave buscada for maior que a chave do nó e não for a última chave do nó
    while (i < no->nroChavesNo && chave > no->dados[i].chave) {
        i++;
    }

    // Chave encontrada
    if (i < no->nroChavesNo && chave == no->dados[i].chave) {
        return no;
    }

    // Chave nao encontrada
    if (no->folha == '1') {
        return no;
    }

    return buscaChaveArvoreB(chave, no->descendentes[i], arquivo);
}

No* dividirNo (No* no, int novoRRN) {
    No *novo = criaNo();
    novo->folha = no->folha;
    novo->alturaNo = no->alturaNo;
    novo->RRNdoNo = novoRRN;

    for (int i = 0; i < novo->nroChavesNo; i++) {
        novo->dados[i] = no->dados[i + (ORDEM / 2)];
        novo->descendentes[i] = no->descendentes[i + (ORDEM / 2)];
    }
    novo->descendentes[novo->nroChavesNo] = no->descendentes[ORDEM - 1];

    no->nroChavesNo = (ORDEM - 1) / 2;

    return novo;
}

void imprimeNo (No* n) {
    printf("folha: %c, nroChavesNo: %d, alturaNo: %d, RRNdoNo: %d, descendentes: ", n->folha, n->nroChavesNo, n->alturaNo, n->RRNdoNo);
    for (int i = 0; i < ORDEM; i++) {
        printf("%d ", n->descendentes[i]);
    }
    printf(", dados: ");
    for (int i = 0; i < ORDEM - 1; i++) {
        printf("%d ", n->dados[i].chave);
    }
    printf("\n");
}

int buscaChaveNo (No* no, int chave) {
    int i = 0;
    while (i < no->nroChavesNo && chave > no->dados[i].chave) {
        i++;
    }

    if (i < no->nroChavesNo && chave == no->dados[i].chave) {
        return i;
    }

    return -1;
}

int insereChaveArvoreB (int chave, int* RRN, FILE *arquivo) {
    No* ind = buscaChaveArvoreB(chave, *RRN, arquivo);

    int indexChave = buscaChaveNo(ind, chave);
    // Chave já está inserida
    if (indexChave != -1 || ind->folha != '1') {
        return -1;
    }

    ind->RRNdoNo = *RRN;
    // Nó não está cheio
    if (ind->nroChavesNo < ORDEM - 1) {
        for (int i = 0; i < ind->nroChavesNo - 1; i++) {
            if (chave < ind->dados[i].chave && chave > ind->dados[i + 1].chave) {
                // Passa todas as chaves para a direita
                for (int j = ind->nroChavesNo; j > i; j--) {
                    ind->dados[j] = ind->dados[j - 1];
                }
                ind->dados[i].chave = chave;
                ind->dados[i].referencia = -1;
                ind->nroChavesNo++;
                break;
            }
        }
        escreveNo(ind, arquivo);
        return SUCESSO;
    }

    // Nó está cheio
    (*RRN)++;
    No* novo = dividirNo(ind, *RRN + 1);

    // Chave a ser inserida é menor que a chave do meio
    if (chave < novo->dados[0].chave) {
        for (int i = ind->nroChavesNo - 1; i >= 0; i--) {
            if (chave < ind->dados[i].chave) {
                ind->dados[i + 1] = ind->dados[i];
                ind->dados[i].chave = chave;
                ind->dados[i].referencia = -1;
                break;
            }
        }
    }
    // Chave a ser inserida é maior que a chave do meio
    else {
        novo->dados[novo->nroChavesNo].chave = chave;
        novo->dados[novo->nroChavesNo].referencia = -1;
        novo->nroChavesNo++;
    }

    escreveNo(ind, arquivo);
    escreveNo(novo, arquivo);
   

    return SUCESSO;
}