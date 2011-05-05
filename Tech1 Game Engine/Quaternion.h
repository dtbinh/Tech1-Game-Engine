/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Vector.h"

#ifndef _QUATERNIONS_
#define _QUATERNIONS_
struct Quaternion {
  float x;
  float y;
  float z;
  float w;
};
typedef struct Quaternion Quaternion;
void QuaternionNormalize(Quaternion * quat);
void QuaternionInvert(Quaternion * quat);
void QuaternionToAxisAngle(Quaternion quat, Vector * axis, float * angle);
Quaternion QuaternionFromAxisAngle(Vector axis, float angle);
Vector QuaternionMultiplyVector(Quaternion * quat, Vector * vector);
Quaternion QuaternionMultiply(Quaternion * quat1, Quaternion * quat2);
Quaternion QuaternionSLERP(Quaternion start, Quaternion end, float alpha);
#endif