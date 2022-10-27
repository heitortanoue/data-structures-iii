#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/funcionalidades.h"
#include "../headers/removeinsere.h"

/*
Heitor Tanoue de Mello - NUSP: 12547260
Pedro Oliveira Torrente - NUSP: 11798853
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
        default:
            break;
    }
}