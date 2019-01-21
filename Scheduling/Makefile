all: simcpu

simcpu: simcpu.o
	gcc -o simcpu simcpu.o

simcpu.o: simcpu.c
	gcc -Wall -c simcpu.c

clean:
	rm -r -f bin *.o *.dump simcpu
