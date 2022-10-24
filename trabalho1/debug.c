#include <stdio.h>
#include <stdlib.h>

int debug() {
    FILE* arq1;
    FILE* arq2;

    arq1 = fopen("./debug/binario1.bin", "rb");
    arq2 = fopen("dados.bin", "rb");

    if (arq1 == NULL || arq2 == NULL) {
        printf("Erro ao abrir arquivo");
        return 1;
    }

    int i = 0;
    while (!feof(arq1) && !feof(arq2)) {
        char c1 = fgetc(arq1);
        char c2 = fgetc(arq2);

        if (c1 != c2) {
            printf("[%d] (%c - %d) (%c - %d)\n", i, c1, c1, c2, c2);
            //return 1;
        }

        i++;
    }

    fclose(arq1);
    fclose(arq2);
    return 0;
}

int main () {
    debug();

    return 0;
}