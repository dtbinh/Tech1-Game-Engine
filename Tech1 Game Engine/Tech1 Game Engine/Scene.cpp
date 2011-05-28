/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#include "Scene.h"
#include "Console.h"
#include "Mesh.h"


extern Console * CON;
extern cMesh * Mesh;
extern gPrintInfo * wPrintInfo;
extern gCamera * wCamera;
extern gTime * wTime;
//extern XPSM * XPSMShadow;
extern Shaders * GameShaders;
extern g3dDevice * w3dDevice;
extern QUAD * Quad;
extern gTerrain * wTerrain;
extern cMaterial * Material;
Scene * Sce;
extern cPSSM * PSSM;
	Bbox * Scene::GetBoundingBoxes() {
		if(!BoundingBoxes)
		{		
			CON->ErrorMessage("Scene::GetBoundingBoxes(): Bounding boxes not allocated");
			return 0;
		}
		return BoundingBoxes; 
	}

		Child * Scene::GetChildrens() {
		if(!Childrens)
		{		
			CON->ErrorMessage("Scene::GetChildrens(): Childrens not allocated");
			return 0;
		}
		return Childrens; 
	}

	MeshI * Scene::GetMeshes() {
		if(!Meshes)
		{		
			CON->ErrorMessage("Scene::GetMeshes(): Meshes not allocated");
			return 0;
		}
		return Meshes; 
	}

		NavMesh * Scene::GetNavMeshes() {
		if(!NavMeshes)
		{		
			CON->ErrorMessage("Scene::GetNavMeshes(): Meshes not allocated");
			return 0;
		}
		return NavMeshes; 
	}

bool Scene::ChildrenAllocMemory(unsigned int NumMemory)
{
	if(Childrens)
	{
		CON->ErrorMessage("ChildrenAllocMemory: Unable to alloc memory. Childrens != NULL");
		return false;
	}
	Childrens = new Child[NumMemory];
	NumChildren = NumMemory;
	return true;
}

bool Scene::DlightsAllocMemory(unsigned int NumMemory)
{
	if(DynamicLights)
	{
		CON->ErrorMessage("DlightsAllocMemory: Unable to alloc memory. DynamicLights != NULL");
		return false;
	}
	DynamicLights = new OneDynamicDirectionalLight[NumMemory];
	NumDlights = NumMemory;
	for(int i=0 ; i!= NumDlights; i++)
	{
		DynamicLights[i].vLightSource = D3DXVECTOR3(0.f, 0.f, 0.f);
		DynamicLights[i].vLightTarget = D3DXVECTOR3(0.f, 0.f, 0.f);
	}
	return true;
}

bool Scene::BBoxAllocMemory(unsigned int NumMemory)
{
	if(BoundingBoxes)
	{
		CON->ErrorMessage("BBoxAllocMemory: Unable to alloc memory. BoundingBoxes != NULL");
		return false;
	}
	BoundingBoxes = new Bbox[NumMemory];
	NumBBoxes = NumMemory;

	// !!!!!!!!!!Todo: Отвязать тень от BBOX и привязать к динамическим обьектам
	//XPSMShadow->ShadowCastersObjectsAllocMemory(NumMemory);
	//XPSMShadow->ShadowResieversObjectsAllocMemory(NumMemory);
	//XPSMShadow->ShadowCastersPointsAllocMemory(NumMemory);
	//XPSMShadow->ShadowResieversPointsAllocMemory(NumMemory);
	//XPSMShadow->PPLSCastersAllocMemory(NumMemory);
	//XPSMShadow->PPLSReceiversAllocMemory(NumMemory);

	PSSM->ShadowCastersObjectsAllocMemory(NumMemory);
	PSSM->ShadowResieversObjectsAllocMemory(NumMemory);
	
	return true;
}

bool Scene::MeshAllocMemory(unsigned int NumMemory)
{
	if(Meshes)
	{
		CON->ErrorMessage("Scene::MeshAllocMemory: Unable to alloc memory. BoundingBoxes != NULL");
		return false;
	}
	Meshes = new MeshI[NumMemory];
	NumMeshes = NumMemory;
	return true;
}

