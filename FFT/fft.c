#include <MLV/MLV_all.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <unistd.h>



#define PI 3.141592645
#define ZOOM_X 1000.0
#define VITESSE_DX 10
#define VITESSE_ZOOM 1.05


void recupere_mot(char mot[5], FILE* fich, int debug);
complex* analyse(short* amplitudes, int depart, int fin, int N);
complex* recupere_point_complex(short* amplitudes, int depart, int fin, int n);
void tracer_spectre(complex* fft, int clicx, int clicy, int n, double dx, double zoomx, int dec_x, int dec_y, double echelley, int freqEch, int l, int h);
void tracer_repere_spectre(int clicx, int clicy, double dx, double zoomx, int dec_x, int dec_y, double max, int n, int freqEch, int l, int h);
void affiche_tab(short* amplitudes, int echan_fourier);
short* signal_sinus(int n);



void usage(char* s){   /* explique le fonctionnement du programme */
  fprintf(stderr,"Usage: %s [options] -f <fichier wav>\n",s);
  fprintf(stderr,"      -f <fichier> : permet de preciser le fichier a utiliser (obligatoire)\n");

  fprintf(stderr,"Options:\n");

  fprintf(stderr,"   Choisir les parametres suivants:\n");
  fprintf(stderr,"      -u : affiche l'usage du programme et quitte\n");
  fprintf(stderr,"      -d : active les informations utiles pour le debugage du programme\n");
    
  fprintf(stderr,"   Definir la taille des parametres suivant:\n");
  fprintf(stderr,"      -0 \"entier\" > 1 : pour definir le temps auquel commence l'analyse de fourier\n");
  fprintf(stderr,"      -t \"entier\" > 1 : pour definir la duree de la fenetre utilisee pour l'analyse de fourier\n");
  fprintf(stderr,"      -n \"entier\" > 1 : pour definir le nombre de point utilise dans la fenetre de l'analyse de fourier (doit etre une puissance de 2)\n");
  fprintf(stderr,"      -p \"entier\" > 1 : pour definir la precision de la collecte des données\n");
  fprintf(stderr,"      -l \"entier\" > 1 : pour definir la largeur de la fenetre\n");
  fprintf(stderr,"      -h \"entier\" > 1 : pour definir la hauteur de la fenetre\n");
}


