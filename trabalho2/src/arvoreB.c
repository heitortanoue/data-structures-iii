#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/arvoreB.h"
#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/busca.h"
#include "../headers/removeinsere.h"

No* criaNo () {
    No *novo = (No*) malloc(sizeof(No));
    novo->dados = (Indice*) malloc(sizeof(Indice) * (ORDEM - 1));
    novo->descendentes = (int*) malloc(sizeof(int) * ORDEM);

    for (int i = 0; i < ORDEM - 1; i++) {
        novo->dados[i].chave = -1;
        novo->dados[i].referencia = -1;
    }
    for (int i = 0; i < ORDEM; i++) {
        novo->descendentes[i] = -1;
    }

    novo->folha = '1';
    novo->nroChavesNo = 0;
    novo->alturaNo = 1;
    novo->RRNdoNo = -1;

    return novo;
}

void destroiNo (No* no) {
    free(no->dados);
    free(no->descendentes);
    free(no);
}

int calculaByteOffsetArvoreB (int RRN) {
    return (TAM_PG_DISCO_INDICE + (RRN * TAM_NO));
}

void escreveNo (No *no, FILE *arquivo) {
    if(no != NULL && no->RRNdoNo >= 0 && (no->folha == '1' || no->folha == '0')){
        fseek(arquivo, calculaByteOffsetArvoreB(no->RRNdoNo), SEEK_SET);
        fwrite(&no->folha, sizeof(char), 1, arquivo);
        fwrite(&no->nroChavesNo, sizeof(int), 1, arquivo);
        fwrite(&no->alturaNo, sizeof(int), 1, arquivo);
        fwrite(&no->RRNdoNo, sizeof(int), 1, arquivo);

        for (int i = 0; i < ORDEM - 1; i++) {
            fwrite(&no->descendentes[i], sizeof(int), 1, arquivo);
            fwrite(&no->dados[i].chave, sizeof(int), 1, arquivo);
            fwrite(&no->dados[i].referencia, sizeof(int), 1, arquivo);
        }
        fwrite(&no->descendentes[ORDEM - 1], sizeof(int), 1, arquivo);
    }
}

No* leNo (int RRN, FILE *arquivo) {
    No *novo = criaNo();

    fseek(arquivo, calculaByteOffsetArvoreB(RRN), SEEK_SET);
    fread(&novo->folha, sizeof(char), 1, arquivo);
    fread(&novo->nroChavesNo, sizeof(int), 1, arquivo);
    fread(&novo->alturaNo, sizeof(int), 1, arquivo);
    fread(&novo->RRNdoNo, sizeof(int), 1, arquivo);

    for (int i = 0; i < ORDEM - 1; i++) {
        fread(&novo->descendentes[i], sizeof(int), 1, arquivo);
        fread(&novo->dados[i].chave, sizeof(int), 1, arquivo);
        fread(&novo->dados[i].referencia, sizeof(int), 1, arquivo);
    }
    fread(&novo->descendentes[ORDEM - 1], sizeof(int), 1, arquivo);
    // imprimeNo(novo);

    return novo;
}

void criaCabecalhoIndice (CabecalhoIndice* cabecalho) {
    cabecalho->status = '0';
    cabecalho->noRaiz = -1;
    cabecalho->RRNproxNo = 0;
    cabecalho->alturaArvore = 0;
    cabecalho->nroChavesTotal = 0;
}

void escreveCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo) {
    fseek(arquivo, 0, SEEK_SET);
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->noRaiz, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroChavesTotal, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->alturaArvore, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->RRNproxNo, sizeof(int), 1, arquivo);

    escreverLixo(arquivo, TAM_PG_DISCO_INDICE - TAM_CABECALHO_INDICE);
}

void leCabecalhoIndice (CabecalhoIndice *cabecalho, FILE *arquivo) {
    fseek(arquivo, 0, SEEK_SET);
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->noRaiz, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroChavesTotal, sizeof(int), 1, arquivo);
    fread(&cabecalho->alturaArvore, sizeof(int), 1, arquivo);
    fread(&cabecalho->RRNproxNo, sizeof(int), 1, arquivo);
}

