
# Arquivos binários
> *Trabalho 3 de Estrutura de Dados III (SCC0607) - 2022/2*


![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)

Este trabalho tem como objetivo armazenar chaves primárias em uma árvore-B, bem como desenvolver funcionalidades para a manipulação desses dados.


## Estrutura

O arquivo final é composto por um cabeçalho (que ocupa uma página de disco inteira) e *n* registros.
```c
// Quantidade de bytes do registro de dados
#define TAM_PG_DISCO 960
// Quantidade de bytes do registro de dados
#define TAM_REGISTRO 64
// Quantidade de bytes da parte de tamanho fixo do registro de dados
#define TAM_REGISTRO_FIXO 20
// Quantidade de bytes a serem alocados para manipulacao de strings
#define TAM_STRING 128
```
```c
// Caractere padrao para preencher bytes vazios do registro
#define LIXO '$'
// Caracteres padrao para remocao logica
#define REMOVIDO '1'
#define NAO_REMOVIDO '0'
// Caractere delimitador de campos de tamanho variavel
#define DELIMITADOR '|'
```

### Cabeçalho

O registro de cabeçalho deve conter os seguintes campos:
- *status*: indica a consistência do arquivo de dados, devido à queda de energia, travamento do programa, etc. Pode assumir os valores ‘0’, para indicar que o arquivo de dados está inconsistente, ou ‘1’, para indicar que o arquivo de dados está consistente. Ao se abrir um arquivo para escrita, seu status deve ser ‘0’ e, ao finalizar o uso desse arquivo, seu status deve ser ‘1’ – tamanho: string de 1 byte.
- *topo*: armazena o RRN de um registro logicamente removido, ou -1 caso não haja registros logicamente removidos – tamanho: inteiro de 4 bytes.
- *proxRRN*: armazena o valor do próximo RRN disponível. Deve ser iniciado com o valor ‘0’ e ser incrementado quando necessário – tamanho: inteiro de 4 bytes.
- *nroRegRem*: armazena o número de registros logicamente marcados como removidos. Deve ser iniciado com o valor ‘0’ e deve ser atualizado sempre que necessário – tamanho: inteiro de 4 bytes.
- *nroPagDisco*: armazena o número de páginas de disco ocupadas pelo arquivo de dados, de acordo com a definição de página de disco feita neste trabalho. Deve ser iniciado com o valor ‘0’ e deve ser atualizado sempre que necessário – tamanho: inteiro de 4 bytes.
- *qttCompacta*: indica a quantidade de vezes que o arquivo de dados foi compactado. Deve ser iniciado com o valor ‘0’ e deve ser atualizado sempre que a funcionalidade de compactação for solicitada – tamanho: inteiro de 4 bytes.

#### Estrutura do cabeçalho
```c
// Tamanho fixo de 21 bytes mas alocar em uma pagina de disco inteira (960 bytes)
typedef struct {
    char status;
    int topo;
    int proxRRN;
    int nroRegRem;
    int nroPagDisco;
    int qttCompacta;
} Cabecalho;
```

### Registros

Os registros de dados são de tamanho fixo, com campos de tamanho fixo e campos de tamanho variável. Para os campos de tamanho variável, deve ser usado o método delimitador entre campos. O delimitador é o caractere ‘|’.
Os campos de tamanho fixo são definidos da seguinte forma:
- *idConecta*: código identificador do ponto de presença (PoPs) – inteiro – tamanho: 4 bytes. Não pode assumir valores nulos. Pode ter valores repetidos.
- *siglaPais*: sigla do país no qual o PoPs está cadastrado – string – tamanho: 2 bytes. Pode assumir valores nulos. Pode ter valores repetidos.
- *idPoPsConectado*: código identificador do PoPs conectado – tamanho: 4 bytes. Pode assumir valores nulos. Pode ter valores repetidos.
- *unidadeMedida*: unidade de medida da velocidade de transmissão – tamanho: string de 1 byte. Pode assumir valores nulos. Pode ter valores repetidos.
- *velocidade: velocidade de transmissão – inteiro – tamanho: 4 bytes. Pode assumir valores nulos. Pode ter valores repetidos.

Os campos de tamanho variável são definidos da seguinte forma:
- *nomePoPs*: nome do PoPs: string. Pode assumir valores nulos. Pode ter valores repetidos.
- *nomePais*: nome do país por escrito – string. Pode assumir valores nulos. Pode ter valores repetidos.

Adicionalmente, os seguintes campos de tamanho fixo também compõem cada registro. Esses campos de controle são necessários para o gerenciamento de registros logicamente removidos.
- *removido*: indica se o registro está logicamente removido. Pode assumir os valores ‘1’, para indicar que o registro está marcado como logicamente removido, ou ‘0’, para indicar que o registro não está marcado como removido.
- *tamanho*: string de 1 byte.
- *encadeamento*: armazena o RRN do próximo registro logicamente removido – tamanho: inteiro de 4 bytes. Deve ser inicializado com o valor -1 quando necessário. 

