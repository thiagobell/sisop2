all: sem varcond

sem: src/sem.c
	gcc -pthread -std=c99  src/sem.c -o build/semaphore
varcond: src/varcond.c
	gcc src/varcond.c -o build/varcond -lpthread
clean:
	rm build/*