No* buscaChaveArvoreB (int chave, int RRN, int* caminho, int* altura, FILE *arquivo, int* status) {
    No *no = leNo(RRN, arquivo);

    int i = 0;
    // Enquanto a chave buscada for maior que a chave do nó e não for a última chave do nó
    while (i < no->nroChavesNo && chave > no->dados[i].chave) {
        i++;
    }

    // Chave encontrada
    if (i < no->nroChavesNo && chave == no->dados[i].chave) {
        *status = SUCESSO;
        return no;
    }

    // Chave nao encontrada
    if (no->folha == '1') {
        *status = NAO_ENCONTRADO;
        return no;
    }

    if (no->descendentes[i] < 0) {
        *status = ERRO;
        return no;
    }

    caminho[*altura] = RRN;
    //printf("RRN: %d, *altura: %d, caminho[*altura]: %d\n", RRN, *altura, caminho[*altura]);
    (*altura)++;

    // printf("i: %d, folha: %c, descendentes[i]: %d\n", i, no->folha, no->descendentes[i]);
    // printf("RRN: %d, RRNPai: %d, status: %d\n", RRN, *RRNPai, *status);
    
    int descendente = no->descendentes[i];
    destroiNo(no);

    return buscaChaveArvoreB(chave, descendente, caminho, altura, arquivo, status);
}

Indice* promoverChave (No* no, Indice* ind, No* no_esq, No* no_dir) {
    // Promove uma chave
    Indice* ind_promovido;
    if (no_esq->nroChavesNo > no_dir->nroChavesNo) {
        // Promove última chave do no esquerdo
        ind_promovido = copiaIndice(&no_esq->dados[no_esq->nroChavesNo - 1]);
        limpaIndice(&no_esq->dados[no_esq->nroChavesNo - 1]);
        retiraNulosIndices(no_esq);
    } else {
        // Promove primeira chave no nó direito
        ind_promovido = copiaIndice(&no_dir->dados[0]);
        limpaIndice(&no_dir->dados[0]);
        retiraNulosIndices(no_dir);
    }

    return ind_promovido;
}

int noCheio (No* no) {
    return no->nroChavesNo == ORDEM - 1;
}

void repartirChavesNos (No* noPrincipal, No* no1, No* no2) {
    int chaveMax = ORDEM - 1;
    if (noPrincipal->RRNdoNo != no1->RRNdoNo) {
        for (int i = 0, j = 0; i < chaveMax/2; i++, j++) {
            no1->dados[j] = noPrincipal->dados[i];
            no1->descendentes[j] = noPrincipal->descendentes[i];
            limpaIndice(&noPrincipal->dados[i]);
            noPrincipal->descendentes[i] = -1;

            no1->nroChavesNo++;
            noPrincipal->nroChavesNo--;
        }
    }
    for (int i = chaveMax/2, j = 0; i < chaveMax; i++, j++) {
        no2->dados[j] = noPrincipal->dados[i];
        no2->descendentes[j] = noPrincipal->descendentes[i];
        limpaIndice(&noPrincipal->dados[i]);
        noPrincipal->descendentes[i] = -1;

        no2->nroChavesNo++;
        noPrincipal->nroChavesNo--;
    }
}

void dividirNo (No* no, No* novo_esq, No* novo_dir, CabecalhoIndice* ci, Indice* ind, FILE* arq) {
    novo_esq->folha = novo_dir->folha = no->folha;
    no->folha = '0';

    novo_esq->alturaNo = novo_dir->alturaNo = no->alturaNo + 1;
    if (novo_esq->RRNdoNo == -1) {
        novo_esq->RRNdoNo = ci->RRNproxNo;
        ci->RRNproxNo++;
    }
    novo_dir->RRNdoNo = ci->RRNproxNo;
    ci->RRNproxNo++;

    repartirChavesNos(no, novo_esq, novo_dir);

    if (ind->chave < novo_dir->dados[0].chave) {
        insereChaveNo(novo_esq, ind, -1);
    } else {
        insereChaveNo(novo_dir, ind, -1);
    }

    Indice* ind_promovido = promoverChave(no, ind, novo_esq, novo_dir);
    int indexInsert = insereChaveNo(no, ind_promovido, novo_dir->RRNdoNo);
    destroiIndice(ind_promovido);
    no->descendentes[indexInsert] = novo_esq->RRNdoNo;

    escreveNo(novo_esq, arq);
    escreveNo(novo_dir, arq);
    escreveNo(no, arq);

    // printf("Nó original após divisão [inserção da chave %d]:\n", ind->chave);
    // imprimeNo(no);
    // printf("Nó esquerdo:\n");
    // imprimeNo(novo_esq);
    // printf("Nó direito:\n");
    // imprimeNo(novo_dir);

    return;
}

