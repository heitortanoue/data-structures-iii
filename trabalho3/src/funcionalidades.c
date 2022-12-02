#include "../headers/funcionalidades.h"
#include "../headers/registro.h"
#include "../headers/orgarquivos.h"
#include "../headers/csv.h"
#include "../headers/busca.h"
#include "../headers/removeinsere.h"
#include "../headers/arvoreB.h"
#include "../headers/grafos.h"
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

    FILE* bin = abreArquivo(nome_arquivo, "rb");
    FILE* ind = abreArquivo(nome_indice, "wb+");

    // conta o num de registros do arquivo binario
    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    //cria o cabecalho do arquivo de indice
    CabecalhoIndice ci;
    criaCabecalhoIndice(&ci);
    escreveCabecalhoIndice(&ci, ind);

    Registro r;
    criaRegistro(&r);
    Indice indice;

    // para cada registro:
    for (int i = 0; i < qtdRegs; i++){
        // para para o Registro r
        lerRegistroArquivo(bin, &r);

        // nao insere caso esteja removido
        if (registroRemovido(&r)) {
            continue;
        }

        // chave primaria é o campo idConecta
        indice.chave = r.idConecta;
        indice.referencia = i;
        
        //insere o registro no arquivo de indice
        if (insereChaveArvoreB(&indice, &ci, ind) == SUCESSO) {
            (ci.nroChavesTotal)++;
        }

    }

    //atualiza o cabecalho do arquivo de indice
    ci.status = '1';
    No* raiz = leNo(ci.noRaiz, ind);
    ci.alturaArvore = raiz->alturaNo;
    escreveCabecalhoIndice(&ci, ind);

    // desaloca memoria
    destroiRegistro(&r);
    fclose(bin);
    fclose(ind);
    destroiNo(raiz);

    binarioNaTela(nome_indice);

    return SUCESSO;
}

//FUNCIONALIDADE 8 - Busca um registro no arquivo de indice
int searchIndex () {
    char nome_arquivo_bin[128];
    scanf("%s", nome_arquivo_bin);
    char nome_arquivo_indice[128];
    scanf("%s", nome_arquivo_indice);

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

    FILE* bin = abreArquivo(nome_arquivo_bin, "rb");
    FILE* ind = abreArquivo(nome_arquivo_indice, "rb");

    int qtdRegs = contarRegistros(bin);
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);
    CabecalhoIndice ci;
    leCabecalhoIndice(&ci, ind);

    Registro r;
    criaRegistro(&r);

    int regsVisitados = 0;

    for (int i = 0; i < qtd_filtros; i++){
        regsVisitados = 1;
        int encontrou = 0;
        printf("Busca %d\n", i + 1);

        // caso o campo a ser buscado seja o idConecta (chave primaria), usa o arquivo de indice
        if (filtros.tipo_campo[i] == 0) {
            // valida a chave passada
            int chave_valida = validaInt(filtros.criterios[i]);
            if (chave_valida == -1) {
                printf("Registro inexistente.\n");
                continue;
            }
            int pgs_acessadas = 0;
            // faz a busca na arvoreB pela chave
            No* no = buscaChaveArvoreB(chave_valida, ci.noRaiz, ind, &encontrou, &pgs_acessadas);      
            if (encontrou == SUCESSO) {
                int RRN_registro = referenciaChaveNo(no, chave_valida);
                fseek(bin, calculaByteoffset(RRN_registro), SEEK_SET);
                lerRegistroArquivo(bin, &r);
                imprimeRegistro(&r);
            }
            // nro de paginas acessadas + 1 (cabecalho de dados) + 1 (cabecalho do indice) + 1 (arquivo de dados)
            filtros.pagDisco = pgs_acessadas + 3;

            destroiNo(no);
        } else {
            // pesquisa normal
            for (int j = 0; j < qtdRegs; j++) {
                lerRegistroArquivo(bin, &r);
                if(testaRegistro(r, &filtros, i)){ //se tiver o campo de busca imprime
                    encontrou = SUCESSO;
                    imprimeRegistro(&r);
                }
                regsVisitados++;
            }
            //calcula a quantidade de pag. de discos acessadas
            filtros.pagDisco = calculaNumPagDisco(regsVisitados);
        }
        fseek(bin, TAM_PG_DISCO, SEEK_SET);

        if(encontrou == SUCESSO) { 
            printf("Numero de paginas de disco: %ld\n\n", filtros.pagDisco);
        } else {
            printf("Registro inexistente.\n\nNumero de paginas de disco: %ld\n\n", filtros.pagDisco);
        }
    }

    destroiFiltro(&filtros);
    destroiRegistro(&r);
    fclose(bin);
    fclose(ind);

    return SUCESSO;
}

