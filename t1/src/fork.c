#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
typedef struct {
  int linebegin;
  int lineend;
}TASK;

void computeLine(int line, MATRIX op1, MATRIX op2, int *data)
{
    for(int j = 0; j < op2.ncol; j++) {
      int sum = 0;
      for(int k = 0; k < op2.ncol; k++) {
        sum += op1.data[line*op1.ncol + k] * op2.data[k*op2.ncol + j];
      }
      data[line*op1.ncol + j] = sum;
    }
}


MATRIX mult(MATRIX op1, MATRIX op2, int n)
{
  MATRIX result;
  result.nline = op1.nline;
  result.ncol = op2.ncol;
  result.data = (int*) calloc(result.nline*result.ncol, sizeof(int));

  //test if n is too big. Each task has to be at least one line long
  if(n > result.nline) {
    printf("there aren't enough lines for the number of processes desired");
    n = result.nline;
    printf(" resetting number of process to %d\n", n);
  }

  //matrix data for the result is stored in a shared memory area
  //creates memory segment
  int segment_id = shmget(IPC_PRIVATE, 2*sizeof(int)*result.nline*result.ncol, IPC_CREAT | 0666);
  int *data = (int*) shmat(segment_id, 0, 0);
  //stores childrens PIDs
  int *child_processes = (int*) calloc(n, 1);
  //allocates space for file descriptors of pipes
  int *write_pipe_descriptors = (int*) calloc(n, sizeof(int));
  int pid, readpipe;
  //spawn processes
  for(int i = 0; i < n; i++) {
    //create pipe
    int p[2];
    p[1]=-1;
    p[2]=-2;
    pipe(p);
    printf("creating child %d\n", i);
    pid = fork();
    if(pid == -1){
      printf("error while forking");
      exit(-1);
    }
    if(pid == 0) {
      //child
      //closing write side of pipe
      close(p[1]);
      readpipe = p[0];
      break;
    } else {
      //parent
      //close read side of pipe
      close(p[0]);
      child_processes[i] = pid;
      write_pipe_descriptors[i]= p[1];
    }
  }
  if(pid != 0) {
    //parent. send taks to children
    int lines_per_process = result.nline/n;

    int lines_left = result.nline;
    int cursor = 0;
    for(int child = 0; child < n; child++) {
      if(lines_left > 0) {
        TASK tasksend;
        tasksend.linebegin = cursor;
        if(lines_left >= lines_per_process && child<n-1){
          cursor = cursor + lines_per_process;
          tasksend.lineend = cursor -1;
          lines_left -= lines_per_process;
        } else {
          cursor = cursor + lines_left;
          tasksend.lineend = cursor -1;
          lines_left = 0;
        }
        printf("sending task to child #%d\n",child);
        write(write_pipe_descriptors[child],&tasksend, sizeof(TASK));
      }
    }
    for(int child = 0; child < n; child++) {
      waitpid(child_processes[child],NULL, 0);
      printf("child %d done\n", child);
    }

    memcpy(result.data, data, result.ncol*result.nline*sizeof(int));
    for(int li=0; li< result.nline;li++) {
      for(int co=0; co< result.ncol;co++) {
        //result.data[li*result.ncol + co] = data[li*result.ncol + co];
      }
    }
    return result;
  } else {
    //process is a child. wait for TASK
    TASK taskreceive;
    int bytes=0;
    while(bytes < sizeof(TASK)) {
      //printf("child waiting bytes received= %d\n",bytes);
      bytes = bytes + read(readpipe, &taskreceive+bytes, sizeof(TASK));
    }
    //bytes = read(readpipe, &taskreceive+bytes, sizeof(TASK));
    printf("task received on process %d. processing  lines [%d,%d]\n", getpid(), taskreceive.linebegin, taskreceive.lineend);
    for(int lineCount=taskreceive.linebegin; lineCount<=taskreceive.lineend; lineCount++) {
      computeLine(lineCount, op1, op2, data);
    }
    exit(0);
  }
  int **queue = (int**) calloc(n, sizeof(int*));
}


int main(int argc, char **argv)
{
  if(argc < 5){
    printf("insufficient parameters\n fork input1 input2 output processorNumber");
  }

  MATRIX op1 = parseMatrix(argv[1]);
  MATRIX op2 = parseMatrix(argv[2]);
  int n = atoi(argv[4]);
  MATRIX result = mult(op1,op2,n);

  writeMatrix(argv[3], result);
}
