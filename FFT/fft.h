#ifndef FFT_H_INCLUDED
#define FFT_H_INCLUDED


complex* analyseFFT(short* amplitudes, int depart, int N, int taille_tab);

void tracer_spectre(complex* fft, int clicx, int clicy, int n, double dx, double zoomx, int dec_x, int dec_y, double echelley, int freqEch, int l, int h);


#endif

