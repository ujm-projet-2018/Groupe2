#include <stdio.h>
#include <stdlib.h>
#include "../inc/createSheet.h"



int main (int argc,char* argv[]){

	FILE* input = NULL;
	FILE* output = NULL;

	input=fopen(argv[1],"r");
	if(input == NULL){
		fprintf(stderr,"Le fichier que vous essayez d'ouvrir n'existe pas\n");
		exit(-1);
	}

	output =  fopen("partition.ly","w");
	if(output == NULL){
		fprintf(stderr,"Le fichier partition.ly n'a pas reussi a etre cree\n");
		exit(-1);
	}

	init_ly(output);
	
	lire_ecrire(input, output);
	
	fprintf(output, "}");
	fclose(input);
	fclose(output);

}
