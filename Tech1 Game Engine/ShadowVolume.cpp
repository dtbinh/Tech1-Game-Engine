/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
float ShadowVolumeLightPos[3] = { 0.0f, 0.0f, 0.0f};

extern cMesh * Mesh;


extern g3dDevice * w3dDevice;
extern gWindow * wWindow;
extern Shaders * GameShaders;
extern gLog * wLog;
extern gCamera * wCamera;
//extern RendererSceleton * RScel;
void ProcessAllPollysToFindFrontCulledPollys(unsigned int MeshIndex)
{
	OneMesh * MeshList = Mesh->GetMeshes();

float DirToLight[3];
float Vertex[3];
int PreviusFrontCulled;
bool FirstLunch=false;
//for(int i = 0; i!= Mesh->Mesh.Mesh.NumMeshes;i++)
//{
		FirstLunch=false;
		for(int j = 0; j!= MeshList[MeshIndex].NumPollys;j++)
		{
			Vertex[0] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[j].PollyVertices[0].VertexIndex].X;
			Vertex[1] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[j].PollyVertices[0].VertexIndex].Y;
			Vertex[2] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[j].PollyVertices[0].VertexIndex].Z;
			VectorSubstract(&ShadowVolumeLightPos[0],&Vertex[0],&DirToLight[0]);
			normalize(&DirToLight[0]);
			MeshList[MeshIndex].DirToLight[j].X = DirToLight[0]*SILUETTE_LENGH;
			MeshList[MeshIndex].DirToLight[j].Y = DirToLight[1]*SILUETTE_LENGH;
			MeshList[MeshIndex].DirToLight[j].Z = DirToLight[2]*SILUETTE_LENGH;
			if(dot_product(&MeshList[MeshIndex].ComputedNormals[j].NormalX,&DirToLight[0]) >= 0.0f)
			{
				
				if(!FirstLunch)
				{
				MeshList[MeshIndex].FirstFrontCulled= j;
				MeshList[MeshIndex].Polygons[j].FrontCulled = true;
				PreviusFrontCulled = j;
				FirstLunch = true;
			}
				else
			{
				MeshList[MeshIndex].Polygons[PreviusFrontCulled].NextFrontCulled = j;
				MeshList[MeshIndex].Polygons[j].FrontCulled = true;
				PreviusFrontCulled = j;
			}
			}else
			{
				MeshList[MeshIndex].Polygons[j].FrontCulled = false;
			}
		}
		
		//TempMesh->Polygons[PreviusFrontCulled].NextFrontCulled = 65535;
		//TempMesh = TempMesh->NextMesh;
	//}
};


