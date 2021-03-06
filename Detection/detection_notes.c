#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <unistd.h>

#include "analyse_notes.h"
#include "fft.h" 
#include "f_tmp.h" 


void recupere_mot(char mot[5], FILE* fich, int debug);
int lettre_valide(char lettre, int debug);
int* decoupage_signal(short* amplitudes, float* temps, int debut, int fin, int traitement_sup, int* nb_note);
int* analyse_periode(int* periodes, float* temps, int nb_periode, int* nb_note);
int* analyse_separation(int* periodes, float* temps, int nb_periode, int* nb_note);


void usage(char* s){   /* explique le fonctionnement du programme */
  fprintf(stderr,"Usage: %s [options] -f <fichier wav>\n",s);
  fprintf(stderr,"      -f <fichier> : permet de preciser le fichier a utiliser (obligatoire)\n");

  fprintf(stderr,"Options:\n");

  fprintf(stderr,"   Choisir les parametres suivants:\n");
  fprintf(stderr,"      -u : affiche l'usage du programme et quitte\n");
  fprintf(stderr,"      -v : active les informations du traitement sur le terminal\n");
  fprintf(stderr,"      -d : active les informations utiles pour le debugage du programme\n");
  fprintf(stderr,"   Definir la taille des parametres suivant:\n");
  fprintf(stderr,"      -e \"entier\" > 1 : pour choisir un echantillonage appliquer lors de la recuperation des donnees (divise le nombre de donnees recuperees)\n");
  fprintf(stderr,"      -n \"entier\" > 1024 : pour choisir le nombre de points donnes a la FFT (doit etre une puissance de 2)\n");
}


