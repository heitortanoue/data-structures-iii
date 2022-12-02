#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../headers/grafos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/cabecalho.h"
#include "../headers/orgarquivos.h"

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
    vertice->cor = BRANCO;

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
    while (aux != NULL && aux->idConecta < id) {
        aux = aux->proxVertice;
    }
    if (aux != NULL && aux->idConecta == id) {
        return aux;
    } else {
        return NULL;
    }
}

Aresta* procuraIdAresta (Vertice* vertice, int id) {
    Aresta* aux = vertice->inicioArestas;
    while (aux != NULL && aux->idPoPsConectado < id) {
        aux = aux->proxAresta;
    }
    if (aux != NULL && aux->idPoPsConectado == id) {
        return aux;
    } else {
        return NULL;
    }
    return aux;
}

int criaGrafoArquivo (Grafo* g, Cabecalho* c, FILE* bin) {
    int qtdRegs = contarRegistros(bin);

    lerCabecalhoArquivo(bin, c);

    Registro r, r_aux;
    criaRegistro(&r);
    criaRegistro(&r_aux);

    if (!qtdRegs) {
        printf("Registro inexistente.\n\n");
        printf("Numero de paginas de disco: %d\n\n", c->nroPagDisco);
        return ERRO;
    }

    for (int i = 0; i < qtdRegs; i++) {
        if(!lerRegistroArquivo(bin, &r)){
            int ja_estava = 1;
            Vertice *v = procuraIdVertice(g, r.idConecta);

            if (v == NULL) {
                v = alocaVertice();
                copiaRegistroParaVertice(&r, v);
                ja_estava = 0;
            }

            Aresta* a = alocaAresta();
            copiaRegistroParaAresta(&r, a);
            adicionaArestaVertice(v, a);

            if (!ja_estava) {
                adicionaVerticeGrafo(g, v);
            }
        }
    }

    destroiRegistro(&r);
    destroiRegistro(&r_aux);

    return SUCESSO;
}

// Busca em profundidade grafos, conta os ciclos
void buscaEmProfundidade (Grafo* g, Vertice* v, int* num_ciclos) {
    v->cor = CINZA;
    Aresta* a = v->inicioArestas;
    while (a != NULL) {
        Vertice* w = procuraIdVertice(g, a->idPoPsConectado);
        if (w == NULL) {
            return;
        }
        if (w->cor == CINZA) {
            (*num_ciclos)++;
        } else if (w->cor == BRANCO) {
            buscaEmProfundidade(g, w, num_ciclos);
        }
        a = a->proxAresta;
    }
    v->cor = PRETO;
}
