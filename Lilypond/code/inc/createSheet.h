#ifndef CREATE_SHEET_H
#define CREATE_SHEET_H

#include <stdio.h>
#include <stdlib.h>
#include "math.h"

int nb_notes_total;
int temps;
int chiffrage;
char type_gamme;
char clef_partition;
int gamme_note_precedente;

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

void decoupage_avant_barre(double temps, int metrique, char* ecriture, FILE* output);
void decoupage_apres_barre(double temps, int metrique, char* ecriture, FILE* output);

void decoupage_avant_barre_accord (double temps, int metrique, int ligne, FILE* output, int accord, int** tableau_notes);
void decoupage_apres_barre_accord (double temps, int metrique, int ligne, FILE* output, int accord, int** tableau_notes);
int** lire_remplir(char* name);

int ecrire_accord_lie(int ligne, FILE* output, int** tableau_notes, int snb_notes_total, int accord, int duree);

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

/*
Fonction : int* remplir_tab_chercher_gamme(int** tableau_notes);
Rempli un tableau servant a deduire la gamme de la partition
*/
int* remplir_tab_chercher_gamme(int** tableau_notes);

/*
Fonction : int chercher_gamme_diese(int notes_tonalites[30][14],int* tab_chercher_gamme);
Input :
** notes_tonalites[30][14] : tableau contenant la contenance des differences gammes
** tab_chercher_gamme : tableau des notes de la partitions dont on doit trouver la gamme

Renvoie le numero de la gamme trouver par rapport a tab_chercher_gamme
*/
int chercher_gamme_diese(int notes_tonalites[30][14],int* tab_chercher_gamme);

/*
Fonction : int chercher_gamme_bemol(int notes_tonalites[30][14],int* tab_chercher_gamme);
Input :
** notes_tonalites[30][14] : tableau contenant la contenance des differences gammes
** tab_chercher_gamme : tableau des notes de la partitions dont on doit trouver la gamme

Renvoie le numero de la gamme trouver par rapport a tab_chercher_gamme
*/
int chercher_gamme_bemol(int notes_tonalites[30][14],int* tab_chercher_gamme);

/*
Fonction : void reconnaissance_gamme(int** tableau_notes,FILE* output);
Input : 
** int** tableau_notes : tableau contenant les informations de toutes les notes jouees

On cherche a reconnaitre la tonalite d'un morceau joue*/
void reconnaissance_gamme(int** tableau_notes,FILE* output);


/*
Fonction : char* retourne_ecriture(int** tableau_notes,int i);
Input : 
regarde la tableau a un indice i et retourne une chaine de caractere contenant les informations de la notes en lilypond*/
char* retourne_ecriture(int** tableau_notes,int i);


/*
Fonction : void modification_tableau_note_bemol(int** tableau_notes,int indice_gamme,int notes_tonalites[30][14]);
Input : */
void modification_tableau_note_bemol(int** tableau_notes,int indice_gamme,int notes_tonalites[30][14]);


/*
Fonction : void modification_tableau_note_diese(int** tableau_notes,int indice_gamme,int notes_tonalites[30][14]);
Input : */
void modification_tableau_note_diese(int** tableau_notes,int indice_gamme,int notes_tonalites[30][14]);


/*
Fonction : int ecrire_accord(int ligne, FILE* output, int** tableau_notes, int nb_notes_total, int accord, int duree);
Input : */
int ecrire_accord(int ligne, FILE* output, int** tableau_notes, int nb_notes_total, int accord, int duree);


/*
Fonction : trie_note(int** tab_chercher_gamme ,int** minima_gamme);
Input : 
on modifie/creer minima_gamme qui sera une gamme compose des notes les plus jouees de tab_chercher_gamme*/
void trie_note(int* tab_chercher_gamme ,int* minima_gamme);

void trie(int indice_note_new,int indice_insertion,int tab_chercher_gamme_trier[7]);
#endif