int main(int argc, char** argv){
    int compteur = 0, nb_point = 0;   // variables gestion du programme
    int freqEch, echantillon, bytePerSec, taille, longueur;   // donnees du fichier WAVE
    int filtre = 1, debug = 0, verbeux = 0, point_fourier = 16384;   // les options
    int op;    /* sert a determiner les options selectionner */
    int nb_note = 0, i;

    short* amplitudes = NULL;   // tableau regroupant les amplitudes a chaque point du signal
    float* temps = NULL;    // tableau regroupant le temps associee a l'amplitude de meme indice dans son tableau
    int* notes = NULL;    // tableau contenant les notes avec indice de depart dans les positions paires et indice de fin en position impaire
    
    float note;  // permet de stocker la frequence d'une note
    short amp, defausse_short, nbCanaux, bitsPerSample, bytePerBloc;
    char* nomFich = NULL;
    char **tabnotes;
    char mot[5];   // permet de recuperer les mot de longueur 4 qui servent a identifier chaque bloc
    mot[4] = '\0';   //rajoute la fin du mot directement
    FILE* fich = NULL;
    signal s;

    // verifie que le nombre d'argument minimal requis soit respecte 
    if (argc < 2){
        usage(argv[0]);
        exit(-1);
    }

    /********** traitement des options entrees sur la ligne de commande ********/
    while ((op = getopt(argc, argv, "e:f:n:uvd")) != -1){   /* cherche les options sur la ligne de commande */
      switch (op){    /* determine l'option recuperer */
      case 'v':
        verbeux = 1;
        break;
      case 'd':
        debug = 1;
        break;
      case 'u':   /* affiche l'usage du programme */
        usage(argv[0]);
        exit(0);
      case 'e':    /* permet de preciser la valeur de l'echantillonage */
        filtre = atoi(optarg);  /* recupere la valeur situer juste apres l'option */
        break;
      case 'f':     /* precise le fichier a utiliser */
        nomFich = optarg;
        break;
      case 'n':     /* precise nombre de point a utiliser pour la FFT */
        point_fourier = atoi(optarg);
        break;
      default:   /* si jamais ne correspond a aucune option on appel usage et on quitte */
        usage(argv[0]);
        exit(-1);
      }
    }
    
    // verifie si le fichier a bien ete donne et la fenetre une taille valide
    if (nomFich == NULL){
        usage(argv[0]);
        exit(-1);
    }
    
    // ouverture du fichier WAVE
    fich = fopen(nomFich, "rb");
    if (fich == NULL){
         fprintf(stderr, "Probleme lors de l'ouverture du fichier\n");
         exit(-1);
    }
    
    // lecture de l'entete au moins 44 bytes a traiter
    recupere_mot(mot, fich, debug);   // recupere le mot DataBlocID
    // cherche le bloc de donne appele 'data' pour commencer la lecture
    while(mot[0] != 'd' || mot[1] != 'a' || mot[2] != 't' || mot[3] != 'a'){   // on recommence tant que la constante 'data' n'a pas ete trouver
        if (mot[0] == 'R' && mot[1] == 'I' && mot[2] == 'F' && mot[3] == 'F'){   // on traite les blocs importants: RIFF, cue, fmt
            fread(&taille, sizeof(int), 1, fich);  //FileSize
            // verifie si on se trouve bien dans un fichier WAVE
            recupere_mot(mot, fich, debug);
            if (!(mot[0] == 'W' && mot[1] == 'A' && mot[2] == 'V' && mot[3] == 'E')){
                fprintf(stderr, "Identifiant %s trouvé à la place du mot 'WAVE' dans le bloc RIFF de votre fichier, il ne s'agit pas d'un fichier WAVE (arret du programme)\n", mot);
                exit(-1);
            }
        }else if (mot[0] == 'c' && mot[1] == 'u' && mot[2] == 'e'){
            fread(&longueur, sizeof(int), 1, fich);  // CUE CHUNK longueur du bloc
            if (debug)
                fprintf(stderr, "[DEBUG] longueur = %d\n", longueur);
            fseek(fich, (long) longueur, SEEK_CUR);    // se deplace de la longuer du bloc
        }else if (mot[0] == 'f' && mot[1] == 'm' && mot[2] == 't'){
            fread(&longueur, sizeof(int), 1, fich);  //BlocSize
            if (debug)
                fprintf(stderr, "[DEBUG] longueur = %d\n", longueur);
            fread(&defausse_short, sizeof(short), 1, fich);  //AudioFormat
            fread(&nbCanaux, sizeof(short), 1, fich);  //NbrCanaux
            fread(&freqEch, sizeof(int), 1, fich);  //Frequence
            fread(&bytePerSec, sizeof(int), 1, fich);  //BytePerSec
            fread(&bytePerBloc, sizeof(short), 1, fich);  //BytePerBloc
            fread(&bitsPerSample, sizeof(short), 1, fich);  //BitsPerSample
            fseek(fich, (long) (longueur-16), SEEK_CUR);    // se deplace de la longueur du bloc-16(les donnees deja lues)
        }else if (mot[0] == 'L' && mot[1] == 'I' && mot[2] == 'S' && mot[3] == 'T'){
            fread(&longueur, sizeof(int), 1, fich);  // LIST CHUNK longueur du bloc
            if (debug)
                fprintf(stderr, "[DEBUG] longueur = %d\n", longueur);
            fseek(fich, (long) longueur, SEEK_CUR);    // se deplace de la longueur du bloc
        }else if (!lettre_valide(mot[0], debug) || !lettre_valide(mot[1], debug) || !lettre_valide(mot[2], debug) || !lettre_valide(mot[3], debug)){ // verifie si l'entete a du sens
            fprintf(stderr, "Le nom du bloc n'est pas valide (arret du programme)\n");
            exit(-1);
        }else{
            fread(&longueur, sizeof(int), 1, fich);  // recupere la longueur du bloc inconnu
            if (debug)
                fprintf(stderr, "[DEBUG] longueur = %d\n", longueur);
            fseek(fich, (long) longueur, SEEK_CUR);    // se deplace de la longueur du bloc
        }
        recupere_mot(mot, fich, debug);   // recupere le mot DataBlocID
    }
    fread(&echantillon, sizeof(int), 1, fich);  //DataSize utile pour arreter la boucle principale
    
    // verifie que le format soit compatible avec notre programme
    if (bitsPerSample != 16){
        fprintf(stderr, "Le format du fichier WAVE fournit n'est pas compatible avec notre programme\n");
        fprintf(stderr, "Données codées sur %d bits changez pour un codage des données sur 16 bits\n", bitsPerSample);
    }
    
    // augmente la valeur du filtre pour ne garder les points que d'un seul canal
    filtre *= nbCanaux;
    // calcule du nombre de point a recupere dans les tableaux
    nb_point = (echantillon/((bitsPerSample/8)*filtre));
    
    // afichage des donnees du fichier WAVE
    fprintf(stderr, "\nSpecification du fichier: %s\n", nomFich);
    fprintf(stderr, "Nombre d'octet lu par seconde: %d\n", bytePerSec);
    fprintf(stderr, "Nombre d'octet par bloc: %d\n", bytePerBloc);
    fprintf(stderr, "Nombre de bits par sample: %u\n", bitsPerSample);
    fprintf(stderr, "Nombre de canaux: %u\n", nbCanaux);
    fprintf(stderr, "Frequence d'echantillonage: %d\n", freqEch);
    fprintf(stderr, "Taille du tableau de donnee: %d\n", nb_point);
    fprintf(stderr, "Echantillon: %d\n\n", echantillon);
    
    // initialisation des tableaux
    amplitudes = (short*) calloc(sizeof(short), nb_point);
    temps = (float*) calloc(sizeof(float), nb_point);
    
    // boucle principale lance une fois l'entete du fichier decrypte: analyse des donnees
    while (compteur*(bitsPerSample/8) < echantillon){
         // remets tous les bits du short 'amp' a 0
         amp = 0;
         
         // lecture des donnees du fichier WAVE
         fread(&amp, sizeof(char)*(bitsPerSample/8), 1, fich);
        
         // remplissage du tableau
         amplitudes[compteur/filtre] = amp;
         temps[compteur/filtre] = (compteur/(bitsPerSample/8))*(1.0/freqEch);
         
	     // affichage des infos sur console
	     if (verbeux){
	         fprintf(stderr, "Temps: %lf\n", temps[compteur/filtre]);
	         fprintf(stderr, "Amplitude: %hd\n\n", amp);
         }

         // gestion des compteurs
         compteur += filtre;
         
         // on avance de filtre-1 elements dans le fichier (accelere le traitement)
         fseek(fich, (long) ((filtre-1)*(bitsPerSample/8)), SEEK_CUR);
    }
    
    // recherche de notes
    notes = decoupage_signal(amplitudes, temps, 0, nb_point, 1, &nb_note);
 
    // analyse des notes trouvees par analyse des periodes
    printf("Analyse des notes grace au longueur des periodes:\n");
    for(i = 0; i<nb_note; i++){
      s = analyse_notes(amplitudes,temps,nb_point,notes[i],notes[i+1]);
      printf("Note %d: %s\n",i+1,s.note);
    }
    printf("\n");
 
    // afichage du parametrage de la FFT
    fprintf(stderr, "Nombre de points analyses lors de la FFT: %d\n\n", (int) pow(2, (int) log2(point_fourier)));
 
    // analyse des notes trouvees par la FFT
    tabnotes = (char**) malloc(sizeof(char*)*nb_note); //table des notes
    
    printf("Analyse des notes grace a la FFT:\n");
    for(i = 0; i<nb_note; i++){
      note = analyse_notes_FFT(amplitudes, notes[i], notes[i+1], point_fourier, freqEch);
      tabnotes[i] = detectionnotes(note); //insertion de la note dans la table 
      printf("Note %d: %s (%lf)\n",i+1,tabnotes[i], note);
    }
    
    // generation du fichier intermediaire
    ecrire_fichier(tabnotes,nb_note);
      
    exit(0);
}

