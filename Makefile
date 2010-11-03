CC = gcc
FLAGS = -g -W -Wall -Wmissing-declarations -Wmissing-prototypes -Wshadow -Wbad-function-cast -Wcast-qual

all: clean libpola.so polash pola-i

libpola.o: libpola.c libpola.h
	gcc -fPIC -c libpola.c -ldl $(FLAGS)

libpola.so: libpola.o
	gcc -shared -o libpola.so libpola.o -ldl

polash: polash.c
	gcc polash.c -o polash $(FLAGS)

pola-i: pola-i.c
	gcc pola-i.c -o pola-i $(FLAGS)

clean:
	@ rm -f polash pola-i libpola.so *.o

	
