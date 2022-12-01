#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/arvoreB.h"
#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/busca.h"
#include "../headers/removeinsere.h"
#include "../headers/cabecalho.h"

// Aloca um novo nó com campos padroes
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

// Desaloca um nó
void destroiNo (No* no) {
    free(no->dados);
    free(no->descendentes);
    free(no);
}

// Calcula o byte offset baseado no RRN
int calculaByteOffsetArvoreB (int RRN) {
    return (TAM_PG_DISCO_INDICE + (RRN * TAM_NO));
}

// Escreve o conteudo de um nó no arquivo de indice, caso ele seja válido
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

// Le o conteudo do arquivo de indice com base em um RRN e coloca em um nó
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

    return novo;
}

// Busca uma chave em uma arvore B. RRN deve ser o RRN do no raiz, se status == SUCESSO, a chave foi encontrada.
No* buscaChaveArvoreB (int chave, int RRN, FILE *arquivo, int* status, int *pgs_acessadas) {
    No *no = leNo(RRN, arquivo);
    (*pgs_acessadas)++;

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

    // descendente invalido
    if (no->descendentes[i] < 0) {
        *status = ERRO;
        return no;
    }
    
    int descendente = no->descendentes[i];
    destroiNo(no);

    return buscaChaveArvoreB(chave, descendente, arquivo, status, pgs_acessadas);
}

// Retorna se nó está cheio
int noCheio (No* no) {
    return no->nroChavesNo == ORDEM - 1;
}

// Busca uma chave dentro de um nó, retorna o index da chave no nó
int buscaChaveNo (No* no, int chave) {
    int i = 0;

    // incrementa i até a ultima chave do nó menor que a chave passada
    while (i < no->nroChavesNo && chave > no->dados[i].chave) {
        i++;
    }

    // caso encontrou a chave, retorna o index (i)
    if (i < no->nroChavesNo && chave == no->dados[i].chave) {
        return i;
    }

    return -1;
}

// Busca a referencia ao arquivo de dados de uma chave dentro de um nó
int referenciaChaveNo (No* no, int chave) {
    return no->dados[buscaChaveNo(no, chave)].referencia;
}

// Coloca os valores de chave e referencia do indice como -1
void limpaIndice (Indice* indice) {
    indice->chave = -1;
    indice->referencia = -1;
}

// Coloca um Indice em um nó
int insereChaveNo (No* no, Indice* indice, int desc) {
    int i = 0;
    // Encontra o lugar onde a chave deve ser inserida
    while (i < no->nroChavesNo && indice->chave > no->dados[i].chave) {
        i++;
    }

    // Caso a chave já esteja lá, retorna ERRO
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
    // Caso o descendente passado como parametro seja valido, insere ele no nó
    if (desc >= 0) {
        no->descendentes[i + 1] = desc;
    }
    no->nroChavesNo++;

    return i;
}

// Função que organiza nó removendo espaços vazios
int organizaNo(No *no){
    if (no == NULL) return ERRO;

    Indice aux = no->dados[0];
    int i = 0;
    int j = 0;
    int count = 0;

    // Percorre todo o nó
    while(i < ORDEM-1){
        // Se for um espaço vazio
        if(aux.chave < 0){
            // Procura o próximo índice válido
            while(aux.chave < 0 && j < ORDEM - 2){
                j++;
                aux = no->dados[j];
            }
            // Adiciona o índice achado no espaço vazio, deslocando um a um até organizar
            no->dados[i] = aux;
            no->dados[j].chave = -1;
            no->dados[j].referencia = -1;
        }
        if(no->dados[i].chave > 0) count++; // atualiza a quantidade de chaves no nó
        i++;
        j = i;
        aux = no->dados[i];
    }

    no->nroChavesNo = count;

    return SUCESSO;
}

