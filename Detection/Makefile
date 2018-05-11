
spectre: detection_notes.c fft analyse_notes tri_fusion_tab
	gcc -Wall -g  detection_notes.c fft.o analyse_notes.o tri_fusion_tab.o -o detection_notes -lm

fft: fft.c fft.h
	gcc -c -Wall fft.c -o fft.o

analyse_notes: 
	gcc -c -Wall analyse_notes.c

tri_fusion_tab:
	gcc -c -Wall tri_fusion_tab.c

clean: 
	rm -f *~ *.o detection_notes
