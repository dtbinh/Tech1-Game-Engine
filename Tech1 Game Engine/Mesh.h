/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#include "Console.h"
extern Console * CON;
#define NUM_MESHES_IN_SCENE 25 //todo эту переменную грузить из scene файла
#ifndef _GAME_MESH_
#define _GAME_MESH_
#define ANIMATION_FPS 24
#define MESH_MAX_VERTICES  1000000

struct Vert
{
float X;
float Y;
float Z;
};


struct AdjencyInfo
{
int Edge1;
int Edge2;
int Edge3;
int Edge4;
};


struct PollyEdgeNormals
{
D3DXVECTOR3  Edge1Normal;
D3DXVECTOR3  Edge2Normal;
D3DXVECTOR3  Edge3Normal;
D3DXVECTOR3  Edge4Normal;
};

struct MESH_CUSTOMVERTEX
{
    FLOAT x, y, z;
   // DWORD color;
	FLOAT tu, tv;
};
#define D3DFVF_MESH_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

enum Shader
{
SHADER_LAMBERT,
SHADER_BLINN,
SHADER_PHONG,
SHADER_PHONGE,
};

struct Color
{
float  ColorR, ColorG, ColorB;
};
struct ShaderData
{
Shader ShaderType;
char  ShaderName[255];
Color ShaderColor;
char Texture[255];
LPDIRECT3DTEXTURE9   TextureHolder;
};

struct MeshVertex
{
float VertexX, VertexY, VertexZ;  // Вертексы
int StartIndex;
int Count;
};

struct NavMeshVertex
{
float VertexX, VertexY, VertexZ;  // Вертексы
int StartIndex;
int Count;
float G;
float F;
float H;
unsigned char VertexStatus; // 0 Если не в Open и не в Closed list.   1 OpenList  2 ClosedList
unsigned int ParrentIndex;
unsigned int IndexInBinaryHeap;
unsigned int ThisTimeCalculated; // Для того чтоб не перерасчитывать соседние ноды при поиске пути.
};

struct MeshNormal
{
float NormalX, NormalY, NormalZ;  // Нормали
};


struct Uv
{
float U, V; // Текстурные координаты
};

struct MeshUV
{
char  UvMapName[255];
int UvSize;
Uv * Uvs;
};

struct PollyVertex
{
unsigned int	VertexIndex;
unsigned int	NormalIndex;
unsigned int 	UVIndex;

//unsigned int *	UVIndex;
};

struct MeshPolly 
{
//unsigned char VertexCount;
	int FaceNormalIndex;
PollyVertex PollyVertices[4];
bool FrontCulled;  // Если fron culled - True   back - false
int NextFrontCulled;
};
struct NavMeshPolly 
{
//unsigned char VertexCount;
	int FaceNormalIndex;
PollyVertex PollyVertices[4];
bool FrontCulled;  // Если fron culled - True   back - false
int NextFrontCulled;
};
struct ShadersForThisMesh
{
char  ShaderName[255];
int NumPollysWithShader;
int ShaderIndex;
};
struct Bone
{
float posX;
float posY;
float posZ;
float orientX;
float orientY;
float orientZ;
float orientW;
};
struct FrameTime
{
int FrameTim; // во сколько времени заканчивается этот кадр(потом нужно включить следующий)
};
struct Weight
{
int JointIndex;
float WeightForThisBone;
float VertexPosX;
float VertexPosY;
float VertexPosZ;
};



struct BoneHierarhy
{
int ParrentIndex;
};


struct Animation
{
int NumHierarhy; // Количество костей в иерархии
int NumFrames;// todo MeshList[NumMeshesLoaded].NumFrames = 0;
BoneHierarhy * Hierarhy;
Bone * Frames;
Bone * BaseFrame;
FrameTime * FrameTimes;
DWORD StartPlayTime;
bool LoopAnim;
DWORD AnimLengh;//
};