// Função de promover um índice nos filhos onde a chave foi inserida e no pai onde vai ser promovido
void promoveIndice(No *noFilhoEsq, No *noFilhoDir, No *noPai, Indice *chave){
    if(noFilhoEsq == NULL || noFilhoDir == NULL){
        return;
    }

    Indice indicePromovido;

    // Se a chave for menor que o primeiro da direita, então ela foi inserida na esquerda
    if(chave->chave < noFilhoDir->dados[0].chave){
        // Procura o último índice válido no nó da esquerda (quem vai ser promovido)
        int i = 0;
        while(noFilhoEsq->dados[i].chave > 0){
            indicePromovido = noFilhoEsq->dados[i];
            i++;
        }

        // Insere nó noPai removendo do nó filho
        organizaNo(noPai);
        insereChaveNo(noPai, &indicePromovido, noFilhoDir->RRNdoNo);
        limpaIndice(&noFilhoEsq->dados[i - 1]);

        // Passa o último descendente do filho da esquerda (direita do promovido) para o primeiro do filho da direita
        noFilhoDir->descendentes[0] = noFilhoEsq->descendentes[i];
        noFilhoEsq->descendentes[i] = -1;
        
    } else { //se não, foi inserido no filho da direita
        // Desloca todos os descendentes para a esquerda, preenchendo o primeiro espaço que estava vazio
        int k;
        for(k = 1; k < ORDEM; k++){
            noFilhoDir->descendentes[k - 1] = noFilhoDir->descendentes[k];
        }
        noFilhoDir->descendentes[k] = -1;

        // O primeiro índice será promovido e inserido no noPai, depois apaga e organiza no noFilho
        indicePromovido = noFilhoDir->dados[0];
        organizaNo(noPai);
        insereChaveNo(noPai, &indicePromovido, noFilhoDir->RRNdoNo);
        limpaIndice(&noFilhoDir->dados[0]);
        organizaNo(noFilhoDir);
    }

    return;
}

// Retorna o RRN do proximo nó na busca
int buscaProxNo(No* noBusca, Indice *chave){
    if (noBusca->folha == '1') return -1;

    int i = 0;
    while(i < noBusca->nroChavesNo && chave->chave > noBusca->dados[i].chave){
        i++;
    }

    return noBusca -> descendentes[i];
}

