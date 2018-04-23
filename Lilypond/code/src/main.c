#include <stdio.h>
#include <stdlib.h>
#include "../inc/createSheet.h"



int main (int argc,char* argv[]){
	FILE* output = NULL;

	int** tableau_notes;
	
	
   	 output =  fopen("./partition.ly","w");
   	 if(output == NULL){
      	 	 fprintf(stderr,"Le fichier partition.ly n'a pas reussi a etre cree\n");
       		 exit(-1);
   	 }

   	 init_ly(output);
      
   	 tableau_notes = lire_remplir(argv[1]);
	 reconnaissance_gamme(tableau_notes,output);
   	 ecrire(tableau_notes, output);
	 fprintf(output, "\n\\bar \"|.\"\n}");
         
       	 fclose(output);
}
