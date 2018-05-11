CC = gcc -Wall -g
FLAGS = -lm

createSheet: createSheet.o
	$(CC) src/main.c obj/createSheet.o -o bin/createSheet $(FLAGS)

createSheet.o: 
	$(CC) -c src/createSheet.c -o obj/createSheet.o

clean:
	rm -f obj/* src/~ bin/createSheet bin/partition.ly
