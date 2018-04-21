#include <stdio.h>
#include <stdlib.h>
#include "../inc/createSheet.h"



int main (int argc,char* argv[]){
	FILE* input = NULL;
	FILE* output = NULL;

	int** tableau_notes;
	char noms_tonalites[30][4];
	int notes_tonalites[30][14];
	initialisation_tableau_gamme(noms_tonalites,notes_tonalites);
	
	afficher_gamme_et_noms(noms_tonalites,notes_tonalites);
	
   	 output =  fopen("partition.ly","w");
   	 if(output == NULL){
      	 	 fprintf(stderr,"Le fichier partition.ly n'a pas reussi a etre cree\n");
       		 exit(-1);
   	 }

   	 init_ly(output);
      
   	 tableau_notes = lire_remplir(argv[1]);
   	 ecrire(tableau_notes, output);
	 fprintf(output, "\n\\bar \"|.\"\n}");
	 fclose(input);
       	 fclose(output);
 

}
