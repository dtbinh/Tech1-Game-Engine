/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "PSSM.h"
#include "common.h"
extern Scene * Sce;
cPSSM * PSSM;
extern gCamera * wCamera;
extern g3dDevice * w3dDevice;
extern cRenderToTexture * RenderToTexture;
extern Shaders * GameShaders;
extern gTerrain * wTerrain;
// clamp variables of any type
template<class Type> inline Type Clamp(Type A, Type Min, Type Max) {
  if(A<Min) return Min;
  if(A>Max) return Max;
  return A;
}




bool cPSSM::ShadowCastersObjectsAllocMemory(unsigned int NumMemory)
{
	if(ShadowCastersObjects)
	{
		CON->ErrorMessage("ShadowCastersObjectsAllocMemory: Unable to alloc memory. ShadowCastersObjects != NULL");
		return false;
	}
	ShadowCastersObjects = new unsigned int[NumMemory];
	NumShadowCastersObjects = NumMemory;
	return true;
}

bool cPSSM::ShadowResieversObjectsAllocMemory(unsigned int NumMemory)
{
	if(ShadowReceiversObjects)
	{
		CON->ErrorMessage("ShadowReceiversObjectsAllocMemory: Unable to alloc memory. ShadowReceiversObjects != NULL");
		return false;
	}
	ShadowReceiversObjects = new unsigned int[NumMemory];
	NumShadowReceiversObjects = NumMemory;
	return true;
}
cPSSM::cPSSM()
{
RenderToTexture->Create(ShadowMapSize, ShadowMapSize, D3DFMT_D24S8, D3DFMT_A8R8G8B8);
ShadowCastersObjects=0;
ShadowReceiversObjects=0;
NumShadowCastersObjects=0;
NumShadowReceiversObjects=0;
SplitSchemeBias[0] = 0.64f;
SplitSchemeBias[1] = 1.5f;
}

cPSSM::~cPSSM()
{

}

void cPSSM::ComputeVirtualCameraParametersSubBoxen(Bbox * BoxToTest, unsigned int BoxIndex)
{
	Child * Childrens = Sce->GetChildrens();
	Bbox * BoundingBoxes = Sce->GetBoundingBoxes();
	unsigned int i;
	unsigned int ChIndex;
	Sphere Sphere;
	bool WasMesh = false; 

	switch(wCamera->BoxInFrustum2(&BoxToTest->AABB))
	{
		case 0:   // outside frustum -- test swept sphere for potential shadow caster
			{
				Sphere.centerVec = ((BoxToTest->AABB.bMin + BoxToTest->AABB.bMax) * 0.5f);
				Sphere.radius = D3DXVec3Length(&(BoxToTest->AABB.bMax - Sphere.centerVec));

				if (wCamera->TestSweptSphere(&Sphere, &sweepDir))
				{
					if(BoxToTest->NumChildrens)
					{
						for( i=BoxToTest->StartChildrenIndex ; i!= BoxToTest->NumChildrens + BoxToTest->StartChildrenIndex; i++)
						{
							if(Childrens[i].ChildrenType == TYPE_BBOX) // Не BBOX пропускаем
							{
								ComputeVirtualCameraParametersSubBoxen(&BoundingBoxes[Childrens[i].ChildIndex], Childrens[i].ChildIndex);
							}else
							{
								WasMesh = true;
							}
						}
						if(!WasMesh) return; // Если нет дочерних элементов  - мешей то выходим.
					}

					
					ShadowCastersObjects[NumShadowCastersObjects++] = BoxIndex;
					ShadowReceiversObjects[NumShadowReceiversObjects++] = BoxIndex;
		Sce->Visibility[BoxToTest->BBoxIndex] = 2;
				}
				break;
			}
		case 1: //  fully inside frustum.  so store large bounding box
			{Sce->Visibility[BoxToTest->BBoxIndex] = 1;
					if(BoxToTest->NumChildrens)
					{
						for( i=BoxToTest->StartChildrenIndex ; i!= BoxToTest->NumChildrens + BoxToTest->StartChildrenIndex; i++)
						{
							if(Childrens[i].ChildrenType == TYPE_BBOX) // Не BBOX пропускаем
							{
								ComputeVirtualCameraParametersSubBoxen(&BoundingBoxes[Childrens[i].ChildIndex], Childrens[i].ChildIndex);
							}else
							{
								WasMesh = true;
							}
						}
						if(!WasMesh) return; // Если нет дочерних элементов  - мешей то выходим.
					}
					// Есть дочерние элементы меши которые надо отрендерить.

				ShadowCastersObjects[NumShadowCastersObjects++] = BoxIndex;
				ShadowReceiversObjects[NumShadowReceiversObjects++] = BoxIndex;
				Sce->Visibility[BoxToTest->BBoxIndex] = 1;
				break;
			}
		case 2:
			{
					if(BoxToTest->NumChildrens)
					{
						for( i=BoxToTest->StartChildrenIndex ; i!= BoxToTest->NumChildrens + BoxToTest->StartChildrenIndex; i++)
						{
							if(Childrens[i].ChildrenType == TYPE_BBOX) // Не BBOX пропускаем
							{
								ComputeVirtualCameraParametersSubBoxen(&BoundingBoxes[Childrens[i].ChildIndex], Childrens[i].ChildIndex);
							}else
							{
								WasMesh = true;
							}
						}
						if(!WasMesh) return; // Если нет дочерних элементов  - мешей то выходим.
					}
					// Есть дочерние элементы меши которые надо отрендерить.

				ShadowCastersObjects[NumShadowCastersObjects++] = BoxIndex;
				ShadowReceiversObjects[NumShadowReceiversObjects++] = BoxIndex;
				Sce->Visibility[BoxToTest->BBoxIndex] = 3;
			}
	}

}


