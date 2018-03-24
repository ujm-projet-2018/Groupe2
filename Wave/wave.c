#include <stdlib.h>
#include <stdio.h>
#include <MLV/MLV_all.h>
#include <unistd.h>

//#include "GL/gl.h"
//#include "GL/glut.h"

#define ZOOM_X 1000.0
#define VITESSE_DX 10
#define VITESSE_ZOOM 1.05



//void clavier(unsigned char c, int i, int j);
void tracer_repere(int clicx, int clicy, double dx, double zoomx, int dec_x, int dec_y, int l, int h);
void recupere_mot(char mot[5], FILE* fich, int debug);
void tracerCourbe(int clicx, int clicy, double dx, int dec_x, int dec_y, double zoom, short* amplitude, float* temps, int nb_point, int filtre, int l, int h, int anim, int verbeux, int debug,int *tfre,int ntfre);
void tracerSegment(double dx, int dec_x, int dec_y, double zoom, double x1, double y1, double x2, double y2, int l, int h,int bool);
void tracerPoint(double dx, int dec_x, double zoom, double x1, double y1, int l, int h);



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
    int compteur = 0, arret = 0, nb_point = 0, dec_x, dec_y, clicx = 0, clicy = 0;   
    int freqEch, echantillon, defausse_entier, bytePerSec, taille, longueur;   // donnees du fichier WAVE
    int filtre = 1, precision = 1, l = 1200, h = 750, op_son = 0, anim = 0, debug = 0, verbeux = 0;   // les options
    int op;    /* sert a determiner les options selectionner */

    int ampmax = 0,*tfre,ntfre = 0;
    double frequence = 0, frequencemax = -1000000,t1 = 0,t2 = 0,t1max,t2max;
    
    short* amplitudes;
    float* temps;
    double zoom = 1.0, dx = 0.0;
    
    short amp, defausse_short, nbCanaux, bitsPerSample, bytePerBloc;
    char* nomFich = NULL;
    char mot[5];   // permet de recuperer les mot de longueur 4 qui servent a identifier chaque bloc
    mot[4] = '\0';   //rajoute la fin du mot directement
    FILE* fich;
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
        break;
      case 'h':  /* precise la hauteur de la fenetre */
        h = atoi(optarg);
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
        sleep(1);
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
    tfre = (int*) malloc(sizeof(int)*1);
    // attent que les informations aient ete lu
    sleep(3);
    
    // boucle principale lance une fois l'entete du fichier decrypte: analyse des donnees
    
    while (compteur*(bitsPerSample/8) < echantillon){
         // remets tous les bits du short 'amp' a 0
         amp = 0;
         // lecture des donnees du fichier WAVE
         fread(&amp, sizeof(char)*(bitsPerSample/8), 1, fich);
         
         // Lucas fondamental***********************************************************
	 t2 = (compteur/(bitsPerSample/8))*(1.0/freqEch);
	 if (ampmax == 0){
	   ampmax = amp;
	 }
	 if(amp > ampmax-(ampmax*0.05) && amp < ampmax*1.05 && t2 > t1+0.001){
	   if(ampmax != 0){
	     if(t2-t1!=0){
	       frequence = 1/(t2-t1);
	     }else frequence = 1/t2;
	     if(frequence > frequencemax){
	       t1max = t1;
	       t2max = t2;
	       frequencemax = frequence;
	     }
	   }
	   ntfre ++;
	   tfre = realloc(tfre,sizeof(int)*ntfre);
	   tfre[ntfre-1] = compteur/filtre;
	   ampmax = amp;
	   t1 = t2;
	     
	 }
	 else if (amp > ampmax ){
	   t1 = t2;
	   ampmax = amp;
	 }
	 //*****************************************************************************
	 // affichage des infos sur console
	 if (verbeux){
	   
	   fprintf(stderr, "Temps: %lf\n", t2);
	   fprintf(stderr, "Amplitude: %hd\n\n", amp);
	   
         }
         // remplissage du tableau
         amplitudes[compteur/filtre] = amp;
         temps[compteur/filtre] = (compteur/(bitsPerSample/8))*(1.0/freqEch);
         
         // gestion des compteurs
         compteur += filtre;
         
         // on avance de filtre-1 elements dans le fichier (accelere le traitement)
         fseek(fich, (long) ((filtre-1)*(bitsPerSample/8)), SEEK_CUR);
    }
    
    //MLV_Keyboard_button touche;
    if (son != NULL && op_son)
        MLV_play_sound(son, 1.0f);
    tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, anim, verbeux, debug,tfre,ntfre);
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
             tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,tfre,ntfre);
         }else if (MLV_get_keyboard_state(275) == MLV_PRESSED){   // fleche gauche
             dx -= VITESSE_DX*(1.0/zoom);
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,tfre,ntfre);
         }else if (MLV_get_keyboard_state(274) == MLV_PRESSED){     // fleche bas
             zoom /= VITESSE_ZOOM;
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,tfre,ntfre);
         }else if (MLV_get_keyboard_state(273) == MLV_PRESSED){      // fleche haut
             zoom *= VITESSE_ZOOM;
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,tfre,ntfre);
         }else if (MLV_get_mouse_button_state(MLV_BUTTON_LEFT) == MLV_PRESSED){
             MLV_get_mouse_position(&clicx, &clicy);
             // nettoyage de la fenetre   
             MLV_clear_window(MLV_rgba(0, 0, 0, 255));
             // appel la fonction de tracer
             tracerCourbe(clicx, clicy, dx, dec_x, dec_y, zoom, amplitudes, temps, nb_point, precision, l, h, 0, verbeux, debug,tfre,ntfre);
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

void tracerCourbe(int clicx, int clicy, double dx, int dec_x, int dec_y, double zoom, short* amplitude, float* temps, int nb_point, int filtre, int l, int h, int anim, int verbeux, int debug,int *tfre,int ntfre){
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
        tracerSegment(dx, dec_x, dec_y, zoom, oldTemps*ZOOM_X, (dec_y+oldAmp*dec_y/32767.0), temps[i+1]*ZOOM_X, (double) (dec_y+amplitude[i+1]*(dec_y/32767.0)), l, h,bool);    // trace le segment entre le dernier point et le point actuel
        
        // recuperer le dernier point tracer
        oldTemps = temps[i+1]; oldAmp = amplitude[i+1];
            
        // anim le tracer de la courbe
        if (anim && (oldTemps*ZOOM_X)<l){
            MLV_wait_milliseconds(1);
            MLV_actualise_window();
           //glFlush();
        }
    }
    
    // tracer du repere    
    tracer_repere(clicx, clicy, dx, zoom, dec_x, dec_y, l, h);

    // reactualise l'affichage
    MLV_actualise_window();
    // affiche pour le debugage le nombre de point traces a l'ecran
    /*if (verbeux)
      fprintf(stderr, "Nombre de points traces %d\n", nb_point/filtre);*/
}

