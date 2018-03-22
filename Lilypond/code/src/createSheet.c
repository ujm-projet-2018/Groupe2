#include <stdlib.h>
#include <stdio.h>
#include "math.h"
#include "../inc/createSheet.h"

void init_ly(FILE* f){
	fprintf(f, "\\version \"2.18.2\" { \n");
	fprintf(f, "\\language \"english\"\n"); 
}

void lire_ecrire(FILE* input, FILE* output){
	char notes;
	int metrique,temps;
	double temps_restant;

	fscanf(input, "%d %d", &temps,&metrique);
	fprintf(output, "\\time %d\\%d\n", temps, metrique);
	temps_restant=temps;
	do{
            fprintf(stderr,"**debuggage lire_ecrire\n");
            fscanf(input, "%c %d", &notes,&temps);
            temps_restant -= metrique/temps;
            if(temps_restant <0){
                if(temps_restant ==  2.0*temps){
                    fprintf(output, "%c%d( %c%d)\n",notes,temps*2, notes,temps*2);	
                }
                else {
                    decoupage_liaison(notes,metrique,temps,temps_restant,output);//devrait marcher pour 4/4 TODO revoir pour autre métriques
                    
                }
		
                //fprintf(output,"%c%d",notes,); Decouper puis liaison quand c'est inferieur recupere la note qu'on est en train de traiter prendre son temps la diviser par deux
            }
            //deduire_armure
            //remplir tableau des 12 notes possibles selon sorti pour déduire armure ??*/
            //fputc(int caractere, output);
            if(temps_restant ==0){
                fprintf(output, "\n");	
            }
            
	}while(notes != EOF );
        
        
}

void decoupage_liaison(char notes, int metrique, int temps, int temps_restant,FILE* output){
	//Premiere partie : le métrique/(temps restant ( négatif) + metrique) : 
	//Calcul la note de la fin de la premiere mesure	
	double temps_notes_seconde,temps_point_seconde;//temps_point = la moitié du temps qu'on vient de trouverœ
	int i=0;
	int tps_derniere_note = metrique/(temps_restant+metrique);
	fprintf(output, "%c%d",notes,tps_derniere_note);
	//Deuxieme partie : on va calculer la valeur absolu du temsp restant, on part de la ronde ( pour l'instant à voir lorsqu'on changera la métrique ,boucle sur puissance de 2 , où on metrique/pow(2,i), si supérieur. tant que temps restant pas egale à 0 on continue a diminuer
	temps_restant=abs(temps_restant);
	while(temps_restant!=0){
		temps_notes_seconde =metrique/pow(2,i);
		if(temps_notes_seconde < temps_restant){
			
			fprintf(output, "( %c%d)",notes,i);//TODO ajouter hauteur
			temps_restant -= temps_notes_seconde;
			temps_point_seconde= temps_notes_seconde/2;
			if(temps_restant - temps_point_seconde>0){
			  	fprintf(output, ".");
				temps_restant -= temps_point_seconde;	
			
			}
                        fprintf(output, ")");
			i++;
			
		}
		else{
			i++;
		}
	}	
}
