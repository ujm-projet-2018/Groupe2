#include <stdlib.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <MLV/MLV_all.h>
#include <unistd.h>

#include "detection_notes.h"
#include "fft.h" 
//#include "GL/gl.h"
//#include "GL/glut.h"

#define ZOOM_X 1000.0
#define VITESSE_DX 10
#define VITESSE_ZOOM 1.05



//void clavier(unsigned char c, int i, int j);
short* signalPeriodique(short* amplitudes, int depart, int n, int repetition);
void tracer_repere(int clicx, int clicy, double dx, double zoomx, int dec_x, int dec_y, int l, int h);
void recupere_mot(char mot[5], FILE* fich, int debug);
void tracerCourbe(int clicx, int clicy, double dx, int dec_x, int dec_y, double zoom, short* amplitude, float* temps, int nb_point, int filtre, int l, int h, int anim, int verbeux, int debug, int *tfre, int ntfre, int* notes, int nb_note);
void tracerSegment(double dx, int dec_x, int dec_y, double zoom, double x1, double y1, double x2, double y2, int l, int h, int bool, MLV_Color color);
void tracerPoint(double dx, int dec_x, double zoom, double x1, double y1, int l, int h);
int* decoupage_signal(short* amplitudes, float* temps, int nb_point, int* nb_note);
int* analyse_periode(int* periodes, float* temps, int nb_periode, int* nb_note);
void tracer_notes(double dx, int dec_x, int dec_y, double zoom, int* notes, int taille, float* temps, int l, int h);
int* nettoyage(int* notes, int t1, int t2);


//void detectionnotes(double ff);
double cleanFrequence(double *tabfreq,int nfre);
double* tri_fusion_notes(double *tab,int nbel);
double* tri_fusion_notes_bis(double *tab1,int nbel1,double *tab2, int nbel2);
double cleanFrequence2(double *tabfreq,int nfre);
double cleanFrequence3(double *tabfreq,int nfre);
double cleanFrequence4(double *tabfreq,int nfre);


void usage(char* s){   /* explique le fonctionnement du programme */
  fprintf(stderr,"Usage: %s [options] -f <fichier wav>\n",s);
  fprintf(stderr,"      -f <fichier> : permet de preciser le fichier a utiliser (obligatoire)\n");

  fprintf(stderr,"Options:\n");

  fprintf(stderr,"   Choisir les parametres suivants:\n");
  fprintf(stderr,"      -u : affiche l'usage du programme et quitte\n");
  fprintf(stderr,"      -v : active les informations du traitement sur le terminal\n");
  fprintf(stderr,"      -d : active les informations utiles pour le debugage du programme\n");
  fprintf(stderr,"      -m : pour jouer le fichier wave lors de son premier trace a l'ecran\n");
  fprintf(stderr,"      -a : pour activer l'animation du tracer de la courbe\n");
    
  fprintf(stderr,"   Definir la taille des parametres suivant:\n");
  fprintf(stderr,"      -e \"entier\" > 1 : pour choisir un echantillonage appliquer lors de la recuperation des donnees (divise le nombre de donnees recuperees)\n");
  fprintf(stderr,"      -p \"entier\" > 1 : pour definir la precision du tracer du signal: ne touche pas aux donnees et fluidifie le tracer\n");
  fprintf(stderr,"      -l \"entier\" > 800 : pour choisir la largeur de la fenetre (1200 par defaut)\n");
  fprintf(stderr,"      -h \"entier\" > 600 : pour choisir la largeur de la fenetre (750 par defaut)\n");
}


