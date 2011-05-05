/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
gCamera * wCamera;

void gCamera::SetRespawnPosition(D3DXVECTOR3 * vCameraPos, D3DXVECTOR3 * vLookAtPos)
{
	CameraPos = D3DXVECTOR3(vCameraPos->x, vCameraPos->y, vCameraPos->z);
	RespawnLookAtPos = D3DXVECTOR3(vLookAtPos->x, vLookAtPos->y, vLookAtPos->z);
    D3DXVec3Normalize(&RespawnLookAtPos,&(RespawnLookAtPos-CameraPos));

}
D3DXMATRIXA16 * gCamera::GetView()
{
	return &ViewMatrix;
}


void gCamera::RotateCam(int X, int Y)
{
	RotationValueY = RotationValueY + float(Y * 0.01f  );
	RotationValueX = RotationValueX + float(X * 0.01f  );

	D3DXMatrixRotationX( &RotationXmatrix, RotationValueY );
	D3DXMatrixRotationY( &RotationYmatrix, RotationValueX );
	CombinedRotation = RotationXmatrix * RotationYmatrix;

	D3DXVec3TransformCoord( &LookAtPos ,&RespawnLookAtPos ,&CombinedRotation);
	
	D3DXMatrixLookAtLH( &ViewMatrix, &CameraPos, &(LookAtPos+CameraPos), &VectorUp );
	//g_pd3dDevice->SetTransform( D3DTS_VIEW, &ViewMatrix );
}

void gCamera::MoveForward()
{
CameraPos = CameraPos + (LookAtPos * 0.06f);
}

void gCamera::MoveBackward()
{
CameraPos = CameraPos - (LookAtPos * 0.06f);
}
	void gCamera::StrifeLeft()
	{
		D3DXVec3Cross(&VectorRight, &RespawnLookAtPos, &VectorUp);
		D3DXVec3Normalize(&VectorRight,&VectorRight);

		D3DXVec3TransformCoord( &VectorRight ,&VectorRight ,&CombinedRotation);
		CameraPos = CameraPos + (VectorRight * 0.06f);	
	}
	void gCamera::StrifeRight()
	{

		D3DXVec3Cross(&VectorRight, &RespawnLookAtPos, &VectorUp);
		D3DXVec3Normalize(&VectorRight,&VectorRight);

		D3DXVec3TransformCoord( &VectorRight ,&VectorRight ,&CombinedRotation);
		CameraPos = CameraPos - (VectorRight * 0.06f);	
	}
void gCamera::CalculateViewMatrix()
{
	
}

void gCamera::CalculateProjectionMatrix()
{
    D3DXMatrixPerspectiveFovLH( &ProjectionMatrix, FOV, AspectRatio, CameraNear, CameraFar );
    //g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &ProjectionMatrix );

}
void gCamera::SetUpVector()
{
	VectorUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
}
gCamera::gCamera()
{
	AspectRatio = 1.33f;
	FOV = D3DX_PI/4;
	SetIdentityWorldMatrix();
	SetUpVector();
	CalculateProjectionMatrix();
	RotationValueX = 0.0f;
	RotationValueY = 0.0f;
}
void gCamera::SetWorldMatrix(D3DXMATRIXA16 * mWorldMatrix)
{
	WorldMatrix = *mWorldMatrix;
}
void gCamera::SetIdentityWorldMatrix()
{
	D3DXMatrixIdentity(&WorldMatrix);
}

D3DXMATRIXA16 * gCamera::GetWorldViewProjectionCombined()
{
WorldViewProjectionCombined = WorldMatrix * ViewMatrix * ProjectionMatrix;
return &WorldViewProjectionCombined;
}

D3DXMATRIXA16 * gCamera::GetViewProjectionCombined()
{
static D3DXMATRIXA16 MatCombined;
MatCombined =  ViewMatrix * ProjectionMatrix;
return &MatCombined;
}

