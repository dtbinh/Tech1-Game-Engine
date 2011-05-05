/*
Copyright (C) 2011 Tech1 Game Engine.
*/

struct UvsIndex
{
unsigned int UvIndex;
};
struct uvset
{
char Name[255];
unsigned int NumUVs;
uvset * NextSet;
};

struct EVertexForPolly
{
unsigned VertexIndex;
unsigned NormalIndex;
UvsIndex * UvIndexes;
};
struct oneUV
{
float u;
float v;
};
struct EUvSet
{
char name[255];
unsigned int count;
oneUV * UvS;
};
struct EVertex
{
float X;
float Y;
float Z;
unsigned int  StartIndex;
unsigned int  Count;
};
struct ENormal
{
float X;
float Y;
float Z;
};


struct EPolly
{
unsigned int VertexCount;
unsigned int FaceNormalIndex;

EVertexForPolly * Vertices;
};
struct ShaderInfo
{
char name[255];
unsigned int ShaderIndex; // Избавится от ShaderIndex
unsigned int NumFacesToRender; // Количество фейсов для рендеринга с этим шейдером
};

struct OneMesh
{
char  Name[255];
char MeshFileName[255];
unsigned int MeshType;
bool ConnectedToBone;
float Matrix[16];
unsigned int NumInstances;
unsigned int  PolygonCount;
unsigned int  Num3AnglePollys;
unsigned int  Num4AnglePollys;
unsigned int  NumVertices;
unsigned int  NumNormals;
unsigned int  NumUVs; // Количество наборов текстурных координат
uvset * UvsetInfo;
uvset * LastSetAdded; // Последний сет добавленный в список.  для того чтоб не менялся порядок
unsigned int  NumShaders;
EVertex * Vertices;
ENormal * Normals;
EUvSet * UVsets;
EPolly * Pollys;

ShaderInfo * ShadersUsed;
OneMesh * NextMesh;
};

struct SaveMesh
{
unsigned int NumMeshes;
OneMesh * FirstMesh;
OneMesh * LastMeshAdded;
};


struct OneExportShader
{
char Name[255];
char Type[255];
float colorR;
float colorG;
float colorB;
char TextureName[255];
};
struct ExportShader  // Сделать чтоб шейдеры сохранялись в отдельный файл
{
unsigned int NumShaders;
OneExportShader * Shaders;
};
struct ExHierarhy
{
char BoneName[255];
unsigned int BoneIndex;
char ParentName[255];
unsigned int ParentIndex;
bool Parrent;
};
struct Hierarhy
{
unsigned int NumBones;
ExHierarhy * Hy;
};

struct ExBone
{
char Name[255];//bone name
float PosX;
float PosY;
float PosZ;
float QuatX;
float QuatY;
float QuatZ;
float QuatW;
};

struct OneExportFrame
{
unsigned int FrameNum;
unsigned int FrameTime;
ExBone * Bones;
};
struct FramesForExport
{
unsigned int NumFrames;
OneExportFrame * ExFrames;
};

struct OneWeight
{
unsigned int Count;
char InfluencedMeshName[255];
unsigned int idx;
char JointName[255];
unsigned int BoneIndex;
float Weight;
float PosX;
float PosY;
float PosZ;
OneWeight * NextWeight;
};

struct WeightsExporter
{
unsigned int NumWeights;
unsigned int WeightsPerVertex;
OneWeight * LastWeightAdded;
OneWeight * Weights;
};

struct MeshCount
{
unsigned int NumMeshes;
unsigned int NumNavMeshes;
unsigned int NumBoxes;
};


struct Parrents
{
char ParrentName[255];
Parrents * NextParrent;
};
struct  OneParrentInfo
{
unsigned int NumParrents;
char MeshName[255];
Parrents * ParrentData;
OneParrentInfo * NextParrentInfo;
};