int main(int argc, char** argv){
    int compteur = 0, arret = 0, nb_point = 0, dec_x, dec_y, clicx = 0, clicy = 0, spectre = 0, echelley = 700;   // variables gestion du programme
    int freqEch, echantillon, defausse_entier, bytePerSec, taille, longueur;   // donnees du fichier WAVE
    int filtre = 1, precision = 1, l = 1200, l1 = 1200, h = 750, h1 = 750, op_son = 0, anim = 0, debug = 0, verbeux = 0;   // les options
    int op;    /* sert a determiner les options selectionner */
    int n, depart; // taille pour la fft et indice de depart dans le tableau des amplitudes
    int nb_note = 0;
    int* notes = NULL;

    //int ampmax = 0, *tfre, ntfre = 0,nfre=0, ampmaxold = 0, ifreq1 = 0, ifreq2 = 0, ifreqmax1, ifreqmax2;
    //double frequence = 0, frequencemax = -1000000, t1 = 0, t2 = 0,*tabfreq;
    float t2;
    complex* fft = NULL; // tableau comptenant le resultat de la fft
    short* amplitudes = NULL;   // tableau regroupant les amplitudes a chaque point du signal
    float* temps = NULL;    // tableau regroupant le temps associee a l'amplitude de meme indice dans son tableau
    signal s;
    double zoom = 1.0, dx = 0.0;   // variable de transformations geometriques
    
    short amp, defausse_short, nbCanaux, bitsPerSample, bytePerBloc;
    char* nomFich = NULL;
    char mot[5];   // permet de recuperer les mot de longueur 4 qui servent a identifier chaque bloc
    mot[4] = '\0';   //rajoute la fin du mot directement
    FILE* fich = NULL;
    MLV_Sound* son = NULL;

    // verifie que le nombre d'argument minimal requis soit respecte    
    if (argc < 2){
        usage(argv[0]);
        exit(-1);
    }

    /********** traitement des options entrees sur la ligne de commande ********/
    while ((op = getopt(argc, argv, "e:f:l:h:p:mauvd")) != -1){   /* cherche les options sur la ligne de commande */
      switch (op){    /* determine l'option recuperer */
      case 'm':
        op_son = 1;
        break;
      case 'a':
        anim = 1;
        break;
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
      case 'p':     /* precise la precision du trace */
        precision = atoi(optarg);
        break;
      case 'l':  /* precise la largeur de la fenetre */
        l = atoi(optarg);
        l1 = l;
        break;
      case 'h':  /* precise la hauteur de la fenetre */
        h = atoi(optarg);
        h1 = h;
        break;
      default:   /* si jamais ne correspond a aucune option on appel usage et on quitte */
        usage(argv[0]);
        exit(-1);
      }
    }
    
    // verifie si le fichier a bien ete donne et la fenetre une taille valide
    if (nomFich == NULL || l < 800 || h < 600){
        usage(argv[0]);
        exit(-1);
    }
    
    // initialisation des variables de dacalage 
    dec_x = l/2; dec_y = h/2;
    
    // initialisation du lecteur et chargeur audio
    MLV_init_audio();
    son = MLV_load_sound(nomFich);  // chargement du son
    if (son == NULL)
        fprintf(stderr, "Le fichier audio n'a pu etre ouvert par MLV il ne sera donc pas lu.\n");
    
    fich = fopen(nomFich, "rb");
    if (fich == NULL){
         fprintf(stderr, "Probleme lors de l'ouverture du fichier\n");
         exit(-1);
    }
    
    /*
    // initialisation de glut
    glutInit(&argc, argv);
    // initialisation du mode d'affichage
    glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE);
    // position et taille de la fenetre
    glutInitWindowSize(1200, 750);   // remplacer par glutFullScreen pour du pleine ecran
    glutInitWindowPosition(50, 50);
    // creation  de la fenetre
    glutCreateWindow("Spetre d'un fichier WAV");
    // activation d'une fonction gerant le clavier
    glutKeyboardFunc(clavier); 
    // initialisation de la redirection des evenements vers les fonctions associees
    //glutMainLoop();
    */
    // creation de la fenetre MLV
    MLV_create_window("Spetre d'un fichier WAV","Spectre",l,h);
    
    // lecture de l'entete au moins 44 bytes a traiter
    recupere_mot(mot, fich, debug);   // recupere le mot DataBlocID
    while(mot[0] != 'd' || mot[1] != 'a' || mot[2] != 't' || mot[3] != 'a'){   // on recommence tant que la constante 'data' n'a pas ete trouver
        if (mot[0] == 'R' && mot[1] == 'I' && mot[2] == 'F' && mot[3] == 'F'){   // on traite les blocs importants: RIFF, cue, fmt
            fread(&taille, sizeof(int), 1, fich);  //FileSize
            fread(&defausse_entier, sizeof(int), 1, fich);  //FileFormatID
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
            fseek(fich, (long) (longueur-16), SEEK_CUR);    // se deplace de la longueur du bloc-16(les donnees lus)
        }else if (mot[0] == 'L' && mot[1] == 'I' && mot[2] == 'S' && mot[3] == 'T'){
            fread(&longueur, sizeof(int), 1, fich);  // LIST CHUNK longueur du bloc
            if (debug)
                fprintf(stderr, "[DEBUG] longueur = %d\n", longueur);
            fseek(fich, (long) longueur, SEEK_CUR);    // se deplace de la longueur du bloc
        }else{
            fread(&longueur, sizeof(int), 1, fich);  // recupere la longueur du bloc inconnu
            if (debug)
                fprintf(stderr, "[DEBUG] longueur = %d\n", longueur);
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
    //tfre = (int*) malloc(sizeof(int)*1);
    //tabfreq = (double*) malloc(sizeof(double)*1);
    
    // boucle principale lance une fois l'entete du fichier decrypte: analyse des donnees
    
    while (compteur*(bitsPerSample/8) < echantillon){
         // remets tous les bits du short 'amp' a 0
         amp = 0;
         // lecture des donnees du fichier WAVE
         fread(&amp, sizeof(char)*(bitsPerSample/8), 1, fich);
         /*amp /=500;
	 if(amp > 1){
	   amp*=10000;
	   }*/
         // Lucas fondamental***********************************************************
	     t2 = (compteur/(bitsPerSample/8))*(1.0/freqEch);
	     /*ifreq2 = compteur/filtre;
	     if (ampmax == 0){
	       ampmax = amp;
	     }
	     if(amp > ampmax-(ampmax*0.30) && amp < ampmax*1.30 && t2 > t1+0.001){
	       if(ampmax != 0){
	       frequence = 1/(t2-t1); */
	         /*if(ampmax > 2*ampmaxold){
	           frequencemax = 0;
		   }*/
	         /*if(frequencemax >2*frequence){
		   nfre--;
		   frequencemax = 0;
		   }*/
	     // if(frequence > frequencemax){  // enregistre la nouvelle frequence max
	           // recupere les indices de la frequence maximale
      /*      ifreqmax1 = ifreq1;  
	           ifreqmax2 = ifreq2;
	           frequencemax = frequence;
	         }
	       }
	       ntfre ++;
	       nfre ++;
	       tfre = realloc(tfre,sizeof(int)*ntfre);
	       tabfreq = realloc(tabfreq,sizeof(double)*nfre);
	       tabfreq[nfre-1] = frequence;
	       tfre[ntfre-1] = compteur/filtre;
	       ampmaxold = ampmax;
	       ampmax = amp;
	       t1 = t2;
	       ifreq1 = ifreq2;  // recupere les indices des temps t1 et t2 pour avoir l'indice de la frequence max
	     }else if (amp > ampmax ){
	       //t1 = t2;
	       //ifreq1 = ifreq2;
	       ampmax = amp;
	       }*/
	     //*****************************************************************************
	     
	     // affichage des infos sur console
	     if (verbeux){
	         fprintf(stderr, "Temps: %lf\n", t2);
	         fprintf(stderr, "Amplitude: %hd\n\n", amp);
         }
        
         // remplissage du tableau
         amplitudes[compteur/filtre] = amp;
         temps[compteur/filtre] = (compteur/(bitsPerSample/8))*(1.0/freqEch);
	     //printf("%d %f\n",compteur,temps[compteur/filtre]);
         
         // gestion des compteurs
         compteur += filtre;
         
         // on avance de filtre-1 elements dans le fichier (accelere le traitement)
         fseek(fich, (long) ((filtre-1)*(bitsPerSample/8)), SEEK_CUR);
    }
    // analyse des notes trouvees
    s = analyse_notes(amplitudes,temps,nb_point);
    // recherche de notes
    notes = decoupage_signal(amplitudes, temps, nb_point, &nb_note);
    
    
    
    
    printf("retour Analyse: %s \n",s.note);
    /*frequencemax = cleanFrequence(tabfreq,nfre);
    printf("Frequence 1 : %f\n",frequencemax);
    detectionnotes(frequencemax);
    frequencemax = cleanFrequence2(tabfreq,nfre);
    printf("\nFrequence 2 : %f\n",frequencemax);
    detectionnotes(frequencemax);
    frequencemax = cleanFrequence3(tabfreq,nfre);
    printf("\nFrequence 3 : %f\n",frequencemax);
    detectionnotes(frequencemax);
    frequencemax = cleanFrequence4(tabfreq,nfre);
    printf("\nFrequence 4 : %f\n",frequencemax);
    detectionnotes(frequencemax);*/

    
    
    // Analyse de fourier sur le signal
    //n = ifreqmax2-ifreqmax1;  // calcul du nombre de point a analyser pour la frequence max
    //fprintf(stderr, "nb point fft = %d | freqence max = %f\n", n, frequencemax);
    //depart = ifreqmax1;   // point de depart dans le tableau des amplitudes  pour l'analyse sur la frequence max
    //n = pow(2, ((int) log2(n*10))+1);  // transmets la premiere puissance de 2 superieur au nombre de point qui seront analyse
    //fft = analyseFFT(signalPeriodique(amplitudes, ifreqmax1, ifreqmax2-ifreqmax1, 10), 0, (ifreqmax2-ifreqmax1)*10, n);  // 10 = nombre de fois que la periode trouve doit etre repete
    
    
    //MLV_Keyboard_button touche;
    if (son != NULL && op_son)
        MLV_play_sound(son, 1.0f);
    MLV_clear_window(MLV_rgba(255, 255, 255, 255));   // affichage fond blanc
    tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, anim, verbeux, debug, s.tfre, s.ntfre, notes, nb_note);
    // visualisation du graphe
    while (!arret){
         // nettoyage de la fenetre
         //glClear(GL_COLOR_BUFFER_BIT);
        
         // definition de l'espace de dessin
         //glMatrixMode(GL_PROJECTION);
         //glLoadIdentity();
         //glOrtho(0, 1200, 0, 750, 0, 1);
         
         // gestion des controles
         if (MLV_get_keyboard_state(276) == MLV_PRESSED){   // fleche droite
             dx += VITESSE_DX*(1.0/zoom);
             // nettoyage de la fenetre
             MLV_clear_window(MLV_rgba(255, 255, 255, 255));
             // appel la fonction de tracer
             /*if (spectre)
	       tracer_spectre(fft, clicx, clicy, n, dx, zoom, 0, 50, echelley, freqEch, l, h);
	       else*/ tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,s.tfre,s.ntfre, notes, nb_note);
         }else if (MLV_get_keyboard_state(275) == MLV_PRESSED){   // fleche gauche
             dx -= VITESSE_DX*(1.0/zoom);
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(255, 255, 255, 255));
             // appel la fonction de tracer
             /*if (spectre)
                  tracer_spectre(fft, clicx, clicy, n, dx, zoom, 0, 50, echelley, freqEch, l, h);
		  else*/ tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,s.tfre,s.ntfre, notes, nb_note);
         }else if (MLV_get_keyboard_state(274) == MLV_PRESSED){     // fleche bas
             zoom /= VITESSE_ZOOM;
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(255, 255, 255, 255));
             // appel la fonction de tracer
             /*if (spectre)
                  tracer_spectre(fft, clicx, clicy, n, dx, zoom, 0, 50, echelley, freqEch, l, h);
		  else*/ tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,s.tfre,s.ntfre, notes, nb_note);
         }else if (MLV_get_keyboard_state(273) == MLV_PRESSED){      // fleche haut
             zoom *= VITESSE_ZOOM;
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(255, 255, 255, 255));
             // appel la fonction de tracer
             /*if (spectre)
                  tracer_spectre(fft, clicx, clicy, n, dx, zoom, 0, 50, echelley, freqEch, l, h);
		  else*/ tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,s.tfre,s.ntfre, notes, nb_note);
         }else if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_PRESSED){
             MLV_get_mouse_position(&clicx, &clicy);
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(255, 255, 255, 255));
             // appel la fonction de tracer
             /*if (spectre)
                  tracer_spectre(fft, clicx, clicy, n, dx, zoom, 0, 50, echelley, freqEch, l, h);
		  else*/ tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,s.tfre,s.ntfre, notes, nb_note);
         }else if (MLV_get_keyboard_state(102) == MLV_PRESSED){   // touche 'f'
             // changement de la fonction tracer: spectre ou signal
             spectre = 1-spectre;
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(255, 255, 255, 255));
             // tracer de la courbe
             /*if (spectre)
                  tracer_spectre(fft, clicx, clicy, n, dx, zoom, 0, 50, echelley, freqEch, l, h);
		  else*/ tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,s.tfre,s.ntfre, notes, nb_note);
         }else if (MLV_get_keyboard_state(32) == MLV_PRESSED){    // en appuyant sur espace on passe en pleine ecran
             
             if (MLV_is_full_screen() == 0){
                 MLV_enable_full_screen();
                 l = MLV_get_desktop_width();
                 h = MLV_get_desktop_height();	
                 MLV_change_window_size(l, h);	
             }else{
                 MLV_disable_full_screen();
                 l = l1;
                 h = h1;	
                 MLV_change_window_size(l, h);
             }
             // reaffichage du signal
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(255, 255, 255, 255));
             // appel la fonction de tracer
             dec_x = l/2; dec_y = h/2;
             tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,s.tfre,s.ntfre, notes, nb_note);
         }else if (MLV_get_keyboard_state(27) == MLV_PRESSED){
             arret = 1;
         }

         // attente en seconde
         MLV_wait_milliseconds(30);
         
         // envoi des donnees
         //glFlush();
    }
    
    // libere l'espace allouee par la fenetre
    MLV_free_window();
    
    exit(0);
}

