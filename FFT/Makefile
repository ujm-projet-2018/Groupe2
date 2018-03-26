

spectre: fft.c complex.o
	gcc -Wall -g `pkg-config --cflags MLV` `pkg-config --libs-only-other --libs-only-L MLV` fft.c complex.o -o fft `pkg-config --libs-only-l MLV` -lm

complex.o: complex.c complex.h
	gcc -c complex.c -o complex.o


clean: 
	rm -f *~ *.o fft