gCamera::~gCamera()
{

}
//  Tests if an AABB is inside/intersecting the view frustum
int gCamera::BoxInFrustum2(AxisAlignedBoundingBox * AABB) 
{
    bool intersect = false;
    for (int i=0; i<6; i++)
    { 
		if ( (((FrustumPlanes[i].a)*((nVertexLUT[i]&1)?AABB->bMin.x:AABB->bMax.x)) + ((FrustumPlanes[i].b)*((nVertexLUT[i]&2)?AABB->bMin.y:AABB->bMax.y)) + ((FrustumPlanes[i].c)*((nVertexLUT[i]&4)?AABB->bMin.z:AABB->bMax.z )) + (FrustumPlanes[i].d))< 0.f )
            return 0;
		if (((FrustumPlanes[i].a)*((nVertexLUT[i]&1)?AABB->bMax.x:AABB->bMin.x) + (FrustumPlanes[i].b)*((nVertexLUT[i]&2)?AABB->bMax.y:AABB->bMin.y) + (FrustumPlanes[i].c)*((nVertexLUT[i]&4)?AABB->bMax.z:AABB->bMin.z ) +  (FrustumPlanes[i].d)) < 0.f )
            intersect = true;
    }

    return (intersect)?2 : 1;
}
bool gCamera::BoxInFrustum (AxisAlignedBoundingBox * AABB)
{
D3DXVECTOR3 bMin = *(D3DXVECTOR3 *)&AABB[0];
D3DXVECTOR3 bMax = *(D3DXVECTOR3 *)&AABB[3];
D3DXVECTOR3 NearPoint,FarPoint;



for (int i=0;i!=6;i++)
{
	if (FrustumPlanes[i].a > 0.0f)
	{
			if (FrustumPlanes[i].b > 0.0f)
			{
				if (FrustumPlanes[i].c > 0.0f)
				{
					 NearPoint.x = bMin.x; NearPoint.y = bMin.y; NearPoint.z = bMin.z;
				} else
				{
					NearPoint.x = bMin.x; NearPoint.y = bMin.y; NearPoint.z = bMax.z; }
				} else
				{
					if (FrustumPlanes[i].c > 0.0f)
					{
						NearPoint.x = bMin.x; NearPoint.y = bMax.y; NearPoint.z = bMin.z;
					} else
					{
						NearPoint.x = bMin.x; NearPoint.y = bMax.y; NearPoint.z = bMax.z;
					}
				}
	} else
	{
		if (FrustumPlanes[i].b > 0.0f)
		{
			 if(FrustumPlanes[i].c > 0.0f)
			{
				NearPoint.x = bMax.x; NearPoint.y = bMin.y; NearPoint.z = bMin.z;
			} else
			{
				NearPoint.x = bMax.x; NearPoint.y = bMin.y; NearPoint.z = bMax.z;
			}
		}else
		{
			if (FrustumPlanes[i].c > 0.0f)
			{
				NearPoint.x = bMax.x; NearPoint.y = bMax.y; NearPoint.z = bMin.z;
			}else
			{
				NearPoint.x = bMax.x; NearPoint.y = bMax.y; NearPoint.z = bMax.z;
			}
		}
	}
// near extreme point is outside, and thus
// the AABB is Totally outside the polyhedron
if (D3DXPlaneDotCoord(&FrustumPlanes[i],&NearPoint) > 0.0f) return false;
}

return true;
}


