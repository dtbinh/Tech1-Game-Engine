/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include<vector>
#include <memory>
#include <maya/MFnPlugin.h>
#include "DynamicBuffer.h"
#include "Str.h"
#include "FileIO.h"
#include "SpecialMath.h"
#include<maya/MFnMesh.h>
#include<maya/MFnTransform.h>
#include<maya/MFnPhongEShader.h>

#include<maya/MItDag.h>
#include<maya/MPointArray.h>
#include<maya/MFloatVectorArray.h>
#include<maya/MFloatArray.h>
#include<maya/MPlugArray.h>
#include<maya/MItMeshPolygon.h>
#include<maya/MDagPathArray.h>
 	#include<maya/MDagPath.h>
#include<maya/MItGeometry.h>
#include<maya/MSelectionList.h>
#include<maya/MDoubleArray.h>

#include<maya/MFnSkinCluster.h>
#include<maya/MQuaternion.h>
#include<maya/MFnIkJoint.h>
#include <maya/MTypes.h>
#include<maya/MFnLambertShader.h>
#include<maya/MFnBlinnShader.h>
#include<maya/MFnPhongShader.h>
#include<maya/MItDependencyNodes.h> 
#include "DoublesSorter.h"
#include <maya/MFnAnimCurve.h>
#include<maya/MAnimControl.h>
#include "StringDynamicBuffer.h"
#include "BoneIndexer.h"
#include "Mesh.h"
#include "Quaternion.h"
#include<maya/MMatrix.h>
#include<ConnectedListSTRINGS.h>
#include "SaveMesh.h"
#include "Parrenting.h"
bool SavedMeshHas4AnglePollys;
bool SavedMeshHas3AnglePollys;
BoneIndexer MeshesIndexer;
ExportShader ExShaer;
BoneIndexer BBOXIndexer;
BoneIndexer NavMeshIndexer;
Parenting PT;
Parenting SP;
Parenting TREE;
MeshCount MeshCounter;
SaveMesh SMesh;
WeightsExporter WE;
FramesForExport ExFrames;
Hierarhy  BHierarhy;
ConnectedListStrings ConnStr;
ConnectedListStrings ConnStrAllBones;
void OutputColor(MFnDependencyNode& fn,const char* name,StringDynamicBuffer & DBuffer, OneExportShader *);
void MayaAddFloatToBuffer(float num,StringDynamicBuffer & DBuffer);
void MayaAddIntToBuffer(int num,StringDynamicBuffer & DBuffer);
//void OutputSkinCluster(MObject& obj,StringDynamicBuffer & DBuffer);
bool OutputBone(MObject& obj, StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer, ExHierarhy * BH);
bool OutputBoneForFrame(MObject& obj, StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer,int FrameNum,int & BoneNum, OneExportFrame * EF);
int SaveBonesOnlyForCalculation();
//void OutputParent(MObject& obj, StringDynamicBuffer & DBuffer);
void SaveFrameTimes(DoublesSorter & Times);
int SaveBones(StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer, ExHierarhy * BH);
int SaveBonesForFrame(StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer, int FrameNum, OneExportFrame * EF);
void SaveFrame(int FrameNum, StringDynamicBuffer & DBuffer, DoublesSorter & Times,BoneIndexer & BIndexer, OneExportFrame * EF);
MObject ReturnObjectToAdd(BoneIndexer & NamesIndexer, int & MeshesAddedCounter, bool & Reset );
void OutputSkinCluster(MObject& obj,StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer,int NumMeshes);
bool OutputBoneOnlyForCalculation(MObject& obj);
void SaveDefaultFrame(StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer);
void ConvertJointsFromLocalToWorld(void);
float * GetMeshMatrix(char * MeshName);
char *  GetMeshName(char * MeshSTR);
cMesh   GlobalMesh;
unsigned int GetBOXIndex(char * BoxName);
bool MeshHasParrents(char * MeshName);
 void SaveMesh(OneMesh * MeshToSave);
float * GetMeshAABB(char * MeshName);
char * GetParrent(char * MeshName, bool & result);
void CheckSceneRoot();
unsigned int GetNumObj(char * ObjName);
struct BoneType
{
int parent;
double posX;
double posY;
double posZ;
double orientX;
double orientY;
double orientZ;
};

struct PollyToExport_UVs
{
	int uv_index;
};

struct PollyToExport // Перед экспортом сохраним в  массив этого типа полигоны для того чтоб при записи в файл их сортировать по материалу(шейдерам)
{
	int VertexIndex;
	int NormalIndex;
	PollyToExport_UVs * UVs;
};
struct PollyToExport_Data
{
	int VertexCount;
	PollyToExport * Data;
	int FaceNormalIndex;
};


struct MemoryForWeightsCalculator
{
	int StartIndex;
	int Count;
};

struct PluginMeshes
{
char MeshName[255];
int NumVertices;
MemoryForWeightsCalculator * Calcs;
};
int OutputSkinClusterOnlyForalculation(BoneIndexer & NamesIndexer,MObject& obj, int & MaxInd, int GeomNumber, int & RetValueWeightsCalculated, PluginMeshes * MeshCalculatorMemory, int & NumVertexes);
bool SaveBonesToMemory(MObject& obj,BoneType * MemoryForBone, int memCounter);
MString GetShaderName(MObject shadingEngine)
{
	// attach a function set to the shading engine
	MFnDependencyNode fn( shadingEngine );

	// get access to the surfaceShader attribute. This will be connected to
	// lambert , phong nodes etc.
	MPlug sshader = fn.findPlug("surfaceShader");

	// will hold the connections to the surfaceShader attribute
	MPlugArray materials;

	// get the material connected to the surface shader
	sshader.connectedTo(materials,true,false);

	// if we found a material
	if(materials.length())
	{
		MFnDependencyNode fnMat(materials[0].node());
		return fnMat.name();
	}
	return "none";
}

void SaveObjectsToAdd()
{
	MDagPath skinPath;
		MObject object;
		bool GoToNextIttem = false;
		MItDependencyNodes iter( MFn::kSkinClusterFilter );
		MDagPathArray infs;
		MStatus       stat;
		for ( ; !iter.isDone(); iter.next() )
			{

					GoToNextIttem = false;
					object = iter.item();
					MFnSkinCluster skinCluster(object);
						if (!skinCluster.influenceObjects(infs,&stat))
						 {					
							GoToNextIttem = true;
						 }
					int num_geoms=skinCluster.numOutputConnections();
						for (int i=0;i<num_geoms;i++)
						{
							int index=skinCluster.indexForOutputConnection(i);
							 skinCluster.getPathAtIndex(index,skinPath);
							 MFnDagNode dagNode(skinPath);
								if (dagNode.isIntermediateObject())
								{
									GoToNextIttem = true;
									break;
								}
								if(!skinPath.hasFn( MFn::kMesh ))
								{
									GoToNextIttem = true;
									break;
								}


							}
						if(!GoToNextIttem)
						{
							// Сохраним обьект.
							//cout << "Object ToExport" <<(char *)skinPath.partialPathName().asChar()<< endl;
							ConnStr.AddUnit((char *)skinPath.partialPathName().asChar());
							//return object;
						}

				}

}
bool IsItRootBone(char * BoneName)
{
	char ErrorMessage[255];
	for(int j=0;j!=BHierarhy.NumBones ; j++)
		{
			if(!strcmp(&BHierarhy.Hy[j].BoneName[0], BoneName))
			{
				if(!BHierarhy.Hy[j].Parrent) 
				{
					return true;
				} else return false;
			}
		}

// Ошибка. Не нашли кость с таким именем.
sprintf(&ErrorMessage[0], "IsItRootBone: could not find bone \"%s\"",  BoneName);
MessageBoxA(0, &ErrorMessage[0], "ERROR", MB_OK);
return false;
}

bool IsThisMeshAlreadyDefined(char * MeshName)
{char ErrorMessage[255];


OneMesh * TempExMesh = SMesh.FirstMesh;
for(int i = 0; i!=SMesh.NumMeshes; i++)
{
	if(!strcmp( &TempExMesh->Name[0], MeshName))
	{
		sprintf(&ErrorMessage[0], "IsThisMeshAlreadyDefined: Mesh \"%s\" already defined.",  MeshName);
		MessageBoxA(0, &ErrorMessage[0], "ERROR", MB_OK);
		return true;
	}
}


return false;
}

bool IsThisMeshFileNameAlreadyDefined(char * MeshFileName)
{char ErrorMessage[255];


OneMesh * TempExMesh = SMesh.FirstMesh;
for(int i = 0; i!=SMesh.NumMeshes; i++)
{
	if(!strcmp( &TempExMesh->MeshFileName[0], MeshFileName))
	{
		sprintf(&ErrorMessage[0], "IsThisMeshFileNameAlreadyDefined: File name \"%s\" already defined.",  MeshFileName);
		MessageBoxA(0, &ErrorMessage[0], "ERROR", MB_OK);
		return true;
	}
}


return false;
}
ExBone * GetBoneCoords(char * BoneName)
{
char ErrorMessage[255];
int i;
	for( i=0 ; i!=BHierarhy.NumBones; i++ )
	{
		if(!strcmp( &ExFrames.ExFrames[0].Bones[i].Name[0], BoneName))
		{
			return &ExFrames.ExFrames[0].Bones[i];
		}
	}
sprintf(&ErrorMessage[0], "GetBoneCoords: could not find bone \"%s\"",  BoneName);
MessageBoxA(0, &ErrorMessage[0], "ERROR", MB_OK);
return 0;
}
ExBone * GetRootBoneForMesh(char * MeshName)
{	char ErrorMessage[255];
	ExBone * ReturnValue = 0;
	OneWeight * TempWeight = WE.Weights;; 
	
	while(TempWeight)
	{
		if(!strcmp(GetMeshName(&TempWeight->InfluencedMeshName[0]), MeshName))
		{
			if(IsItRootBone(&TempWeight->JointName[0]))
			{
				if(ReturnValue)
				{
					if(strcmp(&TempWeight->JointName[0], &ReturnValue->Name[0]))
					{
						sprintf(&ErrorMessage[0], "GetRootBoneForMesh: Mesh \"%s\" has more than one root joint",  MeshName);
						MessageBoxA(0, &ErrorMessage[0], "ERROR", MB_OK);
						return 0;
					}
				}
				ReturnValue = GetBoneCoords(&TempWeight->JointName[0]);

			}
		}
		TempWeight = TempWeight->NextWeight;
	}

	return ReturnValue;
}

void SaveMeshTransforms(Meshes * TempMesh)
{

MItDag TransformEnum(MItDag::kDepthFirst, MFn::kTransform); 
bool FoundMesh;
while(!TransformEnum.isDone())
{ 
	FoundMesh = false;
MFnTransform TransItem(TransformEnum.item());
if( !TransItem.isIntermediateObject() ) { 	
//cout<<"Transform "<< fn.name().asChar() <<endl;


	if(CompareStringMaya(&TempMesh->name[0], (char *)TransItem.name().asChar(),GetStringLengh(&TempMesh->name[0]),GetStringLengh((char *)TransItem.name().asChar())))
	{
		FoundMesh = true;
			
	}else
	{
FoundMesh = false;
	}

if(FoundMesh)
{
//cout << TempMesh->name << endl;
	MMatrix mat = TransItem.transformation().asMatrix();
MQuaternion Rotation(0,0,0,1);
TransItem.getRotation(Rotation);

TempMesh->MeshMatrix.m[0] = mat.matrix[0][0];
TempMesh->MeshMatrix.m[1] = mat.matrix[0][1];
TempMesh->MeshMatrix.m[2] = mat.matrix[0][2];
TempMesh->MeshMatrix.m[3] = mat.matrix[0][3];

TempMesh->MeshMatrix.m[4] = mat.matrix[1][0];
TempMesh->MeshMatrix.m[5] = mat.matrix[1][1];
TempMesh->MeshMatrix.m[6] = mat.matrix[1][2];
TempMesh->MeshMatrix.m[7] = mat.matrix[1][3];

TempMesh->MeshMatrix.m[8] = mat.matrix[2][0];
TempMesh->MeshMatrix.m[9] = mat.matrix[2][1];
TempMesh->MeshMatrix.m[10] = mat.matrix[2][2];
TempMesh->MeshMatrix.m[11] = mat.matrix[2][3];

TempMesh->MeshMatrix.m[12] = mat.matrix[3][0];
TempMesh->MeshMatrix.m[13] = mat.matrix[3][1];
TempMesh->MeshMatrix.m[14] = mat.matrix[3][2];
TempMesh->MeshMatrix.m[15] = mat.matrix[3][3];




TempMesh->MeshPos.x =	mat.matrix[3][0];
TempMesh->MeshPos.y =	mat.matrix[3][1];
TempMesh->MeshPos.z =	mat.matrix[3][2];
TempMesh->MeshOrientation.x = Rotation.x;
TempMesh->MeshOrientation.y = Rotation.y;
TempMesh->MeshOrientation.z = Rotation.z;
TempMesh->MeshOrientation.w = Rotation.w;
/*
//cout	<< "translation " 
<< mat.matrix[3][0] << " " 
<< mat.matrix[3][1] << " "
<< mat.matrix[3][2] << endl; 
//cout	<< "rotation " 
<< Rotation.x << " " 
<< Rotation.y << " " 
<< Rotation.z << " "
<< Rotation.w << endl; 
*/
}
}
TransformEnum.next(); 

}






}
BoneIndexer ShaderNamesIndexer;

char * Token(char * Data, unsigned int & Size)
{
Size = 0;
if(Data)
{
if(*Data)
{
char * ReturnValue = Data;
while (!(((*ReturnValue >= '0')&&(*ReturnValue <= '9')) || ((*ReturnValue >= 'a')&&(*ReturnValue <= 'z')) || ((*ReturnValue >= 'A')&&(*ReturnValue <= 'Z')) ))
{
ReturnValue++;
}

while (((*ReturnValue >= '0')&&(*ReturnValue <= '9')) || ((*ReturnValue >= 'a')&&(*ReturnValue <= 'z')) || ((*ReturnValue >= 'A')&&(*ReturnValue <= 'Z')) )
{
ReturnValue++;
Size++;
}
if(*ReturnValue==0)
{
return NULL;
}
while (!(((*ReturnValue >= '0')&&(*ReturnValue <= '9')) || ((*ReturnValue >= 'a')&&(*ReturnValue <= 'z')) || ((*ReturnValue >= 'A')&&(*ReturnValue <= 'Z')) ))
{

ReturnValue++;
if(*ReturnValue==0)
{
return NULL;
}
}

	return ReturnValue;
}
}
return NULL;
}

enum {TYPE_UNKNOUN, TYPE_MESH, TYPE_BBOX, TYPE_NAVMESH};
unsigned int  GetMeshType(char * MeshSTR)
{char ErrorMessage[255];
	char * DATA;
	char * STR;
	unsigned int Size;
DATA = MeshSTR;
DATA = Token(DATA, Size);
STR = MeshSTR;
	if(Size == 3)
	{
		if((STR[0] == 'B')&&(STR[1] == 'O')&&(STR[2] == 'X'))
		{
			return TYPE_BBOX;
		}
	}
	if(Size == 4)
	{
		if((STR[0] == 'M')&&(STR[1] == 'E')&&(STR[2] == 'S')&&(STR[3] == 'H'))
		{
			return TYPE_MESH;
		}
	}
	if(Size == 3)
	{
		if((STR[0] == 'N')&&(STR[1] == 'A')&&(STR[2] == 'V'))
		{
			return TYPE_NAVMESH;
		}
	}
sprintf(&ErrorMessage[0], "Mesh \"%s\" has UNKNOUN TYPE",  MeshSTR);
MessageBox(0, &ErrorMessage[0], "ERROR", MB_OK);
return TYPE_UNKNOUN;
}
char *  GetMeshName(char * MeshSTR)
{
char ErrorMessage[255];
unsigned int Size;
char ReturnValue[255];
char * DATA;
char * STR;
DATA = MeshSTR;
DATA = Token(DATA, Size);
STR = DATA;
DATA = Token(DATA, Size);
if(Size == 0)
{
sprintf(&ErrorMessage[0], "Mesh \"%s\" has NO  NAME",  MeshSTR);
MessageBox(0, &ErrorMessage[0], "ERROR", MB_OK);
return 0;
}
ReturnValue[Size] = 0;
memcpy(&ReturnValue[0], STR, Size);

return &ReturnValue[0];//STR;
}
unsigned int  GetMeshTypeNoMessage(char * MeshSTR)
{
	char * DATA;
	char * STR;
	unsigned int Size;
DATA = MeshSTR;
DATA = Token(DATA, Size);
STR = MeshSTR;
	if(Size == 3)
	{
		if((STR[0] == 'B')&&(STR[1] == 'O')&&(STR[2] == 'X'))
		{
			return 1;
		}
	}
	if(Size == 4)
	{
		if((STR[0] == 'M')&&(STR[1] == 'E')&&(STR[2] == 'S')&&(STR[3] == 'H'))
		{
			return 1;
		}
	}
	if(Size == 3)
	{
		if((STR[0] == 'N')&&(STR[1] == 'A')&&(STR[2] == 'V'))
		{
			return 1;
		}
	}
return 0;
}