bool Scene::NavMeshAllocMemory(unsigned int NumMemory)
{
	if(NavMeshes)
	{
		CON->ErrorMessage("Scene::NavMeshAllocMemory: Unable to alloc memory. BoundingBoxes != NULL");
		return false;
	}
	NavMeshes = new NavMesh[NumMemory];
	NumNavMeshes = NumMemory;
	return true;
}



Scene::Scene()
{
DynamicLights = 0;
BoundingBoxes = 0;
Meshes = 0;
NavMeshes = 0;
Childrens = 0;
NumDlights = 0;
NumMeshes = 0;

NumNavMeshes = 0;
NumBBoxes = 0;
NumChildren = 0;

}
void Scene::SetSceneRootIndex(unsigned int RI)
{
SceneRootIndex = RI;
}
Scene::~Scene()
{
	if(BoundingBoxes)
	{
		delete [] BoundingBoxes;
		BoundingBoxes = 0;
	}
	if(Meshes)
	{
		delete [] Meshes;
		Meshes = 0;
	}
	if(NavMeshes)
	{
		delete [] NavMeshes;
		NavMeshes = 0;
	}

	if(Childrens)
	{
		delete [] Childrens;
		Childrens = 0;
	}
}

void Scene::RenderBoxes()
{
	int i;
	for( i=0 ; i!= NumBBoxes; i++)
	{
		Mesh->Render(BoundingBoxes[i].BBoxIndex, true);
	}
}

void Scene::RenderMeshes()
{
	int i;
	for( i=0 ; i!= NumMeshes; i++)
	{
		Mesh->Render(Meshes[i].MeshIndex, false);
	}
}

