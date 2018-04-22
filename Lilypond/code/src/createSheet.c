#include "../inc/createSheet.h"
#include "string.h"

void init_ly(FILE* f){
    fprintf(f, "\\version \"2.18.2\" \n");
    fprintf(f, "\\language \"english\"\n"); 
    fprintf(f, "\\relative c'' {\n"); 
}

void ecrire(int** tableau_notes, FILE* output){
    int i;

    int duree;
    
    double temps_restant;
    double temps_avant;
    char* ecriture;
        /*
	tableau_notes[i][0] = note;
        tableau_notes[i][1] = octave;
        tableau_notes[i][2] = alteration;
        tableau_notes[i][3] = duree;
        tableau_notes[i][4] = accord;*/
    temps_restant = temps;

    for (i = 0; i < nb_notes_total; i++) {
        duree = tableau_notes[i][3];
        
        temps_restant -= (double)chiffrage/duree;   
 	ecriture = retourne_ecriture(tableau_notes,i);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
      
        if (temps_restant < 0) {
             if (fabs(temps_restant) == (double)chiffrage/(double)(duree*2)) {
                 if (tableau_notes[i][4] == 0) { fprintf(stderr,"%s",ecriture);
                     fprintf(output, "%s%d( %s%d)\n", ecriture, duree*2, ecriture, duree*2);
                 }
                 else {
                     i = ecrire_accord_lie(i, output, tableau_notes, nb_notes_total, tableau_notes[i][4], duree*2);
                 }
                 temps_restant = chiffrage + temps_restant;
             }
             else {
                 
                // Doit marcher en métrique 4/4
                // TODO Autres métriques
                                
                temps_avant = (double)chiffrage/duree + temps_restant;
                
                if (tableau_notes[i][4] == 0) {
                    decoupage_avant_barre(temps_avant, chiffrage, ecriture, output);
                    decoupage_apres_barre(temps_restant, chiffrage, ecriture, output);
                }
                else {
                    // Découpe d'un accord
                    decoupage_avant_barre_accord(temps_avant, chiffrage, i, output, tableau_notes[i][4], tableau_notes);
                    decoupage_apres_barre_accord(temps_restant, chiffrage, i, output, tableau_notes[i][4], tableau_notes);
                }
                
                temps_restant = fabs(temps_restant);
                
                 
                // Découper la note si besoin puis liaison entre deux notes.
                // Découpage : récupérer la note qu'on est en train de traiter, prendre son temps et le diviser par 2.
                 
              }
            //Il faut remettre le temps restant à sa bonne valeur
            temps_restant = fabs(temps_restant);
            
            
            // deduire_armure
            // remplir tableau des 12 notes possibles selon sorti pour déduire armure ??
            
        }
        else if (temps_restant == 0) {
            if (tableau_notes[i][4] == 0) {
                fprintf(output, "%s%d ", ecriture, duree);
            }
            else {
                i = ecrire_accord(i, output, tableau_notes, nb_notes_total, tableau_notes[i][4], duree);
                //i = ecrire_accord_lie(i, output, tableau_notes, nb_notes_total, tableau_notes[i][4], duree);
            }
            //fprintf(output, "%c%d\n", note, duree);
            temps_restant = temps;
        }
        else if (temps_restant > 0) {
            if (tableau_notes[i][4] == 0) {
                fprintf(output, "%s%d ", ecriture, duree);
            }
            else {
                i = ecrire_accord(i, output, tableau_notes, nb_notes_total, tableau_notes[i][4], duree);
                //i = ecrire_accord_lie(i, output, tableau_notes, nb_notes_total, tableau_notes[i][4], duree);
            }
            //fprintf(output, "%c%d ", note, duree);
        } 
    }
}

void decoupage_avant_barre (double temps, int metrique, char* ecriture, FILE* output) {
    int i = 0;

    double temps_point_note_courante;
    double temps_note_courante;
    double temps_avb = temps;
 
    while (temps_avb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);
        
        if (temps_note_courante == temps_avb) {
            fprintf(output, "%s%d( ",ecriture, (int)pow(2, i));
            break;
        }
        else if (temps_note_courante > temps_avb) {
            i++;
        }
        else {
            temps_point_note_courante = temps_note_courante/2.0;
            temps_avb -= temps_note_courante;
            
            if (temps_point_note_courante == temps_avb) {
                temps_avb -= temps_point_note_courante;
                fprintf(output, "%s%d.( ", ecriture, (int)pow(2, i));
                break;
            }
            else if (temps_point_note_courante < temps_avb) {
                temps_avb -= temps_point_note_courante;
                fprintf(output, "%s%d.( ", ecriture, (int)pow(2, i));
                decoupage_apres_barre(temps_avb, metrique, ecriture, output);
            }
        }
    }
}