char *  GetMeshNameNoMessage(char * MeshSTR)
{
unsigned int Size;
char ReturnValue[255];
char * DATA;
char * STR;
DATA = MeshSTR;
DATA = Token(DATA, Size);
STR = DATA;
DATA = Token(DATA, Size);
if(Size == 0)
{
return "";
}
ReturnValue[Size] = 0;
memcpy(&ReturnValue[0], STR, Size);

return &ReturnValue[0];//STR;
}

char *  GetMeshFileNameNoMessage(char * MeshSTR)
{
unsigned int Size;
char ReturnValue[255];
char * DATA;
char * STR;
DATA = MeshSTR;
DATA = Token(DATA, Size);
DATA = Token(DATA, Size);
STR = DATA;
DATA = Token(DATA, Size);
if(Size == 0)
{
return 0;
}
ReturnValue[Size] = 0;
memcpy(&ReturnValue[0], STR, Size);
DATA = Token(DATA, Size);
if(Size)
{
return 0;
}
return &ReturnValue[0];//STR;
}
char *  GetMeshFileName(char * MeshSTR)
{
char ErrorMessage[255];
unsigned int Size;
char ReturnValue[255];
char * DATA;
char * STR;
DATA = MeshSTR;
DATA = Token(DATA, Size);
DATA = Token(DATA, Size);
STR = DATA;
DATA = Token(DATA, Size);
if(Size == 0)
{
sprintf(&ErrorMessage[0], "Mesh \"%s\" has NO FILE NAME",  MeshSTR);
MessageBox(0, &ErrorMessage[0], "ERROR", MB_OK);
return 0;
}
ReturnValue[Size] = 0;
memcpy(&ReturnValue[0], STR, Size);
DATA = Token(DATA, Size);
if(Size)
{
sprintf(&ErrorMessage[0], "Mesh \"%s\" has invalid NAME",  MeshSTR);
MessageBox(0, &ErrorMessage[0], "ERROR", MB_OK);
return 0;
}
return &ReturnValue[0];//STR;
}


char * RemoveShapeSymbol(char * Name)
{
	char  ReturnValue[255];
unsigned int l = strlen(Name);
unsigned int j;
if(l < 5) return Name;
		if((Name[l-5] == 'S')&&(Name[l-4] == 'h')&&(Name[l-3] == 'a')&&(Name[l-2] == 'p')&&(Name[l-1] == 'e'))
		{
			l-=5;
			memcpy(&ReturnValue[0], Name, l);
			ReturnValue[l]=0;
			return &ReturnValue[0];
		}else if((Name[l-4] == 'O')&&(Name[l-3] == 'r')&&(Name[l-2] == 'i')&&(Name[l-1] == 'g'))
		{
			l-=4;
			memcpy(&ReturnValue[0], Name, l);
			ReturnValue[l]=0;
			return &ReturnValue[0];
		}
j = l;
j--;
while(j)
{
	if(!((Name[j] >= '0') && (Name[j]<= '9'))) break;
	j--;
}
if(j < 4) return Name;
if((Name[j-3] == 'O')&&(Name[j-2] == 'r')&&(Name[j-1] == 'i')&&(Name[j] == 'g'))
{
j-=3;
			memcpy(&ReturnValue[0], Name, j);
			ReturnValue[j]=0;
			return &ReturnValue[0];
}

if(j < 5) return Name;
if((Name[j-4] == 'S')&&(Name[j-3] == 'h')&&(Name[j-2] == 'a')&&(Name[j-1] == 'p')&&(Name[j] == 'e'))
{
j-=4;
			memcpy(&ReturnValue[0], Name, j);
			ReturnValue[j]=0;
			return &ReturnValue[0];
}

			return Name;
}

void outputParenting(MObject& obj, char * MeshName)
{
int l;
	char RetVal[255];
	char * ChildName;
MFnDagNode fn(obj);
for( int i=0; i!=fn.childCount(); ++i ) {
MObject obj = fn.child(i);
MFnDagNode fnChild(obj);
if(GetMeshTypeNoMessage((char *)fnChild.name().asChar()))
{	
	
	if(GetMeshNameNoMessage((char *)fnChild.name().asChar()))
	{
		
			if(GetMeshFileNameNoMessage((char *)fnChild.name().asChar()))
			{
				
				l = strlen((char *)fnChild.name().asChar());
				ChildName =  RemoveShapeSymbol((char *)fnChild.name().asChar());
				if(strlen(ChildName) == l)
				{// Если имя без Shape
				memcpy(&RetVal[0], ChildName, strlen(ChildName)+1);
				ChildName =  GetMeshNameNoMessage(&RetVal[0]);
				memcpy(&RetVal[0], ChildName, strlen(ChildName)+1);
				if(GetStringLengh(&RetVal[0]))
				{
					//MessageBox(0, MeshName, &RetVal[0],MB_OK);
					PT.AddChild(MeshName, &RetVal[0]);
				}
				}
				
			}
	
	}
	
}

}

} 

void SParents(MObject& obj, char * MeshName)
{
int l;
	char RetVal[255];
	char * ParrentName;
MFnDagNode fn(obj);
if(fn.parentCount() > 1)
{
	char ErrorMessage[255];
	sprintf(&ErrorMessage[0], "Mesh \"%s\" has more than 1 parrent." , MeshName);
	MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
}

for( int i=0; i!=fn.parentCount(); ++i ) {
MObject obj = fn.parent(i);
MFnDagNode fnParent(obj);
if(GetMeshTypeNoMessage((char *)fnParent.name().asChar()))
{	
	
	if(GetMeshNameNoMessage((char *)fnParent.name().asChar()))
	{
		
			if(GetMeshFileNameNoMessage((char *)fnParent.name().asChar()))
			{
				
				l = strlen((char *)fnParent.name().asChar());
				ParrentName =  RemoveShapeSymbol((char *)fnParent.name().asChar());
				if(strlen(ParrentName) == l)
				{// Если имя без Shape
				memcpy(&RetVal[0], ParrentName, strlen(ParrentName)+1);
				ParrentName =  GetMeshNameNoMessage(&RetVal[0]);
				memcpy(&RetVal[0], ParrentName, strlen(ParrentName)+1);
				if(GetStringLengh(&RetVal[0]))
				{
					//MessageBox(0, MeshName, &RetVal[0],MB_OK);
					SP.AddChild(MeshName, &RetVal[0]);
				}
				}
				
			}
	
	}
	
}

}

} 

void SaveParentInfo(void)
{
char RetVal[255];
MItDag it(MItDag::kDepthFirst, MFn::kTransform);
char * Name;
while(!it.isDone())
{
MFnTransform fn(it.item());
if( !fn.isIntermediateObject() ) {


	if(GetMeshTypeNoMessage((char *)fn.name().asChar()))
	{	
	if(GetMeshNameNoMessage((char *)fn.name().asChar()))
	{
			if(GetMeshFileNameNoMessage((char *)fn.name().asChar()))
			{
				Name = RemoveShapeSymbol((char *)fn.name().asChar());
				memcpy(&RetVal[0], Name, strlen(Name)+1);

				Name = GetMeshNameNoMessage(&RetVal[0]);
				memcpy(&RetVal[0], Name, strlen(Name)+1);

					if(GetStringLengh(&RetVal[0]))
					{
						
						outputParenting(it.item(),&RetVal[0] );
						
					}
			}
	}
	}


}
it.next();
}

}
void SaveParents(void)
{
char RetVal[255];
MItDag it(MItDag::kDepthFirst, MFn::kTransform);
char * Name;
while(!it.isDone())
{
MFnTransform fn(it.item());
if( !fn.isIntermediateObject() ) {


	if(GetMeshTypeNoMessage((char *)fn.name().asChar()))
	{	
	if(GetMeshNameNoMessage((char *)fn.name().asChar()))
	{
			if(GetMeshFileNameNoMessage((char *)fn.name().asChar()))
			{
				Name = RemoveShapeSymbol((char *)fn.name().asChar());
				memcpy(&RetVal[0], Name, strlen(Name)+1);

				Name = GetMeshNameNoMessage(&RetVal[0]);
				memcpy(&RetVal[0], Name, strlen(Name)+1);

					if(GetStringLengh(&RetVal[0]))
					{
						
						SParents(it.item(),&RetVal[0] );
						
					}
			}
	}
	}


}
it.next();
}

}


MStatus initializePlugin( MObject obj )
{
	ExBone * RBone;
	MeshCounter.NumMeshes = 0;
	MeshCounter.NumNavMeshes = 0;
	MeshCounter.NumBoxes = 0;

	OneExportShader * TempShader;
	
	ExShaer.NumShaders = 0;
	uvset * TempUvSetInfo;
	SMesh.NumMeshes=0;
	SMesh.FirstMesh = 0;
	SMesh.LastMeshAdded = 0;
	OneMesh * MeshToAdd;
	MVector FaceNormal;
	
	int SavedNormalIndex;
	bool ConnectedToBone=false;
	SaveObjectsToAdd();  //Сохраним обьекты, привязанные к костям
	Meshes * TempMesh;
	char DebugText[255];
	int NumMeshesCalculated=0;
	MObjectArray Shaders;
int MeshNameLengh;
int SecondMeshNameLengh;
		// this is used to hold indices to the materials returned in the object array
		MIntArray    FaceIndices;

BoneIndexer NamesIndexer;


BoneIndexer BonIndexer;
	int PollyNum;
	int NumInstances;
	int uv_index;
	int SetsLengh;
	int vc;
	bool bUvs;

bool UvSetsLenghAlreadyCalculated = false;
	char * UvSetName;
	StringDynamicBuffer DBuffer;
	MStatus   status;
	MFnPlugin plugin( obj, "", "2008", "Any");
MPointArray vts;
MPointArray vtsForCount;
// this will hold the returned vertex positions
MFloatVectorArray nmls;
MStringArray uvsets;

// Нам надо посчитать сколько у нас мешей за одно их проиндексируем
MItDag MeshIterator(MItDag::kDepthFirst,MFn::kMesh);
int NumMeshes=0;
while(!MeshIterator.isDone())
{
// attach the function set to the object
MFnMesh Meshfn(MeshIterator.item());
if( !Meshfn.isIntermediateObject() ) {
//Meshfn.getPoints(vtsForCount);
//vtsForCount.length();
	if(ConnStr.CheckIsItemInList((char *)Meshfn.name().asChar()))  // Индексируем только те обьекты которые привязаны к костям
	{
	NamesIndexer.AddBone((char *)Meshfn.name().asChar());
	
	}
	NumMeshes++;
}
// get next mesh
MeshIterator.next();
} 
PluginMeshes * MeshCalculatorMemory;
MeshCalculatorMemory = new PluginMeshes[NumMeshes+1];
NumMeshesCalculated = NumMeshes;
NumMeshes = 0;
// Теперь посчитаем сколько вертексов в каждом меше
MItDag MeshIteratorNumVertex(MItDag::kDepthFirst,MFn::kMesh);
while(!MeshIteratorNumVertex.isDone())
{
// attach the function set to the object
MFnMesh MeshfnNumVertex(MeshIteratorNumVertex.item());
if( !MeshfnNumVertex.isIntermediateObject() ) {
	memcpy(MeshCalculatorMemory[NumMeshes].MeshName,(char *)MeshfnNumVertex.name().asChar(),GetStrSize((char *)MeshfnNumVertex.name().asChar())+1);
	
MeshfnNumVertex.getPoints(vtsForCount);
MeshCalculatorMemory[NumMeshes].NumVertices  = vtsForCount.length();
MeshCalculatorMemory[NumMeshes].Calcs = new MemoryForWeightsCalculator[MeshCalculatorMemory[NumMeshes].NumVertices+1];
for(int i = 0; i!= MeshCalculatorMemory[NumMeshes].NumVertices; i++)
{
	MeshCalculatorMemory[NumMeshes].Calcs[i].Count = 0;
	MeshCalculatorMemory[NumMeshes].Calcs[i].StartIndex = 0;
}
NumMeshes++;

}
// get next mesh
MeshIteratorNumVertex.next();
} 
GlobalMesh.Mesh.NumMeshes = NumMeshes;
if(NumMeshes)
{
TempMesh = &GlobalMesh.Mesh.Mesh;
for(int i =0; i != NumMeshes-1;i++)
{
	TempMesh->NextMesh = new Meshes;
	
	TempMesh = TempMesh->NextMesh;
}
TempMesh->NextMesh = NULL;
}
int IndexForThisMesh;
int WeightsCalculated=0;
int NumGeometry=0;
int MaxWeightsCalculator=0;
int Param=0;
int NumVertexes=0;
int WeightsCalculator = 0;
MItDependencyNodes iterCalculation( MFn::kSkinClusterFilter );
for ( ; !iterCalculation.isDone(); iterCalculation.next() ) {
MObject objectforCalc = iterCalculation.item();
Param=0;
WeightsCalculator+=OutputSkinClusterOnlyForalculation(NamesIndexer,objectforCalc,Param,NumGeometry,WeightsCalculated, MeshCalculatorMemory,NumVertexes);
NumGeometry++;

//if(MaxWeightsCalculator) // Если хотябы 1 раз запускали
//{
	//if(Param)
	//{
//if(Param != MaxWeightsCalculator)MaxWeightsCalculator = 1000000;
	//}
	//}
if(Param > MaxWeightsCalculator)MaxWeightsCalculator = Param;
//OutputSkinCluster(object,DBuffer);
}

TempMesh = &GlobalMesh.Mesh.Mesh;
//MessageBox(0,"1","1",MB_OK);
// create an iterator to go through all meshes
MItDag it(MItDag::kDepthFirst,MFn::kMesh);
NumMeshes = 0;
while(!it.isDone())
{
  	

// attach the function set to the object
MFnMesh fn(it.item());
fn.getConnectedShaders(0,Shaders,FaceIndices);

// only want non-history items
if( !fn.isIntermediateObject() ) {

	fn.getPoints(vts);
fn.getNormals(nmls);
fn.getUVSetNames(uvsets);
MItMeshPolygon itPoly(it.item());
MItMeshPolygon itPolyForCalc(it.item());
// Посчитаем сколько у нас треугольников и квадов
int Num3AnglePollys = 0;
int Num4AnglePollys = 0;

while(!itPolyForCalc.isDone()) {
vc = itPolyForCalc.polygonVertexCount();
if(vc == 3) Num3AnglePollys++;
if(vc == 4) Num4AnglePollys++;
itPolyForCalc.next();
}

  	// print mesh name
//cout<<"Mesh "<< fn.name().asChar() <<endl;
//fn.getUVSetNames(uvsets);

if( !uvsets.length() || !fn.numUVs(uvsets[0]) ) {
  	bUvs = false;
} else bUvs = true; 

//DBuffer.AddData("Mesh	{\n");
MeshToAdd = new OneMesh;


//DBuffer.AddData("			Name		");
//DBuffer.AddData((char *)fn.name().asChar());
MeshNameLengh = GetStringLengh((char *)fn.name().asChar());


//MessageBox(0,"2","2",MB_OK);
SecondMeshNameLengh = MeshNameLengh;
memcpy(TempMesh->name,(char *)fn.name().asChar(),MeshNameLengh+1);
MeshNameLengh--;
while((TempMesh->name[MeshNameLengh] >= '0')&&(TempMesh->name[MeshNameLengh] <= '9') )
{
MeshNameLengh--;
}
memcpy(&TempMesh->name[MeshNameLengh-4],&TempMesh->name[MeshNameLengh+1] ,SecondMeshNameLengh-MeshNameLengh);
if(GetMeshType((char *)&TempMesh->name) == TYPE_UNKNOUN) return status; // Прежде чем работать с именем проверим правильный ли тип
if(GetMeshName((char *)&TempMesh->name) == 0) return status; // Прежде чем работать с именем проверим правильное ли  имя меша
if(GetMeshFileName((char *)&TempMesh->name) == 0) return status; // Прежде чем работать с именем проверим правильное ли имя файла

memcpy(&MeshToAdd->Name[0],GetMeshName((char *)&TempMesh->name),GetStringLengh(GetMeshName((char *)&TempMesh->name))+1);
if(IsThisMeshAlreadyDefined(&MeshToAdd->Name[0])) return status;
//MessageBox(0, &MeshToAdd->Name[0], "Test", MB_OK);
//DBuffer.AddData((char *)&TempMesh->name);
memcpy(&MeshToAdd->MeshFileName[0],GetMeshFileName((char *)&TempMesh->name),GetStringLengh(GetMeshFileName((char *)&TempMesh->name))+1);
MeshToAdd->MeshType = GetMeshType((char *)&TempMesh->name);
//enum {TYPE_UNKNOUN, TYPE_MESH, TYPE_BBOX, TYPE_NAVMESH};
if(MeshToAdd->MeshType == TYPE_MESH) MeshCounter.NumMeshes++;
if(MeshToAdd->MeshType == TYPE_BBOX) MeshCounter.NumBoxes++;
if(MeshToAdd->MeshType == TYPE_NAVMESH) MeshCounter.NumNavMeshes++;
if(GetStringLengh( RemoveShapeSymbol(&MeshToAdd->MeshFileName[0])) != GetStringLengh(&MeshToAdd->MeshFileName[0]))
{
char ErrorMessage[255];
sprintf(&ErrorMessage[0], "Mesh name: \"%s\". Invalid mesh name. Mesh has \"Shape\" in name. " , &MeshToAdd->MeshFileName[0]);
MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
return status;
}
PT.AddElement(&MeshToAdd->Name[0]);
SP.AddElement(&MeshToAdd->Name[0]);


NumInstances =  fn.parentCount();
SaveMeshTransforms(TempMesh);
if(ConnStr.CheckIsItemInList((char *)fn.name().asChar()))
{
//DBuffer.AddData("\n			ConnectedToBone	Yes");
MeshToAdd->ConnectedToBone = true;
ConnectedToBone = true;
}else
{
MeshToAdd->ConnectedToBone = false;
//DBuffer.AddData("\n			ConnectedToBone	No");
//DBuffer.AddData("\n			MeshMatrix	");
ConnectedToBone = false;
//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[0], DBuffer);
MeshToAdd->Matrix[0] = TempMesh->MeshMatrix.m[0];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[1], DBuffer);
MeshToAdd->Matrix[1] = TempMesh->MeshMatrix.m[1];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[2], DBuffer);
MeshToAdd->Matrix[2] = TempMesh->MeshMatrix.m[2];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[3], DBuffer);
MeshToAdd->Matrix[3] = TempMesh->MeshMatrix.m[3];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[4], DBuffer);
MeshToAdd->Matrix[4] = TempMesh->MeshMatrix.m[4];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[5], DBuffer);
MeshToAdd->Matrix[5] = TempMesh->MeshMatrix.m[5];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[6], DBuffer);
MeshToAdd->Matrix[6] = TempMesh->MeshMatrix.m[6];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[7], DBuffer);
MeshToAdd->Matrix[7] = TempMesh->MeshMatrix.m[7];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[8], DBuffer);
MeshToAdd->Matrix[8] = TempMesh->MeshMatrix.m[8];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[9], DBuffer);
MeshToAdd->Matrix[9] = TempMesh->MeshMatrix.m[9];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[10], DBuffer);
MeshToAdd->Matrix[10] = TempMesh->MeshMatrix.m[10];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[11], DBuffer);
MeshToAdd->Matrix[11] = TempMesh->MeshMatrix.m[11];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[12], DBuffer);
MeshToAdd->Matrix[12] = TempMesh->MeshMatrix.m[12];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[13], DBuffer);
MeshToAdd->Matrix[13] = TempMesh->MeshMatrix.m[13];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[14], DBuffer);
MeshToAdd->Matrix[14] = TempMesh->MeshMatrix.m[14];
//DBuffer.AddData(" ");

