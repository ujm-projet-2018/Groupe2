

spectre: fft.c 
	gcc -Wall -g `pkg-config --cflags MLV` `pkg-config --libs-only-other --libs-only-L MLV` fft.c -o fft `pkg-config --libs-only-l MLV` -lm

clean: 
	rm -f *~ *.o fft