void cPSSM::ComputeVirtualCameraParameters(Bbox * BoxToTest, unsigned int DlightNum, unsigned int BoxIndex)
{	int i, j;
	Bbox * BoundingBoxes = Sce->GetBoundingBoxes();
	OneDynamicDirectionalLight * DynamicLights = Sce->GetDlights();
	sweepDir = DynamicLights[DlightNum].vLightDir;


	NumShadowCastersObjects = 0;
	NumShadowReceiversObjects = 0;

	ComputeVirtualCameraParametersSubBoxen(BoxToTest, BoxIndex);
		AxisAlignedBoundingBox SceneAABB;
	SceneAABB.bMin = D3DXVECTOR3(1e33f, 1e33f, 1e33f);
	SceneAABB.bMax = D3DXVECTOR3(-1e33f, -1e33f, -1e33f);
	for(i = 0; i!=NumShadowCastersObjects; i++)
	{
		for(j = 0; j!=8; j++)
		{
			wCamera->Merge( &SceneAABB, &wCamera->AABBPoint(j, &BoundingBoxes[ShadowCastersObjects[i]].AABB));
		}
	}

	for(i = 0; i!=NumShadowReceiversObjects; i++)
	{
		for(j = 0; j!=8; j++)
		{
			wCamera->Merge( &SceneAABB, &wCamera->AABBPoint(j, &BoundingBoxes[ShadowReceiversObjects[i]].AABB));
		}
	}

	for(i = 0; i!=8; i++)
	{
		SceAABB[i] = wCamera->AABBPoint(i, &SceneAABB);
	}
}
// Computes corner points of a frustum
//
//
void cPSSM::CalculateFrustumCorners(D3DXVECTOR3 *pPoints,
                             const D3DXVECTOR3 &vSource,
                             const D3DXVECTOR3 &vTarget,
                             const D3DXVECTOR3 &vUp,
                             float fNear, float fFar,
                             float fFOV, float fAspect, float fScale) 
{

    //I thought there was a problem with this function so I rewrote it.
    //Turns out there likely wasn't, but here's a different way to calculate
    //the frustum corners!

    D3DXMATRIX lookAt;
    D3DXMATRIX proj;
    
    D3DXMatrixLookAtLH(&lookAt,&vSource,&vTarget,&vUp);
    D3DXMatrixPerspectiveFovLH(&proj,fFOV,fAspect,fNear,fFar);

    D3DXVECTOR4 c[8] = {D3DXVECTOR4( 1.0f, 1.0f,-1.0f, 1.0f ),  
                        D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ),  
                        D3DXVECTOR4(-1.0f, 1.0f,-1.0f, 1.0f ),  
                        D3DXVECTOR4(-1.0f, 1.0f, 1.0f, 1.0f ),  
                        D3DXVECTOR4(-1.0f,-1.0f,-1.0f, 1.0f ),  
                        D3DXVECTOR4(-1.0f,-1.0f, 1.0f, 1.0f ),  
                        D3DXVECTOR4( 1.0f,-1.0f,-1.0f, 1.0f ),  
                        D3DXVECTOR4( 1.0f,-1.0f, 1.0f, 1.0f )}; 
    
    D3DXMATRIX projToView, viewToWorld;
    
    D3DXMatrixInverse(&projToView,NULL,&proj);
    D3DXMatrixInverse(&viewToWorld,NULL,&lookAt);

    for(int i = 0; i < 8; i++)
    {
        D3DXVECTOR4 vec,vec2;
        D3DXVec4Transform(&vec,&c[i],&projToView);
        vec /= vec.w;
        D3DXVec4Transform(&vec2,&vec,&viewToWorld);
        vec2 /= vec2.w;
        pPoints[i].x = vec2.x;
        pPoints[i].y = vec2.y;
        pPoints[i].z = vec2.z;
    }
}