//MayaAddFloatToBuffer(TempMesh->MeshMatrix.m[15], DBuffer);
MeshToAdd->Matrix[15] = TempMesh->MeshMatrix.m[15];
//DBuffer.AddData(" ");
}
//DBuffer.AddData("\n			NumInstances	");
//MayaAddIntToBuffer(NumInstances,DBuffer);
MeshToAdd->NumInstances = NumInstances;
///MessageBox(0,"3","3",MB_OK);

//DBuffer.AddData("\n			NumPollys	");
unsigned int PolygonCount = itPoly.count();
MeshToAdd->PolygonCount = PolygonCount;
MeshToAdd->Pollys = new EPolly[PolygonCount];
//MayaAddIntToBuffer(PolygonCount,DBuffer);

//DBuffer.AddData("\n			Num3AnglePollys	");
MeshToAdd->Num3AnglePollys = Num3AnglePollys;
//MayaAddIntToBuffer(Num3AnglePollys,DBuffer);
if((MeshToAdd->MeshType == TYPE_NAVMESH) && Num3AnglePollys)
{
	char ErrorMessage[255];
	sprintf(&ErrorMessage[0], "Mesh name: \"%s\". NavMesh does not support 3 angle pollys. Quadrangulate it!" , &MeshToAdd->Name[0]);
MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
return status;
}
//DBuffer.AddData("\n			Num4AnglePollys	");
MeshToAdd->Num4AnglePollys = Num4AnglePollys;
//MayaAddIntToBuffer(Num4AnglePollys,DBuffer);
if(MeshToAdd->Num4AnglePollys && MeshToAdd->Num3AnglePollys) 
{
	char ErrorMessage[255];
	sprintf(&ErrorMessage[0], "Mesh \"%s\" has 3 and 4 agle pollys. Engine does ot support." , &MeshToAdd->Name[0]);
MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
return status;
}
//DBuffer.AddData("\n			NumVertices	");
//MayaAddIntToBuffer(vts.length(),DBuffer);
MeshToAdd->NumVertices = vts.length();
TempMesh->NumVertices = vts.length();
TempMesh->MeshVertices = new MeshVertex[vts.length()];
//DBuffer.AddData("\n			NumNormals	");
//MayaAddIntToBuffer(nmls.length(),DBuffer);
MeshToAdd->NumNormals = nmls.length()+itPoly.count();
if(!UvSetsLenghAlreadyCalculated)
{
SetsLengh = uvsets.length();
UvSetsLenghAlreadyCalculated = true;
}
//DBuffer.AddData("\n			NumUVs	");
//MayaAddIntToBuffer(SetsLengh,DBuffer);
MeshToAdd->NumUVs = SetsLengh;
MeshToAdd->UvsetInfo = 0;
MeshToAdd->LastSetAdded = 0;
for(int j=0;j!=SetsLengh;++j) {
TempUvSetInfo = new uvset;
//DBuffer.AddData("\n			UVset	");
UvSetName = (char *)uvsets[j].asChar();
memcpy(&TempUvSetInfo->Name[0],UvSetName,GetStringLengh(UvSetName)+1);
//DBuffer.AddData(UvSetName);
MFloatArray u_coords;
MFloatArray v_coords;
fn.getUVs(u_coords,v_coords,&uvsets[j]);
//DBuffer.AddData(" ");
TempUvSetInfo->NumUVs = fn.numUVs(uvsets[j]);
if(MeshToAdd->LastSetAdded == 0)
{
MeshToAdd->UvsetInfo = TempUvSetInfo;
MeshToAdd->LastSetAdded =  TempUvSetInfo;
}else
{
	MeshToAdd->LastSetAdded->NextSet = TempUvSetInfo;
	MeshToAdd->LastSetAdded = TempUvSetInfo;
}
//MayaAddIntToBuffer(fn.numUVs(uvsets[j]),DBuffer);
}


//DBuffer.AddData("\n			NumShaders	");
//MayaAddIntToBuffer(Shaders.length(),DBuffer);
MeshToAdd->NumShaders = Shaders.length();
//MessageBox(0,"OK1","OK1",MB_OK);
MeshToAdd->Vertices = new EVertex[vts.length()];
for(int i=0;i!=vts.length();++i) {
	
//DBuffer.AddData("\n			Vertex	");

//MayaAddIntToBuffer(i,DBuffer);
TempMesh->MeshVertices[i].VertexX = (float)vts[i].x;
TempMesh->MeshVertices[i].VertexY = (float)vts[i].y;
TempMesh->MeshVertices[i].VertexZ = (float)vts[i].z;
//---------X-----------
MeshToAdd->Vertices[i].X = (float)vts[i].x;
MeshToAdd->Vertices[i].Y = (float)vts[i].y;
MeshToAdd->Vertices[i].Z = (float)vts[i].z;
//cout << vts[i].x << "\n"; 
//MayaAddFloatToBuffer((float)vts[i].x, DBuffer);
//---------X----------

//---------Y-----------
//MayaAddFloatToBuffer((float)vts[i].y, DBuffer);
//---------Y----------


//---------Z-----------

//cout << vts[i].x << "\n"; 
//MayaAddFloatToBuffer((float)vts[i].z, DBuffer);
//---------Z----------


if(ConnectedToBone)
{
//DBuffer.AddData(" ");
IndexForThisMesh = NamesIndexer.AddBone((char *)fn.name().asChar());
//MayaAddIntToBuffer(MeshCalculatorMemory[IndexForThisMesh].Calcs[i].StartIndex*MaxWeightsCalculator,DBuffer);
MeshToAdd->Vertices[i].StartIndex = MeshCalculatorMemory[IndexForThisMesh].Calcs[i].StartIndex*MaxWeightsCalculator;
//DBuffer.AddData(" ");
MeshToAdd->Vertices[i].Count = MeshCalculatorMemory[IndexForThisMesh].Calcs[i].Count;
//MayaAddIntToBuffer(MeshCalculatorMemory[IndexForThisMesh].Calcs[i].Count,DBuffer);
}
}
NumMeshes++;
// only want non-history items
MeshToAdd->Normals = new ENormal[MeshToAdd->NumNormals];
for(SavedNormalIndex=0;SavedNormalIndex!=nmls.length();++SavedNormalIndex) {

	
	//-------------------------NORMALS

//DBuffer.AddData("\n			Normal	");
//MayaAddIntToBuffer(SavedNormalIndex,DBuffer);
	
//---------X-----------

//cout << vts[i].x << "\n"; 

//MayaAddFloatToBuffer(nmls[SavedNormalIndex].x, DBuffer);
MeshToAdd->Normals[SavedNormalIndex].X = nmls[SavedNormalIndex].x;
//---------X-----------
//--------Y---------


//cout << vts[i].x << "\n"; 
//MayaAddFloatToBuffer(nmls[SavedNormalIndex].y, DBuffer);
MeshToAdd->Normals[SavedNormalIndex].Y = nmls[SavedNormalIndex].y;
//-------Y---------

//--------z---------


//cout << vts[i].x << "\n"; 
MeshToAdd->Normals[SavedNormalIndex].Z = nmls[SavedNormalIndex].z;
//MayaAddFloatToBuffer(nmls[SavedNormalIndex].z, DBuffer);
//-------z---------
	/*
	// print normal
//cout 	<<  <<" "
<< nmls[i].y <<" "
<< nmls[i].z << "\n";
*/
}


// Теперь экспортируем полигоны
PollyToExport_Data * SavedPollys  = new PollyToExport_Data[PolygonCount];

PollyNum = 0;
while(!itPoly.isDone()) {
vc = itPoly.polygonVertexCount();
  itPoly.getNormal(FaceNormal);
 
 // DBuffer.AddData("\n			Normal	");
//MayaAddIntToBuffer(SavedNormalIndex,DBuffer);

MeshToAdd->Normals[SavedNormalIndex].X = FaceNormal.x;
 
MeshToAdd->Normals[SavedNormalIndex].Y = FaceNormal.y;
 
MeshToAdd->Normals[SavedNormalIndex].Z = FaceNormal.z;
//MayaAddFloatToBuffer(FaceNormal.x, DBuffer);
//MayaAddFloatToBuffer(FaceNormal.y, DBuffer);
//MayaAddFloatToBuffer(FaceNormal.z, DBuffer);

SavedPollys[PollyNum].FaceNormalIndex=SavedNormalIndex;
 SavedNormalIndex++;

//DBuffer.AddData("\n			Polly	");
//MayaAddIntToBuffer(PollyNum++,DBuffer);
//DBuffer.AddData("	");
//MayaAddIntToBuffer(vc,DBuffer);// количество вершин в буфере
//SavedPollys[PollyNum++].UVs = new PollyToExport_UVs[vc];
SavedPollys[PollyNum].Data = new PollyToExport[vc];

SavedPollys[PollyNum].VertexCount = vc;


// print all vertex,normal and uv indices
for(int i=0;i<vc;++i) {
  //	DBuffer.AddData("	");
	SavedPollys[PollyNum].Data[i].VertexIndex = itPoly.vertexIndex(i);
	
//MayaAddIntToBuffer(itPoly.vertexIndex(i),DBuffer); // Сохраняем индекс вершины в буфере вершин
// output indices for this vertex
//DBuffer.AddData("	");
//
	SavedPollys[PollyNum].Data[i].NormalIndex = itPoly.normalIndex(i);
//MayaAddIntToBuffer(itPoly.normalIndex(i),DBuffer); // Сохраняем индекс нормали в буфере нормалей
	SavedPollys[PollyNum].Data[i].UVs = new PollyToExport_UVs[SetsLengh];
for(int k=0;k<SetsLengh;++k) {
//DBuffer.AddData("	");
itPoly.getUVIndex(i,uv_index,&uvsets[k]);
//MayaAddIntToBuffer(uv_index,DBuffer); // индекс текстурной координаты
SavedPollys[PollyNum].Data[i].UVs[k].uv_index = uv_index;
}


}

PollyNum++;
itPoly.next();

}


//----------- UVs -----------
MeshToAdd->UVsets = new EUvSet[SetsLengh];
for(int j=0;j!=SetsLengh;++j) {
  	

//output uvset name
UvSetName = (char *)uvsets[j].asChar();

// two arrays for the uv texturecoords
MFloatArray u_coords;
MFloatArray v_coords;

// get the uv data
fn.getUVs(u_coords,v_coords,&uvsets[j]);

// write number of texture coordinate
//cout 	<< "NumUvs "
//<< fn.numUVs(uvsets[j]) << "\n";

MeshToAdd->UVsets[j].count = fn.numUVs(uvsets[j]);
MeshToAdd->UVsets[j].UvS = new oneUV[MeshToAdd->UVsets[j].count];
// write each tex coord
for(int i=0;i!=fn.numUVs(uvsets[j]) ; ++i){
//DBuffer.AddData("\n			Uv	");
//MayaAddIntToBuffer(i,DBuffer);

//DBuffer.AddData("	");
//DBuffer.AddData(UvSetName);


//--------U---------
memcpy(&MeshToAdd->UVsets[j].name[0],UvSetName,GetStringLengh(UvSetName)+1);

//cout << vts[i].x << "\n"; 
//MayaAddFloatToBuffer(u_coords[i], DBuffer);
MeshToAdd->UVsets[j].UvS[i].u = u_coords[i];
//-------U---------
//--------V---------


//cout << vts[i].x << "\n"; 
//MayaAddFloatToBuffer(1.0f-v_coords[i], DBuffer);
MeshToAdd->UVsets[j].UvS[i].v = 1.0f-v_coords[i];
//-------V---------

// print tex coord
//cout 	<< u_coords[i] <<" "
//<< v_coords[i] <<"\n";

}

}
//----------- UVs -----------

