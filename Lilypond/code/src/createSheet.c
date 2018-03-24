#include "../inc/createSheet.h"

void init_ly(FILE* f){
    fprintf(f, "\\version \"2.18.2\" { \n");
    fprintf(f, "\\language \"english\"\n"); 
}


void lire_ecrire(FILE* input, FILE* output){
    int i;
    char note;
    int duree;
    int metrique, temps, nb_notes;
    double temps_restant;

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

                decoupage_avant_barre(temps_restant, metrique, note, output);
                decoupage_apres_barre(temps_restant, metrique, note, output);

                temps_restant = metrique - fabs(temps_restant);
                
                 /*
                   Découper la note si besoin puis liaison entre deux notes.
                   Découpage : récupérer la note qu'on est en train de traiter, prendre son temps et le diviser par 2.
                 */
            }
            //Il faut remettre le temps restant à sa bonne valeur
            temps_restant = abs(temps_restant);
            
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
    double temps_avb = metrique + temps;
 
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
                fprintf(output, "%c%d.(", note, (int)pow(2, i));
                break;
            }
            else if (temps_point_note_courante < temps_avb) {
                temps_avb -= temps_point_note_courante;
                fprintf(output, "%c%d.(", note, (int)pow(2, i));
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
            fprintf(output, "%c%d)",note, (int)pow(2, i));
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
                fprintf(output, ".)");
                break;
            }
            else if (temps_point_note_courante < temps_apb) {
                temps_apb -= temps_point_note_courante;
                fprintf(output, ".)(");
                decoupage_apres_barre(temps_apb, metrique, note, output);
            }
        }
    }
}