int dividirNoComPai (No* pai, No* no_esq, No* novo_dir, CabecalhoIndice* ci, Indice* ind, FILE* arq) {
    novo_dir->RRNdoNo = ci->RRNproxNo;
    ci->RRNproxNo++;
    novo_dir->folha = no_esq->folha;

    repartirChavesNos(no_esq, no_esq, novo_dir);

    if (ind->chave < novo_dir->dados[0].chave) {
        insereChaveNo(no_esq, ind, -1);
    } else {
        insereChaveNo(novo_dir, ind, -1);
    }

    if (noCheio(pai)) {
        return ERRO;
    }

    // escreveNo(no_esq, arq);
    // escreveNo(novo_dir, arq);
    // escreveNo(pai, arq);

    return SUCESSO;
}

void imprimeNo (No* n) {
    if(n != NULL){
        printf("folha: %c, nroChavesNo: %d, alturaNo: %d, RRNdoNo: %d, descendentes: ", n->folha, n->nroChavesNo, n->alturaNo, n->RRNdoNo);
        for (int i = 0; i < ORDEM; i++) {
            printf("%d ", n->descendentes[i]);
        }
        printf(", dados: ");
        for (int i = 0; i < ORDEM - 1; i++) {
            printf("%d (%d) ", n->dados[i].chave, n->dados[i].referencia);
        }
        printf("\n");
    }
}

int buscaChaveNo (No* no, int chave) {
    int i = 0;
    while (i < no->nroChavesNo && chave > no->dados[i].chave) {
        i++;
    }

    if (i < no->nroChavesNo && chave == no->dados[i].chave) {
        return i;
    }

    return -1;
}

int referenciaChaveNo (No* no, int chave) {
    return no->dados[buscaChaveNo(no, chave)].referencia;
}

Indice* criaIndice (int chave, int referencia) {
    Indice *novo = (Indice*) malloc(sizeof(Indice));
    novo->chave = chave;
    novo->referencia = referencia;

    return novo;
}

void destroiIndice (Indice* indice) {
    free(indice);
}

void limpaIndice (Indice* indice) {
    indice->chave = -1;
    indice->referencia = -1;
}

void retiraNulosIndices (No* no) {
    for (int i = 0; i < no->nroChavesNo; i++) {
        if (no->dados[i].chave == -1) {
            for (int j = i; j < ORDEM - 2; j++) {
                no->dados[j] = no->dados[j + 1];
            }
            no->nroChavesNo--;
        }
    }
}

Indice* copiaIndice (Indice* origem) {
    Indice *novo = criaIndice(origem->chave, origem->referencia);

    return novo;
}

int insereChaveNo (No* no, Indice* indice, int desc) {
    int i = 0;
    while (i < no->nroChavesNo && indice->chave > no->dados[i].chave) {
        i++;
    }

    if (i < no->nroChavesNo && indice->chave == no->dados[i].chave) {
        return ERRO;
    }

    // Passa os dados e descendentes para a direita
    for (int j = no->nroChavesNo; j > i; j--) {
        no->dados[j] = no->dados[j - 1];
        no->descendentes[j + 1] = no->descendentes[j];
    }


    no->dados[i].chave = indice->chave;
    no->dados[i].referencia = indice->referencia;
    if (desc >= 0) {
        no->descendentes[i + 1] = desc;
    }
    no->nroChavesNo++;

    return i;
}