//cout << "-------------" << endl;
//cout << "-------------" << endl;
//cout << "-------------" << endl;
//for(int n = 0; n != PolygonCount; n++)
//{
	//cout << n  << " " << SavedPollys[n].VertexCount << endl;
//}
PollyNum = 0;
//cout << (char *)&TempMesh->name;
std::vector< std::vector< int > > FacesByMatID;
FacesByMatID.resize(Shaders.length());
for(int j=0;j < FaceIndices.length();++j)
{
	FacesByMatID[ FaceIndices[j] ].push_back(j);
}

// now write each material and the face indices that use them
				for(int j=0;j < Shaders.length();++j)
				{
					//cout << "\t\t\t"
						//<< GetShaderName( Shaders[j] ).asChar()
						//<< "\n\t\t\t"
						//<< FacesByMatID[j].size()
						//<< "\n\t\t\t\t";
				//	cout<< endl << GetShaderName( Shaders[j] ).asChar() << j << endl;
					std::vector< int >::iterator it = FacesByMatID[j].begin();
					for( ; it != FacesByMatID[j].end(); ++it )
					{
						
							//DBuffer.AddData("\n			Polly	");
							//MayaAddIntToBuffer(PollyNum,DBuffer);
							
							//DBuffer.AddData("	");
										
						//MayaAddIntToBuffer(SavedPollys[*it].VertexCount,DBuffer);// количество вершин в буфере
						

						MeshToAdd->Pollys[PollyNum].VertexCount = SavedPollys[*it].VertexCount;
						
						//DBuffer.AddData("	");
						//MayaAddIntToBuffer(SavedPollys[*it].FaceNormalIndex,DBuffer);// индекс нормали фейса
						
						MeshToAdd->Pollys[PollyNum].FaceNormalIndex = SavedPollys[*it].FaceNormalIndex;
						
						
						//MeshToAdd->Pollys[PollyNum].VertexCount

						MeshToAdd->Pollys[PollyNum].Vertices = new EVertexForPolly[MeshToAdd->Pollys[PollyNum].VertexCount];			
						
						for(int vertc = 0; vertc !=SavedPollys[*it].VertexCount; vertc++)
						{
							MeshToAdd->Pollys[PollyNum].Vertices[vertc].UvIndexes = new UvsIndex[MeshToAdd->NumUVs];
							
							//DBuffer.AddData("	");
							//MayaAddIntToBuffer(SavedPollys[*it].Data[vertc].VertexIndex,DBuffer); // Сохраняем индекс вершины в буфере вершин
							MeshToAdd->Pollys[PollyNum].Vertices[vertc].VertexIndex = SavedPollys[*it].Data[vertc].VertexIndex;
							//DBuffer.AddData("	");
							//MayaAddIntToBuffer(SavedPollys[*it].Data[vertc].NormalIndex,DBuffer); // Сохраняем индекс нормали в буфере нормалей
							MeshToAdd->Pollys[PollyNum].Vertices[vertc].NormalIndex = SavedPollys[*it].Data[vertc].NormalIndex;
							for(int uvset=0; uvset!=SetsLengh;uvset++)
							{
								//DBuffer.AddData("	");
								//MayaAddIntToBuffer(SavedPollys[*it].Data[vertc].UVs[uvset].uv_index,DBuffer); // индекс текстурной координаты
								MeshToAdd->Pollys[PollyNum].Vertices[vertc].UvIndexes[uvset].UvIndex = SavedPollys[*it].Data[vertc].UVs[uvset].uv_index;
							}
						}
					
					PollyNum++;
					}
					
				}
// Завершили экспорт полигонов


//sprintf(tex,"%d %d",i,NumMeshes);

///
//memcpy(&MeshToAdd->Name[0],(char *)&TempMesh->name,GetStringLengh((char *)&TempMesh->name)+1);
MeshToAdd->ShadersUsed = new ShaderInfo[Shaders.length()];
for(int j=0;j < Shaders.length();++j)
{
//DBuffer.AddData("\n			Shader	");

//DBuffer.AddData((char *)GetShaderName( Shaders[j] ).asChar());
memcpy(&MeshToAdd->ShadersUsed[j].name[0],(char *)GetShaderName( Shaders[j] ).asChar(),GetStringLengh((char *)GetShaderName( Shaders[j] ).asChar())+1);
	
//	DBuffer.AddData("	");
//MayaAddIntToBuffer(ShaderNamesIndexer.AddBone((char *)GetShaderName( Shaders[j] ).asChar()),DBuffer);
MeshToAdd->ShadersUsed[j].ShaderIndex = ShaderNamesIndexer.AddBone((char *)GetShaderName( Shaders[j] ).asChar());
//DBuffer.AddData("	");

	//MayaAddIntToBuffer(FacesByMatID[j].size(),DBuffer);
	MeshToAdd->ShadersUsed[j].NumFacesToRender = FacesByMatID[j].size();
}
///MessageBox(0,"4","4",MB_OK);
//DBuffer.AddData("\n	};\n\n");

if(SMesh.FirstMesh == 0)
{
SMesh.FirstMesh = MeshToAdd;
SMesh.LastMeshAdded = MeshToAdd;
SMesh.NumMeshes++;
}else
{
	SMesh.LastMeshAdded->NextMesh = MeshToAdd;
	SMesh.LastMeshAdded = MeshToAdd;
	SMesh.NumMeshes++;
}

GetNumObj(&MeshToAdd->Name[0]);

TempMesh=TempMesh->NextMesh;
}

// get next mesh

it.next();
}

/*
//  Сохраним трансформации мешей



MItDag TransformEnum(MItDag::kDepthFirst, MFn::kTransform); 
bool FoundMesh;
while(!TransformEnum.isDone())
{ 
	FoundMesh = false;
MFnTransform TransItem(TransformEnum.item());
if( !TransItem.isIntermediateObject() ) { 	
//cout<<"Transform "<< fn.name().asChar() <<endl;
TempMesh = &GlobalMesh.Mesh.Mesh;
for(int i =0; i != NumMeshes;i++)
{
	if(CompareStringMaya(&TempMesh->name[0], (char *)TransItem.name().asChar(),GetStringLengh(&TempMesh->name[0]),GetStringLengh((char *)TransItem.name().asChar())))
	{
		FoundMesh = true;
			break;
	}
	TempMesh = TempMesh->NextMesh;
}
if(FoundMesh)
{

//cout << TempMesh->name << endl;
MMatrix mat = TransItem.transformation().asMatrix();
MQuaternion Rotation(0,0,0,1);
TransItem.getRotation(Rotation);

TempMesh->MeshMatrix.m[0] = mat.matrix[0][0];
TempMesh->MeshMatrix.m[1] = mat.matrix[0][1];
TempMesh->MeshMatrix.m[2] = mat.matrix[0][2];
TempMesh->MeshMatrix.m[3] = mat.matrix[0][3];

TempMesh->MeshMatrix.m[4] = mat.matrix[1][0];
TempMesh->MeshMatrix.m[5] = mat.matrix[1][1];
TempMesh->MeshMatrix.m[6] = mat.matrix[1][2];
TempMesh->MeshMatrix.m[7] = mat.matrix[1][3];

TempMesh->MeshMatrix.m[8] = mat.matrix[2][0];
TempMesh->MeshMatrix.m[9] = mat.matrix[2][1];
TempMesh->MeshMatrix.m[10] = mat.matrix[2][2];
TempMesh->MeshMatrix.m[11] = mat.matrix[2][3];

TempMesh->MeshMatrix.m[12] = mat.matrix[3][0];
TempMesh->MeshMatrix.m[13] = mat.matrix[3][1];
TempMesh->MeshMatrix.m[14] = mat.matrix[3][2];
TempMesh->MeshMatrix.m[15] = mat.matrix[3][3];




TempMesh->MeshPos.x =	mat.matrix[3][0];
TempMesh->MeshPos.y =	mat.matrix[3][1];
TempMesh->MeshPos.z =	mat.matrix[3][2];
TempMesh->MeshOrientation.x = Rotation.x;
TempMesh->MeshOrientation.y = Rotation.y;
TempMesh->MeshOrientation.z = Rotation.z;
TempMesh->MeshOrientation.w = Rotation.w;

}
}
TransformEnum.next(); 

}




*/


//cout << "\n MeshPos " << GlobalMesh.Mesh.Mesh.MeshPos.x << GlobalMesh.Mesh.Mesh.MeshPos.y << GlobalMesh.Mesh.Mesh.MeshPos.z;

// Теперь материалы



/// Сначало пройдемся по всем материалам для того чтоб их посчитать

///////////////////////////////////////////

int NumShaders = 0;

MItDependencyNodes itDepForCount(MFn::kLambert);
while (!itDepForCount.isDone())
{
  	switch(itDepForCount.item().apiType()) {
// if found phong shader
case MFn::kPhong:
{
NumShaders++;
}
break;
case MFn::kLambert:
{
NumShaders++;
}
break;
case MFn::kPhongExplorer:
{
NumShaders++;
}
break;
// if found blinn shader
case MFn::kBlinn:
{
NumShaders++;
}
break;
}
itDepForCount.next();
}
//////////////////////////////////////////
ExShaer.NumShaders = NumShaders;
ExShaer.Shaders = new OneExportShader[NumShaders];
if(NumShaders) //Если есть шейдера...
{
//DBuffer.AddData("Shaders	");
//MayaAddIntToBuffer(NumShaders,DBuffer);
//DBuffer.AddData("{");

//LambertStart

MItDependencyNodes itDep(MFn::kLambert);
while (!itDep.isDone())
{
  	switch(itDep.item().apiType()) {
  	// if found phong shader
		
//case MFn::kPhong:
//{
//MFnPhongShader fn( itDep.item() );
//DBuffer.AddData("\n			Phong	");
//DBuffer.AddData((char *)fn.name().asChar());
//DBuffer.AddData("	");
//OutputColor(fn,"color",DBuffer);
/*

..OutputColor(fn,"ambientColor");

OutputColor(fn,"specularColor");
OutputColor(fn,"incandescence");
OutputColor(fn,"transparency");
//cout<<"\tcos "<<fn.cosPower()<< endl;
OutputBumpMap(itDep.item());
OutputEnvMap(itDep.item());
//*/
//}
//break;

// if found lambert shader
case MFn::kLambert:
{
MFnLambertShader fn( itDep.item() );
//DBuffer.AddData("\n			Lambert	");
unsigned int ShaderIndex =ShaderNamesIndexer.AddBone((char *)fn.name().asChar());
//MayaAddIntToBuffer(ShaderNamesIndexer.AddBone((char *)fn.name().asChar()),DBuffer);

//DBuffer.AddData("	");
memcpy(&ExShaer.Shaders[ShaderIndex].Name[0],(char *)fn.name().asChar(),GetStringLengh((char *)fn.name().asChar())+1);
memcpy(&ExShaer.Shaders[ShaderIndex].Type[0],"Lambert",GetStringLengh("Lambert")+1);
//DBuffer.AddData((char *)fn.name().asChar());
//DBuffer.AddData("	");
OutputColor(fn,"color",DBuffer, &ExShaer.Shaders[ShaderIndex]);
/*
//cout<<"Lambert "<<fn.name().asChar()<<"\n";
OutputColor(fn,"ambientColor");
OutputColor(fn,"color");
OutputColor(fn,"incandescence");
OutputColor(fn,"transparency");
OutputBumpMap(itDep.item());
OutputEnvMap(itDep.item());
*/
}
break;
// if found blinn shader
//case MFn::kBlinn:
//{
//MFnBlinnShader fn( itDep.item() );
//DBuffer.AddData("\n			Blinn	");
//DBuffer.AddData((char *)fn.name().asChar());
//DBuffer.AddData("	");
//OutputColor(fn,"color",DBuffer);
/*
//cout<<"Blinn "<<fn.name().asChar()<<"\n";
OutputColor(fn,"ambientColor");
OutputColor(fn,"color");
OutputColor(fn,"specularColor");
OutputColor(fn,"incandescence");
OutputColor(fn,"transparency");
//cout 	<<"\teccentricity "
<<fn.eccentricity()<< endl;
//cout 	<< "\tspecularRollOff "
<< fn.specularRollOff()<< endl;
OutputBumpMap(itDep.item());
OutputEnvMap(itDep.item());
*/
//}
//break;


}
		
itDep.next();
}

//LambertEnd


//PhongEStart

MItDependencyNodes itDepPhongE(MFn::kPhongExplorer);
while (!itDepPhongE.isDone())
{
  	switch(itDepPhongE.item().apiType()) {
  	// if found phong shader
case MFn::kPhongExplorer:
{
MFnPhongEShader fnPhongE( itDepPhongE.item() );
//DBuffer.AddData("\n			PhongE	");
//MayaAddIntToBuffer(ShaderNamesIndexer.AddBone((char *)fnPhongE.name().asChar()),DBuffer);
unsigned int ShaderIndex =ShaderNamesIndexer.AddBone((char *)fnPhongE.name().asChar());
//DBuffer.AddData("	");
//DBuffer.AddData((char *)fnPhongE.name().asChar());
//DBuffer.AddData("	");
memcpy(&ExShaer.Shaders[ShaderIndex].Type[0],"PhongE",GetStringLengh("PhongE")+1);
memcpy(&ExShaer.Shaders[ShaderIndex].Name[0],(char *)fnPhongE.name().asChar(),GetStringLengh((char *)fnPhongE.name().asChar())+1);
OutputColor(fnPhongE,"color",DBuffer,&ExShaer.Shaders[ShaderIndex]);
/*

..OutputColor(fnPhongE,"ambientColor");

OutputColor(fnPhongE,"specularColor");
OutputColor(fnPhongE,"incandescence");
OutputColor(fnPhongE,"transparency");
//cout<<"\tcos "<<fnPhongE.cosPower()<< endl;
OutputBumpMap(itDepPhongE.item());
OutputEnvMap(itDepPhongE.item());
*/
}
break;
	}
		
itDepPhongE.next();
}

//PhongEEnd



//DBuffer.AddData("\n	};\n\n");
}


