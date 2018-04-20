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

void decoupage_avant_barre(double temps, int metrique, char note, FILE* output);
void decoupage_apres_barre(double temps, int metrique, char note, FILE* output);
int** lire_remplir(char* name);
#endif
