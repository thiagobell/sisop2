

serial: src/matrix.c src/serial.c
	gcc  -std=c99  src/matrix.c src/serial.c -o build/serial
fork : src/matrix.c src/fork.c
	gcc  -std=c11 -g src/matrix.c src/fork.c -o build/fork
pthreads: src/matrix.c src/pthreads.c
	gcc -g src/matrix.c src/pthreads.c -o build/pthreads -lpthread
clean:
	rm build/*
