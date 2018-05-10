#ifndef FFT_H_INCLUDED
#define FFT_H_INCLUDED



/* 
    Fonction: analyse un signal en partant de debut pour renvoyer le spectre grace a la FFT
    Entree: un tableau representant les amplitudes du signal, un indice de depart, un indice de fin et le log2 du nombre de point a considerer pour l'analyse
    Sortie: un tableau de nombre complex resultat de la FFT
*/
complex* analyseFFT(short* amplitudes, int depart, int N, int taille_tab);

/* 
    Fonction: trace le spectre renvoye par la FFT
    Entree: un tableau de nombres complexes representant le spectre, les coordonnees d'un clic de souris, le nombre de point de la FFT, une valeur de deplacement,
            un zoom sur les x, un decalage constant sur les x et les y, l'echelle de l'intensite d'une frequence lors du tracer, la frequence d'echantillonage,
            la largeur et la hauteur de la fenetre
    Sortie: X
*/
void tracer_spectre(complex* fft, int clicx, int clicy, int n, double dx, double zoomx, int dec_x, int dec_y, double echelley, int freqEch, int l, int h);

/* 
    Fonction: analyse un signal en partant de debut pour renvoyer la note correspondant a ce morceau de signal
    Entree: un tableau representant les amplitudes du signal, un indice de depart, un indice de fin, le nombre de point a considerer pour l'analyse et
            la frequence d'echantillonage pour calculer des frequences
    Sortie: un reel representant une frequence
*/
float analyse_notes_FFT(short* amplitudes, int debut, int fin, int nb_point_fourier, int freqEch);

#endif

