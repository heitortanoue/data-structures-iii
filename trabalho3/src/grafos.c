#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/grafos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"

double converterVelocidadeParaGbps (double velocidade, char unidade) {
    if (unidade == 'M') {
        return velocidade * 0.0009765625;
    } else {
        return velocidade;
    }
}

Vertice* alocaVertice () {
    Vertice* vertice = (Vertice*) malloc (sizeof (Vertice));
    vertice->idConecta = -1;
    vertice->nomePais = malloc(sizeof (char) * TAM_STRING);
    vertice->nomePoPs = malloc(sizeof (char) * TAM_STRING);
    vertice->siglaPais = malloc(sizeof (char) * (TAM_SIGLA+1));
    vertice->tamanho = 0;
    vertice->inicioArestas = NULL;
    vertice->proxVertice = NULL;
    return vertice;
}

Aresta* alocaAresta () {
    Aresta *aresta;
    aresta = (Aresta*) malloc(sizeof(Aresta));
    aresta->idPoPsConectado = -1;
    aresta->velocidade = -1;
    aresta->proxAresta = NULL;
    return aresta;
}

void destroiAresta (Aresta* aresta) {
    free(aresta);
}

void copiaRegistroParaAresta (Registro* r, Aresta* aresta) {
    aresta->idPoPsConectado = r->idPoPsConectado;
    aresta->velocidade = converterVelocidadeParaGbps((double) r->velocidade, r->unidadeMedida);
}

void copiaRegistroParaVertice (Registro* r, Vertice* vertice) {
    vertice->idConecta = r->idConecta;
    strcpy(vertice->nomePoPs, r->nomePoPs);
    strcpy(vertice->nomePais, r->nomePais);
    strcpy(vertice->siglaPais, r->siglaPais);
}

void destroiVertice (Vertice* vertice) {
    Aresta* aresta = vertice->inicioArestas;
    Aresta* aux;
    while (aresta != NULL) {
        aux = aresta;
        aresta = aresta->proxAresta;
        destroiAresta(aux);
    }
    free(vertice->nomePais);
    free(vertice->nomePoPs);
    free(vertice->siglaPais);
    free(vertice);
}

// Adiciona na lista de adjacências com idConecta crescente na lista encadeada
void adicionaArestaVertice (Vertice* vertice, Aresta* aresta) {
    if (!intValido(aresta->idPoPsConectado) || !doubleValido(aresta->velocidade)) {
        destroiAresta(aresta);
        return;
    }

    Aresta* aux = vertice->inicioArestas;
    Aresta* anterior = NULL;

    if (vertice->inicioArestas == NULL) {
        vertice->inicioArestas = aresta;
        return;
    }

    while (aux != NULL && aux->idPoPsConectado < aresta->idPoPsConectado) {
        anterior = aux;
        aux = aux->proxAresta;
    }
    if (anterior == NULL) {
        aresta->proxAresta = vertice->inicioArestas;
        vertice->inicioArestas = aresta;
    } else {
        aresta->proxAresta = anterior->proxAresta;
        anterior->proxAresta = aresta;
    }
    vertice->tamanho++;
}

void imprimeVertice (Vertice* v) {
    Aresta* aux = v->inicioArestas;
    while (aux != NULL) {
        printf("%d %s %s %s %d %lfGbps\n", v->idConecta, v->nomePoPs, v->nomePais, v->siglaPais, aux->idPoPsConectado, aux->velocidade);
        aux = aux->proxAresta;
    }
}

void imprimeGrafo (Grafo* grafo) {
    Vertice* aux = grafo->inicioVertices;
    while (aux != NULL) {
        imprimeVertice(aux);
        // printf("\n");
        aux = aux->proxVertice;
    }
}

Grafo* alocaGrafo () {
    Grafo* grafo = (Grafo*) malloc (sizeof (Grafo));
    grafo->tamanho = 0;
    grafo->inicioVertices = NULL;
    return grafo;
}

void destroiGrafo (Grafo* grafo) {
    Vertice* vertice = grafo->inicioVertices;
    Vertice* aux;
    while (vertice != NULL) {
        aux = vertice;
        vertice = vertice->proxVertice;
        destroiVertice(aux);
    }
    free(grafo);
}

void adicionaVerticeGrafo (Grafo* grafo, Vertice *vertice) {
    Vertice* aux = grafo->inicioVertices;
    Vertice* anterior = NULL;

    if (grafo->inicioVertices == NULL) {
        grafo->inicioVertices = vertice;
        return;
    }

    while (aux != NULL && aux->idConecta < vertice->idConecta) {
        anterior = aux;
        aux = aux->proxVertice;
    }
    if (anterior == NULL) {
        vertice->proxVertice = grafo->inicioVertices;
        grafo->inicioVertices = vertice;
    } else {
        vertice->proxVertice = anterior->proxVertice;
        anterior->proxVertice = vertice;
    }
    grafo->tamanho++;
}

Vertice* procuraIdVertice (Grafo* grafo, int id) {
    Vertice* aux = grafo->inicioVertices;
    while (aux != NULL && aux->idConecta != id) {
        aux = aux->proxVertice;
    }
    return aux;
}

// int qntCiclosGrafo (Grafo* grafo) {

// }