void decoupage_apres_barre (double temps, int metrique, char* ecriture, FILE* output) {
    int i = 0;

    double temps_point_note_courante;
    double temps_note_courante;
    double temps_apb = fabs(temps);

    while (temps_apb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);

        if (temps_note_courante == temps_apb) {
            fprintf(output, "%s%d) ",ecriture, (int)pow(2, i));
            break;
        }
        else if (temps_note_courante > temps_apb) {
            i++;
        }
        else {
            fprintf(output, "%s%d", ecriture, (int)pow(2, i));
            temps_point_note_courante = temps_note_courante/2.0;
            temps_apb -= temps_note_courante;
            
            if (temps_point_note_courante == temps_apb) {
                temps_apb -= temps_point_note_courante;
                fprintf(output, ".) ");
                break;
            }
            else if (temps_point_note_courante < temps_apb) {
                temps_apb -= temps_point_note_courante;
                fprintf(output, ".) (");
                decoupage_apres_barre(temps_apb, metrique, ecriture, output);
            }
        }
    }
}


void copie_tableau_notes_tonalites(int tab1[30][14],int indice1,int indice2 ,int taille){
	int i;
	for(i=0;i<taille;i++){
		tab1[indice1][i] = tab1[indice2][i];
	}
}


void initialisation_tableau_gamme(char noms_tonalites[30][4],int notes_tonalites[30][14]){
	int diese[14]={7,8,2,3,9,10,4,5,11,12,6,7,13,2};//
	int bemol[14]={13,12,6,5,11,10,4,3,9,8,2,13,7,6};
	char noms_tonalites2[30][4]={"cM","am","gM","em","dM","bm","aM","fms","eM","cms","dM","gms","fMs","dms","cMs","ams","fM","dm","bMf","gm","eMf","cm","aMf","fm","dMf","bmf","gMf","emf","cMf","amf"};//Tableau contenant le nom des tonalités (pour savoir si elles sont en dièses ou en bémol le lien se fait avec le second tableau
	
	int i,j=0;
	int notes_tonalites2[30][14] = {{0,0,1,0,1,0,1,1,0,1,0,1,0,1},{1,0,1,0,1,0,1,1,0,1,0,1,0,1},{},{},{},{},{},{},{},{},{},{},{},{},{14,0,1,0,1,0,1,1,0,1,0,1,0,1},{},{},{},{},{},{},{},{},{},{},{},{},{},{} ,{}};// Une ligne du tableau type : indiceNomGamme nombreAltérations do do# re re# etc.... avec 0 si non présent, 1 si présent. (gamme majeur et gamme mineur naturelle)
	   // on se sert de ces gammes pour construire les autres

	for(i=0;i<30;i++){                        // on copie la tableau local a celui passe en parametre
		for(j=0;j<4;j++){
			noms_tonalites[i][j] = noms_tonalites2[i][j];
		}
	}
	j=0;
	for(i=2;i<=14;i+=2){

		/*Gammes avec des dieses*/
		copie_tableau_notes_tonalites(notes_tonalites2,i,i-2,14);
		notes_tonalites2[i][0]+=2;         //On met affilie au nom de sa gamme
		notes_tonalites2[i][1]+=1;         //On met à jour le nombre d'alteration
		notes_tonalites2[i][diese[j]] = 0; // permet de creer la gamme par rapport a la precedente
		notes_tonalites2[i][diese[j+1]] = 1;
	
		copie_tableau_notes_tonalites(notes_tonalites2,i+1,i,14);
		notes_tonalites2[i+1][0]+=1;       // on augmente pour donner le nom de la relatif mineur

		/*Gammes avec des bemols*/
		copie_tableau_notes_tonalites(notes_tonalites2,i+14,i+12,14);
		notes_tonalites2[i+14][0]+=2;
		notes_tonalites2[i+14][1]+=1;
		notes_tonalites2[i+14][bemol[j]] = 0;
		notes_tonalites2[i+14][bemol[j+1]] = 1;

		copie_tableau_notes_tonalites(notes_tonalites2,i+15,i+14,14);
		notes_tonalites2[i+15][0]+=1;
		j+=2;                             //permet de se deplacer dans le tableau des bemols et des dièses pour la modification suivante
	}	
	for(i=0;i<30;i++){
		for(j=0;j<14;j++){
			notes_tonalites[i][j] = notes_tonalites2[i][j];
		}
	}
}

