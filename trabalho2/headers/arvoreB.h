#ifndef __ARVORE_B_H__
#define __ARVORE_B_H__

#include <stdio.h>
#define ORDEM 5
#define TAM_PG_DISCO_INDICE 65
#define TAM_CABECALHO_INDICE 17
#define TAM_NO 65

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

No* criaNo ();
void destroiNo (No* no);
void escreveNo (No *no, FILE *arquivo);
No* leNo (int RRN, FILE *arquivo);
void imprimeNo (No* n);

int calculaByteOffsetArvoreB (int RRN);

void criaCabecalhoIndice (CabecalhoIndice* cabecalho);
void escreveCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo);
void leCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo);

Indice* criaIndice (int chave, int referencia);
void destroiIndice (Indice* indice);
void limpaIndice (Indice* indice);
Indice* copiaIndice (Indice* origem);
Indice* promoveIndice(No *noFilhoEsq, No *noFilhoDir, No *noPai, Indice *chave);

int noCheio (No* no);

int buscaChaveNo (No* no, int chave);
int referenciaChaveNo (No* no, int chave);
int insereChaveNo (No* no, Indice* indice, int desc);
void organizaDescendentes(No *no);
int organizaNo(No *no);
int buscaProxNo(No* noBusca, Indice *chave);
void divideNo(No *noAntigo, No *noNovo, CabecalhoIndice *cabecalho);

No* insereDivisaoRecursivo(No* noPai, No* noFilho, FILE *arquivo, Indice *chave, CabecalhoIndice *cabecalho);

No* buscaChaveArvoreB (int chave, int RRN, FILE *arquivo, int* status, int *pgs_acessadas);
void imprimeArvoreB(FILE* arquivo, CabecalhoIndice* c);
int insereChaveArvoreB (Indice* indice, CabecalhoIndice* ci, FILE *arquivo);

#endif