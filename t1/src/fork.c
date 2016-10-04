#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
typedef struct {
  long mtype;
  int done;
  int linebegin;
  int lineend;
}TASK;

void computeLine(int line, MATRIX op1, MATRIX op2, int *data)
{
    printf("computing line:%d\n", line);
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
  //matrix data for the result is stored in a shared memory area
  //creates memory segment
  int segment_id = shmget(IPC_PRIVATE, 2*sizeof(int)*result.nline*result.ncol, IPC_CREAT);
  int *data = (int*) shmat(segment_id, NULL, 0);
  int *child_processes = (int*) calloc(n, sizeof(int));
  int *child_mailboxes = (int*) calloc(n, sizeof(int));

  //mailbox of parent
  int mailbox_parent = msgget(IPC_PRIVATE, IPC_CREAT);
  int outbox, inbox;
  int process_id;
  TASK ta;
  //spawn processes
  for(int i = 0; i < n; i++) {
    int mailbox_temp = msgget(IPC_PRIVATE, IPC_CREAT);
    process_id = fork();
    if(process_id == 0){
      //child
      outbox = mailbox_parent;
      inbox = mailbox_temp;
      break;
    } else {
      //parent
      child_mailboxes[i] = mailbox_temp;
      child_processes[i] = process_id;
    }
  }

  if(process_id != 0) {
    //parent. send taks to children
    int lines_per_process = result.nline/n;
    if(result.nline%n != 0)
      lines_per_process++;
    int lines_left = result.nline;
    int cursor = 0;
    for(int child = 0; child < n; child++) {
      if(lines_left > 0) {
        TASK tasksend;
        tasksend.linebegin = cursor;
        if(lines_left >= lines_per_process){
          cursor = cursor + lines_per_process;
          tasksend.lineend = cursor -1;
          lines_left -= lines_per_process;
        } else {
          cursor = cursor + lines_left;
          tasksend.lineend = cursor -1;
          lines_left = 0;
        }
        printf("sending task para processo %d [%d %d]",child , tasksend.linebegin, tasksend.lineend);
        msgsnd(child_mailboxes[child], &tasksend, sizeof(TASK)-sizeof(long),0);
      }
    }
    for(int child = 0; child < n; child++) {
      waitpid(child_processes[child],NULL, 1);
    }
    printf("children terminated\n");
    printf("lines %d, columns %d ", result.nline, result.ncol);
    //memcpy(result.data, data, result.ncol*result.nline*sizeof(int));
    for(int li=0; li< result.nline;li++) {
      for(int co=0; co< result.ncol;co++) {
        //result.data[li*result.ncol + co] = data[li*result.ncol + co];
      }
    }
    printf("copied\n");
    return result;
  } else {
    //process is a child. wait for TASK
    TASK taskreceive;
    msgrcv(inbox, &taskreceive, sizeof(TASK),1, 0);
    int lineCount;
    printf("line%d %d\n", taskreceive.linebegin, taskreceive.lineend);
    for(lineCount=taskreceive.linebegin; lineCount<=taskreceive.lineend; lineCount++) {

      computeLine(lineCount, op1, op2, data);
    }
    exit(0);
  }



  int **queue = (int**) calloc(n, sizeof(int*));

}


int main(int argc, char **argv)
{
  if(argc < 4){
    printf("insufficient parameters\n serial input1 input2 output");
  }

  MATRIX op1 = parseMatrix(argv[1]);
  MATRIX op2 = parseMatrix(argv[2]);
  MATRIX result = mult(op1,op2,2);
  writeMatrix(argv[3], result);
}
