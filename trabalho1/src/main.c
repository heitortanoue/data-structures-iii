#include "orgarquivos.h"
#include "registro.h"
#include "funcionalidades.h"

int main () {
    int comando;
    scanf("%d", &comando);
    switch (comando) {
        case 1:
            funcionalidade1();
            break;
        case 2:
            funcionalidade2();
            break;
        default:
            break;
    }
}