#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"
#ifndef _GAME_MESH_
#define _GAME_MESH_

enum Shader
{
SHADER_LAMBERT,
SHADER_BLINN,
SHADER_PHONG,
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
};

struct MeshVertex
{
float VertexX, VertexY, VertexZ;  // Вертексы
int StartIndex;
int Count;
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
unsigned int *	UVIndex;
};

struct MeshPolly 
{
unsigned char VertexCount;
PollyVertex Polly[4];
};


struct Meshes
{
char name[255];
char ShaderName[255];
int NameSize;
int NumPollys;
int NumVertices;
int NumNormals;
int NumUVs;
int NumShaders;
Matrix MeshMatrix;
Vector MeshPos;
Quaternion MeshOrientation;
Shader ShaderType;
MeshVertex * MeshVertices;
MeshNormal * Normals;
MeshUV     * MeshUVs;
MeshPolly  * Polygons;
Meshes * NextMesh;
};


struct Weight
{
int JointIndex;
float WeightForThisBone;
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

struct BoneHierarhy
{
int ParrentIndex;
};

struct FrameTime
{
int FrameTim;
};
struct MeshInfo
{
Bone * Frames;
FrameTime * FrameTimes;
int NumFrames;
int NumHierarhy;
int WeightsBlockSize;
int WeightsBufferSize;
int NumMeshes;
int NumShaders;
ShaderData * Shaders;
Meshes Mesh;
Weight * WeightsBuffer;
BoneHierarhy * Hierarhy;
};




class cMesh
{
public:
bool ErrorLoadingMesh;
MeshInfo  Mesh;
char * MeshFileData;
cMesh();
~cMesh();
};

#endif