void divideNoRecursivo (No* no, int* caminho, int altura, Indice* indice, CabecalhoIndice* ci, FILE* arquivo) {
    No* novo_esq = criaNo();
    No* novo_dir = criaNo();
    int RRN_busca_pai = caminho[altura];
    printf("RRN_busca_pai: %d\n", RRN_busca_pai);
    if (RRN_busca_pai >= 0) {
        // printf("[=-=] Nó cheio com pai, dividindo...\n");
        No* pai = leNo(RRN_busca_pai, arquivo);
        // printf("Pai: \n");
        // imprimeNo(pai);
        if( dividirNoComPai(pai, no, novo_dir, ci, indice, arquivo) == ERRO ) {
            Indice* ind_promovido = promoverChave(pai, indice, no, novo_dir);
            insereChaveNo(pai, ind_promovido, novo_dir->RRNdoNo);
            divideNoRecursivo(pai, caminho, altura - 1, ind_promovido, ci, arquivo);
        
            destroiIndice(ind_promovido);
        }
        // printf("Ind: \n");
        // imprimeNo(ind);
        // printf("Novo dir: \n");
        // imprimeNo(novo_dir); 
        destroiNo(pai);
    } else {
        dividirNo(no, novo_esq, novo_dir, ci, indice, arquivo);
    }

}

