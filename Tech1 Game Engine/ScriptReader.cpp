/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#define _CRTDBG_MAPALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "ScriptReader.h"
#include <iostream>
#include "Mesh.h"
//#include "atlconv.h"

using namespace std;
extern g3dDevice * w3dDevice;
extern cMesh * Mesh;
ScriptReader * ScriptLoader;
int ConvertTextToNumberInteger(char * Text, int size, int & LastCurNum) // size - размер текста без 0
{
size--;
int i = 0;
int j;
int CurNum;
int RetValue=0;
LastCurNum = 10;
	while(i!=size+1)
	{
		CurNum = ((int)Text[size-i] - (int)'0');
		for(j=0; j!=i; j++)
			{
				CurNum	*=	10;
				if(i == size) LastCurNum *= 10;
			}
		RetValue +=  CurNum;
		i++;
	}
return RetValue;
}

int GetStrSize(char * Str)
{
	int i=0;
	while(Str[i])
	{
		i++;
	}
	return i;
}
float ConvertTextToNumberFloat(char * Text, int size)
{
float ReturnValue;
int i=0;
int BeforeDot;
int AfterDot;
int LastCurNum;
while(Text[i]!='.')
	{
		i++;
		
	}
if(Text[0]=='-')
	{
		i--;
		BeforeDot = ConvertTextToNumberInteger(Text+1, i, LastCurNum);
		AfterDot = ConvertTextToNumberInteger(Text+1 + (char)i+(char)1, size - i-2,LastCurNum);
		ReturnValue =-(((float)AfterDot / (float)LastCurNum)+BeforeDot);
	}else
	{
		BeforeDot = ConvertTextToNumberInteger(Text, i,LastCurNum);
		AfterDot = ConvertTextToNumberInteger(Text + (char)i+(char)1, size - i - 1,LastCurNum);
		//cout << "L:" << LastCurNum << endl;
		ReturnValue = ((float)AfterDot / (float)LastCurNum)+BeforeDot;
	}

return ReturnValue;
}
void ScriptReader::LoadAnimation(unsigned int AnimationIndex, char * Data)
{
ErrorLoadingScript = false;
ScriptData = Data;
LastTokenStart = 0;
LastTokenStartReadPos = 0;
LastTokenSize = 0;
if(!ProcessAnimationScript(AnimationIndex)) ErrorLoadingScript = true;
}
void ScriptReader::LoadMesh(unsigned int MeshIndex, char * Data, bool PoocMesh = false, bool NavMesh=false)
{
	MeshList = Mesh->GetMeshes();
LoadingNavMesh = NavMesh;
ErrorLoadingScript = false;
VerticesLoaded = 0;
ScriptData = Data;
LastTokenStart = 0;
LastTokenStartReadPos = 0;
LastTokenSize = 0;
if(PoocMesh)
if(!ProcessMeshScript(MeshIndex)) ErrorLoadingScript = true;
}
ScriptReader::ScriptReader()
{

}

ScriptReader::~ScriptReader()
{

}



bool CmpString(char * Str1, char * Str2, unsigned int Str1Size)
{
unsigned int Str2Size = strlen(Str2);
if( Str1Size != Str2Size) return false;
	while(Str1Size)
	{
		if(*Str1 != *Str2) return false;
		Str1++;
		Str2++;
		Str1Size--;
	}
	return true;
}

