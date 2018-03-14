

spectre: wave.c
	gcc -Wall -g `pkg-config --cflags MLV` `pkg-config --libs-only-other --libs-only-L MLV` wave.c -o wave `pkg-config --libs-only-l MLV` -lm

clean: 
	rm -f *~ *.o wave
