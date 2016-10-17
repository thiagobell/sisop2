#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

int num;
char *estados;
sem_t muteximprimir;
pthread_mutex_t mutexcomer;
pthread_cond_t liberar;

void alteraEstado(int filosofo, char estado);
void *simulate (void *arg);
void hungry (int filosofo);
void eat (int filosofo);
void think (int filosofo);

int main(int argc, char *argv[]) 
{

	if(argc < 1){
    	printf("Numero insuficiente de parametros\n");
		return -1;
	}


	num = atoi(argv[1]);

	if(num < 0){
		printf("Erro! Número inválido de filosofos\n");
		return -1;
	}


	//setting up random number generation
  	time_t t;
  	srand((unsigned) time(&t));
	
	//inicializar estados
	estados = (char*) malloc(num*sizeof(char));
	int i;
	for (i = 0; i < num; i++)
		estados[i] = 'T';
	
	pthread_mutex_init(&mutexcomer, NULL);
	sem_init(&muteximprimir, 0, 1);

	alteraEstado(0, 'T');

	//criar filósofos
	pthread_t filosofos[num];
	for (i = 0; i < num; i++){
		pthread_create(&filosofos[i], NULL, simulate, (void*) &i);
		usleep(100);	
	}

	for (i = 0; i < num; i++)
		pthread_join(filosofos[i], NULL);
	
	return 0;
}


void alteraEstado(int filosofo, char estado)
{
	sem_wait(&muteximprimir);
	
	estados[filosofo] = estado;
	int i;
	for (i = 0; i < num; i++)
		printf("%c", estados[i]);
	printf("\n");

	sem_post(&muteximprimir);
}

void *simulate (void *arg) 
{
	int filosofo = *(int *) arg;
	while(1)
	{
		hungry(filosofo);
		eat(filosofo);
		think(filosofo);
	}
}

void hungry (int filosofo)
{
	sleep(rand()%10+1);
	alteraEstado(filosofo, 'H');
}

void eat (int filosofo)
{
	pthread_mutex_lock(&mutexcomer);
	while(estados[(filosofo+num-1)%num] == 'E' || estados[(filosofo+1)%num] == 'E')
		pthread_cond_wait(&liberar, &mutexcomer);
	alteraEstado(filosofo, 'E');
	pthread_cond_signal(&liberar);
	pthread_mutex_unlock(&mutexcomer);	
}

void think(int filosofo)
{
	sleep(rand()%10+1);
	alteraEstado(filosofo, 'T');
}

