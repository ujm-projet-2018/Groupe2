#include "tri_fusion_tab.h"

typedef struct signal{
  int*tfre;
  double *tabfreq;
  int ntfre;
  int nfre;
  char *note;
}signal;

char* detectionnotes(double ff);
double cleanFrequence2(double *tabfreq,int nfre);
double cleanFrequence3(double *tabfreq,int nfre);
double cleanFrequence4(double *tabfreq,int nfre);
double cleanFrequence(double *tabfreq,int nfre);
signal analyse_notes(short *amp, float *temps, int nb_point,int deb,int fin);