void Scene::Render()
{
DlightsTick();
int i;
wCamera->ExtractFrustumPlanes();
//RenderMeshes();
//RenderBoxes();
memset(&Visibility[0], 0, 4*16);
//RenderTree(&BoundingBoxes[SceneRootIndex]);
PSSM->Render();

//XPSMShadow->RenderShadowMap();
//GameShaders->SetShader(VertexXPSMFinal);
//GameShaders->SetShader(PixelXPSMFinal);
//w3dDevice->g_pd3dDevice->SetTexture(GameShaders->GetTextureIndexForShader(PixelXPSMFinal, 0), XPSMShadow->m_pSMColorTexture);
//XPSMShadow->SetRenderStates(false);


/*
 float fOffsetX = 0.5f + (0.5f / (float)TEXDEPTH_WIDTH_20);
 float fOffsetY = 0.5f + (0.5f / (float)TEXDEPTH_HEIGHT_20);
    unsigned int range = 1;
    float fBias    = 0.3f;
    D3DXMATRIX texScaleBiasMat( 0.5f,     0.0f,     0.0f,         0.0f,
                                0.0f,    -0.5f,     0.0f,         0.0f,
                                0.0f,     0.0f,     (float)range, 0.0f,
                                fOffsetX, fOffsetY, 0.0f,         1.0f );

GameShaders->SetFloatForShader(PixelXPSMFinal, "ZBias", 0.03f);
D3DXMATRIX textureMatrix;
D3DXMatrixMultiply(&textureMatrix, &XPSMShadow->m_LightViewProj, &texScaleBiasMat);

w3dDevice->g_pd3dDevice->SetTexture(GameShaders->GetTextureIndexForShader(PixelXPSMFinal, 1), wTerrain->g_pTexture[1]);
	//m_pEffect->SetFloat("ZBias", m_XPSM_ZBias);
for(int i = 0; i != XPSMShadow->NumShadowReceiversObjects; i++)
{
wCamera->SetIdentityWorldMatrix();
Sce->RenderBOX2(&BoundingBoxes[XPSMShadow->ShadowReceiversObjects[i]], wCamera->GetWorldViewProjectionCombined(),&textureMatrix, false);
}

*/
///--------------------------------------------------


/*
 D3DXMATRIXA16 tempScaleMatrix;
        D3DXMATRIXA16 viewProjMatrix;
        D3DXMATRIXA16 scaledTextureMatrix;
        D3DXMatrixScaling(&tempScaleMatrix, Quad->scaleVec.x, Quad->scaleVec.y, Quad->scaleVec.z);
		D3DXMatrixMultiply(&viewProjMatrix, wCamera->GetView(), &wCamera->ProjectionMatrix);
        D3DXMatrixMultiply(&scaledTextureMatrix, &tempScaleMatrix, &textureMatrix);

	D3DXMATRIX worldViewProjMat = tempScaleMatrix * viewProjMatrix; // tempScaleMatrix == world mat
*/
    //D3DXMATRIXA16 worldITMat;    
   // D3DXMatrixInverse(&worldITMat, NULL, &tempScaleMatrix);
   // D3DXMatrixTranspose(&worldITMat, &worldITMat);
/*
GameShaders->SetShader(VertexXPSMFinal);
GameShaders->SetShader(PixelXPSMFinal);
GameShaders->SetMatrixForShader(VertexXPSMFinal, "TexTransform", &scaledTextureMatrix);
GameShaders->SetMatrixForShader(VertexXPSMFinal, "WorldViewProj", &viewProjMatrix);
*/
///------------------------------
/* 
GameShaders->SetShader(QuadShaderIndex);
	GameShaders->SetShader(VertexMeshRendering);
	w3dDevice->g_pd3dDevice->SetTexture(GameShaders->GetTextureIndexForShader(QuadShaderIndex, 0), XPSMShadow->m_pSMColorTexture); 
D3DXMATRIXA16 view; //= *wCamera->GetView();
//D3DXMatrixInverse(&view, NULL, &view);
		D3DXMATRIXA16 viewProjMatrix;
D3DXMatrixTranslation(&view, -210, 130, 0);
		D3DXMatrixOrthoRH(&viewProjMatrix, 640, 480, 1, 100);

		D3DXMatrixMultiply(&viewProjMatrix, &view, &viewProjMatrix);


GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", &viewProjMatrix);
Quad->Render();
*/
/////-----------------------------------
//for(unsigned int i = 0; i != XPSMShadow->NumShadowReceiversObjects; i++)
//{
//Sce->RenderBOX2(&BoundingBoxes[XPSMShadow->ShadowReceiversObjects[i]], &XPSMShadow->m_LightViewProj);
//}
   // m_pEffect->SetMatrix("WorldViewProj", &worldViewProjMat);
   // m_pEffect->SetMatrix("WorldIT", &worldITMat);
   // m_pEffect->SetMatrix("TexTransform", &texMat);
	//	SetVertexShaderMatrices(tempScaleMatrix, viewProjMatrix, scaledTextureMatrix);


//for(int i = 0; i!= Mesh->NumMeshesLoaded; i++)
//{
//Mesh->Render(i);
//}
//for( i=0 ; i!= NumBBoxes; i++)
//	{
		
//	}
swprintf(wPrintInfo->Console_Output,L"Box 1 %d\nBox 2 %d\nBox 3 %d\nBox 4 %d\nBox 5 %d\nBox 6 %d\nBox 7 %d\nBox 8 %d\nBox 9 %d\nBox 10 %d\nBox 11 %d\nBox 12 %d\nBox 13 %d\nBox 14 %d\nBox 15 %d\nBox 16 %d\n", Visibility[0], Visibility[1], Visibility[2], Visibility[3], Visibility[4], Visibility[5], Visibility[6], Visibility[7], Visibility[8], Visibility[9], Visibility[10], Visibility[11], Visibility[12], Visibility[13], Visibility[14], Visibility[15]);



}




