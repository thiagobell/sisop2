#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>

MATRIX mult(MATRIX op1, MATRIX op2, int n)
{
  MATRIX result;
  result.nline = op1.nline;
  result.ncol = op2.ncol;
  result.data = (int**) calloc(result.nline, sizeof(int*));
  for(int i= 0; i< result.nline; i++) {
    result.data[i] = (int*) calloc(result.ncol, sizeof(int));
  }

  int **queue = (int**) calloc(n, sizeof(int*));
  
}