int NumNormals = 0; // только для дебага
int NumUV = 0;// только для дебага
int NumPolly = 0;// только для дебага
bool ScriptReader::MeshLoader(unsigned int MeshIndex)
{
	MeshList = Mesh->GetMeshes();
	MeshList[MeshIndex].AdjencyBuffer = NULL;
int PollysLoadedWithNoError =0;
int UvsCounter = 0;
int LastCurNum;
int TempShaderNumber=0;
int NumVertex=0;
int PollyVertexCount;
//int OnlyForDebug = 0;
//int TemporaryInteger;
int type = 0;
char * Token;
Token = GetNextTokenSkipSpacebar(ScriptData ,type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
if(type == OPEN_BRACKET) // Должен идти OPEN_BRACKET
{
		Token = GetNextTokenSkipSpacebar(ScriptData ,type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		while(type != CLOSE_BRACKET)
		{
			if(type == END_EOF)
			{
				cout << "MeshLoader: Unexpected end of file";
				return false;
			}

				if(CmpString(Token, "Name",LastTokenSize))
				{
					cout << "Script Reader: " << "Mesh Name Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
						}
					memcpy(MeshList[MeshIndex].name, Token, LastTokenSize);
					MeshList[MeshIndex].name[LastTokenSize]=0;

					MeshList[MeshIndex].NameSize = LastTokenSize;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

				if(CmpString(Token, "ConnectedToBone",LastTokenSize))
				{
					cout << "Script Reader: " << "ConnectedToBone Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
						}
					MeshList[MeshIndex].ConnectedToBone = true;
					if(CmpString(Token, "No",LastTokenSize))
					{
						MeshList[MeshIndex].ConnectedToBone = false;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
						}
						
						if(CmpString(Token, "MeshMatrix",LastTokenSize))
						{
							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[0][0] = ConvertTextToNumberFloat(Token,LastTokenSize);

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[0][1] = ConvertTextToNumberFloat(Token,LastTokenSize);


							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[0][2] = ConvertTextToNumberFloat(Token,LastTokenSize);
							

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[0][3] = ConvertTextToNumberFloat(Token,LastTokenSize);
						

														Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[1][0] = ConvertTextToNumberFloat(Token,LastTokenSize);

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[1][1] = ConvertTextToNumberFloat(Token,LastTokenSize);


							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[1][2] = ConvertTextToNumberFloat(Token,LastTokenSize);
							

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[1][3] = ConvertTextToNumberFloat(Token,LastTokenSize);

						
														Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[2][0] = ConvertTextToNumberFloat(Token,LastTokenSize);

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[2][1] = ConvertTextToNumberFloat(Token,LastTokenSize);


							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[2][2] = ConvertTextToNumberFloat(Token,LastTokenSize);
							

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[2][3] = ConvertTextToNumberFloat(Token,LastTokenSize);
						

														Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[3][0] = ConvertTextToNumberFloat(Token,LastTokenSize);  // Инвертируем X т.к у нас RL а в maya RH 

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[3][1] = ConvertTextToNumberFloat(Token,LastTokenSize);


							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[3][2] = ConvertTextToNumberFloat(Token,LastTokenSize);
							

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
							}
							MeshList[MeshIndex].MeshMatrix.m[3][3] = ConvertTextToNumberFloat(Token,LastTokenSize);
						
						}else
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							return false;
						}


					}

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}


				if(CmpString(Token, "NumPollys",LastTokenSize))
				{
					
					cout << "Script Reader: " << "NumPollys Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid number of Pollys" << endl;
							return false;
						}

					MeshList[MeshIndex].NumPollys = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					MeshList[MeshIndex].ComputedNormals = new MeshNormal[MeshList[MeshIndex].NumPollys];
					if(!LoadingNavMesh)
					{
						MeshList[MeshIndex].Polygons = new MeshPolly[MeshList[MeshIndex].NumPollys];
					}
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

				if(CmpString(Token, "Num3AnglePollys",LastTokenSize))
				{
					
					cout << "Script Reader: " << "Num3AnglePollys Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid number of Num3AnglePollys" << endl;
							return false;
						}
					MeshList[MeshIndex].Num3AnglePollys = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					if(MeshList[MeshIndex].Num3AnglePollys)
					{
						w3dDevice->g_pd3dDevice->CreateVertexBuffer(MeshList[MeshIndex].Num3AnglePollys * 3 * sizeof( MESH_CUSTOMVERTEX ), D3DUSAGE_WRITEONLY, D3DFVF_MESH_CUSTOMVERTEX,D3DPOOL_DEFAULT, &MeshList[MeshIndex].MeshVertexBuffer, NULL );
					}
					//w3dDevice->g_pd3dDevice->CreateIndexBuffer(Mesh->Num3AnglePollys * 3 * sizeof(WORD),  D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &Mesh->MeshIndexBuffer, NULL); 
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

				if(CmpString(Token, "Num4AnglePollys",LastTokenSize))
				{
					cout << "Script Reader: " << "Num4AnglePollys Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid number of Num4AnglePollys" << endl;
							return false;
						}
					MeshList[MeshIndex].Num4AnglePollys = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					if(MeshList[MeshIndex].Num4AnglePollys)
					{
					MeshList[MeshIndex].NavMeshPolygons = new NavMeshPolly[MeshList[MeshIndex].Num4AnglePollys];
					}
					//if(Mesh->Num4AnglePollys)
					//{
					//	MessageBoxA(0,"Critical error:\n Mesh has 4 angle Pollys.  GameEngine does not support 4 angle pollys.",(char *)&Mesh->name[0], MB_OK);
					//return false;
					//}											//(Mesh->Num4AnglePollys * 2)  Каждый полигон нужно делить на 2
					if(MeshList[MeshIndex].Num4AnglePollys)
					{
					w3dDevice->g_pd3dDevice->CreateVertexBuffer((MeshList[MeshIndex].Num4AnglePollys * 2) * 3 * sizeof( MESH_CUSTOMVERTEX ) , D3DUSAGE_WRITEONLY, D3DFVF_MESH_CUSTOMVERTEX,D3DPOOL_DEFAULT, &MeshList[MeshIndex].MeshVertexBuffer, NULL );
					}					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						
					continue;
				}

				if(CmpString(Token, "NumVertices",LastTokenSize))
				{
					cout << "Script Reader: " << "NumVertices Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid number of Vertices" << endl;
							return false;
						}

					MeshList[MeshIndex].NumVertices = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					if(LoadingNavMesh)
					{
						MeshList[MeshIndex].NavMeshVertices = new NavMeshVertex[MeshList[MeshIndex].NumVertices];
					}else
					{
						MeshList[MeshIndex].MeshVertices = new MeshVertex[MeshList[MeshIndex].NumVertices];
					}
					if(MeshList[MeshIndex].ConnectedToBone)
					{
					MeshList[MeshIndex].MeshVerticesForRendering = new Vert[MeshList[MeshIndex].NumVertices];
					}
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

				if(CmpString(Token, "NumNormals",LastTokenSize))
				{
					cout << "Script Reader: " << "NumNormals Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid number of Normals" << endl;
							return false;
						}

					MeshList[MeshIndex].NumNormals = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
