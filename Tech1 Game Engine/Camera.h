/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef ORB_CAMERA_
#define ORB_CAMERA_
#include <d3dx9.h>

struct AxisAlignedBoundingBox
{
D3DXVECTOR3 bMin;
D3DXVECTOR3 bMax;
};

struct Sphere
{
    D3DXVECTOR3 centerVec;
    float       radius;
};


#define CameraNear 1.0f
#define CameraFar 1000.0f
class gCamera
{
public :

	 int nVertexLUT[6];

	float FOV;
	float AspectRatio;

	float RotationValueX;
	float RotationValueY;

	D3DXMATRIXA16 WorldMatrix;
	D3DXMATRIXA16 ViewMatrix;
	D3DXMATRIXA16 ProjectionMatrix;

	D3DXPLANE  FrustumPlanes[6];

	D3DXVECTOR3 CameraPos;
	D3DXVECTOR3 LookAtPos;
	D3DXVECTOR3 VectorUp;
	D3DXVECTOR3 VectorRight;
	
	D3DXVECTOR3 RespawnLookAtPos;

	D3DXMATRIXA16 RotationXmatrix;
	D3DXMATRIXA16 RotationYmatrix;
	D3DXMATRIXA16 CombinedRotation;
	D3DXMATRIXA16 WorldViewProjectionCombined;
	gCamera();
	~gCamera();
	void SetIdentityWorldMatrix();
	void CalculateViewMatrix();
	void SetUpVector();
	void SetRespawnPosition(D3DXVECTOR3 * vCameraPos, D3DXVECTOR3 * vLookAtPos);
	void MoveForward();
	void MoveBackward();
	void StrifeLeft();
	void StrifeRight();
	void RotateCam(int X, int Y);
	void CalculateProjectionMatrix();
	D3DXMATRIXA16 * GetWorldViewProjectionCombined();
	void SetWorldMatrix(D3DXMATRIXA16 * mWorldMatrix);
	D3DXMATRIXA16 * GetViewProjectionCombined();
	D3DXMATRIXA16 * GetView();
	void ExtractFrustumPlanes();
	bool BoxInFrustum (AxisAlignedBoundingBox * AABB);
	int BoxInFrustum2(AxisAlignedBoundingBox * AABB);
	bool TestSphere(Sphere * sphere);
	bool TestSweptSphere(Sphere * sphere,  D3DXVECTOR3 *sweepDir);
	bool SweptSpherePlaneIntersect(float& t0, float& t1,  D3DXPLANE* plane, Sphere * sphere, D3DXVECTOR3* sweepDir);
	D3DXVECTOR3 AABBPoint(unsigned int i, AxisAlignedBoundingBox * AABB);
	void XFormBoundingBox( AxisAlignedBoundingBox* result, AxisAlignedBoundingBox* src, D3DXMATRIX* matrix );
	void Merge(AxisAlignedBoundingBox* AABB, D3DXVECTOR3* vec);
	bool IntersectAABB2D(AxisAlignedBoundingBox& c, AxisAlignedBoundingBox& a, AxisAlignedBoundingBox& b);
};

#endif