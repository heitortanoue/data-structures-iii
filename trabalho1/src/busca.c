#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/orgarquivos.h"
#include "../headers/registro.h"
#include "../headers/csv.h"
#include "../headers/busca.h"

//Trata os filtros identificando o campo e o criterio
int trataFiltros(Busca *filtros, char *campo, char **criterios){
	filtros -> campo = campo;
	filtros -> criterios = criterios;
	filtros -> pagDisco = 0;
	
	if(!strcmp(campo, "idConecta")) filtros -> tipo_campo = 0;
	if(!strcmp(campo, "nomePoPs")) filtros -> tipo_campo = 1;
	if(!strcmp(campo, "nomePais")) filtros -> tipo_campo = 2;
	if(!strcmp(campo, "siglaPais")) filtros -> tipo_campo = 3;
	if(!strcmp(campo, "idPoPsConectado")) filtros -> tipo_campo = 4;
	if(!strcmp(campo, "velocidade")) filtros -> tipo_campo = 5;

	return SUCESSO;
}

int testaRegistro (Registro reg, Busca *filtro, int numFiltro){
	int valido = 0;
	if (registroRemovido(&reg)){
		return 0;
	} else {
		switch (filtro->tipo_campo)
		{
		case 0:
			if (reg.idConecta == atoi(filtro->criterios[numFiltro])) valido = 1;
			break;
		case 1:
			if (!strcmp(reg.nomePoPs, filtro->criterios[numFiltro])) valido = 1;
			break;
		case 2:
			if (!strcmp(reg.nomePais, filtro->criterios[numFiltro])) valido = 1;
			break;
		case 3:
			if (!strcmp(reg.siglaPais, filtro->criterios[numFiltro])) valido = 1;
			break;
		case 4:
			if (reg.idPoPsConectado == atoi(filtro->criterios[numFiltro])) valido = 1;
			break;
		case 5:
			if (reg.velocidade == atoi(filtro->criterios[numFiltro])) valido = 1;
			break;
		default:
			break;
		}

		return valido;
	}
}