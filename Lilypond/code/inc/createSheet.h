#ifndef CREATE_SHEET_H
#define CREATE_SHEET_H

#include <stdio.h>
#include <stdlib.h>
#include "math.h"

int nb_notes_total;
int temps;
int chiffrage;

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

void lire_ecrire(FILE* input, FILE* output);
*/
void ecrire(int** tableau_notes, FILE* output);

void decoupage_avant_barre(double temps, int metrique, int note, FILE* output);
void decoupage_apres_barre(double temps, int metrique, int note, FILE* output);

void decoupage_avant_barre_accord (double temps, int metrique, int ligne, FILE* output, int accord, int** tableau_notes);
void decoupage_apres_barre_accord (double temps, int metrique, int ligne, FILE* output, int accord, int** tableau_notes);
int** lire_remplir(char* name);

int ecrire_accord_lie(int ligne, FILE* output, int** tableau_notes, int nb_notes_total, int accord, int duree);
void ecrire_notes(int** tableau_notes, FILE* output, int nb_notes_total);

#endif
