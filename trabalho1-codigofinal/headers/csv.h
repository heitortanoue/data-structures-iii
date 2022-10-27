#ifndef __CSV_H__
#define __CSV_H__
#include "registro.h"

int lerCsvRegistro (Registro *r, char *linha);
char* validaString (char* str);
int validaInt (char* i);
char validaChar (char* c);
int stringValida (char* str);
int intValido (int i);
int charValido (char c);
void meuStrtok (char* pedaco, char *str, char delim);

#endif