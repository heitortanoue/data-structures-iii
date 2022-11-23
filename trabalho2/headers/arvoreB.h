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
void escreveNo (No *no, FILE *arquivo);
No* leNo (int RRN, FILE *arquivo);
void imprimeNo (No* n);
int referenciaChaveNo (No* no, int chave);
void escreveCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo);
void leCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo);
void limpaIndice (Indice* indice);
No* buscaChaveArvoreB (int chave, int RRN, int* caminho, int* altura, FILE *arquivo, int* status);
int insereChaveArvoreB (Indice* indice, CabecalhoIndice* ci, FILE *arquivo);
void criaCabecalhoIndice (CabecalhoIndice* cabecalho);
int insereChaveNo (No* no, Indice* indice, int desc);
void dividirNo (No* no, No* no_esq, No* no_dir, CabecalhoIndice* ci, Indice* ind, FILE* arq);
int dividirNoComPai (No* pai, No* no_esq, No* novo_dir, CabecalhoIndice* ci, Indice* ind, FILE* arq);
Indice* copiaIndice (Indice* origem);
void destroiIndice (Indice* indice);
void retiraNulosIndices (No* no);
void divideNoTeste(No *noAntigo, No *noNovo, CabecalhoIndice *cabecalho);
No* insereDivisaoRecursivo(No* noPai, No* noFilho, FILE *arquivo, Indice *chave, CabecalhoIndice *cabecalho);
int buscaProxNo(No* noBusca, Indice *chave);
Indice* promoveIndice(No *noFilhoEsq, No *noFilhoDir, No *noPai, Indice *chave);
int organizaNo(No *no);
void imprimeArvoreB(FILE* arquivo, CabecalhoIndice* c);
void organizaDescendentes(No *no);

#endif