//FUNCIONALIDADE 9 - Insere um registro colocando sua correspondencia no arquivo de indices
int insertWithIndex () {
    char nome_arquivo_bin[128];
    scanf("%s", nome_arquivo_bin);

    char nome_arquivo_indice[128];
    scanf("%s", nome_arquivo_indice);

    int qnt_insercoes;
    scanf("%d", &qnt_insercoes);

    // abre os arquivos
    FILE* bin = abreArquivo(nome_arquivo_bin, "rb+");
    FILE* ind = abreArquivo(nome_arquivo_indice, "rb+");

    // le os cabecalhos
    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);
    CabecalhoIndice ci;
    leCabecalhoIndice(&ci, ind);

    // coloca os status iniciais '0'
    c.status = '0';
    ci.status = '0';
    fseek(bin, 0, SEEK_SET);
    fseek(ind, 0, SEEK_SET);
    escreveCabecalhoArquivo(bin, &c);
    escreveCabecalhoIndice(&ci, ind);

    Registro r;
    criaRegistro(&r);
    Indice indice;

    getchar();
    for (int i = 0; i < qnt_insercoes; i++){
        entradaDados(&r);
        // insere registro no arquivo de dados
        indice.referencia = insereRegistro(&r, &c, bin);

        indice.chave = r.idConecta;
        // insere no na arvoreb, campo de busca é o idConecta
        insereChaveArvoreB(&indice, &ci, ind);
    }

    // atualiza os cabecalhos dos arquivos
    fseek(bin, 0, SEEK_SET);
    atualizarStatusCabecalho(&c, '1');
    atualizarNumPagDiscoCabecalho(&c, c.proxRRN);
    escreveCabecalhoArquivo(bin, &c);

    fseek(ind, 0, SEEK_SET);
    ci.status = '1';
    ci.nroChavesTotal += qnt_insercoes;
    escreveCabecalhoIndice(&ci, ind);

    // desaloca memoria
    destroiRegistro(&r);
    fclose(bin);
    fclose(ind);

    binarioNaTela(nome_arquivo_bin);
    binarioNaTela(nome_arquivo_indice);

    return SUCESSO;
}

// FUNCIONALIDADE 10 - Junta dois arquivos de dados pelos seus campos
int joinOn () {
    char nome_arq_dados_1[128];
    scanf("%s", nome_arq_dados_1);

    char nome_arq_dados_2[128];
    scanf("%s", nome_arq_dados_2);

    char nome_campo_arquivo_1[128];
    scanf("%s", nome_campo_arquivo_1);

    char nome_campo_arquivo_2[128];
    scanf("%s", nome_campo_arquivo_2);

    char nome_arq_indice[128];
    scanf("%s", nome_arq_indice);

    FILE* bin1 = abreArquivo(nome_arq_dados_1, "rb");
    FILE* bin2 = abreArquivo(nome_arq_dados_2, "rb");
    FILE* ind = abreArquivo(nome_arq_indice, "rb");

    int qtdRegs1 = contarRegistros(bin1);

    Cabecalho c1;
    lerCabecalhoArquivo(bin1, &c1);
    Cabecalho c2;
    lerCabecalhoArquivo(bin2, &c2);
    CabecalhoIndice ci;
    leCabecalhoIndice(&ci, ind);

    Registro r1, r2;
    criaRegistro(&r1);
    criaRegistro(&r2);

    // para cada registro no bin1, busca no bin2 pelo correspondente através do campo idConecta com o arquivo de indice (ind)
    // bin1.idPoPsConectado = bin2.idConecta
    int correspondencias = 0;
    for (int i = 0; i < qtdRegs1; i++) {
        lerRegistroArquivo(bin1, &r1);
        int chave_valida = r1.idPoPsConectado;
        int pgs_acessadas = 0;
        int encontrou = 0;

        No* no = buscaChaveArvoreB(chave_valida, ci.noRaiz, ind, &encontrou, &pgs_acessadas);      
        if (encontrou == SUCESSO) {
            int RRN_registro = referenciaChaveNo(no, chave_valida);
            fseek(bin2, calculaByteoffset(RRN_registro), SEEK_SET);
            lerRegistroArquivo(bin2, &r2);
            imprimeDoisRegistros(&r1, &r2);

            correspondencias++;
        }

        destroiNo(no);
    }

    // caso nao encontre nenhum
    if (!correspondencias) {
        printf("Registro inexistente.\n");
    }

    fclose(bin1);
    fclose(bin2);
    fclose(ind);
    destroiRegistro(&r1);
    destroiRegistro(&r2);

    return SUCESSO;
}

// FUNCIONALIDADE 11 - Gera um grafo a partir de um arquivo de dados
int createGraph () {
    char nome_arquivo[128];
    scanf("%s", nome_arquivo);

    FILE* bin = abreArquivo(nome_arquivo, "rb");

    int qtdRegs = contarRegistros(bin);

    Cabecalho c;
    lerCabecalhoArquivo(bin, &c);

    Registro r, r_aux;
    criaRegistro(&r);
    criaRegistro(&r_aux);

    if (!qtdRegs) {
        printf("Registro inexistente.\n\n");
        printf("Numero de paginas de disco: %d\n\n", c.nroPagDisco);
        return ERRO;
    }

    Grafo *g = alocaGrafo();

    for (int i = 0; i < qtdRegs; i++) {
        //Vertice *v = alocaVertice();

        // fseek(bin, calculaByteoffset(i), SEEK_SET);
        if(!lerRegistroArquivo(bin, &r)){
            int ja_estava = 1;
            Vertice *v = procuraIdVertice(g, r.idConecta);

            // se nao existe o vertice, cria
            if (v == NULL) {
                v = alocaVertice();
                copiaRegistroParaVertice(&r, v);
                ja_estava = 0;
            }

            Aresta* a = alocaAresta();
            copiaRegistroParaAresta(&r, a);
            adicionaArestaVertice(v, a);

            if (!ja_estava) {
                adicionaVerticeGrafo(g, v);
            }
        }
    }

    imprimeGrafo(g);
    
    destroiRegistro(&r);
    destroiRegistro(&r_aux);
    destroiGrafo(g);
    fclose(bin);

    return SUCESSO;
}