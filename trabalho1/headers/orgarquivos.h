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
int inserirCampoFixo (void* r, size_t t, size_t n, FILE* arq);
int inserirCampoVariavel (void* r, FILE* arq);

#endif