GlobalMesh.Mesh.NumHierarhy =  SaveBonesOnlyForCalculation();
if(GlobalMesh.Mesh.NumHierarhy)
{
//DBuffer.AddData("Hierarhy ");
	BHierarhy.Hy = new ExHierarhy[GlobalMesh.Mesh.NumHierarhy];
	BHierarhy.NumBones = GlobalMesh.Mesh.NumHierarhy;
//MayaAddIntToBuffer(GlobalMesh.Mesh.NumHierarhy,DBuffer);
GlobalMesh.Mesh.Hierarhy = new BoneHierarhy[GlobalMesh.Mesh.NumHierarhy];
								 
//DBuffer.AddData("{");

SaveBones(DBuffer,BonIndexer, &BHierarhy.Hy[0]);
//DBuffer.AddData("\n	};\n\n");


DoublesSorter Times; // Сюда сохраним время для фреймов.
SaveFrameTimes(Times);

if(!Times.counter)
{
	GlobalMesh.Mesh.NumFrames = 1;
	GlobalMesh.Mesh.Frames = new Bone[GlobalMesh.Mesh.NumHierarhy];
	SaveDefaultFrame( DBuffer,BonIndexer);  // Если нет фреймов  сохраним дефаултный фрейм


}
else
{
	GlobalMesh.Mesh.Frames = new Bone[GlobalMesh.Mesh.NumHierarhy*Times.counter];
	ExFrames.NumFrames = Times.counter;
	ExFrames.ExFrames = new OneExportFrame[Times.counter];
	for (int i = 0; i != Times.counter; i++)
	{	ExFrames.ExFrames[i].FrameNum = i;
	SaveFrame(i, DBuffer, Times,BonIndexer, &ExFrames.ExFrames[i]);
}
}





// GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientX

ConvertJointsFromLocalToWorld();// Переведем первый сохраненный фрейм из локальных в мировые.

if(WeightsCalculator != 0) //Если у нас есть веса то экспортируем их
{
	//if(MaxWeightsCalculator == 1000000) MessageBox(0,"Export error: Invalid file format: Invalid weights per vertex. Exported file is corrupted","Error while exporting",MB_OK);

int MeshesAddedCounter =0;
bool WasReset;

//DBuffer.AddData("Weights	");
//MayaAddIntToBuffer(NumVertexes,DBuffer);//WeightsCalculator
WE.NumWeights = NumVertexes;
//DBuffer.AddData(" ");
//MayaAddIntToBuffer(MaxWeightsCalculator,DBuffer);
WE.WeightsPerVertex = MaxWeightsCalculator;
//DBuffer.AddData("{");

int count=0;
 //MItDependencyNodes iter( MFn::kSkinClusterFilter );
//for ( ; !iter.isDone(); iter.next() ) {

//NumMeshesCalculated - скольковсего мешей



WE.LastWeightAdded = NULL;
WE.Weights = NULL;
while(MeshesAddedCounter!=ConnStr.NumUnits)
{
WasReset = false;
MObject object = ReturnObjectToAdd(NamesIndexer,MeshesAddedCounter,WasReset);
if(WasReset)
OutputSkinCluster(object,DBuffer, BonIndexer, NumMeshes);
count++;
}
}
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
OneMesh * TempExMesh = SMesh.FirstMesh;
for(int i = 0; i!=SMesh.NumMeshes; i++)
{
SaveMesh(TempExMesh);
TempExMesh = TempExMesh->NextMesh;
}

/*
//////////////
//////////////
//////////////
if(ExShaer.NumShaders)
{
DBuffer.AddData("Shaders	");
MayaAddIntToBuffer(ExShaer.NumShaders,DBuffer);
DBuffer.AddData("{");
for(int j=0;j!=ExShaer.NumShaders ; j++)
{
DBuffer.AddData("\n			");
DBuffer.AddData(&ExShaer.Shaders[j].Type[0]);
DBuffer.AddData(" ");
MayaAddIntToBuffer(j,DBuffer);
DBuffer.AddData("	");
DBuffer.AddData(&ExShaer.Shaders[j].Name[0]);
DBuffer.AddData("	");
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorR,DBuffer);
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorG,DBuffer);
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorB,DBuffer);
DBuffer.AddData("	 ");
DBuffer.AddData(&ExShaer.Shaders[j].TextureName[0]);
}
DBuffer.AddData("\n	};\n\n");
}

/////////////////
/////////////////
/////////////////
DBuffer.AddData("Hierarhy ");
MayaAddIntToBuffer(BHierarhy.NumBones,DBuffer);
DBuffer.AddData("{");
for(int j=0;j!=BHierarhy.NumBones ; j++)
{
	
DBuffer.AddData("\n			Bone	");
DBuffer.AddData(&BHierarhy.Hy[j].BoneName[0]);
DBuffer.AddData("	");
MayaAddIntToBuffer(BHierarhy.Hy[j].BoneIndex,DBuffer);
DBuffer.AddData("	");
DBuffer.AddData(&BHierarhy.Hy[j].ParentName[0]);
if(BHierarhy.Hy[j].Parrent)
{
DBuffer.AddData(" ");
MayaAddIntToBuffer(BHierarhy.Hy[j].ParentIndex,DBuffer);
}
}

DBuffer.AddData("\n	};\n\n");
/////////////////
/////////////////
/////////////////
for(int j=0;j!=ExFrames.NumFrames; j++)
{
	DBuffer.AddData("Frame	");
	if(!j)
	{ // в первом фрейме добавим количество фреймов
		MayaAddIntToBuffer(ExFrames.NumFrames,DBuffer);
	}
	DBuffer.AddData("{");
	DBuffer.AddData("\n			FrameNum	");
	MayaAddIntToBuffer(ExFrames.ExFrames[j].FrameNum,DBuffer);
	DBuffer.AddData("\n			FrameTime	");
	MayaAddIntToBuffer(ExFrames.ExFrames[j].FrameTime,DBuffer);

	for(int k=0;k!=BHierarhy.NumBones; k++)
	{
		DBuffer.AddData("\n			Bone	");
		DBuffer.AddData(&ExFrames.ExFrames[j].Bones[k].Name[0]);
		DBuffer.AddData("	");
		MayaAddIntToBuffer(k,DBuffer);
		DBuffer.AddData("	");
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].PosX, DBuffer);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].PosY, DBuffer);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].PosZ, DBuffer);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatX, DBuffer);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatY, DBuffer);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatZ, DBuffer);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatW, DBuffer);
	}


	DBuffer.AddData("\n	};\n\n");
}


///////////////////////
////////////////////////
////////////////////////
////////////////////////
char text[255];

DBuffer.AddData("Weights	");
MayaAddIntToBuffer(WE.NumWeights,DBuffer);//WeightsCalculator
DBuffer.AddData(" ");
MayaAddIntToBuffer(WE.WeightsPerVertex,DBuffer);
DBuffer.AddData("{");
OneWeight * TempWeight; 
TempWeight = WE.Weights;
while(TempWeight)
{

	
DBuffer.AddData("\n			Weight	");
MayaAddIntToBuffer(TempWeight->Count,DBuffer);
DBuffer.AddData("	");
DBuffer.AddData(&TempWeight->InfluencedMeshName[0]);
MayaAddIntToBuffer(TempWeight->idx,DBuffer);
DBuffer.AddData("	");
DBuffer.AddData(&TempWeight->JointName[0]);
DBuffer.AddData("	");
MayaAddIntToBuffer(TempWeight->BoneIndex,DBuffer);
MayaAddFloatToBuffer(TempWeight->Weight, DBuffer);
DBuffer.AddData("	");
MayaAddFloatToBuffer(TempWeight->PosX, DBuffer);
DBuffer.AddData("	");
MayaAddFloatToBuffer(TempWeight->PosY, DBuffer);
DBuffer.AddData("	");
MayaAddFloatToBuffer(TempWeight->PosZ, DBuffer);

TempWeight = TempWeight->NextWeight;
}
DBuffer.AddData("\n	};\n\n");
*/

//delete [] BonMem;


cout << "---------------------\n";
cout << "---------------------\n";
cout << "---------------------\n";
cout << "---------------------\n";
cout << "---------------------\n";
cout << "---------------------\n";
cout << "----------START PARRENTS--------\n";
GetMeshMatrix("BOX10");
/*
unsigned int NumParrents = SP.GetNumChilds(&TempExMesh->Name[0]);
char * ChName;
MayaAddIntToBuffer(NumChildren,DBuffer);
for(int z = 0; z!= NumChildren; z++)
	{
		DBuffer.AddData("\n				");
		ChName = PT.GetChildNum(&TempExMesh->Name[0], z);
		DBuffer.AddData(ChName);
		DBuffer.AddData("	");
		MayaAddIntToBuffer(GetBOXIndex(ChName) ,DBuffer);
	}
}
*/
DBuffer.PrintAllUnitsToFile("c:\\Data.txt");


//MessageBox(0,"Pollyok","PollyOK",MB_OK);

for (int i = 0;i!=NumMeshesCalculated;i++)
{
delete []	MeshCalculatorMemory[i].Calcs;
}
delete [] MeshCalculatorMemory;

	return status;
}

unsigned int GetNumObj(char * ObjName)
{
int i;
unsigned int NumObj = 0;
OneMesh * TempExMesh = SMesh.FirstMesh;
for(i = 0; i!=SMesh.NumMeshes; i++)
	{
		if(!strcmp(&TempExMesh->Name[0], ObjName))
		{
			NumObj++;
		}
			
		TempExMesh = TempExMesh->NextMesh;
}

if(NumObj>1)
{
		char ErrorMessage[255];
		sprintf(&ErrorMessage[0], " There are %d  objects with save name \"%s\" " ,NumObj, ObjName );
		MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
		return NumObj;
}
		return NumObj;
}

void CheckSceneRoot()
{
	int i;
unsigned int NumRoots = 0;
bool result;
int foundR =0;
OneMesh * TempExMesh = SMesh.FirstMesh;
for(i = 0; i!=SMesh.NumMeshes; i++)
	{
		if(!strcmp(&TempExMesh->Name[0], "SceneRoot"))
		{
			foundR++;
		}
			GetParrent(&TempExMesh->Name[0], result);
			if(!result) 
			{
				if(strcmp(&TempExMesh->Name[0], "SceneRoot"))
				{
					NumRoots++;
				}
			}
		TempExMesh = TempExMesh->NextMesh;
}
if(!foundR)
{
MessageBoxA(0, "Could not found SceneRoot", "", MB_OK);
return;
}
if(foundR>1)
{
MessageBoxA(0, "More than 1 SceneRoot", "", MB_OK);
return;
}
if(NumRoots)
{
MessageBoxA(0, "There are objects that have not connected to SceneRoot", "", MB_OK);
return;
}
		

}





float * GetMeshAABB(char * MeshName)
{
static float AABB[6]; // AABB[0][1][2] - min AABB[3][4][5] - max
int i, j;
char ErrorMessage[255];
OneMesh * TempExMesh = SMesh.FirstMesh;
for(i = 0; i!=SMesh.NumMeshes; i++)
	{

		if(!strcmp(&TempExMesh->Name[0], MeshName))
		{
			
			for(j = 0; j!=TempExMesh->NumVertices; j++)
			{
				if(!j)
				{
					AABB[0] = TempExMesh->Vertices[j].X;
					AABB[1] = TempExMesh->Vertices[j].Y;
					AABB[2] = TempExMesh->Vertices[j].Z;
					
					AABB[3] = TempExMesh->Vertices[j].X;
					AABB[4] = TempExMesh->Vertices[j].Y;
					AABB[5] = TempExMesh->Vertices[j].Z;
				}

				if(TempExMesh->Vertices[j].X < AABB[0]) AABB[0] = TempExMesh->Vertices[j].X;
				if(TempExMesh->Vertices[j].Y < AABB[1]) AABB[1] = TempExMesh->Vertices[j].Y;
				if(TempExMesh->Vertices[j].Z < AABB[2]) AABB[2] = TempExMesh->Vertices[j].Z;

				if(TempExMesh->Vertices[j].X > AABB[3]) AABB[3] = TempExMesh->Vertices[j].X;
				if(TempExMesh->Vertices[j].Y > AABB[4]) AABB[4] = TempExMesh->Vertices[j].Y;
				if(TempExMesh->Vertices[j].Z > AABB[5]) AABB[5] = TempExMesh->Vertices[j].Z;
			}

		Matrix MatA = *(Matrix *)GetMeshMatrix(MeshName);
		Vector Vec;
		Vec = MatrixMultiplyVector(MatA, *(Vector *)&AABB[0]);
		AABB[0] = Vec.x;
		AABB[1] = Vec.y;
		AABB[2] = Vec.z;

		Vec = MatrixMultiplyVector(MatA, *(Vector *)&AABB[3]);
		AABB[3] = Vec.x;
		AABB[4] = Vec.y;
		AABB[5] = Vec.z;

		return &AABB[0];
		}
		TempExMesh = TempExMesh->NextMesh;
	}

		
		sprintf(&ErrorMessage[0], " Unable to get AABB for \"%s\"" , MeshName);
		MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
		return 0;
}


char * GetParrent(char * MeshName, bool & result)
{
char * Name;
char * ParrentName;
unsigned int NUME;
unsigned int NUMP;
NUME = SP.GetNumElements();
for(int i = 0; i!= NUME; i++)
{
	Name = SP.GetElementNum(i);
	if(!strcmp(MeshName, Name))
	{
		NUMP = SP.GetNumChilds(Name);
		if(NUMP)
		{
			if(NUMP > 1)
			{
					char ErrorMessage[255];
					sprintf(&ErrorMessage[0], "Mesh \"%s\" has more than 1 parrent." , MeshName);
					MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
					return 0;
			}
			for(int z = 0; z!= NUMP; z++)
			{
				ParrentName = SP.GetChildNum(Name, z);
				result = true;
				return ParrentName;
			}
		}else
		{// У этого элемента нет Родителей
			result = false;
			return 0;
		}
	}
}
result = false;
	return 0;
}

float * MeshMat(char * MeshName)
{
OneMesh * TempExMesh = SMesh.FirstMesh;
for(int i = 0; i!=SMesh.NumMeshes; i++)
	{
		if(!strcmp(&TempExMesh->Name[0], MeshName))
		{
			return &TempExMesh->Matrix[0];
		}
		TempExMesh = TempExMesh->NextMesh;
	}

		char ErrorMessage[255];
		sprintf(&ErrorMessage[0], " Unable to get mesh matrix for \"%s\"" , MeshName);
		MessageBoxA(0, &ErrorMessage[0], "Error while exporting", MB_OK);
		return 0;

}

bool MeshHasParrents(char * MeshName)
{
bool result = false;
GetParrent(MeshName, result);
return result;
}

