#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../headers/grafos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/cabecalho.h"
#include "../headers/orgarquivos.h"

// Converte a velocidade dependendo da unidade passada por parametro
long int converterGbpsParaMpbs (int velocidade, char unidade) {
    if (unidade == 'G') {
        return velocidade * 1024;
    }
    return velocidade;
}

// Aloca memoria para um vertice
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

// Aloca memoria para uma aresta
Aresta* alocaAresta () {
    Aresta *aresta;
    aresta = (Aresta*) malloc(sizeof(Aresta));
    aresta->idPoPsConectado = -1;
    aresta->velocidade = -1;
    aresta->proxAresta = NULL;
    return aresta;
}

// Desaloca memoria de uma aresta
void destroiAresta (Aresta* aresta) {
    free(aresta);
}

// Copia os dados do registro para a aresta
void copiaRegistroParaAresta (Registro* r, Aresta* aresta) {
    aresta->idPoPsConectado = r->idPoPsConectado;
    aresta->velocidade = converterGbpsParaMpbs(r->velocidade, r->unidadeMedida);
}

// Copia os dados do registro para o vertice
void copiaRegistroParaVertice (Registro* r, Vertice* vertice) {
    vertice->idConecta = r->idConecta;
    strcpy(vertice->nomePoPs, r->nomePoPs);
    strcpy(vertice->nomePais, r->nomePais);
    strcpy(vertice->siglaPais, r->siglaPais);
}

// Desaloca memoria de um vertice
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
    // Verifica se os dados da aresta são validos
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

    // Percorre a lista de adjacências do vertice e para antes do maior idConecta (maior que o passado)
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

// Iprime um vertice de acordo com as especificações do trabalho
void imprimeVertice (Vertice* v) {
    Aresta* aux = v->inicioArestas;
    // percorre a lista de adjacências do vertice
    while (aux != NULL) {
        printf("%d %s %s %s %d %ldMbps\n", v->idConecta, v->nomePoPs, v->nomePais, v->siglaPais, aux->idPoPsConectado, aux->velocidade);
        aux = aux->proxAresta;
    }
}

// Imprime um grafo inteiro de acordo com as especificações do trabalho
void imprimeGrafo (Grafo* grafo) {
    Vertice* aux = grafo->inicioVertices;
    // percorre a lista de vertices do grafo
    while (aux != NULL) {
        imprimeVertice(aux);
        aux = aux->proxVertice;
    }
}

// Aloca memoria para um grafo
Grafo* alocaGrafo () {
    Grafo* grafo = (Grafo*) malloc (sizeof (Grafo));
    grafo->tamanho = 0;
    grafo->inicioVertices = NULL;
    return grafo;
}

// Desaloca memoria de um grafo, seus vertices e arestas
void destroiGrafo (Grafo* grafo) {
    Vertice* vertice = grafo->inicioVertices;
    Vertice* aux;
    // percorre a lista de vertices do grafo, destruindo-os
    while (vertice != NULL) {
        aux = vertice;
        vertice = vertice->proxVertice;
        destroiVertice(aux);
    }
    free(grafo);
}

// Adiciona um vertice em um grafo de acordo com o idConecta (ordem crescente)
void adicionaVerticeGrafo (Grafo* grafo, Vertice *vertice) {
    Vertice* aux = grafo->inicioVertices;
    Vertice* anterior = NULL;

    // Caso o grafo esteja vazio, o vertice é adicionado no inicio
    if (grafo->inicioVertices == NULL) {
        grafo->inicioVertices = vertice;
        return;
    }

    // Se o grafo nao estiver vazio, percorre a lista de vertices do grafo, procurando o vertice com idConecta maior que o passado
    while (aux != NULL && aux->idConecta < vertice->idConecta) {
        anterior = aux;
        aux = aux->proxVertice;
    }
    // Para antes do vertice com idConecta maior que o passado
    // Se o vertice com idConecta maior que o passado for o primeiro, o vertice é adicionado no inicio
    if (anterior == NULL) {
        vertice->proxVertice = grafo->inicioVertices;
        grafo->inicioVertices = vertice;
    } 
    // Se o vertice com idConecta maior que o passado nao for o primeiro, o vertice é adicionado antes dele
    else {
        vertice->proxVertice = anterior->proxVertice;
        anterior->proxVertice = vertice;
    }
    // Incrementa o tamanho do grafo
    grafo->tamanho++;
}

