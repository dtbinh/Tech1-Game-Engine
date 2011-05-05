/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _VECTORS_
#define _VECTORS_

struct Vector {
  float x;
  float y;
  float z;
};
typedef struct Vector Vector;

void VectorNormalize(Vector * vector);
float VectorDot(Vector vector1, Vector vector2);
Vector VectorCross(Vector vector1, Vector vector2);


#endif