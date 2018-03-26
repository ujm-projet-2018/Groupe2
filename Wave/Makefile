
spectre: wave.c fft
	gcc -Wall -g `pkg-config --cflags MLV` `pkg-config --libs-only-other --libs-only-L MLV` wave.c fft.o -o wave `pkg-config --libs-only-l MLV` -lm

fft: fft.c fft.h
	gcc -c -Wall fft.c -o fft.o


clean: 
	rm -f *~ *.o wave
