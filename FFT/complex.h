#ifndef COMPLEX_H_INCLUDED
#define COMPLEX_H_INCLUDED

typedef struct {
    float a;
    float b;
} complex;



complex* creer_complex(float reel, float imaginaire);

complex* add_complex(complex* c1, complex* c2);

complex* mult_complex(complex* c1, complex* c2);

double module(complex* c);

double argument(complex* c);

complex** tab_complex(short* tab_a, float* tab_b, int n);


#endif
