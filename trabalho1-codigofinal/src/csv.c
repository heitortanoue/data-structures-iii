#include "../headers/csv.h"
#include "../headers/registro.h"
#include "../headers/orgarquivos.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Lê uma 'linha' do arquivo .csv, valida os dados e insere no registro 'r'
int lerCsvRegistro (Registro *r, char *linha) {
    char *temp = malloc(sizeof(char) * 64);

    meuStrtok(temp, linha, ',');
    r->idConecta = validaInt(temp);

    meuStrtok(temp, linha, ',');
    strcpy(r->nomePoPs, validaString(temp));

    meuStrtok(temp, linha, ',');
    strcpy(r->nomePais, validaString(temp));

    meuStrtok(temp, linha, ',');
    strcpy(r->siglaPais, validaString(temp));

    meuStrtok(temp, linha, ',');
    r->idPoPsConectado = validaInt(temp);

    meuStrtok(temp, linha, ',');
    r->unidadeMedida = validaChar(temp);

    meuStrtok(temp, linha, ',');
    // printf("Velocidade: %d, %d\n", atoi(temp), atoi(temp) == 0);  
    r->velocidade = validaInt(temp);

    free(temp);

    return SUCESSO;
}

// Faz o parse de uma linha, como se fosse a função split de outras linguagens
// Retorna em pedaço a substring da linha até o delimitador
// Retorna em linha o resto da linha
void meuStrtok (char* pedaco, char *str, char delim) {
    int i = 0;
    while (str[i] != delim && str[i] != '\0') {
        pedaco[i] = str[i];
        i++;
    }

    if (pedaco[i - 1] == ' ') {
        pedaco[i - 1] = '\0';
    } else {
        pedaco[i] = '\0';
    }

    if (str[i] == delim) {
        i++;
    }

    int j = 0;
    while (str[i] != '\0') {
        str[j] = str[i];
        i++;
        j++;
    }
    str[j] = '\0';
}

// As funções abaixo tornam válidas as informações lidas do arquivo .csv
char* validaString (char* str) {
    return str && strlen(str) ? str : "*";
}
int validaInt (char* i) {
    return i && intValido(atoi(i)) ? atoi(i) : -1;
}
char validaChar (char* c) {
    return c && c[0] != '\0' ? c[0] : '*';
}

// As funções abaixo verificam a validade das informações
int stringValida (char* str) {
    return str && str[0] && str[0] != '*';
}
int intValido (int i) {
    return i > 0;
}
int charValido (char c) {
    return c != '*';
}