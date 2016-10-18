#include "matrix.h"
#include <stdio.h>
#include <math.h>
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
      for(int k = 0; k < op2.nline; k++) {
        sum += op1.data[line*op1.ncol + k] * op2.data[k*op2.ncol + j];
      }
      data[line*op2.ncol + j] = sum;
    }
}

TASK* allocateTasks(int process_num, int line_num) 
{
  int lines_per_process = line_num/process_num;

  int lines_left = line_num;
  int cursor = 0;
  TASK *tasks = (TASK*) calloc(sizeof(TASK), process_num);
  for(int child = 0; child < process_num; child++) {
      if(lines_left > 0) {
        tasks[child].linebegin = cursor;
        if(lines_left >= lines_per_process && child<process_num-1){
          cursor = cursor + lines_per_process;
          tasks[child].lineend = cursor -1;
          lines_left -= lines_per_process;
        } else {
          cursor = cursor + lines_left;
          tasks[child].lineend = cursor -1;
          lines_left = 0;
        }
      }
    }
    return(tasks);
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
  //generate tasks
  TASK *tasks = allocateTasks(n, result.nline);
  int child_id = -1;
  //spawn processes
  for(int i = 0; i < n; i++) {
    child_id = i;
    //printf("creating child %d\n", i);
    pid = fork();
    if(pid == -1){
      printf("error while forking");
      exit(-1);
    }
    if(pid == 0) {
      //child
      break;
    } else {
      //parent
      //close read side of pipe
      child_id = -1;
      child_processes[i] = pid;
    }
  }
  if(pid != 0) {
    //parent. waits children
    
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
    TASK taskreceive = tasks[child_id];

    printf("task received on process %d. processing  lines [%d,%d]\n", getpid(), taskreceive.linebegin, taskreceive.lineend);
    for(int lineCount=taskreceive.linebegin; lineCount<=taskreceive.lineend; lineCount++) {
      //printf("computing line %d\n", lineCount);
      computeLine(lineCount, op1, op2, data);
    }
    exit(0);
  }
}


int main(int argc, char **argv)
{
  if(argc < 2){
    printf("insufficient parameters\n fork processNumber");
    return(-1);
  }

  MATRIX op1 = parseMatrix("in1.txt");
  MATRIX op2 = parseMatrix("in2.txt");
  int n = atoi(argv[1]);
  MATRIX result = mult(op1,op2,n);

  writeMatrix("out.txt", result);
}
