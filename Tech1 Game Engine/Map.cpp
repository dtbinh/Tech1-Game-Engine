/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Map.h"
#include "common.h"
#include "Scene.h"
SceneMap * Map;
extern cMesh * Mesh;
extern Scene * Sce;
extern gCamera * wCamera;
extern Console * CON;
extern cMaterial * Material;
SceneMap::SceneMap()
{
MapFolderName = "Data\\Maps";
MeshFolderName = "Data\\Mesh";
LastTokenStart = 0;
LastTokenStartReadPos = 0;
LastTokenSize = 0;
}

SceneMap::~SceneMap()
{

}

void SceneMap::LoadMap(char * MapName)
{
char TempString[255];
sprintf(&TempString[0],"%s\\%s", MapFolderName, MapName);
//MessageBoxA(0, &FileToLoad[0], "", MB_OK);

//Mesh->LoadMeshFromFile(0,FileName,false);//0 - индекс загружаемого меша
//if(Mesh->ErrorLoadingMesh) return false;
//return true;
//MessageBoxA(0, &TempString[0], "", MB_OK);
int FileSize = GetFileSizez(&TempString[0]);
if(!FileSize)
{
	CON->ErrorMessage("Error loading map \"%s\"", MapName);
	return;
}
Files File;
File.OpenFile(&TempString[0], "rb");
MapFileData = new char[FileSize+1];
File.ReadFile(MapFileData, FileSize);
MapFileData[FileSize]=0;
File.CloseFile();
//ScriptLoader->LoadMesh(MeshIndex,MeshFileData, true, NMesh);
CurrentMapFileName = MapName;
ParseMapData(MapFileData, Mesh->GetMeshHolderAdress());
}

bool SceneMap::ParseMapData(char * Data, OneMesh ** MeshList)
{
unsigned int NumChildrensLoaded = 0;
char TempStr[255];
int type = 0;
char * Token;
int LastCurNum;
for(;;)
{
	Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
	if(type == 0)
	{
		break;//cout << "Uncnown type" << endl;

	}
	if(type==BIG_OR_SMALL_LETERS_AND_NUMBERS)
	{
		
			if(CmpString(Token, "TotalNumChildren",LastTokenSize))
			{
				Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". Invalid  TotalNumChildren", CurrentMapFileName );
							
							return false;
						}
				Sce->ChildrenAllocMemory( ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum));
				continue;
			}
		
		if(CmpString(Token, "TotalNumMeshes",LastTokenSize))
			{
				Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". Invalid  TotalNumMeshes", CurrentMapFileName );
							
							return false;
						}
				CurrentMapNumMeshesTotal = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				Mesh->MeshAllocMemory(CurrentMapNumMeshesTotal);
				continue;
			}
			if(CmpString(Token, "NumMeshes",LastTokenSize))
			{
				Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". Invalid  NumMeshes", CurrentMapFileName );
							
							return false;
						}
				//CurrentMapNumMeshes 
				Sce->MeshAllocMemory( ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum));
				continue;
			}

			if(CmpString(Token, "NumBBox",LastTokenSize))
			{
				Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". Invalid  NumBBox", CurrentMapFileName );
							return false;
						}
			//	CurrentMapNumBBOX = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				Sce->BBoxAllocMemory( ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum));
			continue;
			}

			if(CmpString(Token, "NumNavMesh",LastTokenSize))
			{
				Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". Invalid  NumNavMesh", CurrentMapFileName );
							return false;
						}
				//CurrentMapNumNavMesh = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				Sce->NavMeshAllocMemory( ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum));
			continue;
			}

			if(CmpString(Token, "Mesh",LastTokenSize))
			{
					if(!LoadMesh(Data, NumChildrensLoaded))
						{
							return false;
						}
					
			continue;
			}
	}

	if(type == END_EOF)
	{
		break;	
	}
}
return true;
}

