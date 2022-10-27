#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/busca.h"
#include "../headers/removeinsere.h"

// Remove o registro de RRN recebido
int remocaoReg(FILE *arq, int rrn, Cabecalho *c){
    fseek(arq, calculaByteoffset(rrn), SEEK_SET);
    fwrite(STR_REMOVIDO, sizeof(char), 1, arq);

    int topo = c->topo; //atualiza pilha no registro logicamente removido
    fwrite(&topo, sizeof(int), 1, arq);

    int tamLixo; //completa com lixo o restante do registro
    tamLixo = calculaByteoffset(rrn+1) - ftell(arq);
    escreverLixo(arq, tamLixo);
    
    c->topo = rrn; //atualiza os dados do cabeçalho
    c->nroRegRem++;
    fseek(arq, 0, SEEK_SET);

    return SUCESSO;
}

// Le os dados do teclado e salva na struct reg
int entradaDados(Registro *reg){
    char **dados_string = (char **) alocaMemoria(sizeof(char *) * 7);
    for (int i = 0; i < 7; i++){
        dados_string[i] = (char * ) alocaMemoria(sizeof(char) * TAM_REGISTRO);
    }

    
    for (int i = 0; i < 7; i++){
        scanTeclado(dados_string[i]); //le ate espaço ou \n removendo as aspas se tiver
        if(!strcmp(dados_string[i], "NULO")){
            strcpy(dados_string[i], "");
        }
    }

    trataDados(reg, dados_string);

    for(int i = 0; i < 7; i++){
        free(dados_string[i]);
    }
    free(dados_string);

    return SUCESSO;
}

// Adiciona os dados em string para uma struct registro
int trataDados(Registro *reg, char **dados_string){
    reg->idConecta = validaInt(dados_string[0]);

    strcpy(reg->nomePoPs, dados_string[1]);
    strcpy(reg->nomePais, dados_string[2]);
    strcpy(reg->siglaPais, dados_string[3]);

    reg->idPoPsConectado = validaInt(dados_string[4]);
    reg->unidadeMedida = validaChar(dados_string[5]);
    reg->velocidade = validaInt(dados_string[6]);

    return SUCESSO;
}

// Insere o novo registro no arquivo atualizando a pilha
int insereRegistro(Registro *reg, Cabecalho *c, FILE *arq){
    int RRNInsercao = c->topo;
    char removido = reg->removido;

    if(RRNInsercao >= 0){ //se tiver campo logicamente removido insere nele
        fseek(arq, calculaByteoffset(RRNInsercao), SEEK_SET);

        //salva o proximo registro da pilha e atualiza a informacoes iniciais do novo registro
        int proxTopo = 0;
        fwrite(&removido, sizeof(char), 1, arq);
        fread(&proxTopo, sizeof(int), 1, arq);
        fseek(arq, -4, SEEK_CUR);
        fwrite(&reg->encadeamento, sizeof(int), 1, arq);

        //insere os dados
        inserirCampoFixo(&reg->idConecta, sizeof(int), 1, sizeof(int), arq);
        inserirStringCampoFixo(reg->siglaPais, TAM_SIGLA, arq);

	    inserirCampoFixo(&reg->idPoPsConectado, sizeof(int), 1, sizeof(int), arq);
	    inserirCharCampoFixo(reg->unidadeMedida, 1, arq);
	    inserirCampoFixo(&reg->velocidade, sizeof(int), 1, sizeof(int), arq);

	    inserirCampoVariavel(reg->nomePoPs, arq);
	    inserirCampoVariavel(reg->nomePais, arq);

        //completa com lixo
        int tam_inicial = calculaByteoffset(RRNInsercao);
        int tam_final = ftell(arq);
	    int tam = tam_final - tam_inicial;
	    int tam_lixo = TAM_REGISTRO - tam;
	    escreverLixo(arq, tam_lixo);

        //atualiza o topo
        c->topo = proxTopo;
        c->nroRegRem--;

    } else { //insere no final
        fseek(arq, calculaByteoffset(c->proxRRN), SEEK_SET);
        inserirRegistroArquivo(arq, reg);
        c->proxRRN++;
    }
    return SUCESSO;
}

// Le um campo do novo registro removendo as aspas
void scanTeclado(char* strDest){
    char c = getchar();
    if (c != '\"'){ //se for uma entrada sem aspas le ate o proximo espaco, enter ou valor invalido
        int i = 0;
        while((c != ' ' && c != '\n') || (c > 32 && c < 126)){
            if (i > 60){
                break;
            }
            strDest[i] = c;
            c = getchar();
            i++;
        }
        strDest[i] = '\0';
    } else { //se for uma entrada com aspas le ate a proxima aspas
        int i = 0;
        c = getchar();
        while(c != '\"'){
            if (i > 60){
                break;
            }
            strDest[i] = c;
            c = getchar();
            i++;
        }
        getchar();
        strDest[i] = '\0';
    }
}