int main(int argc, char** argv){
    int compteur = 0, arret = 0, nb_point = 0, dec_x, dec_y, clicx = 0, clicy = 0, echelley = 700;   
    int freqEch = 44100, echantillon, defausse_entier, bytePerSec, taille, longueur;   // donnees du fichier WAVE
    int l = 1200, h = 750, nb_point_fourier = 1024, filtre = 1, debug = 1, depart = 0, longueur_fenetre = 1, fin = 0;   // les options
    int op;    /* sert a determiner les options selectionner */

    complex* fft;
    short* amplitudes;
    float* temps;
    double zoom = 1.0, dx = 0.0;
    
    short amp, defausse_short, nbCanaux, bitsPerSample, bytePerBloc;
    char* nomFich = NULL;
    char mot[5];   // permet de recuperer les mot de longueur 4 qui servent a identifier chaque bloc
    mot[4] = '\0';   //rajoute la fin du mot directement
    FILE* fich;

    // verifie que le nombre d'argument minimal requis soit respecte    
    if (argc < 2){
         usage(argv[0]);
         exit(-1);
    }

    /********** traitement des options entrees sur la ligne de commande ********/
    while ((op = getopt(argc, argv, "t:n:0:f:l:h:p:ud")) != -1){   /* cherche les options sur la ligne de commande */
      switch (op){    /* determine l'option recuperer */
      case 'd':
        debug = 1;
        break;
      case 'u':   /* affiche l'usage du programme */
        usage(argv[0]);
        exit(0);
      case 'n':    /* permet de preciser le nombre de point utilise */
        nb_point_fourier = atoi(optarg);  /* recupere la valeur situer juste apres l'option */
        break;
      case 'f':     /* precise le fichier a utiliser */
        nomFich = optarg;
        break;
      case '0':     /* temps de depart en ms de l'analyse */
        depart = atoi(optarg);
        break;
      case 't':     /* duree en ms de l'analyse */
        longueur_fenetre = atoi(optarg);
        break;
      case 'l':  /* precise la largeur de la fenetre */
        l = atoi(optarg);
        break;
      case 'h':  /* precise la hauteur de la fenetre */
        h = atoi(optarg);
        break;
      case 'p':  /* precise la largeur de la fenetre */
        filtre = atoi(optarg);
        break;
      default:   /* si jamais ne correspond a aucune option on appel usage et on quitte */
        usage(argv[0]);
        exit(-1);
      }
    }
    
    // verifie si le fichier a bien ete donne et la fenetre une taille valide
    if (l < 800 || h < 600){
        usage(argv[0]);
        exit(-1);
    }
    
    // initialisation des variables de decalage 
    dec_x = 0; dec_y = 35;
    
    fich = fopen(nomFich, "rb");
    if (fich == NULL){
        fprintf(stderr, "Probleme lors de l'ouverture du fichier\n");
        exit(-1);
    }
    
    // creation de la fenetre MLV
    MLV_create_window("Spectre d'un signal","Spectre",l,h);
    
    // lecture de l'entete au moins 44 bytes a traiter
    recupere_mot(mot, fich, debug);   // recupere le mot DataBlocID
    while(mot[0] != 'd' || mot[1] != 'a' || mot[2] != 't' || mot[3] != 'a'){   // on recommence tant que la constante 'data' n'a pas ete trouver
        if (mot[0] == 'R' && mot[1] == 'I' && mot[2] == 'F' && mot[3] == 'F'){   // on traite les blocs importants: RIFF, cue, fmt
            fread(&taille, sizeof(int), 1, fich);  //FileSize
            fread(&defausse_entier, sizeof(int), 1, fich);  //FileFormatID
            // TODO si autre que 'WAVE' alors on quitte car le fichier n'est pas un fichier wave
        }else if (mot[0] == 'c' && mot[1] == 'u' && mot[2] == 'e'){
            fread(&longueur, sizeof(int), 1, fich);  // CUE CHUNK longueur du bloc
            fseek(fich, (long) longueur, SEEK_CUR);    // se deplace de la longuer du bloc
        }else if (mot[0] == 'f' && mot[1] == 'm' && mot[2] == 't'){
            fread(&longueur, sizeof(int), 1, fich);  //BlocSize
            fread(&defausse_short, sizeof(short), 1, fich);  //AudioFormat
            fread(&nbCanaux, sizeof(short), 1, fich);  //NbrCanaux
            fread(&freqEch, sizeof(int), 1, fich);  //Frequence
            fread(&bytePerSec, sizeof(int), 1, fich);  //BytePerSec
            fread(&bytePerBloc, sizeof(short), 1, fich);  //BytePerBloc
            fread(&bitsPerSample, sizeof(short), 1, fich);  //BitsPerSample
            fseek(fich, (long) (longueur-16), SEEK_CUR);    // se deplace de la longueur du bloc-16(les donnees lus)
        }else if (mot[0] == 'L' && mot[1] == 'I' && mot[2] == 'S' && mot[3] == 'T'){
            fread(&longueur, sizeof(int), 1, fich);  // LIST CHUNK longueur du bloc
            fseek(fich, (long) longueur, SEEK_CUR);    // se deplace de la longueur du bloc
        }else{
            fread(&longueur, sizeof(int), 1, fich);  // recupere la longueur du bloc inconnu
            fseek(fich, (long) longueur, SEEK_CUR);    // se deplace de la longueur du bloc
        }
        recupere_mot(mot, fich, debug);   // recupere le mot DataBlocID
    }
    fread(&echantillon, sizeof(int), 1, fich);  //DataSize util pour arreter la boucle principale
    
    // augmente la valeur du filtre pour ne garder les points que d'un seul canal
    filtre *= nbCanaux;
    // calcule du nombre de point a recupere dans les tableaux
    nb_point = (echantillon/((bitsPerSample/8)*filtre));

    // afichage des donnees du fichier WAVE
    fprintf(stderr, "\nSpecification du fichier:\n");
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
         
         // gestion des compteurs
         compteur += filtre;
         
         // on avance de filtre-1 elements dans le fichier (accelere le traitement)
         fseek(fich, (long) ((filtre-1)*(bitsPerSample/8)), SEEK_CUR);
    }

    // calcul des indices depart et fin
    fin = (((depart+longueur_fenetre)/1000.0)*freqEch);
    depart = ((depart/1000.0)*freqEch);
    // lancement de l'analyse
    fft = analyse(amplitudes, depart, fin, log2(nb_point_fourier));
    
    fprintf(stderr, "depart = %d(%f), fin = %d(%f), freq = %lf\n", depart, temps[depart], fin, temps[fin], (longueur_fenetre/1000.0)/nb_point_fourier);
    
    tracer_spectre(fft, clicx, clicy, /*pow(2, log2(*/nb_point_fourier/*))*/, dx, zoom, dec_x, dec_y, echelley, /*(longueur_fenetre/1000.0)/nb_point_fourier*/nb_point_fourier, l, h);
    // visualisation du graphe
    while (!arret){
         // nettoyage de la fenetre
         //glClear(GL_COLOR_BUFFER_BIT);
        
         // definition de l'espace de dessin
         //glMatrixMode(GL_PROJECTION);
         //glLoadIdentity();
         //glOrtho(0, 1200, 0, 750, 0, 1);
         
         //MLV_wait_keyboard(&touche, NULL, NULL);
         //fprintf(stderr, "touche = %d\n", touche);
         // gestion des controles
         if (MLV_get_keyboard_state(276) == MLV_PRESSED){   // fleche droite
             dx += VITESSE_DX*(1.0/zoom);
             // nettoyage de la fenetre
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracer_spectre(fft, clicx, clicy, /*pow(2, log2(*/nb_point_fourier/*))*/, dx, zoom, dec_x, dec_y, echelley, /*(longueur_fenetre/1000.0)/nb_point_fourier*/nb_point_fourier, l, h);
         }else if (MLV_get_keyboard_state(275) == MLV_PRESSED){   // fleche gauche
             dx -= VITESSE_DX*(1.0/zoom);
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracer_spectre(fft, clicx, clicy, /*pow(2, log2(*/nb_point_fourier/*))*/, dx, zoom, dec_x, dec_y, echelley, /*(longueur_fenetre/1000.0)/nb_point_fourier*/nb_point_fourier, l, h);
         }else if (MLV_get_keyboard_state(274) == MLV_PRESSED){     // fleche bas
             zoom /= VITESSE_ZOOM;
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracer_spectre(fft, clicx, clicy, /*pow(2, log2(*/nb_point_fourier/*))*/, dx, zoom, dec_x, dec_y, echelley, /*(longueur_fenetre/1000.0)/nb_point_fourier*/nb_point_fourier, l, h);
         }else if (MLV_get_keyboard_state(273) == MLV_PRESSED){      // fleche haut
             zoom *= VITESSE_ZOOM;
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracer_spectre(fft, clicx, clicy, /*pow(2, log2(*/nb_point_fourier/*))*/, dx, zoom, dec_x, dec_y, echelley, /*(longueur_fenetre/1000.0)/nb_point_fourier*/nb_point_fourier, l, h);
         }else if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_PRESSED){
             MLV_get_mouse_position(&clicx, &clicy);
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracer_spectre(fft, clicx, clicy, /*pow(2, log2(*/nb_point_fourier/*))*/, dx, zoom, dec_x, dec_y, echelley, /*(longueur_fenetre/1000.0)/nb_point_fourier*/nb_point_fourier, l, h);
         }else if (MLV_get_keyboard_state(27) == MLV_PRESSED){
             arret = 1;
         }

         // attente en seconde
         MLV_wait_milliseconds(10);
         
         // envoi des donnees
         //glFlush();
    }
    

    MLV_free_window();
    
    exit(0);

}

