#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "matrix.h"

void formatingError(char *comment)
{
  printf("input file formating error %s\n", comment);
  exit(-1);
}

MATRIX createMatrix( int nline, int ncol)
{
  MATRIX result;
  result.ncol = ncol;
  result.nline = nline;
  result.data = (int*) calloc(nline*ncol, sizeof(int));
  return result;
}

MATRIX parseMatrix(char *filename)
{
  FILE *f = fopen(filename,"r");
  if(f == NULL) {
    printf("file could not be opened: %s\n", filename);
    exit(-1);
  }
  char identifier[50];
  int attributesToRead = 2, nlines =-1, ncol=-1;
  while(attributesToRead > 0) {
    if(fscanf(f,"%s", identifier)<=0)
      formatingError("");
    if(strcmp(identifier, "LINHAS") == 0) {
      if(fscanf(f," = %d", &nlines)<=0)
        formatingError("invalid number of lines");
      attributesToRead--;
    } else {
      if(strcmp(identifier, "COLUNAS") == 0) {
        if(fscanf(f," = %d", &ncol)<=0)
          formatingError("invalid number of columns");
        attributesToRead--;
      }
    }
  }
  MATRIX matrix = createMatrix(nlines,ncol);
  for(int i= 0; i< nlines; i++) {
    for(int col= 0; col < ncol; col++) {
      if(fscanf(f," %d ", &matrix.data[i*matrix.ncol + col])<=0)
        formatingError("matrix syntax error");
      printf("%d ", matrix.data[i*matrix.ncol + col]);
    }
    printf("\n");
  }
  fclose(f);
  matrix.ncol = ncol;
  matrix.nline = nlines;
  return matrix;
}

int writeMatrix(char *filename, MATRIX matrix)
{
  FILE *f = fopen(filename,"w");
  if(f == NULL) {
    printf("file could not be opened: %s\n", filename);
    exit(-1);
  }
  fprintf(f,"LINHAS = %d\nCOLUNAS = %d\n",matrix.nline,matrix.ncol);
  for(int line= 0; line< matrix.nline; line++) {
    for(int col= 0; col < matrix.ncol; col++) {
      if(col< matrix.ncol-1)
        fprintf(f, "%d ", matrix.data[line*matrix.ncol + col]);
      else
        fprintf(f, "%d\n", matrix.data[line*matrix.ncol + col]);
    }
  }
  fclose(f);
}