float * GetMeshMatrix(char * MeshName)
{
	static Matrix MatA;
Matrix MatB;
bool result = true;
char * CheckMeshName = MeshName;
unsigned int NUMP;
TREE.AddElement("MAIN");
while(result)
{
	CheckMeshName = GetParrent(CheckMeshName, result);
	if(result) 
	{

		TREE.AddChild("MAIN", CheckMeshName);
	}
}
NUMP = TREE.GetNumChilds("MAIN");
 MatrixLoadIdentity(&MatA);
		if(NUMP)
		{

			
			for(int z = NUMP-1; z!= -1; z--)
			{
				MatB = *(Matrix *)MeshMat( TREE.GetChildNum("MAIN", z) );
				MatrixMultiply(&MatA, MatB);
				

			
			}
			MatB = *(Matrix *)MeshMat( MeshName );
			MatrixMultiply(&MatA, MatB);
		}

TREE.DeleteAllElements();

return &MatA.m[0];
}

 void SaveMesh(OneMesh * MeshToSave)
 {bool ConnectedToBone;
 char FileNameToSave[255];
 uvset * TempUvSetInfo;
	StringDynamicBuffer BufferToSave;
	StringDynamicBuffer BufferToSaveForAnimation;
	OneMesh * TempExMesh = SMesh.FirstMesh;

BufferToSave.AddData("Mesh	{\n");
BufferToSave.AddData("			Name		");
BufferToSave.AddData(&MeshToSave->Name[0]);
if(MeshToSave->ConnectedToBone)
{
ConnectedToBone = true;
BufferToSave.AddData("\n			ConnectedToBone	Yes");
}else
{
ConnectedToBone = false;
BufferToSave.AddData("\n			ConnectedToBone	No");
BufferToSave.AddData("\n			MeshMatrix	");

MayaAddFloatToBuffer(MeshToSave->Matrix[0], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[1], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[2], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[3], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[4], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[5], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[6], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[7], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[8], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[9], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[10], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[11], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[12], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[13], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[14], BufferToSave);
BufferToSave.AddData(" ");

MayaAddFloatToBuffer(MeshToSave->Matrix[15], BufferToSave);
BufferToSave.AddData(" ");
}
BufferToSave.AddData("\n			NumInstances	");
MayaAddIntToBuffer(MeshToSave->NumInstances,BufferToSave);

BufferToSave.AddData("\n			NumPollys	");
MayaAddIntToBuffer(MeshToSave->PolygonCount,BufferToSave);
SavedMeshHas3AnglePollys = false;
BufferToSave.AddData("\n			Num3AnglePollys	");
MayaAddIntToBuffer(MeshToSave->Num3AnglePollys,BufferToSave);
if(MeshToSave->Num3AnglePollys) SavedMeshHas3AnglePollys = true;
SavedMeshHas4AnglePollys = false;
BufferToSave.AddData("\n			Num4AnglePollys	");
MayaAddIntToBuffer(MeshToSave->Num4AnglePollys,BufferToSave);
if(MeshToSave->Num4AnglePollys) SavedMeshHas4AnglePollys = true;
BufferToSave.AddData("\n			NumVertices	");
MayaAddIntToBuffer(MeshToSave->NumVertices,BufferToSave);

BufferToSave.AddData("\n			NumNormals	");
MayaAddIntToBuffer(MeshToSave->NumNormals,BufferToSave);

BufferToSave.AddData("\n			NumUVs	");
MayaAddIntToBuffer(MeshToSave->NumUVs,BufferToSave);
TempUvSetInfo = MeshToSave->UvsetInfo;
for(int j=0;j!=MeshToSave->NumUVs;j++) {
BufferToSave.AddData("\n			UVset	");
BufferToSave.AddData(&TempUvSetInfo->Name[0]);
BufferToSave.AddData("	");
MayaAddIntToBuffer(TempUvSetInfo->NumUVs,BufferToSave);
TempUvSetInfo = TempUvSetInfo->NextSet;
}
BufferToSave.AddData("\n			NumShaders	");
MayaAddIntToBuffer(MeshToSave->NumShaders,BufferToSave);
for(int j=0;j!=MeshToSave->NumVertices;++j) {
	BufferToSave.AddData("\n			Vertex	");
	MayaAddIntToBuffer(j,BufferToSave);
	MayaAddFloatToBuffer(MeshToSave->Vertices[j].X, BufferToSave);
	MayaAddFloatToBuffer(MeshToSave->Vertices[j].Y, BufferToSave);
	MayaAddFloatToBuffer(MeshToSave->Vertices[j].Z, BufferToSave);
	if(ConnectedToBone)
	{
BufferToSave.AddData(" ");
MayaAddIntToBuffer(MeshToSave->Vertices[j].StartIndex,BufferToSave);
BufferToSave.AddData(" ");
MayaAddIntToBuffer(MeshToSave->Vertices[j].Count,BufferToSave);
}


}

for(int j=0;j!=MeshToSave->NumNormals;j++) {
BufferToSave.AddData("\n			Normal	");
MayaAddIntToBuffer(j,BufferToSave);

MayaAddFloatToBuffer(MeshToSave->Normals[j].X, BufferToSave);
MayaAddFloatToBuffer(MeshToSave->Normals[j].Y, BufferToSave);
MayaAddFloatToBuffer(MeshToSave->Normals[j].Z, BufferToSave);
}


for(int j=0;j!=MeshToSave->NumUVs ; j++)
{
	
	for(int k=0;k!=MeshToSave->UVsets[j].count ; k++)
	{
		BufferToSave.AddData("\n			Uv	");
		MayaAddIntToBuffer(k,BufferToSave);
		BufferToSave.AddData("	");
		BufferToSave.AddData(&MeshToSave->UVsets[j].name[0]);
		MayaAddFloatToBuffer(MeshToSave->UVsets[j].UvS[k].u,BufferToSave);
		MayaAddFloatToBuffer(MeshToSave->UVsets[j].UvS[k].v,BufferToSave);
	}
}

for(int j=0;j!=MeshToSave->PolygonCount ; j++)
{
BufferToSave.AddData("\n			Polly	");
MayaAddIntToBuffer(j,BufferToSave);
BufferToSave.AddData("	");
MayaAddIntToBuffer(MeshToSave->Pollys[j].VertexCount,BufferToSave);
BufferToSave.AddData("	");
MayaAddIntToBuffer(MeshToSave->Pollys[j].FaceNormalIndex,BufferToSave);
for(int k=0;k!=MeshToSave->Pollys[j].VertexCount ; k++)
	{
		BufferToSave.AddData("	");
		MayaAddIntToBuffer(MeshToSave->Pollys[j].Vertices[k].VertexIndex,BufferToSave);
		BufferToSave.AddData("	");
		MayaAddIntToBuffer(MeshToSave->Pollys[j].Vertices[k].NormalIndex,BufferToSave);
		for(int l=0;l!=MeshToSave->NumUVs ; l++)
		{
				BufferToSave.AddData("	");
				MayaAddIntToBuffer(MeshToSave->Pollys[j].Vertices[k].UvIndexes[l].UvIndex, BufferToSave );
		}
	}
}


for(int j=0;j!=MeshToSave->NumShaders ; j++)
{
BufferToSave.AddData("\n			Shader	");
BufferToSave.AddData(&MeshToSave->ShadersUsed[j].name[0]);
BufferToSave.AddData("	");
MayaAddIntToBuffer(MeshToSave->ShadersUsed[j].ShaderIndex,BufferToSave);
BufferToSave.AddData("	");
MayaAddIntToBuffer(MeshToSave->ShadersUsed[j].NumFacesToRender,BufferToSave);
}

BufferToSave.AddData("\n	};\n\n");


//////////////
//////////////
//////////////
/*
if(ExShaer.NumShaders)
{
BufferToSave.AddData("Shaders	");
MayaAddIntToBuffer(ExShaer.NumShaders,BufferToSave);
BufferToSave.AddData("{");
for(int j=0;j!=ExShaer.NumShaders ; j++)
{
BufferToSave.AddData("\n			");
BufferToSave.AddData(&ExShaer.Shaders[j].Type[0]);
BufferToSave.AddData(" ");
MayaAddIntToBuffer(j,BufferToSave);
BufferToSave.AddData("	");
BufferToSave.AddData(&ExShaer.Shaders[j].Name[0]);
BufferToSave.AddData("	");
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorR,BufferToSave);
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorG,BufferToSave);
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorB,BufferToSave);
BufferToSave.AddData("	 ");
BufferToSave.AddData(&ExShaer.Shaders[j].TextureName[0]);
}
BufferToSave.AddData("\n	};\n\n");
}
*/
/////////////////
/////////////////
/////////////////
BufferToSaveForAnimation.AddData("Hierarhy ");
MayaAddIntToBuffer(BHierarhy.NumBones,BufferToSaveForAnimation);
BufferToSaveForAnimation.AddData("{");
for(int j=0;j!=BHierarhy.NumBones ; j++)
{
	
BufferToSaveForAnimation.AddData("\n			Bone	");
BufferToSaveForAnimation.AddData(&BHierarhy.Hy[j].BoneName[0]);
BufferToSaveForAnimation.AddData("	");
MayaAddIntToBuffer(BHierarhy.Hy[j].BoneIndex,BufferToSaveForAnimation);
BufferToSaveForAnimation.AddData("	");
BufferToSaveForAnimation.AddData(&BHierarhy.Hy[j].ParentName[0]);
if(BHierarhy.Hy[j].Parrent)
{
BufferToSaveForAnimation.AddData(" ");
MayaAddIntToBuffer(BHierarhy.Hy[j].ParentIndex,BufferToSaveForAnimation);
}
}

BufferToSaveForAnimation.AddData("\n	};\n\n");
/////////////////
/////////////////
/////////////////
for(int j=0;j!=ExFrames.NumFrames; j++)
{
	BufferToSaveForAnimation.AddData("Frame	");
	if(!j)
	{ // в первом фрейме добавим количество фреймов
		MayaAddIntToBuffer(ExFrames.NumFrames,BufferToSaveForAnimation);
	}
	BufferToSaveForAnimation.AddData("{");
	BufferToSaveForAnimation.AddData("\n			FrameNum	");
	MayaAddIntToBuffer(ExFrames.ExFrames[j].FrameNum,BufferToSaveForAnimation);
	BufferToSaveForAnimation.AddData("\n			FrameTime	");
	MayaAddIntToBuffer(ExFrames.ExFrames[j].FrameTime,BufferToSaveForAnimation);

	for(int k=0;k!=BHierarhy.NumBones; k++)
	{
		BufferToSaveForAnimation.AddData("\n			Bone	");
		BufferToSaveForAnimation.AddData(&ExFrames.ExFrames[j].Bones[k].Name[0]);
		BufferToSaveForAnimation.AddData("	");
		MayaAddIntToBuffer(k,BufferToSaveForAnimation);
		BufferToSaveForAnimation.AddData("	");
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].PosX, BufferToSaveForAnimation);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].PosY, BufferToSaveForAnimation);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].PosZ, BufferToSaveForAnimation);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatX, BufferToSaveForAnimation);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatY, BufferToSaveForAnimation);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatZ, BufferToSaveForAnimation);
		MayaAddFloatToBuffer(ExFrames.ExFrames[j].Bones[k].QuatW, BufferToSaveForAnimation);
	}


	BufferToSaveForAnimation.AddData("\n	};\n\n");
}








char text[255];
if(WE.NumWeights)
{
BufferToSave.AddData("Weights	");
MayaAddIntToBuffer(WE.NumWeights,BufferToSave);//WeightsCalculator
BufferToSave.AddData(" ");
MayaAddIntToBuffer(WE.WeightsPerVertex,BufferToSave);
BufferToSave.AddData("{");
OneWeight * TempWeight; 
TempWeight = WE.Weights;
while(TempWeight)
{

	
BufferToSave.AddData("\n			Weight	");
MayaAddIntToBuffer(TempWeight->Count,BufferToSave);
BufferToSave.AddData("	");
//BufferToSave.AddData(&TempWeight->InfluencedMeshName[0]);
MayaAddIntToBuffer(TempWeight->idx,BufferToSave);
BufferToSave.AddData("	");
BufferToSave.AddData(&TempWeight->JointName[0]);
BufferToSave.AddData("	");
MayaAddIntToBuffer(TempWeight->BoneIndex,BufferToSave);
MayaAddFloatToBuffer(TempWeight->Weight, BufferToSave);
BufferToSave.AddData("	");
MayaAddFloatToBuffer(TempWeight->PosX, BufferToSave);
BufferToSave.AddData("	");
MayaAddFloatToBuffer(TempWeight->PosY, BufferToSave);
BufferToSave.AddData("	");
MayaAddFloatToBuffer(TempWeight->PosZ, BufferToSave);

TempWeight = TempWeight->NextWeight;
}
BufferToSave.AddData("\n	};\n\n");
}


//delete [] BonMem;

sprintf(&FileNameToSave[0], "c:\\Export\\%s.t1m", &MeshToSave->MeshFileName[0]);
BufferToSave.PrintAllUnitsToFile(&FileNameToSave[0]);
sprintf(&FileNameToSave[0], "c:\\Export\\%s.t1a", &MeshToSave->MeshFileName[0]);
BufferToSaveForAnimation.PrintAllUnitsToFile(&FileNameToSave[0]);
 }
void SaveShaders()
 {
	StringDynamicBuffer BufferToSave;

if(ExShaer.NumShaders)
{
BufferToSave.AddData("Shaders	");
MayaAddIntToBuffer(ExShaer.NumShaders,BufferToSave);
BufferToSave.AddData("{");
for(int j=0;j!=ExShaer.NumShaders ; j++)
{
BufferToSave.AddData("\n			");
BufferToSave.AddData(&ExShaer.Shaders[j].Type[0]);
BufferToSave.AddData(" ");
MayaAddIntToBuffer(j,BufferToSave);
BufferToSave.AddData("	");
BufferToSave.AddData(&ExShaer.Shaders[j].Name[0]);
BufferToSave.AddData("	");
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorR,BufferToSave);
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorG,BufferToSave);
MayaAddFloatToBuffer(ExShaer.Shaders[j].colorB,BufferToSave);
BufferToSave.AddData("	 ");
BufferToSave.AddData(&ExShaer.Shaders[j].TextureName[0]);
}
BufferToSave.AddData("\n	};\n\n");
}

//delete [] BonMem;
char FileNameToSave[255];
sprintf(&FileNameToSave[0], "c:\\Export\\%s.t1s", "Shaders");
BufferToSave.PrintAllUnitsToFile(&FileNameToSave[0]);
 }

unsigned int GetBOXIndex(char * BoxName)
{
	char ErrorMessage[255];
OneMesh * TempExMesh = SMesh.FirstMesh;
	for(int i = 0; i!=SMesh.NumMeshes; i++)
	{

		if(!strcmp(&TempExMesh->Name[0], BoxName))
		{
			return BBOXIndexer.AddBone(&TempExMesh->MeshFileName[0]);
		}
		TempExMesh = TempExMesh->NextMesh;
	}
	sprintf(&ErrorMessage[0], "GetBOXIndex: Could not find mesh \"%s\"",  BoxName);
	MessageBox(0, &ErrorMessage[0], "ERROR", MB_OK);
}
void ConvertJointsFromLocalToWorld(void)  // переводим джоинты из локальных в мировые
{int i;
	Quaternion  TempQuat;
Vector ResultVector;
//GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientX
 for (i = 0; i < GlobalMesh.Mesh.NumHierarhy; i++ )
 {



        if ( GlobalMesh.Mesh.Hierarhy[i].ParrentIndex != -1 ) //Первую кость пропустим потому что она и так в мировых
		{



/*
			{
FILE *file;
char  TestString[255];
sprintf(TestString,"ConvertingJoint %d\n",-1);
file = fopen( "C:\\Files.txt", "a+" );
fputs(TestString, file);
fclose( file );
	}		
*/
				 
				 ResultVector = QuaternionMultiplyVector((Quaternion *)&GlobalMesh.Mesh.Frames[GlobalMesh.Mesh.Hierarhy[i].ParrentIndex].orientX,(Vector *)&GlobalMesh.Mesh.Frames[i].posX);

				GlobalMesh.Mesh.Frames[i].posX=GlobalMesh.Mesh.Frames[GlobalMesh.Mesh.Hierarhy[i].ParrentIndex].posX+ResultVector.x;
				GlobalMesh.Mesh.Frames[i].posY=GlobalMesh.Mesh.Frames[GlobalMesh.Mesh.Hierarhy[i].ParrentIndex].posY+ResultVector.y;
				GlobalMesh.Mesh.Frames[i].posZ=GlobalMesh.Mesh.Frames[GlobalMesh.Mesh.Hierarhy[i].ParrentIndex].posZ+ResultVector.z;
				TempQuat = QuaternionMultiply((Quaternion *)&GlobalMesh.Mesh.Frames[GlobalMesh.Mesh.Hierarhy[i].ParrentIndex].orientX,(Quaternion *)&GlobalMesh.Mesh.Frames[i].orientX);
				GlobalMesh.Mesh.Frames[i].orientX = TempQuat.x;
				GlobalMesh.Mesh.Frames[i].orientY = TempQuat.y;
				GlobalMesh.Mesh.Frames[i].orientZ = TempQuat.z;
				GlobalMesh.Mesh.Frames[i].orientW = TempQuat.w;
				//TempQuat  = QuaternionMultiply((Quaternion *)&GlobalMesh.Mesh.Frames[GlobalMesh.Mesh.Hierarhy[i].ParrentIndex].orientX,
			//	Vector * vector = ;
				//TempQuat = 	;			
				
				//joints [i].pos    = joints [i].parent -> pos + 
             //                   joints [i].parent -> orient.rotate ( joints [i].pos );
           // joints [i].orient = joints [i].parent -> orient * joints [i].orient;
        }
 }
}

MObject ReturnObjectToAdd(BoneIndexer & NamesIndexer, int & MeshesAddedCounter, bool & Reset )
{
	MObject object;
	//int MeshesAddedCounter=0;
	//bool Reset = false;
bool GoToNextIttem = false;
 MItDependencyNodes iter( MFn::kSkinClusterFilter );
 MDagPathArray infs;
MStatus       stat;
for ( ; !iter.isDone(); iter.next() ) {
  	Reset=false;
GoToNextIttem = false;
object = iter.item();
MFnSkinCluster skinCluster(object);
 if (!skinCluster.influenceObjects(infs,&stat)) {
        GoToNextIttem = true;
    }
int num_geoms=skinCluster.numOutputConnections();
for (int i=0;i<num_geoms;i++) {
	

		int index=skinCluster.indexForOutputConnection(i);
        MDagPath skinPath;
        skinCluster.getPathAtIndex(index,skinPath);


        // sanity check
        MFnDagNode dagNode(skinPath);
        if (dagNode.isIntermediateObject())
        {
			
            //Intermeidate object affected by skin cluster
          //  iter.next();
			GoToNextIttem = true;
			//MessageBox(0,"isIntermediateObject","isIntermediateObject",MB_OK);
			break;
        }

		if(!skinPath.hasFn( MFn::kMesh ))
		{
			GoToNextIttem = true;
			//MessageBox(0,"isIntermediateObject","isIntermediateObject",MB_OK);
			break;
		}
// проверяем -  совпадает ли имя   если не совпадает со списком то пропускаем  iter.next();
//int indexa = NamesIndexer.AddBone((char *)skinPath.partialPathName().asChar());
//char debugtext[255];
//sprintf(debugtext,"%d %d %d %d",indexa,MeshesAddedCounter, NumMeshesCalculated, MeshesAddedCounter );
//MessageBox(0,debugtext,(char *)skinPath.partialPathName().asChar(),MB_OK);
	

		if(MeshesAddedCounter!=NamesIndexer.AddBone((char *)skinPath.partialPathName().asChar()))
		{
			//MessageBox(0,"GoingToNext","isIntermediateObject",MB_OK);
			GoToNextIttem = true;  
			break;
		}
	}
if(!GoToNextIttem)
{
//OutputSkinCluster(object,DBuffer);

//MessageBox(0,"AddedMesh","OK",MB_OK);
MeshesAddedCounter++;
Reset = true;
return object; // Каждый раз мы должны возвращатся отсюда - у нас всегда должен быть обьект для экспорта
//if(MeshesAddedCounter == NumMeshesCalculated ) 
//{
//	MessageBox(0,"AllMeshesAdded","OK",MB_OK);
//		break;
//}

}





}
MessageBox(0,"Error While Exporting Weights", "CriticalError", MB_OK);
// Программа сюда никогда не должна возвращатся отсюда
}
int SaveBonesForFrame(StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer, int FrameNum, OneExportFrame * EF)
{
	EF->FrameNum = FrameNum;
	EF->Bones = new ExBone[BHierarhy.NumBones];
int BoneCounter = 0;
MItDag BoneIterator(MItDag::kDepthFirst, MFn::kTransform);
while(!BoneIterator.isDone())
{
// attach the function set to the object
MFnTransform fn(BoneIterator.item());

// only want non-history items
if( !fn.isIntermediateObject() ) {
  	

OutputBoneForFrame(BoneIterator.item(),DBuffer,BIndexer,FrameNum,BoneCounter, EF);
	

// described in the sections below
//outputTransformData(it.item());
//outputParentingInfo(it.item());

}

// get next transform
BoneIterator.next();
} 

return BoneCounter;
}