// Função recursiva de inserção (para o caso em que o nó folha está cheio - e não é o nó raiz)
No* insereDivisaoRecursivo(No* noPai, No* noFilho, FILE *arquivo, Indice *chave, CabecalhoIndice *cabecalho){
    No* novoNo = criaNo();
    No* noDividido;
    
    if(noFilho == NULL){ //chegou no no folha -> retorna
        // Divide o nó
        divideNo(noPai, novoNo, cabecalho); //noPai é o nó folha
        
        // Insere o novo índice na folha
        // Identifica em qual nó deve ser inserida a chave
        if(chave->chave < novoNo->dados[0].chave){ //menor que o primeiro da direita, insere na esquerda
            insereChaveNo(noPai, chave, -1);
        } else { //caso contrário
            insereChaveNo(novoNo, chave, -1);
        }
        organizaNo(noPai);
        organizaNo(novoNo);
        
        // Retorna o nó dividido
        return novoNo;
    }

    // Encontra o próximo nó
    int proxRRN = buscaProxNo(noFilho, chave);
    No *aux;
    if (proxRRN >= 0){
        aux = leNo(proxRRN, arquivo);
    } else {
        aux = NULL; //nó filho já é o folha
    }

    // Avança na recursão - o nó filho passa a ser o nó pai
    // após o retorno tem-se:
    // o noPai, onde vai receber o nó promovido
    // o novoNo (irmão do nó pai - para quando o noPai também está cheio) também pode receber a promoção
    // o noFilho e no noDividido, onde foi inserido o indice (da promoção/inserção passada)
    noDividido = insereDivisaoRecursivo(noFilho, aux, arquivo, chave, cabecalho);

    // Não tem mais o que fazer, apenas volta na recursão
    if (noDividido == NULL) {
        destroiNo(novoNo);
        destroiNo(aux);
        return NULL;
    }

    //Se o nó pai está cheio também, divide antes de promover
    if (noCheio(noPai)){
        if(noPai->RRNdoNo == cabecalho->noRaiz){ //se o noPai for a raiz, cria outra raiz
            divideNo(noPai, novoNo, cabecalho);

            //Cria e atualiza os dados da raiz
            No* novoNoRaiz = criaNo();
            novoNoRaiz->alturaNo = noPai->alturaNo + 1;
            novoNoRaiz->RRNdoNo = cabecalho->RRNproxNo;
            novoNoRaiz->folha = '0';
            cabecalho->RRNproxNo++;

            // Identifica em qual no deve ser inserido o índice promovido
            if(chave->chave < novoNo->dados[0].chave){
                promoveIndice(noFilho, noDividido, noPai, chave);
            } else {
                promoveIndice(noFilho, noDividido, novoNo, chave);
            }

            organizaNo(noFilho);
            organizaNo(noDividido);

            // Promove pela segunda vez na nova raiz
            promoveIndice(noPai, novoNo, novoNoRaiz, chave);

            novoNoRaiz->descendentes[0] = noPai->RRNdoNo;
            cabecalho->noRaiz = novoNoRaiz->RRNdoNo;

            // Organiza todos os nós
            organizaNo(novoNo);
            organizaNo(noPai);
            organizaNo(noFilho);
            organizaNo(noDividido);
            organizaNo(novoNoRaiz);

            // Escreve todos no arquivo
            escreveNo(novoNoRaiz, arquivo);
            escreveNo(novoNo, arquivo);
            escreveNo(noPai, arquivo);
            escreveNo(noFilho, arquivo);
            escreveNo(noDividido, arquivo);

            // Desaloca os nós
            destroiNo(novoNoRaiz);
            destroiNo(novoNo);
            destroiNo(noDividido);
            if (aux != NULL) destroiNo(aux);

            return NULL; // encerra a recursão
        }

        // Caso em que o pai ta cheio e não é raiz
        // divide
        divideNo(noPai, novoNo, cabecalho);

        // promove
        // a chave passa a ser o indice promovido
        if(chave->chave < novoNo->dados[0].chave){
            promoveIndice(noFilho, noDividido, noPai, chave);
        } else {
            promoveIndice(noFilho, noDividido, novoNo, chave);
        }

        // organiza todos os nós
        organizaNo(noPai);
        organizaNo(noFilho);
        organizaNo(noDividido);
        organizaNo(novoNo);

        // escreve no arquivo os nós filhos (não terão mais alterações)
        escreveNo(noDividido, arquivo);
        escreveNo(noFilho, arquivo);

        // desaloca o noDividido (será criado na próxima recursão)
        destroiNo(noDividido);
        if (aux != NULL) destroiNo(aux);

        return novoNo; // será o noDividido da próxima recursão
    } else { // Se o pai não está cheio apenas promove no pai e encerra a recursão
        promoveIndice(noFilho, noDividido, noPai, chave);

        organizaNo(noPai);
        organizaNo(noFilho);
        organizaNo(noDividido);

        escreveNo(noDividido, arquivo);
        escreveNo(noFilho, arquivo);
        escreveNo(noPai, arquivo);

        destroiNo(noDividido);
        destroiNo(novoNo);
        if (aux != NULL) destroiNo(aux);

        return NULL;
    }
}

