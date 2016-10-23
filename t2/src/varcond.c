#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>


typedef struct mesa_s {
	pthread_mutex_t mutexmesa;
	pthread_t* filosofos;
	pthread_cond_t* cond;
	char* estado;
} MESA;

int num;
MESA* mesa;

void imprimeEstado(void);
void *filosofo (void *);
void hungry (int);
void eat (int);
void think (int);

int main(int argc, char *argv[]) 
{

	if(argc < 2){
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
	int i;

	mesa = (MESA*) malloc(sizeof(MESA));
	pthread_mutex_init(&mesa->mutexmesa, NULL);

	//inicializar estados
	mesa->estado = (char*) malloc(num*sizeof(char));
	for (i = 0; i < num; i++)
		mesa->estado[i] = 'T';

	imprimeEstado();

	//criar filósofos
	mesa->cond = (pthread_cond_t*) malloc(num*sizeof(pthread_cond_t));
	for (i = 0; i < num; i++)
		pthread_cond_init(&mesa->cond[i], NULL);
	mesa->filosofos = (pthread_t*) malloc(num*sizeof(pthread_t));
	for (i = 0; i < num; i++)
	{
		pthread_create(&mesa->filosofos[i], NULL, filosofo, (void*) &i);
		usleep(100);
	}
	

	for (i = 0; i < num; i++)
		pthread_join(mesa->filosofos[i], NULL);
	
	return 0;
}


void imprimeEstado()
{
	int i;
	for (i = 0; i < num; i++)
		printf("%c ", mesa->estado[i]);
	printf("\n");

}

void *filosofo (void *arg) 
{
	int fil = *(int *) arg;
	while(1)
	{
		
		hungry(fil);
		sleep(rand()%10+1);
		think(fil);
		sleep(rand()%10+1);
	}
}

void hungry (int filosofo)
{
	pthread_mutex_lock(&mesa->mutexmesa);
	mesa->estado[filosofo] = 'H';
	imprimeEstado();
	eat(filosofo);
	if (mesa->estado[filosofo] != 'E')
		pthread_cond_wait(&mesa->cond[filosofo], &mesa->mutexmesa);
	imprimeEstado();
	pthread_mutex_unlock(&mesa->mutexmesa);
}

void eat (int filosofo)
{
	if (mesa->estado[(filosofo+num-1)%num] != 'E' && 
		mesa->estado[filosofo] == 'H' &&
		mesa->estado[(filosofo+1)%num] != 'E')
	{
		mesa->estado[filosofo] = 'E';
		pthread_cond_signal(&mesa->cond[filosofo]);
	}
}


void think(int filosofo)
{
	pthread_mutex_lock(&mesa->mutexmesa);
	mesa->estado[filosofo] = 'T';
	imprimeEstado();
	eat((filosofo+num-1)%num);
	eat((filosofo+1)%num);
	pthread_mutex_unlock(&mesa->mutexmesa);
}