double tab_max(complex* tab, int n){
    int i;
    double max = cabs(tab[0]);
    
    for (i=1; i<n; i++){
        if (cabs(tab[i]) > max){
            max = cabs(tab[i]);
        }
    }
    
    return max;
}

complex* recupere_point_complex(short* amplitudes, int depart, int fin, int n){
     int i; int pas;
     complex* tab = (complex*) malloc(sizeof(complex)*n);
     if (tab == NULL){
         fprintf(stderr, "fft.c::recupere_point_complexe()::probleme d'allocation memoire\n");
         exit(-1);
     }
     
     
     if (amplitudes == NULL){
          for (i=0; i<n; i++){
              tab[i] = 0+I*0;
          }
          
          return tab;
     }
     
     pas = (fin-depart)/n;
     if (pas == 0){
         fprintf(stderr, "Pas assez de point dans l'intervalle pour faire l'analyse de fourier\n");
         exit(-1);
     }
     
     for (i=0; i<(n*pas); i+=pas){
         tab[i/pas] = amplitudes[depart+i]+I*0;
     }
     
     
     return tab;
}


int inversionBits(int q, int k){
    int m = 0;
    int i = k;
    int b;

    for (b = 0; b<q; b++){
        m = m|(i&1);
        m = m<<1;
        i = i>>1;
    }
    
    m = m>>1;
    return m;
}