void tracerCourbe(int clicx, int clicy, double dx, int dec_x, int dec_y, double zoom, short* amplitude, float* temps, int nb_point, int filtre, int l, int h, int anim, int verbeux, int debug,int *tfre, int ntfre, int* notes, int nb_note){
    int i,j,bool;
    double oldTemps = 0, oldAmp = dec_y;
    
    for (i = 0,j=0; i<nb_point-1; i+=filtre){
        // affiche sur le terminal le point actuellement trace
        if (debug)
            fprintf(stderr, "[DEBUG] Point n %d\n", i);
        if(i == tfre[j]){
	        if(j+1!=ntfre){
	          j++;
	        }
	        bool = 1;
	    }
	    else{
	        bool = 0;
	    }
	    
        tracerSegment(dx, dec_x, dec_y, zoom, oldTemps*ZOOM_X, (dec_y+oldAmp*dec_y/32767.0), temps[i+1]*ZOOM_X, (double) (dec_y+amplitude[i+1]*(dec_y/32767.0)), l, h, bool, MLV_rgba(150, 20, 20, 255));    // trace le segment entre le dernier point et le point actuel
        
        // recuperer le dernier point tracer
        oldTemps = temps[i+1]; oldAmp = amplitude[i+1];

        
        // anim le tracer de la courbe
        if (anim && (oldTemps*ZOOM_X)<l){
            MLV_wait_milliseconds(1);
            MLV_actualise_window();
           //glFlush();
        }
    }
    
    // tracer des notes trouvees            
    tracer_notes(dx, dec_x, dec_y, zoom, notes, nb_note, temps, l, h);

    // tracer du repere    
    tracer_repere(clicx, clicy, dx, zoom, dec_x, dec_y, l, h);

    // reactualise l'affichage
    MLV_actualise_window();
    // affiche pour le debugage le nombre de point traces a l'ecran
    /*if (verbeux)
      fprintf(stderr, "Nombre de points traces %d\n", nb_point/filtre);*/
}

