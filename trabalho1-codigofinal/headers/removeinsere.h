#ifndef __REMOVEINSERE_H__
#define __REMOVEINSERE_H__

int remocaoReg(FILE *arq, int rrn, Cabecalho *c);
int entradaDados(Registro *reg);
int trataDados(Registro *reg, char **dados_string);
//int insereStringReg(void *campo, char *dado);
int insereRegistro(Registro *reg, Cabecalho *c, FILE *arq);
void scanTeclado(char* strDest);
int compactaRecursivo(FILE *arq, Registro *r, int *posVolta, int *posInsere, int qtdReg);

#endif