if(LoadingNavMesh)
{
MeshList[MeshIndex].Normals = new MeshNormal[MeshList[MeshIndex].NumNormals]; // В буфере нормалей кроме  вершинных нормалей еще нормали фейсов
}else
{
MeshList[MeshIndex].Normals = new MeshNormal[MeshList[MeshIndex].NumNormals]; // В буфере нормалей кроме  вершинных нормалей еще нормали фейсов
}				
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

				if(CmpString(Token, "NumUVs",LastTokenSize))
				{
					cout << "Script Reader: " << "NumUVs Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid number of UVs" << endl;
							return false;
						}

					MeshList[MeshIndex].NumUVs = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					MeshList[MeshIndex].MeshUVs = new MeshUV[MeshList[MeshIndex].NumUVs];
					if(MeshList[MeshIndex].NumUVs>1) 
					{
						MessageBoxA(0, "Error:  Mesh has more than one uvset.  GameEngine does not support more than one uv set",&MeshList[MeshIndex].name[0],MB_OK);
						return false;
					}
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

				if(CmpString(Token, "UVset",LastTokenSize))
				{
					
					cout << "Script Reader: " << "UVset Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid UVset name" << endl;
							return false;
						}
					memcpy(MeshList[MeshIndex].MeshUVs[UvsCounter].UvMapName, Token, LastTokenSize);
					MeshList[MeshIndex].MeshUVs[UvsCounter].UvMapName[LastTokenSize] = 0;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid UVset size" << endl;
							return false;
						}
					MeshList[MeshIndex].MeshUVs[UvsCounter].UvSize = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					MeshList[MeshIndex].MeshUVs[UvsCounter].Uvs = new Uv[MeshList[MeshIndex].MeshUVs[UvsCounter].UvSize];
					
					//Mesh->Mesh.NumUVs = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					//Mesh->Mesh.MeshUVs = new MeshUV[Mesh->Mesh.NumUVs];
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					UvsCounter++;
					
					continue;
				}

				if(CmpString(Token, "NumShaders",LastTokenSize))
				{
					cout << "Script Reader: " << "NumShaders Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid number of Shaders" << endl;
							return false;
						}

					MeshList[MeshIndex].NumShadersUsedForThisMesh = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					MeshList[MeshIndex].MeshShadingInfo = new ShadersForThisMesh[MeshList[MeshIndex].NumShadersUsedForThisMesh];
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					
					continue;
				}

			if(CmpString(Token, "NumInstances",LastTokenSize))
			{
				cout << "Script Reader: " << "NumInstances Detected" << endl;

				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid number of instances";// Invalid number of instances
					return false;
				}
				//memcpy(MeshInfo->name, Token, LastTokenSize);
				//MeshInfo->name[LastTokenSize] = 0;
				//MeshInfo->NameSize = LastTokenSize;
				if(LastTokenSize != 1)
				{
					// Неправильный формат файла   To do: Сделать вывод сообщения об этом
					cout <<"Invalid file format: mesh instances != 1";// Invalid number of instances
					return false;
				}
				if((ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum)) != 1)
				{
					// Неправильный формат файла   To do: Сделать вывод сообщения об этом
					cout <<"Invalid file format: mesh instances != 1";// Invalid number of instances
					return false;
				}
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				continue;
			}

			if(CmpString(Token, "Vertex",LastTokenSize)) //////   Загрузка вершины
			{
				cout << "Script Reader: " << "Vertex Detected " << VerticesLoaded << endl;
				
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Vertex";// Invalid number of instances
					return false;
				}
				
				NumVertex =  ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Vertex " << "Vertex_X num: " << NumVertex;
					return false;
				}
				if(LoadingNavMesh)
				{
					MeshList[MeshIndex].NavMeshVertices[NumVertex].ParrentIndex =  MESH_MAX_VERTICES;
					MeshList[MeshIndex].NavMeshVertices[NumVertex].VertexStatus = 0;
					MeshList[MeshIndex].NavMeshVertices[NumVertex].ThisTimeCalculated = 0;
					MeshList[MeshIndex].NavMeshVertices[NumVertex].VertexX = ConvertTextToNumberFloat(Token,LastTokenSize);
				}else
				{
					MeshList[MeshIndex].MeshVertices[NumVertex].VertexX = ConvertTextToNumberFloat(Token,LastTokenSize);
				}
				
				

				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Vertex " << "Vertex_Y num: " << NumVertex;
					return false;
				}

				if(LoadingNavMesh)
				{
					MeshList[MeshIndex].NavMeshVertices[NumVertex].VertexY = ConvertTextToNumberFloat(Token,LastTokenSize);

				}else
				{
					MeshList[MeshIndex].MeshVertices[NumVertex].VertexY = ConvertTextToNumberFloat(Token,LastTokenSize);

				}
				

				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Vertex " << "Vertex_Z num: " << NumVertex;
					return false;
				}
				if(LoadingNavMesh)
				{
					MeshList[MeshIndex].NavMeshVertices[NumVertex].VertexZ = ConvertTextToNumberFloat(Token,LastTokenSize);

				}else
				{
					MeshList[MeshIndex].MeshVertices[NumVertex].VertexZ = -ConvertTextToNumberFloat(Token,LastTokenSize);  // Инвертируем Z для того чтоб перевести координатную систему из Maya в DirectX LH

				}

				if(MeshList[MeshIndex].ConnectedToBone)
				{
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Vertex " << "Vertex Weights StartIndex: " << NumVertex;
					return false;
				}
				if(LoadingNavMesh)
				{
					MeshList[MeshIndex].NavMeshVertices[NumVertex].StartIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				}else
				{
					MeshList[MeshIndex].MeshVertices[NumVertex].StartIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				}
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Vertex " << "Vertex weights count: " << NumVertex;
					return false;
				}
				if(LoadingNavMesh)
				{
					MeshList[MeshIndex].NavMeshVertices[NumVertex].Count = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				}else
				{
					MeshList[MeshIndex].MeshVertices[NumVertex].Count = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				}
				
				}
				
				VerticesLoaded++; //  To do Удалить эту переменную   она только для статистики во время разработки


				//memcpy(MeshInfo->name, Token, LastTokenSize);
				//MeshInfo->name[LastTokenSize] = 0;
				//MeshInfo->NameSize = LastTokenSize;

				//if((ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum)) != 1)
				//{
					// Неправильный формат файла   To do: Сделать вывод сообщения об этом
				//	cout <<"Invalid file format: mesh instances != 1";// Invalid number of instances
				//	return;
				//}
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				continue;
			}

			// Нормалм

			if(CmpString(Token, "Normal",LastTokenSize)) //////   Загрузка вершины
			{
				
				cout << "Script Reader: " << "Normal Detected " << NumNormals << endl;
NumNormals++;
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Normal";// Invalid number of instances
					return false;
				}
				// На самом деле это NumNormal  Это чтоб не заводить переменную
				NumVertex  = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Normal " << "Normal_X num: " << NumVertex;
					return false;
				}
				MeshList[MeshIndex].Normals[NumVertex].NormalX = ConvertTextToNumberFloat(Token,LastTokenSize);
				

				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Normal " << "Normal_Y num: " << NumVertex;
					return false;
				}
				MeshList[MeshIndex].Normals[NumVertex].NormalY = ConvertTextToNumberFloat(Token,LastTokenSize);


				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Normal " << "Normal_Z num: " << NumVertex;
					return false;
				}
				MeshList[MeshIndex].Normals[NumVertex].NormalZ = ConvertTextToNumberFloat(Token,LastTokenSize);


				//VerticesLoaded++; //  To do Удалить эту переменную   она только для статистики во время разработки


				//memcpy(MeshInfo->name, Token, LastTokenSize);
				//MeshInfo->name[LastTokenSize] = 0;
				//MeshInfo->NameSize = LastTokenSize;

				//if((ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum)) != 1)
				//{
					// Неправильный формат файла   To do: Сделать вывод сообщения об этом
				//	cout <<"Invalid file format: mesh instances != 1";// Invalid number of instances
				//	return;
				//}
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				continue;
			}
			if(CmpString(Token, "Shader",LastTokenSize))
				{
					
					cout << "Script Reader: " << "Shader Detected" << endl;


					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "Script Reader: " << "Invalid Shader name" << endl;
							return false;
						}

					memcpy(MeshList[MeshIndex].MeshShadingInfo[TempShaderNumber].ShaderName, Token, LastTokenSize);
					MeshList[MeshIndex].MeshShadingInfo[TempShaderNumber].ShaderName[LastTokenSize] = 0;
					//Mesh->NameSize = LastTokenSize;


					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid ShaderIndex";
					return false;
				}
				
					MeshList[MeshIndex].MeshShadingInfo[TempShaderNumber].ShaderIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				


					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid pollys with this shader count";
					return false;
				}
				
					MeshList[MeshIndex].MeshShadingInfo[TempShaderNumber].NumPollysWithShader = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				

					
					
					TempShaderNumber++;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

			if(CmpString(Token, "Uv",LastTokenSize)) //////   Загрузка uv
			{
				cout << "Script Reader: " << "Uv Detected " << NumUV << endl;
NumUV++;
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Uv";// Invalid number of instances
					return false;
				}
				// Используем NumVertex как временную переменную
				NumVertex  = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= BIG_OR_SMALL_LETERS_AND_NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Uv Map Name";
					return false;
				}
				//Mesh->Mesh. ConvertTextToNumberFloat(Token,LastTokenSize);
				for(int j = 0; j != MeshList[MeshIndex].NumUVs; j++) // Проходим по всем именам мешей
					{
						//cout << "UV " << i << " " << Mesh.Mesh.Mesh.MeshUVs[i].Uvs[j].U << " " << Mesh.Mesh.Mesh.MeshUVs[i].Uvs[j].V <<endl;
						if(CmpString(Token, &MeshList[MeshIndex].MeshUVs[j].UvMapName[0], LastTokenSize))
						{

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!= NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
								cout <<"Invalid Uv coord U";
								return false;
							}
							MeshList[MeshIndex].MeshUVs[j].Uvs[NumVertex].U = ConvertTextToNumberFloat(Token,LastTokenSize);

							Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!= NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
								cout <<"Invalid Uvcoord V";
								return false;
							}
							MeshList[MeshIndex].MeshUVs[j].Uvs[NumVertex].V = ConvertTextToNumberFloat(Token,LastTokenSize);
						}
					
					}

				
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				continue;
			}

			if(CmpString(Token, "Polly",LastTokenSize)) //////   Загрузка поликов
			{
				
				cout << "Script Reader: " << "Polly Detected " << NumPolly << endl;
NumPolly++;

				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Polly";// Invalid number of instances
					return false;
				}
				// На самом деле это NumPolly  Это чтоб не заводить переменную
				NumVertex  = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				
				if(type!= NUMBERS)
				{
					// Файл поврежден   To do: Сделать вывод сообщения об этом
					cout <<"Invalid Polly VertexCount" << NumVertex;
					return false;
				}
				// Сохраняем количество вертексов
				PollyVertexCount = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				

				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!= NUMBERS)
						{
						// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout <<"Invalid Polly face index" << NumVertex;
							return false;
						}
						if(LoadingNavMesh)
						{
							MeshList[MeshIndex].NavMeshPolygons[NumVertex].FaceNormalIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
						
						}else
						{
							MeshList[MeshIndex].Polygons[NumVertex].FaceNormalIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
						}
				//if((PollyVertexCount==3))
				//{
					PollysLoadedWithNoError++;
					for(int i = 0; i!=PollyVertexCount; i++)
					{
						Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!= NUMBERS)
						{
						// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout <<"Invalid Polly VertexIndex" << NumVertex;
							return false;
						}

						if(LoadingNavMesh)
						{
							MeshList[MeshIndex].NavMeshPolygons[NumVertex].PollyVertices[i].VertexIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
						}else
						{
						MeshList[MeshIndex].Polygons[NumVertex].PollyVertices[i].VertexIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
						}
						Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
						if(type!= NUMBERS)
						{
						// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout <<"Invalid Polly NormalIndex" << NumVertex;
							return false;
						}

						if(LoadingNavMesh)
						{
							
							MeshList[MeshIndex].NavMeshPolygons[NumVertex].PollyVertices[i].NormalIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
						}else
						{
							MeshList[MeshIndex].Polygons[NumVertex].PollyVertices[i].NormalIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
						}
						
						Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!= NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
								cout <<"Invalid Polly UVIndex" << NumVertex;
							return false;
							}

							if(LoadingNavMesh)
							{
								MeshList[MeshIndex].NavMeshPolygons[NumVertex].PollyVertices[i].UVIndex=ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
							}
							else
							{
								MeshList[MeshIndex].Polygons[NumVertex].PollyVertices[i].UVIndex=ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
							}

							
						/* // убрали поддержку нескольких текстур
						Mesh->Polygons[NumVertex].Polly[i].UVIndex = new unsigned int[Mesh->NumUVs];
						for(int j = 0; j!=Mesh->NumUVs; j++)
						{
							Token = GetNextTokenSkipSpacebar(type);
							if(type!= NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
								cout <<"Invalid Polly UVIndex" << NumVertex << "J: " << j;
							return false;
							}
							Mesh->Polygons[NumVertex].Polly[i].UVIndex[j]=ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
						}
					*/

					}

				//}else
				//{
					//MessageBoxA(0,"Critical error:\n Mesh has more than 3 angle Pollys.  GameEngine does not support more than 3 angle pollys",(char *)&Mesh->name[0], MB_OK);
					//Mesh->NumPollys=PollysLoadedWithNoError; //Чтоб деструктор не удалял uvs
				//return false;
				//}

				

				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				continue;
			}

	
			// сюда попадаем если нет загрузчика
			Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			
			//OnlyForDebug++;
			continue;

		}

	}else
	{ // Файл поврежден   To do: Сделать вывод сообщения об этом
		cout << "MeshLoader: Unexpected token.  Expected OpenBracket";
	}

}
void ScriptReader::ShadersLoader(unsigned int MeshIndex)
{
int ShaderIndex;
int type;
int LastCurNum;
char * Token;
Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
if(type == OPEN_BRACKET) // Должен идти OPEN_BRACKET
{
		Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		while(type != CLOSE_BRACKET)
		{
			if(type == END_EOF)
			{
				cout << "ShadersLoader:Unexpected end of file";
				return;
			}

			if(CmpString(Token, "Lambert",LastTokenSize))
				{
					cout << "ShadersLoader: " << "Shader Lambert Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderIndex" << endl;
							return;
						}
					ShaderIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderName" << endl;
							return;
						}
					memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName, Token, LastTokenSize);
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName[LastTokenSize] = 0;
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorR" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorR = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorG" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorG = ConvertTextToNumberFloat(Token,LastTokenSize);


					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorB" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorB = ConvertTextToNumberFloat(Token,LastTokenSize);
					
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid Texture Name" << endl;
							return;
						}
					
					if(CmpString(Token, "NoTexture",LastTokenSize))
					{
						MeshList[MeshIndex].Shaders[ShaderIndex].Texture[0] = NULL;
					}else
					{
						memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].Texture, Token, LastTokenSize);
						MeshList[MeshIndex].Shaders[ShaderIndex].Texture[LastTokenSize] = 0;
					}
					
					//Mesh->NumNormals = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					//Mesh->Normals = new MeshNormal[Mesh->NumNormals];
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderType = SHADER_LAMBERT;
					//ShadersLoaded++;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

			if(CmpString(Token, "Blinn",LastTokenSize))
				{
					cout << "ShadersLoader: " << "Shader Blinn Detected" << endl;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderIndex" << endl;
							return;
						}
					ShaderIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderName" << endl;
							return;
						}
					memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName, Token, LastTokenSize);
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName[LastTokenSize] = 0;
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorR" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorR = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorG" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorG = ConvertTextToNumberFloat(Token,LastTokenSize);


					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorB" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorB = ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid Texture Name" << endl;
							return;
						}
					
					if(CmpString(Token, "NoTexture",LastTokenSize))
					{
						MeshList[MeshIndex].Shaders[ShaderIndex].Texture[0] = NULL;
					}
					memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].Texture, Token, LastTokenSize);
					MeshList[MeshIndex].Shaders[ShaderIndex].Texture[LastTokenSize] = 0;
					//Mesh->NumNormals = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					//Mesh->Normals = new MeshNormal[Mesh->NumNormals];
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderType = SHADER_BLINN;
					//ShadersLoaded++;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

						if(CmpString(Token, "Phong",LastTokenSize))
				{
					cout << "ShadersLoader: " << "Shader Phong Detected" << endl;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderIndex" << endl;
							return;
						}
					ShaderIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderName" << endl;
							return;
						}
					memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName, Token, LastTokenSize);
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName[LastTokenSize] = 0;
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorR" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorR = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorG" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorG = ConvertTextToNumberFloat(Token,LastTokenSize);


					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorB" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorB = ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid Texture Name" << endl;
							return;
						}
					
					if(CmpString(Token, "NoTexture",LastTokenSize))
					{
						MeshList[MeshIndex].Shaders[ShaderIndex].Texture[0] = NULL;
					}
					memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].Texture, Token, LastTokenSize);
					MeshList[MeshIndex].Shaders[ShaderIndex].Texture[LastTokenSize] = 0;
					//Mesh->NumNormals = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					//Mesh->Normals = new MeshNormal[Mesh->NumNormals];
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderType = SHADER_PHONG;
					//ShadersLoaded++;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
						}


						if(CmpString(Token, "PhongE",LastTokenSize))
				{
					cout << "ShadersLoader: " << "Shader Phong Detected" << endl;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderIndex" << endl;
							return;
						}
					ShaderIndex = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderName" << endl;
							return;
						}
					memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName, Token, LastTokenSize);
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderName[LastTokenSize] = 0;
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorR" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorR = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorG" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorG = ConvertTextToNumberFloat(Token,LastTokenSize);


					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid ShaderColorB" << endl;
							return;
						}
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderColor.ColorB = ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid Texture Name" << endl;
							return;
						}
					
					if(CmpString(Token, "NoTexture",LastTokenSize))
					{
						MeshList[MeshIndex].Shaders[ShaderIndex].Texture[0] = NULL;
					}
					memcpy(MeshList[MeshIndex].Shaders[ShaderIndex].Texture, Token, LastTokenSize);
					MeshList[MeshIndex].Shaders[ShaderIndex].Texture[LastTokenSize] = 0;
					//Mesh->NumNormals = ConvertTextToNumberInteger(Token,LastTokenSize, LastCurNum);
					//Mesh->Normals = new MeshNormal[Mesh->NumNormals];
					MeshList[MeshIndex].Shaders[ShaderIndex].ShaderType = SHADER_PHONGE;
					//ShadersLoaded++;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
						}
			// сюда попадаем если нет загрузчика
			Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			
			//OnlyForDebug++;
			continue;

		}

	}else
	{ // Файл поврежден   To do: Сделать вывод сообщения об этом
		cout << "ShadersLoader: Unexpected token.  Expected OpenBracket";
	}
}