void tracerSegment(double dx, int dec_x, int dec_y, double zoom, double x1, double y1, double x2, double y2, int l, int h, int Tfre, MLV_Color color){
    // transformation de la coordonnee x1 (zoom + decalage du zoom) 
    x1 = (dx+x1-dec_x)*zoom;
    x1 += dec_x;
    // transformation de la coordonnee x2 (zoom + decalage du zoom)
    x2 = (dx+x2-dec_x)*zoom;
    x2 += dec_x;
    
    // tracer du segment grace a MLV
    if (x1>=0 && x2>=0 && y1>=0 && y2>=0 && x1<=l && x2<=l && y1<=h && y2<=h)
        MLV_draw_line(x1, -(y1-dec_y)+dec_y, x2, -(y2-dec_y)+dec_y, color);
    /********************Lucas signalisation de la fréquence***************************/
    if(Tfre == 1){
    //  MLV_draw_line(x1,0,x1,600,MLV_rgba(255,255,255,255));
    }
    /**1********************************************************************************/
    
    /*glBegin(GL_LINES);
    //glColor3f(255*(1.0/255.0), 0, 0);
    fprintf(stderr, "ICI\n");
    glVertex2d(x1, y1); 
    glVertex2d(x2, y2); 
    glEnd();*/      
}

