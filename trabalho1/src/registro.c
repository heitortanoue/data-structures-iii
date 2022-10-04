#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"
#include "orgarquivos.h"
#define LIXO '$'

//Chamada da leitura dos campos a cada registro
int lerDadosRegistroTeclado(Registro *t) {
    char str[128];
    t->removido = 0;
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

int registroRemovido (Registro *r) {
    return r->removido == 0;
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

int atualizarCabecalhoPadrao (Cabecalho *c ) {
	c->status = 0;
    c->topo = -1;
    c->proxRRN = 0;
    c->nroRegRem = 0;
    c->nroPagDisco = 0;
    c->qttCompacta = 0;

    return SUCESSO;
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

int lerCsvRegistro (Registro *r, char *linha) {
    char *temp = malloc(sizeof(char) * 64);

    meuStrtok(temp, linha, ',');
    r->idConecta = validaInt(temp);

    meuStrtok(temp, linha, ',');
    strcpy(r->nomePoPs, validaString(temp));

    meuStrtok(temp, linha, ',');
    strcpy(r->nomePais, validaString(temp));

    meuStrtok(temp, linha, ',');
    strcpy(r->siglaPais, validaString(temp));

    meuStrtok(temp, linha, ',');
    r->idPoPsConectado = validaInt(temp);

    meuStrtok(temp, linha, ',');
    r->unidadeMedida = validaChar(temp);

    meuStrtok(temp, linha, ',');
    r->velocidade = validaInt(temp);

    free(temp);
    return SUCESSO;
}

void meuStrtok (char* pedaco, char *str, char delim) {
    int i = 0;
    while (str[i] != delim && str[i] != '\0') {
        pedaco[i] = str[i];
        i++;
    }
    pedaco[i] = '\0';

    if (str[i] == delim) {
        i++;
    }

    int j = 0;
    while (str[i] != '\0') {
        str[j] = str[i];
        i++;
        j++;
    }
    str[j] = '\0';
}

char* validaString (char* str) {
    return str && str[0] != '\0' ? str : "\0";
}
int validaInt (char* i) {
    return i ? atoi(i) : -1;
}
char validaChar (char* c) {
    return c && c[0] != '\0' ? c[0] : '\0';
}

int stringValida (char* str) {
    return str && str[0] != '\0';
}
int intValido (int i) {
    return i != -1;
}
int charValido (char c) {
    return c != '\0';
}