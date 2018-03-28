
spectre: wave.c fft detection_notes tri_fusion_tab
	gcc -Wall -g `pkg-config --cflags MLV` `pkg-config --libs-only-other --libs-only-L MLV` wave.c fft.o detection_notes.o tri_fusion_tab.o -o wave `pkg-config --libs-only-l MLV` -lm

fft: fft.c fft.h
	gcc -c -Wall fft.c -o fft.o

detection_notes: 
	gcc -c -Wall detection_notes.c

tri_fusion_tab:
	gcc -c -Wall tri_fusion_tab.c

clean: 
	rm -f *~ *.o wave
