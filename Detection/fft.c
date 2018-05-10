#include <MLV/MLV_all.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

#define PI 3.14156


void tracer_repere_spectre(int clicx, int clicy, double dx, double zoomx, int dec_x, int dec_y, double max, int n, int freqEch, int l, int h);

// cherche le module max du tableau de complex
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
     int i;
     
     complex* tab = (complex*) malloc(sizeof(complex)*n);
     if (tab == NULL){
         fprintf(stderr, "fft.c::recupere_point_complexe()::probleme d'allocation memoire\n");
         exit(-1);
     }
     
     // si le tableau est null alors on rempli de 0
     if (amplitudes == NULL){
          for (i=0; i<n; i++){
              tab[i] = 0+I*0;
          }
          
          return tab;
     }
     
     // rempli a partir des donnees du tableau
     for (i=0; i<n; i++){
         tab[i] = amplitudes[depart+i]+I*0;
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
        y[m] = freq[k]; 
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
        W = cos(phi)+sin(phi)*I;
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


complex* analyseFFT(short* amplitudes, int depart, int fin, int q){
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


void tracerSegmentFFT(double dx, int dec_x, int dec_y, double zoom, double x1, double y1, double x2, double y2, int l, int h){

    // transformation de la coordonnee x1 (zoom + decalage du zoom) 
    x1 = (dx+x1-dec_x)*zoom;
    x1 += dec_x;

    // transformation de la coordonnee x2 (zoom + decalage du zoom)
    x2 = (dx+x2-dec_x)*zoom;
    x2 += dec_x;
    
    // tracer du segment grace a MLV
    if (x1>=0 && x2>=0 && y1>=0 && y2>=0 && x1<=l && x2<=l && y1<=h && y2<=h)
        MLV_draw_line((int) x1, (int) (h-dec_y)-y1, (int) x2, (int) (h-dec_y)-y2, MLV_rgba(150,20,20,255));
    
}


void tracer_spectre(complex* fft, int clicx, int clicy, int n, double dx, double zoomx, int dec_x, int dec_y, double echelley, int freqEch, int l, int h){
     int indice;
     double i;
     double max = tab_max(fft, n);
     double x = 0, y = cabs(fft[0])*echelley/max;
     double T = 1.0/freqEch;  // duree d'un echantillon
     
     for (i = 0.0; i<8000.0/*n/2-1*/; i += 1/(T*n)){
          indice = (i+1)*T*n;
          tracerSegmentFFT(dx, l/2, dec_y, zoomx, x, y, (i+1)/8.0, cabs(fft[indice])*echelley/max, l, h);
          x = (i+1)/8.0;
          y = cabs(fft[indice])*echelley/max;
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
        MLV_draw_line(0, clicy, l, clicy, MLV_rgba(50, 150, 50, 255));
        MLV_draw_line(clicx, 0, clicx, h, MLV_rgba(50, 150, 50, 255));
    }
    
    // tracer la valeur en x et y pointee par la souris
    if (clicx != 0 && clicy != 0){
        valeur = (clicx-dx)-czoom;   // je recentre ma valeur x-dx sur l'origine
        valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
        valeur += czoom;   // je repositionne ma valeur
        valeur = valeur*8;
        sprintf(valeurs,"(%.3lf, %.3lf)", valeur, (double) (h-dec_y-clicy)*(max/h));
    }
    MLV_draw_text(l-150, 25, valeurs, MLV_rgba(50, 150, 50, 255));
    
    // trace le repere de l'axe des x
    MLV_draw_line(5, h-5, l, h-5, MLV_rgba(50, 150, 50, 255));
    // trace la graduation
    for (i=pas/nb_grad; i<l; i+=(pas/nb_grad)){
        if (i % pas == 0){
            valeur = (i-dx)-czoom;   // je recentre ma valeur x-dx sur l'origine
            valeur /= zoomx;    // je fais la mise a l'echelle inverse au zoom
            valeur += czoom;   // je repositionne ma valeur
            valeur = valeur*8;
            sprintf(text,"%.3lf", valeur);
            MLV_get_size_of_text(text, &tailleText, NULL);
            MLV_draw_line(i, h-5, i, h-15, MLV_rgba(20, 12, 174, 255));
            if (valeur > 0 && valeur <= 8000){
                 MLV_draw_text((i-tailleText/2), h-30, text, MLV_rgba(20, 12, 174, 255));
            }
        }else{
            MLV_draw_line(i, h-5, i, h-10, MLV_rgba(20, 12, 174, 255));
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


float analyse_notes_FFT(short* amplitudes, int debut, int fin, int nb_point_fourier, int freqEch){
    int i;
    int i_max = 0, i_max2 = 0;
    float freq;
    double freq1, freq2;
    double T = 1.0/freqEch;  // la periode d'un echantillon
    double rapport_limite = 5;
    complex* fft = NULL;
    
    // lancement de l'analyse
    fft = analyseFFT(amplitudes, debut, fin, log2(nb_point_fourier));
    
    // parcours le tableau a la recherche du max dans la premiere moitie du spectre: travail sur le module du complex
    for (i = 0; i<nb_point_fourier/2; i++){
        if (cabs(fft[i]) > cabs(fft[i_max])){
            i_max = i;
        }
    }
    
    // on cherche a recuperer l'harmonique 2 inferieur (la plus grande possible) par rapport a la frequence deja trouvee
    for (i = 0; i<nb_point_fourier/2; i++){         
        freq1 = i_max/(T*nb_point_fourier);
        freq2 = i/(T*nb_point_fourier);
        if (2*freq2 > freq1*0.98 && 2*freq2 < freq1*1.02){
            i_max2 = i;
        }
    }
    
    // verifie si i_max2 est proche de i_max
    if (cabs(fft[i_max])/cabs(fft[i_max2]) < rapport_limite)
        i_max = i_max2;  // i_max2 est prefere a i_max
    
    // calcul la frequence
    freq = i_max/(T*nb_point_fourier);
    
    // destruction du spectre
    if (fft != NULL)
        free(fft);
    
    return freq;    

}

