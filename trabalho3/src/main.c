#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/funcionalidades.h"
#include "../headers/removeinsere.h"

/*
      ---------------------------------------------
    [  TRABALHO3 - ESTRUTURA DE DADOS III - 2022/2  ]
      ---------------------------------------------
    
    ->  Heitor Tanoue de Mello
        NUSP: 12547260
        100% da sua parte   

    ->  Pedro Oliveira Torrente
        NUSP: 11798853
        100% da sua parte
*/

int main () {
    int comando;
    scanf("%d", &comando);
    switch (comando) {
        case 1:
            createTable();
            break;
        case 2:
            selectFrom();
            break;
        case 3:
            selectWhere();
            break;
        case 4:
            removeRegistro();
            break;
        case 5:
            insert();
            break;
        case 6:
            compact();
            break;
        case 7:
            createIndex();
            break;
        case 8:
            searchIndex();
            break;
        case 9:
            insertWithIndex();
            break;
        case 10:
            joinOn();
            break;
        case 11:
            createGraph();
            break;
        default:
            break;
    }
}