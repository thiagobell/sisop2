#ifndef MATRIXH
#define MATRIXH
/* this file defines the matrix input and output functions
  a matrix is representeded as an array matrix matrix[line][column]
*/

typedef struct {
  int **data;
  int ncol;
  int nline;
} MATRIX;

MATRIX parseMatrix(char *filename);
int writeMatrix(char *filename, MATRIX matrix);

#endif
