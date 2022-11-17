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

void destroiNo (No* no) {
    free(no->dados);
    free(no->descendentes);
    free(no);
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
    // imprimeNo(novo);

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

No* buscaChaveArvoreB (int chave, int RRN, int* RRNPai, FILE *arquivo, int* status) {
    No *no = leNo(RRN, arquivo);

    int i = 0;
    // Enquanto a chave buscada for maior que a chave do nó e não for a última chave do nó
    while (i < no->nroChavesNo && chave > no->dados[i].chave) {
        i++;
    }

    // Chave encontrada
    if (i < no->nroChavesNo && chave == no->dados[i].chave) {
        *status = SUCESSO;
        return no;
    }

    // Chave nao encontrada
    if (no->folha == '1') {
        *status = NAO_ENCONTRADO;
        return no;
    }

    if (no->descendentes[i] < 0) {
        *status = ERRO;
        return no;
    }
    *RRNPai = RRN;
    printf("i: %d, folha: %c, descendentes[i]: %d\n", i, no->folha, no->descendentes[i]);
    printf("RRN: %d, RRNPai: %d, status: %d\n", RRN, *RRNPai, *status);
    return buscaChaveArvoreB(chave, no->descendentes[i], RRNPai, arquivo, status);
}

void dividirNo (No* no, No* novo_esq, No* novo_dir, CabecalhoIndice* ci, Indice* ind, FILE* arq) {
    novo_esq->folha = no->folha;
    novo_dir->folha = no->folha;
    no->folha = '0';

    novo_esq->alturaNo = no->alturaNo + 1;
    novo_dir->alturaNo = no->alturaNo + 1;
    novo_esq->RRNdoNo = ci->RRNproxNo;
    novo_dir->RRNdoNo = ci->RRNproxNo + 1;
    ci->RRNproxNo += 2;

    // printf("Nó original:\n");
    // imprimeNo(no);

    int chaveMax = ORDEM - 1;
    for (int i = 0, j = 0; i < chaveMax/2; i++, j++) {
        novo_esq->dados[j] = no->dados[i];
        novo_esq->descendentes[j] = no->descendentes[i];
        limpaIndice(&no->dados[i]);
        no->descendentes[i] = -1;

        novo_esq->nroChavesNo++;
        no->nroChavesNo--;
    }
    for (int i = chaveMax/2, j = 0; i < chaveMax; i++, j++) {
        novo_dir->dados[j] = no->dados[i];
        novo_dir->descendentes[j] = no->descendentes[i];
        limpaIndice(&no->dados[i]);
        no->descendentes[i] = -1;

        novo_dir->nroChavesNo++;
        no->nroChavesNo--;
    }

    if (ind->chave < novo_dir->dados[0].chave) {
        insereChaveNo(novo_esq, ind, arq);
    } else {
        insereChaveNo(novo_dir, ind, arq);
    }

    // printf("Nó original após divisão [inserção da chave %d]:\n", ind->chave);
    // imprimeNo(no);
    // printf("Nó esquerdo:\n");
    // imprimeNo(novo_esq);
    // printf("Nó direito:\n");
    // imprimeNo(novo_dir);

    return;
}

void imprimeNo (No* n) {
    printf("folha: %c, nroChavesNo: %d, alturaNo: %d, RRNdoNo: %d, descendentes: ", n->folha, n->nroChavesNo, n->alturaNo, n->RRNdoNo);
    for (int i = 0; i < ORDEM; i++) {
        printf("%d ", n->descendentes[i]);
    }
    printf(", dados: ");
    for (int i = 0; i < ORDEM - 1; i++) {
        printf("%d (%d) ", n->dados[i].chave, n->dados[i].referencia);
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

int referenciaChaveNo (No* no, int chave) {
    return no->dados[buscaChaveNo(no, chave)].referencia;
}

Indice* criaIndice (int chave, int referencia) {
    Indice *novo = (Indice*) malloc(sizeof(Indice));
    novo->chave = chave;
    novo->referencia = referencia;

    return novo;
}

void destroiIndice (Indice* indice) {
    free(indice);
}

void limpaIndice (Indice* indice) {
    indice->chave = -1;
    indice->referencia = -1;
}

void retiraNulosIndices (No* no) {
    int range = no->nroChavesNo;

    for (int i = 0; i < range; i++) {
        if (no->dados[i].chave == -1) {
            for (int j = i; j < ORDEM - 2; j++) {
                no->dados[j] = no->dados[j + 1];
            }
            no->nroChavesNo--;
        }
    }
}

Indice* copiaIndice (Indice* origem) {
    Indice *novo = criaIndice(origem->chave, origem->referencia);

    return novo;
}

int insereChaveNo (No* no, Indice* indice, FILE* arquivo) {
    int i = 0;
    while (i < no->nroChavesNo && indice->chave > no->dados[i].chave) {
        i++;
    }

    if (i < no->nroChavesNo && indice->chave == no->dados[i].chave) {
        return ERRO;
    }

    // Passa os dados e descendentes para a direita
    for (int j = no->nroChavesNo; j > i; j--) {
        no->dados[j] = no->dados[j - 1];
        no->descendentes[j + 1] = no->descendentes[j];
    }


    no->dados[i].chave = indice->chave;
    no->dados[i].referencia = indice->referencia;
    no->nroChavesNo++;

    escreveNo(no, arquivo);

    return SUCESSO;
}

int insereChaveArvoreB (Indice* indice, CabecalhoIndice* ci, FILE *arquivo) {
    if (ci->noRaiz == -1) {
        No *novo = criaNo();
        novo->folha = '1';
        novo->alturaNo = 1;
        novo->RRNdoNo = ci->RRNproxNo;
        insereChaveNo(novo, indice, arquivo);

        printf("[1] Nova raiz criada\n");
        imprimeNo(novo);

        destroiNo(novo);
        ci->noRaiz = ci->RRNproxNo;
        return SUCESSO;
    }

    int status_busca, RRN_busca_pai = -1;
    No* ind = buscaChaveArvoreB(indice->chave, ci->noRaiz, &RRN_busca_pai, arquivo, &status_busca);

    // Se a chave já existe, retorna erro
    if (status_busca != NAO_ENCONTRADO) {
        printf("[ERRO NA BUSCA] Status: %d\n", indice->chave, status_busca);
        destroiNo(ind);
        return ERRO; 
    }

    // Cria um novo nó caso não exista
    if (ind->RRNdoNo < 0) {
        ind->RRNdoNo = ci->RRNproxNo;
    }

    // Verifica se o nó não está cheio e é folha
    if (ind->nroChavesNo < ORDEM - 1 && ind->folha == '1') {
        insereChaveNo(ind, indice, arquivo);

        if (ind->nroChavesNo == ORDEM - 1) {
            (ci->RRNproxNo)++;
        }

        printf("[2] Nó folha não cheio\n");
        imprimeNo(ind);

        destroiNo(ind);
        return SUCESSO;
    }

    // Se o nó está cheio, divide o nó
    No* novo_esq = criaNo();
    No* novo_dir = criaNo();
    dividirNo(ind, novo_esq, novo_dir, ci, indice, arquivo);

    // Promove uma chave
    Indice* ind_promovido;
    if (novo_esq->nroChavesNo > novo_dir->nroChavesNo) {
        // Promove última chave do no esquerdo
        ind_promovido = copiaIndice(&novo_esq->dados[novo_esq->nroChavesNo - 1]);
        limpaIndice(&novo_esq->dados[novo_esq->nroChavesNo - 1]);
        retiraNulosIndices(novo_esq);
    } else {
        // Promove primeira chave no nó direito
        ind_promovido = copiaIndice(&novo_dir->dados[0]);
        limpaIndice(&novo_dir->dados[0]);
        retiraNulosIndices(novo_dir);
    }

    insereChaveNo(ind, ind_promovido, arquivo);
    destroiIndice(ind_promovido);

    printf("[3] Nó cheio, promovendo chave\n");
    printf("Nó original:\n");
    imprimeNo(ind);
    printf("Nó esquerdo:\n");
    imprimeNo(novo_esq);
    printf("Nó direito:\n");
    imprimeNo(novo_dir);

    // if (RRN_busca_pai < 0) {
    //     // Cria novo nó raiz
    //     ind->folha = '0';
    //     ind->alturaNo = ind->alturaNo + 1;
    //     ind->descendentes[0] = novo_esq->RRNdoNo;
    //     ind->descendentes[1] = novo_dir->RRNdoNo;
    //     insereChaveNo(ind, ind_promovido, arquivo);
    //     ci->noRaiz = ind->RRNdoNo;
    //     (ci->RRNproxNo)++;

    //     destroiNo(ind);
    //     destroiNo(novo_esq);
    //     destroiNo(novo_dir);

    //     return SUCESSO;
    // }

    // No* pai = leNo(RRN_busca_pai, arquivo);
    // insereChaveNo(pai, ind_promovido, arquivo);

    destroiNo(ind);
    destroiNo(novo_esq);
    destroiNo(novo_dir);

    return SUCESSO;
}