void afficher_gamme_et_noms(char noms_tonalites[30][4],int notes_tonalites[30][14]){
	int i,j;
	for(i=0;i<30;i++){
		for(j=0;j<14;j++){
			fprintf(stderr,"%d, ", notes_tonalites[i][j]);

		}
		fprintf(stderr,"\n");
	}
	for(i=0;i<30;i++){
		for(j=0;j<4;j++){
			fprintf(stderr,"%c, ", noms_tonalites[i][j]);

		}
		fprintf(stderr,"\n");
	}
}

	
void decoupage_avant_barre_accord (double temps, int metrique, int ligne, FILE* output, int accord, int** tableau_notes) {
    int i = 0;
    int j;
    
    double temps_point_note_courante;
    double temps_note_courante;
    double temps_avb = temps;
    char* ecriture;
 
    while (temps_avb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);
        
        if (temps_note_courante == temps_avb) {
	    ecriture = retourne_ecriture(tableau_notes,ligne);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
            fprintf(output, "<%s ", ecriture);
    
            for (j = ligne + 1; j < nb_notes_total; j++) {
                if (tableau_notes[j][4] == accord) {
		    ecriture = retourne_ecriture(tableau_notes,j);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
                    fprintf(output, "%s ", ecriture);
                }
                else {
                    break;
                }
            }
            fprintf(output, ">%d( ", (int)pow(2, i));
            
            break;
        }
        else if (temps_note_courante > temps_avb) {
            i++;
        }
        else {
            temps_point_note_courante = temps_note_courante/2.0;
            temps_avb -= temps_note_courante;
            
            if (temps_point_note_courante == temps_avb) {
                temps_avb -= temps_point_note_courante;
		ecriture = retourne_ecriture(tableau_notes,ligne);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
                fprintf(output, "<%s ", ecriture);
    
                for (j = ligne + 1; j < nb_notes_total; j++) {
                    if (tableau_notes[j][4] == accord) {
			ecriture = retourne_ecriture(tableau_notes,j);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
                        fprintf(output, "%s ", ecriture);
                    }
                    else {
                        break;
                    }
                }
                fprintf(output, ">%d.(", (int)pow(2, i));
                
                break;
            }
            else if (temps_point_note_courante < temps_avb) {
                temps_avb -= temps_point_note_courante;
		ecriture = retourne_ecriture(tableau_notes,ligne);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
                fprintf(output, "<%s ", ecriture);
    
                for (j = ligne + 1; j < nb_notes_total; j++) {
                    if (tableau_notes[j][4] == accord) {
			ecriture = retourne_ecriture(tableau_notes,j);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
                        fprintf(output, "%s ", ecriture);
                    }
                    else {
                        break;
                    }
                }
                fprintf(output, ">%d.(", (int)pow(2, i));
                
                decoupage_apres_barre_accord(temps_avb, metrique, ligne, output, accord, tableau_notes);
            }
        }
    }
}

void decoupage_apres_barre_accord (double temps, int metrique, int ligne, FILE* output, int accord, int** tableau_notes) {
    int i = 0;
    int j;

    double temps_point_note_courante;
    double temps_note_courante;
    double temps_apb = fabs(temps);
    char* ecriture;

    while (temps_apb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);
	ecriture = retourne_ecriture(tableau_notes,ligne);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
        if (temps_note_courante == temps_apb) {
            fprintf(output, "<%s ", ecriture);
    
            for (j = ligne + 1; j < nb_notes_total; j++) {
                if (tableau_notes[j][4] == accord) {
		    ecriture = retourne_ecriture(tableau_notes,j);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
                    fprintf(output, "%s ", ecriture);
                }
                else {
                    break;
                }
            }
            fprintf(output, ">%d)", (int)pow(2, i));
            break;
        }
        else if (temps_note_courante > temps_apb) {
            i++;
        }
        else {
            fprintf(output, "<%s ", ecriture);
            
            for (j = ligne + 1; j < nb_notes_total; j++) {
                if (tableau_notes[j][4] == accord) {
		    ecriture = retourne_ecriture(tableau_notes,j);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
                    fprintf(output, "%s ", ecriture);
                }
                else {
                    break;
                }
            }
            fprintf(output, ">%d", (int)pow(2, i));
            
            temps_point_note_courante = temps_note_courante/2.0;
            temps_apb -= temps_note_courante;
            
            if (temps_point_note_courante == temps_apb) {
                temps_apb -= temps_point_note_courante;
                fprintf(output, ".) ");
                break;
            }
            else if (temps_point_note_courante < temps_apb) {
                temps_apb -= temps_point_note_courante;
                fprintf(output, ".) (");
                decoupage_apres_barre_accord(temps_apb, metrique, ligne, output, accord, tableau_notes);
            }
        }
    }
}