bool SceneMap::LoadMesh(char * Data, unsigned int & NumChildrensLoaded)
{
	bool LoadingRoot = false;
unsigned int i;
float * SaveToMatrix;
unsigned int TempVariable;
	unsigned int CurrentMeshIndex;
	unsigned int ThisMeshChildrensStartIndex = NumChildrensLoaded;
	float Matrix[16];
	float RootJoint[7];
int ConnectedToBone=-1;
int LoadingMeshType = -1;
//OneMesh * SavePlace=0;
//char TempStr[255];
//int ShaderIndex;
int type;
int LastCurNum;
char * Token;
Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
if(type == OPEN_BRACKET) // Должен идти OPEN_BRACKET
{
		Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		while(type != CLOSE_BRACKET)
		{
			if(type == END_EOF)
			{
				CON->ErrorMessage("Error loading \"%s\". LoadMesh:Unexpected end of file", CurrentMapFileName );
				return false;
			}

			if(CmpString(Token, "Name",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshName", CurrentMapFileName );
							return false;
						}
					LoadingRoot = false;
					if(!strcmp(GetStr(Token, LastTokenSize),"SceneRoot"))
					{
						LoadingRoot = true;
					}
					//memcpy(&TempStr[0] , Token, LastTokenSize);
					//TempStr[LastTokenSize] = 0;
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}

				if(CmpString(Token, "Type",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshType", CurrentMapFileName );
							return false;
						}
					
						if(CmpString(Token, "MESH",LastTokenSize))
						{
							LoadingMeshType = TYPE_MESH;
						}else if(CmpString(Token, "BBOX",LastTokenSize))
						{
							LoadingMeshType = TYPE_BBOX;
						}else if(CmpString(Token, "NAVMESH",LastTokenSize))
						{
							LoadingMeshType = TYPE_NAVMESH;
						}else
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshType", CurrentMapFileName );
							return false;
						}
					
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}

				if(CmpString(Token, "Index",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid mesh Index", CurrentMapFileName );
							return false;
						}
					if(LoadingMeshType == -1)
					{
						CON->ErrorMessage("Error loading \"%s\". LoadMesh:  Mesh Type not loaded", CurrentMapFileName );
						return false;
					}
					if(LoadingRoot)
					{
						Sce->SetSceneRootIndex(ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum));
					}
					if(LoadingMeshType == TYPE_MESH)
					{
						CurrentMeshIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
						Sce->GetMeshes()[CurrentMeshIndex].MeshIndex = Mesh->GetNumMeshesLoaded();
					}else if(LoadingMeshType == TYPE_BBOX)
					{
						CurrentMeshIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
						Sce->GetBoundingBoxes()[CurrentMeshIndex].BBoxIndex = Mesh->GetNumMeshesLoaded();
						int xxx = Mesh->GetNumMeshesLoaded();
					}else if(LoadingMeshType == TYPE_NAVMESH)
					{
						CurrentMeshIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
						Sce->GetNavMeshes()[CurrentMeshIndex].NavMeshIndex = Mesh->GetNumMeshesLoaded();
					}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}

			if(CmpString(Token, "AABB",LastTokenSize))
			{

					if(LoadingMeshType == -1)
					{
						CON->ErrorMessage("Error loading \"%s\". LoadMesh:  Mesh Type not loaded", CurrentMapFileName );
						return false;
					}
					if(LoadingMeshType == TYPE_MESH)
					{
						
					}else if(LoadingMeshType == TYPE_BBOX)
					{
						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid AABB", CurrentMapFileName );
							return false;
						}
						Sce->GetBoundingBoxes()[CurrentMeshIndex].AABB.bMin.x  = ConvertTextToNumberFloat(Token,LastTokenSize);

						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid AABB", CurrentMapFileName );
							return false;
						}
						Sce->GetBoundingBoxes()[CurrentMeshIndex].AABB.bMin.y  = ConvertTextToNumberFloat(Token,LastTokenSize);

						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid AABB", CurrentMapFileName );
							return false;
						}
						Sce->GetBoundingBoxes()[CurrentMeshIndex].AABB.bMin.z  = -ConvertTextToNumberFloat(Token,LastTokenSize);// Инвертируем - переводим из системы координат Maya в систему координат DirectX LH

						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid AABB", CurrentMapFileName );
							return false;
						}
						Sce->GetBoundingBoxes()[CurrentMeshIndex].AABB.bMax.x  = ConvertTextToNumberFloat(Token,LastTokenSize);

						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid AABB", CurrentMapFileName );
							return false;
						}
						Sce->GetBoundingBoxes()[CurrentMeshIndex].AABB.bMax.y  = ConvertTextToNumberFloat(Token,LastTokenSize);

						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid AABB", CurrentMapFileName );
							return false;
						}
						Sce->GetBoundingBoxes()[CurrentMeshIndex].AABB.bMax.z  = ConvertTextToNumberFloat(Token,LastTokenSize);
					}else if(LoadingMeshType == TYPE_NAVMESH)
					{
						
					}

					    for ( int i=0; i<8; i++ )
						{
							Sce->GetBoundingBoxes()[CurrentMeshIndex].AABBCoords[i] = wCamera->AABBPoint(i, &Sce->GetBoundingBoxes()[CurrentMeshIndex].AABB);
						}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}



			if(CmpString(Token, "ConnectedToBone",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid ConnectedToBone value.", CurrentMapFileName );
							return false;
						}
				if(CmpString(Token, "Yes",LastTokenSize))
				{
					ConnectedToBone = 1;
				}else if(CmpString(Token, "No",LastTokenSize))
				{
					ConnectedToBone = 0;
				}else
				{
					CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid ConnectedToBone value.", CurrentMapFileName );
					return false;
				}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}


			if(CmpString(Token, "MeshMatrix",LastTokenSize))
			{
				if(ConnectedToBone == -1)
				{
					CON->ErrorMessage("Error loading \"%s\". LoadMesh: No connected to bone information", CurrentMapFileName );
					return false;
				}
				if(ConnectedToBone == 1)
				{
					CON->ErrorMessage("Error loading \"%s\". LoadMesh: Mesh is connected to bone but have matrix.", CurrentMapFileName );
					return false;
				}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[0] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[1] = ConvertTextToNumberFloat(Token,LastTokenSize);
						

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[2] = ConvertTextToNumberFloat(Token,LastTokenSize);
					

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[3] = ConvertTextToNumberFloat(Token,LastTokenSize);


					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[4] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[5] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[6] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[7] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[8] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[9] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[10] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[11] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[12] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[13] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[14] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid MeshMatrix", CurrentMapFileName );
							return false;
						}
					Matrix[15] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}


			if(CmpString(Token, "RootJoint",LastTokenSize))
			{

				if(ConnectedToBone == -1)
				{
					CON->ErrorMessage("Error loading \"%s\". LoadMesh: No connected to bone information", CurrentMapFileName );
					return false;
				}
				if(ConnectedToBone == 0)
				{
					CON->ErrorMessage("Error loading \"%s\". LoadMesh: Mesh is not connected to bone but have root joint.", CurrentMapFileName );
					return false;
				}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid RootJoint", CurrentMapFileName );
							return false;
						}
					RootJoint[0] = ConvertTextToNumberFloat(Token,LastTokenSize);
						
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid RootJoint", CurrentMapFileName );
							return false;
						}
					RootJoint[1] = ConvertTextToNumberFloat(Token,LastTokenSize);


					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid RootJoint", CurrentMapFileName );
							return false;
						}
					RootJoint[2] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid RootJoint", CurrentMapFileName );
							return false;
						}
					RootJoint[3] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid RootJoint", CurrentMapFileName );
							return false;
						}
					RootJoint[4] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid RootJoint", CurrentMapFileName );
							return false;
						}
					RootJoint[5] = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid RootJoint", CurrentMapFileName );
							return false;
						}
					RootJoint[6] = ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}


			if(CmpString(Token, "NumChilds",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid NumChilds", CurrentMapFileName );
							return false;
						}
					TempVariable = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					
					
					for (i = 0; i != TempVariable; i++)
					{
						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid children name", CurrentMapFileName );
							return false;
						}

						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid children index", CurrentMapFileName );
							return false;
						}
						Sce->GetChildrens()[NumChildrensLoaded].ChildIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid children TYPE", CurrentMapFileName );
							return false;
						}
									if(CmpString(Token, "TYPE_BBOX",LastTokenSize))
									{
										Sce->GetChildrens()[NumChildrensLoaded].ChildrenType = TYPE_BBOX;
									}else if(CmpString(Token, "TYPE_MESH",LastTokenSize))
									{
										Sce->GetChildrens()[NumChildrensLoaded].ChildrenType = TYPE_MESH;
									}else if(CmpString(Token, "TYPE_NAVMESH",LastTokenSize))
									{
										Sce->GetChildrens()[NumChildrensLoaded].ChildrenType = TYPE_NAVMESH;
									}else
									{
										CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid children TYPE", CurrentMapFileName );
									}
						
						NumChildrensLoaded++;
					}
					if(i)
					{
					Sce->GetBoundingBoxes()[CurrentMeshIndex].StartChildrenIndex = ThisMeshChildrensStartIndex;
					Sce->GetBoundingBoxes()[CurrentMeshIndex].NumChildrens = TempVariable;
					}else
					{
					Sce->GetBoundingBoxes()[CurrentMeshIndex].StartChildrenIndex = 0;
					Sce->GetBoundingBoxes()[CurrentMeshIndex].NumChildrens = 0;
					}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}

			if(CmpString(Token, "Shader",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid ShaderName", CurrentMapFileName );
							return false;
						}
					
					Sce->GetBoundingBoxes()[CurrentMeshIndex].Material = Material->LoadMaterial(GetStr(Token, LastTokenSize));
					
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}

			if(CmpString(Token, "FileName",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
					{
						CON->ErrorMessage("Error loading \"%s\". LoadMesh: Invalid FileName", CurrentMapFileName );
						return false;
					}
					if(LoadingMeshType==TYPE_MESH)
					{
						Mesh->LoadMeshFromFile(Str("Data\\Meshes\\%s.t1m" , GetStr(Token, LastTokenSize)), false);
											
					if(Mesh->GetMeshes()[Mesh->GetNumMeshesLoaded()-1].Num4AnglePollys)
						{
							CON->ErrorMessage("Error loading \"%s\". LoadMesh: Mesh has 4 angle pollys.", CurrentMapFileName );
							return false;
						}
					SaveToMatrix = (float *)	&Mesh->GetMeshes()[Mesh->GetNumMeshesLoaded()-1].MeshMatrix.m[0][0];
					SaveToMatrix[0] = Matrix[0];
					SaveToMatrix[1] = Matrix[1];
					SaveToMatrix[2] = Matrix[2];
					SaveToMatrix[3] = Matrix[3];
					SaveToMatrix[4] = Matrix[4];
					SaveToMatrix[5] = Matrix[5];
					SaveToMatrix[6] = Matrix[6];
					SaveToMatrix[7] = Matrix[7];
					SaveToMatrix[8] = Matrix[8];
					SaveToMatrix[9] = Matrix[9];
					SaveToMatrix[10] = Matrix[10];
					SaveToMatrix[11] = Matrix[11];
					SaveToMatrix[12] = Matrix[12];
					SaveToMatrix[13] = Matrix[13];
					SaveToMatrix[14] = Matrix[14];
					SaveToMatrix[15] = Matrix[15];
					}else if(LoadingMeshType==TYPE_BBOX)
					{
						//Mesh->LoadMeshFromFile(Str("Data\\Meshes\\%s.t1m" , GetStr(Token, LastTokenSize)), false);
						//if(Mesh->GetMeshes()[Mesh->GetNumMeshesLoaded()-1].Num4AnglePollys)
						//{
							//CON->ErrorMessage("Error loading \"%s\". LoadMesh: Bounding box has 4 angle pollys.", CurrentMapFileName );
							//return false;
						//}
					//SaveToMatrix = (float *)	&Mesh->GetMeshes()[Mesh->GetNumMeshesLoaded()-1].MeshMatrix.m[0][0];
					////SaveToMatrix[0] = Matrix[0];
					//SaveToMatrix[1] = Matrix[1];
					//SaveToMatrix[2] = Matrix[2];
					///SaveToMatrix[3] = Matrix[3];
					//SaveToMatrix[4] = Matrix[4];
					//SaveToMatrix[5] = Matrix[5];
					//SaveToMatrix[6] = Matrix[6];
					//SaveToMatrix[7] = Matrix[7];
					//SaveToMatrix[8] = Matrix[8];
					//SaveToMatrix[9] = Matrix[9];
					//SaveToMatrix[10] = Matrix[10];
					//SaveToMatrix[11] = Matrix[11];
					//SaveToMatrix[12] = Matrix[12];
					//SaveToMatrix[13] = Matrix[13];
					//SaveToMatrix[14] = Matrix[14];
					//SaveToMatrix[15] = Matrix[15];
					}
					else if(LoadingMeshType==TYPE_NAVMESH)
					{
						//Mesh->LoadMeshFromFile(Str("Data\\Meshes\\%s.t1m" , GetStr(Token, LastTokenSize)), false);
					}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
			}



			// сюда попадаем если нет загрузчика
			Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			
			//OnlyForDebug++;
			continue;

		}

	}else
	{ // Файл поврежден   To do: Сделать вывод сообщения об этом
				CON->ErrorMessage("Error loading \"%s\". LoadMesh: Unexpected token.  Expected OpenBracket", CurrentMapFileName );
				return false;
	}
return true;
}