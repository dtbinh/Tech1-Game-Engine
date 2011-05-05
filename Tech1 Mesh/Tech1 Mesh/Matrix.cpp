#include "Matrix.h"
#include "Vector.h"
void MatrixLoadIdentity(Matrix * matrix) {
  matrix->m[0]  = 1.0f;
  matrix->m[1]  = 0.0f;
  matrix->m[2]  = 0.0f;
  matrix->m[3]  = 0.0f;
  matrix->m[4]  = 0.0f;
  matrix->m[5]  = 1.0f;
  matrix->m[6]  = 0.0f;
  matrix->m[7]  = 0.0f;
  matrix->m[8]  = 0.0f;
  matrix->m[9]  = 0.0f;
  matrix->m[10] = 1.0f;
  matrix->m[11] = 0.0f;
  matrix->m[12] = 0.0f;
  matrix->m[13] = 0.0f;
  matrix->m[14] = 0.0f;
  matrix->m[15] = 1.0f;
}

void MatrixMultiply(Matrix * matrix1, Matrix m2) {
  Matrix m1, result;
  
  m1 = *matrix1;
  
  result.m[0]  = m1.m[0] * m2.m[0]  + m1.m[4] * m2.m[1]  + m1.m[8]  * m2.m[2]  + m1.m[12] * m2.m[3];
  result.m[1]  = m1.m[1] * m2.m[0]  + m1.m[5] * m2.m[1]  + m1.m[9]  * m2.m[2]  + m1.m[13] * m2.m[3];
  result.m[2]  = m1.m[2] * m2.m[0]  + m1.m[6] * m2.m[1]  + m1.m[10] * m2.m[2]  + m1.m[14] * m2.m[3];
  result.m[3]  = m1.m[3] * m2.m[0]  + m1.m[7] * m2.m[1]  + m1.m[11] * m2.m[2]  + m1.m[15] * m2.m[3];
  result.m[4]  = m1.m[0] * m2.m[4]  + m1.m[4] * m2.m[5]  + m1.m[8]  * m2.m[6]  + m1.m[12] * m2.m[7];
  result.m[5]  = m1.m[1] * m2.m[4]  + m1.m[5] * m2.m[5]  + m1.m[9]  * m2.m[6]  + m1.m[13] * m2.m[7];
  result.m[6]  = m1.m[2] * m2.m[4]  + m1.m[6] * m2.m[5]  + m1.m[10] * m2.m[6]  + m1.m[14] * m2.m[7];
  result.m[7]  = m1.m[3] * m2.m[4]  + m1.m[7] * m2.m[5]  + m1.m[11] * m2.m[6]  + m1.m[15] * m2.m[7];
  result.m[8]  = m1.m[0] * m2.m[8]  + m1.m[4] * m2.m[9]  + m1.m[8]  * m2.m[10] + m1.m[12] * m2.m[11];
  result.m[9]  = m1.m[1] * m2.m[8]  + m1.m[5] * m2.m[9]  + m1.m[9]  * m2.m[10] + m1.m[13] * m2.m[11];
  result.m[10] = m1.m[2] * m2.m[8]  + m1.m[6] * m2.m[9]  + m1.m[10] * m2.m[10] + m1.m[14] * m2.m[11];
  result.m[11] = m1.m[3] * m2.m[8]  + m1.m[7] * m2.m[9]  + m1.m[11] * m2.m[10] + m1.m[15] * m2.m[11];
  result.m[12] = m1.m[0] * m2.m[12] + m1.m[4] * m2.m[13] + m1.m[8]  * m2.m[14] + m1.m[12] * m2.m[15];
  result.m[13] = m1.m[1] * m2.m[12] + m1.m[5] * m2.m[13] + m1.m[9]  * m2.m[14] + m1.m[13] * m2.m[15];
  result.m[14] = m1.m[2] * m2.m[12] + m1.m[6] * m2.m[13] + m1.m[10] * m2.m[14] + m1.m[14] * m2.m[15];
  result.m[15] = m1.m[3] * m2.m[12] + m1.m[7] * m2.m[13] + m1.m[11] * m2.m[14] + m1.m[15] * m2.m[15];
  
  *matrix1 = result;
}

Vector MatrixMultiplyVector(Matrix matrix, Vector vector) {
  Vector multipliedVector;
  
  multipliedVector.x = ((matrix.m[0] * vector.x) + (matrix.m[4] * vector.y) + (matrix.m[8]  * vector.z) + matrix.m[12]);
  multipliedVector.y = ((matrix.m[1] * vector.x) + (matrix.m[5] * vector.y) + (matrix.m[9]  * vector.z) + matrix.m[13]);
  multipliedVector.z = ((matrix.m[2] * vector.x) + (matrix.m[6] * vector.y) + (matrix.m[10] * vector.z) + matrix.m[14]);
  return multipliedVector;
}

void MatrixInvert(Matrix * matrix) {
  float determinant;
  Matrix result;
  int index, indexTransposed;
  int sign;
  
  determinant = MatrixDeterminant(*matrix);
  for (index = 0; index < 16; index++) {
    sign = 1 - (((index % 4) + (index / 4)) % 2) * 2;
    indexTransposed = (index % 4) * 4 + index / 4;
    result.m[indexTransposed] = MatrixSubdeterminant(*matrix, index) * sign / determinant;
  }
  
  *matrix = result;
}


float MatrixDeterminant(Matrix matrix) {
  float subdeterminant0, subdeterminant1, subdeterminant2, subdeterminant3;
  
  subdeterminant0 = MatrixSubdeterminant(matrix, 0);
  subdeterminant1 = MatrixSubdeterminant(matrix, 4);
  subdeterminant2 = MatrixSubdeterminant(matrix, 8);
  subdeterminant3 = MatrixSubdeterminant(matrix, 12);
  
  return matrix.m[0]  *  subdeterminant0 +
         matrix.m[4]  * -subdeterminant1 +
         matrix.m[8]  *  subdeterminant2 +
         matrix.m[12] * -subdeterminant3;
}

static float MatrixSubdeterminant(Matrix matrix, int excludeIndex) {
  int index4x4, index3x3;
  float matrix3x3[9];
  
  index3x3 = 0;
  for (index4x4 = 0; index4x4 < 16; index4x4++) {
    if (index4x4 / 4 == excludeIndex / 4 ||
        index4x4 % 4 == excludeIndex % 4) {
      continue;
    }
    matrix3x3[index3x3++] = matrix.m[index4x4];
  }
  
  return matrix3x3[0] * (matrix3x3[4] * matrix3x3[8] - matrix3x3[5] * matrix3x3[7]) -
         matrix3x3[3] * (matrix3x3[1] * matrix3x3[8] - matrix3x3[2] * matrix3x3[7]) +
         matrix3x3[6] * (matrix3x3[1] * matrix3x3[5] - matrix3x3[2] * matrix3x3[4]);
}