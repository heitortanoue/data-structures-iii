#ifndef __REGISTRO_H__
#define __REGISTRO_H__

#define TAM_SIGLA 2
#define TAM_STATUS 1

#define TAM_PG_DISCO 960
#define TAM_REGISTRO 64
#define TAM_CABECALHO 21
#define TAM_STRING 128

// tamanho fixo de 64 bytes
typedef struct {
    // campos de controle
    // TAM = 5 bytes
    char removido;
    int encadeamento;

    // campos de tamanho fixo
    // TAM = 15 bytes
    int idConecta;
    char siglaPais[TAM_STRING];
    int idPoPsConectado;
    char unidadeMedida;
    int velocidade;

    // campos de tamanho variável
    // TAM_MAX = 44 bytes
    char nomePoPs[TAM_STRING];
    char nomePais[TAM_STRING];
} Registro;

// Tamanho fixo de 21 bytes mas alocar em uma pagina de disco inteira (960 bytes)
typedef struct {
    char status;
    int topo;
    int proxRRN;
    int nroRegRem;
    int nroPagDisco;
    int qttCompacta;
} Cabecalho;

typedef struct {
    Cabecalho cabecalho;
    Registro* registros;
} Arquivo;


int lerDadosRegistroTeclado(Registro *t);
int insereStringRegistro (char* str, char* str_registro, int tam_campo);
void imprimeRegistro (Registro *r);
int registroRemovido (Registro *r);
int atualizarCabecalhoPadrao (Cabecalho *c);
void readline(char* string);

int lerCsvRegistro (Registro *r, char *linha);
char* validaString (char* str);
int validaInt (char* i);
char validaChar (char* c);
int stringValida (char* str);
int intValido (int i);
int charValido (char c);
void meuStrtok (char* pedaco, char *str, char delim);

#endif