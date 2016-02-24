.PHONY: all clean

all: histogram pthread-arg-example

histogram: histogram.c pgma_io.c pgma_io.h
	gcc -g -O3 -fopenmp -o histogram histogram.c pgma_io.c -lm -lpthread

pthread-arg-example: pthread-arg-example.c
	gcc -g -O3 -o pthread-arg-example pthread-arg-example.c -lpthread

clean:
	rm -f histogram pthread-arg-example