void recupere_mot(char mot[5], FILE* fich, int debug){    // recupere un mot de 4 octets (recupere l'ID d'un bloc)
    fread(mot, sizeof(char), 1, fich);
    fread(mot+1, sizeof(char), 1, fich);
    fread(mot+2, sizeof(char), 1, fich);
    fread(mot+3, sizeof(char), 1, fich);
    if (debug)
        fprintf(stderr, "[DEBUG] mot = %s\n", mot);   // affiche le mot trouvee pour debugage
}

int lettre_valide(char lettre, int debug){
    if (debug)
        fprintf(stderr, "[DEBUG] lettre lu %c\n", lettre);
    return (lettre == ' ' || (lettre >= 'a' && lettre <= 'z') || (lettre >= 'A' && lettre <= 'Z'));
}

/************* PARTIE CONSACRE A LA DETECTION DES NOTES **************/

// calcul la moyenne de la duree d'une note
double moyenne_temps(float* temps, int* notes, int debut, int fin){
    int i;
    double somme = 0.0;
    
    for (i=debut; i<fin-1; i++){
        somme += (temps[notes[i+1]]-temps[notes[i]]);
    }
    
    return somme/(fin-debut);

}

// calcule la moyenne des amplitudes de l'indice debut a l'indice fin
double moyenne(short* amplitudes, int debut, int fin){
    int i;
    double somme = 0.0;
    
    for (i=debut; i<fin; i++){
        somme += amplitudes[i];
    }
    
    return somme/(fin-debut);

}

