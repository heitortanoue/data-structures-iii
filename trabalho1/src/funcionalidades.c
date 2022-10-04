#include "funcionalidades.h"
#include "registro.h"
#include "orgarquivos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CREATE TABLE - Cria uma tabela com os campos especificados com base em um arquivo CSV
int funcionalidade1 (void) {
    char nome_arquivo_entrada[128];
    scanf("%s", nome_arquivo_entrada);
    char nome_arquivo_saida[128];
    scanf("%s", nome_arquivo_saida);

    // Abre o arquivo .csv de entrada
    FILE* csv = abreArquivo(nome_arquivo_entrada, "r");

	char cabecalho[128];
	fgets(cabecalho, 128, csv);

    // cria um novo cabeçalho e atualiza com as informações iniciais
    Cabecalho c;
    atualizarCabecalhoPadrao(&c);
	Registro r;

	// Cria o arquivo binário
	FILE* bin = abreArquivo(nome_arquivo_saida, "wb");

	// Escreve o cabeçalho (no inicio do arquivo)
	escreveCabecalhoArquivo(bin, &c);

    // Leitura do arquivo .csv
    char linha[128];
    while (fgets(linha, 128, csv)) {
        lerCsvRegistro(&r, linha);
        imprimeRegistro(&r);
    }

    fclose(csv);
    fclose(bin);
    return SUCESSO;
}