// Adjusts the camera planes to contain the visible scene
// as tightly as possible. This implementation is not very
// accurate.
//
void cPSSM::AdjustCameraPlanes(void) 
	{

		

  D3DXMATRIXA16 * mViewProj = wCamera->GetViewProjectionCombined();

  // Find the most distant point of AABB
  //
  float fMaxZ=0;
  for(int i=0;i<8;i++) 
  {
    
    // transform z coordinate with mViewProj
    float fZ=SceAABB[i].x*mViewProj->_13+SceAABB[i].y*mViewProj->_23+SceAABB[i].z*mViewProj->_33+1*mViewProj->_43;

    // check if its largest
    if(fZ>fMaxZ) fMaxZ=fZ; 

  }

  // use largest Z coordinate as new far plane

  //I took this out as this makes the slice size change and I want them to stay the same
  //_fCameraFar=fMaxZ+_fCameraNear;

}



// Calculates the frustum split distances, or in other
// words, the near and far planes for each split.
//
//
void cPSSM::CalculateSplitDistances(void) 
{
float fIDM;
float fLog;
float fUniform;
int i;
int iSplit;
//  // Reallocate array in case the split count has changed
//  //
//  delete[] _pSplitDistances;
//  _pSplitDistances=new float[_iNumSplits+1];

 
  // practical
  if(SplitScheme==0) {


    // Practical split scheme:
    // same as in the paper, but modified to scale between logarithmic and uniform
    //
    // CLi = n*(f/n)^(i/numsplits)
    // CUi = n + (f-n)*(i/numsplits)
    // Ci = CLi*(bias) + CUi*(1-bias)
    //

    SplitSchemeBias[0]=Clamp(SplitSchemeBias[0],0.0f,1.0f);
    
    for(i=0;i<NUM_SPLITS;i++)
	{
      fIDM=(float)i/(float)NUM_SPLITS;
      fLog=CameraNear*powf((CameraFar/CameraNear),fIDM);
      fUniform=CameraNear+(CameraFar-CameraNear)*fIDM;
      SplitDistances[i]=fLog*SplitSchemeBias[0]+fUniform*(1-SplitSchemeBias[0]);
    }


  // exponential
  } else if(SplitScheme==1) {


    // Exponential split scheme:
    //
    // Ci = (n - f)*(i/numsplits)^(bias+1) + n;
    //

    SplitSchemeBias[1]=Clamp(SplitSchemeBias[1],0.0f,3.0f);

    for(i=0;i<NUM_SPLITS;i++) {
      fIDM=(float)i/(float)NUM_SPLITS;
      SplitDistances[i]=(CameraFar-CameraNear)*(powf(fIDM,SplitSchemeBias[1]+1))+CameraNear;
    }


  }

  // make sure border values are right
  SplitDistances[0]=CameraNear;
  SplitDistances[NUM_SPLITS]=CameraFar;
}