// calcule l'ecart-type des amplitudes de l'indice debut a l'indice fin
double ecart_type(short* amplitudes, int debut, int fin){
    int i;
    double moy = moyenne(amplitudes, debut, fin);
    double somme = 0.0;
    
    // calcul la somme des (x-moy)²
    for (i = debut; i<fin; i++){
        somme += (amplitudes[i]-moy)*(amplitudes[i]-moy);
    }
    
    // divise la somme par n
    somme /= (fin-debut);
    
    // return la racine carre
    return sqrt(somme);

}

// decale tous les elements a gauche en ecrasant les valeurs valant -1
void decalage_gauche(int* notes, int* nb_note){
    int i, decalage = 0;
    
    for (i = 0; i < *nb_note; i++){
        if (i-decalage >= 0){
            if (notes[i] == -1)  // si on trouve un -1 on decalage de un cran de plus vers la gauche pour la prochaine iteration 
                decalage ++; 
            else notes[i-decalage] = notes[i];
        }
    }
    
    // mets a jour le nombre de notes
    *nb_note -= decalage;
}

void enleve_note_courte(int* notes, float* temps, int* nb_note){
    int i, debut_note = 0;
    double moyenne_temporelle = moyenne_temps(temps, notes, 0, *nb_note);
    
    for (i=1; i < *nb_note; i++){
        if ((temps[notes[i]]-temps[notes[debut_note]]) <= moyenne_temporelle){   // compare la duree entre deux notes
            notes[i] = -1;   // si trop court alors on enleve
        }else{
            debut_note = i;   // sinon on garde et on mets a jour le depart de la prochaine duree
        }
    }
    decalage_gauche(notes, nb_note);   // enleve les valeurs a -1 dans le tableau tout en decalant vers la gauche

}

int* insertion_droite(int* notes, int* nb_note, int* note_sup, int nb_note_sup){
    int i, j, k, n;
    int* notes2;
    
    // cree un nouveau tableau de la taille du tableau de notes + le nombre de notes supplementaires
    notes2 = (int*) malloc(sizeof(int)*(*nb_note+nb_note_sup));
    if (notes2 == NULL){
        fprintf(stderr, "wave.c::decalage_droite()::probleme allocation memoire\n");
        exit(-1);
    }
    
    // sauvegarde sa valeur et met a jour le nombre de note du nombre de notes supplementaires
    n = *nb_note;
    *nb_note += nb_note_sup;

    // decalage sur la droite et insertion des notes au bon endroit
    for (i = 0, j = 0, k = 0; i<*nb_note && k<n; i++){
        if (j<nb_note_sup && note_sup[j] == notes[k]){ // si une nouvelle note est egale a une note deja existante on ne l'ecrit qu une fois
            notes2[i] = note_sup[j];     // insertion de la note dans le tableau
            j ++;  // passe a la nouvelle note suivante
            k ++;   // on saute la note dans les anciennes car elle est identique a la nouvelle         
        }else if (j<nb_note_sup && note_sup[j] < notes[k]){   // cherche la prochaine note a placer: nouvelle note dans l'ordre croissant
            notes2[i] = note_sup[j];     // insertion de la note dans le tableau
            j ++;  // passe a la nouvelle note suivante 
        }else{
            notes2[i] = notes[k];   // si pas d'insertion on continue de decaler les anciennes notes a droite
            k ++;  // on passe a la prochaine note a decaler
        }
    }
 
    // supprime l'ancien tableau
    if (notes != NULL)
        free(notes);
    
    return notes2;  // retourne le nouveau pointeur
    
}