void Scene::RenderTree(Bbox * BoxToRendex)
{
	unsigned int i;
	unsigned int ChIndex;
	if(wCamera->BoxInFrustum2(&BoxToRendex->AABB))
	{	// Если бокс виден
		// Рендерим его
		Mesh->Render(BoxToRendex->BBoxIndex, true);
		Visibility[BoxToRendex->BBoxIndex] = 1;
		if(BoxToRendex->NumChildrens)
		{
			for( i=BoxToRendex->StartChildrenIndex ; i!= BoxToRendex->NumChildrens + BoxToRendex->StartChildrenIndex; i++)
			{// И его потомков
				if(Childrens[i].ChildrenType == TYPE_MESH)
				{


					
					Mesh->Render(Meshes[Childrens[i].ChildIndex].MeshIndex, true);
					
				}else if(Childrens[i].ChildrenType == TYPE_BBOX)
				{
					RenderTree(&BoundingBoxes[Childrens[i].ChildIndex]);
					
				}
			}
		}

	}
}

void Scene::RenderBOX(Bbox * BoxToRendex)
{
	unsigned int i;
	unsigned int ChIndex;
	// Если бокс виден
		// Рендерим его
		if(BoxToRendex->NumChildrens)
		{
			for( i=BoxToRendex->StartChildrenIndex ; i!= BoxToRendex->NumChildrens + BoxToRendex->StartChildrenIndex; i++)
			{// И его потомков
				if(Childrens[i].ChildrenType == TYPE_MESH)
				{
					Mesh->Render(Meshes[Childrens[i].ChildIndex].MeshIndex, false);
					//Visibility[Childrens[i].ChildIndex] = 1;
				}
			}
		}

	
}
void Scene::RenderBOX2(Bbox * BoxToRendex, D3DXMATRIX * ProjMatrix, D3DXMATRIX * texturemat, bool RentderToShadowMap)
{
	unsigned int i;
	unsigned int ChIndex;
	// Если бокс виден
		// Рендерим его
				tRunScript RunScript;
Material->SetDWORDTemporary(GameShaders->GetTextureIndexForShader(PixelPSSMFinal, 1));
if(BoxToRendex->Material)
{
RunScript= (tRunScript)BoxToRendex->Material->CompiledMaterial ;
RunScript();
}
	//Visibility[BoxToRendex->BBoxIndex] = 1;

		if(BoxToRendex->NumChildrens)
		{
			for( i=BoxToRendex->StartChildrenIndex ; i!= BoxToRendex->NumChildrens + BoxToRendex->StartChildrenIndex; i++)
			{// И его потомков
				if(Childrens[i].ChildrenType == TYPE_MESH)
				{	

				Mesh->RenderNoShadersSet(Meshes[Childrens[i].ChildIndex].MeshIndex, ProjMatrix, texturemat, RentderToShadowMap);
				}
			}
		}

	
}

static float _fLightRotation=DegreeToRadian(150);
void Scene::DlightsTick()
{
	int i;
	for(i = 0; i!= NumDlights; i++)
	{
DynamicLights[i].vLightSource =D3DXVECTOR3(-200*sinf(_fLightRotation),120,200*cosf(_fLightRotation));
DynamicLights[i].vLightTarget =D3DXVECTOR3(0,0,0);
DynamicLights[i].vLightDir = DynamicLights[i].vLightTarget-DynamicLights[i].vLightSource;
D3DXVec3Normalize(&DynamicLights[i].vLightDir, &DynamicLights[i].vLightDir);
/*
	static float phi = 0.f;

    phi = ( (wTime->GetTimeFromStart() * 0.001f) )/1000.0f;
    phi = phi-floorf(phi);
    phi = 3.14159265f *phi;

	DynamicLights[i].m_lightDir.x = 0.99;//cosf(phi);
    DynamicLights[i].m_lightDir.y = 0.01;//sinf(phi);
   DynamicLights[i]. m_lightDir.z = 0.0f;
    D3DXVec3Normalize(&DynamicLights[i].m_lightDir, &DynamicLights[i].m_lightDir);

	//!!!!!!!!!! m_pEffect->SetValue("LightVec", (float*)&m_lightDir, sizeof(float)*3);
*/
	}
}

OneDynamicDirectionalLight * Scene::GetDlights()
{
return &DynamicLights[0];
}

unsigned int Scene::GetRootIndex()
{
	return SceneRootIndex;
}


void Scene::RunOnce()
{
	tRunScript RunScript;
	RunScript= (tRunScript)RunOnceMaterial->CompiledMaterial;
	RunScript();
}
