#include "../headers/funcionalidades.h"
#include "../headers/registro.h"
#include "../headers/orgarquivos.h"
#include "../headers/csv.h"
#include "../headers/busca.h"
#include "../headers/removeinsere.h"
#include "../headers/arvoreB.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


// FUNCIONALIDADE 1 - Cria uma tabela com os campos especificados com base em um arquivo .CSV
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

// FUNCIONALIDADE 2 - Imprime todos os registros de uma tabela
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

//FUNCIONALIDADE 3 - Imprime todos os registros que contém o(s) campo(s) especificado(s)
int selectWhere (void){
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    int qtd_filtros;
    scanf("%d", &qtd_filtros);

    Busca filtros;
    criaFiltro(&filtros, qtd_filtros);
    
    //salva os filtros na ordem
    for (int i = 0; i < qtd_filtros; i++){
        scanf("%s", (filtros.campo)[i]);
        getchar();
        scanTeclado((filtros.criterios)[i]); //entrada do teclado removendo aspas
        trataFiltros(&filtros, i); //identifica o campo
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
            if(testaRegistro(r, &filtros, i)){ //se tiver o campo de busca imprime
                encontrou = 1;
                imprimeRegistro(&r);
            }
            regsVisitados++;
        }

        //calcula a quantidade de pag. de discos acessadas
        filtros.pagDisco = calculaNumPagDisco(regsVisitados);
        fseek(bin, TAM_PG_DISCO, SEEK_SET);
        encontrou ? printf("Numero de paginas de disco: %ld\n\n", filtros.pagDisco) : printf("Registro inexistente.\n\nNumero de paginas de disco: %ld\n\n", filtros.pagDisco);
    }

    destroiFiltro(&filtros);
    destroiRegistro(&r);
    fclose(bin);

    return SUCESSO;
}

//FUNCIONALIDADE 4 - Remove todos os registros que possuem o campo especifícado na entrada
int removeRegistro(void){
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    int qtd_filtros;
    scanf("%d", &qtd_filtros);

    Busca filtros;
    criaFiltro(&filtros, qtd_filtros);
    
    //Entrada dos campos e valores a serem excluidos
    for (int i = 0; i < qtd_filtros; i++){
        scanf("%s", (filtros.campo)[i]);
        getchar();
        scanTeclado((filtros.criterios)[i]);
        trataFiltros(&filtros, i); //identifica os campos
    }

    FILE* bin = abreArquivo(nome_arquivo, "rb+");

    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    Registro r;
    criaRegistro(&r);

    //encontra os registros a serem excluidos, salvando seu RRN em regExcluidos
    int *regsExcluidos = (int*) alocaMemoria(sizeof(int) * qtdRegs);
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

    //remove os registros marcados
    for(int i = 0; i<auxi; i++){
        remocaoReg(bin, regsExcluidos[i], &c);
    }

    fseek(bin, 0, SEEK_SET);
    atualizarStatusCabecalho(&c, '1');
    escreveCabecalhoArquivo(bin, &c);

    destroiFiltro(&filtros);
    destroiRegistro(&r);
    fclose(bin);
    free(regsExcluidos);

    binarioNaTela(nome_arquivo);

    return SUCESSO;
}

//FUNCIONALIDADE 5 - Insere um registro novo no arquivo no lugar de um reg. logicamente removido ou no final do arquivo
int insert () {
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    int qtd_filtros;
    scanf("%d", &qtd_filtros);

    FILE* bin = abreArquivo(nome_arquivo, "rb+");

    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    Registro r;
    criaRegistro(&r);    

    //le todos os dados a serem inseridos e insere no topo da pilha (n vezes)
    getchar();
    for (int i = 0; i < qtd_filtros; i++){
        entradaDados(&r);
        insereRegistro(&r, &c, bin);
    }

    //atualiza o cabecalho
    fseek(bin, 0, SEEK_SET);
    atualizarStatusCabecalho(&c, '1');
    atualizarNumPagDiscoCabecalho(&c, c.proxRRN);
    escreveCabecalhoArquivo(bin, &c);

    //encerra a funcao
    destroiRegistro(&r);
    fclose(bin);

    binarioNaTela(nome_arquivo);
    return SUCESSO;
}

