process: process.c ./lib/primedecompose.c
	gcc -Wall -o ./exec/process process.c ./lib/primedecompose.c -lgmp

thread: thread.c ./lib/primedecompose.c
	gcc -Wall -o ./exec/thread thread.c ./lib/primedecompose.c -lgmp -pthread