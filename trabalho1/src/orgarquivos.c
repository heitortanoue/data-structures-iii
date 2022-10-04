#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "orgarquivos.h"
#include "registro.h"

// Abre o arquivo de saída, de acordo com o nome passado. Tanto para leitura quanto para escrita.
FILE* abreArquivo(char *nome_arquivo, char *type) {
    FILE *arquivo = fopen(nome_arquivo, type);
    if (arquivo == NULL) {
        printf("Falha no processamento do arquivo\n");
        exit(SUCESSO);
    }
    return arquivo;
}

//Faz a inserção dos dados lidos a cada registro
int inserirRegistroArquivo(FILE *arq, Registro* r) {
	inserirCampoFixo(&r->removido, sizeof(char), 1, arq);
	inserirCampoFixo(&r->encadeamento, sizeof(int), 1, arq);

	inserirCampoFixo(&r->idConecta, sizeof(int), 1, arq);
	inserirCampoFixo(r->siglaPais, sizeof(char), TAM_SIGLA, arq);
	inserirCampoFixo(&r->idPoPsConectado, sizeof(int), 1, arq);
	inserirCampoFixo(&r->unidadeMedida, sizeof(char), 1, arq);
	inserirCampoFixo(&r->velocidade, sizeof(int), 1, arq);

	inserirCampoVariavel(r->nomePoPs, arq);
	inserirCampoVariavel(r->nomePais, arq);

	return SUCESSO;
}

// Lê um determinado registro do arquivo correspondente
int lerRegistroArquivo(FILE *arq, Registro* r){
	fread(&r->removido, sizeof(char), 1, arq);
	fread(&r->encadeamento, sizeof(int), 1, arq);

	fread(&r->idConecta, sizeof(int), 1, arq);
	fread(&r->siglaPais, sizeof(char), TAM_SIGLA, arq);
	fread(&r->idPoPsConectado, sizeof(int), 1, arq);
	fread(&r->unidadeMedida, sizeof(char), 1, arq);
	fread(&r->velocidade, sizeof(int), 1, arq);
	
	fread(&r->nomePoPs, sizeof(char), strlen(r->nomePoPs), arq);
	fread(&r->nomePais, sizeof(char), strlen(r->nomePais), arq);

	return SUCESSO;
}

// Insere um campo de tamanho fixo no arquivo
int inserirCampoFixo (void* r, size_t t, size_t n, FILE* arq) {
	fwrite(r, t, n, arq);
	return SUCESSO;
}

// Insere um campo de tamanho variável no arquivo
int inserirCampoVariavel (void* r, FILE* arq) {
	int tam = strlen(r);
	fwrite(r, sizeof(char), tam, arq);
	fwrite("|", sizeof(char), 1, arq);

	return SUCESSO;
}

int escreveCabecalhoArquivo (FILE* arq, Cabecalho* c) {
	inserirCampoFixo(&c->status, sizeof(char), 1, arq);
	inserirCampoFixo(&c->topo, sizeof(int), 1, arq);
	inserirCampoFixo(&c->proxRRN, sizeof(int), 1, arq);
	inserirCampoFixo(&c->nroRegRem, sizeof(int), 1, arq);
	inserirCampoFixo(&c->nroPagDisco, sizeof(int), 1, arq);
	inserirCampoFixo(&c->qttCompacta, sizeof(int), 1, arq);

	// Preenche o resto da página com lixo
	fprintf(arq, "%*c", TAM_PG_DISCO - TAM_CABECALHO, '$');

	return SUCESSO;
}

void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o código dessa função. */

	/* Use essa função para comparação no run.codes. Lembre-se de ter fechado (fclose) o arquivo anteriormente.
	*  Ela vai abrir de novo para leitura e depois fechar (você não vai perder pontos por isso se usar ela). */

	unsigned long i, cs;
	unsigned char *mb;
	size_t fl;
	FILE *fs;
	if(nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
		fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): não foi possível abrir o arquivo que me passou para leitura. Ele existe e você tá passando o nome certo? Você lembrou de fechar ele com fclose depois de usar?\n");
		return;
	}
	fseek(fs, 0, SEEK_END);
	fl = ftell(fs);
	fseek(fs, 0, SEEK_SET);
	mb = (unsigned char *) malloc(fl);
	fread(mb, 1, fl, fs);

	cs = 0;
	for(i = 0; i < fl; i++) {
		cs += (unsigned long) mb[i];
	}
	printf("%lf\n", (cs / (double) 100));
	free(mb);
	fclose(fs);
}