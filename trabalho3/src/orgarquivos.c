#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/busca.h"

// Abre o arquivo de saída, de acordo com o nome passado. Tanto para leitura quanto para escrita.
FILE* abreArquivo(char *nome_arquivo, char *type) {
    FILE *arquivo = fopen(nome_arquivo, type);
    if (arquivo == NULL) {
        printf("Falha na execução da funcionalidade.\n");
        exit(SUCESSO);
    }
    
    char c = fgetc(arquivo);
    if (c == '0') {
        printf("Falha na execução da funcionalidade.\n");
        exit(SUCESSO);
    }
    fseek(arquivo, 0, SEEK_SET);

    return arquivo;
}

//Faz a inserção dos dados lidos a cada registro
int inserirRegistroArquivo(FILE *arq, Registro* r) {
	int tam_inicial = ftell(arq);

	inserirCampoFixo(&r->removido, sizeof(char), 1, sizeof(char), arq);
	inserirCampoFixo(&r->encadeamento, sizeof(int), 1, sizeof(int), arq);

	inserirCampoFixo(&r->idConecta, sizeof(int), 1, sizeof(int), arq);
	inserirStringCampoFixo(r->siglaPais, TAM_SIGLA, arq);

	inserirCampoFixo(&r->idPoPsConectado, sizeof(int), 1, sizeof(int), arq);
	inserirCharCampoFixo(r->unidadeMedida, 1, arq);
	inserirCampoFixo(&r->velocidade, sizeof(int), 1, sizeof(int), arq);

	inserirCampoVariavel(r->nomePoPs, arq);
	inserirCampoVariavel(r->nomePais, arq);

	// preencher o resto do registro com lixo
	int tam_final = ftell(arq);
	int tam = tam_final - tam_inicial;
	int tam_lixo = TAM_REGISTRO - tam;
	escreverLixo(arq, tam_lixo);

	return SUCESSO;
}

// Lê um determinado registro do arquivo correspondente
int lerRegistroArquivo(FILE *arq, Registro* r){
	lerCampoFixo(arq, &r->removido, sizeof(char), 1);
	if (!registroRemovido(r)){
		lerCampoFixo(arq, &r->encadeamento, sizeof(int), 1);

		lerCampoFixo(arq, &r->idConecta, sizeof(int), 1);
		
		lerStringCampoFixo(arq, r->siglaPais, TAM_SIGLA);
		lerCampoFixo(arq, &r->idPoPsConectado, sizeof(int), 1);
		lerCampoFixo(arq, &r->unidadeMedida, sizeof(char), 1);
		lerCampoFixo(arq, &r->velocidade, sizeof(int), 1);

		lerStringCampoVariavel(arq, r->nomePoPs);
		lerStringCampoVariavel(arq, r->nomePais);

		// pular o restante de lixo
		int tam_campo_variavel = strlen(r->nomePoPs) + strlen(r->nomePais) + 2;
		fseek(arq, TAM_REGISTRO - TAM_REGISTRO_FIXO - tam_campo_variavel, SEEK_CUR);
		return SUCESSO;
	} else {
		fseek(arq, TAM_REGISTRO - 1, SEEK_CUR);
		return 1;
	}
}

// Leitura de um campo de tamanho fixo de tamanho 'tam_campo * qtd_campo' bytes
int lerCampoFixo (FILE *arq, void *campo, int tam_campo, int qtd_campo) {
	fread(campo, tam_campo, qtd_campo, arq);
	return SUCESSO;
}

// Leitura de uma string em um campo de tamanho fixo com 'tam_campo' bytes
int lerStringCampoFixo (FILE *arq, char *campo, int tam_campo) {
	char *aux = (char*) alocaMemoria(sizeof(char) * (tam_campo + 1));
	lerCampoFixo(arq, aux, sizeof(char), tam_campo);

	int i = 0;
	// Faz a leitura do campo até encontrar lixo ou até quando o campo terminar
	while (aux[i] != LIXO && i < tam_campo) {
		campo[i] = aux[i];
		i++;
	}
	campo[i] = '\0';

	free(aux);
	return SUCESSO;
}

// Leitura de uma string em um campo de tamanho variável
int lerStringCampoVariavel (FILE *arq, char *campo) {
	char c;
	int i = 0;
	// Faz a leitura do campo até encontrar o delimitador
	while ((c = fgetc(arq)) != DELIMITADOR && c != EOF) {
		campo[i] = c;
		i++;
	}
	campo[i] = '\0';
	
	return SUCESSO;
}

