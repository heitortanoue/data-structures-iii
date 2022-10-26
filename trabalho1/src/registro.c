#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/registro.h"
#include "../headers/orgarquivos.h"
#include "../headers/csv.h"
#define LIXO '$'

// Cria um novo registro e aloca memoria para suas strings
int criaRegistro (Registro *r) {
    r->removido = '0';
    r->encadeamento = -1;
    r->siglaPais = (char*) malloc(sizeof(char) * (TAM_SIGLA + 1));
    r->nomePoPs = (char*) malloc(sizeof(char) * TAM_STRING);
    r->nomePais = (char*) malloc(sizeof(char) * TAM_STRING);

    return SUCESSO;
}

// Libera a memória alocada para um registro
int destroiRegistro (Registro *r) {
    free(r->siglaPais);
    free(r->nomePoPs);
    free(r->nomePais);

    return SUCESSO;
}

//Chamada da leitura dos campos a cada registro no teclado
int lerDadosRegistroTeclado(Registro *t) {
    char str[128];
    t->removido = '0';
    t->encadeamento = -1;

    scanf("%d", &t->idConecta);
    readline(str);
    insereStringRegistro(str, t->siglaPais, TAM_SIGLA);
    scanf("%d", &t->idPoPsConectado);
    readline(str);
    scanf("%c", &t->unidadeMedida);
    scanf("%d", &t->velocidade);
    readline(t->nomePoPs);
    readline(t->nomePais);

    return SUCESSO;
}

// Copia toda a string, sem parar no '/0'
int copiaStringRegistro (char* destino, char* origem, int tam) {
    for (int i = 0; i < tam; i++) {
        destino[i] = origem[i];
    }
    return SUCESSO;
}

// Indica se o registro foi removido logicamente
int registroRemovido (Registro *r) {
    return r->removido == REMOVIDO;
}

void readline(char* string){
    char c = 0;

    do{
        c = (char) getchar();

    } while(c == '\n' || c == '\r');

    int i = 0;

    do{
        string[i] = c;
        i++;
        c = getchar();
    } while(c != '\n' && c != '\r');

    string[i]  = '\0';
}

//Leitura de cada campo no respectivo registro
int insereStringRegistro (char* str, char* str_registro, int tam_campo) {
    char campo_trunc[tam_campo];

    int tam_str = strlen(str) + 1;

    if (tam_campo > tam_str) {
        // Copia a string e preenche o resto com lixo
        int dif = tam_campo - tam_str;
        for (int i = 0; i < tam_str; i++) {
            campo_trunc[i] = str[i];
        }
        for (int i = 0; i < dif; i++) {
            campo_trunc[tam_str + i] = LIXO;
        }
    } else {
        // Corta o pedaço da string que não cabe no campo
        for (int i = 0; i < tam_campo - 1; i++) {
            campo_trunc[i] = str[i];
        }
        campo_trunc[tam_campo - 1] = '\0';
    }

    copiaStringRegistro(str_registro, campo_trunc, tam_campo);
    return SUCESSO;
}

// Imprime todas as informações do registro
void imprimeRegistro (Registro *r ) {
    if (intValido(r->idConecta)) {
        printf("Identificador do ponto: %d\n", r->idConecta);
    }
    if (stringValida(r->nomePoPs)) {
        printf("Nome do ponto: %s\n", r->nomePoPs);
    }
    if (stringValida(r->nomePais)) {
        printf("Pais de localizacao: %s\n", r->nomePais);
    }
    if (stringValida(r->siglaPais)) {
        printf("Sigla do pais: %s\n", r->siglaPais);
    }
    if (intValido(r->idPoPsConectado)) {
        printf("Identificador do ponto conectado: %d\n", r->idPoPsConectado);
    }
    if (intValido(r->velocidade) && charValido(r->unidadeMedida)) {
        printf("Velocidade de transmissao: %d %c%s\n", r->velocidade, r->unidadeMedida, "bps");
    }
    printf("\n");
}

// Função auxiliar para impressao do registro inteiro
void imprimeRegistroRaw (FILE* arq) {
    for (int i = 0; i < TAM_REGISTRO; i++) {
        char c = fgetc(arq);
        printf("%c", c);
    }
    printf("\n\n");
}

// Conta quantos registros há em um determinado arquivo
int contarRegistros (FILE *arq) {
    fseek(arq, 0, SEEK_END);
    long tamFinal = ftell(arq);
    fseek(arq, TAM_PG_DISCO - 1, SEEK_SET);
    long tamInicial = ftell(arq);
    fseek(arq, 0, SEEK_SET);

    return (tamFinal - tamInicial) / TAM_REGISTRO;
}

int calculaRRN(long byteoffset){
    return (byteoffset / TAM_REGISTRO) - 15;
}

int calculaByteoffset (int rrn){
    return ((rrn + 15) * TAM_REGISTRO);
}

int imprimePilha (){
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);
    
    FILE* bin = abreArquivo(nome_arquivo, "rb");

    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    int pos;
    pos = c.topo;
    while (pos != -1){
        printf("%d\n", pos);
        
        fseek(bin, calculaByteoffset(pos) + 1, SEEK_SET);
        fread(&pos, sizeof(int), 1, bin);
    }
    printf("%d\n", pos);

    return SUCESSO;
}

void* alocaMemoria (size_t s) {
    void* ptr = malloc(s);
    if (ptr == NULL) {
        printf("Erro de alocacao de memoria\n");
        exit(1);
    }
    return ptr;
}