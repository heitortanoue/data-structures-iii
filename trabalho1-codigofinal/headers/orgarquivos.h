#ifndef __ORGARQUIVOS_H__
#define __ORGARQUIVOS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registro.h"

#define SUCESSO 0
#define ERRO -1

FILE* abreArquivo(char *nome_arquivo, char *type);
int inserirRegistroArquivo(FILE *arq, Registro* r);
int lerRegistroArquivo(FILE *arq, Registro* r);
void binarioNaTela(char *nomeArquivoBinario);
int escreveCabecalhoArquivo (FILE* arq, Cabecalho* c);
void scan_quote_string(char *str);

int inserirCampoFixo (void* r, size_t size_dado, size_t num_dado, size_t tam_campo, FILE* arq);
int inserirStringCampoFixo (char* r, size_t tam_campo, FILE* arq);
int inserirCharCampoFixo (char r, size_t tam_campo, FILE* arq);
int inserirCampoVariavel (void* r, FILE* arq);

int lerCampoFixo (FILE *arq, void *campo, int tam_campo, int qtd_campo);
int lerStringCampoFixo (FILE *arq, char *campo, int tam_campo);
int lerStringCampoVariavel (FILE *arq, char *campo);
int escreverLixo(FILE *arq, size_t tam);
int testaRegistro (Registro reg, Busca *filtro, int numFiltro);

#endif