// Função de split de um nó
void divideNo(No *noAntigo, No *noNovo, CabecalhoIndice *cabecalho){
    // Atualiza os dados do novo nó, de acordo com o nó a ser dividido
    noNovo->alturaNo = noAntigo->alturaNo;
    noNovo->folha = noAntigo->folha;
    noNovo->RRNdoNo = cabecalho->RRNproxNo;
    cabecalho->RRNproxNo++;

    // Faz a divisão
    int counterAux = noAntigo->nroChavesNo;
    int i = 0;
    for (i = 0; i < (counterAux / 2); i++){
        // Adiciona a segunda metade do nó antigo no começo do novo nó
        noNovo->dados[i] = noAntigo->dados[counterAux/2 + i];
        noAntigo->dados[counterAux/2 + i].chave = - 1;
        noAntigo->dados[counterAux/2 + i].referencia = - 1;

        // Atualiza a quantidade de índice em cada um
        noAntigo->nroChavesNo--;
        noNovo->nroChavesNo++;

        // Desloca os descendentes da direita para o novo nó
        noNovo->descendentes[i + 1] = noAntigo->descendentes[counterAux/2 + i + 1];
        noAntigo->descendentes[counterAux/2 + i + 1] = - 1;
    }
}

// Função principal da inserção - trata todos os casos de inserção
int insereChaveArvoreB (Indice* indice, CabecalhoIndice* ci, FILE *arquivo) {
    // Não tem raiz - primeira inserção da tabela de indices
    if (ci->noRaiz == -1) {
        // Cria uma nova raiz
        No *novo = criaNo();
        novo->folha = '1';
        novo->alturaNo = 1;
        novo->RRNdoNo = ci->RRNproxNo;
        
        insereChaveNo(novo, indice, -1);
        escreveNo(novo, arquivo);

        destroiNo(novo);
        ci->noRaiz = ci->RRNproxNo;
        ci->RRNproxNo++;

        return SUCESSO;
    }

    // Busca o nó folha onde tem que ser inserida a chave
    int status_busca, pgs = 0;
    No* ind = buscaChaveArvoreB(indice->chave, ci->noRaiz, arquivo, &status_busca, &pgs);

    // Se a chave já existe, retorna erro
    if (status_busca != NAO_ENCONTRADO) {
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

        destroiNo(ind);

        return SUCESSO;
    }

    //No raiz cheio, cria nova raiz    
    if(ind->folha == '1' && ind->RRNdoNo == ci->noRaiz && noCheio(ind)){
        No *novoNo = criaNo();
        No *novoNoRaiz = criaNo();

        // Faz o split da antiga raiz e atualiza os dados da nova raiz
        divideNo(ind, novoNo, ci);
        novoNoRaiz->alturaNo = ind->alturaNo + 1;
        novoNoRaiz->RRNdoNo = ci->RRNproxNo;
        novoNoRaiz->folha = '0';
        ci->RRNproxNo++;

        // Insere o índice
        if(indice->chave > novoNo->dados[0].chave){
            insereChaveNo(novoNo, indice, -1);
        } else {
            insereChaveNo(ind, indice, -1);
        }

        // Faz a promoção na nova raiz
        promoveIndice(ind, novoNo, novoNoRaiz, indice);
        novoNoRaiz->descendentes[0] = ind->RRNdoNo;

        ci->noRaiz = novoNoRaiz->RRNdoNo;

        organizaNo(novoNo);
        organizaNo(ind);

        escreveNo(novoNoRaiz, arquivo);
        escreveNo(novoNo, arquivo);
        escreveNo(ind, arquivo);

        if (novoNo != NULL) destroiNo(novoNo);
        destroiNo(novoNoRaiz);
        destroiNo(ind);

        return SUCESSO;
    }


    // Se o nó está cheio, divide o nó
    No *raiz = leNo(ci->noRaiz, arquivo);
    No *filho;

    // Próximo nó depos da raiz
    int RRNfilho = buscaProxNo(raiz, indice);
    if (RRNfilho == -1){
        filho = NULL;
    } else {
        filho = leNo(RRNfilho, arquivo);
    }

    // Inicia a recursão a partir da raiz
    if(filho != NULL){
        insereDivisaoRecursivo(raiz, filho, arquivo, indice, ci);
    }

    destroiNo(ind);
    destroiNo(raiz);
    destroiNo(filho);

    return SUCESSO;
}