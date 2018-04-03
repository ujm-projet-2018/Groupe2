#include "tri_fusion_tab.h"

elem* tri_fusion(elem *tab,int nbel,int (*compar)(elem,elem)){
  int nbel2 = 0;
  elem * tab2 = NULL;
  if(nbel < 2){
    return tab;
  }
  nbel2 = nbel - (nbel/2);
  tab2 = &tab[nbel/2];
  return tri_fusion_bis(tri_fusion(tab,nbel/2,compar),nbel/2,tri_fusion(tab2,nbel2,compar),nbel2,compar);
  
}
elem* tri_fusion_bis(elem *tab1, int nbel,elem *tab2, int nbel2,int (*compar)(elem,elem)){
  int i;
  elem *tab,*tmp;
  tab = (elem*)malloc(sizeof(elem)*(nbel+nbel2));
  //tmp = (elem*)malloc(sizeof(elem)*(nbel+nbel2)-1);
  if(nbel == 0) return tab2;
  if(nbel2 == 0) return tab1;
  if(compar(tab1[0],tab2[0]) ){
    tab[0] = tab1[0];
    nbel --;
    tmp = tri_fusion_bis(&tab1[1],nbel,tab2,nbel2,compar);
    for(i=1;i<nbel+nbel2+1;i++){
      tab[i]=tmp[i-1];
    }
    return tab;
  }
  else{
    tab[0] = tab2[0];
    nbel2--;
    tmp = tri_fusion_bis(tab1,nbel,&tab2[1],nbel2,compar);
    for(i=1;i<nbel+nbel2+1;i++){
      tab[i]=tmp[i-1];
    }
    return tab;
  }
}
