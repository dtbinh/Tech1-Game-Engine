/*
Copyright (C) 2011 Tech1 Game Engine.
*/
 // todo функции матана отправить в define
#include "Vector.h"
#include "Math.h"

float dot_product(float * v1, float * v2)
{
	return (v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
}
void normalize(float * v)
{
	float f = 1.0f / sqrt(dot_product(v, v));

	v[0] *= f;
	v[1] *= f;
	v[2] *= f;
}


void VectorSubstract(float * v1, float * v2, float * Result) // Result = v1 - v2
{
Result[0] = v1[0] - v2[0];
Result[1] = v1[1] - v2[1];
Result[2] = v1[2] - v2[2];
}
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