#### Estrutura do registro
```c
// tamanho fixo de 64 bytes
typedef struct {
    // campos de controle
    // TAM = 5 bytes
    char removido;
    int encadeamento;

    // campos de tamanho fixo
    // TAM = 15 bytes
    int idConecta;
    char* siglaPais;
    int idPoPsConectado;
    char unidadeMedida;
    int velocidade;

    // campos de tamanho variável
    // TAM_MAX = 44 bytes
    char* nomePoPs;
    char* nomePais;
} Registro;
```
## Funcionalidades

### Create Table
Cria uma tabela com os campos especificados com base em um arquivo .CSV e gera de saída um arquivo .BIN
```
1 [nome-arquivo-entrada-csv] [nome-arquivo-saida-bin]
```
A saída é o valor da função binarioNaTela, cedida pela professora para correção do trabalho.

### Select From
Imprime todos os registros de uma arquivo .BIN
```
2 [nome-arquivo-entrada-bin]
```
Exemplo de saída:
```
Identificador do ponto: 80
Sigla do pais: MD
Identificador do ponto conectado: 82
Velocidade de transmissao: 1 Gbps

Identificador do ponto: 26
Nome do ponto: Americas Light
Pais de localizacao: Brazil
Sigla do pais: BR

Identificador do ponto: 73
Pais de localizacao: Germany
Identificador do ponto conectado: 85
Velocidade de transmissao: 1 Gbps

Numero de paginas de disco: 1
```

### Select Where
Imprime todos os registros que contém o(s) campo(s) especificado(s).
```
3 [nome-arquivo-entrada-bin] [quantidade-de-filtros]
[campo1] [valor1]
[...] [...]
```
Exemplo de entrada onde são filtrados os registros com campos velocidade == 3,
nomePoPs == "Zagreb" e siglaPais == "NL"

```
3 binario8.bin 3
velocidade 3
nomePoPs "Zagreb"
siglaPais "NL"
```
A saída é indicada para cada busca separadamente, os filtros não são simultâneos. Exemplo:
```
Busca 1
Identificador do ponto: 18
Nome do ponto: Porto Velho
Pais de localizacao: Brazil
Sigla do pais: BR
Identificador do ponto conectado: 19
Velocidade de transmissao: 3 Gbps

Identificador do ponto: 29
Nome do ponto: Belem
Pais de localizacao: Brazil
Sigla do pais: BR
Identificador do ponto conectado: 30
Velocidade de transmissao: 3 Gbps

Numero de paginas de disco: 6

Busca 2
Identificador do ponto: 96
Nome do ponto: Zagreb
Pais de localizacao: Croatia
Sigla do pais: HR
Identificador do ponto conectado: 97
Velocidade de transmissao: 5 Gbps

Numero de paginas de disco: 6

Busca 3
Registro inexistente.

Numero de paginas de disco: 6

```

### Delete
Remove todos os registros que possuem o campo especifícado na entrada. Mesmo formato de entrada da função 3 (Select Where).
```
4 [nome-arquivo-entrada-bin] [quantidade-de-filtros]
[campo1] [valor1]
[...] [...]
```
A saída é o valor da função binarioNaTela, cedida pela professora para correção do trabalho.

### Insert Into
Insere um registro novo no arquivo no lugar de um logicamente removido ou no final do arquivo, caso não haja.
```
5 [nome-arquivo-entrada-bin] [quantidade-de-registros]
[idConecta] "[nomePoPs]" "[nomePais]" "[siglaPais]" [idPoPsConectado] "[medidaVelocidade]" [velocidade]
...
```
Caso não deseje inserir algum dos valores, colocar *NULO*, sem aspas no lugar.
A saída é o valor da função binarioNaTela, cedida pela professora para correção do trabalho.

### Compaction
Compacta o arquivo removendo os registros excluidos logicamente.
```
6 [nome-arquivo-entrada-bin]
```
A saída é o valor da função binarioNaTela, cedida pela professora para correção do trabalho.


## Rodando localmente

Clone o projeto

```bash
  git clone https://github.com/totoi690/data-structures-iii
```

Entre no diretório do projeto

```bash
  cd ./trabalho2
```

Compile o código

```bash
  make compile
```

Rode o código

```bash
  make run
```


## Autores

- [@totoi690](https://www.github.com/totoi690)
- [@pdrtorrente](https://www.github.com/pdrtorrente)


## Licença

[MIT](https://choosealicense.com/licenses/mit/)

