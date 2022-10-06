#ifndef __REGISTRO_H__
#define __REGISTRO_H__

#include "cabecalho.h"
#include <stdio.h>

#define TAM_SIGLA 2
#define TAM_STATUS 1

#define TAM_PG_DISCO 960
#define TAM_REGISTRO 64
#define TAM_REGISTRO_FIXO 20
#define TAM_STRING 128

#define LIXO '$'
#define REMOVIDO '*'
#define DELIMITADOR '|'
#define STR_DELIMITADOR "|"

// tamanho fixo de 64 bytes
typedef struct {
    // campos de controle
    // TAM = 5 bytes
    char removido;
    int encadeamento;

    // campos de tamanho fixo
    // TAM = 15 bytes
    int idConecta;
    char* siglaPais;
    int idPoPsConectado;
    char unidadeMedida;
    int velocidade;

    // campos de tamanho variável
    // TAM_MAX = 44 bytes
    char* nomePoPs;
    char* nomePais;
} Registro;


typedef struct {
    Cabecalho cabecalho;
    Registro* registros;
} Arquivo;

int criaRegistro (Registro *r);
int destroiRegistro (Registro *r);

int lerDadosRegistroTeclado(Registro *t);
int insereStringRegistro (char* str, char* str_registro, int tam_campo);
void imprimeRegistro (Registro *r);
int registroRemovido (Registro *r);
void readline(char* string);
int contarRegistros (FILE *arq);

void imprimeRegistroRaw (FILE* arq);

#endif