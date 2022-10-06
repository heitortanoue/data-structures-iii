#include "funcionalidades.h"
#include "registro.h"
#include "orgarquivos.h"
#include "csv.h"
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
    criaRegistro(&r);

	// Cria o arquivo binário
	FILE* bin = abreArquivo(nome_arquivo_saida, "wb");

	// Escreve o cabeçalho (no inicio do arquivo)
	escreveCabecalhoArquivo(bin, &c);

    // Leitura do arquivo .csv
    char linha[128];
    int numRegistros = 0;
    while (fgets(linha, 128, csv)) {
        lerCsvRegistro(&r, linha);
        inserirRegistroArquivo(bin, &r);
        numRegistros++;
    }

    // Atualiza o cabeçalho
    atualizarStatusCabecalho(&c, '1');
    atualizarNumPagDiscoCabecalho(&c, numRegistros);
    // Volta o cursor para o começo do arquivo
    fseek(bin, 0, SEEK_SET);
    escreveCabecalhoArquivo(bin, &c);

    destroiRegistro(&r);
    fclose(csv);
    fclose(bin);

    binarioNaTela(nome_arquivo_saida);

    return SUCESSO;
}

// SELECT FROM - Imprime todos os registros de uma tabela
int funcionalidade2 () {
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    FILE* bin = abreArquivo(nome_arquivo, "rb");

    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    Registro r;
    criaRegistro(&r);

    for (int i = 0; i < qtdRegs; i++) {
        // lerRegistroArquivo(bin, &r);
        // imprimeRegistro(&r);
        imprimeRegistroRaw(bin);
    }

    destroiRegistro(&r);
    fclose(bin);

    return SUCESSO;
}