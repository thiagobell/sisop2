#include "../matrix.h"
#include <stdio.h>
#include<stdlib.h>

MATRIX mult(MATRIX op1, MATRIX op2)
{
  MATRIX result;
  result.nline = op1.nline;
  result.ncol = op2.ncol;
  result.data = (int**) calloc(result.nline, sizeof(int*));
  for(int i= 0; i< result.nline; i++) {
    result.data[i] = (int*) calloc(result.ncol, sizeof(int));
  }
  for(int i= 0; i< result.nline; i++) {
    for(int j = 0; j < result.ncol; j++) {
      int sum = 0;
      for(int k = 0; k < result.ncol; k++) {
        sum += op1.data[i][k] * op2.data[k][j];
      }
      result.data[i][j] = sum;
    }
  }
return result;
}

int main(int argc, char **argv)
{
  if(argc < 4){
    printf("insufficient parameters\n serial input1 input2 output");
  }

  MATRIX op1 = parseMatrix(argv[1]);
  MATRIX op2 = parseMatrix(argv[2]);
  MATRIX result = mult(op1,op2);
  writeMatrix(argv[3], result);
}