//FUNCIONALIDADE 6 - Compacta o arquivo removendo os registros excluidos
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

    //se nao for removido copia no arquivo novoBinario
    int qtdRegValido = 0;
    for (int i = 0; i < qtdRegs; i++){
        if(!lerRegistroArquivo(bin, &r)){
            inserirRegistroArquivo(compacBin, &r);
            qtdRegValido++;
        }
    }

    //atualiza o cabecalho do arquivo
    atualizarStatusCabecalho(&c, '1');
    atualizarNumPagDiscoCabecalho(&c, qtdRegValido);
    c.nroRegRem = 0;
    c.proxRRN = qtdRegValido;
    c.topo = -1;
    c.qttCompacta++;

    fseek(compacBin, 0, SEEK_SET);
    escreveCabecalhoArquivo(compacBin, &c);

    fclose(bin);
    fclose(compacBin);

    //remove o arquivo antigo e renomeia o arquivo novo
    remove(nome_arquivo);
    rename("novoBinario.bin", nome_arquivo);

    binarioNaTela(nome_arquivo);
    return SUCESSO;
}

//FUNCIONALIDADE 7 - Cria um arquivo de indice primario para um arquivo binario
int createIndex () {
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    char nome_indice[128];
    scanf("%s", nome_indice);

    FILE* bin = abreArquivo(nome_arquivo, "rb+");
    FILE* ind = abreArquivo(nome_indice, "wb+");

    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    //cria o cabecalho do arquivo de indice
    CabecalhoIndice ci;
    criaCabecalhoIndice(&ci);

    Registro r;
    criaRegistro(&r);
    Indice indice;
    for (int i = 0; i < qtdRegs; i++){
        lerRegistroArquivo(bin, &r);

        if (registroRemovido(&r)) {
            continue;
        }

        indice.chave = r.idConecta;
        indice.referencia = i;
        //printf("i = %d\n", i);
        
        //insere o registro no arquivo de indice
        printf("i: %d\n", i);
        if (insereChaveArvoreB(&indice, &ci, ind) == SUCESSO) {
            (ci.nroChavesTotal)++;
        }

    }

    //atualiza o cabecalho do arquivo de indice
    ci.status = '1';
    // ...
    escreveCabecalhoIndice(&ci, ind);

    destroiRegistro(&r);
    fclose(bin);
    fclose(ind);

    binarioNaTela(nome_indice);

    return SUCESSO;
}

//FUNCIONALIDADE 8 - Busca um registro no arquivo de indice
int searchIndex () {
    char nome_indice[128];
    scanf("%s", nome_indice);

    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    int id;
    scanf("%d", &id);

    FILE* ind = abreArquivo(nome_indice, "rb+");
    FILE* bin = abreArquivo(nome_arquivo, "rb+");

    CabecalhoIndice ci;
    leCabecalhoIndice(&ci, ind);

    //busca o registro no arquivo de indice
    int status, a = 0;
    No* pos = buscaChaveArvoreB(id, ci.noRaiz, &a, &a, ind, &status);

    if (status == ERRO) {
        printf("Registro inexistente.\n");
        return ERRO;
    }

    int ref = referenciaChaveNo(pos, id);
    int byteOffsetBin = calculaByteoffset(ref); 
    Registro r;
    criaRegistro(&r);

    fseek(bin, byteOffsetBin, SEEK_SET);
    lerRegistroArquivo(bin, &r);
    imprimeRegistro(&r);

    destroiRegistro(&r);
    fclose(ind);
    fclose(bin);

    return SUCESSO;
}