int insereChaveArvoreB (Indice* indice, CabecalhoIndice* ci, FILE *arquivo) {
    printf("Insere a chave: %d\n", indice->chave);
    // Não tem raiz
    if (ci->noRaiz == -1) {
        No *novo = criaNo();
        novo->folha = '1';
        novo->alturaNo = 1;
        novo->RRNdoNo = ci->RRNproxNo;
        
        insereChaveNo(novo, indice, -1);
        escreveNo(novo, arquivo);

        printf("[1] Nova raiz criada\n");
        imprimeNo(novo);

        destroiNo(novo);
        ci->noRaiz = ci->RRNproxNo;
        ci->RRNproxNo++;
        printf("=== ARVORE B ===\n");
        imprimeArvoreB(arquivo, ci);
        return SUCESSO;
    }

    int status_busca, altura = 0;
    int *caminho = (int*) malloc(sizeof(int) * 64);
    No* ind = buscaChaveArvoreB(indice->chave, ci->noRaiz, caminho, &altura, arquivo, &status_busca);

    // Se a chave já existe, retorna erro
    if (status_busca != NAO_ENCONTRADO) {
        //printf("[ERRO NA BUSCA] Status: %d\n", indice->chave, status_busca);
        destroiNo(ind);
        return ERRO; 
    }

    // Cria um novo nó caso não exista
    if (ind->RRNdoNo < 0) {
        ind->RRNdoNo = ci->RRNproxNo;
    }

    // Verifica se o nó não está cheio e é folha
    if (!noCheio(ind) && ind->folha == '1') {
        insereChaveNo(ind, indice, -1);
        escreveNo(ind, arquivo);

        /*if (ind->nroChavesNo == ORDEM - 1) {
            (ci->RRNproxNo)++;
        }*/

        printf("[2] Nó folha não cheio\n");
        imprimeNo(ind);

        destroiNo(ind);
        printf("=== ARVORE B ===\n");
        imprimeArvoreB(arquivo, ci);
        return SUCESSO;
    }

    //No raiz cheio, cria nova raiz
    No *novoNo = criaNo();
    No *novoNoRaiz = criaNo();
    
    if(ind->folha == '1' && ind->RRNdoNo == ci->noRaiz && noCheio(ind)){
        divideNoTeste(ind, novoNo, ci);
        novoNoRaiz->alturaNo = ind->alturaNo + 1;
        novoNoRaiz->RRNdoNo = ci->RRNproxNo;
        novoNoRaiz->folha = '0';
        ci->RRNproxNo++;

        promoveIndice(ind, novoNo, novoNoRaiz, indice);

        if(indice->chave > novoNo->dados[0].chave){
            insereChaveNo(novoNo, indice, -1);
        } else {
            insereChaveNo(ind, indice, -1);
        }

        novoNoRaiz->descendentes[0] = ind->RRNdoNo;
        ci->noRaiz = novoNoRaiz->RRNdoNo;

        organizaNo(novoNo);
        organizaNo(ind);

        imprimeNo(novoNoRaiz);
        imprimeNo(ind);
        imprimeNo(novoNo);

        escreveNo(novoNoRaiz, arquivo);
        escreveNo(novoNo, arquivo);
        escreveNo(ind, arquivo);

        printf("=== ARVORE B ===\n");
        imprimeArvoreB(arquivo, ci);
        return SUCESSO;
    }


    // Se o nó está cheio, divide o nó
        No *raiz = leNo(ci->noRaiz, arquivo);
        No *filho;

        printf("Chave de insercao: %d\n", indice->chave);
        int RRNfilho = buscaProxNo(raiz, indice);
        if (RRNfilho == -1){
            filho = NULL;
        } else {
            filho = leNo(RRNfilho, arquivo);
        }

        if(filho != NULL){
            insereDivisaoRecursivo(raiz, filho, arquivo, indice, ci);
            //getc(stdin);
        }
        

    // No* novo_esq = criaNo();
    // No* novo_dir = criaNo();
    // printf("ALTURA: %d\n", altura);
    // if (altura >= 0 && caminho[altura] >= 0) {
    //     // printf("[=-=] Nó cheio com pai, dividindo...\n");
    //     No* pai = leNo(caminho[altura], arquivo);
    //     // printf("Pai: \n");
    //     // imprimeNo(pai);
    //     if( dividirNoComPai(pai, ind, novo_dir, ci, indice, arquivo) == ERRO ) {
    //         Indice* ind_promovido = promoverChave(pai, indice, ind, novo_dir);
    //         insereChaveNo(pai, ind_promovido, novo_dir->RRNdoNo);
    //         divideNoRecursivo(pai, caminho, altura - 1, ind_promovido, ci, arquivo);
        
    //         destroiIndice(ind_promovido);
    //     }
    //     // printf("Ind: \n");
    //     // imprimeNo(ind);
    //     // printf("Novo dir: \n");
    //     // imprimeNo(novo_dir); 
    //     destroiNo(pai);
    // } else {
    //     dividirNo(ind, novo_esq, novo_dir, ci, indice, arquivo);
    // }
    

    // printf("[3] Nó cheio, promovendo chave\n");
    // printf("Nó original:\n");
    // imprimeNo(ind);
    // printf("Nó esquerdo:\n");
    // imprimeNo(novo_esq);
    // printf("Nó direito:\n");
    // imprimeNo(novo_dir);

    destroiNo(ind);
    // destroiNo(novo_esq);
    // destroiNo(novo_dir);
    printf("=== ARVORE B ===\n");
    imprimeArvoreB(arquivo, ci);
    return SUCESSO;
}
/*
int TESTEinsereChaveArvoreB (Indice* indice, CabecalhoIndice* ci, FILE *arquivo) {
    No* no = criaNo();

    // Arvore vaiza, criacao da riz
    if (ci->noRaiz == -1) {
        no->folha = '1';
        no->alturaNo = 1;
        no->RRNdoNo = ci->RRNproxNo;
        insereChaveNo(no, indice, -1);
        escreveNo(no, arquivo);

        printf("[1] Nova raiz criada\n");
        imprimeNo(no);

        destroiNo(no);
        ci->noRaiz = ci->RRNproxNo;
        return SUCESSO;
    }

    no = leNo(ci->noRaiz, arquivo);
    // Raiz está cheia
    if (noCheio(no)) {
        No* novo = criaNo();
        novo->descendentes[0] = ci->noRaiz;
        TESTEdividirNo(novo, no);
        int i = 0;
        if (novo->dados[0].chave < indice->chave) {
            i++;
        }
        No* desc = leNo(novo->descendentes[i], arquivo);
        //TESTEinserirNoNaoCheio(desc, indice);
        no = novo;

        destroiNo(no);
        destroiNo(novo);
        destroiNo(desc);

        return SUCESSO;
    }
    
    //TESTEinserirNoNaoCheio(no, indice);
    destroiNo(no);
    return SUCESSO;
}*/
/*
int TESTEinserirNoNaoCheio(No* no, Indice* ind, FILE* arquivo) {
    int i = no->nroChavesNo - 1;
    if (no->folha = '1') {
        while (i >= 0 && no->dados[i].chave > ind->chave) {
            no->dados[i + 1] = no->dados[i];
            i--;
        }     

        no->dados[i + 1] = *ind;
        no->nroChavesNo++;
        return SUCESSO;
    }

    // Nó nao é folha
    while (i >= 0 && no->dados[i].chave > ind->chave) {
        i--;
    }

    No* filho = leNo(no->descendentes[i + 1], arquivo);
    if (noCheio(filho)) {
        TESTEdividirNo(i+1, filho);
        if (no->dados[i + 1].chave < ind->chave) {
            i++;
        }
    }
    return TESTEinserirNoNaoCheio(filho, ind, arquivo);
}
*/
/*int TESTEdividirNo(int i, No* no, CabecalhoIndice* ci, FILE* arquivo) {
    No* novo = criaNo();
    novo->folha = no->folha;
    novo->RRNdoNo = ci->RRNproxNo;
    ci->RRNproxNo++;

    for (int j = 0; j < (ORDEM-1)/2; j++) {
        novo->dados[j] = no->dados[j + (ORDEM-1)/2];
    }
    if (no->folha == '0') {
        for (int j = 0; j < ORDEM/2; j++) {
            novo->descendentes[j] = no->descendentes[j + ORDEM/2];
        }
    }
    no->nroChavesNo = ((ORDEM-1)/2) - 1;
    
    for (int j = )
}*/