void ScriptReader::WeightsLoader(unsigned int MeshIndex)
{
int Adress=-1;
int LastCurrIndexLoaded=-1;
int JointIndex;
int LastVertexIndexLoaded = -1;
int CurrentIndexLoadedTimes = 0;
int CurrIndexLoaded;
int LastCurNum;
int type;
char * Token;
Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
if(type == OPEN_BRACKET) // Должен идти OPEN_BRACKET
{
		Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		while(type != CLOSE_BRACKET)
		{
			if(type == END_EOF)
			{
				cout << "WeightsLoader:Unexpected end of file";
				return;
			}

			if(CmpString(Token, "Weight",LastTokenSize))
			{
					cout << "WeightLoader: " << "Weight Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid Weight" << endl;
							return;
						}
				// Загруженное число ничего нам не дает поэьлму не сохраняем его и загружаем следующие данные
			//	Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			//		if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
			//			{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
				//			cout << "WeightLoader: " << "Invalid MeshName" << endl;
				//			return;
				//		}
					// Загруженное имя меша ничего нам не дает поэтому не сохраняем его и загружаем следующие данные
					
					
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid VertexIndex" << endl;
							return;
					}
					CurrIndexLoaded =  ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
					if(LastVertexIndexLoaded!=CurrIndexLoaded)
					{
						LastVertexIndexLoaded = CurrIndexLoaded;
						CurrentIndexLoadedTimes = 0;
					}else
					{
						CurrentIndexLoadedTimes++;
					}

					if(LastCurrIndexLoaded != CurrIndexLoaded)
					{
						Adress++;
						LastCurrIndexLoaded = CurrIndexLoaded;
					}

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid JointName" << endl;
							return;
					}
					// Загруженное имя кости ничего нам не дает поэтому не сохраняем его и загружаем следующие данные

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid JointIndex" << endl;
							return;
					}
					JointIndex = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid Weight(float value)" << endl;
							return;
					}

				
					MeshList[MeshIndex].WeightsBuffer[(MeshList[MeshIndex].WeightsBlockSize*Adress)+CurrentIndexLoadedTimes].WeightForThisBone = ConvertTextToNumberFloat(Token,LastTokenSize);
					MeshList[MeshIndex].WeightsBuffer[(MeshList[MeshIndex].WeightsBlockSize*Adress)+CurrentIndexLoadedTimes].JointIndex = JointIndex;
					int adresss = (MeshList[MeshIndex].WeightsBlockSize*Adress)+CurrentIndexLoadedTimes;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid VertexPosX" << endl;
							return;
					}

					MeshList[MeshIndex].WeightsBuffer[(MeshList[MeshIndex].WeightsBlockSize*Adress)+CurrentIndexLoadedTimes].VertexPosX = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid VertexPosY" << endl;
							return;
					}

					MeshList[MeshIndex].WeightsBuffer[(MeshList[MeshIndex].WeightsBlockSize*Adress)+CurrentIndexLoadedTimes].VertexPosY = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightLoader: " << "Invalid VertexPosZ" << endl;
							return;
					}

					MeshList[MeshIndex].WeightsBuffer[(MeshList[MeshIndex].WeightsBlockSize*Adress)+CurrentIndexLoadedTimes].VertexPosZ = ConvertTextToNumberFloat(Token,LastTokenSize);

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

			

			// сюда попадаем если нет загрузчика
			Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			
			//OnlyForDebug++;
			continue;

		}

	}else
	{ // Файл поврежден   To do: Сделать вывод сообщения об этом
		cout << "WeightsLoader: Unexpected token.  Expected OpenBracket";
	}
}

