#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

MATRIX mult(MATRIX op1, MATRIX op2)
{
  MATRIX result;
  result = createMatrix(op1.nline, op2.ncol);
  for(int i= 0; i< result.nline; i++) {
    for(int j = 0; j < result.ncol; j++) {
      int sum = 0;
      for(int k = 0; k < op2.nline; k++) {
        sum += op1.data[i*op1.ncol + k] * op2.data[k*op2.ncol + j];
      }
      result.data[i*op2.ncol + j] = sum;
    }
  }
return result;
}

int main(int argc, char **argv)
{

  MATRIX op1 = parseMatrix("in1.txt");
  MATRIX op2 = parseMatrix("in2.txt");
  MATRIX result = mult(op1,op2);
  writeMatrix("out.txt", result);
}