void divideNoTeste(No *noAntigo, No *noNovo, CabecalhoIndice *cabecalho){//Divide um nó em dois
    noNovo->alturaNo = noAntigo->alturaNo;
    noNovo->folha = noAntigo->folha;
    noNovo->RRNdoNo = cabecalho->RRNproxNo;
    cabecalho->RRNproxNo++;

//    imprimeNo(noAntigo);

    int counterAux = noAntigo->nroChavesNo;
    int i = 0;
    for (i = 0; i < (counterAux / 2); i++){
        noNovo->dados[i] = noAntigo->dados[counterAux/2 + i];
        noAntigo->dados[counterAux/2 + i].chave = - 1;
        noAntigo->dados[counterAux/2 + i].referencia = - 1;

        noNovo->descendentes[i] = noAntigo->descendentes[counterAux/2 + i];
        noAntigo->descendentes[counterAux/2 + i] = - 1;
    }
    noNovo->descendentes[(counterAux + 1) / 2] = noAntigo->descendentes[counterAux];
    noAntigo->descendentes[counterAux] = - 1;

    noAntigo->nroChavesNo -= i;
    noNovo->nroChavesNo = i + 1;
    //printf("%d %d\n", noAntigo->nroChavesNo, noNovo->nroChavesNo);

    //imprimeNo(noAntigo);
    //imprimeNo(noNovo);
}

