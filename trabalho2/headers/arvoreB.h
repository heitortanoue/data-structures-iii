#ifndef __ARVORE_B_H__
#define __ARVORE_B_H__

#include <stdio.h>
#define ORDEM 5
#define TAM_PG_DISCO_INDICE 73
#define TAM_CABECALHO_INDICE 17
#define TAM_NO 73

typedef struct {
    char status;
    int noRaiz;
    int nroChavesTotal;
    int alturaArvore;
    int RRNproxNo;
} CabecalhoIndice;

typedef struct {
    int chave;
    int referencia;
} Indice;

typedef struct {
    char folha;
    int nroChavesNo;
    int alturaNo;
    int RRNdoNo;
    Indice* dados;
    int *descendentes;
} No;

// typedef int ArvoreB;

No* criaNo ();
void escreveNo (No *no, FILE *arquivo);
No* leNo (int RRN, FILE *arquivo);
void escreveCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo);
void leCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo);
No* buscaChaveArvoreB (int chave, int RRN, FILE *arquivo);
int insereChaveArvoreB (int chave, int* RRN, FILE *arquivo);
void criaCabecalhoIndice (CabecalhoIndice* cabecalho);

#endif