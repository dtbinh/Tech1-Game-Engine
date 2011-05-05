/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "RenderSceleton.h"
#include "common.h"
#include "Quaternion.h"
#include "Vector.h"
#include "Mesh.h"
extern g3dDevice * w3dDevice;
RendererSceleton * RScel;
extern Shaders * GameShaders;
extern gCamera * wCamera;
extern gTerrain * wTerrain;
extern cMesh * Mesh;

RendererSceleton::RendererSceleton()
{
	MeshList = Mesh->GetMeshes();
	ScelVertexBuffer = NULL;
   w3dDevice->g_pd3dDevice->CreateVertexBuffer(36 * sizeof( RENDSCELETONCUSTOMVERTEX ), // по 2 треугольника на  каждый фейс куба
                                                  D3DUSAGE_WRITEONLY, D3DFVF_RENDSCELETONCUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &ScelVertexBuffer, NULL );
}

void RendererSceleton::RenderSkeleton(unsigned int MeshIndex, unsigned int AnimationIndex)
{
	Animation * AnimList = Mesh->GetAnimList();
Vector TempVect;
Vector TempVect2;
	Quaternion TempQuat;

	TempQuat.x = AnimList[AnimationIndex].BaseFrame[3].orientX;
	TempQuat.y = AnimList[AnimationIndex].BaseFrame[3].orientY;
	TempQuat.z = AnimList[AnimationIndex].BaseFrame[3].orientZ;
	TempQuat.w = AnimList[AnimationIndex].BaseFrame[3].orientW;

	TempVect.x = AnimList[AnimationIndex].BaseFrame[4].posX;
	TempVect.y = AnimList[AnimationIndex].BaseFrame[4].posY;
	TempVect.z = AnimList[AnimationIndex].BaseFrame[4].posZ;

	TempVect2.x =AnimList[AnimationIndex].BaseFrame[5].posX;
	TempVect2.y = AnimList[AnimationIndex].BaseFrame[5].posY;
	TempVect2.z = AnimList[AnimationIndex].BaseFrame[5].posZ;

TempVect = QuaternionMultiplyVector(&TempQuat,&TempVect);
render(&D3DXVECTOR3( AnimList[AnimationIndex].BaseFrame[3].posX, AnimList[AnimationIndex].BaseFrame[3].posY, AnimList[AnimationIndex].BaseFrame[3].posZ ),&D3DXVECTOR3( TempVect.x, TempVect.y, TempVect.z ));
render(&D3DXVECTOR3( TempVect.x, TempVect.y, TempVect.z ),&D3DXVECTOR3( TempVect2.x, TempVect2.y, TempVect2.z ));
}

void RendererSceleton::RenderNormals()
{
	/*
D3DXVECTOR3 * TempVector1;
D3DXVECTOR3 * TempVector2;
D3DXVECTOR3 * TempVector3;
D3DXVECTOR3 * TempVector4;
Meshes * TempMesh = &Mesh->Mesh.Mesh;
for(int i = 0; i!= Mesh->Mesh.Mesh.NumMeshes;i++)
	{
		if(TempMesh->ConnectedToBone)
		{
			for(int j = 0; j!= TempMesh->NumPollys ;j++)
			{
				TempVector4 = (D3DXVECTOR3 *)&TempMesh->ComputedNormals[j].NormalX;
				TempVector1 = (D3DXVECTOR3 *)&TempMesh->MeshVerticesForRendering[TempMesh->Polygons[j].PollyVertices[0].VertexIndex].X;
				TempVector2 = (D3DXVECTOR3 *)&TempMesh->MeshVerticesForRendering[TempMesh->Polygons[j].PollyVertices[1].VertexIndex].X;
				TempVector3 = (D3DXVECTOR3 *)&TempMesh->MeshVerticesForRendering[TempMesh->Polygons[j].PollyVertices[2].VertexIndex].X;
				render(TempVector1,&(*TempVector1 + *TempVector4));

			}
		}
		TempMesh = TempMesh->NextMesh;
	}
	*/
}

void RendererSceleton::render(D3DXVECTOR3 * StartPos, D3DXVECTOR3 * EndPos)
{

RENDSCELETONCUSTOMVERTEX g_Vertices[] =
    {
		{ StartPos->x, StartPos->y, StartPos->z , 0.0f, 1.0f, },
		{  EndPos->x, EndPos->y,  EndPos->z, 0.0f, 1.0f, },
	
        
    };
 VOID* pVertices;
    ScelVertexBuffer->Lock( 0, sizeof( g_Vertices ), ( void** )&pVertices, 0 );
        
    memcpy( pVertices, g_Vertices, sizeof( g_Vertices ));
    ScelVertexBuffer->Unlock();
w3dDevice->g_pd3dDevice->SetStreamSource( 0, ScelVertexBuffer, 0, sizeof( RENDSCELETONCUSTOMVERTEX ) );
GameShaders->SetShader(VertexMeshRendering);
GameShaders->SetShader(PixelMeshRendering);
w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclarationMeshRendering);
wCamera->SetIdentityWorldMatrix();
GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());
w3dDevice->g_pd3dDevice->SetTexture(0,wTerrain->g_pTexture[1]);
        w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 1 );
}
RendererSceleton::~RendererSceleton()
{
if(ScelVertexBuffer)
ScelVertexBuffer->Release();
 

}
