#include <stdio.h>
#include <semaphore.h>

int main() {
  int n;
  //loop variables
  int forkcounter,phcounter;
  
  //sets up all the fork's semaphores
  sem_t *forks = (sem_t*) calloc(sizeof(sem_t),n);
  for(forkcounter = 0; forkcounter<n; forkcounter++){
    sem_init(forks[forkcounter], 0, 1);  
  }

  //philoshophers states
  char *states = calloc(1, n);
  for(phcounter = 0; phcounter < n; phcounter++)
    states[phcounter] = ;
  
  

  

}