void distributionInversionBits(int q, complex* freq, complex* y){
    int N = pow(2, q);
    int m; int k;
    
    for (k=0; k<N; k++){
        m = inversionBits(q, k);
        y[m] = freq[k];   // attention rajouter copie du nombre complexe
    }
    
}


void calculFFT(complex* freq, complex* y, int q, int e){
    int ne = pow(2,e);
    int nem1 = ne/2;
    int i,k,h;
    double phi;
    complex W;
    
    for (k=1; k<nem1; k++){
        phi = 2*PI*k/ne;
        W = cos(phi)+sin(phi)*1/I;
        for (i=0; i<pow(2,q-e); i++){
            h = i*ne;
            y[h+k] = freq[h+k]+W*freq[h+k+nem1];
            y[h+ne-k] = freq[h+ne-k-nem1]+conj(W)*freq[h+ne-k];
        }
    }
    
    for (i=0; i<pow(2,q-e); i++){
        h = i*ne;
        y[h] = freq[h]+freq[h+nem1];
        y[h+nem1] = freq[h]-freq[h+nem1];
    }

}


complex* analyse(short* amplitudes, int depart, int fin, int q){
     int e, N = pow(2, q);
     complex* echange;
     complex* freq;
     complex* y = recupere_point_complex(NULL, 0, 0, N);
     
     freq = recupere_point_complex(amplitudes, depart, fin, N);
     // rempli 'y' a partir de 'x' en redistribuant les valeurs de 'x' au indice inverse bit a bit de 'y'
     distributionInversionBits(q, freq, y);
     
     // echange des deux tableaux de nombres complexes
     echange = freq;
     freq = y;
     y = echange;
     
     for (e = 1; e<q+1; e++){
        
        // calcul de la fft
        calculFFT(freq, y, q, e);
        
        // on reechange les deux tableaux
        echange = freq;
        freq = y;
        y = echange;
     }

     return freq;
}


void tracerSegment(double dx, int dec_x, int dec_y, double zoom, double x1, double y1, double x2, double y2, int l, int h){
    // transformation de la coordonnee x1 (zoom + decalage du zoom) 
    x1 = (dx+x1-dec_x)*zoom;
    x1 += dec_x;
    // transformation de la coordonnee x2 (zoom + decalage du zoom)
    x2 = (dx+x2-dec_x)*zoom;
    x2 += dec_x;
    
    // tracer du segment grace a MLV
    if (x1>=0 && x2>=0 && y1>=0 && y2>=0 && x1<=l && x2<=l && y1<=h && y2<=h)
        MLV_draw_line((int) x1, (int) (h-dec_y)-y1, (int) x2, (int) (h-dec_y)-y2, MLV_rgba(255,0,0,255));
    
    /*glBegin(GL_LINES);
    //glColor3f(255*(1.0/255.0), 0, 0);
    fprintf(stderr, "ICI\n");
    glVertex2d(x1, y1); 
    glVertex2d(x2, y2); 
    glEnd();*/      
}


void tracer_spectre(complex* fft, int clicx, int clicy, int n, double dx, double zoomx, int dec_x, int dec_y, double echelley, int freqEch, int l, int h){
     int i;
     double max = tab_max(fft, n);
     double x = 0, y = cabs(fft[0])*echelley/max;
     
     for (i = 0; i<n-1; i++){
          tracerSegment(dx, l/2, dec_y, zoomx, x, y, ((double) (i+1)/n)*freqEch, cabs(fft[i+1])*echelley/max, l, h);
          x = ((double) (i+1)/n)*freqEch;
          y = cabs(fft[i+1])*echelley/max;
     }
     
     tracer_repere_spectre(clicx, clicy, dx, zoomx, dec_x, dec_y, 1.0, n, freqEch, l, h);
     
     MLV_actualise_window();
}