void GoThrowAllFrontCulledPollys(unsigned int MeshIndex, int FirstFrontCulled)
{
	OneMesh * MeshList = Mesh->GetMeshes();
	float DirToLight[3];
	float Vertex[3];
	SHADOW_CUSTOMVERTEX* pVertices;
	MeshList[MeshIndex].VertexBufferForShadowVolume->Lock( 0, MeshList[MeshIndex].Num3AnglePollys * 6 * sizeof( SHADOW_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	int PollyNum=FirstFrontCulled;
	int CurrPollyAdress=MeshList[MeshIndex].NumTrianglesInVertexBufferForVolume*6;
	while(PollyNum!=65535)
	{
D3DXVECTOR3 vecend;
D3DXVECTOR3 vecstart =		D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//

		//TempMesh->Polygons[PollyNum].Polly[0].VertexIndex;
		//int fox = TempMesh->AdjencyBuffer[PollyNum].Edge1;
		if(!MeshList[MeshIndex].Polygons[MeshList[MeshIndex].AdjencyBuffer[PollyNum].Edge1].FrontCulled) // если сосед back culled
		{// добавляем грань в волюм
			//TempMesh->MeshVerticesForRendering;
			//TempMesh->MeshVerticesForRendering[TempMesh->Polygons[TempMesh->AdjencyBuffer[PollyNum].Edge1].Polly[0].VertexIndex].X;
			
			pVertices[CurrPollyAdress+0].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].X;
			pVertices[CurrPollyAdress+0].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Y;
			pVertices[CurrPollyAdress+0].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Z;
			
			pVertices[CurrPollyAdress+1].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X;
			pVertices[CurrPollyAdress+1].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y;
			pVertices[CurrPollyAdress+1].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z;
			
			pVertices[CurrPollyAdress+2].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].X-MeshList[MeshIndex].DirToLight[PollyNum].X;
			pVertices[CurrPollyAdress+2].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Y-MeshList[MeshIndex].DirToLight[PollyNum].Y;
			pVertices[CurrPollyAdress+2].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Z-MeshList[MeshIndex].DirToLight[PollyNum].Z;
			//vecend =		D3DXVECTOR3( TempMesh->DirToLight[PollyNum].X, TempMesh->DirToLight[PollyNum].Y, TempMesh->DirToLight[PollyNum].Z );
			pVertices[CurrPollyAdress+3].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X;
			pVertices[CurrPollyAdress+3].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y;
		pVertices[CurrPollyAdress+3].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z;
		
		pVertices[CurrPollyAdress+5].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].X-MeshList[MeshIndex].DirToLight[PollyNum].X;
		pVertices[CurrPollyAdress+5].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Y-MeshList[MeshIndex].DirToLight[PollyNum].Y;
		pVertices[CurrPollyAdress+5].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Z-MeshList[MeshIndex].DirToLight[PollyNum].Z;
			


		Vertex[0] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X;
		Vertex[1] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y;
		Vertex[2] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z;
		VectorSubstract(&ShadowVolumeLightPos[0],&Vertex[0],&DirToLight[0]);
		normalize(&DirToLight[0]);

		pVertices[CurrPollyAdress+4].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X-(DirToLight[0]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+4].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y-(DirToLight[1]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+4].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z-(DirToLight[2]*SILUETTE_LENGH);
	//	RScel->render(vecstart, vecend  );
		MeshList[MeshIndex].NumTrianglesInVertexBufferForVolume+=2;
		CurrPollyAdress+=6; 
	}
	
	if(!MeshList[MeshIndex].Polygons[MeshList[MeshIndex].AdjencyBuffer[PollyNum].Edge2].FrontCulled)// если сосед back culled
	{// добавляем грань в волюм
		pVertices[CurrPollyAdress+1].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X;
		pVertices[CurrPollyAdress+1].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y;
		pVertices[CurrPollyAdress+1].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z;
		
		pVertices[CurrPollyAdress+0].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X;
		pVertices[CurrPollyAdress+0].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y;
		pVertices[CurrPollyAdress+0].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z;
		Vertex[0] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X;
		Vertex[1] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y;
		Vertex[2] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z;
		VectorSubstract(&ShadowVolumeLightPos[0],&Vertex[0],&DirToLight[0]);
		normalize(&DirToLight[0]);
		pVertices[CurrPollyAdress+2].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X-(DirToLight[0]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+2].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y-(DirToLight[1]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+2].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z-(DirToLight[2]*SILUETTE_LENGH);
		//vecend =		D3DXVECTOR3( TempMesh->DirToLight[PollyNum].X, TempMesh->DirToLight[PollyNum].Y, TempMesh->DirToLight[PollyNum].Z );
		pVertices[CurrPollyAdress+3].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X;
		pVertices[CurrPollyAdress+3].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y;
		pVertices[CurrPollyAdress+3].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z;
		
		pVertices[CurrPollyAdress+4].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].X-(DirToLight[0]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+4].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Y-(DirToLight[1]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+4].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[1].VertexIndex].Z-(DirToLight[2]*SILUETTE_LENGH);
			


		Vertex[0] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X;
		Vertex[1] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y;
		Vertex[2] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z;
		VectorSubstract(&ShadowVolumeLightPos[0],&Vertex[0],&DirToLight[0]);
		normalize(&DirToLight[0]);

		pVertices[CurrPollyAdress+5].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X-(DirToLight[0]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+5].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y-(DirToLight[1]*SILUETTE_LENGH);
		pVertices[CurrPollyAdress+5].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z-(DirToLight[2]*SILUETTE_LENGH);
	//	RScel->render(vecstart, vecend  );
		MeshList[MeshIndex].NumTrianglesInVertexBufferForVolume+=2;
		CurrPollyAdress+=6; 
	}

	if(!MeshList[MeshIndex].Polygons[MeshList[MeshIndex].AdjencyBuffer[PollyNum].Edge3].FrontCulled)// если сосед back culled
	{// добавляем грань в волюм
		pVertices[CurrPollyAdress+0].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X;
		pVertices[CurrPollyAdress+0].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y;
		pVertices[CurrPollyAdress+0].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z;
			
		pVertices[CurrPollyAdress+2].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].X;
		pVertices[CurrPollyAdress+2].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Y;
		pVertices[CurrPollyAdress+2].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Z;
		Vertex[0] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X;
		Vertex[1] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y;
		Vertex[2] = MeshList[MeshIndex].MeshVerticesForRendering[ MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z;
		VectorSubstract(&ShadowVolumeLightPos[0],&Vertex[0],&DirToLight[0]);
	normalize(&DirToLight[0]);

pVertices[CurrPollyAdress+1].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X-(DirToLight[0]*SILUETTE_LENGH);
pVertices[CurrPollyAdress+1].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y-(DirToLight[1]*SILUETTE_LENGH);
pVertices[CurrPollyAdress+1].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z-(DirToLight[2]*SILUETTE_LENGH);
	
			pVertices[CurrPollyAdress+3].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].X;
			pVertices[CurrPollyAdress+3].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Y;
			pVertices[CurrPollyAdress+3].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Z;
			
			pVertices[CurrPollyAdress+4].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].X-(DirToLight[0]*SILUETTE_LENGH);
			pVertices[CurrPollyAdress+4].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Y-(DirToLight[1]*SILUETTE_LENGH);
			pVertices[CurrPollyAdress+4].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[2].VertexIndex].Z-(DirToLight[2]*SILUETTE_LENGH);
			
			pVertices[CurrPollyAdress+5].x = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].X-MeshList[MeshIndex].DirToLight[PollyNum].X;
			pVertices[CurrPollyAdress+5].y = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Y-MeshList[MeshIndex].DirToLight[PollyNum].Y;
			pVertices[CurrPollyAdress+5].z = MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[PollyNum].PollyVertices[0].VertexIndex].Z-MeshList[MeshIndex].DirToLight[PollyNum].Z;
			
			MeshList[MeshIndex].NumTrianglesInVertexBufferForVolume +=2;
			CurrPollyAdress+=6; 
		}
		
		
		PollyNum = MeshList[MeshIndex].Polygons[PollyNum].NextFrontCulled;
	}
	MeshList[MeshIndex].VertexBufferForShadowVolume->Unlock();
}


void RenderSiluete(unsigned int MeshIndex)
{
//SHADOW_CUSTOMVERTEX
//D3DFVF_SHADOW_CUSTOMVERTEX
OneMesh * MeshList = Mesh->GetMeshes();

wCamera->SetIdentityWorldMatrix();
GameShaders->SetMatrixForShader(VertexShadowVolume, "mat_mvp", wCamera->GetWorldViewProjectionCombined());
GameShaders->SetShader(PixelShadowVolume);
GameShaders->SetShader(VertexShadowVolume);
		w3dDevice->g_pd3dDevice->SetStreamSource( 0, MeshList[MeshIndex].VertexBufferForShadowVolume, 0, sizeof( SHADOW_CUSTOMVERTEX ) );
		
       w3dDevice->g_pd3dDevice->SetFVF( D3DFVF_SHADOW_CUSTOMVERTEX );

       w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST,0, MeshList[MeshIndex].NumTrianglesInVertexBufferForVolume);



}