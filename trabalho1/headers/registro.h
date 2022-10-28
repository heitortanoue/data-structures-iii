#ifndef __REGISTRO_H__
#define __REGISTRO_H__

#include "cabecalho.h"
#include <stdio.h>

// Quantidade de bytes do campo 'siglaPais'
#define TAM_SIGLA 2

// Quantidade de bytes do registro de dados
#define TAM_PG_DISCO 960
// Quantidade de bytes do registro de dados
#define TAM_REGISTRO 64
// Quantidade de bytes da parte de tamanho fixo do registro de dados
#define TAM_REGISTRO_FIXO 20
// Quantidade de bytes a serem alocados para manipulacao de strings
#define TAM_STRING 128

// Caractere padrao para preencher bytes vazios do registro
#define LIXO '$'
// Caracteres padrao para remocao logica
#define REMOVIDO '1'
#define STR_REMOVIDO "1"
#define NAO_REMOVIDO '0'
// Caractere delimitador de campos de tamanho variavel
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

typedef struct{
    char **campo;
    char **criterios;
    int qtdFiltros;
    int *tipo_campo;
    long pagDisco; //pag de disco acessadas

} Busca;

int criaRegistro (Registro *r);
int destroiRegistro (Registro *r);

int lerDadosRegistroTeclado(Registro *t);
int insereStringRegistro (char* str, char* str_registro, int tam_campo);
void imprimeRegistro (Registro *r);
int registroRemovido (Registro *r);
void readline(char* string);

int contarRegistros (FILE *arq);
int calculaRRN(long byteoffset);
int calculaByteoffset (int rrn);

void imprimeRegistroRaw (FILE* arq);
int imprimePilha ();
void* alocaMemoria (size_t s);

#endif