void ScriptReader::HierarhyLoader(unsigned int AnimationIndex)
{
	Animation * AnimList = Mesh->GetAnimList();
int BoneIndex;
int LastCurNum;
int type;
char * Token;
Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
if(type == OPEN_BRACKET) // Должен идти OPEN_BRACKET
{
		Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		while(type != CLOSE_BRACKET)
		{
			if(type == END_EOF)
			{
				cout << "HierarhyLoader:Unexpected end of file";
				return;
			}

			if(CmpString(Token, "Bone",LastTokenSize))
			{
					cout << "HierarhyLoader: " << "Bone Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "HierarhyLoader: " << "Invalid BoneName" << endl;
							return;
						}
				// Загруженное имя ничего нам не дает поэтому не сохраняем его и загружаем следующие данные
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "HierarhyLoader: " << "Invalid BoneIndex" << endl;
							return;
						}
					BoneIndex =  ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// сначало проверим не является ли токен NoParrent
							if(CmpString(Token, "NoParrent",LastTokenSize))
							{
								AnimList[AnimationIndex].Hierarhy[BoneIndex].ParrentIndex=-1;
								Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
								continue;
							}else
							{

								Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
							if(type!=NUMBERS)
							{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "HierarhyLoader: " << "Invalid BoneParentIndex" << endl;
							return;
							}
							AnimList[AnimationIndex].Hierarhy[BoneIndex].ParrentIndex=ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
							
							}
							
						}
					
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

			

			// сюда попадаем если нет загрузчика
			Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			
			//OnlyForDebug++;
			continue;

		}

	}else
	{ // Файл поврежден   To do: Сделать вывод сообщения об этом
		cout << "HierarhyLoader: Unexpected token.  Expected OpenBracket";
	}
}
void ScriptReader::FramesLoader(unsigned int AnimationIndex)
{
Animation * AnimList = Mesh->GetAnimList();
int BonesForCurrentFrameLoaded=0;
int LoadingFrameNum;
int LoadingFrameTime;
int BoneIndex;
int LastCurNum;
int type;
char * Token;
Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
if(type == OPEN_BRACKET) // Должен идти OPEN_BRACKET
{
		Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		while(type != CLOSE_BRACKET)
		{
			if(type == END_EOF)
			{
				cout << "FramesLoader:Unexpected end of file";
				return;
			}

			if(CmpString(Token, "Bone",LastTokenSize))
			{
					cout << "FramesLoader: " << "FramesLoader Bone Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid BoneName" << endl;
							return;
						}
				// Загруженное имя ничего нам не дает поэтому не сохраняем его и загружаем следующие данные
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid BoneIndex" << endl;
							return;
						}
					BoneIndex =  ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid BonePosX" << endl;
							return;
						}
					AnimList[AnimationIndex].Frames[(LoadingFrameNum*AnimList[AnimationIndex].NumHierarhy)+BonesForCurrentFrameLoaded].posX=ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid BonePosY" << endl;
							return;
						}
					AnimList[AnimationIndex].Frames[(LoadingFrameNum*AnimList[AnimationIndex].NumHierarhy)+BonesForCurrentFrameLoaded].posY=ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid BonePosZ" << endl;
							return;
						}
					AnimList[AnimationIndex].Frames[(LoadingFrameNum*AnimList[AnimationIndex].NumHierarhy)+BonesForCurrentFrameLoaded].posZ=ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid orientX" << endl;
							return;
						}
					AnimList[AnimationIndex].Frames[(LoadingFrameNum*AnimList[AnimationIndex].NumHierarhy)+BonesForCurrentFrameLoaded].orientX=ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid orientY" << endl;
							return;
						}
					AnimList[AnimationIndex].Frames[(LoadingFrameNum*AnimList[AnimationIndex].NumHierarhy)+BonesForCurrentFrameLoaded].orientY=ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid orientZ" << endl;
							return;
						}
					AnimList[AnimationIndex].Frames[(LoadingFrameNum*AnimList[AnimationIndex].NumHierarhy)+BonesForCurrentFrameLoaded].orientZ=ConvertTextToNumberFloat(Token,LastTokenSize);
					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{ 
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid orientW" << endl;
							return;
						}
					AnimList[AnimationIndex].Frames[(LoadingFrameNum*AnimList[AnimationIndex].NumHierarhy)+BonesForCurrentFrameLoaded].orientW=ConvertTextToNumberFloat(Token,LastTokenSize);
					
					
					
					
					BonesForCurrentFrameLoaded++;
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

			if(CmpString(Token, "FrameNum",LastTokenSize))
			{
					cout << "FramesLoader: " << "FrameNum Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid FrameNum" << endl;
							return;
						}
				
					LoadingFrameNum = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
					
					
					

					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

			if(CmpString(Token, "FrameTime",LastTokenSize))
			{
					cout << "FramesLoader: " << "FrameTime Detected" << endl;

					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid FrameTime" << endl;
							return;
						}
				
					LoadingFrameTime = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
					AnimList[AnimationIndex].FrameTimes[LoadingFrameNum].FrameTim = (int)(((float)LoadingFrameTime/(float)ANIMATION_FPS)*1000.0f);
					
					

					
					Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

			

			// сюда попадаем если нет загрузчика
			Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			
			//OnlyForDebug++;
			continue;

		}

	}else
	{ // Файл поврежден   To do: Сделать вывод сообщения об этом
		cout << "FramesLoader: Unexpected token.  Expected OpenBracket";
	}
}
bool ScriptReader::ProcessMeshScript(unsigned int MeshIndex)
{
int NumFramesLoaded=0;
int type = 0;
char * Token;

int LastCurNum;
for(;;)
{
	Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
	if(type == 0)
	{
		break;//cout << "Uncnown type" << endl;

	}
	if(type==BIG_OR_SMALL_LETERS_AND_NUMBERS)
		//cout << "BIG_OR_SMALL_LETERS_AND_NUMBERS"<< endl;
	{
		
			if(CmpString(Token, "Mesh",LastTokenSize))
			{
				cout << "Script Reader: " << "MeshDetected " << endl;
				

				
				//if(!MeshList[MeshIndex].NumMeshes) // Если это первый мещ.
				//{
					//MeshList[MeshIndex].NumMeshes++; !!!!!!!!!
					//Mesh->Mesh.MeshIndexBuffer = NULL;
					MeshList[MeshIndex].MeshVertexBuffer = NULL;
					if(!MeshLoader(MeshIndex))
											{
											//	MeshList[MeshIndex].NumMeshes--;
												return false;
											}
				//}else
				//{
					//TempMesh  = &Mesh->Mesh; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					// Если не первый To do: Сделать загрузку остальных мешей
				//for(int i =0; i!= MeshList[MeshIndex].NumMeshes-1; i++)
					//{
					//	TempMesh = TempMesh->NextMesh;
					//}
//					TempMesh->NextMesh = new Meshes;
					//MeshList[MeshIndex].NumMeshes++;
					//TempMesh->NextMesh->MeshIndexBuffer = NULL;
					//MeshList[MeshIndex].MeshVertexBuffer = NULL;
					//if(!MeshLoader(MeshIndex, TempMesh->NextMesh))
					//{
					//	MeshList[MeshIndex].NumMeshes--;
					//	return false;
				//	}
					
					//}
			continue;
			}

		
			if(CmpString(Token, "Shaders",LastTokenSize))
			{
				cout << "Script Reader: " << "Shaders Detected " << endl;

				
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid NumShaders" << endl;
							return false;
						}
				MeshList[MeshIndex].NumShaders = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				MeshList[MeshIndex].Shaders = new ShaderData[MeshList[MeshIndex].NumShaders];


				ShadersLoader(MeshIndex);

				continue;
				}

			if(CmpString(Token, "Weights",LastTokenSize))
			{
				cout << "Script Reader: " << "Weights Detected " << endl;
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightsLoader: " << "Invalid NumWeights" << endl;
							return false;
						}
				MeshList[MeshIndex].WeightsBufferSize = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "WeightsLoader: " << "Invalid WeightsMemBlockSize" << endl;
							return false;
						}
				MeshList[MeshIndex].WeightsBlockSize = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				MeshList[MeshIndex].WeightsBufferSize *= MeshList[MeshIndex].WeightsBlockSize;
				MeshList[MeshIndex].WeightsBuffer = new Weight[MeshList[MeshIndex].WeightsBufferSize];
				
				//Mesh->NumShaders = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				//Mesh->Shaders = new ShaderData[Mesh->NumShaders];
				//ShadersLoader(Mesh);
				WeightsLoader(MeshIndex);
				continue;
				}
	}

	if(type == END_EOF)
		//cout << "END_EOF"<< endl;
	{
		
		
		// Загрузим текстуры для меша
		
		//for(int i =0; i!= Mesh->NumMeshes; i++)
			//	{
				//	if(i) TempMesh = TempMesh->NextMesh;
						//TempMesh->ShaderType = 
						// Сначало ищем Шейдер с таким именем
						for(int j =0; j!= MeshList[MeshIndex].NumShaders; j++)
						{
							//if(CmpString(&Mesh->Shaders[j].ShaderName[0], &TempMesh->ShaderName[0],GetStrSize(&Mesh->Shaders[j].ShaderName[0]), GetStrSize(&TempMesh->ShaderName[0])))
							//{
							//	TempMesh->ShaderType = Mesh->Shaders[j].ShaderType;
							//}
							if(MeshList[MeshIndex].Shaders[j].Texture[0])
							{
							if( FAILED( D3DXCreateTextureFromFileA( w3dDevice->g_pd3dDevice, MeshList[MeshIndex].Shaders[j].Texture, &MeshList[MeshIndex].Shaders[j].TextureHolder  ) ) )
						 {
								//wLog->addMessage(L"Error load texture %s",Mesh->Shaders[Mesh->Mesh.MeshShadingInfo[j].ShaderIndex].Texture);
							MessageBoxA(0,"Error loading texture",MeshList[MeshIndex].Shaders[j].Texture,MB_OK);
						}
							}
						}
						
					//}
		
		break;
		
	}
}