// Builds a standard perspective view and projection matrix
//
//
void cPSSM::CalculateViewProj(D3DXMATRIX &mView, D3DXMATRIX &mProj,
                       D3DXVECTOR3 vSource, D3DXVECTOR3 vTarget, D3DXVECTOR3 vUpVector,
                       float fFOV, float fNear, float fFar, float fAspect) 
{

  // view matrix
  D3DXMatrixLookAtLH(&mView, &vSource, &vTarget, &vUpVector);

  // projection matrix
 // D3DXMatrixPerspectiveFovLH(&mProj, DegreeToRadian(fFOV), fAspect, fNear, fFar);

}

// Calculates the view and projection matrix for a light. The projection
// matrix is "zoomed in" on the given frustum split.
//
//
void cPSSM::CalculateLightForFrustum(D3DXVECTOR3 *pCorners, unsigned int LightIndex) 
{
	OneDynamicDirectionalLight * DynamicLights = Sce->GetDlights();
  // calculate standard view and projection matrices for light
  CalculateViewProj(LightView,LightProj,
                   DynamicLights[0].vLightSource,DynamicLights[0].vLightTarget, D3DXVECTOR3(0,1,0),
                    LIGHT_FOV, LIGHT_NEAR, LIGHT_FAR_MAX,1);


  // Next we will find the min and max values of the current
  // frustum split in lights post-projection space
  // (where coordinate range is from -1.0 to 1.0)
  //
  float fMaxX=-FLT_MAX;
  float fMaxY=-FLT_MAX;
  float fMinX=FLT_MAX;
  float fMinY=FLT_MAX;
  float fMaxZ=0;

  //D3DXMATRIX mLightViewProj=_mLightView * _mLightProj;
  //Swiching to a directional light to make life easier ;)
  D3DXMATRIX mLightViewProj=LightView;

  // find the center of the frustum and the radius.
  // this isn't an optimal bounding sphere necessarily.. 
  // even more wasted shadow map!
  D3DXVECTOR3 vTransformed[8];
  D3DXVECTOR3 sphereCenter(0,0,0);
  for(int i=0; i<8; i++)
  {
      D3DXVECTOR4 temp;
      D3DXVec3Transform(&temp,&pCorners[i],&mLightViewProj);
      vTransformed[i].x = temp.x;
      vTransformed[i].y = temp.y;
      vTransformed[i].z = temp.z;
      sphereCenter += vTransformed[i];
  }
  sphereCenter /= 8.0f;

  float sphereRadius = 0;
  for(int i = 0; i<8; i++)
  {
      D3DXVECTOR3 diff = sphereCenter - vTransformed[i];
      float length = D3DXVec3Length(&diff);
      if(length > sphereRadius) sphereRadius = length;
  }


  //Find sphere center in texture coordinates
  //Align the bounding sphere on pixel boundaries so the shadows won't shift and shimmer as we move
  //512 should really be the real texture size we are using
  float texelWidth = sphereRadius / 512;
  //remove the texel fraction from the sphere center
  sphereCenter.x -= fmod(sphereCenter.x,texelWidth);
  sphereCenter.y -= fmod(sphereCenter.y,texelWidth); 

  //Use the sphere to choose coords
  fMaxX = sphereCenter.x + sphereRadius;
  fMaxY = sphereCenter.y + sphereRadius;
  fMinX = sphereCenter.x - sphereRadius;
  fMinY = sphereCenter.y - sphereRadius;

  /*

  // for each corner point
  for(int i=0;i<8;i++) {

    // transform point
    D3DXVECTOR4 vTransformed;
    D3DXVec3Transform(&vTransformed,&pCorners[i],&mLightViewProj);

    // project x and y
    vTransformed.x/=vTransformed.w;
    vTransformed.y/=vTransformed.w;

    // find min and max values
    if(vTransformed.x>fMaxX) fMaxX=vTransformed.x;
    if(vTransformed.y>fMaxY) fMaxY=vTransformed.y;
    if(vTransformed.y<fMinY) fMinY=vTransformed.y;
    if(vTransformed.x<fMinX) fMinX=vTransformed.x;

    // find largest z distance
    if(vTransformed.z>fMaxZ) fMaxZ=vTransformed.z;
  }

   set values to valid range (post-projection)
  fMaxX=Clamp(fMaxX,-1.0f,1.0f);
  fMaxY=Clamp(fMaxY,-1.0f,1.0f);
  fMinX=Clamp(fMinX,-1.0f,1.0f);
  fMinY=Clamp(fMinY,-1.0f,1.0f);

  // Adjust the far plane of the light to be at the farthest
  // point of the frustum split. Some bias may be necessary.
  //
  _fLightFar=fMaxZ+_fLightNear+1.5f;

  */



  // re-calculate lights matrices with the new far plane
  CalculateViewProj(LightView,LightProj,
                    DynamicLights[0].vLightSource,DynamicLights[0].vLightTarget, D3DXVECTOR3(0,1,0),
                    LIGHT_FOV, LIGHT_NEAR, LIGHT_FAR_MAX, 1);

  //Calculate projection around the sphere.
  D3DXMatrixOrthoOffCenterLH(&LightProj,fMinX,fMaxX,fMinY,fMaxY,LIGHT_NEAR,LIGHT_FAR_MAX);

  /*
  // Next we build a special matrix for cropping the lights view
  // to only contain points of the current frustum split
  //
  
  float fScaleX=2.0f/(fMaxX-fMinX);
  float fScaleY=2.0f/(fMaxY-fMinY);

  float fOffsetX=-0.5f*(fMaxX+fMinX)*fScaleX;
  float fOffsetY=-0.5f*(fMaxY+fMinY)*fScaleY;

  D3DXMATRIX mCropView(  fScaleX,     0.0f,  0.0f,   0.0f,
                            0.0f,  fScaleY,  0.0f,   0.0f,
                            0.0f,     0.0f,  1.0f,   0.0f,
                        fOffsetX, fOffsetY,  0.0f,   1.0f );

  // multiply the projection matrix with it
  _mLightProj*=mCropView;

  // finally modify projection matrix for linearized depth
  //_mLightProj._33/=_fLightFar;
  //_mLightProj._43/=_fLightFar;
  */
}