// Insere um campo de tamanho fixo no arquivo
int inserirCampoFixo (void* r, size_t size_dado, size_t num_dado, size_t tam_campo, FILE* arq) {
	// insere os dados
	fwrite(r, size_dado, num_dado, arq);

	// insere o restante com lixo
	int tam_lixo = tam_campo - (size_dado * num_dado);
	if (tam_lixo <= 0) return SUCESSO;

	escreverLixo(arq, tam_lixo);

	return SUCESSO;
}

// Insere uma string em um campo de tamanho fixo no arquivo
int inserirStringCampoFixo (char* r, size_t tam_campo, FILE* arq) {
	// insere os dados
	if (!stringValida(r)) {
		escreverLixo(arq, tam_campo);
		return SUCESSO;
	}

	int tam = strlen(r);
	fwrite(r, sizeof(char), tam, arq);

	// insere o restante com lixo
	int tam_lixo = tam_campo - tam;
	if (tam_lixo <= 0) {
		return SUCESSO;
	}

	escreverLixo(arq, tam_lixo);

	return SUCESSO;
}

// Insere um char em um campo de tamanho fixo no arquivo
int inserirCharCampoFixo (char r, size_t tam_campo, FILE* arq) {
	if (!charValido(r)) {
		escreverLixo(arq, tam_campo);
		return SUCESSO;
	}

	// insere os dados
	fwrite(&r, sizeof(char), 1, arq);

	// insere o restante com lixo
	int tam_lixo = tam_campo - 1;
	if (tam_lixo <= 0) {
		return SUCESSO;
	}

	escreverLixo(arq, tam_lixo);

	return SUCESSO;
}

// Insere um campo de tamanho variável no arquivo
int inserirCampoVariavel (void* r, FILE* arq) {
	// nao coloca o /0 no final
	if (stringValida(r)) {
		int tam = strlen(r);
		fwrite(r, sizeof(char), tam, arq);
	}

	fwrite(STR_DELIMITADOR, sizeof(char), 1, arq);
	return SUCESSO;
}

// Escreve cabecalho 'c' no arquivo 'arq'
int escreveCabecalhoArquivo (FILE* arq, Cabecalho* c) {
	inserirCampoFixo(&c->status, sizeof(char), 1, sizeof(char), arq);
	inserirCampoFixo(&c->topo, sizeof(int), 1, sizeof(int), arq);
	inserirCampoFixo(&c->proxRRN, sizeof(int), 1, sizeof(int), arq);
	inserirCampoFixo(&c->nroRegRem, sizeof(int), 1, sizeof(int), arq);
	inserirCampoFixo(&c->nroPagDisco, sizeof(int), 1, sizeof(int), arq);
	inserirCampoFixo(&c->qttCompacta, sizeof(int), 1, sizeof(int), arq);

	// Preenche o resto da página com lixo
	escreverLixo(arq, TAM_PG_DISCO - TAM_CABECALHO);

	return SUCESSO;
}

// Escreve lixo no arquivo 'arq' até completar 'tam' bytes
int escreverLixo(FILE *arq, size_t tam) {
	char strLixo[tam + 1];
	for (int i = 0; i < tam; i++) {
		strLixo[i] = LIXO;
	}
	strLixo[tam] = '\0';
	fwrite(strLixo, sizeof(char), tam, arq);

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

void scan_quote_string(char *str) {

	/*
	*	Use essa função para ler um campo string delimitado entre aspas (").
	*	Chame ela na hora que for ler tal campo. Por exemplo:
	*
	*	A entrada está da seguinte forma:
	*		nomeDoCampo "MARIA DA SILVA"
	*
	*	Para ler isso para as strings já alocadas str1 e str2 do seu programa, você faz:
	*		scanf("%s", str1); // Vai salvar nomeDoCampo em str1
	*		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2 (sem as aspas)
	*
	*/

	char R;

	while((R = getchar()) != EOF && isspace(R)); // ignorar espaços, \r, \n...

	if(R == 'N' || R == 'n') { // campo NULO
		getchar(); getchar(); getchar(); // ignorar o "ULO" de NULO.
		strcpy(str, ""); // copia string vazia
	} else if(R == '\"') {
		if(scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
			strcpy(str, "");
		}
		getchar(); // ignorar aspas fechando
	} else if(R != EOF){ // vc tá tentando ler uma string que não tá entre aspas! Fazer leitura normal %s então, pois deve ser algum inteiro ou algo assim...
		str[0] = R;
		scanf("%s", &str[1]);
	} else { // EOF
		strcpy(str, "");
	}
}