int** lire_remplir(char* name) {
    int i;

    FILE* input = NULL;

    int** tableau_notes;
    
    int octave, alteration, duree, accord;
    char note;
    
    input = fopen(name, "r");
    if(input == NULL){
        fprintf(stderr,"Le fichier que vous essayez d'ouvrir n'existe pas\n");
        exit(-1);
    }

    fscanf(input, "%d\n", &nb_notes_total);
    fscanf(input, "%d %d\n", &temps, &chiffrage);

    tableau_notes = malloc(nb_notes_total * sizeof(int*));

    for (i = 0; i < nb_notes_total; i++) {
        tableau_notes[i] = malloc(sizeof(int) * 5);
    }
   
    for (i = 0; i < nb_notes_total; i++) {
        fscanf(input, "%c %d %d %d %d\n", &note, &octave, &alteration, &duree, &accord);
        
        tableau_notes[i][0] = note;
        tableau_notes[i][1] = octave;
        tableau_notes[i][2] = alteration;
        tableau_notes[i][3] = duree;
        tableau_notes[i][4] = accord;
    }

    fclose(input);
    
    return tableau_notes;
}

int ecrire_accord_lie(int ligne, FILE* output, int** tableau_notes, int nb_notes_total, int accord, int duree) {
    int i;
    char* ecriture ;
    ecriture = retourne_ecriture(tableau_notes,ligne);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
      
    fprintf(output, "<%s ", ecriture);
    
    for (i = ligne + 1; i < nb_notes_total; i++) {
        if (tableau_notes[i][4] == accord) {
	    ecriture = retourne_ecriture(tableau_notes,i);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
            fprintf(output, "%s ", ecriture);
        }
        else {
            break;
        }
    }
    fprintf(output, ">%d (", duree);

    ecriture = retourne_ecriture(tableau_notes,ligne);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
    fprintf(output, "<%s ", ecriture);
    
    for (i = ligne + 1; i < nb_notes_total; i++) {
        if (tableau_notes[i][4] == accord) {
	    ecriture = retourne_ecriture(tableau_notes,i);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
            fprintf(output, "%s ", ecriture);
        }
        else {
            break;
        }
    }
    fprintf(output, ">%d)\n", duree);

    return i-1;
}

int* remplir_tab_chercher_gamme(int** tableau_notes){
	int i=0 , alteration;
	char note;
        int * tab_chercher_gamme = malloc(14 * sizeof(int));//tableau type : indiceNomGamme nombreAltérations do do# re re# etc.... avec 0 si non présent, 1 si présent. (gamme majeur et gamme mineur naturelle)

	for(i=0;i<14;i++){
		tab_chercher_gamme[i] =0;
	}
	
	i=0;

	while(i<nb_notes_total){//On rempli le tableau : tab_chercher_gamme 

		note = tableau_notes[i][0];//On recupere la note courant
		alteration = tableau_notes[i][2];//On recupere l'info sur la presence d'alteration

		/*On rempli le tableau des notes presente de la partition qui servira a determiner sa gamme*/

		if(alteration ==0){//Si il n'y a pas d'alteration
			switch(note){
				case 'c':
					tab_chercher_gamme[2]+=1;
				break;
				case 'd':
					tab_chercher_gamme[4]+=1;
				break;
				case 'e':
					tab_chercher_gamme[6]+=1;
				break;
				case 'f':
					tab_chercher_gamme[7]+=1;
				break;
				case 'g':
					tab_chercher_gamme[9]+=1;
				break;
				case 'a':
					tab_chercher_gamme[11]+=1;
				break;
				case 'b':
					tab_chercher_gamme[13]+=1;
				break;
			}
		}
		else{//Si il y a une alteration
			switch(note){
				case 'c':
					if(tab_chercher_gamme[3] == 0) tab_chercher_gamme[1]+=1;
					tab_chercher_gamme[3]+=1;
				break;
				case 'd':
					if(tab_chercher_gamme[5] == 0) tab_chercher_gamme[1]+=1;
					tab_chercher_gamme[5]+=1;
				break;
				case 'f':
					if(tab_chercher_gamme[8] == 0) tab_chercher_gamme[1]+=1;
					tab_chercher_gamme[8]+=1;
				break;
				case 'g':
					if(tab_chercher_gamme[10] == 0) tab_chercher_gamme[1]+=1;
					tab_chercher_gamme[10]+=1;
				break;
				case 'a':
					if(tab_chercher_gamme[12] == 0) tab_chercher_gamme[1]+=1;
					tab_chercher_gamme[12]+=1;
				break;
			}			
		}
		
		i++;
	}
	return tab_chercher_gamme;
}

