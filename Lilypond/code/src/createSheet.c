#include "../inc/createSheet.h"
#include "string.h"

void init_ly(FILE* f){
    fprintf(f, "\\version \"2.18.2\" \n");
    fprintf(f, "\\language \"english\"\n"); 
    fprintf(f, "\\relative c'' {\n"); 
}


void lire_ecrire(FILE* input, FILE* output){
    int i;
    char note;
    int duree;
    int metrique, temps, nb_notes;
    double temps_restant;
    double temps_avant;

    fscanf(input, "%d\n", &nb_notes);
    fscanf(input, "%d %d\n", &temps, &metrique);
    fprintf(output, "\\time %d/%d\n", temps, metrique);
        
    temps_restant = temps;

    for (i = 0; i < nb_notes; i++) {
        fscanf(input, "%c %d\n", &note, &duree); 

        temps_restant -= (double)metrique/duree;                
        
        if (temps_restant < 0) {
             if (fabs(temps_restant) == (double)metrique/(double)(duree*2)) {
                fprintf(output, "%c%d( %c%d)\n", note, duree*2, note, duree*2);
                temps_restant = metrique + temps_restant;
             }
             else {
                 /*
                  Doit marcher en métrique 4/4
                  TODO Autres métriques
                */
                
                temps_avant = (double)metrique/duree + temps_restant;
                
                decoupage_avant_barre(temps_avant, metrique, note, output);
                decoupage_apres_barre(temps_restant, metrique, note, output);

                temps_restant = fabs(temps_restant);
                
                 /*
                   Découper la note si besoin puis liaison entre deux notes.
                   Découpage : récupérer la note qu'on est en train de traiter, prendre son temps et le diviser par 2.
                 */
              }
            //Il faut remettre le temps restant à sa bonne valeur
            temps_restant = fabs(temps_restant);
            
            /*
              deduire_armure
              remplir tableau des 12 notes possibles selon sorti pour déduire armure ??
            */
        }
        else if (temps_restant == 0) {
            fprintf(output, "%c%d\n", note, duree);
            temps_restant = temps;
        }
        else if (temps_restant > 0) {
            fprintf(output, "%c%d ", note, duree);
        } 
    }
}

void decoupage_avant_barre (double temps, int metrique, char note, FILE* output) {
    int i = 0;

    double temps_point_note_courante;
    double temps_note_courante;
    double temps_avb = temps;
 
    while (temps_avb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);
        
        if (temps_note_courante == temps_avb) {
            fprintf(output, "%c%d( ",note, (int)pow(2, i));
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
                fprintf(output, "%c%d.( ", note, (int)pow(2, i));
                break;
            }
            else if (temps_point_note_courante < temps_avb) {
                temps_avb -= temps_point_note_courante;
                fprintf(output, "%c%d.( ", note, (int)pow(2, i));
                decoupage_apres_barre(temps_avb, metrique, note, output);
            }
        }
    }
}

void decoupage_apres_barre (double temps, int metrique, char note, FILE* output) {
    int i = 0;

    double temps_point_note_courante;
    double temps_note_courante;
    double temps_apb = fabs(temps);

    while (temps_apb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);

        if (temps_note_courante == temps_apb) {
            fprintf(output, "%c%d) ",note, (int)pow(2, i));
            break;
        }
        else if (temps_note_courante > temps_apb) {
            i++;
        }
        else {
            fprintf(output, "%c%d", note, (int)pow(2, i));
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
                decoupage_apres_barre(temps_apb, metrique, note, output);
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
	char noms_tonalites2[30][4]={"cM","am","gM","em","dM","bm","aM","fsm","eM","csm","dM","gsm","fsM","dsm","csM","asm","fM","dm","bfM","gm","efM","cm","afM","fm","dfM","bfm","gfM","efm","cfM","afm"};//Tableau contenant le nom des tonalités (pour savoir si elles sont en dièses ou en bémol le lien se fait avec le second tableau
	
	int i,j=0;
	int notes_tonalites2[30][14] = {{0,0,1,0,1,0,1,1,0,1,0,1,0,1},{1,0,1,0,1,0,1,1,0,1,0,1,0,1},{},{},{},{},{},{},{},{},{},{},{},{},{14,0,1,0,1,0,1,1,0,1,0,1,0,1},{},{},{},{},{},{},{},{},{},{},{},{},{},{} ,{}};// Une ligne du tableau type : indiceNomGamme nombreAltérations do do# re re# etc.... avec 0 si non présent, 1 si présent. (gamme majeur et gamme mineur naturelle)
	// on se sert de ces gammes pour construire les autres

	for(i=0;i<30;i++){// on copie la tableau local a celui passe en parametre
		for(j=0;j<4;j++){
			noms_tonalites[i][j] = noms_tonalites2[i][j];
		}
	}
	j=0;
	for(i=2;i<=14;i+=2){

		
		copie_tableau_notes_tonalites(notes_tonalites2,i,i-2,14);
		notes_tonalites2[i][0]+=2;//On met affilie au nom de sa gamme
		notes_tonalites2[i][1]+=1;//On met à jour le nombre d'alteration
		notes_tonalites2[i][diese[j]] = 0;// permet de creer la gamme par rapport a la precedente
		notes_tonalites2[i][diese[j+1]] = 1;
	
		copie_tableau_notes_tonalites(notes_tonalites2,i+1,i,14);
		notes_tonalites2[i+1][0]+=1;// on augmente pour donner le nom de la relatif mineur

		copie_tableau_notes_tonalites(notes_tonalites2,i+14,i+12,14);
		notes_tonalites2[i+14][0]+=2;
		notes_tonalites2[i+14][1]+=1;
		notes_tonalites2[i+14][bemol[j]] = 0;
		notes_tonalites2[i+14][bemol[j+1]] = 1;

		copie_tableau_notes_tonalites(notes_tonalites2,i+15,i+14,14);
		notes_tonalites2[i+15][0]+=1;
		j+=2;//permet de se deplacer dans le tableau des bemols et des dièses pour la modification suivante
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

	