No* insereDivisaoRecursivo(No* noPai, No* noFilho, FILE *arquivo, Indice *chave, CabecalhoIndice *cabecalho){
    //getc(stdin);
    No* novoNo = criaNo();
    No* noDividido;

    Indice *chaveAux = chave; //*********
    printf("Chave: %d\n", chaveAux->chave);

    printf("No pai: ");
    imprimeNo(noPai);

    printf("No filho: \n");
    if (noFilho != NULL) imprimeNo(noFilho);
    
    if(noFilho == NULL){ //chegou no no folha -> retorna
        divideNoTeste(noPai, novoNo, cabecalho); //noPai é o nó folha
        if(chaveAux->chave < novoNo->dados[0].chave){
            insereChaveNo(noPai, chaveAux, -1);
        } else {
            insereChaveNo(novoNo, chaveAux, -1);
        }
        organizaNo(noPai);
        organizaNo(novoNo);
        //imprimeNo(noPai);
        //imprimeNo(novoNo);
        return novoNo;
    }

    int proxRRN = buscaProxNo(noFilho, chave);
    printf("Prox: %d\n", proxRRN);
    No *aux;
    if (proxRRN >= 0){
        aux = leNo(proxRRN, arquivo);
        //imprimeNo(aux);
    } else {
        aux = NULL;
    }
    //imprimeNo(aux);
    //printf("No pai e filho:\n");
    //imprimeNo(noPai);
    //imprimeNo(noFilho);
    //getc(stdin);
    noDividido = insereDivisaoRecursivo(noFilho, aux, arquivo, chave, cabecalho);

    //printf("Prox: %d\n", chaveAux->chave);
    /*if(noFilho == NULL){
        return novoNo;
    }*/
    if (noCheio(noPai)){
        if(noPai->RRNdoNo == cabecalho->noRaiz){
            divideNoTeste(noPai, novoNo, cabecalho);

            No* novoNoRaiz = criaNo();
            novoNoRaiz->alturaNo = noPai->alturaNo + 1;
            novoNoRaiz->RRNdoNo = cabecalho->RRNproxNo;
            novoNoRaiz->folha = '0';
            cabecalho->RRNproxNo++;

            if(chaveAux->chave < novoNo->dados[0].chave){
                chaveAux = promoveIndice(noFilho, noDividido, noPai, chaveAux);
            } else {
                chaveAux = promoveIndice(noFilho, noDividido, novoNo, chaveAux);
            }

            organizaNo(noFilho);
            organizaNo(noDividido);

            chaveAux = promoveIndice(noPai, novoNo, novoNoRaiz, chaveAux);

            novoNoRaiz->descendentes[0] = noPai->RRNdoNo;
            cabecalho->noRaiz = novoNoRaiz->RRNdoNo;

            organizaNo(novoNo);
            organizaNo(noPai);
            organizaNo(noFilho);
            organizaNo(noDividido);
            organizaNo(novoNoRaiz);

            imprimeNo(novoNoRaiz);
            imprimeNo(noPai);
            imprimeNo(novoNo);

            escreveNo(novoNoRaiz, arquivo);
            escreveNo(novoNo, arquivo);
            escreveNo(noPai, arquivo);
            escreveNo(noFilho, arquivo);
            escreveNo(noDividido, arquivo);
            return NULL;
        }

        divideNoTeste(noPai, novoNo, cabecalho);

        if(chaveAux->chave < novoNo->dados[0].chave){
            chaveAux = promoveIndice(noFilho, noDividido, noPai, chaveAux);
        } else {
            chaveAux = promoveIndice(noFilho, noDividido, novoNo, chaveAux);
        }

        //chaveAux = promoveIndice(noFilho, noDividido, noPai, chaveAux);

        organizaNo(noPai);
        organizaNo(noFilho);
        organizaNo(noDividido);
        organizaNo(novoNo);

        imprimeNo(noPai);
        imprimeNo(noFilho);
        imprimeNo(noDividido);

        escreveNo(noDividido, arquivo);
        escreveNo(noFilho, arquivo);
        //getc(stdin);
        return novoNo;
    } else if (noDividido == NULL){
        return NULL;
    }
    else {

        chaveAux = promoveIndice(noFilho, noDividido, noPai, chaveAux);

        organizaNo(noPai);
        organizaNo(noFilho);
        organizaNo(noDividido);

        imprimeNo(noPai);
        imprimeNo(noFilho);
        imprimeNo(noDividido);

        escreveNo(noDividido, arquivo);
        escreveNo(noFilho, arquivo);
        escreveNo(noPai, arquivo);
        //getc(stdin);
        return NULL;
    }
}

int buscaProxNo(No* noBusca, Indice *chave){
    if (noBusca->folha == '1') return -1;

    int i = 0;
    while(i < noBusca->nroChavesNo && chave->chave > noBusca->dados[i].chave){
        i++;
    }

    // if(chave->chave < noBusca->dados[i].chave){
    return noBusca -> descendentes[i];
    // } else {
    //     return noBusca -> descendentes[i + 1];
    // }
}

