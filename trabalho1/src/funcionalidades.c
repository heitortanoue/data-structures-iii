#include "../headers/funcionalidades.h"
#include "../headers/registro.h"
#include "../headers/orgarquivos.h"
#include "../headers/csv.h"
#include "../headers/busca.h"
#include "../headers/removeinsere.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// CREATE TABLE - Cria uma tabela com os campos especificados com base em um arquivo CSV
int createTable (void) {
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
        c.proxRRN++;
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
int selectFrom () {
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    FILE* bin = abreArquivo(nome_arquivo, "rb");

    int qtdRegs = contarRegistros(bin);

    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    Registro r;
    criaRegistro(&r);

    if (!qtdRegs) {
        printf("Registro inexistente.\n\n");
        printf("Numero de paginas de disco: %d\n\n", c.nroPagDisco);
        return ERRO;
    }

    for (int i = 0; i < qtdRegs; i++) {
        if(!lerRegistroArquivo(bin, &r)){
            imprimeRegistro(&r);
        } else {
            continue;
        }
    }

    printf("Numero de paginas de disco: %d\n\n", c.nroPagDisco);
    
    destroiRegistro(&r);
    fclose(bin);

    return SUCESSO;
}

//SELECT WHERE - imprime todos os registros que contém o(s) campo(s) especificado(s)
int selectWhere (void){
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    int qtd_filtros;
    scanf("%d", &qtd_filtros);

    Busca filtros;
    criaFiltro(&filtros, qtd_filtros);
    
    for (int i = 0; i < qtd_filtros; i++){
        scanf("%s", (filtros.campo)[i]);
        getchar();
        scanTeclado((filtros.criterios)[i]);
        trataFiltros(&filtros, i);
    }

    FILE* bin = abreArquivo(nome_arquivo, "rb");

    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    Registro r;
    criaRegistro(&r);

    int regsVisitados = 0;

    for (int i = 0; i < qtd_filtros; i++){
        regsVisitados = 1;
        int encontrou = 0;
        printf("Busca %d\n", i + 1);

        for (int j = 0; j < qtdRegs; j++) {
            lerRegistroArquivo(bin, &r);
            if(testaRegistro(r, &filtros, i)){
                encontrou = 1;
                imprimeRegistro(&r);
            }
            regsVisitados++;
        }

        filtros.pagDisco = calculaNumPagDisco(regsVisitados);
        fseek(bin, TAM_PG_DISCO, SEEK_SET);
        encontrou ? printf("Numero de paginas de disco: %ld\n\n", filtros.pagDisco) : printf("Registro inexistente.\n\nNumero de paginas de disco: %ld\n\n", filtros.pagDisco);
    }

    destroiFiltro(&filtros);
    destroiRegistro(&r);
    fclose(bin);

    return SUCESSO;
}

//Remove todos os registros que possuem o campo especifícado na entrada
int removeRegistro(void){
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    int qtd_filtros;
    scanf("%d", &qtd_filtros);

    Busca filtros;
    criaFiltro(&filtros, qtd_filtros);
    
    for (int i = 0; i < qtd_filtros; i++){
        scanf("%s", (filtros.campo)[i]);
        getchar();
        scanTeclado((filtros.criterios)[i]);
        trataFiltros(&filtros, i);
    }

    FILE* bin = abreArquivo(nome_arquivo, "rb+");

    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    Registro r;
    criaRegistro(&r);

    int *regsExcluidos = malloc(sizeof(int) * qtdRegs);
    int auxi = 0;
    int posAtual;
    for (int i = 0; i < qtd_filtros; i++){
        for (int j = 0; j < qtdRegs; j++) {
            posAtual = ftell(bin);
            lerRegistroArquivo(bin, &r);

            if(testaRegistro(r, &filtros, i)){
                regsExcluidos[auxi] = calculaRRN(posAtual);
                auxi++;
            }
        }
        fseek(bin, TAM_PG_DISCO, SEEK_SET);
    }

    for(int i = 0; i<auxi; i++){
        remocaoReg(bin, regsExcluidos[i], &c);
    }

    fseek(bin, 0, SEEK_SET);
    atualizarStatusCabecalho(&c, '1');
    escreveCabecalhoArquivo(bin, &c);
    // imprimeCabecalho(&c);

    destroiFiltro(&filtros);
    destroiRegistro(&r);
    fclose(bin);
    free(regsExcluidos);

    binarioNaTela(nome_arquivo);

    return SUCESSO;
}

//Insere um registro novo no arquivo no lugar de um reg. logicamente removido ou no final do arquivo
int insert () {
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    int qtd_filtros;
    scanf("%d", &qtd_filtros);

    FILE* bin = abreArquivo(nome_arquivo, "rb+");

    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);
    // imprimeCabecalho(&c);

    Registro r;
    criaRegistro(&r);    

    getchar();
    for (int i = 0; i < qtd_filtros; i++){
        entradaDados(&r);
        insereRegistro(&r, &c, bin);
    }

    fseek(bin, 0, SEEK_SET);
    atualizarStatusCabecalho(&c, '1');
    atualizarNumPagDiscoCabecalho(&c, c.proxRRN);
    escreveCabecalhoArquivo(bin, &c);
    // imprimeCabecalho(&c);
    destroiRegistro(&r);
    fclose(bin);

    binarioNaTela(nome_arquivo);
    return SUCESSO;
}

//Compacta o arquivo removendo os registros excluidos
int compact(){
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    FILE* bin = abreArquivo(nome_arquivo, "rb+");
    FILE* compacBin = abreArquivo("novoBinario.bin", "wb");

    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);
    escreveCabecalhoArquivo(compacBin, &c);

    Registro r;
    criaRegistro(&r);

    int qtdRegValido = 0;
    for (int i = 0; i < qtdRegs; i++){
        if(!lerRegistroArquivo(bin, &r)){
            inserirRegistroArquivo(compacBin, &r);
            qtdRegValido++;
        }
    }

    atualizarStatusCabecalho(&c, '1');
    atualizarNumPagDiscoCabecalho(&c, qtdRegValido);
    c.nroRegRem = 0;
    c.proxRRN = qtdRegValido;
    c.topo = -1;
    c.qttCompacta++;

    fseek(compacBin, 0, SEEK_SET);
    escreveCabecalhoArquivo(compacBin, &c);

    // imprimeCabecalho(&c);
    fclose(bin);
    fclose(compacBin);

    remove(nome_arquivo);
    rename("novoBinario.bin", nome_arquivo);

    binarioNaTela(nome_arquivo);
    return SUCESSO;
}