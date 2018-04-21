#include "../inc/createSheet.h"

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
        
    temps_restant = temps;

    for (i = 0; i < nb_notes_total; i++) {
        duree = tableau_notes[i][3];
        
        temps_restant -= (double)chiffrage/duree;                
        
        if (temps_restant < 0) {
             if (fabs(temps_restant) == (double)chiffrage/(double)(duree*2)) {
                 if (tableau_notes[i][4] == 0) {
                     fprintf(output, "%c%d( %c%d)\n", tableau_notes[i][0], duree*2, tableau_notes[i][0], duree*2);
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
                    decoupage_avant_barre(temps_avant, chiffrage, tableau_notes[i][0], output);
                    decoupage_apres_barre(temps_restant, chiffrage, tableau_notes[i][0], output);
                }
                else {
                    // Découpe d'un accord
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
                fprintf(output, "%c%d ", tableau_notes[i][0], duree);
            }
            else {
                i = ecrire_accord_lie(i, output, tableau_notes, nb_notes_total, tableau_notes[i][4], duree);
            }
            //fprintf(output, "%c%d\n", note, duree);
            temps_restant = temps;
        }
        else if (temps_restant > 0) {
            if (tableau_notes[i][4] == 0) {
                fprintf(output, "%c%d ", tableau_notes[i][0], duree);
            }
            else {
                i = ecrire_accord_lie(i, output, tableau_notes, nb_notes_total, tableau_notes[i][4], duree);
            }
            //fprintf(output, "%c%d ", note, duree);
        } 
    }
}

void decoupage_avant_barre (double temps, int metrique, int note, FILE* output) {
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

void decoupage_apres_barre (double temps, int metrique, int note, FILE* output) {
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

void decoupage_avant_barre_accord (double temps, int metrique, int ligne, FILE* output, int accord, int** tableau_notes) {
    int i = 0;
    int j;
    
    double temps_point_note_courante;
    double temps_note_courante;
    double temps_avb = temps;
 
    while (temps_avb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);
        
        if (temps_note_courante == temps_avb) {
            fprintf(output, "<%c ", tableau_notes[ligne][0]);
    
            for (j = ligne + 1; j < nb_notes_total; j++) {
                if (tableau_notes[j][4] == accord) {
                    fprintf(output, "%c ", tableau_notes[j][0]);
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
                fprintf(output, "<%c ", tableau_notes[ligne][0]);
    
                for (j = ligne + 1; j < nb_notes_total; j++) {
                    if (tableau_notes[j][4] == accord) {
                        fprintf(output, "%c ", tableau_notes[j][0]);
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
                fprintf(output, "<%c ", tableau_notes[ligne][0]);
    
                for (j = ligne + 1; j < nb_notes_total; j++) {
                    if (tableau_notes[j][4] == accord) {
                        fprintf(output, "%c ", tableau_notes[j][0]);
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

    while (temps_apb != 0 && i < 6) {
        temps_note_courante = metrique/pow(2, i);

        if (temps_note_courante == temps_apb) {
            fprintf(output, "<%c ", tableau_notes[ligne][0]);
    
            for (j = ligne + 1; j < nb_notes_total; j++) {
                if (tableau_notes[j][4] == accord) {
                    fprintf(output, "%c ", tableau_notes[j][0]);
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
            fprintf(output, "<%c ", tableau_notes[ligne][0]);
            
            for (j = ligne + 1; j < nb_notes_total; j++) {
                if (tableau_notes[j][4] == accord) {
                    fprintf(output, "%c ", tableau_notes[j][0]);
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
    
    fprintf(output, "<%c ", tableau_notes[ligne][0]);
    
    for (i = ligne + 1; i < nb_notes_total; i++) {
        if (tableau_notes[i][4] == accord) {
            fprintf(output, "%c ", tableau_notes[i][0]);
        }
        else {
            break;
        }
    }
    fprintf(output, ">%d (", duree);

    fprintf(output, "<%c ", tableau_notes[ligne][0]);
    
    for (i = ligne + 1; i < nb_notes_total; i++) {
        if (tableau_notes[i][4] == accord) {
            fprintf(output, "%c ", tableau_notes[i][0]);
        }
        else {
            break;
        }
    }
    fprintf(output, ">%d)\n", duree);

    return i;
}

void ecrire_notes(int** tableau_notes, FILE* output, int nb_notes_total) {
    int i, j;

    for (i = 0; i < nb_notes_total; i++) {
        if (tableau_notes[i][0] != 'r' && tableau_notes[i][4] != 0) {
            fprintf(output, "<%c ", tableau_notes[i][0]);

            for (j = i+1; j < nb_notes_total; j++) {
                if (tableau_notes[j][4] == tableau_notes[i][4]) {
                    fprintf(output, "%c ", tableau_notes[j][0]);
                }
                else {
                    i = j;
                    break;
                }
            }
            fprintf(output, ">%d ", tableau_notes[i][3]);
        }
        else {
            fprintf(output, "%c%d ", tableau_notes[i][0], tableau_notes[i][3]);
        }
    }
}