int chercher_gamme_diese(int notes_tonalites[30][14],int* tab_chercher_gamme){

	int verif_diese[7]={7,2,9,4,11,6,13};//contient les indices des notes non altere a verifier par rapport a l'ordre des dieses a verifier
	int non,indice_note,indice_note_A,j,i=0;
	while(i<=15){
		if(notes_tonalites[i][1]>=tab_chercher_gamme[1]){//On regarde si le nombre d'alteration est suffisant , sinon ne sert a rien de verifier si c'est la bonne
			j=0;
			non=0;
			while(j<7 && non != 1){//On verifie
				indice_note = verif_diese[j];// note non altere
				indice_note_A = verif_diese[j]+1; // note altere
				if(indice_note_A >= 13) indice_note_A = 2;//Si on sort des bornes

/*On regarde la presence de la note non altere et sa correspondante altere, si elle y sont en meme temps on n'est pas dans la gamme (sauf exception) OU si il y a la note non altere dans la gamme de reference, mais que celle de la partition est altere incompatibilite OU si il n'y a pas une note dans la gamme de reference , mais dans la partition il y a presence*/

				if(((notes_tonalites[i][indice_note] == 1 && tab_chercher_gamme[indice_note_A]>0)&& i<12)
				  ||(notes_tonalites[i][indice_note_A] ==0 && tab_chercher_gamme[indice_note_A] == 1)
				  ||(notes_tonalites[i][indice_note] ==0 && tab_chercher_gamme[indice_note] > 0)){

					non=1;
					i++;
				}
				else{
					j++;
				}
			}
			if(j==7 && non !=1){//Si on a passer toutes les notes de la gamme de reference sans rencontrer d'erreur
				return notes_tonalites[i][0]; // on retourne l'indice de nom de la gamme si trouver
			}
		}
		else{
			i++;
		}

	}
	return -1; //Quand aucune gamme avec une armure contenant des dieses n'a ete trouve
}


int chercher_gamme_bemol(int notes_tonalites[30][14],int* tab_chercher_gamme){
	
	int verif_bemol[7]={13,6,11,4,9,2,7}; //contient les indices des notes non altere a verifier par rapport a l'ordre des dieses a verifier
	int non,indice_note,indice_note_A,j,i=16;

	while(i<=29){
		if(notes_tonalites[i][1]>=tab_chercher_gamme[1]){//On regarde si le nombre d'alteration est suffisant , sinon ne sert a rien de verifier si c'est la bonne
			j=0;
			non=0;
			while(j<7 && non != 1){//On verifie
				indice_note = verif_bemol[j];// note non altere
				indice_note_A = verif_bemol[j]-1; // note altere
				if(indice_note_A == 12) indice_note_A = 2;
				if(indice_note_A == 1) indice_note_A =13;

/*On regarde la presence de la note non altere et sa correspondante altere, si elle y sont en meme temps on n'est pas dans la gamme (sauf exception) OU si il y a la note non altere dans la gamme de reference, mais que celle de la partition est altere incompatibilite OU si il n'y a pas une note dans la gamme de reference , mais dans la partition il y a presence*/

				if(((notes_tonalites[i][indice_note] == 1 && tab_chercher_gamme[indice_note_A]>0) && i>26)
				  ||(notes_tonalites[i][indice_note_A] ==0 && tab_chercher_gamme[indice_note_A] == 1)
				  ||(notes_tonalites[i][indice_note] ==0 && tab_chercher_gamme[indice_note] > 0)){
					non=1;
					i++;
				}
				else{
					j++;
				}
			}
			if(j==7 && non !=1){
				return notes_tonalites[i][0]; // on retourne l'indice de nom de la gamme si trouve
			}
		}
		else{
			i++;
		}

	}
	return -1; //Quand aucune gamme avec une armure contenant des dieses n'a ete trouver
}