int SaveBonesOnlyForCalculation()
{
int BoneCounter = 0;
MItDag BoneIterator(MItDag::kDepthFirst, MFn::kTransform);
while(!BoneIterator.isDone())
{
// attach the function set to the object
MFnTransform fn(BoneIterator.item());

// only want non-history items
if( !fn.isIntermediateObject() ) {
  	

if(OutputBoneOnlyForCalculation(BoneIterator.item()))
 BoneCounter++;

// described in the sections below
//outputTransformData(it.item());
//outputParentingInfo(it.item());

}

// get next transform
BoneIterator.next();
} 

return BoneCounter;
}

int SaveBones(StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer, ExHierarhy * BH)
{
int BoneCounter = 0;
MItDag BoneIterator(MItDag::kDepthFirst, MFn::kTransform);
while(!BoneIterator.isDone())
{
// attach the function set to the object
MFnTransform fn(BoneIterator.item());

// only want non-history items
if( !fn.isIntermediateObject() ) {
  	

if(OutputBone(BoneIterator.item(),DBuffer,BIndexer, &BH[BoneCounter]))
 BoneCounter++;

// described in the sections below
//outputTransformData(it.item());
//outputParentingInfo(it.item());

}

// get next transform
BoneIterator.next();
} 

return BoneCounter;
}



#define ROUND_VAL 0.5f
#define GO_MINUS_TO 100
int Rounding(float NumToRound)  // Округляем флоат до целого
{
if((int)((float)NumToRound+GO_MINUS_TO - (float)ROUND_VAL) == (int)NumToRound+GO_MINUS_TO) return (int)NumToRound+1;
return (int)NumToRound;
};

// если у нас нет фреймов то сохраним 1 фрейм для того чтоб у нас были координаты костей
void SaveDefaultFrame(StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer)
{
	ExFrames.NumFrames = 1;
	ExFrames.ExFrames = new OneExportFrame[1];
	
//DBuffer.AddData("Frame	");
//MayaAddIntToBuffer(1,DBuffer);// В первом фрейме добавим количество фреймов
//DBuffer.AddData("{");
//DBuffer.AddData("\n			FrameNum	");
//MayaAddIntToBuffer(0,DBuffer);
	ExFrames.ExFrames[0].FrameNum = 0;
	ExFrames.ExFrames[0].FrameTime = 0;
	double Time = 0.0f;
//DBuffer.AddData("\n			FrameTime	");
//MayaAddIntToBuffer(0,DBuffer);
MAnimControl::setCurrentTime(MTime(Time,MTime::kSeconds));
SaveBonesForFrame(DBuffer,BIndexer,0, &ExFrames.ExFrames[0]);
		
//DBuffer.AddData("\n	};\n\n");
}

void SaveFrame(int FrameNum, StringDynamicBuffer & DBuffer, DoublesSorter & Times,BoneIndexer & BIndexer, OneExportFrame * EF)
{
//DBuffer.AddData("Frame	");
//if(!FrameNum)  MayaAddIntToBuffer(Times.counter,DBuffer);// В первом фрейме добавим количество фреймов
//DBuffer.AddData("{");
//DBuffer.AddData("\n			FrameNum	");
//MayaAddIntToBuffer(FrameNum,DBuffer);
double Time = Times.PrintUnit((FrameNum+1));
//DBuffer.AddData("\n			FrameTime	");
//MayaAddIntToBuffer(Rounding((float)((double)Time/((double)1.0f/(double)24.0f))),DBuffer);
EF->FrameTime = Rounding((float)((double)Time/((double)1.0f/(double)24.0f)));
MAnimControl::setCurrentTime(MTime(Time,MTime::kSeconds));



SaveBonesForFrame(DBuffer,BIndexer,FrameNum, EF);

//DBuffer.AddData("\n	};\n\n");
}

void SaveFrameTimes(DoublesSorter & Times)
{ // 


MItDependencyNodes it(MFn::kAnimCurve);
		
while(!it.isDone())
{

	///----------------

MFnAnimCurve fnAnimCurve(it.item());

unsigned int iKeyCount = fnAnimCurve.numKeys();




// dont bother if its a pointless curve....
if(iKeyCount == 0)
{
it.next();
continue;
}


//cout << "AnimCurve " << fnAnimCurve.name().asChar() << endl;
//cout << "NumKeys " << iKeyCount << endl;

// get all keyframe times & values
for(unsigned int i=0;i<iKeyCount;i++)
{


MTime Time = fnAnimCurve.time(i).value();
//float Value = fnAnimCurve.value(i);

// now i'm not too sure about this, so I will
// state something that may be untrue. When
// getting the tangents to the animation curves
// you can pretty much ignore the x tangent
// values because time is generally a linear
// value. (am i right here?) If I am wrong then
// write the x values as well.

//float ix,iy,ox,oy;
//fnAnimCurve.getTangent(i,ix,iy,true);
//fnAnimCurve.getTangent(i,ox,oy,false);

//cout << "AddValue" << Time.value() << " " << i<< endl;


Times.AddValue(Time.as(MTime::kSeconds));  //сохраняем время


// write keyframe info
//cout << "\n time " << Time.as(MTime::kSeconds)<<endl;
//cout << " value " << Value;
//cout << " InTangent " << iy;
//cout << " OutTangent " << oy << endl;

}



//----------------

// get next mesh
it.next();


}


}

bool SaveBonesToMemory(MObject& obj,BoneType * MemoryForBone, int memCounter)
{
	bool WasJoint = false;

	MFnDagNode nod(obj);
 	MQuaternion JointOrient(0,0,0,1);
MVector Translation;
// attach the function set to the object
MFnTransform fn(obj);
if( fn.object().hasFn(MFn::kJoint) )
{
MFnIkJoint fnJoint(fn.object());
fnJoint.getOrientation(JointOrient);
Translation = fnJoint.getTranslation(MSpace::kTransform );

//DBuffer.AddData("\n			Bone	");
//DBuffer.AddData((char *)fn.name().asChar());
//DBuffer.AddData("	");


if(!nod.parentCount())
{
//DBuffer.AddData("NoParrent");
}else
{
MObject obj = nod.parent(0);

// attach a function set to the parent object
MFnDagNode fnParent(obj);
//DBuffer.AddData((char *)fnParent.name().asChar());
}

MemoryForBone[memCounter].posX = Translation.x;
MemoryForBone[memCounter].posY = Translation.y;
MemoryForBone[memCounter].posZ = Translation.z;

MemoryForBone[memCounter].orientX = JointOrient.x;
MemoryForBone[memCounter].orientY = JointOrient.y;
MemoryForBone[memCounter].orientZ = JointOrient.z;
WasJoint = true;
}
return WasJoint;
}
//void OutputParent(MObject& obj, DynamicBuffer & DBuffer)
//{

// output parent names
//cout << "numParents " << fn.parentCount() << endl;

//}

bool OutputBone(MObject& obj, StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer, ExHierarhy * BH)
{
	int BoneIndex;
	bool WasJoint = false;
	MFnDagNode nod(obj);
 	MQuaternion JointOrient(0,0,0,1);
	MQuaternion Rotation(0,0,0,1);
MVector Translation;
// attach the function set to the object
MFnTransform fn(obj);

if( fn.object().hasFn(MFn::kJoint) )
{
MFnIkJoint fnJoint(fn.object());
fnJoint.getOrientation(JointOrient);

Translation = fnJoint.getTranslation(MSpace::kTransform );
fn.getRotation(Rotation);
//DBuffer.AddData("\n			Bone	");
memcpy(&BH->BoneName[0],(char *)fn.name().asChar(),GetStringLengh((char *)fn.name().asChar())+1);
//DBuffer.AddData((char *)fn.name().asChar());
//DBuffer.AddData("	");
BoneIndex = BIndexer.AddBone((char *)fn.name().asChar());
//MayaAddIntToBuffer(BoneIndex,DBuffer);
BH->BoneIndex = BoneIndex;
//DBuffer.AddData("	");
MObject obj = nod.parent(0);

// attach a function set to the parent object
MFnDagNode fnParent(obj);


if(ConnStrAllBones.CheckIsItemInList((char *)fnParent.name().asChar()))
{ // если такой в списке есть значит это не root joint
//DBuffer.AddData((char *)fnParent.name().asChar());
	memcpy(&BH->ParentName[0],(char *)fnParent.name().asChar(),GetStringLengh((char *)fnParent.name().asChar())+1);
	//DBuffer.AddData(" ");
GlobalMesh.Mesh.Hierarhy[BoneIndex].ParrentIndex = BIndexer.AddBone((char *)fnParent.name().asChar());
//MayaAddIntToBuffer(GlobalMesh.Mesh.Hierarhy[BoneIndex].ParrentIndex,DBuffer);
BH->ParentIndex = GlobalMesh.Mesh.Hierarhy[BoneIndex].ParrentIndex;
BH->Parrent = true;
}else
{ // если в списке нет значит это root joint
//DBuffer.AddData("NoParrent");
memcpy(&BH->ParentName[0],"NoParrent",GetStringLengh("NoParrent")+1);
GlobalMesh.Mesh.Hierarhy[BoneIndex].ParrentIndex = -1;
BH->Parrent = false;
}




//MayaAddFloatToBuffer((float)Translation.x, DBuffer);
//MayaAddFloatToBuffer((float)Translation.y, DBuffer);
//MayaAddFloatToBuffer((float)Translation.z, DBuffer);

//MayaAddFloatToBuffer((float)JointOrient.x, DBuffer);
//MayaAddFloatToBuffer((float)JointOrient.y, DBuffer);
//MayaAddFloatToBuffer((float)JointOrient.z, DBuffer);
//MayaAddFloatToBuffer((float)JointOrient.w, DBuffer);
WasJoint = true;
}
return WasJoint;
}
bool OutputBoneOnlyForCalculation(MObject& obj)
{
	bool WasJoint = false;
// attach the function set to the object
MFnTransform fn(obj);
if( fn.object().hasFn(MFn::kJoint) )
{
	ConnStrAllBones.AddUnit((char *)fn.name().asChar()); //Сохраняем сюда все боны чтоб потом можно было легко найти root джоинты
WasJoint = true;
}
return WasJoint;
}
bool OutputBoneForFrame(MObject& obj, StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer,int FrameNum,int & BoneNum, OneExportFrame * EF)
{
	bool WasJoint = false;
	MFnTransform fn(obj);
Quaternion  TempQuat1;
Quaternion  TempQuat2;
Quaternion  TempQuat3;
Quaternion  TempQuat4;
// If you want the transform data as a matrix
// then you can do something like this. I personally
// am more interested in getting quaternions....
MMatrix mat = fn.transformation().asMatrix();

MQuaternion JointOrient(0,0,0,1);
MQuaternion Rotation(0,0,0,1);
MQuaternion RotateAxis(0,0,0,1);//rotateOrient (attribute name is rotateAxis)
double Scale[3];

// get the transforms local translation 
MVector Translation = fn.translation(MSpace::kWorld);

// get the transforms scale
fn.getScale(Scale);

// get the transforms rotation as a quaternion

if( fn.object().hasFn(MFn::kJoint) )
{
MFnIkJoint fnJoint(fn.object());
fnJoint.getOrientation(JointOrient);
fnJoint.getRotation(Rotation);
fnJoint.getScaleOrientation(RotateAxis);
//fn.getRotation(JointRotation);
/*
Detailed Description
Function set for joints.

This is the function set for joints.

The transformation matrix for a joint node is below.

    * matrix = [S] * [RO] * [R] * [JO] * [IS] * [T]

(where '*' denotes matrix multiplication).

These matrices are defined as follows:

    * [S] : scale
    * [RO] : rotateOrient (attribute name is rotateAxis)
    * [R] : rotate
    * [JO] : jointOrient
    * [IS] : parentScaleInverse
    * [T] : translate

The methods to get the value of these matrices are:

    * [S] : getScale
    * [RO] : getScaleOrientation
    * [R] : getRotation
    * [JO] : getOrientation
    * [IS] : (the inverse of the getScale on the parent transformation matrix)
    * [T] : translation
*/

//  RotateAxis * Rotation * JointOrient;
Translation = fnJoint.getTranslation(MSpace::kTransform );

//DBuffer.AddData("\n			Bone	");
//DBuffer.AddData((char *)fn.name().asChar());

//DBuffer.AddData("	");
unsigned int BIndex = BIndexer.AddBone((char *)fn.name().asChar());
//MayaAddIntToBuffer(BIndexer.AddBone((char *)fn.name().asChar()),DBuffer);

memcpy(&EF->Bones[BIndex].Name[0],(char *)fn.name().asChar(),GetStringLengh((char *)fn.name().asChar())+1);

//DBuffer.AddData("	");


GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].posX=(float)Translation.x; //Запишем все от всех фреймов. Хотя нам реально нужен только первый фрейм чтоб в весах записать позиции вертексов
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].posY=(float)Translation.y;
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].posZ=(float)Translation.z;
EF->Bones[BIndex].PosX = (float)Translation.x;
EF->Bones[BIndex].PosY = (float)Translation.y;
EF->Bones[BIndex].PosZ = (float)Translation.z;

//MayaAddFloatToBuffer((float)Translation.x, DBuffer);
//MayaAddFloatToBuffer((float)Translation.y, DBuffer);
//MayaAddFloatToBuffer((float)Translation.z, DBuffer);


if(GlobalMesh.Mesh.Hierarhy[BoneNum].ParrentIndex != -1)
{
        TempQuat1.x = (float)RotateAxis.x;
		TempQuat1.y = (float)RotateAxis.y;
		TempQuat1.z = (float)RotateAxis.z;
		TempQuat1.w = (float)RotateAxis.w;

		TempQuat2.x = (float)Rotation.x;
		TempQuat2.y = (float)Rotation.y;
		TempQuat2.z = (float)Rotation.z;
		TempQuat2.w = (float)Rotation.w;

		TempQuat4.x = (float)JointOrient.x;
		TempQuat4.y = (float)JointOrient.y;
		TempQuat4.z = (float)JointOrient.z;
		TempQuat4.w = (float)JointOrient.w;

TempQuat3 = QuaternionMultiply(&TempQuat1, &TempQuat2);
TempQuat4 = QuaternionMultiply(&TempQuat4, &TempQuat3);
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientX=TempQuat4.x;
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientY=TempQuat4.y;
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientZ=TempQuat4.z;
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientW=TempQuat4.w;
//MayaAddFloatToBuffer(TempQuat4.x, DBuffer);
//MayaAddFloatToBuffer(TempQuat4.y, DBuffer);
//MayaAddFloatToBuffer(TempQuat4.z, DBuffer);
//MayaAddFloatToBuffer(TempQuat4.w, DBuffer);
EF->Bones[BIndex].QuatX = TempQuat4.x;
EF->Bones[BIndex].QuatY = TempQuat4.y;
EF->Bones[BIndex].QuatZ = TempQuat4.z;
EF->Bones[BIndex].QuatW = TempQuat4.w;
}else

