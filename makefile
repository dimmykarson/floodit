CFLAGS = -g -Wall -Wno-unused-result -O3 

all: floodit heuristicas

heuristicas: floodit

floodit: mapa.h mapa.c floodit.c

clean:
	rm -f floodit  *.o