void reconnaissance_gamme(int** tableau_notes,FILE* output){

	int* tab_chercher_gamme = remplir_tab_chercher_gamme(tableau_notes );
	char noms_tonalites[30][4];
	int notes_tonalites[30][14];
	int indice_gamme=0;
	char alt = ' ';//Sert si le nom de la gamme est diese ou bemol

	initialisation_tableau_gamme(noms_tonalites,notes_tonalites);//Creation des differentes gammes	

	afficher_gamme_et_noms(noms_tonalites,notes_tonalites);
	/*Affiche le tableau de la partition pour verification : */int i;
	for(i=0;i<14;i++){ fprintf(stderr,"%d ",tab_chercher_gamme[i]);}
	fprintf(stderr,"\n");

        /*On cherche la gamme*/
	indice_gamme = chercher_gamme_diese(notes_tonalites,tab_chercher_gamme);
	type_gamme ='s';//Variable globale servant pour plus tard pour ecrire les notes dans le fichier

	if(indice_gamme == -1){
		indice_gamme = chercher_gamme_bemol(notes_tonalites,tab_chercher_gamme);
		type_gamme ='f';
		if(indice_gamme == -1) {indice_gamme =0; type_gamme ='r';}
		else{ modification_tableau_note_bemol(tableau_notes,indice_gamme,notes_tonalites);}	
	}
	
	
	fprintf(stderr,"indice gamme %d\n",indice_gamme);
	
	/*On ecrit dans le fichier le nom de la gamme*/
	if(noms_tonalites[indice_gamme][2] == 'f' || noms_tonalites[indice_gamme][2] =='s'){
		alt = noms_tonalites[indice_gamme][2];
	}
	if(noms_tonalites[indice_gamme][1] == 'M'){
        	fprintf(output, "\\key %c%c \\major\n",noms_tonalites[indice_gamme][0] ,alt);
	}
	else{
        	fprintf(output, "\\key %c%c \\minor\n",noms_tonalites[indice_gamme][0] ,alt);
	}
	
}

void modification_tableau_note_bemol(int** tableau_notes,int indice_gamme,int notes_tonalites[30][14]){
	
	int i=0,alteration;
	char note;
	for(i=0;i<nb_notes_total;i++){
		note = tableau_notes[i][0];
		alteration = tableau_notes[i][2];
		switch(note){
			case 'a':
				if(notes_tonalites[indice_gamme][12]==1 && alteration == 1){
					tableau_notes[i][0]='b';
				}
			break;
			case 'd':
				if(notes_tonalites[indice_gamme][5]==1 && alteration == 1){
					tableau_notes[i][0]='e';
				}
			break;
			case 'g':
				if(notes_tonalites[indice_gamme][10]==1 && alteration == 1){
					tableau_notes[i][0]='a';
				}
			break;
			case 'c':
				if(notes_tonalites[indice_gamme][3]==1 && alteration == 1){
					tableau_notes[i][0]='d';
				}
			break;
			case 'f':
				if(notes_tonalites[indice_gamme][8]==1 && alteration == 1){
					tableau_notes[i][0]='g';
				}
			break;
			case 'b':
				if(notes_tonalites[indice_gamme][13]==1){
					tableau_notes[i][0]='c';
				}
			break;
			case 'e':
				if(notes_tonalites[indice_gamme][6]==1){
					tableau_notes[i][0]='f';
				}
			break;

		}
	}
	
}
char* retourne_ecriture(int** tableau_notes,int i){
	char* ecriture = malloc(5 * sizeof(char));
	ecriture[0]=tableau_notes[i][0];
	if(tableau_notes[i][2] == 1) {	
		if(type_gamme == 'r'){
			ecriture[1] = 's';
		}
		else{
			ecriture[1] = type_gamme;
   		}
		ecriture[2] ='\0';
	}      
	else{
		ecriture[1]='\0';
	} 
	return ecriture;

}
	
int ecrire_accord (int ligne, FILE* output, int** tableau_notes, int nb_notes_total, int accord, int duree) {
    int i;
    char* ecriture;

    ecriture = retourne_ecriture(tableau_notes,ligne);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))

    fprintf(output, "<%s ", ecriture);
    
    for (i = ligne + 1; i < nb_notes_total; i++) {
        if (tableau_notes[i][4] == accord) {
	    ecriture = retourne_ecriture(tableau_notes,i);//contient la note ainsi que son alteration si il y en a une (//TODO ajouter la hauteur))
            fprintf(output, "%s ", ecriture);
        }
        else {
            break;
        }
    }
    fprintf(output, ">%d ", duree);

    return i - 1;
}