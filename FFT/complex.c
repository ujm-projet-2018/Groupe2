#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "complex.h"


complex* creer_complex(float reel, float imaginaire){
    complex* c = (complex*) malloc(sizeof(complex));   
    if (c == NULL){
         fprintf(stderr, "complex.c::creer_complex()::probleme d'allocation memoire\n");
         exit(-1);
    }
    
    c->a = reel;
    c->b = imaginaire;
    
    return c;
}


complex* add_complex(complex* c1, complex* c2){
    
    if (c1 == NULL || c2 == NULL){
        fprintf(stderr, "complex.c::add_complex()::argument NULL\n");
        exit(-1);
    }
    
    return creer_complex(c1->a+c2->a, c1->b+c2->b);
}



complex* mult_complex(complex* c1, complex* c2){
    
    if (c1 == NULL || c2 == NULL){
        fprintf(stderr, "complex.c::mult_complex()::argument NULL\n");
        exit(-1);
    }
    
    return creer_complex(c1->a*c2->a - c1->b*c2->b, c1->a*c2->b + c1->b*c2->a);
}


double module(complex* c){
    return sqrt(c->a*c->a+c->b*c->b);
}


double argument(complex* c){
    return 2*atan(c->b/(c->a+module(c)));
}
    

complex** tab_complex(short* tab_a, float* tab_b, int n){
    int i;
    complex** tab_c = (complex**) malloc(n*sizeof(complex*)); 
    if (tab_c == NULL){
         fprintf(stderr, "complex.c::tab_complex()::probleme d'allocation memoire\n");
         exit(-1);
    } 
    
    for (i=0; i<n; i++){
        if (tab_a == NULL && tab_b == NULL)
            tab_c[i] = creer_complex(0.0f, 0.0f);
        else if (tab_a == NULL)
            tab_c[i] = creer_complex(0.0f, tab_b[i]);
        else if (tab_b == NULL)
            tab_c[i] = creer_complex((float) tab_a[i], 0.0f);
        else tab_c[i] = creer_complex((float) tab_a[i], tab_b[i]);
    }
    
    return tab_c;
}