void gCamera::ExtractFrustumPlanes()
{
D3DXMATRIXA16 ViewProj = *GetViewProjectionCombined();

// Left clipping plane
FrustumPlanes[0].a = (ViewProj._14 + ViewProj._11);
FrustumPlanes[0].b = (ViewProj._24 + ViewProj._21);
FrustumPlanes[0].c = (ViewProj._34 + ViewProj._31);
FrustumPlanes[0].d = (ViewProj._44 + ViewProj._41);

// Right clipping plane
FrustumPlanes[1].a = (ViewProj._14 - ViewProj._11);
FrustumPlanes[1].b = (ViewProj._24 - ViewProj._21);
FrustumPlanes[1].c = (ViewProj._34 - ViewProj._31);
FrustumPlanes[1].d = (ViewProj._44 - ViewProj._41);

// Top clipping plane
FrustumPlanes[2].a = (ViewProj._14 - ViewProj._12);
FrustumPlanes[2].b = (ViewProj._24 - ViewProj._22);
FrustumPlanes[2].c = (ViewProj._34 - ViewProj._32);
FrustumPlanes[2].d = (ViewProj._44 - ViewProj._42);

// Bottom clipping plane
FrustumPlanes[3].a = (ViewProj._14 + ViewProj._12);
FrustumPlanes[3].b = (ViewProj._24 + ViewProj._22);
FrustumPlanes[3].c = (ViewProj._34 + ViewProj._32);
FrustumPlanes[3].d = (ViewProj._44 + ViewProj._42);

// Near clipping plane
FrustumPlanes[4].a = (ViewProj._14 + ViewProj._13);
FrustumPlanes[4].b = (ViewProj._24 + ViewProj._23);
FrustumPlanes[4].c = (ViewProj._34 + ViewProj._33);
FrustumPlanes[4].d = (ViewProj._44 + ViewProj._43);

// Far clipping plane
FrustumPlanes[5].a = (ViewProj._14 - ViewProj._13);
FrustumPlanes[5].b = (ViewProj._24 - ViewProj._23);
FrustumPlanes[5].c = (ViewProj._34 - ViewProj._33);
FrustumPlanes[5].d = (ViewProj._44 - ViewProj._43);


// NORMALIZE THE PLANES (Not Really Needed in Our Demo)

       
for (int i=0;i!=6;i++)
{
float denom = 1.0f / D3DXVec3Length((D3DXVECTOR3 *)&FrustumPlanes[i].a);// Get magnitude of Vector
FrustumPlanes[i].a *= denom;
FrustumPlanes[i].b *= denom;
FrustumPlanes[i].c *= denom;
FrustumPlanes[i].d *= denom;
 nVertexLUT[i] = ((FrustumPlanes[i].a<0.f)?1:0) | ((FrustumPlanes[i].b<0.f)?2:0) | ((FrustumPlanes[i].c<0.f)?4:0);
}

}// End Function Extract Clip Planes



//  test if a sphere is within the view frustum
bool gCamera::TestSphere(Sphere * sphere)
{
    bool inside = true;
    float radius = sphere->radius;

    for (int i=0; (i<6) && inside; i++)
        inside &= ((D3DXPlaneDotCoord(&FrustumPlanes[i], &sphere->centerVec) + radius) >= 0.f);

    return inside;
}

	//  this function tests if the projection of a bounding sphere along the light direction intersects
//  the view frustum 

bool gCamera::SweptSpherePlaneIntersect(float& t0, float& t1,  D3DXPLANE* plane, Sphere * sphere, D3DXVECTOR3* sweepDir)
{
    float b_dot_n = D3DXPlaneDotCoord(plane, &sphere->centerVec);
    float d_dot_n = D3DXPlaneDotNormal(plane, sweepDir);

    if (d_dot_n == 0.f)
    {
        if (b_dot_n <= sphere->radius)
        {
            //  effectively infinity
            t0 = 0.f;
            t1 = 1e32f;
            return true;
        }
        else
            return false;
    }
    else
    {
        float tmp0 = ( sphere->radius - b_dot_n) / d_dot_n;
        float tmp1 = (-sphere->radius - b_dot_n) / d_dot_n;
        t0 = min(tmp0, tmp1);
        t1 = max(tmp0, tmp1);
        return true;
    }
}