void tracer_repere_spectre(int clicx, int clicy, double dx, double zoomx, int dec_x, int dec_y, double max, int n, int freqEch, int l, int h){
    int i, pas = 100, nb_grad = 10;
    double valeur, czoom;
    int tailleText = 0;
    char text[10];
    char valeurs[25]="(0.000, 0.000)";
    
    text[9] = '\0';
    
    czoom = l/2-dx;   // on zoom les valeures par rapport a leur representation et non a leur coordonnees dans la fenetre
    
    // trace de la croix permettant de connaitre une coordonnee precise
    if (clicx != 0 && clicy != 0){
        MLV_draw_line(0, clicy, l, clicy, MLV_rgba(0, 255, 0, 255));
        MLV_draw_line(clicx, 0, clicx, h, MLV_rgba(0, 255, 0, 255));
    }
    
    // tracer la valeur en x et y pointee par la souris
    if (clicx != 0 && clicy != 0){
        valeur = (clicx-dx)-czoom;   // je recentre ma valeur x-dx sur l'origine
        valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
        valeur += czoom;   // je repositionne ma valeur
        valeur *= ((double) freqEch)/n;
        sprintf(valeurs,"(%.3lf, %.3lf)", valeur, (double) (h-dec_y-clicy)*(max/h));
    }
    MLV_draw_text(l-150, 25, valeurs, MLV_rgba(0, 255, 0, 255));
    
    // trace le repere de l'axe des x
    MLV_draw_line(5, h-5, l, h-5, MLV_rgba(0, 0, 255, 255));
    // trace la graduation
    for (i=pas/nb_grad; i<l; i+=(pas/nb_grad)){
        if (i % pas == 0){
            valeur = i-dx-czoom;   // je recentre ma valeur i-dx sur l'origine
            valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
            valeur += czoom;   // je repositionne ma valeur
            valeur *= ((double) freqEch)/n;
            sprintf(text,"%.3lf", valeur);
            MLV_get_size_of_text(text, &tailleText, NULL);
            MLV_draw_line(i, h-5, i, h-15, MLV_rgba(0, 0, 255, 255));
            if (n-valeur > 0 && n-valeur <= n){
                 MLV_draw_text((i-tailleText/2), h-30, text, MLV_rgba(0, 0, 255, 255));
            }
        }else{
            MLV_draw_line(i, h-5, i, h-10, MLV_rgba(0, 0, 255, 255));
        }
    }
    
    // trace le repere de l'axe des y
    MLV_draw_line(5, h-5, 5, 0, MLV_rgba(0, 0, 255, 255));
    // trace la graduation
    for (i=0; i<h; i+=(pas/nb_grad)){
        if (i % pas == 0){
            sprintf(text,"%.3lf",i*(max/h));
            MLV_get_size_of_text(text, NULL, &tailleText);
            MLV_draw_line(5, (h-dec_y)-i, 15, (h-dec_y)-i, MLV_rgba(0, 0, 255, 255));
            MLV_draw_text(20, (h-dec_y)-i-tailleText/2, text, MLV_rgba(0, 0, 255, 255));
        }else{
            MLV_draw_line(5, (h-dec_y)+i, 10, (h-dec_y)+i, MLV_rgba(0, 0, 255, 255));
            MLV_draw_line(5, (h-dec_y)-i, 10, (h-dec_y)-i, MLV_rgba(0, 0, 255, 255));
        }
    }
}


void recupere_mot(char mot[5], FILE* fich, int debug){    // recupere un mot de 4 octets (recupere l'ID d'un bloc)
    fread(mot, sizeof(char), 1, fich);
    fread(mot+1, sizeof(char), 1, fich);
    fread(mot+2, sizeof(char), 1, fich);
    fread(mot+3, sizeof(char), 1, fich);
}


short* signal_sinus(int n){
    int i;
    short* amp = (short*) malloc(sizeof(short)*n);
    if (amp == NULL){
        fprintf(stderr, "fft.c::signal_sinus()::probleme allocation memoire\n");
        exit(-1);
    }

    for (i = 0; i<n; i++)
        amp[i] = sin(2.0*PI*i/n) + 0.5*sin(4.0*PI*i/n) + 0.25*cos(10.0*PI*i/n)*32767.0;

    return amp;
}


void affiche_tab(short* amplitudes, int echan_fourier){
    int i;
    
    for (i=0; i<echan_fourier; i++)
         fprintf(stdout, "%d %d\n", i, amplitudes[i]);
    fprintf(stdout, "fin\n");
    
}




