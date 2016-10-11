#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
char *states;
int n;
sem_t *forks;
void print_states() {
  int count;
  char buf[100],temp[10];
  buf[0] = '\0';
  for(count = 0; count < n; count++) {
    sprintf(temp, "%c ", states[count]);
    strcat(buf,temp);
  }
  puts(buf);
}


void *simulate( void *a) {
  //a is a pointer to an int with philosophers id
  int id = *((int*) a);
  while(1) {
    //main loop
    if(states[id] == 'T') {
      sleep(rand()%10+1);
      states[id] = 'H';
    } else if (states[id] == 'H') {
      if(id == 0){
	//start by right fork
	sem_wait(&forks[1]);
	sem_wait(forks);
      } else {
	//start by left fork
	sem_wait(&forks[id]);
	if(id == n-1) {
	  //if it is the last philosopher must take first fork
	  //because it is a circular list
	  sem_wait(forks);
	} else {
	  sem_wait(&forks[id+1]);
	}
      }
      states[id] = 'E';
    } else if (states[id] == 'E') {
      sleep(rand()%10+1);
      
      if(id == n-1) {
	sem_post(forks);
      } else {
	sem_post(&forks[id+1]);
      }
      sem_post(&forks[id]);
      
      states[id] = 'T';
    }
    print_states();
  }
}

int main() {
  n= 7;
  //loop variables
  int forkcounter,phcounter,thcounter,createstatus,*temppoint;

  //setting up random number generation
  time_t t;
  srand((unsigned) time(&t));
  
  //philosophers threads
  pthread_t *threads = calloc(sizeof(pthread_t), n);
  
  //philoshophers states
  states = calloc(1, n);
  for(phcounter = 0; phcounter < n; phcounter++)
    states[phcounter] = 'T';

  //sets up all the fork's semaphores
  forks = (sem_t*) calloc(sizeof(sem_t),n);
  for(forkcounter = 0; forkcounter<n; forkcounter++){
    sem_init(forks + forkcounter, 0, 1);  
  }

  //creating pthreads
  for(thcounter = 0; thcounter < n; thcounter++) {
    temppoint = calloc(sizeof(int),1);
    *temppoint = thcounter;
    createstatus = pthread_create( threads + thcounter, NULL, simulate, (void*) temppoint);
    if(createstatus != 0) {
      printf("Error starting thread\n");
      exit(-1);
    }    
  }
  for(thcounter = 0; thcounter < n; thcounter++) {
    pthread_join(threads[thcounter], NULL);
  }
}
