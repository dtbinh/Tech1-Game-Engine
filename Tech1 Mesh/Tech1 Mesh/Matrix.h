#include "Vector.h"
struct Matrix {
  float m[16];
};
typedef struct Matrix Matrix;

void MatrixLoadIdentity(Matrix * matrix);

void MatrixMultiply(Matrix * matrix1, Matrix m2);

Vector MatrixMultiplyVector(Matrix matrix, Vector vector);

void MatrixInvert(Matrix * matrix);
float MatrixDeterminant(Matrix matrix);

static float MatrixSubdeterminant(Matrix matrix, int excludeIndex);