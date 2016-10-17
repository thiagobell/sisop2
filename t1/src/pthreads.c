#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

typedef struct {
  int threadnumber;
  int linebegin;
  int lineend;
}TASK;

int numeroThreads;
MATRIX matrizA, matrizB, matrizR;

void printMatrix()
{
	printf("######\n");
	int i, col;
	for(i= 0; i< matrizR.nline; i++) {
    	for(col= 0; col < matrizR.ncol; col++) {
			printf("%d ", matrizR.data[i*matrizR.ncol+col]);
		}
		printf("\n");
	}
	printf("######\n");
}

void computeLine(int line)
{
    printf("computing line:%d\n", line);
	int j;
    for(j = 0; j < matrizB.ncol; j++) {
		matrizR.data[line*matrizR.ncol + j] = 0;
    	int k;
    	for(k = 0; k < matrizB.nline; k++) {
    		matrizR.data[line*matrizR.ncol + j] += matrizA.data[line*matrizA.ncol + k] * matrizB.data[k*matrizB.ncol + j];
		
    	}
		printf("computed line:%d, col:%d, value:%d\n", line, j, matrizR.data[line*matrizR.ncol + j]);
    }
}

void *func (void* arg)
{
	TASK* task = (TASK *) arg;
	printf("Starting thread:%d\n", task->threadnumber);
	
	int line = task->linebegin;
	while(line <= task->lineend)
	{
		computeLine(line);
		line++;
	}
	
	printf("Finishing thread:%d\n", task->threadnumber);
	free(task);
}

int main (int argc, char *argv[])
{
	if(argc < 2){
    	printf("Numero insuficiente de parametros, use:\n");
		printf("\t./pthreads <numeroDeThreads>\n");
		return -1;
  	}

	numeroThreads = atoi(argv[1]);
	
	matrizA = parseMatrix("test/m1.txt");
    matrizB = parseMatrix("test/m2.txt");
	
	matrizR.nline = matrizA.nline;
	matrizR.ncol = matrizB.ncol;
	matrizR.data = (int*) calloc(matrizR.nline*matrizR.ncol, sizeof(int));

	int t;
	pthread_t threads[numeroThreads];

	int lines_per_process = matrizR.nline/numeroThreads;
    if(matrizR.nline%numeroThreads != 0)
      lines_per_process++;
	int lines_left = matrizR.nline;
    int cursor = 0;

	for(t = 0; t < numeroThreads; t++){
		TASK *task = malloc(sizeof(TASK));
		if(lines_left > 0) {
			task->threadnumber = t;
			task->linebegin = cursor;
			if(lines_left >= lines_per_process){
          		cursor = cursor + lines_per_process;
          		task->lineend = cursor -1;
          		lines_left -= lines_per_process;
        	} else {
          		cursor = cursor + lines_left;
          		task->lineend = cursor -1;
          		lines_left = 0;
	        }
		}
		printf("Creating thread #%d, linhas:%d até %d\n", task->threadnumber, task->linebegin, task->lineend);
		pthread_create(&threads[t], NULL, func, (void*) task);
	}

	for(t = 0; t < numeroThreads; t++)
		pthread_join(threads[t], NULL);
	
	printMatrix();

	writeMatrix("test/out.txt", matrizR);

	return 0;

}