struct OneMesh
{
Weight * WeightsBuffer;
ShaderData * Shaders;
bool NavMesh;
int WeightsBlockSize;
int WeightsBufferSize;
//int NumMeshes;
int NumShaders; // TotalNumber
LPDIRECT3DVERTEXBUFFER9 MeshVertexBuffer;
//LPDIRECT3DINDEXBUFFER9 MeshIndexBuffer;
char name[255];
D3DXMATRIXA16 MeshMatrix;
//char ShaderName[255];
int NameSize;
int Num3AnglePollys;
int Num4AnglePollys;
int NumNormals;
int NumUVs;
int NumShadersUsedForThisMesh;
//Shader ShaderType;
bool ConnectedToBone;
int FirstFrontCulled; // shadowvolume
LPDIRECT3DVERTEXBUFFER9 VertexBufferForShadowVolume;// shadowvolume
int NumTrianglesInVertexBufferForVolume;
//unsigned char * LightmapForThisMesh;
PollyEdgeNormals * PollygonEdgeNormals;
MeshUV     * MeshUVs;
Vert * MeshVerticesForRendering;
int NumVertices;
ConnectedListVertexAdjency * VertexAdjency;
AdjencyInfo * AdjencyBuffer;
MeshVertex * MeshVertices;
NavMeshVertex * NavMeshVertices;
MeshPolly  * Polygons;
NavMeshPolly * NavMeshPolygons;
MeshNormal * ComputedNormals;//?????? подумать можно ли убрать
int NumPollys;
MeshNormal * Normals;
Vert * DirToLight;
ShadersForThisMesh * MeshShadingInfo; // todo добавить удаление памяти в деструкторе
};


class cMesh
{
private:

public:
	unsigned int NumMeshesLoaded;
	unsigned int GetNumMeshesLoaded();
	OneMesh * MeshList;// !!!!!!!!!!!!!!!!!!  todo MeshList[NumMeshesLoaded].NumHierarhy = 0;
	Animation * AnimList;
 OneMesh * GetMeshes();
 Animation * GetAnimList();
 inline OneMesh ** GetMeshHolderAdress(){return &MeshList;}
bool MeshAllocMemory(unsigned int NumMemory);
void LoadMeshFromFile(char * MeshFilePath, bool NMesh);
void RenderOnlyFrontCulledPollys();
void ComputeNormalsForAnimatedMeshes(unsigned int MeshIndex);
void GenerateAdjencyForMesh(unsigned int MeshIndex);
void GenerateAdjencyForNavMesh(unsigned int MeshIndex);
void GenerateAdjency(unsigned int MeshIndex);
bool ErrorLoadingMesh;
//void PrintJointCoordsToFile();
void SetAnimInfo(unsigned int AnimationIndex, DWORD SetStartPlayTime,bool loop);
void SetAnimBones(unsigned int MeshIndex, unsigned int AnimationIndex); // Проигрывает анимацию. Сохраняет результат в BaseFrame
void Render(unsigned int MeshIndex, bool wireframe);
void ResetJoints(unsigned int AnimationIndex);
void setFrame (int no, unsigned int AnimationIndex);
void CalculateVertices(unsigned int MeshIndex, unsigned int AnimationIndex);
void BuildJoints(unsigned int AnimationIndex);
void RenderOneMesh();
void RenderAnimatedMesh(unsigned int MeshIndex, unsigned int AnimationIndex);
void LoadAnimationFromFile( char * AnimFilePath, unsigned int AnimationIndex);
bool AnimAllocMemory(unsigned int NumMemory);
void MergeAnimation( unsigned int FirstAnimationIndex, DWORD FirstAnimationStartPlayTime, unsigned int SecondAnimationIndex, DWORD SecondAnimationStartPlayTime, float MergeValue);
char * MeshFileData;
void RenderNoShadersSet(unsigned int MeshIndex, D3DXMATRIX * ProjectionMatrix, D3DXMATRIX * texturemat, bool RentderToShadowMap);
cMesh();
~cMesh();
};

#endif