// Procura um vertice em um grafo de acordo com o idConecta
Vertice* procuraIdVertice (Grafo* grafo, int id) {
    Vertice* aux = grafo->inicioVertices;

    // percorre a lista de vertices do grafo enquanto o idConecta for menor que o passado
    while (aux != NULL && aux->idConecta < id) {
        aux = aux->proxVertice;
    }
    // Se o idConecta for igual ao passado, retorna o vertice
    if (aux != NULL && aux->idConecta == id) {
        return aux;
    }
    // Se o idConecta nao for igual ao passado, retorna NULL
    else {
        return NULL;
    }
}

// Procura uma aresta em um vertice de acordo com o idConecta
Aresta* procuraIdAresta (Vertice* vertice, int id) {
    Aresta* aux = vertice->inicioArestas;
    // percorre a lista de adjacências do vertice enquanto o idPoPsConectado for menor que o passado
    while (aux != NULL && aux->idPoPsConectado < id) {
        aux = aux->proxAresta;
    }
    // Se o idPoPsConectado for igual ao passado, retorna a aresta
    if (aux != NULL && aux->idPoPsConectado == id) {
        return aux;
    } 
    // Se o idPoPsConectado nao for igual ao passado, retorna NULL
    else {
        return NULL;
    }
}

// Cria um grafo a partir de um arquivo binario
int criaGrafoArquivo (Grafo* g, Cabecalho* c, FILE* bin) {
    // Conta a quantidade de registros no arquivo
    int qtdRegs = contarRegistros(bin);

    // Le o cabecalho do arquivo
    lerCabecalhoArquivo(bin, c);

    // Aloca memoria para dois registros
    Registro r, r_aux;
    criaRegistro(&r);
    criaRegistro(&r_aux);

    // Se o arquivo estiver vazio, retorna ERRO
    if (!qtdRegs) {
        printf("Falha na execução da funcionalidade.\n");
        return ERRO;
    }

    // Le os registros do arquivo e adiciona-os no grafo
    for (int i = 0; i < qtdRegs; i++) {
        // Se o registro nao for removido, adiciona-o no grafo
        if(!lerRegistroArquivo(bin, &r)){
            // Adiciona o registro no grafo
            adicionaRegistroGrafo(g, &r);

            // Se o registro de idConecta == idPoPsConectado nao existir, nao adiciona a aresta
            if (buscaIdConecta(r.idPoPsConectado, &r_aux, bin, qtdRegs) == NAO_ENCONTRADO) {
                printf("Falha na execução da funcionalidade.\n");
                return ERRO;
            }
            // Copia os dados necessarios do registro para o registro auxiliar
            r_aux.velocidade = r.velocidade;
            r_aux.unidadeMedida = r.unidadeMedida;
            r_aux.idPoPsConectado = r.idConecta;

            // Adiciona o registro auxiliar no grafo
            adicionaRegistroGrafo(g, &r_aux);
        }
    }

    destroiRegistro(&r);
    destroiRegistro(&r_aux);

    return SUCESSO;
}

// Adiciona um registro em um grafo
void adicionaRegistroGrafo (Grafo* g, Registro* r) {
    // Flag para verificar se o vertice ja estava no grafo
    int ja_estava = 1;
    // Procura o vertice com idConecta igual ao do registro
    Vertice *v = procuraIdVertice(g, r->idConecta);

    // Se o vertice nao existir, aloca memoria para ele e copia os dados do registro para ele
    if (v == NULL) {
        v = alocaVertice();
        copiaRegistroParaVertice(r, v);
        ja_estava = 0;
    }

    // Aloca memoria para uma aresta e copia os dados do registro para ela
    Aresta* a = alocaAresta();
    copiaRegistroParaAresta(r, a);
    // Adiciona a aresta no vertice
    adicionaArestaVertice(v, a);

    // Se o vertice nao existia no grafo, adiciona ele no grafo
    if (!ja_estava) {
        adicionaVerticeGrafo(g, v);
    }
}