void tracerPoint(double dx, int dec_x, double zoom, double x1, double y1, int l, int h){
    x1 = (dx+x1-dec_x)*zoom;
    x1 += dec_x;
    
    // tracer du segment grace a MLV
    if (x1>=0 && y1>=0 && x1<=l && y1<=h)
        MLV_draw_point(x1, y1, MLV_rgba(255,0,0,255));
}

void tracer_repere(int clicx, int clicy, double dx, double zoomx, int dec_x, int dec_y, int l, int h){
    int i, pas = 100, nb_grad = 10;
    double valeur;
    int tailleText = 0;
    char text[10];
    char valeurs[25]="(0.000, 0.000)";
    
    text[9] = '\0';
    
    dec_x = dec_x-dx;   // on zoom les valeures par rapport a leur representation et non a leur coordonnees reelles 
    
    // trace de la croix permettant de connaitre une coordonnee precise
    if (clicx != 0 && clicy != 0){
        MLV_draw_line(0, clicy, l, clicy, MLV_rgba(50, 150, 50, 255));
        MLV_draw_line(clicx, 0, clicx, h, MLV_rgba(50, 150, 50, 255));
    }
    
    // tracer la valeur en x et y pointee par la souris
    if (clicx != 0 && clicy != 0){
        valeur = (clicx-dx)-dec_x;   // je recentre ma valeur i-dx sur l'origine
        valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
        valeur += dec_x;   // je recentre ma valeur
        sprintf(valeurs,"(%.3lf, %.3lf)", valeur, (double) -(clicy-dec_y));
    }
    MLV_draw_text(l-150, 25, valeurs, MLV_rgba(50, 150, 50, 255));
    
    
    // trace le repere de l'axe des x
    MLV_draw_line(5, h-5, l, h-5, MLV_rgba(20, 12, 174, 255));
    // trace la graduation
    for (i=pas/nb_grad; i<l; i+=(pas/nb_grad)){
        if (i % pas == 0){
            valeur = (i-dx)-dec_x;   // je recentre ma valeur i-dx sur l'origine
            valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
            valeur += dec_x;   // je recentre ma valeur
            sprintf(text,"%.3lf",valeur);
            //DEBUG fprintf(stderr, "Texte x = %s\n", text);
            MLV_get_size_of_text(text, &tailleText, NULL);
            MLV_draw_line(i, h-5, i, h-15, MLV_rgba(20, 12, 174, 255));
            MLV_draw_text((i-tailleText/2), h-30, text, MLV_rgba(20, 12, 174, 255));
        }else{
            MLV_draw_line(i, h-5, i, h-10, MLV_rgba(20, 12, 174, 255));
        }
    }
    
    // trace le repere de l'axe des y
    MLV_draw_line(5, h-5, 5, 0, MLV_rgba(20, 12, 174, 255));
    // trace la graduation
    for (i=0; i<h/2; i+=(pas/nb_grad)){
        if (i % pas == 0){
            // graduation negative
            sprintf(text,"%d",-i);
            MLV_get_size_of_text(text, NULL, &tailleText);
            MLV_draw_line(5, dec_y+i, 15, dec_y+i, MLV_rgba(20, 12, 174, 255));
            MLV_draw_text(20, dec_y+i-tailleText/2, text, MLV_rgba(20, 12, 174, 255));
            // graduation positive
            sprintf(text,"%d",i);
            // DEBUG fprintf(stderr, "Texte y = %s\n", text);
            MLV_get_size_of_text(text, NULL, &tailleText);
            MLV_draw_line(5, dec_y-i, 15, dec_y-i, MLV_rgba(20, 12, 174, 255));
            MLV_draw_text(20, dec_y-i-tailleText/2, text, MLV_rgba(20, 12, 174, 255));
        }else{
            MLV_draw_line(5, dec_y+i, 10, dec_y+i, MLV_rgba(20, 12, 174, 255));
            MLV_draw_line(5, dec_y-i, 10, dec_y-i, MLV_rgba(20, 12, 174, 255));
        }
    }
}

