#ifndef CREATE_SHEET_H
#define CREATE_SHEET_H

#include <stdio.h>
#include <stdlib.h>
#include "math.h"

/*
Fonction : void init_ly(FILE* f);
Permet d'initialiser les premiere lignes d'une partition Lilypond
FILE* f : fichier d'écriture de la partition
*/
void init_ly(FILE* f);

/*
Fonction : void lire_ecrire(FILE* input, FILE* output);
Ecris le fichier "input" en langage lilypond dans output
FILE* output : fichier d'écriture de la partition
*/
void lire_ecrire(FILE* input, FILE* output);

void decoupage_avant_barre (double temps, int metrique, char note, FILE* output);
void decoupage_apres_barre (double temps, int metrique, char note, FILE* output);

/*
Fonction : void initialisation_tableau_gamme();
Initialise deux tableaux . Un creeant  les noms des differentes gammes majeur et mineur , l'autre indiquant quelles notes y sont.
*/
void initialisation_tableau_gamme(char noms_tonalites[30][4],int notes_tonalites[30][14]);

/*
Fonction : void afficher_gamme_et_noms(char noms_tonalites[30][4],int notes_tonalites[30][14])
Affiche les tableaux construits par initialisation_tableau_gamme();
*/
void afficher_gamme_et_noms(char noms_tonalites[30][4],int notes_tonalites[30][14]);

#endif
