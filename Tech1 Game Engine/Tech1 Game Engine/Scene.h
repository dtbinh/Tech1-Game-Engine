/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Console.h"
#include "Mesh.h"
#include "Material.h"
#ifndef _SCENE_
#define _SCENE_

#define DegreeToRadian(fDegrees) ((3.14159265f/180.0f)*fDegrees)

struct Bbox
{
unsigned int BBoxIndex;
unsigned int StartChildrenIndex;
unsigned int NumChildrens;
AxisAlignedBoundingBox AABB;
//AxisAlignedBoundingBox AABBWorld;
D3DXVECTOR3 AABBCoords[8];
//D3DXVECTOR3 AABBCoordsLightSpaceShadowCaster[8];
//D3DXVECTOR3 AABBCoordsLightSpaceShadowReceiver[8];
OneMaterial * Material;
};
struct MeshI
{
unsigned int MeshIndex;
};
struct NavMesh
{
unsigned int NavMeshIndex;
};

struct Child
{
unsigned int ChildIndex;
unsigned char ChildrenType;
};

struct OneDynamicDirectionalLight
{
D3DXVECTOR3 vLightSource;
D3DXVECTOR3 vLightTarget;
D3DXVECTOR3 vLightDir;
};

class Scene
{

D3DXVECTOR3 vSceneAABB[8];




	Bbox * BoundingBoxes;
	MeshI * Meshes;
	NavMesh * NavMeshes;
	Child * Childrens;
	OneDynamicDirectionalLight * DynamicLights;
	unsigned int NumDlights;
	unsigned int NumMeshes;
	unsigned int NumNavMeshes;
	unsigned int NumBBoxes;
	unsigned int NumChildren;
	
	unsigned int SceneRootIndex;






public:
	int Visibility[16];
	Scene::Scene();
	Scene::~Scene();
	Bbox * GetBoundingBoxes();
MeshI* GetMeshes();
	NavMesh * GetNavMeshes();
	Child * GetChildrens();
	OneDynamicDirectionalLight * GetDlights();
	bool ChildrenAllocMemory(unsigned int NumMemory);
	bool BBoxAllocMemory(unsigned int NumMemory);
	bool MeshAllocMemory(unsigned int NumMemory);
	bool NavMeshAllocMemory(unsigned int NumMemory);
	void RenderMeshes();
	void RenderBoxes();
	void Render();
	void SetSceneRootIndex(unsigned int RI);
	void RenderTree(Bbox * BoxToRendex);
	bool DlightsAllocMemory(unsigned int NumMemory);
	void DlightsTick();
	unsigned int GetRootIndex();
	void RenderBOX(Bbox * BoxToRendex);
	void RenderBOX2(Bbox * BoxToRendex, D3DXMATRIX * ProjMatrix, D3DXMATRIX * texturemat, bool RentderToShadowMap);
	void RunOnce();
	void SetRunOnce(OneMaterial * RunOnceMaterial) { this->RunOnceMaterial = RunOnceMaterial; }
	private:
	OneMaterial * RunOnceMaterial;
};

#endif