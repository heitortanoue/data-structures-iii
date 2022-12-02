#ifndef __GRAFOS_H__
#define __GRAFOS_H__

#include "../headers/registro.h"
#define BRANCO 1
#define CINZA 2
#define PRETO 3

typedef struct Aresta {
    int idPoPsConectado;
    double velocidade;
    struct Aresta* proxAresta;
} Aresta;

typedef struct Vertice {
    int idConecta;
    char *nomePoPs;
    char *nomePais;
    char *siglaPais;

    int tamanho;
    Aresta* inicioArestas;
    struct Vertice* proxVertice;

    int cor;
} Vertice;

typedef struct {
    int tamanho;
    Vertice* inicioVertices;
} Grafo;

double converterVelocidadeParaGbps (double velocidade, char unidade);

Aresta* alocaAresta ();
void destroiAresta (Aresta* aresta);
void copiaRegistroParaAresta (Registro* r, Aresta* aresta);

Vertice* alocaVertice();
void destroiVertice (Vertice* lista);
void copiaRegistroParaVertice (Registro* r, Vertice* vertice);
void adicionaArestaVertice (Vertice* lista, Aresta* aresta);
void imprimeVertice (Vertice* lista);

Grafo* alocaGrafo ();
void destroiGrafo (Grafo* grafo);
void adicionaVerticeGrafo (Grafo* grafo, Vertice *vertice);
void imprimeGrafo (Grafo* grafo);

Vertice* procuraIdVertice (Grafo* grafo, int id);

void buscaEmProfundidade (Grafo* g, Vertice* v, int* num_ciclos);

int criaGrafoArquivo (Grafo* g, Cabecalho* c, FILE* bin);

#endif