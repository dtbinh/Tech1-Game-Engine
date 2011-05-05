/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Quaternion.h"
#include "Vector.h"
#include "Math.h"

void QuaternionInvert(Quaternion * quat) {
  float length;
  
  length = (1.0f / ((quat->x * quat->x) +
                    (quat->y * quat->y) +
                    (quat->z * quat->z) +
                    (quat->w * quat->w)));
  quat->x *= -length;
  quat->y *= -length;
  quat->z *= -length;
  quat->w *= length;
}



void QuaternionNormalize(Quaternion * quat) {
  float magnitude;
  
  magnitude = sqrt((quat->x * quat->x) + (quat->y * quat->y) + (quat->z * quat->z) + (quat->w * quat->w));
  quat->x /= magnitude;
  quat->y /= magnitude;
  quat->z /= magnitude;
  quat->w /= magnitude;
}


Quaternion QuaternionFromAxisAngle(Vector axis, float angle) {
  Quaternion result;
  float sinAngle;
  
  angle *= 0.5f;
  VectorNormalize(&axis);
  sinAngle = sin(angle);
  result.x = (axis.x * sinAngle);
  result.y = (axis.y * sinAngle);
  result.z = (axis.z * sinAngle);
  result.w = cos(angle);
  return result;
}

void QuaternionToAxisAngle(Quaternion quat, Vector * axis, float * angle) {
  float sinAngle;
  
  QuaternionNormalize(&quat);
  sinAngle = sqrt(1.0f - (quat.w * quat.w));
  if (fabs(sinAngle) < 0.0005f) sinAngle = 1.0f;
  axis->x = (quat.x / sinAngle);
  axis->y = (quat.y / sinAngle);
  axis->z = (quat.z / sinAngle);
  *angle = (acos(quat.w) * 2.0f);
}


Vector QuaternionMultiplyVector(Quaternion * quat, Vector * vector) {
  Quaternion vectorQuat, inverseQuat, resultQuat;
  Vector resultVector;
  
  vectorQuat.x = vector->x;
  vectorQuat.y = vector->y;
  vectorQuat.z = vector->z;
  vectorQuat.w = 0.0f;
  
  inverseQuat = *quat;
  QuaternionInvert(&inverseQuat);
  resultQuat = QuaternionMultiply(&vectorQuat, &inverseQuat);
  resultQuat = QuaternionMultiply(quat, &resultQuat);
  
  resultVector.x = resultQuat.x;
  resultVector.y = resultQuat.y;
  resultVector.z = resultQuat.z;
  
  return resultVector;
}


Quaternion QuaternionMultiply(Quaternion * quat1, Quaternion * quat2) {
  Vector vector1, vector2, cross;
  Quaternion result;
  float angle;
  
  vector1.x = quat1->x;
  vector1.y = quat1->y;
  vector1.z = quat1->z;
  vector2.x = quat2->x;
  vector2.y = quat2->y;
  vector2.z = quat2->z;
  angle = ((quat1->w * quat2->w) - (VectorDot(vector1, vector2)));
  
  cross = VectorCross(vector1, vector2);
  vector1.x *= quat2->w;
  vector1.y *= quat2->w;
  vector1.z *= quat2->w;
  vector2.x *= quat1->w;
  vector2.y *= quat1->w;
  vector2.z *= quat1->w;
  
  result.x = (vector1.x + vector2.x + cross.x);
  result.y = (vector1.y + vector2.y + cross.y);
  result.z = (vector1.z + vector2.z + cross.z);
  result.w = angle;
  
  return result;
}



#define SLERP_TO_LERP_SWITCH_THRESHOLD 0.01f

Quaternion QuaternionSLERP(Quaternion start, Quaternion end, float alpha) {
  float startWeight, endWeight, difference;
  Quaternion result;
  
  difference = ((start.x * end.x) + (start.y * end.y) + (start.z * end.z) + (start.w * end.w));
  if ((1.0f - difference) > SLERP_TO_LERP_SWITCH_THRESHOLD) {
    float theta, oneOverSinTheta;
    
    theta = acos(fabs(difference));
    oneOverSinTheta = (1.0f / sin(theta));
    startWeight = (sin(theta * (1.0f - alpha)) * oneOverSinTheta);
    endWeight = (sin(theta * alpha) * oneOverSinTheta);
    if (difference < 0.0f) {
      startWeight = -startWeight;
    }
  } else {
    startWeight = (1.0f - alpha);
    endWeight = alpha;
  }
  result.x = ((start.x * startWeight) + (end.x * endWeight));
  result.y = ((start.y * startWeight) + (end.y * endWeight));
  result.z = ((start.z * startWeight) + (end.z * endWeight));
  result.w = ((start.w * startWeight) + (end.w * endWeight));
  QuaternionNormalize(&result);
  return result;
}
