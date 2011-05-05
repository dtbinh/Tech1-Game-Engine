#include "Vector.h"
#include "Math.h"


Vector VectorCross(Vector vector1, Vector vector2) {
  Vector result;
  
  result.x = ((vector1.y * vector2.z) - (vector1.z * vector2.y));
  result.y = ((vector1.z * vector2.x) - (vector1.x * vector2.z));
  result.z = ((vector1.x * vector2.y) - (vector1.y * vector2.x));
  
  return result;
}

float VectorDot(Vector vector1, Vector vector2) {
  return ((vector1.x * vector2.x) +
          (vector1.y * vector2.y) +
          (vector1.z * vector2.z));
}

void VectorNormalize(Vector * vector) {
  float magnitude;
  
  magnitude = sqrt((vector->x * vector->x) +
                   (vector->y * vector->y) +
                   (vector->z * vector->z));
  vector->x /= magnitude;
  vector->y /= magnitude;
  vector->z /= magnitude;
}