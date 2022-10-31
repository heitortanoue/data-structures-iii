#ifndef __ARVORE_B_H__
#define __ARVORE_B_H__

#define ORDEM 5
#define TAM_PG_DISCO 73
#define TAM_CABECALHO 73
#define TAM_REGISTRO 73

typedef struct {
    char status;
    int noRaiz;
    int nroChavesTotal;
    int alturaArvore;
    int RRNproxNo;
} Cabecalho;

typedef struct {
    int descendente;
    int chave;
    int referencia;
} Indice;

typedef struct {
    char folha;
    int nroChavesNo;
    int alturaNo;
    int RRNdoNo;
    Indice* chaves;
} No;

typedef int ArvoreB;


#endif