// trace les notes a l'ecran pour plus de lisibilite
void tracer_notes(double dx, int dec_x, int dec_y, double zoom, int* notes, int taille, float* temps, int l, int h){
    int i;
    
    for (i=0; i<taille; i+=2){
        if (temps[notes[i]] != 0)
            tracerSegment(dx, dec_x, dec_y, zoom, temps[notes[i]]*ZOOM_X, 50, temps[notes[i]]*ZOOM_X, h-50, l, h, 0, MLV_rgba(255, 150, 0, 255));
    }

}

void recupere_mot(char mot[5], FILE* fich, int debug){    // recupere un mot de 4 octets (recupere l'ID d'un bloc)
    fread(mot, sizeof(char), 1, fich);
    fread(mot+1, sizeof(char), 1, fich);
    fread(mot+2, sizeof(char), 1, fich);
    fread(mot+3, sizeof(char), 1, fich);
    if (debug)
        fprintf(stderr, "[DEBUG] mot = %s\n", mot);   // affiche le mot trouvee pour debugage
}


short* signalPeriodique(short* amplitudes, int depart, int n, int repetition){
    int i, r;
    short* signal = (short*) malloc(sizeof(short)*(n*repetition));
    if (signal == NULL){
         fprintf(stderr, "wace.c::signalPeriodique()::Probleme d'allocation memoire\n");
         exit(-1);
    }
    
    for (r = 0; r<repetition; r++){
        for (i = 0; i<n; i++){
             signal[i+r*n] = amplitudes[depart+i];
        }
    }

    return signal;
}