return true;
}
bool ScriptReader::ProcessAnimationScript(unsigned int AnimationIndex)
{
Animation * AnimList = Mesh->GetAnimList();
int NumFramesLoaded=0;
int type = 0;
char * Token;

int LastCurNum;
for(;;)
{
	Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
	if(type == 0)
	{
		break;//cout << "Uncnown type" << endl;

	}
	if(type==BIG_OR_SMALL_LETERS_AND_NUMBERS)
	{
				if(CmpString(Token, "Hierarhy",LastTokenSize))
				{
				cout << "Script Reader: " << "Hierarhy Detected "<<  endl;
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "ShadersLoader: " << "Invalid NumHierarhy" << endl;
							return false;
						}
				AnimList[AnimationIndex].NumHierarhy = ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				AnimList[AnimationIndex].Hierarhy = new BoneHierarhy[AnimList[AnimationIndex].NumHierarhy];
				HierarhyLoader(AnimationIndex);
				continue;
				}

				if(CmpString(Token, "Frame",LastTokenSize))
				{
				cout << "Script Reader: " << "Frame Detected " << endl;
				if(!NumFramesLoaded)
				{
				Token = GetNextTokenSkipSpacebar(ScriptData , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
				if(type!=NUMBERS)
						{
							// Файл поврежден   To do: Сделать вывод сообщения об этом
							cout << "FramesLoader: " << "Invalid NumFrames" << endl;
							return false;
						}

				AnimList[AnimationIndex].NumFrames =  ConvertTextToNumberInteger(Token,LastTokenSize,LastCurNum);
				AnimList[AnimationIndex].FrameTimes = new FrameTime[AnimList[AnimationIndex].NumFrames];
				AnimList[AnimationIndex].Frames = new Bone[AnimList[AnimationIndex].NumHierarhy*AnimList[AnimationIndex].NumFrames];
				AnimList[AnimationIndex].BaseFrame = new Bone[AnimList[AnimationIndex].NumHierarhy];
				}
				FramesLoader(AnimationIndex);
				NumFramesLoaded++;
				continue;
				}


	}

	if(type == END_EOF)
		//cout << "END_EOF"<< endl;
	{
		
		
		// Загрузим текстуры для меша
		
		//for(int i =0; i!= Mesh->NumMeshes; i++)
			//	{
				//	if(i) TempMesh = TempMesh->NextMesh;
						//TempMesh->ShaderType = 
						// Сначало ищем Шейдер с таким именем

						
					//}
		
		break;
		
	}
}

return true;
}
char * GetNextTokenSkipSpacebar(char * Data, int & type,int & LastTokenSize, int & LastTokenStartReadPos, int & LastTokenStart)
{
	char * ReturnData;
	do	
	{
		ReturnData = GetNextToken(type, Data, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		if(type == 0) return 0;
	}
	while((type == SPACEBAR_OR_TAB));

return ReturnData;
}

char * GetNextToken(int & type, char * ScriptData,int & LastTokenSize, int & LastTokenStartReadPos, int & LastTokenStart)
{
char * ReturnData = ScriptData;
LastTokenSize = Token(ScriptData,LastTokenStartReadPos+LastTokenStart+LastTokenSize, type, LastTokenStartReadPos, LastTokenStart);
ReturnData+= LastTokenStart+LastTokenStartReadPos;

return ReturnData;
}


int Token(char * Buffer, int  StartPos, int & type, int & LastTokenStartReadPos, int & LastTokenStart)
{
	LastTokenStartReadPos = StartPos;
	LastTokenStart = 0;
	bool wasDot = false;
	int size = 0;
	int ToExpect = 0;
	char * Token = Buffer+StartPos;
	type = 0;
	while (*Token)
	{
		if(*Token == 0)
		{ type = ToExpect;
			return size;
			break;
		}
		if((*Token == 10)||(*Token == 13))
		{

			switch(ToExpect)
			{
				case BIG_OR_SMALL_LETERS_AND_NUMBERS:
				type = ToExpect;
				return size;
				break;

				case  NUMBERS:
				type = ToExpect;
				return size;
				break;

				case  SPACEBAR_OR_TAB:
				type = ToExpect;
				return size;
				break;
			}

 			Token++;
			LastTokenStart++;
			continue;
		}
			if(!((*Token >= 'A' && *Token <= 'Z')||(*Token >= 'a' && *Token <= 'z')||(*Token >= '0' && *Token <= '9')||(*Token == ' ')||(*Token == '	')||(*Token == ';')||(*Token == '}')||(*Token == '{')||(*Token == '.')||(*Token == '-')||(*Token == '_')||(*Token == ':')||(*Token == '/'))) 	// Проверяем  разрешенный ли это стмвол
			{
					type = ToExpect;
					LastTokenStart = (unsigned int)Token;
					return -1;			
			}
		switch(ToExpect)
		{
			case BIG_OR_SMALL_LETERS_AND_NUMBERS:
				if((*Token >= 'A' && *Token <= 'Z')||(*Token >= 'a' && *Token <= 'z')||(*Token >= '0' && *Token <= '9')||(*Token == '_')||(*Token == ':')||(*Token == '/')||(*Token == '.'))
				{
					size++;
					Token++;
					  //Если это больщая буква
					continue;
				}
				type = ToExpect;
				return size;
			break;

			case  SPACEBAR_OR_TAB:
				if((*Token == ' ')||(*Token == '	'))
				{
					size++;
					Token++;
					  //Если это больщая буква
					continue;
				}
				type = ToExpect;
				return size;
			break;

			case  NUMBERS:
				if((*Token >= '0' && *Token <= '9') || (*Token == '.'))
				{
					if(*Token == '.')
					{
						if(wasDot)
						{
					type = 0;
					LastTokenStart = (unsigned int)Token;
					return -1; // в числе не может быть больше 1й точки
						}else
						{
							wasDot = true;
						}
					}

					size++;
					Token++;
					  //Если это больщая буква
					continue;
				}else
				{
					if(*Token == '-')
					{
						type = 0;
						LastTokenStart = (unsigned int)Token;
						return -1; // После минуса не число
					}
					
				}
				type = ToExpect;
				return size;
			break;

				//if(
		case 0: // Первая буква
			{
				if ((*Token >= 'A' && *Token <= 'Z')||(*Token >= 'a' && *Token <= 'z'))
				{
					 ToExpect = BIG_OR_SMALL_LETERS_AND_NUMBERS;
					 type = BIG_OR_SMALL_LETERS_AND_NUMBERS;
				}
				if ((*Token >= '0' && *Token <= '9')||(*Token == '-'))
				{
					 ToExpect = NUMBERS;
					 type =	NUMBERS;
				}
				
				if (*Token == ' ') 
				{
					ToExpect = SPACEBAR_OR_TAB; // Пробел
					type = SPACEBAR_OR_TAB;
				}
				if (*Token == '	') 
				{
					ToExpect = SPACEBAR_OR_TAB; // Табуляция
					type = SPACEBAR_OR_TAB;
				}
				if (*Token == '{') 
				{	type = 	OPEN_BRACKET;
					return 1;
				}
				if (*Token == '}')
				{	type = 	CLOSE_BRACKET;
					return 1;
				}
				if (*Token == ';')
				{	type = 	KOMA_DOT;
					return 1;
				}
				if (*Token == '.')
				{
					type = DOT;
					LastTokenStart = (unsigned int)Token;
					return -1;  //Точка не может быть первым символом				
				}
				size = 1;
				Token++;
				continue;
				break;
			}
		}
	}
if(type)
{ 
type = ToExpect;
}else
{
type = END_EOF;
}
return size;
}