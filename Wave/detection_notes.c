#include "detection_notes.h"
#include <string.h>
int inf(elem e1,elem e2){
  return e1<e2;
}
  

char* detectionnotes(double ff){
  double notes[12][8]={{32.7 ,  65.41, 130.81, 261.63, 523.25, 1046.50, 2093.00, 4186.01},
		       {34.65,  69.30, 138.59, 277.18, 554.37, 1108.73, 2217.46, 4434.92},
		       {36.71,  73.42, 146.83, 293.00, 587.33, 1174.66, 2349.32, 4698.64},
		       {38.89,  77.78, 155.56, 311.13, 622.25, 1244.51, 2489.02, 4978.03},
		       {41.20,  82.41, 164.81, 329.63, 659.26, 1318.51, 2637.02, 5274.04},
		       {43.65,  87.31, 174.61, 349.23, 698.46, 1396.91, 2793.83, 5587.65},
		       {46.25,  92.50, 185.00, 369.99, 739.99, 1479.98, 2959.96, 5919.91},
		       {49.00,  98.00, 196.00, 392.00, 783.99, 1567.98, 3135.96, 6271.93},
		       {51.91, 103.83, 207.65, 415.30, 830.61, 1661.22, 3322.44, 6644.88},
		       {55.00, 110.00, 220.00, 440.00, 880.00, 1760.00, 3520.00, 7040.00},
		       {58.27, 116.54, 233.08, 466.16, 932.33, 1864.66, 3729.31, 7458.62},
		       {61.74, 123.47, 246.94, 493.88, 987.77, 1975.53, 3951.07, 7902.13}};
  char *notesS[]=      {"do","do#","re","re#","mi","fa","fa#","sol","sol#","la","la#","si"};
  int i,j,fin;
  char *note = (char*) malloc(sizeof(char)*20);
  fin = 0;
  for(j=0;j<8;j++){
    for(i=0;i<12;i++){
      if(i == 11){
	if(ff < (notes[i][j]+(notes[0][j+1] - notes[i][j])/2) && ff > (notes[i][j]-(notes[i][j]-notes[i-1][j])/2) ) {
	  sprintf(note,"%s %d",notesS[i],j);
	  //printf("note : %s %d\n",notesS[i],j);
	    fin = 1;
	    break;
	}
      }
      else if(i == 0){
	if(ff < (notes[i][j]+(notes[i+1][j] - notes[i][j])/2) && ff > (notes[i][j]-(notes[i][j]-notes[11][j-1])/2) ) {
	  if(i == 0){
	    sprintf(note,"%s %d",notesS[0],j);
	    //printf("note : %s %d\n",notesS[0],j);
	    fin = 1;
	    break;
	  }
	}
      }
      else if(ff < (notes[i][j]+(notes[i+1][j] - notes[i][j])/2) && ff > (notes[i][j]-(notes[i][j]-notes[i-1][j])/2) ) {
	sprintf(note,"%s %d",notesS[i],j);
	//printf("note : %s %d\n",notesS[i],j);
	fin = 1;
	break;
      }
    }
    if(fin == 1){
      break;
    }
  }
  return note;
}


double cleanFrequence2(double *tabfreq,int nfre){
  double *tab = tri_fusion(tabfreq,nfre,inf);
  return tab[nfre/2];
  
}

double cleanFrequence3(double *tabfreq,int nfre){
  int del = nfre*0.01,i;
  //printf("del %d\n",del);
  double frequencemax = 0;
  double *tab = tri_fusion(tabfreq,nfre,inf);
  tab = &tab[del];
  nfre -= del;
  for(i = 0;i<nfre;i++){
    frequencemax += tabfreq[i];
  }
  frequencemax /= nfre;
  return frequencemax;
  
}

double cleanFrequence4(double *tabfreq,int nfre){
  int maxcompt = 0,max,i;
  double /*frequencemax = 0,*/vmax = 0,v=0;
  double *tab = tri_fusion(tabfreq,nfre,inf);

  v = tab[0];
  max = 1;

  for(i = 1;i<nfre;i++){
    if((int)tabfreq[i]==(int)v){
      max++;
    }
    else{
      if(max > maxcompt){
	maxcompt = max;
	vmax = tabfreq[i-1];
      }
      v = tabfreq[i-1];
      max = 0;
    }
  }
  return vmax;
  
}

double cleanFrequence(double *tabfreq,int nfre){
  int i,j;
  double frequencemax = 0;
  for(i = 0;i<nfre;i++){
    frequencemax += tabfreq[i];
  }
  frequencemax /= nfre;
  for(i = 0; i<nfre;i++){
    if(tabfreq[i] < frequencemax/2 || tabfreq[i] > frequencemax/2){
      for(j=i;j+1 < nfre;j++){
	tabfreq[j] = tabfreq[j+1];
      }
      nfre --;
    }
  }
  frequencemax = 0;
  for(i = 0;i<nfre;i++){
    frequencemax += tabfreq[i];
  }
  frequencemax /= nfre;

  return frequencemax;
}

signal analyse_notes(short *amp, float *temps, int nb_point,int deb, int fin){
  int i;
  short am;

  signal s;
  s.ntfre = 0;
  s.nfre = 0;
  
  int ampmax = 0, /**tfre, ntfre = 0,nfre=0,*/ ampmaxold = 0;
  double frequence = 0, frequencemax = -1000000, t1 = 0/*,*tabfreq*/;
  float t2 = 0.0;
  s.tfre = (int*) malloc(sizeof(int)*1);
  s.tabfreq = (double*) malloc(sizeof(double)*1);
  for(i=deb;i<fin;i++){
    am = amp[i];
    am = amp[i]/500;
    if(am > 1){
      am*=10000;
    }
    t2 = temps[i];
    if (ampmax == 0){
      ampmax = am;
    }
    if(am > ampmax-(ampmax*0.30) && am < ampmax*1.30 && t2 > t1+0.001){
      if(ampmax != 0){
	frequence = 1/(t2-t1); 
	if(frequence > frequencemax){  // enregistre la nouvelle frequence max
	  // recupere les indices de la frequence maximale
	  frequencemax = frequence;
	}
      }
      s.ntfre ++;
      s.nfre ++;
      s.tfre = realloc(s.tfre,sizeof(int)*s.ntfre);
      s.tabfreq = realloc(s.tabfreq,sizeof(double)*s.nfre);
      s.tabfreq[s.nfre-1] = frequence;
      s.tfre[s.ntfre-1] = nb_point;
      ampmaxold = ampmax;
      ampmax = am;
      t1 = t2;
      
    }else if (am > ampmax ){
      ampmax = am;
    }
  }

  frequencemax = cleanFrequence(s.tabfreq,s.nfre);
  //printf("Frequence 1 : %f\n",frequencemax);
  detectionnotes(frequencemax);
  frequencemax = cleanFrequence2(s.tabfreq,s.nfre);
  //printf("\nFrequence 2 : %f\n",frequencemax);
  detectionnotes(frequencemax);
  frequencemax = cleanFrequence3(s.tabfreq,s.nfre);
  //printf("\nFrequence 3 : %f\n",frequencemax);
  detectionnotes(frequencemax);
  //frequencemax = cleanFrequence4(s.tabfreq,s.nfre);
  //printf("\nFrequence 4 : %f\n",frequencemax);
  s.note = detectionnotes(frequencemax);
  return s;
  
}