static float DepthBias=0.00035f;
static float SlopeScaleDepthBias=0.5f;
void cPSSM::Render()
{
	int Split;
	float fNear;
	float fFar;
	D3DXVECTOR3 pCorners[8];
	Bbox * BoundingBoxes = Sce->GetBoundingBoxes();
	unsigned int RootIndex = Sce->GetRootIndex();
	ComputeVirtualCameraParameters(&BoundingBoxes[RootIndex], 0, RootIndex); // 0 - LightIndex
	AdjustCameraPlanes();
	CalculateSplitDistances();
   w3dDevice->g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DXCOLOR(0.5f,0.5f,0.5f,0.5f), 1.0f, 0);



  // Render each split from front to back
  //
  //
  for( Split=0;Split<NUM_SPLITS;Split++) {

    // near and far planes for current frustum split
    fNear=SplitDistances[Split];
    fFar=SplitDistances[Split+1];

    // Calculate corner points of frustum split
    //
    // To avoid edge problems, scale the frustum so
    // that it's at least a few pixels larger
    //


    CalculateFrustumCorners(pCorners,wCamera->CameraPos,wCamera->LookAtPos,wCamera->VectorUp,
                            fNear,fFar,wCamera->FOV,wCamera->AspectRatio,SPLIT_SCALE);

    // Calculate view and projection matrices
    //
    CalculateLightForFrustum(pCorners, 0); // 0 - LightIndex


    // Enable rendering to shadowmap
    //
	RenderToTexture->EnableRenderToTexture();


    // Clear the shadowmap with black (nearest depth values)
    //
    w3dDevice->g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);


    // Render the depth values of current frustum split
    // to the shadowmap - this will allow early-z culling
    // to discard most pixels that are outside the frustum.
    //

GameShaders->SetShader(VertexPSSM);
GameShaders->SetShader(PixelPSSM);
w3dDevice->g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS );
//D3DRS_ZWRITEENABLE
  //  ZWriteEnable = TRUE;
    //ZEnable = TRUE;
    // dont compare with existing depth
    //ZFunc = ALWAYS;


GameShaders->SetFloatForShader(VertexPSSM, "g_fDepthBias", DepthBias);

