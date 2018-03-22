#ifndef CREATE_SHEET_H
#define CREATE_SHEET_H

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

/*
Fonction : void decoupage_liaison(char notes, int metrique, int temps, int temps_restant ,FILE* output);
Permet de découper les notes en liaisons.
char notes: note en cours a découper
int métrique : temps de référence
int temps : nombre de temps dans un mesure
int temps_restant : temps restant dans la mesures actuelle
FILE* output : fichier d'écriture de la partition

*/
void decoupage_liaison(char notes, int metrique, int temps, int temps_restant ,FILE* output);

#endif
