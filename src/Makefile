CC=gcc
CFLAGS=

mysh: shell.c interpreter.c shellmemory.c pcb.c readyQueue.c kernel.c cpu.c mmu.c util.c test.c
	gcc -D fr_size=$(framesize) -D var_size=$(varmemsize) -c shell.c interpreter.c shellmemory.c pcb.c readyQueue.c kernel.c cpu.c mmu.c util.c test.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o cpu.o readyQueue.o kernel.o mmu.o util.o test.o

clean: 
	rm mysh; rm *.o


