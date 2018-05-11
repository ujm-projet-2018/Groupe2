#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>

#define PI 3.14156



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

