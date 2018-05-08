#ifndef FFT_H_INCLUDED
#define FFT_H_INCLUDED

// fonction calculant le spectre a partir des amplitudes d'un signal
complex* analyseFFT(short* amplitudes, int depart, int N, int taille_tab);

// trace le spectre calcule par la fonction de dessus
void tracer_spectre(complex* fft, int clicx, int clicy, int n, double dx, double zoomx, int dec_x, int dec_y, double echelley, int freqEch, int l, int h);

// fonction calculant un spectre et renvoyant la frequence la plus interessante
float analyse_notes_FFT(short* amplitudes, int debut, int fin, int nb_point_fourier, int freqEch);

#endif