{
	//cout << " Root Dectected " << BoneNum << endl;
 TempQuat1.x = (float)RotateAxis.x;
		TempQuat1.y = (float)RotateAxis.y;
		TempQuat1.z = (float)RotateAxis.z;
		TempQuat1.w = (float)RotateAxis.w;

		TempQuat2.x = (float)Rotation.x;
		TempQuat2.y = (float)Rotation.y;
		TempQuat2.z = (float)Rotation.z;
		TempQuat2.w = (float)Rotation.w;

		TempQuat4.x = (float)JointOrient.x;
		TempQuat4.y = (float)JointOrient.y;
		TempQuat4.z = (float)JointOrient.z;
		TempQuat4.w = (float)JointOrient.w;

TempQuat3 = QuaternionMultiply(&TempQuat1, &TempQuat2);
TempQuat4 = QuaternionMultiply(&TempQuat4, &TempQuat3);

GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientX=TempQuat4.x;
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientY=TempQuat4.y;
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientZ=TempQuat4.z;
GlobalMesh.Mesh.Frames[(FrameNum*GlobalMesh.Mesh.NumHierarhy)+BoneNum].orientW=TempQuat4.w;
//MayaAddFloatToBuffer(TempQuat4.x, DBuffer);
//MayaAddFloatToBuffer(TempQuat4.y, DBuffer);
//MayaAddFloatToBuffer(TempQuat4.z, DBuffer);
//MayaAddFloatToBuffer(TempQuat4.w, DBuffer);
EF->Bones[BIndex].QuatX = TempQuat4.x;
EF->Bones[BIndex].QuatY = TempQuat4.y;
EF->Bones[BIndex].QuatZ = TempQuat4.z;
EF->Bones[BIndex].QuatW = TempQuat4.w;
}

BoneNum++;
//MayaAddFloatToBuffer((float)JointOrient.w, DBuffer);
WasJoint = true;
}
return WasJoint;
}




int OutputSkinClusterOnlyForalculation(BoneIndexer & NamesIndexer,MObject& obj, int & MaxInd, int GeomNumber, int & RetValueWeightsCalculated, PluginMeshes * MeshCalculatorMemory,int & NumVertexes)
{
	int MeshIndex;
int counterForVertexes=0;
int idx;
	MObject RetObject;
	MDagPath pathForObj;
MDoubleArray DoubleArrey;
MSelectionList Results;
int counter = 0;
MDagPathArray infs;
MStatus       stat;
MFnSkinCluster skinCluster(obj);
    if (!skinCluster.influenceObjects(infs,&stat)) {
        return 0;
    }
int num_geoms=skinCluster.numOutputConnections();
for (int i=0;i<num_geoms;i++) {
	
		int index=skinCluster.indexForOutputConnection(i);
        MDagPath skinPath;
        skinCluster.getPathAtIndex(index,skinPath);


        // sanity check
        MFnDagNode dagNode(skinPath);
        if (dagNode.isIntermediateObject())
        {
            //Intermeidate object affected by skin cluster
            return 0;
        }

MeshIndex = NamesIndexer.AddBone((char *)skinPath.partialPathName().asChar());
//cout << endl << skinPath.partialPathName().asChar() << endl;
		
/*
	skinCluster.getPointsAffectedByInfluence(infs[infl_index], Results, DoubleArrey);
	//cout << Results.length() << " " << skinPath.partialPathName().asChar() << " " << infs.length();


	//Results.getDagPath(0,pathForObj,RetObject);
	//cout << pathForObj.partialPathName().asChar() << "\n";

for(int i = 0; i < Results.length(); i++)
{
Results.getDagPath(i,pathForObj,RetObject);
//cout << pathForObj.partialPathName().asChar() << DoubleArrey.length() <<  "\n";
}
*/
	//float * gen_weights;
            MItGeometry geomIter(skinPath);
            int vertexes_count = geomIter.count();
            //gen_weights=new float[vertexes_count];
            // do some real work
			for (idx=0;!geomIter.isDone();geomIter.next(),idx++){
				if (MaxInd<counterForVertexes) MaxInd = counterForVertexes;
				counterForVertexes=0;
				MeshCalculatorMemory[GeomNumber].Calcs[idx].StartIndex = RetValueWeightsCalculated;
				RetValueWeightsCalculated++;
				for (int infl_index=0;infl_index<infs.length();infl_index++) {
                //ASSERT(idx<geomIter.count());
                MFloatArray weights;
                skinCluster.getWeights(skinPath,geomIter.component(),infl_index,weights);
				if(weights[0]!=0.0f)
				{
					MeshCalculatorMemory[MeshIndex].Calcs[idx].Count++;
					
					
					counterForVertexes++;
					counter++;
					/*
                //ASSERT(weights.length()==1);
				//cout << weights.length() << "\n";
                //gen_weights[idx]=weights[0];
				DBuffer.AddData("\n			Weight	");
				MayaAddIntToBuffer(counter,DBuffer);
				counter++;
				DBuffer.AddData("	");
				DBuffer.AddData((char *)skinPath.partialPathName().asChar());
				DBuffer.AddData("	");
				MayaAddIntToBuffer(idx,DBuffer);
				DBuffer.AddData("	");
				DBuffer.AddData((char *)infs[infl_index].partialPathName().asChar());
				MayaAddFloatToBuffer(weights[0], DBuffer);
				*/
				}
				//DBuffer.AddData("	");
				//MayaAddIntToBuffer(weights.length(),DBuffer);
				
				/*
for (unsigned int kk = 0; kk < infs.length(); ++kk) {
	DBuffer.AddData("	");
  	DBuffer.AddData((char *)infs[kk].partialPathName().asChar());
} 
*/
			}









}
		NumVertexes+=idx;
	}
	return counter;
}

void OutputSkinCluster(MObject& obj,StringDynamicBuffer & DBuffer,BoneIndexer & BIndexer, int NumMeshes)
{
OneWeight * NewWeight;
	int BoneIndex;
	Quaternion TempQuaternion;
	Vector TempVector;
	 Meshes * TempMesh;
	char TempName[255];
int MeshNameLengh;
int SecondMeshNameLengh;
	MObject RetObject;
	MDagPath pathForObj;
MDoubleArray DoubleArrey;
MSelectionList Results;
int counter = 0;
MDagPathArray infs;
MStatus       stat;
MFnSkinCluster skinCluster(obj);
    if (!skinCluster.influenceObjects(infs,&stat)) {
        return;
    }
int num_geoms=skinCluster.numOutputConnections();
	for (int i=0;i<num_geoms;i++) {
	
		int index=skinCluster.indexForOutputConnection(i);
        MDagPath skinPath;
        skinCluster.getPathAtIndex(index,skinPath);


        // sanity check
        MFnDagNode dagNode(skinPath);
        if (dagNode.isIntermediateObject())
        {
            //Intermeidate object affected by skin cluster
            return;
        }



		
/*
	skinCluster.getPointsAffectedByInfluence(infs[infl_index], Results, DoubleArrey);
	//cout << Results.length() << " " << skinPath.partialPathName().asChar() << " " << infs.length();


	//Results.getDagPath(0,pathForObj,RetObject);
	//cout << pathForObj.partialPathName().asChar() << "\n";

for(int i = 0; i < Results.length(); i++)
{
Results.getDagPath(i,pathForObj,RetObject);
//cout << pathForObj.partialPathName().asChar() << DoubleArrey.length() <<  "\n";
}
*/
	//float * gen_weights;
            MItGeometry geomIter(skinPath);
            int vertexes_count = geomIter.count();
            //gen_weights=new float[vertexes_count];
            // do some real work
            for (int idx=0;!geomIter.isDone();geomIter.next(),idx++){
			for (int infl_index=0;infl_index<infs.length();infl_index++) {
                //ASSERT(idx<geomIter.count());
                MFloatArray weights;
                skinCluster.getWeights(skinPath,geomIter.component(),infl_index,weights);
				if(weights[0]!=0.0f)
				{
                //ASSERT(weights.length()==1);
				//cout << weights.length() << "\n";
                //gen_weights[idx]=weights[0];
NewWeight = new OneWeight;
				
				//DBuffer.AddData("\n			Weight	");
				//MayaAddIntToBuffer(counter,DBuffer);
NewWeight->Count = counter;
				counter++;
			//	DBuffer.AddData("	");
//
MeshNameLengh = GetStringLengh((char *)skinPath.partialPathName().asChar());
SecondMeshNameLengh = MeshNameLengh;
memcpy(&TempName,(char *)skinPath.partialPathName().asChar(),MeshNameLengh+1);
MeshNameLengh--;
while((TempName[MeshNameLengh] >= '0')&&(TempName[MeshNameLengh] <= '9') )
{
MeshNameLengh--;
}
memcpy(&TempName[MeshNameLengh-4],&TempName[MeshNameLengh+1] ,SecondMeshNameLengh-MeshNameLengh);
//DBuffer.AddData((char *)&TempMesh->name);

TempMesh = &GlobalMesh.Mesh.Mesh;
for(int count =0; count != NumMeshes;count++)
{
	if(CompareStringMaya(&TempMesh->name[0], &TempName[0],GetStringLengh(&TempMesh->name[0]),GetStringLengh(&TempName[0])))
	{
			break;
	}
	TempMesh = TempMesh->NextMesh;
}
//cout << TempMesh->name << endl;
TempVector.x = TempMesh->MeshVertices[idx].VertexX;
TempVector.y = TempMesh->MeshVertices[idx].VertexY;
TempVector.z = TempMesh->MeshVertices[idx].VertexZ;
TempVector = QuaternionMultiplyVector(&TempMesh->MeshOrientation,&TempVector);
TempVector.x +=  TempMesh->MeshPos.x;
TempVector.y +=  TempMesh->MeshPos.y;
TempVector.z +=  TempMesh->MeshPos.z;
BoneIndex = BIndexer.AddBone((char *)infs[infl_index].partialPathName().asChar());
TempQuaternion.x = GlobalMesh.Mesh.Frames[BoneIndex].orientX;
TempQuaternion.y = GlobalMesh.Mesh.Frames[BoneIndex].orientY;
TempQuaternion.z = GlobalMesh.Mesh.Frames[BoneIndex].orientZ;
TempQuaternion.w = GlobalMesh.Mesh.Frames[BoneIndex].orientW;
QuaternionInvert(&TempQuaternion);
TempVector.x -= GlobalMesh.Mesh.Frames[BoneIndex].posX;
TempVector.y -= GlobalMesh.Mesh.Frames[BoneIndex].posY;
TempVector.z -= GlobalMesh.Mesh.Frames[BoneIndex].posZ;
TempVector = QuaternionMultiplyVector(&TempQuaternion,&TempVector);

memcpy(&NewWeight->InfluencedMeshName[0],&TempName[0],GetStringLengh(&TempName[0])+1);
				//DBuffer.AddData(&TempName[0]);
				//DBuffer.AddData("	");
				//MayaAddIntToBuffer(idx,DBuffer);
				NewWeight->idx = idx;
				//DBuffer.AddData("	");
				//DBuffer.AddData((char *)infs[infl_index].partialPathName().asChar());
				memcpy(&NewWeight->JointName[0],(char *)infs[infl_index].partialPathName().asChar(),GetStringLengh((char *)infs[infl_index].partialPathName().asChar())+1);
				//DBuffer.AddData("	");
				
				//MayaAddIntToBuffer(BoneIndex,DBuffer);
				NewWeight->BoneIndex = BoneIndex;
				//MayaAddFloatToBuffer(weights[0], DBuffer);
				NewWeight->Weight = weights[0];
				//DBuffer.AddData("	");
				NewWeight->PosX = TempVector.x;
				//MayaAddFloatToBuffer(TempVector.x, DBuffer);
				//DBuffer.AddData("	");
				NewWeight->PosY =TempVector.y;
				//MayaAddFloatToBuffer(TempVector.y, DBuffer);
				//DBuffer.AddData("	");
				//MayaAddFloatToBuffer(TempVector.z, DBuffer);
				NewWeight->PosZ =TempVector.z;
				NewWeight->NextWeight = 0;
				if(!WE.LastWeightAdded)
				{
					WE.LastWeightAdded = NewWeight;
					WE.Weights = NewWeight;
				}else
				{
					WE.LastWeightAdded->NextWeight = NewWeight;
					WE.LastWeightAdded = NewWeight;
				}
				}
				//DBuffer.AddData("	");
				//MayaAddIntToBuffer(weights.length(),DBuffer);
				
				/*
for (unsigned int kk = 0; kk < infs.length(); ++kk) {
	DBuffer.AddData("	");
  	DBuffer.AddData((char *)infs[kk].partialPathName().asChar());
} 
*/
			}









}
		
	}
	
}
void OutputColor(MFnDependencyNode& fn,const char* name,StringDynamicBuffer & DBuffer,OneExportShader * ExportShader)
{MStatus status;

	bool texture = false;
MPlug p;

MString r = name;
r += "R";
MString g = name;
g += "G";
MString b = name;
b += "B";
MString a = name;
a += "A";

// get the color value
MColor color;

// get a plug to the attribute
p = fn.findPlug(r);
p.getValue(color.r);
p = fn.findPlug(g);
p.getValue(color.g);
p = fn.findPlug(b);
p.getValue(color.b);
p = fn.findPlug(a);
p.getValue(color.a);
//p = fn.findPlug(name.c_str());
p = fn.findPlug(name);
// will hold the txture node name
MString texname;

// get plugs connected to colour attribute
MPlugArray plugs;
p.connectedTo(plugs,true,false);

// see if any file textures are present

for(int i=0;i!=plugs.length();++i)
{
  	// if file texture found
	MObject oTex = plugs[i].node();
if(oTex.apiType() == MFn::kFileTexture)
{
  	// bind a function set to it ....
MFnDependencyNode fnDep(plugs[i].node());

// to get the node name
//texname = fnDep.name();

MPlug ftnPlug = fnDep.findPlug( "fileTextureName", &status );
        if ( status == MS::kSuccess )
        {
			texture = true;
          MString fileTextureName;
          ftnPlug.getValue( fileTextureName );
		  texname =  fileTextureName;
        }



 
// stop looping


}

}
if( 	name == "color" &&
color.r <0.01 &&
color.g < 0.01 &&
color.b < 0.01)

{
  	color.r=color.g=color.b=0.6f;

}
ExportShader->colorR = color.r;
ExportShader->colorG = color.g;
ExportShader->colorB = color.b;
//MayaAddFloatToBuffer(color.r, DBuffer);
//MayaAddFloatToBuffer(color.g, DBuffer);
//MayaAddFloatToBuffer(color.b, DBuffer);
//DBuffer.AddData("	");
if(texture)
{
//DBuffer.AddData((char *)texname.asChar());
memcpy(&ExportShader->Name[0],(char *)texname.asChar(),GetStringLengh((char *)texname.asChar())+1);
}else
{
//DBuffer.AddData("NoTexture");
	memcpy(&ExportShader->TextureName[0],"NoTexture",GetStringLengh("NoTexture")+1);
}
// output the name, color and texture ID
/*
<< color.r << " "
<< color.g << " "
<< color.b << " "
<< color.a << " tex= "
<< texname.asChar() << "\n";
*/
} 


void MayaAddIntToBuffer(int num,StringDynamicBuffer & DBuffer)
{
char * Text = new char[20];
Text[0] = 0;
sprintf(Text,"%d",num);
//char * Text = "Int";
DBuffer.AddData(Text);
delete [] Text;
}

void MayaAddFloatToBuffer(float num,StringDynamicBuffer & DBuffer)
{
		
char * Text = new char[20];
Text[0] = 0;
sprintf(Text,"%f",num);
DBuffer.AddData(" ");
DBuffer.AddData(Text);
delete [] Text;

	//char * Text = "Float";
//	DBuffer.AddData(Text);

}
MStatus uninitializePlugin( MObject obj )

{
	MStatus   status;
	MFnPlugin plugin( obj );



	return status;
}
