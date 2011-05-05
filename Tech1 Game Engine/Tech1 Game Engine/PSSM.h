/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _PSSM_
#define _PSSM_
#include "Scene.h"

#define ShadowMapSize 1024
#define SplitScheme 0
#define MAX_SPLITS 8
#define NUM_SPLITS 4
#define LIGHT_FOV 90.0f
#define LIGHT_NEAR 1.0f
#define LIGHT_FAR 1000.0f
#define LIGHT_FAR_MAX 1000.0f
#define SPLIT_SCALE 1.1f

class cPSSM
{
private:
	
public:
D3DXMATRIX LightView;
D3DXMATRIX LightProj;
 D3DXMATRIX m_LightViewProj;

float SplitSchemeBias[2];
float SplitDistances[ MAX_SPLITS ];
D3DXVECTOR3 SceAABB[8];


D3DXVECTOR3 sweepDir; // ---
	unsigned int * ShadowCastersObjects;
	unsigned int * ShadowReceiversObjects;
	unsigned int NumShadowCastersObjects;
	unsigned int NumShadowReceiversObjects;

	void Render();
	void ComputeVirtualCameraParameters(Bbox * BoxToTest, unsigned int DlightNum, unsigned int BoxIndex);
	void ComputeVirtualCameraParametersSubBoxen(Bbox * BoxToTest, unsigned int BoxIndex);
bool ShadowCastersObjectsAllocMemory(unsigned int NumMemory);
bool ShadowResieversObjectsAllocMemory(unsigned int NumMemory);
void AdjustCameraPlanes();
void CalculateSplitDistances();
void cPSSM::CalculateFrustumCorners(D3DXVECTOR3 *pPoints,
                             const D3DXVECTOR3 &vSource,
                             const D3DXVECTOR3 &vTarget,
                             const D3DXVECTOR3 &vUp,
                             float fNear, float fFar,
                             float fFOV, float fAspect, float fScale);


void CalculateViewProj(D3DXMATRIX &mView, D3DXMATRIX &mProj,
                       D3DXVECTOR3 vSource, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUpVector,
                       float fFOV, float fNear, float fFar, float fAspect);

void CalculateLightForFrustum(D3DXVECTOR3 *pCorners, unsigned int LightIndex);
	cPSSM::cPSSM();
	cPSSM::~cPSSM();

	void RenderVisiblePartOfScene();
};

#endif