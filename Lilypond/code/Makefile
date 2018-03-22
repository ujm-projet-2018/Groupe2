CC = gcc -Wall
FLAGS = -lm -g


createSheet : ./obj/createSheet.o
	$(CC) ./src/main.c ./obj/createSheet.o -o ./bin/createSheet $(FLAGS)

createSheet.o: ./src/createSheet.c ./inc/createSheet.h
	$(CC) ./src/createSheet.c -c

clean:
	rm -f *./obj/.o *./obj/~ *./src/~ ./bin/createSheet 
