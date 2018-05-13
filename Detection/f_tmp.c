#include "f_tmp.h"

void ecrire_fichier(char *notes[],int n){

  int i,j;
  FILE* fichier = NULL;
  char note[10];
  char tmp_note[4];
  char tmp_octave [2];

  fichier = fopen("notes.txt", "w");

  sprintf(note,"%d\n",n);

  if (fichier != NULL){

    fputs(note, fichier);
    fputs("4 4\n", fichier);
    


  
    for(i = 0; i < n; i ++){

      for(j=0; notes[i][j] != 0x20;j++){
	tmp_note[j] = notes[i][j];
      }
      tmp_note[j] = '\0';
      j++;
      tmp_octave[0] = notes[i][j];
      tmp_octave[1] ='\0';
      if(!strcmp("do",tmp_note)){
	sprintf(note,"c 0 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("do#",tmp_note)){
	sprintf(note,"c 1 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("re",tmp_note)){
	sprintf(note,"d 0 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("re#",tmp_note)){

	sprintf(note,"d 1 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("mi",tmp_note)){
	sprintf(note,"e 0 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("fa",tmp_note)){
	sprintf(note,"f 0 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("fa#",tmp_note)){
	sprintf(note,"f 1 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("sol",tmp_note)){
	sprintf(note,"g 0 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("sol#",tmp_note)){

	sprintf(note,"g 1 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("la",tmp_note)){

	sprintf(note,"a 0 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("la#",tmp_note)){
	sprintf(note,"a 1 %s 4 0 0\n",tmp_octave);
      }
      else if(!strcmp("si",tmp_note)){
	sprintf(note,"b 0 %s 4 0 0\n",tmp_octave);
      }

      else{
	sprintf(note,"z 0 %s 4 0 0\n",tmp_octave);
      }
      fputs(note, fichier);
    }

    fclose(fichier);
  }
   
}
