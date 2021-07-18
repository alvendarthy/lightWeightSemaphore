all: main

main: main.c  lightweightsemaphore.c lightweightsemaphore.h
	gcc main.c lightweightsemaphore.c -o main -g -I./ -lpthread
clean:
	rm -rf main
	rm -rf *.o