bool gCamera::TestSweptSphere(Sphere * sphere,  D3DXVECTOR3 *sweepDir)
{
    //  algorithm -- get all 12 intersection points of the swept sphere with the view frustum
    //  for all points >0, displace sphere along the sweep driection.  if the displaced sphere
    //  is inside the frustum, return TRUE.  else, return FALSE
    float displacements[12];
    int cnt = 0;
    float a, b;
    bool inFrustum = false;

    for (int i=0; i<6; i++)
    {
        if (SweptSpherePlaneIntersect(a, b, &FrustumPlanes[i], sphere, sweepDir))
        {
            if (a>=0.f)
                displacements[cnt++] = a;
            if (b>=0.f)
                displacements[cnt++] = b;
        }
    }

    for (int i=0; i<cnt; i++)
    {
        Sphere displacedSphere = *sphere;
        displacedSphere.centerVec += (*sweepDir)*displacements[i];
        displacedSphere.radius *= 1.1f;
        inFrustum |= TestSphere(&displacedSphere);
    }
    return inFrustum;
}



 D3DXVECTOR3 gCamera::AABBPoint(unsigned int i, AxisAlignedBoundingBox * AABB)
 { 
	 return D3DXVECTOR3( (i&1)?AABB->bMin.x:AABB->bMax.x, (i&2)?AABB->bMin.y:AABB->bMax.y, (i&4)?AABB->bMin.z:AABB->bMax.z );  
 }


 void gCamera::XFormBoundingBox( AxisAlignedBoundingBox* result, AxisAlignedBoundingBox* src, D3DXMATRIX* matrix )
{
    D3DXVECTOR3  pts[8];
	 D3DXVECTOR3 tmp;
    for ( int i=0; i<8; i++ )
        pts[i] = AABBPoint(i, src);

    result->bMin = D3DXVECTOR3(3.3e33f, 3.3e33f, 3.3e33f);
    result->bMax = D3DXVECTOR3(-3.3e33f, -3.3e33f, -3.3e33f);

    for (int i=0; i<8; i++)
    {
       
        D3DXVec3TransformCoord(&tmp, &pts[i], matrix);
        Merge(result, &tmp);
    }
}

 void gCamera::Merge(AxisAlignedBoundingBox* AABB, D3DXVECTOR3* vec)
{
	AABB->bMin.x = min(AABB->bMin.x, vec->x);
    AABB->bMin.y = min(AABB->bMin.y, vec->y);
    AABB->bMin.z = min(AABB->bMin.z, vec->z);
    AABB->bMax.x = max(AABB->bMax.x, vec->x);
    AABB->bMax.y = max(AABB->bMax.y, vec->y);
    AABB->bMax.z = max(AABB->bMax.z, vec->z);
}


 bool gCamera::IntersectAABB2D(AxisAlignedBoundingBox& c, AxisAlignedBoundingBox& a, AxisAlignedBoundingBox& b)
{
	c.bMin.z = c.bMax.z = 0.0f;

	if (a.bMax.x < b.bMin.x || a.bMin.x > b.bMax.x)
		return false;

	if (a.bMax.y < b.bMin.y || a.bMin.y > b.bMax.y)
		return false;

	if (a.bMax.x <= b.bMax.x)
		c.bMax.x = a.bMax.x;
	else
		c.bMax.x = b.bMax.x;

	if (a.bMax.y <= b.bMax.y)
		c.bMax.y = a.bMax.y;
	else
		c.bMax.y = b.bMax.y;

	if (a.bMin.x <= b.bMin.x)
		c.bMin.x = b.bMin.x;
	else
		c.bMin.x = a.bMin.x;

	if (a.bMin.y <= b.bMin.y)
		c.bMin.y = b.bMin.y;
	else
		c.bMin.y = a.bMin.y;

	// x
	if (a.bMax.x <= b.bMax.x)
		c.bMax.x = a.bMax.x;
	else
		c.bMax.x = b.bMax.x;

	if (a.bMin.x <= b.bMin.x)
		c.bMin.x = b.bMin.x;
	else
		c.bMin.x = a.bMin.x;

	// y
	if (a.bMax.y <= b.bMax.y)
		c.bMax.y = a.bMax.y;
	else
		c.bMax.y = b.bMax.y;

	if (a.bMin.y <= b.bMin.y)
		c.bMin.y = b.bMin.y;
	else
	    c.bMin.y = a.bMin.y;
}