void tracerSegment(double dx, int dec_x, int dec_y, double zoom, double x1, double y1, double x2, double y2, int l, int h,int Tfre){
    // transformation de la coordonnee x1 (zoom + decalage du zoom) 
    x1 = (dx+x1-dec_x)*zoom;
    x1 += dec_x;
    // transformation de la coordonnee x2 (zoom + decalage du zoom)
    x2 = (dx+x2-dec_x)*zoom;
    x2 += dec_x;
    
    // tracer du segment grace a MLV
    if (x1>=0 && x2>=0 && y1>=0 && y2>=0 && x1<=l && x2<=l && y1<=h && y2<=h)
        MLV_draw_line(x1, -(y1-dec_y)+dec_y, x2, -(y2-dec_y)+dec_y, MLV_rgba(255,0,0,255));
    /********************Lucas signalisation de la fréquence***************************/
    if(Tfre == 1){
      MLV_draw_line(x1,0,x1,600,MLV_rgba(255,255,255,255));
    }
    /**********************************************************************************/
    
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
        MLV_draw_line(0, clicy, l, clicy, MLV_rgba(0, 255, 0, 255));
        MLV_draw_line(clicx, 0, clicx, h, MLV_rgba(0, 255, 0, 255));
    }
    
    // tracer la valeur en x et y pointee par la souris
    if (clicx != 0 && clicy != 0){
        valeur = (clicx-dx)-dec_x;   // je recentre ma valeur i-dx sur l'origine
        valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
        valeur += dec_x;   // je recentre ma valeur
        sprintf(valeurs,"(%.3lf, %.3lf)", valeur, (double) -(clicy-dec_y));
    }
    MLV_draw_text(l-150, 25, valeurs, MLV_rgba(0, 255, 0, 255));
    
    
    // trace le repere de l'axe des x
    MLV_draw_line(5, h-5, l, h-5, MLV_rgba(0, 0, 255, 255));
    // trace la graduation
    for (i=pas/nb_grad; i<l; i+=(pas/nb_grad)){
        if (i % pas == 0){
            valeur = (i-dx)-dec_x;   // je recentre ma valeur i-dx sur l'origine
            valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
            valeur += dec_x;   // je recentre ma valeur
            sprintf(text,"%.3lf",valeur);
            MLV_get_size_of_text(text, &tailleText, NULL);
            MLV_draw_line(i, h-5, i, h-15, MLV_rgba(0, 0, 255, 255));
            MLV_draw_text((i-tailleText/2), h-30, text, MLV_rgba(0, 0, 255, 255));
        }else{
            MLV_draw_line(i, h-5, i, h-10, MLV_rgba(0, 0, 255, 255));
        }
    }
    
    // trace le repere de l'axe des y
    MLV_draw_line(5, h-5, 5, 0, MLV_rgba(0, 0, 255, 255));
    // trace la graduation
    for (i=0; i<h/2; i+=(pas/nb_grad)){
        if (i % pas == 0){
            // graduation negative
            sprintf(text,"%d",-i);
            MLV_get_size_of_text(text, NULL, &tailleText);
            MLV_draw_line(5, dec_y+i, 15, dec_y+i, MLV_rgba(0, 0, 255, 255));
            MLV_draw_text(20, dec_y+i-tailleText/2, text, MLV_rgba(0, 0, 255, 255));
            // graduation positive
            sprintf(text,"%d",i);
            MLV_get_size_of_text(text, NULL, &tailleText);
            MLV_draw_line(5, dec_y-i, 15, dec_y-i, MLV_rgba(0, 0, 255, 255));
            MLV_draw_text(20, dec_y-i-tailleText/2, text, MLV_rgba(0, 0, 255, 255));
        }else{
            MLV_draw_line(5, dec_y+i, 10, dec_y+i, MLV_rgba(0, 0, 255, 255));
            MLV_draw_line(5, dec_y-i, 10, dec_y-i, MLV_rgba(0, 0, 255, 255));
        }
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

/*void clavier(unsigned char c, int i, int j){   // fonction gerant les evenements clavier

}*/



