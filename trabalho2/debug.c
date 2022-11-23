#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

#define ORDEM 5
#define TAM_PG_DISCO_INDICE 65
#define TAM_CABECALHO_INDICE 17
#define TAM_NO 65


// Funcao apenas com a finalidade de debugar um arquivo binario indicando quais bytes sao diferentes do arquivo de referencia
int debug(FILE* arq1, FILE* arq2) {
    int i = 0;
    while (!feof(arq1) && !feof(arq2)) {
        char c1 = fgetc(arq1);
        char c2 = fgetc(arq2);

        if (c1 != c2) {
            printf("[%d] (%c - %d) (%c - %d)\n", i, c1, c1, c2, c2);
            //return 1;
        }

        i++;
    }

    fclose(arq1);
    fclose(arq2);
    return 0;
}

int calculaByteOffsetArvoreB (int RRN) {
    return (TAM_PG_DISCO_INDICE + (RRN * TAM_NO));
}

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

No* leNo (int RRN, FILE *arquivo) {
    No* novo = criaNo();

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

void leCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo) {
    fseek(arquivo, 0, SEEK_SET);
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->noRaiz, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroChavesTotal, sizeof(int), 1, arquivo);
    fread(&cabecalho->alturaArvore, sizeof(int), 1, arquivo);
    fread(&cabecalho->RRNproxNo, sizeof(int), 1, arquivo);
}

void escreveNoTxt (No *n, FILE *arquivo) {
    fprintf(arquivo, "folha: %c, nroChavesNo: %d, alturaNo: %d, RRNdoNo: %d, descendentes: ", n->folha, n->nroChavesNo, n->alturaNo, n->RRNdoNo);
    for (int i = 0; i < ORDEM; i++) {
        fprintf(arquivo, "%d ", n->descendentes[i]);
    }
    fprintf(arquivo, ", dados: ");
    for (int i = 0; i < ORDEM - 1; i++) {
        fprintf(arquivo, "%d (%d) ", n->dados[i].chave, n->dados[i].referencia);
    }
    fprintf(arquivo, "\n");
}

void escreveCabecalhoIndiceTxt (CabecalhoIndice *c, FILE *arquivo) {
    fprintf(arquivo, "status: %c, RRNproxNo: %d, alturaArvore: %d, noRaiz: %d, nroChavesTotal: %d\n", c->status, c->RRNproxNo, c->alturaArvore, c->noRaiz, c->nroChavesTotal);
}

int createTxt (FILE* arq, FILE* arqTxt) {
    int i = 0;
    CabecalhoIndice c;
    leCabecalhoIndice(&c, arq);
    escreveCabecalhoIndiceTxt(&c, arqTxt);
    
    No *n;
    while (!feof(arq)) {
        n = leNo(i, arq);
        escreveNoTxt(n, arqTxt);
        i++;
    }

    fclose(arq);
    fclose(arqTxt);
    return 0;
}


FILE* abrirArquivo (char* nomeArquivo, char* modo) {
    FILE* arq = fopen(nomeArquivo, modo);
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }
}

int main (int argc, char* argv[]) {
    // int comando;
    // char nomeArquivo[100];
    // char nomeArquivo2[100];

    // scanf("%d", &comando);

    switch (atoi(argv[1])) {
    case 1:
        // printf("Digite o nome do arquivo binario 1: ");
        // scanf("%s", nomeArquivo);
        // printf("Digite o nome do arquivo binario 2: ");
        // scanf("%s", nomeArquivo2);

        debug(abrirArquivo(argv[2], "rb"), abrirArquivo(argv[3], "rb"));

        break;
    case 2:
        // printf("Digite o nome do arquivo binario: ");
        // scanf("%s", nomeArquivo);
        // printf("Digite o nome do arquivo texto de saida: ");
        // scanf("%s", nomeArquivo2);

        createTxt(abrirArquivo(argv[2], "rb"), abrirArquivo(argv[3], "w"));
        break;
    case 3:
        // argv
        // 1 - comando
        // 2 - nome do arquivo binario de indice meu
        // 3 - nome do arquivo binario de indice de debug
        debug(abrirArquivo(argv[2], "rb"), abrirArquivo(argv[3], "rb"));
        createTxt(abrirArquivo(argv[2], "rb"), abrirArquivo(argv[4], "w"));
        createTxt(abrirArquivo(argv[3], "rb"), abrirArquivo(argv[5], "w"));
        break;

    default:
        break;
    }

    return 0;
}