int* analyse_note_long(short* amplitudes, float* temps, int* notes, int* nb_note){
    int i, n = *nb_note;
    int* note_sup, *notes2;
    int nb_note_sup;
    // copi le tableau de note dans notes2
    notes2 = (int*) malloc(sizeof(int)*n);
    if (notes2 == NULL){
        fprintf(stderr, "wave.c::abalyse_note_long()::probleme allocation memoire\n");
        exit(-1);
    }
    
    for (i=0; i<n; i++)
        notes2[i] = notes[i];
    
    // regarde s'il faut analyser un espace entre 2 notes
    for (i=0; i < n-1; i++){
        if ((temps[notes[i+1]]-temps[notes[i]]) > 1 ){   // compare la duree d'une note pour savoir si elle depasse une seconde
            // recherche de nouvelles notes entre notes[i] et notes[i+1]
            note_sup = decoupage_signal(amplitudes, temps, notes[i], notes[i+1], 0, &nb_note_sup);
            // integration des nouvelles notes dans le tableau de notes
            notes2 = insertion_droite(notes2, nb_note, note_sup, nb_note_sup);
        }
        
    }
   
    // liberation du tableau d'entree
    if (notes != NULL)
        free(notes);
    
    return notes2;
}

int* decoupage_signal(short* amplitudes, float* temps, int debut, int fin, int traitement_sup, int* nb_n){
    //int amp_max = 0; 
    int test1, test2;
    int i = debut, j = 0, nb_note = fin-debut;
    double coupure = 2*ecart_type(amplitudes, debut, fin)*(375/32768.0);
    
    int* notes2;
    int* notes = (int*) malloc(sizeof(int) * nb_note);
    if (notes == NULL){
        fprintf(stderr, "wave.c::decoupe_signel()::probleme lors de l'allocation memoire\n");
        exit(-1);
    }
    
    // detecte les amplitudes les plus hautes en premier et les stocke
    while (i<fin-1){
        
        // recherche une amplitude basse
        do{
            // arrondi des amplitudes a tester (enlever de la precision seul la forme generale du signal nous interesse et non les valeures)
            test1 = amplitudes[i]*(375/32768.0); test2 = amplitudes[i+1]*(375/32768.0);
            i++;
        }while (test1 >= test2 && i < fin-1);
        
        // recherche une amplitude haute
        do{
            // arrondi des amplitudes a tester (enlever de la precision seul la forme generale du signal nous interesse et non les valeures)
            test1 = amplitudes[i]*(375/32768.0); test2 = amplitudes[i+1]*(375/32768.0);
            i++;
        }while (test1 <= test2 && i < fin-1);
        
        if (test1 > coupure){     // coupure des periodes lorsque le son est trop faible
            // recupere la nouvelle amplitude max: servira pour la prochaine comparaison
            //amp_max = i-1;
            
            // l'amplitude max devient une periode
            notes[j] = i-1;
            j ++;
        }
    }
    
    // application d'un algo supplementaire cherchant des separations entre les periodes pour detecter un changement de note
    notes2 = analyse_separation(notes, temps, j, &nb_note);

    if (traitement_sup == 1){
        // enleve les notes trop courtes
        enleve_note_courte(notes2, temps, &nb_note);
    
        // analyse les notes trop long pour trouver de nouvelles notes
        notes2 = analyse_note_long(amplitudes, temps, notes2, &nb_note);
        
        // renettoye le tableau de notes
        enleve_note_courte(notes2, temps, &nb_note);
    }
    
    // libere le tableau temporaire 'notes'    
    if (notes != NULL)
        free(notes);

    // recupere la taille pour le programme appelant
    *nb_n = nb_note;
        
    return notes2;
}

