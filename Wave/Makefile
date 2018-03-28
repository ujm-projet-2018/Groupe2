
spectre: wave.c fft tri_fusion_tab
	gcc -Wall -g `pkg-config --cflags MLV` `pkg-config --libs-only-other --libs-only-L MLV` wave.c fft.o tri_fusion_tab.o -o wave `pkg-config --libs-only-l MLV` -lm

fft: fft.c fft.h
	gcc -c -Wall fft.c -o fft.o
tri_fusion_tab:
	gcc -c -Wall tri_fusion_tab.c


clean: 
	rm -f *~ *.o wave