// Busca um idConecta em um grafo
int buscaIdConecta (int id, Registro *r, FILE* bin, int qtdRegs) {
    // Se o idConecta for invalido, retorna ERRO
    if (!intValido(id)) {
        return ERRO;
    }
    // Salva a posicao atual do arquivo
    long pos_atual = ftell(bin);
    // Volta para o inicio dos registros
    fseek(bin, calculaByteoffset(0), SEEK_SET);

    // Aloca memoria para um registro auxiliar
    Registro r_temp;
    criaRegistro(&r_temp);

    // Percorre os registros do arquivo
    for (int i = 0; i < qtdRegs; i++) {
        // Se o registro for removido, pula para o proximo
        if (!lerRegistroArquivo(bin, &r_temp)) {
            continue;
        }
        // Se o idConecta for igual ao passado, copia os dados do registro para o registro passado por parametro
        if (r_temp.idConecta == id) {
            copiaRegistro(r, &r_temp);
            destroiRegistro(&r_temp);
            fseek(bin, pos_atual, SEEK_SET);
            return SUCESSO;
        }
    }

    // Se o idConecta nao for encontrado, retorna NAO_ENCONTRADO
    destroiRegistro(&r_temp);
    fseek(bin, pos_atual, SEEK_SET);
    return NAO_ENCONTRADO;
}

// Conta a quantidade de ciclos simples em um grafo
int ContaCiclosSimplesGrafo(Grafo* g) {
    int n = 0;
    Vertice* u = g->inicioVertices;

    // Procura o maior idConecta
    while (u != NULL) {
        n = (u->idConecta > n) ? u->idConecta : n;
        u = u->proxVertice;
    }

    // Aloca memoria para os vetores de cores e pais do tamanho do maior idConecta
    unsigned short* color = (unsigned short*) malloc(sizeof(unsigned short) * (n + 1));
    Vertice **parent = (Vertice**) malloc(sizeof(Vertice*) * (n + 1));
    for (int i = 0; i <= n; i++) {
        color[i] = BRANCO;
        parent[i] = NULL;
    }

    int count = 0;

    u = g->inicioVertices;
    // Percorre os vertices do grafo e chama a busca em profundidade para cada um que ainda nao foi visitado
    while (u != NULL) {
        if (color[u->idConecta] == BRANCO) {
            count += buscaEmProfundidade(g, u, NULL, color, parent);
        }
        u = u->proxVertice;
    }

    free(parent);
    free(color);

    return count;
}

// Busca em profundidade recursiva para contar ciclos simples
int buscaEmProfundidade(Grafo* g, Vertice* verticeAtual, Vertice* pai, unsigned short* cores, Vertice** verticesPais) {
    // Se o vertice ja foi visitado, retorna 0
    if (cores[verticeAtual->idConecta] == PRETO) {
        return 0;
    }
    // Se o vertice é parcialmente visitado, retorna 1
    if (cores[verticeAtual->idConecta] == CINZA) {
        return 1;
    }

    // Salva o pai do vertice atual
    verticesPais[verticeAtual->idConecta] = pai;
    // Marca o vertice como parcialmente visitado
    cores[verticeAtual->idConecta] = CINZA;
    int contagem = 0;

    Aresta *a = verticeAtual->inicioArestas;
    // Percorre as arestas do vertice atual
    while (a != NULL) {
        // Se o vertice que a aresta se refere nao foi visitado, chama a busca em profundidade para ele
        if (!verticesPais[verticeAtual->idConecta] || a->idPoPsConectado != verticesPais[verticeAtual->idConecta]->idConecta) {
            contagem += buscaEmProfundidade(g, procuraIdVertice(g, a->idPoPsConectado), verticeAtual, cores, verticesPais);
        }

        a = a->proxAresta;
    }

    // Marca o vertice como totalmente visitado
    cores[verticeAtual->idConecta] = PRETO;
    return contagem;
}