int* decoupage_signal(short* amplitudes, float* temps, int nb_point, int* nb_n){
    int amp_max=0, amp_depart=0, amp_prec=0;
    int test1, test2;
    int bruit = 1;
    int i = 0, j = 0, nb_note = nb_point/10;
    
    float precision = .0;
    
    int* notes2;
    int* notes = (int*) malloc(sizeof(int) * nb_point);
    if (notes == NULL){
        fprintf(stderr, "wave.c::decoupe_signel()::probleme lors de l'allocation memoire\n");
        exit(-1);
    }
    
    // detecte les amplitudes les plus hautes en premier et les stocke
    while (i<nb_point-1){
        
        // recherche une amplitude basse
        do{
            // arrondi des amplitudes a tester (enlever de la precision seul la forme generale du signal nous interesse et non les valeures)
            test1 = amplitudes[i]*(375/32768.0); test2 = amplitudes[i+1]*(375/32768.0);
            i++;
            //fprintf(stderr, "Test1 = %d\n", test1);
        }while (test1 >= test2 && i < nb_point-1);
        
        // recherche une amplitude haute
        do{
            // arrondi des amplitudes a tester (enlever de la precision seul la forme generale du signal nous interesse et non les valeures)
            test1 = amplitudes[i]*(375/32768.0); test2 = amplitudes[i+1]*(375/32768.0);
            i++;
            //fprintf(stderr, "Test1 = %d\n", test1);
        }while (test1 <= test2 && i < nb_point-1);
        
        if (test1 > 9.0){
            // recupere l'amplitude max
            amp_prec = amp_max;
            amp_max = i-1;
        
            // arrondi des amplitudes a tester (enlever de la precision seul la forme generale du signal nous interesse et non les valeures)
            test1 = amplitudes[amp_max]*(375/32768.0); test2 = amplitudes[amp_prec]*(375/32768.0);
            
            // verifie si l'amplitude max ne serait pas un debut de note
            if (test1 >= test2 *(1.+precision*(test2/20.0))){
                /*if (!bruit){  // si pas on pas croiser de bruit alors on ajoute deux fois le point: 1 fin de la note 2 debut de la suivante
                    notes[j] = amp_max;
                    j++;
                    // recupere le depart de cette note
                    amp_depart = amp_max;
                }*/
                
                notes[j] = amp_max;
                j ++;
                //bruit = 0; // on a croiser une note donc pas de bruit
            }/*else if ((amplitudes[amp_depart]*(375/32768.0))/test2 < -0.5){    // verifie si on est pas en presence d'un bruit blanc: forte decroissance
                notes[j] = amp_max;
                j++;
                bruit = 1;  // on a croiser du bruit
                
                // recupere le depart de cette note
                amp_depart = amp_max;
            }*/
        }
        
        if (j > 0){
            fprintf(stderr, "amp_max = %f | amp_prec = %f\n", amplitudes[amp_max]*(375/32768.0), amplitudes[amp_prec]*(375/32768.0));
            fprintf(stderr, "notes[%d] = %d = %f\n", j-1, notes[j-1], amplitudes[notes[j-1]]*(375/32768.0));
            // sleep(1);
        }
    }
    
    notes2 = analyse_periode(notes, temps, j, &nb_note);
    
    /*// alloue un nouveau tableau
    notes2 = (int*) calloc(sizeof(int), j);
    if (notes2 == NULL){
        fprintf(stderr, "wave.c::decoupe_signel()::probleme lors de l'allocation memoire de notes2\n");
        exit(-1);
    }
    
    // reinitialise le compteur de note pour ajuster la taille du tableau final
    nb_note = 0; amp_prec = 0;
    // deuxieme phase du traitement: apres recuperation des periodes on regarde la duree de chaque periode pour detecter les variations
    for (i=2; i<j-2; i++){
        fprintf(stderr, "notes[%d] = %d = %f | notes[%d] = %d = %f\n", amp_prec, notes[amp_prec], amplitudes[notes[amp_prec]]*(375/32768.0), i, notes[i], amplitudes[notes[i]]*(375/32768.0));
        // sleep(1);
        
        if (notes[i] > 0){
            test1 = amplitudes[notes[amp_prec]]*(375/32768.0); test2 = amplitudes[notes[i]]*(375/32768.0);
            
            // verifie si l'amplitude max ne serait pas un debut de note
            if (notes[i]-notes[amp_prec] >= 44100){
                amp_prec = i;
                nb_note ++;
            }else if (test2 > test1 *(1.+0.1*(test1/20.0))){
                printf("ici\n");
                //notes[amp_prec] = -1;
                //notes[amp_prec+1] = -1;
                amp_prec = i;
                nb_note ++;
            }else{
                notes[i] = -1;
                notes[i+1] = -1;
                //nb_note ++;
            }
        }
        
    }*/
    
    /*notes = nettoyage(notes, nb_point, 2*nb_note);    

    for (i=0; i<nb_note*2; i++)
        fprintf(stderr, "notes[%d] = %d = %f\n", i, notes[i], amplitudes[notes[i]]*(375/32768.0));
    */
    
    // libere le tableau temporaire 'notes'
    
    free(notes);
    fprintf(stderr, "ici\n");
    
    //for (i = 0; i<j; i++)
     //   fprintf(stderr, "notes[%d] = %d = %f\n", i, notes[i], amplitudes[notes[i]]*(375/32768.0));
    
    *nb_n = nb_note;
    return notes2;
}


