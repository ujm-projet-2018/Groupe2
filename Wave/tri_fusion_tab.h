#include <stdio.h>
#include <stdlib.h>

typedef double elem;



elem* tri_fusion(elem *tab,int nbel,int (*compar)(elem,elem));
elem* tri_fusion_bis(elem *tab1, int nbel,elem *tab2, int nbel2,int (*compar)(elem,elem));