Indice* promoveIndice(No *noFilhoEsq, No *noFilhoDir, No *noPai, Indice *chave){

    if(noFilhoEsq == NULL || noFilhoDir == NULL){
        return chave;
    }

    Indice *indicePromovido = malloc(sizeof(Indice));

    /*printf("Chave: %d\n", chave->chave);
    printf("No de promocao antes:\n");
    imprimeNo(noPai);
    imprimeNo(noFilhoDir);
    imprimeNo(noFilhoEsq);*/
    //imprimeNo(noPai);
    
    if(chave->chave < noFilhoDir->dados[0].chave){
        int i = 0;
        while(noFilhoEsq->dados[i].chave > 0){
            indicePromovido = &noFilhoEsq->dados[i];
            i++;
        }
        organizaNo(noPai);
        insereChaveNo(noPai, indicePromovido, noFilhoDir->RRNdoNo);
        limpaIndice(&noFilhoEsq->dados[i - 1]);
        //noFilhoEsq->nroChavesNo--;
        
    } else {
        int i = 0;
        while(noFilhoEsq->dados[i].chave > 0){
            i++;
        }
        noFilhoEsq->descendentes[i] = noFilhoDir->descendentes[0];
        noFilhoDir->descendentes[0] = -1;
        //organizaDescendentes(noFilhoDir);
        int k;
        for(k = 1; k < ORDEM; k++){
            noFilhoDir->descendentes[k - 1] = noFilhoDir->descendentes[k];
        }
        noFilhoDir->descendentes[k] = -1;

        indicePromovido = &noFilhoDir->dados[0];
        organizaNo(noPai);
        insereChaveNo(noPai, indicePromovido, noFilhoDir->RRNdoNo);
        limpaIndice(&noFilhoDir->dados[0]);
        organizaNo(noFilhoDir);
        
        /*int i = 0;
        //Indice aux = noPai->dados[0];
        while(noPai->dados[i].chave != indicePromovido->chave && i < ORDEM - 1){
            i++;
            //aux = noPai->dados[i];
        }

        noPai->descendentes[i] = noFilhoEsq->RRNdoNo;*/

        //noFilhoDir->nroChavesNo--;
    }
    /*printf("No de promocao depois:\n");
    imprimeNo(noPai);
    imprimeNo(noFilhoDir);
    imprimeNo(noFilhoEsq);*/
    return indicePromovido;
}

/*int insereIndiceNo2(No *noInsere, Indice* chave){ //Insere o índice na menor posição do nó
    Indice auxA, auxB;
    int descAuxA, descAuxB;
    int i, encontrou = 0;
    int posInsercao;
    while(i < noInsere->nroChavesNo){
        if(encontrou){
            auxB = auxA;
            auxA = noInsere->dados[i];
            noInsere->dados[i] = auxB;
        }

        if(!encontrou && chave->chave < noInsere->dados[i].chave){
            posInsercao = i;
            encontrou = 1;
            auxA = noInsere->dados[i];
            noInsere->dados[i] = *chave;
        }

        i++;
    }
}*/

int organizaNo(No *no){

    if (no == NULL) return ERRO;

    /*no->nroChavesNo = ORDEM - 1;
    for (int i = 0; i < ORDEM -1; i++) {
        if (no->dados[i].chave == -1) {
            for (int j = i; j < ORDEM - 2; j++) {
                no->dados[j] = no->dados[j + 1];
            }
            no->nroChavesNo--;
        }
    }*/

    Indice aux = no->dados[0];
    int i = 0;
    int j = 0;
    int count = 0;
    while(i < ORDEM-1){
        if(aux.chave < 0){
            while(aux.chave < 0 && j < ORDEM - 2){
                j++;
                aux = no->dados[j];
            }
            
            no->dados[i] = aux;
            //if(aux.chave > 0) count++;
            no->dados[j].chave = -1;
            no->dados[j].referencia = -1;
            //aux = no->dados[j]; //????
        }
        if(no->dados[i].chave > 0) count++;
        i++;
        j = i;
        aux = no->dados[i];
    }

    no->nroChavesNo = count;

    return SUCESSO;
}

void imprimeArvoreB(FILE* arquivo, CabecalhoIndice* c){
    No* no;
    for (int i = 0; i < c->nroChavesTotal; i++){
        no = leNo(i, arquivo);
        if(no->RRNdoNo >= 0){
            imprimeNo(no);
        }
    }
}

void organizaDescendentes(No *no){
    int aux, i, j;
    aux = no->descendentes[0];
    i = 0;
    j = 0;
    while(i < ORDEM){
        if(aux == -1){
            while(aux < 0 && j < ORDEM){
                j++;
                aux = no->descendentes[j];
            }
            no->descendentes[i] = aux;
            no->descendentes[j] = -1;
        }
        i++;
        i = j;
        aux = no->descendentes[i];
    }
}