int* nettoyage(int* notes, int t1, int t2){
    int i,j = 0;
    int* notes2 = NULL;
/*    int* notes2 = (int*) calloc(sizeof(int), t2);
    if (notes2 == NULL){
        fprintf(stderr, "wave.c::nettoyage()::probleme lors de la l'allocation memoire du tableau de notes\n");
        exit(-1);
    }
    
    printf("Nb_note = %d\n", t2);
    
    for (i=0; i<t1; i++){
         if (notes[i] > 0){
              notes2[j] = notes[i];
              j ++;
         }
    }*/
    
    
    return notes2;
}


int* analyse_periode(int* periodes, float* temps, int nb_periode, int* nb_note){
    float periode_prec, debut, fin;
    int i,j = 1;
    
    // alloue un nouveau tableau
    int* notes = (int*) calloc(sizeof(int), nb_periode);
    if (notes == NULL){
        fprintf(stderr, "wave.c::analyse_periode()::probleme lors de l'allocation memoire\n");
        exit(-1);
    }

    // ajoute obligatoirement une note au debut    
    *nb_note = 0;
    *nb_note += 1;

    periode_prec = temps[periodes[1]]-temps[periodes[0]];
    notes[0] = periodes[0];

    // deuxieme phase du traitement: apres recuperation des periodes on regarde la duree de chaque periode pour detecter les variations
    for (i=2; i<nb_periode-2; i++){
        // recupere le debut et la fin de la nouvelle periode
        debut = temps[periodes[i]];
        fin = temps[periodes[i+1]];

        fprintf(stderr, "indice = %d | max_indice = %d | periodes precedente = %f | nouvelle periode = %f\n", i, nb_periode, periode_prec, fin-debut);
        // sleep(1);
        
        if (fin-debut > periode_prec*1.06 || fin-debut < periode_prec*1.06){
            notes[j] = periodes[i];
            notes[j+1] = periodes[i];
            j += 2;
            *nb_note += 2;
        }
        
        periode_prec = fin-debut;
        
    }
    
    
    return notes;
    
}


/*void clavier(unsigned char c, int i, int j){   // fonction gerant les evenements clavier

}*/