w3dDevice->g_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, SlopeScaleDepthBias);

    // Render the scene to the shadowmap
    //
  //  RenderScene(_mLightView,_mLightProj);
m_LightViewProj = LightView * LightProj;
D3DXMATRIX TempIdentity;
D3DXMatrixIdentity(&TempIdentity);

w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW );

for(int i = 0; i != NumShadowCastersObjects; i++)
{
Sce->RenderBOX2(&BoundingBoxes[ShadowCastersObjects[i]], &m_LightViewProj,&TempIdentity, true);
}
w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_CW );
    // Go back to normal rendering
    //
RenderToTexture->DisableRenderToTexture();




    /////////////////////////////////////////////////////////////
    // At this point we have the shadowmap texture rendered.   //
    /////////////////////////////////////////////////////////////




    // Calculate a matrix to transform points to shadowmap texture coordinates
    // (this should be exactly like in your standard shadowmap implementation)
    //

    float fTexOffset=0.5f+(0.5f/(float)ShadowMapSize);

    D3DXMATRIX mTexScale(   0.5f,               0.0f,      0.0f,   0.0f,
                            0.0f,              -0.5f,      0.0f,   0.0f,
                            0.0f,               0.0f,      1.0f,   0.0f,
                            fTexOffset,    fTexOffset,     0.0f,   1.0f );

    D3DXMATRIX mShadowMap=LightView * LightProj * mTexScale;




    // Since the near and far planes are different for each
    // rendered split, we need to change the depth value range
    // to avoid rendering over previous splits
    //
    D3DVIEWPORT9 CameraViewport;
    w3dDevice->g_pd3dDevice->GetViewport(&CameraViewport);
    // as long as ranges are in order and don't overlap it should be all good...
    CameraViewport.MinZ=Split/(float)NUM_SPLITS;
    CameraViewport.MaxZ=(Split+1)/(float)NUM_SPLITS;
    w3dDevice->g_pd3dDevice->SetViewport(&CameraViewport);


    // use the current splits near and far plane
    // when calculating matrices for the camera
	//CalculateViewProj(wCamera->ViewMatrix, wCamera->ProjectionMatrix,
      //                wCamera->CameraPos,wCamera->LookAtPos,wCamera->VectorUp,
        //              wCamera->FOV, fNear, fFar, wCamera->AspectRatio);
	D3DXMatrixLookAtLH( &wCamera->ViewMatrix, &wCamera->CameraPos, &(wCamera->LookAtPos+wCamera->CameraPos), &wCamera->VectorUp );
//D3DXMatrixPerspectiveFovLH(&wCamera->ProjectionMatrix, DegreeToRadian(fFOV), fAspect, fNear, fFar)

D3DXMatrixPerspectiveFovLH( &wCamera->ProjectionMatrix, wCamera->FOV, wCamera->AspectRatio, CameraNear, CameraFar );
GameShaders->SetShader(VertexPSSMFinal);
GameShaders->SetShader(PixelPSSMFinal);
w3dDevice->g_pd3dDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0.0f);
    // bind shadowmap as a texture
    //GetApp()->GetDevice()->SetTexture(0,_ShadowMapTexture.GetDepthTexture());
w3dDevice->g_pd3dDevice->SetTexture(GameShaders->GetTextureIndexForShader(PixelPSSMFinal, 0), RenderToTexture->m_pDSTexture);

    // render the final scene
    //
    //RenderScene(_mCameraView, _mCameraProj);


w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_CCW );
for(int i = 0; i != NumShadowReceiversObjects; i++)
{
wCamera->SetIdentityWorldMatrix();
Sce->RenderBOX2(&BoundingBoxes[ShadowReceiversObjects[i]], wCamera->GetWorldViewProjectionCombined(),&mShadowMap, false);
}
w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_CW );
    // unbind texture so we can render on it again
    w3dDevice->g_pd3dDevice->SetTexture(GameShaders->GetTextureIndexForShader(PixelPSSMFinal, 0),NULL);



    // draw the shadowmap texture to HUD
    //RenderSplitOnHUD(iSplit);
  }

}

void cPSSM::RenderVisiblePartOfScene()
{

}