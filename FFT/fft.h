#ifndef FFT_H_INCLUDED
#define FFT_H_INCLUDED



/* 
    Fonction: analyse un signal en partant de debut pour renvoyer le spectre grace a la FFT
    Entree: un tableau representant les amplitudes du signal, un indice de depart, un indice de fin et le log2 du nombre de point a considerer pour l'analyse
    Sortie: un tableau de nombre complex resultat de la FFT
*/
complex* analyse(short* amplitudes, int depart, int fin, int N);

/* 
    Fonction: cree un tableau de nombre complex de n elements a partir du tableau des amplitudes et de l'indice depart
    Entree: un tableau representant les amplitudes du signal, un indice de depart, un indice de fin et le nombre de point a prendre a partir de debut
    Sortie: un tableau de nombre complex avec pour parti reel l'amplitude du signal et pour parti imaginaire 0
*/
complex* recupere_point_complex(short* amplitudes, int depart, int fin, int n);

/* 
    Fonction: trace le spectre renvoye par la FFT
    Entree: un tableau de nombres complexes representant le spectre, les coordonnees d'un clic de souris, le nombre de point de la FFT, une valeur de deplacement,
            un zoom sur les x, un decalage constant sur les x et les y, l'echelle de l'intensite d'une frequence lors du tracer, la frequence d'echantillonage,
            la largeur et la hauteur de la fenetre
    Sortie: X
*/
void tracer_spectre(complex* fft, int clicx, int clicy, int n, double dx, double zoomx, int dec_x, int dec_y, double echelley, int freqEch, int l, int h);

/* 
    Fonction: trace le repere du spectre renvoye par la FFT
    Entree: les coordonnees d'un clic de souris, une valeur de deplacement, un zoom sur les x, un decalage constant sur les x et les y, l'intensite max des frequences,
            la frequence d'echantillonage, la largeur et la hauteur de la fenetre
    Sortie: X
*/
void tracer_repere_spectre(int clicx, int clicy, double dx, double zoomx, int dec_x, int dec_y, double max, int n, int freqEch, int l, int h);

/* 
    Fonction: sert a recuperer un entete de bloc lors de la lecture du fichier WAVE
    Entree: un tableau de caracteres dans lequel stocker le resultat, le fichier a lire, un mode servant a afficher sur le terminal la valeur de ce mot
    Sortie: X
*/
void recupere_mot(char mot[5], FILE* fich, int debug);

#endif