int* analyse_separation(int* periodes, float* temps, int nb_periode, int* nb_note){
    float debut, fin;
    int i = 0, j = 0;
    
    // alloue un nouveau tableau
    int* notes = (int*) calloc(sizeof(int), nb_periode*2+1);
    if (notes == NULL){
      fprintf(stderr, "wave.c::analyse_periode()::probleme lors de l'allocation memoire\n");
      exit(-1);
    }

    // cherche une periode valide comme point de depart
    while ((temps[periodes[i+1]]-temps[periodes[i]] >= 0.0166 || temps[periodes[i+1]]-temps[periodes[i]] <= 0.00022) && i < nb_periode-2){
        i++;
    }

    // ajoute obligatoirement une periode servant de point de depart    
    notes[j] = periodes[i];
    *nb_note = 1; j ++;
    
    // apres recuperation des periodes on regarde la duree de chaque periode pour detecter les coupures qui ont ete effectue
    for (i=i; i<nb_periode-2; i++){
        // recupere le debut et la fin de la nouvelle periode
        debut = temps[periodes[i]];
        fin = temps[periodes[i+1]];

        // verifie s'il n'y a pas un 'trou' entre les periodes ce qui signifie une baisse d'amplitude interessante pour nous
        if (fin-debut >= 0.05){  // il faut que le 'trou' fasse plus de 50 millisecondes
            notes[j] = periodes[i+1];   // on garde la prochaine periode comme depart d'une note
            j += 1; *nb_note += 1;  // mets a jour les compteurs
           
        }
        
    }
    
    // rajoute la derniere periode comme fin de derniere note
    notes[j] = periodes[nb_periode-1];
    *nb_note += 1;
    
    return notes;

}

/* algo non retenu: ne marche que l'on arrive a recuperer toutes les periodes sans faute (tres dur) */
int* analyse_periode(int* periodes, float* temps, int nb_periode, int* nb_note){
    float periode_prec, debut, fin;
    int i = 0,j = 0;
    
    // alloue un nouveau tableau
    int* notes = (int*) calloc(sizeof(int), nb_periode);
    if (notes == NULL){
        fprintf(stderr, "wave.c::analyse_periode()::probleme lors de l'allocation memoire\n");
        exit(-1);
    }

    // cherche une periode valide comme point de depart
    while ((temps[periodes[i+1]]-temps[periodes[i]] >= 0.0166 || temps[periodes[i+1]]-temps[periodes[i]] <= 0.00022) && i < nb_periode-2){ // duree de la periode compris entre 16 et 0.22 millisecondes
        i++;
    }
    periode_prec = temps[periodes[i+1]]-temps[periodes[i]];

    // ajoute obligatoirement une periode servant comme point de depart
    notes[j] = periodes[i];
    *nb_note = 1; j ++;
    
    // apres recuperation des periodes on regarde la duree de chaque periode pour detecter les variations
    for (i=i; i<nb_periode-1; i++){
        // recupere le debut et la fin de la nouvelle periode
        debut = temps[periodes[i]];
        fin = temps[periodes[i+1]];
                
        // verifie s'il ne s'agirai pas d'une fausse periode car trop petite pour etre une note
        if (fin-debut >= 0.0166 && fin-debut <= 0.00022){   // si compris entre 16 et 0.22 millisecondes
            continue;   // on passe si oui
        }

        if (fin-debut > periode_prec*1.06 || fin-debut < periode_prec*0.94){  // on regarde si la nouvelle periode est differente de plus ou moins 6%
            notes[j] = periodes[i];   // si oui alors on a une nouvelle note
            j ++;
            *nb_note += 1;
            
            // nouvelle reference pour detecter le changement de periode
            periode_prec = fin-debut;
        }
        
    }
    
    return notes;
    
}

