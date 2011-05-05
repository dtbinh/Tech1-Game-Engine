/*
Copyright (C) 2011 Tech1 Game Engine.
*/


//При загрузке мешей инвертируем Z чтоб  перевести координатную систему из Maya в Directx LH
//При загрузке мешей инвертируем AABB Z чтоб  перевести координатную систему из Maya в Directx LH
#define _CRTDBG_MAPALLOC
#define _CRTDBG_MAP_ALLOC



#include <stdlib.h>
#include <crtdbg.h>
#include "Mesh.h"
#include "FileIO.h"

#include "Vector.h"
#include "Quaternion.h"
extern Console * CON;
extern gCamera * wCamera;
extern Shaders * GameShaders;
extern g3dDevice * w3dDevice;
cMesh * Mesh;
extern gTime * wTime;
extern RendererSceleton * RScel;
extern ScriptReader * ScriptLoader;

//OneMesh * MeshList; //!!!!! При изменении этой переменной изменить во всех cpp файлах extern'ы на эту переменную
//1. scriptreader.cpp
//2. objectpicking.cpp
//3. BinaryHeap
//4. gframe.cpp
//5. PathFind.cpp
//5. ShadowVolume.cpp
//6. ShadowVolume
//7. renderSceleton
//8. map.cpp
 OneMesh * cMesh::GetMeshes()
	{
		if(MeshList == 0)
		{		
			CON->ErrorMessage("GetMeshes: Meshes not allocated");
			return 0;
		}
		return MeshList; 
	}
bool cMesh::MeshAllocMemory(unsigned int NumMemory)
{
	if(MeshList)
	{
		CON->ErrorMessage("MeshAllocMemory: Unable to alloc memory. MeshList != NULL");
		return false;
	}
	MeshList = new OneMesh[NumMemory];
	return true;
}

bool cMesh::AnimAllocMemory(unsigned int NumMemory)
{
	if(AnimList)
	{
		CON->ErrorMessage("AnimAllocMemory: Unable to alloc memory. AnimList != NULL");
		return false;
	}
	AnimList = new Animation[NumMemory];
	return true;
}

Animation * cMesh::GetAnimList()
{
	if(AnimList == 0)
		{		
			CON->ErrorMessage("GetAnimList(): AnimList not allocated");
			return 0;
		}
		return AnimList;
}
void cMesh::SetAnimInfo(unsigned int AnimationIndex, DWORD SetStartPlayTime,bool loop)
{
	if(AnimList[AnimationIndex].NumFrames != 0)
	{
AnimList[AnimationIndex].StartPlayTime = SetStartPlayTime;
AnimList[AnimationIndex].LoopAnim = loop;
AnimList[AnimationIndex].AnimLengh = (AnimList[AnimationIndex].FrameTimes[AnimList[AnimationIndex].NumFrames-1].FrameTim - AnimList[AnimationIndex].FrameTimes[0].FrameTim); // Todo перенести в то место где расчитается 1 раз а не кажый раз...  + странный расчет.  Возможно не правильный. Проверить
if(AnimList[AnimationIndex].AnimLengh==0) AnimList[AnimationIndex].AnimLengh = 1;
	}
}

inline float Interpolate(float Start, float End, float InterpolateValue)
{
return Start + ((End - Start)*InterpolateValue);
}
//Смешивает фреймы
void cMesh::SetAnimBones(unsigned int MeshIndex, unsigned int AnimationIndex) // Проигрывает анимацию. Сохраняет результат в BaseFrame
{
	Quaternion * TempQuaternion;
	float Interpolation;
	int i;
	int adress;
	int TempAdress;
	int LastFrameAdress; // To do Дебажить время
	int CurFrameTime = ((wTime->time - AnimList[AnimationIndex].StartPlayTime) % AnimList[AnimationIndex].AnimLengh)+AnimList[AnimationIndex].FrameTimes[0].FrameTim;  // получается от 41 до 1000
	// Mesh.StartPlayTime  время когда загрузился меш - время начало проигрывания анимации
	// Mesh.AnimLengh - время когда заканчивается последний кадр минус время когда заканчивается первый кадр
	if(AnimList[AnimationIndex].NumFrames>1) // Если у нас больше одного фрейма
	{
	for(i = 1; i != AnimList[AnimationIndex].NumFrames; i++) // первый(нулевой) фрейм пропускаем. 
	{
		if( AnimList[AnimationIndex].FrameTimes[i].FrameTim  >= CurFrameTime )
		{
				 adress = AnimList[AnimationIndex].NumHierarhy*i;
				
				for ( int j = 0; j < AnimList[AnimationIndex].NumHierarhy; j++ )
					{
						Interpolation = (float)(CurFrameTime - AnimList[AnimationIndex].FrameTimes[i-1].FrameTim )/(float)(AnimList[AnimationIndex].FrameTimes[i].FrameTim-AnimList[AnimationIndex].FrameTimes[i-1].FrameTim); // при 41  ==0  при 1000 == 1,0
						TempAdress = adress+j;
						 LastFrameAdress = TempAdress - AnimList[AnimationIndex].NumHierarhy;
						 AnimList[AnimationIndex].BaseFrame[j].posX =Interpolate( AnimList[AnimationIndex].Frames[LastFrameAdress].posX,AnimList[AnimationIndex].Frames[TempAdress].posX,Interpolation);
						 AnimList[AnimationIndex].BaseFrame[j].posY = Interpolate( AnimList[AnimationIndex].Frames[LastFrameAdress].posY,AnimList[AnimationIndex].Frames[TempAdress].posY,Interpolation);
						 AnimList[AnimationIndex].BaseFrame[j].posZ = Interpolate( AnimList[AnimationIndex].Frames[LastFrameAdress].posZ,AnimList[AnimationIndex].Frames[TempAdress].posZ,Interpolation);
						

						 TempQuaternion = (Quaternion *)& AnimList[AnimationIndex].BaseFrame[j].orientX;
						*TempQuaternion = QuaternionSLERP(*(Quaternion *)&AnimList[AnimationIndex].Frames[LastFrameAdress].orientX,*(Quaternion *)&AnimList[AnimationIndex].Frames[TempAdress].orientX , Interpolation);

						 /*
						 Mesh.BaseFrame[j].orientX = Mesh.Frames[TempAdress].orientX;
						 Mesh.BaseFrame[j].orientY = Mesh.Frames[TempAdress].orientY;
						 Mesh.BaseFrame[j].orientZ = Mesh.Frames[TempAdress].orientZ;
						 Mesh.BaseFrame[j].orientW = Mesh.Frames[TempAdress].orientW;
						*/
					}
				break;
			
			/*
			else
			{// Если это самый первый фрейм значит без интерполяции

					for ( int j = 0; j < Mesh.NumHierarhy; j++ )
					{
						 Mesh.BaseFrame[j].posX = Mesh.Frames[j].posX;
						 Mesh.BaseFrame[j].posY = Mesh.Frames[j].posY;
						 Mesh.BaseFrame[j].posZ = Mesh.Frames[j].posZ;
						 Mesh.BaseFrame[j].orientX = Mesh.Frames[j].orientX;
						 Mesh.BaseFrame[j].orientY = Mesh.Frames[j].orientY;
						 Mesh.BaseFrame[j].orientZ = Mesh.Frames[j].orientZ;
						 Mesh.BaseFrame[j].orientW = Mesh.Frames[j].orientW;

					}
					break;
			}
			*/
		}

	}
	}else // если у нас только 1 фрейм
	{
					for ( int j = 0; j < AnimList[AnimationIndex].NumHierarhy; j++ )
					{
						 AnimList[AnimationIndex].BaseFrame[j].posX = AnimList[AnimationIndex].Frames[j].posX;
						 AnimList[AnimationIndex].BaseFrame[j].posY = AnimList[AnimationIndex].Frames[j].posY;
						 AnimList[AnimationIndex].BaseFrame[j].posZ = AnimList[AnimationIndex].Frames[j].posZ;
						 AnimList[AnimationIndex].BaseFrame[j].orientX = AnimList[AnimationIndex].Frames[j].orientX;
						 AnimList[AnimationIndex].BaseFrame[j].orientY = AnimList[AnimationIndex].Frames[j].orientY;
						 AnimList[AnimationIndex].BaseFrame[j].orientZ = AnimList[AnimationIndex].Frames[j].orientZ;
						 AnimList[AnimationIndex].BaseFrame[j].orientW = AnimList[AnimationIndex].Frames[j].orientW;

					}
	}
}
//Смешивает фреймы
void cMesh::MergeAnimation(unsigned int FirstAnimationIndex, DWORD FirstAnimationStartPlayTime, unsigned int SecondAnimationIndex, DWORD SecondAnimationStartPlayTime, float MergeValue) // Смешивает анимацию. Сохраняет результат в BaseFrame Второй анимации
{
	Quaternion * TempQuaternion;
	float Interpolation;
	int i, j;
	int adress;
	int TempAdress;
	int LastFrameAdress; // To do Дебажить время
	int CurFrameTime = ((wTime->time - FirstAnimationStartPlayTime) % AnimList[FirstAnimationIndex].AnimLengh)+AnimList[FirstAnimationIndex].FrameTimes[0].FrameTim;  // получается от 41 до 1000
	// Mesh.StartPlayTime  время когда загрузился меш - время начало проигрывания анимации
	// Mesh.AnimLengh - время когда заканчивается последний кадр минус время когда заканчивается первый кадр
	if(AnimList[FirstAnimationIndex].NumFrames>1) // Если у нас больше одного фрейма
	{
	for(i = 1; i != AnimList[FirstAnimationIndex].NumFrames; i++) // первый(нулевой) фрейм пропускаем. 
	{
		if( AnimList[FirstAnimationIndex].FrameTimes[i].FrameTim  >= CurFrameTime )
		{
				 adress = AnimList[FirstAnimationIndex].NumHierarhy*i;
				
				for ( j = 0; j < AnimList[FirstAnimationIndex].NumHierarhy; j++ )
					{
						Interpolation = (float)(CurFrameTime - AnimList[FirstAnimationIndex].FrameTimes[i-1].FrameTim )/(float)(AnimList[FirstAnimationIndex].FrameTimes[i].FrameTim-AnimList[FirstAnimationIndex].FrameTimes[i-1].FrameTim); // при 41  ==0  при 1000 == 1,0
						TempAdress = adress+j;
						 LastFrameAdress = TempAdress - AnimList[FirstAnimationIndex].NumHierarhy;
						 AnimList[FirstAnimationIndex].BaseFrame[j].posX =Interpolate( AnimList[FirstAnimationIndex].Frames[LastFrameAdress].posX,AnimList[FirstAnimationIndex].Frames[TempAdress].posX,Interpolation);
						 AnimList[FirstAnimationIndex].BaseFrame[j].posY = Interpolate( AnimList[FirstAnimationIndex].Frames[LastFrameAdress].posY,AnimList[FirstAnimationIndex].Frames[TempAdress].posY,Interpolation);
						 AnimList[FirstAnimationIndex].BaseFrame[j].posZ = Interpolate( AnimList[FirstAnimationIndex].Frames[LastFrameAdress].posZ,AnimList[FirstAnimationIndex].Frames[TempAdress].posZ,Interpolation);
						

						 TempQuaternion = (Quaternion *)& AnimList[FirstAnimationIndex].BaseFrame[j].orientX;
						*TempQuaternion = QuaternionSLERP(*(Quaternion *)&AnimList[FirstAnimationIndex].Frames[LastFrameAdress].orientX,*(Quaternion *)&AnimList[FirstAnimationIndex].Frames[TempAdress].orientX , Interpolation);

						 /*
						 Mesh.BaseFrame[j].orientX = Mesh.Frames[TempAdress].orientX;
						 Mesh.BaseFrame[j].orientY = Mesh.Frames[TempAdress].orientY;
						 Mesh.BaseFrame[j].orientZ = Mesh.Frames[TempAdress].orientZ;
						 Mesh.BaseFrame[j].orientW = Mesh.Frames[TempAdress].orientW;
						*/
					}
				break;
			
			/*
			else
			{// Если это самый первый фрейм значит без интерполяции

					for ( int j = 0; j < Mesh.NumHierarhy; j++ )
					{
						 Mesh.BaseFrame[j].posX = Mesh.Frames[j].posX;
						 Mesh.BaseFrame[j].posY = Mesh.Frames[j].posY;
						 Mesh.BaseFrame[j].posZ = Mesh.Frames[j].posZ;
						 Mesh.BaseFrame[j].orientX = Mesh.Frames[j].orientX;
						 Mesh.BaseFrame[j].orientY = Mesh.Frames[j].orientY;
						 Mesh.BaseFrame[j].orientZ = Mesh.Frames[j].orientZ;
						 Mesh.BaseFrame[j].orientW = Mesh.Frames[j].orientW;

					}
					break;
			}
			*/
		}

	}
	}else // если у нас только 1 фрейм
	{
					for ( j = 0; j < AnimList[FirstAnimationIndex].NumHierarhy; j++ )
					{
						 AnimList[FirstAnimationIndex].BaseFrame[j].posX = AnimList[FirstAnimationIndex].Frames[j].posX;
						 AnimList[FirstAnimationIndex].BaseFrame[j].posY = AnimList[FirstAnimationIndex].Frames[j].posY;
						 AnimList[FirstAnimationIndex].BaseFrame[j].posZ = AnimList[FirstAnimationIndex].Frames[j].posZ;
						 AnimList[FirstAnimationIndex].BaseFrame[j].orientX = AnimList[FirstAnimationIndex].Frames[j].orientX;
						 AnimList[FirstAnimationIndex].BaseFrame[j].orientY = AnimList[FirstAnimationIndex].Frames[j].orientY;
						 AnimList[FirstAnimationIndex].BaseFrame[j].orientZ = AnimList[FirstAnimationIndex].Frames[j].orientZ;
						 AnimList[FirstAnimationIndex].BaseFrame[j].orientW = AnimList[FirstAnimationIndex].Frames[j].orientW;

					}
	}



	CurFrameTime = ((wTime->time - SecondAnimationStartPlayTime) % AnimList[SecondAnimationIndex].AnimLengh)+AnimList[SecondAnimationIndex].FrameTimes[0].FrameTim;  // получается от 41 до 1000
	// Mesh.StartPlayTime  время когда загрузился меш - время начало проигрывания анимации
	// Mesh.AnimLengh - время когда заканчивается последний кадр минус время когда заканчивается первый кадр
	if(AnimList[SecondAnimationIndex].NumFrames>1) // Если у нас больше одного фрейма
	{
	for(i = 1; i != AnimList[SecondAnimationIndex].NumFrames; i++) // первый(нулевой) фрейм пропускаем. 
	{
		if( AnimList[SecondAnimationIndex].FrameTimes[i].FrameTim  >= CurFrameTime )
		{
				 adress = AnimList[SecondAnimationIndex].NumHierarhy*i;
				
				for ( j = 0; j < AnimList[SecondAnimationIndex].NumHierarhy; j++ )
					{
						Interpolation = (float)(CurFrameTime - AnimList[SecondAnimationIndex].FrameTimes[i-1].FrameTim )/(float)(AnimList[SecondAnimationIndex].FrameTimes[i].FrameTim-AnimList[SecondAnimationIndex].FrameTimes[i-1].FrameTim); // при 41  ==0  при 1000 == 1,0
						TempAdress = adress+j;
						 LastFrameAdress = TempAdress - AnimList[SecondAnimationIndex].NumHierarhy;
						 AnimList[SecondAnimationIndex].BaseFrame[j].posX =Interpolate( AnimList[SecondAnimationIndex].Frames[LastFrameAdress].posX,AnimList[SecondAnimationIndex].Frames[TempAdress].posX,Interpolation);
						 AnimList[SecondAnimationIndex].BaseFrame[j].posY = Interpolate( AnimList[SecondAnimationIndex].Frames[LastFrameAdress].posY,AnimList[SecondAnimationIndex].Frames[TempAdress].posY,Interpolation);
						 AnimList[SecondAnimationIndex].BaseFrame[j].posZ = Interpolate( AnimList[SecondAnimationIndex].Frames[LastFrameAdress].posZ,AnimList[SecondAnimationIndex].Frames[TempAdress].posZ,Interpolation);
						

						 TempQuaternion = (Quaternion *)& AnimList[SecondAnimationIndex].BaseFrame[j].orientX;
						*TempQuaternion = QuaternionSLERP(*(Quaternion *)&AnimList[SecondAnimationIndex].Frames[LastFrameAdress].orientX,*(Quaternion *)&AnimList[SecondAnimationIndex].Frames[TempAdress].orientX , Interpolation);

						 /*
						 Mesh.BaseFrame[j].orientX = Mesh.Frames[TempAdress].orientX;
						 Mesh.BaseFrame[j].orientY = Mesh.Frames[TempAdress].orientY;
						 Mesh.BaseFrame[j].orientZ = Mesh.Frames[TempAdress].orientZ;
						 Mesh.BaseFrame[j].orientW = Mesh.Frames[TempAdress].orientW;
						*/
					}
				break;
			
			/*
			else
			{// Если это самый первый фрейм значит без интерполяции

					for ( int j = 0; j < Mesh.NumHierarhy; j++ )
					{
						 Mesh.BaseFrame[j].posX = Mesh.Frames[j].posX;
						 Mesh.BaseFrame[j].posY = Mesh.Frames[j].posY;
						 Mesh.BaseFrame[j].posZ = Mesh.Frames[j].posZ;
						 Mesh.BaseFrame[j].orientX = Mesh.Frames[j].orientX;
						 Mesh.BaseFrame[j].orientY = Mesh.Frames[j].orientY;
						 Mesh.BaseFrame[j].orientZ = Mesh.Frames[j].orientZ;
						 Mesh.BaseFrame[j].orientW = Mesh.Frames[j].orientW;

					}
					break;
			}
			*/
		}

	}
	}else // если у нас только 1 фрейм
	{
					for (  j = 0; j < AnimList[SecondAnimationIndex].NumHierarhy; j++ )
					{
						 AnimList[SecondAnimationIndex].BaseFrame[j].posX = AnimList[SecondAnimationIndex].Frames[j].posX;
						 AnimList[SecondAnimationIndex].BaseFrame[j].posY = AnimList[SecondAnimationIndex].Frames[j].posY;
						 AnimList[SecondAnimationIndex].BaseFrame[j].posZ = AnimList[SecondAnimationIndex].Frames[j].posZ;
						 AnimList[SecondAnimationIndex].BaseFrame[j].orientX = AnimList[SecondAnimationIndex].Frames[j].orientX;
						 AnimList[SecondAnimationIndex].BaseFrame[j].orientY = AnimList[SecondAnimationIndex].Frames[j].orientY;
						 AnimList[SecondAnimationIndex].BaseFrame[j].orientZ = AnimList[SecondAnimationIndex].Frames[j].orientZ;
						 AnimList[SecondAnimationIndex].BaseFrame[j].orientW = AnimList[SecondAnimationIndex].Frames[j].orientW;
					}
	}
						for (  j = 0; j < AnimList[FirstAnimationIndex].NumHierarhy; j++ )
					{
						AnimList[FirstAnimationIndex].BaseFrame[j].posX = Interpolate( AnimList[FirstAnimationIndex].BaseFrame[j].posX, AnimList[SecondAnimationIndex].BaseFrame[j].posX, MergeValue);
						AnimList[FirstAnimationIndex].BaseFrame[j].posY = Interpolate( AnimList[FirstAnimationIndex].BaseFrame[j].posY, AnimList[SecondAnimationIndex].BaseFrame[j].posY, MergeValue);
						AnimList[FirstAnimationIndex].BaseFrame[j].posZ = Interpolate( AnimList[FirstAnimationIndex].BaseFrame[j].posZ, AnimList[SecondAnimationIndex].BaseFrame[j].posZ, MergeValue);

						TempQuaternion = (Quaternion *)&AnimList[FirstAnimationIndex].BaseFrame[j].orientX;
						*TempQuaternion = QuaternionSLERP(*(Quaternion *)&AnimList[FirstAnimationIndex].Frames[j].orientX,*(Quaternion *)&AnimList[SecondAnimationIndex].Frames[j].orientX , MergeValue);

						TempQuaternion = (Quaternion *)&AnimList[FirstAnimationIndex].BaseFrame[j].orientY;
						*TempQuaternion = QuaternionSLERP(*(Quaternion *)&AnimList[FirstAnimationIndex].Frames[j].orientY,*(Quaternion *)&AnimList[SecondAnimationIndex].Frames[j].orientY , MergeValue);

						TempQuaternion = (Quaternion *)&AnimList[FirstAnimationIndex].BaseFrame[j].orientZ;
						*TempQuaternion = QuaternionSLERP(*(Quaternion *)&AnimList[FirstAnimationIndex].Frames[j].orientZ,*(Quaternion *)&AnimList[SecondAnimationIndex].Frames[j].orientZ , MergeValue);

						TempQuaternion = (Quaternion *)&AnimList[FirstAnimationIndex].BaseFrame[j].orientW;
						*TempQuaternion = QuaternionSLERP(*(Quaternion *)&AnimList[FirstAnimationIndex].Frames[j].orientW,*(Quaternion *)&AnimList[SecondAnimationIndex].Frames[j].orientW , MergeValue);
					}
}


void	cMesh::BuildJoints(unsigned int AnimationIndex)
{	
	Vector * TempVect;
	Quaternion * TempQuat;
    for(int i = 0; i < AnimList[AnimationIndex].NumHierarhy; i++)
        if(AnimList[AnimationIndex].Hierarhy[i].ParrentIndex != -1) // Root jointы не трогаем
        {	
			TempVect = (Vector *)&AnimList[AnimationIndex].BaseFrame[i].posX;
			*TempVect = QuaternionMultiplyVector((Quaternion *)&AnimList[AnimationIndex].BaseFrame[AnimList[AnimationIndex].Hierarhy[i].ParrentIndex].orientX, TempVect);


			TempVect->x += AnimList[AnimationIndex].BaseFrame[AnimList[AnimationIndex].Hierarhy[i].ParrentIndex].posX;
			TempVect->y += AnimList[AnimationIndex].BaseFrame[AnimList[AnimationIndex].Hierarhy[i].ParrentIndex].posY;
		    TempVect->z += AnimList[AnimationIndex].BaseFrame[AnimList[AnimationIndex].Hierarhy[i].ParrentIndex].posZ;
			TempQuat = (Quaternion *)&AnimList[AnimationIndex].BaseFrame[i].orientX;
			*TempQuat = QuaternionMultiply((Quaternion *)&AnimList[AnimationIndex].BaseFrame[AnimList[AnimationIndex].Hierarhy[i].ParrentIndex].orientX,TempQuat);


			/*
							{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq.txt", "a+" );
	sprintf(data,"i:%d pVertices.x %f pVertices.y %f pVertices.z %f %f\n", i, TempVect->x,TempVect->y,TempVect->z);
	fputs((const char *)&data,file);
	fclose(file);
	}
	*/

			//joints [i].pos    = joints [i].parent -> pos + 
            //                    joints [i].parent -> orient.rotate ( joints [i].pos );
           // joints [i].orient = joints [i].parent -> orient * joints [i].orient;
        }
}

void cMesh::ResetJoints(unsigned int AnimationIndex)
{
	for ( int i = 0; i < AnimList[AnimationIndex].NumHierarhy ; i++ )
    {
        AnimList[AnimationIndex].BaseFrame[i].posX			  = AnimList[AnimationIndex].Frames[i].posX;
		AnimList[AnimationIndex].BaseFrame[i].posY			  = AnimList[AnimationIndex].Frames[i].posY;
		AnimList[AnimationIndex].BaseFrame[i].posZ			  = AnimList[AnimationIndex].Frames[i].posZ;
        AnimList[AnimationIndex].BaseFrame[i].orientX        =  AnimList[AnimationIndex].Frames[i].orientX;
		AnimList[AnimationIndex].BaseFrame[i].orientY        =  AnimList[AnimationIndex].Frames[i].orientY;
		AnimList[AnimationIndex].BaseFrame[i].orientZ        =  AnimList[AnimationIndex].Frames[i].orientZ;
    }
}

void cMesh::setFrame (int no, unsigned int AnimationIndex)
{
   // if ( no < 0 )
     //   no = numFrames - 1;

    //if ( no >= numFrames )
     //   no = 0;

   // frame = no;

   //ResetJoints();
int adress = AnimList[AnimationIndex].NumHierarhy*no;
    for ( int i = 0; i < AnimList[AnimationIndex].NumHierarhy; i++ )
    {
        //int flags = hierarchy [i].flags;
      //  int pos   = hierarchy [i].startIndex;

        //if ( flags & 1 )
             AnimList[AnimationIndex].BaseFrame[i].posX = AnimList[AnimationIndex].Frames[adress+i].posX;

        //if ( flags & 2 )
             AnimList[AnimationIndex].BaseFrame[i].posY = AnimList[AnimationIndex].Frames[adress+i].posY;

        //if ( flags & 4 )
             AnimList[AnimationIndex].BaseFrame[i].posZ = AnimList[AnimationIndex].Frames[adress+i].posZ;

        //if ( flags & 8 )
            AnimList[AnimationIndex].BaseFrame[i].orientX = AnimList[AnimationIndex].Frames[adress+i].orientX;

        //if ( flags & 16 )
            AnimList[AnimationIndex].BaseFrame[i].orientY = AnimList[AnimationIndex].Frames[adress+i].orientY;

        //if ( flags & 32 )
            AnimList[AnimationIndex].BaseFrame[i].orientZ = AnimList[AnimationIndex].Frames[adress+i].orientZ;

			AnimList[AnimationIndex].BaseFrame[i].orientW = AnimList[AnimationIndex].Frames[adress+i].orientW;
        //renormalize ( joints [i].orient );
    }
	
    //buildJoints (); //!!!!!!!!!!!!!!!!!  Если анимация будет плохо работать то подумать над этой строкой
}

cMesh::cMesh() 
{
MeshList = 0;
AnimList = 0;
NumMeshesLoaded = 0;
}

unsigned int cMesh::GetNumMeshesLoaded()
{
return NumMeshesLoaded;
}
void cMesh::LoadAnimationFromFile( char * AnimFilePath, unsigned int AnimationIndex)
{
ErrorLoadingMesh = false;
int FileSize = GetFileSizez(AnimFilePath);
if(!FileSize) ErrorLoadingMesh = true;
Files File;
File.OpenFile(AnimFilePath, "rb");
MeshFileData = new char[FileSize+1];
File.ReadFile(MeshFileData, FileSize);
MeshFileData[FileSize]=0;
File.CloseFile();
ScriptLoader->LoadAnimation(AnimationIndex, MeshFileData);

if(ScriptLoader->ErrorLoadingScript) ErrorLoadingMesh = true;

if(!ErrorLoadingMesh)
{

}
SetAnimInfo(AnimationIndex,wTime->time,true);

}
void cMesh::LoadMeshFromFile( char * MeshFilePath, bool NMesh)// NMesh = true если это NavMesh.  False если обычный меш
{

MeshList[NumMeshesLoaded].NavMesh = NMesh;
//xxx	Mesh.Mesh.BaseFrame = NULL;
		
	
	MeshList[NumMeshesLoaded].MeshVertexBuffer = NULL;
	//Mesh.Mesh.MeshIndexBuffer = NULL;
	MeshList[NumMeshesLoaded].WeightsBufferSize = 0;
MeshList[NumMeshesLoaded].NumShaders = 0;
MeshList[NumMeshesLoaded].MeshUVs = NULL;
MeshList[NumMeshesLoaded].AdjencyBuffer = NULL;
MeshList[NumMeshesLoaded].MeshVertices = NULL; //  Обратить внимание на это место когда будем делать загрузку мешей по отдельности из файлов
MeshList[NumMeshesLoaded].NavMeshVertices = NULL;
//MeshList[MeshIndex].NumMeshes = 0;
MeshList[NumMeshesLoaded].Normals = NULL;
//Mesh.Mesh.NextMesh = NULL;
ErrorLoadingMesh = false;
int FileSize = GetFileSizez(MeshFilePath);
if(!FileSize) ErrorLoadingMesh = true;
Files File;
File.OpenFile(MeshFilePath, "rb");
MeshFileData = new char[FileSize+1];
File.ReadFile(MeshFileData, FileSize);
MeshFileData[FileSize]=0;
File.CloseFile();
ScriptLoader->LoadMesh(NumMeshesLoaded,MeshFileData, true, NMesh);

if( NMesh ) GenerateAdjencyForNavMesh(NumMeshesLoaded);

if(ScriptLoader->ErrorLoadingScript) ErrorLoadingMesh = true;

if(!ErrorLoadingMesh)
{

	int k=0;
	int vertexIndex;
	//int uvIndex;
	 MESH_CUSTOMVERTEX* pVertices;
//WORD* pIndex2;

//for(int i = 0; i!= MeshList[MeshIndex].NumMeshes;i++) // Удаляем всё от всех мешей
//{
	
k=0;
MeshList[NumMeshesLoaded].NumTrianglesInVertexBufferForVolume =0;
MeshList[NumMeshesLoaded].ComputedNormals = NULL;
MeshList[NumMeshesLoaded].NumTrianglesInVertexBufferForVolume = 0;
MeshList[NumMeshesLoaded].VertexBufferForShadowVolume = NULL;

w3dDevice->g_pd3dDevice->CreateVertexBuffer(MeshList[NumMeshesLoaded].Num3AnglePollys * 6 * sizeof( SHADOW_CUSTOMVERTEX ),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_SHADOW_CUSTOMVERTEX,
												  D3DPOOL_DEFAULT, &MeshList[NumMeshesLoaded].VertexBufferForShadowVolume, NULL );




//TempMesh->LightmapForThisMesh = new unsigned char[MeshList[MeshIndex].NumPollys * (DYNAMIC_LIGHTMAP_SIZE * DYNAMIC_LIGHTMAP_SIZE * 3)];
  MeshList[NumMeshesLoaded].MeshVertexBuffer->Lock( 0, MeshList[NumMeshesLoaded].NumVertices * sizeof( MESH_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	
	//TempMesh->MeshIndexBuffer->Lock(0,0,(void**)&pIndex2,0);
	if(MeshList[NumMeshesLoaded].NavMesh)
{
for(int i = 0; i!= MeshList[NumMeshesLoaded].Num4AnglePollys; i++)
	{  // Первый полигон
			vertexIndex = MeshList[0].NavMeshPolygons[i].PollyVertices[0].VertexIndex;
			pVertices[k].x =   MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;
pVertices[k+3].x =    MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k+3].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k+3].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;

//		pVertices[k].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[0].UVIndex].U;
//		pVertices[k].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[0].UVIndex].V;
		//pVertices[k+3].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[0].UVIndex].U;
		//pVertices[k+3].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[0].UVIndex].V;

			vertexIndex = MeshList[0].NavMeshPolygons[i].PollyVertices[1].VertexIndex;
			pVertices[k+1].x =    MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k+1].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k+1].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;
		//pVertices[k+1].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[1].UVIndex].U;
		//pVertices[k+1].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[1].UVIndex].V;
			

			vertexIndex = MeshList[0].NavMeshPolygons[i].PollyVertices[2].VertexIndex;
			pVertices[k+2].x =    MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k+2].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k+2].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;

			pVertices[k+4].x =    MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k+4].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k+4].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;

		//	pVertices[k+2].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[2].UVIndex].U;
		//pVertices[k+2].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[2].UVIndex].V;
		//pVertices[k+4].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[2].UVIndex].U;
		//pVertices[k+4].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[2].UVIndex].V;

			vertexIndex = MeshList[0].NavMeshPolygons[i].PollyVertices[3].VertexIndex;
			pVertices[k+5].x =    MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k+5].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k+5].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;
//pVertices[k+5].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[3].UVIndex].U;
//		pVertices[k+5].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].PollyVertices[3].UVIndex].V;

		k+=6;
	
	
	}
}else
{
for(int i = 0; i!= MeshList[NumMeshesLoaded].NumPollys; i++)
	{  // Первый полигон
		vertexIndex = MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[0].VertexIndex;
		//pIndex2[k]=k;
		if(NMesh)
		{
			pVertices[k+1].x =   MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k+1].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k+1].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;
		}else
		{
			pVertices[k+1].x =   MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexX;
			pVertices[k+1].y =   MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexY;
			pVertices[k+1].z =    MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexZ;
		}
		//uvIndex =;
		//pVertices[k].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].Polly[0].UVIndex].U;
		//pVertices[k].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].Polly[0].UVIndex].V;
		pVertices[k+1].tu = MeshList[NumMeshesLoaded].MeshUVs[0].Uvs[(int)MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[0].UVIndex].U;
		pVertices[k+1].tv = MeshList[NumMeshesLoaded].MeshUVs[0].Uvs[(int)MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[0].UVIndex].V;

		vertexIndex = MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[2].VertexIndex;
		//pIndex2[k+1] = k+1;

		if(NMesh)
		{
			pVertices[k+2].x =    MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k+2].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k+2].z =   MeshList[0].NavMeshVertices[vertexIndex].VertexZ;
		}else
		{
			pVertices[k+2].x =    MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexX;
			pVertices[k+2].y =    MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexY;
			pVertices[k+2].z =    MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexZ;
		}
		//uvIndex =;
		pVertices[k].tu = MeshList[NumMeshesLoaded].MeshUVs[0].Uvs[(int)MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[1].UVIndex].U;
		pVertices[k].tv = MeshList[NumMeshesLoaded].MeshUVs[0].Uvs[(int)MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[1].UVIndex].V;
		//pVertices[k+1].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].Polly[2].UVIndex].U;
		//pVertices[k+1].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].Polly[2].UVIndex].V;



		vertexIndex = MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[1].VertexIndex;
		//pIndex2[k+2] = k+2;
		if(NMesh)
		{
			pVertices[k].x =    MeshList[0].NavMeshVertices[vertexIndex].VertexX;
			pVertices[k].y =    MeshList[0].NavMeshVertices[vertexIndex].VertexY;
			pVertices[k].z =    MeshList[0].NavMeshVertices[vertexIndex].VertexZ;
		}else
		{
			pVertices[k].x =    MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexX;
			pVertices[k].y =    MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexY;
			pVertices[k].z =    MeshList[NumMeshesLoaded].MeshVertices[vertexIndex].VertexZ;
		}
		//uvIndex =;
		pVertices[k+2].tu = MeshList[NumMeshesLoaded].MeshUVs[0].Uvs[(int)MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[2].UVIndex].U;
		pVertices[k+2].tv = MeshList[NumMeshesLoaded].MeshUVs[0].Uvs[(int)MeshList[NumMeshesLoaded].Polygons[i].PollyVertices[2].UVIndex].V;
		//pVertices[k+2].tu = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].Polly[1].UVIndex].U;
		//pVertices[k+2].tv = TempMesh->MeshUVs[0].Uvs[(int)TempMesh->Polygons[i].Polly[1].UVIndex].V;
		
		k+=3;
	
	
	}
}
	
	
	

	MeshList[NumMeshesLoaded].MeshVertexBuffer->Unlock();
	//TempMesh->MeshIndexBuffer->Unlock();

//TempMesh->DirToLight = new Vert[TempMesh->Num3AnglePollys];
//TempMesh = TempMesh->NextMesh;
//}

}
//SetAnimInfo(NumMeshesLoaded,wTime->time,true);
//TempMesh = &Mesh.Mesh;

//TempMesh->Polygons[1].Polly[3].VertexIndex;
GenerateAdjency(NumMeshesLoaded);
NumMeshesLoaded++;
}

void cMesh::CalculateVertices(unsigned int MeshIndex, unsigned int AnimationIndex)
{

	Vector TempVector;
	Vector TempVec;

	//for(int i = 0; i!= MeshList[MeshIndex].NumMeshes;i++)
	//{
		if(MeshList[MeshIndex].ConnectedToBone)
		{
			for(int j = 0; j!= MeshList[MeshIndex].NumVertices;j++)
				{
					TempVector.x=0;
					TempVector.y=0;
					TempVector.z=0;
					if(MeshList[MeshIndex].NavMesh)
					{
							for(int f = 0; f != MeshList[MeshIndex].NavMeshVertices[j].Count; f++)
							{
								TempVec = QuaternionMultiplyVector((Quaternion *)&AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].JointIndex].orientX, (Vector *)&MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].VertexPosX);
								TempVector.x += (TempVec.x + AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].JointIndex].posX) *MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].WeightForThisBone;
								TempVector.y += (TempVec.y + AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].JointIndex].posY) *MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].WeightForThisBone;
								TempVector.z += (TempVec.z + AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].JointIndex].posZ) *MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].NavMeshVertices[j].StartIndex+f].WeightForThisBone;
							}
					}else
					{
							for(int f = 0; f != MeshList[MeshIndex].MeshVertices[j].Count; f++)
							{


							//	QuaternionMultiplyVector(, (Vector *)&MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].VertexPosX);



 								TempVec = QuaternionMultiplyVector((Quaternion *)&AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].JointIndex].orientX, (Vector *)&MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].VertexPosX);
/*
														{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data," pVertices
	
	
	
	.x %f pVertices.y %f pVertices.z %f\n", TempVec.x,TempVec.y,TempVec.z);
	fputs((const char *)&data,file);
	fclose(file);
	}
*/
								TempVector.x += (TempVec.x + AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].JointIndex].posX) *MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].WeightForThisBone;
								TempVector.y += (TempVec.y + AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].JointIndex].posY) *MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].WeightForThisBone;
								TempVector.z += (TempVec.z + AnimList[AnimationIndex].BaseFrame[MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].JointIndex].posZ) *MeshList[MeshIndex].WeightsBuffer[MeshList[MeshIndex].MeshVertices[j].StartIndex+f].WeightForThisBone;
							}
					}



							MeshList[MeshIndex].MeshVerticesForRendering[j].X = TempVector.x;
							MeshList[MeshIndex].MeshVerticesForRendering[j].Y = TempVector.y;
							MeshList[MeshIndex].MeshVerticesForRendering[j].Z = TempVector.z;
							

	

	
				}
		}
		//TempMesh = TempMesh->NextMesh;
	//}
}
/*
void cMesh::PrintJointCoordsToFile()
{
	    for ( int i = 0; i < Mesh.Mesh.NumHierarhy; i++ )
    {
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq999.txt", "a+" );
	sprintf(data," pVertices.x %f pVertices.y %f pVertices.z %f\n", MeshList[MeshIndex].BaseFrame[i].posX ,MeshList[MeshIndex].BaseFrame[i].posY,MeshList[MeshIndex].BaseFrame[i].posZ);
	fputs((const char *)&data,file);
	fclose(file);

		}
}
		/*
														{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data," pVertices.x %f pVertices.y %f pVertices.z %f\n", TempVec.x,TempVec.y,TempVec.z);
	fputs((const char *)&data,file);
	fclose(file);
	}
*/



void cMesh::ComputeNormalsForAnimatedMeshes(unsigned int MeshIndex)
{
D3DXVECTOR3 * TempVector1;
D3DXVECTOR3 * TempVector2;
D3DXVECTOR3 * TempVector3;
D3DXVECTOR3 * TempVector4;

D3DXVECTOR3  VectorOne;
D3DXVECTOR3  VectorTwo;

	//for(int i = 0; i!= MeshList[MeshIndex].NumMeshes;i++)
	//{
		if(MeshList[MeshIndex].ConnectedToBone)
		{
			for(int j = 0; j!= MeshList[MeshIndex].NumPollys ;j++)
			{
				TempVector4 = (D3DXVECTOR3 *)&MeshList[MeshIndex].ComputedNormals[j].NormalX;
				TempVector1 = (D3DXVECTOR3 *)&MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[j].PollyVertices[0].VertexIndex].X;
				TempVector2 = (D3DXVECTOR3 *)&MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[j].PollyVertices[1].VertexIndex].X;
				TempVector3 = (D3DXVECTOR3 *)&MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[j].PollyVertices[2].VertexIndex].X;
				VectorOne = *TempVector2 - *TempVector1; 
				VectorTwo = *TempVector3 - *TempVector1; 
				D3DXVec3Cross(TempVector4, &VectorOne, &VectorTwo);
				D3DXVec3Normalize(TempVector4, TempVector4);
			}
		}
		//TempMesh = TempMesh->NextMesh;
	//}
}
void cMesh::RenderAnimatedMesh(unsigned int MeshIndex, unsigned int AnimationIndex)
{
	int PollysRendered;
	int ShaderNumber;
	if(AnimList[AnimationIndex].NumFrames != 0)
	{
		SetAnimBones(MeshIndex, AnimationIndex);
		//setFrame(3);
		BuildJoints(AnimationIndex);
		///////////////////////////////////////!!!!!!ComputeNormalsForAnimatedMeshes(MeshIndex);
	}
//
CalculateVertices(MeshIndex, AnimationIndex);

//PrintJointCoordsToFile();

//////////////////////////////////////////////////////CalculateVertices(MeshIndex);
Vector TempVector;
Vector TempVec;
if(!ErrorLoadingMesh)
{
	GameShaders->SetShader(PixelMeshRendering);
	GameShaders->SetShader(VertexMeshRendering);
	w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclarationMeshRendering);

	int k=0;
	int vertexIndex;
	int uvIndex;
	 MESH_CUSTOMVERTEX* pVertices;
//WORD* pIndex2;

//for(int i = 0; i!= MeshList[MeshIndex].NumMeshes;i++)
//{
if(!MeshList[MeshIndex].ConnectedToBone)
{
	/*
   TempMesh-> MeshVertexBuffer->Lock( 0, TempMesh->NumVertices * sizeof( MESH_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	for(int i = 0; i!= TempMesh->NumPollys; i++)
	{
		
		vertexIndex = TempMesh->Polygons[i].Polly[0].VertexIndex;

		pVertices[k].x =   TempMesh->MeshVertices[vertexIndex].VertexX;
		pVertices[k].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
		pVertices[k].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
	



		vertexIndex = TempMesh->Polygons[i].Polly[1].VertexIndex;
		pVertices[k+1].x =    TempMesh->MeshVertices[vertexIndex].VertexX;
		pVertices[k+1].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
		pVertices[k+1].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
		

		vertexIndex = TempMesh->Polygons[i].Polly[2].VertexIndex;
		pVertices[k+2].x =    TempMesh->MeshVertices[vertexIndex].VertexX;
		pVertices[k+2].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
		pVertices[k+2].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
		

		k+=3;
		
	}
	TempMesh->MeshVertexBuffer->Unlock();
	*/
	wCamera->SetWorldMatrix(&MeshList[MeshIndex].MeshMatrix);
	//wCamera->SetIdentityWorldMatrix(); // Раскоментировать верхнюю строку а эту удалить!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());
if(FAILED(w3dDevice->g_pd3dDevice->SetStreamSource( 0, MeshList[MeshIndex].MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) )))
{
	MessageBoxA(0,"Failed setstream source",&MeshList[MeshIndex].name[0],MB_OK);
}
w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
//w3dDevice->g_pd3dDevice->SetIndices(TempMesh->MeshIndexBuffer);
//w3dDevice->g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,TempMesh->NumPollys*3,0,TempMesh->NumPollys);
if(MeshList[MeshIndex].NavMesh)
{
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,MeshList[MeshIndex].Num4AnglePollys*2); //
}else
{
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,MeshList[MeshIndex].NumPollys);
}

//w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,TempMesh->NumPollys);
}else
	{

k=0;
	MeshList[MeshIndex]. MeshVertexBuffer->Lock( 0, MeshList[MeshIndex].NumVertices * sizeof( MESH_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	for(int i = 0; i!= MeshList[MeshIndex].NumPollys; i++)
	{
		pVertices[k].x =   MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].X;
		pVertices[k].y =   MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].Y;
		pVertices[k].z =   MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].Z;
		
		pVertices[k+1].x =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].X;
		pVertices[k+1].y =	MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].Y;
		pVertices[k+1].z =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].Z;

		pVertices[k+2].x =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].X;
		pVertices[k+2].y =	MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].Y;
		pVertices[k+2].z =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].Z;


/*
							{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\test.txt", "a+" );
	sprintf(data,"i:%d pVertices.x %f pVertices.y %f pVertices.z %f\n", k, TempVector.x,TempVector.y,TempVector.z);
	fputs((const char *)&data,file);
	fclose(file);
	}
	*/
		k+=3;
		
	}
	MeshList[MeshIndex].MeshVertexBuffer->Unlock();

	//wCamera->SetZeroMeshMatrix();
	wCamera->SetIdentityWorldMatrix();
GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());
w3dDevice->g_pd3dDevice->SetStreamSource( 0, MeshList[MeshIndex].MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) );
//w3dDevice->g_pd3dDevice->SetIndices(TempMesh->MeshIndexBuffer);
PollysRendered = 0;
if(MeshList[MeshIndex].NumShadersUsedForThisMesh)
{
for(ShaderNumber = 0; ShaderNumber!= MeshList[MeshIndex].NumShadersUsedForThisMesh; ShaderNumber++)
{
	

	w3dDevice->g_pd3dDevice->SetTexture(GameShaders->GetTextureIndexForShader(PixelMeshRendering, 0),MeshList[MeshIndex].Shaders[MeshList[MeshIndex].MeshShadingInfo[ShaderNumber].ShaderIndex].TextureHolder );
	w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, PollysRendered*3, MeshList[MeshIndex].MeshShadingInfo[ShaderNumber].NumPollysWithShader );
	
	PollysRendered+=MeshList[MeshIndex].MeshShadingInfo[ShaderNumber].NumPollysWithShader;
	
}
}else
{
	w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, MeshList[MeshIndex].Num3AnglePollys );
}
}



//TempMesh = TempMesh->NextMesh;
//}


	
	}
}



void cMesh::RenderNoShadersSet(unsigned int MeshIndex, D3DXMATRIX * ProjectionMatrix, D3DXMATRIX * texturemat, bool RentderToShadowMap)
{
	int PollysRendered;
	int ShaderNumber;

Vector TempVector;
Vector TempVec;


 D3DXMATRIXA16  texMatrix;


if(!ErrorLoadingMesh)
{
w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclarationMeshRendering);

	int k=0;
	int vertexIndex;
	int uvIndex;
	 MESH_CUSTOMVERTEX* pVertices;
D3DXMATRIXA16 WorldViewProjection;
/*
D3DXMATRIXA16 Inversed;
Inversed = MeshList[MeshIndex].MeshMatrix;
D3DXMatrixInverse(&Inversed,NULL,&Inversed);

D3DXMATRIXA16 WorldViewProjection2;
D3DXMatrixIdentity(&WorldViewProjection2);
D3DXMatrixMultiply(&WorldViewProjection2, &Inversed, ProjectionMatrix);
*/

D3DXMatrixIdentity(&WorldViewProjection);
D3DXMatrixMultiply(&WorldViewProjection, &MeshList[MeshIndex].MeshMatrix, ProjectionMatrix);
D3DXMatrixMultiply(&texMatrix, &MeshList[MeshIndex].MeshMatrix , texturemat);
if(RentderToShadowMap)
{
GameShaders->SetMatrixForShader(VertexPSSM, "g_mViewProj", &WorldViewProjection);
}else
{

	GameShaders->SetMatrixForShader(VertexPSSMFinal, "g_mViewProj", &WorldViewProjection);
	GameShaders->SetMatrixForShader(VertexPSSMFinal, "g_mShadowMap", &texMatrix);



  //_pEffect->SetVector("g_vLightDir",&_vLightDir);
  //_pEffect->SetVector("g_vLightColor",&_vLightDiffuse);
 // _pEffect->SetVector("g_vAmbient",&_vLightAmbient);
 // _pEffect->SetFloat("g_fShadowMapSize",(FLOAT)_iShadowMapSize);
}
//wCamera->SetWorldMatrix(&MeshList[MeshIndex].MeshMatrix);
//GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());
if(FAILED(w3dDevice->g_pd3dDevice->SetStreamSource( 0, MeshList[MeshIndex].MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) )))
{
	MessageBoxA(0,"Failed setstream source",&MeshList[MeshIndex].name[0],MB_OK);
}

w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,MeshList[MeshIndex].NumPollys);
}
}
void cMesh::Render(unsigned int MeshIndex, bool wireframe)
{
	int PollysRendered;
	int ShaderNumber;
	//if(MeshList[MeshIndex].NumFrames != 0)
	//{
		/////////////////////////////////////////////////////SetAnimBones(MeshIndex);
		//setFrame(3);
		//////////////////////////////////////////////////////BuildJoints(MeshIndex);
		//////////////////////////////////ComputeNormalsForAnimatedMeshes(MeshIndex);
	//}
//

//PrintJointCoordsToFile();

//////////////////////////////////////////////////////CalculateVertices(MeshIndex);
Vector TempVector;
Vector TempVec;
if(!ErrorLoadingMesh)
{

if(MeshIndex == 1)
{
GameShaders->SetShader(VertexMeshRendering);
GameShaders->SetShader(PixelMeshRendering2);

}else
{
GameShaders->SetShader(VertexMeshRendering);
GameShaders->SetShader(PixelMeshRendering);
w3dDevice->g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
}

w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclarationMeshRendering);

	int k=0;
	int vertexIndex;
	int uvIndex;
	 MESH_CUSTOMVERTEX* pVertices;
//WORD* pIndex2;

//for(int i = 0; i!= MeshList[MeshIndex].NumMeshes;i++)
//{
if(!MeshList[MeshIndex].ConnectedToBone)
{
	/*
   TempMesh-> MeshVertexBuffer->Lock( 0, TempMesh->NumVertices * sizeof( MESH_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	for(int i = 0; i!= TempMesh->NumPollys; i++)
	{
		
		vertexIndex = TempMesh->Polygons[i].Polly[0].VertexIndex;

		pVertices[k].x =   TempMesh->MeshVertices[vertexIndex].VertexX;
		pVertices[k].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
		pVertices[k].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
	



		vertexIndex = TempMesh->Polygons[i].Polly[1].VertexIndex;
		pVertices[k+1].x =    TempMesh->MeshVertices[vertexIndex].VertexX;
		pVertices[k+1].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
		pVertices[k+1].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
		

		vertexIndex = TempMesh->Polygons[i].Polly[2].VertexIndex;
		pVertices[k+2].x =    TempMesh->MeshVertices[vertexIndex].VertexX;
		pVertices[k+2].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
		pVertices[k+2].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
		

		k+=3;
		
	}
	TempMesh->MeshVertexBuffer->Unlock();
	*/
	wCamera->SetWorldMatrix(&MeshList[MeshIndex].MeshMatrix);
	//wCamera->SetIdentityWorldMatrix(); // Раскоментировать верхнюю строку а эту удалить!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());
if(FAILED(w3dDevice->g_pd3dDevice->SetStreamSource( 0, MeshList[MeshIndex].MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) )))
{
	MessageBoxA(0,"Failed setstream source",&MeshList[MeshIndex].name[0],MB_OK);
}
if(wireframe)
{
w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
}
else
{
w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}
//w3dDevice->g_pd3dDevice->SetIndices(TempMesh->MeshIndexBuffer);
//w3dDevice->g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,TempMesh->NumPollys*3,0,TempMesh->NumPollys);
if(MeshList[MeshIndex].NavMesh)
{
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,MeshList[MeshIndex].Num4AnglePollys*2); //
}else
{
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,MeshList[MeshIndex].NumPollys);
}

//w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,TempMesh->NumPollys);
}else
	{

k=0;
	MeshList[MeshIndex]. MeshVertexBuffer->Lock( 0, MeshList[MeshIndex].NumVertices * sizeof( MESH_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	for(int i = 0; i!= MeshList[MeshIndex].NumPollys; i++)
	{
		pVertices[k].x =   MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].X;
		pVertices[k].y =   MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].Y;
		pVertices[k].z =   MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].Z;
		
		pVertices[k+1].x =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].X;
		pVertices[k+1].y =	MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].Y;
		pVertices[k+1].z =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].Z;

		pVertices[k+2].x =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].X;
		pVertices[k+2].y =	MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].Y;
		pVertices[k+2].z =  MeshList[MeshIndex].MeshVerticesForRendering[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].Z;


/*
							{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\test.txt", "a+" );
	sprintf(data,"i:%d pVertices.x %f pVertices.y %f pVertices.z %f\n", k, TempVector.x,TempVector.y,TempVector.z);
	fputs((const char *)&data,file);
	fclose(file);
	}
	*/
		k+=3;
		
	}
	MeshList[MeshIndex].MeshVertexBuffer->Unlock();
if(wireframe)
{
w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
}
else
{
w3dDevice->g_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}
	//wCamera->SetZeroMeshMatrix();
	wCamera->SetIdentityWorldMatrix();
GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());
w3dDevice->g_pd3dDevice->SetStreamSource( 0, MeshList[MeshIndex].MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) );
//w3dDevice->g_pd3dDevice->SetIndices(TempMesh->MeshIndexBuffer);
PollysRendered = 0;
for(ShaderNumber = 0; ShaderNumber!= MeshList[MeshIndex].NumShadersUsedForThisMesh; ShaderNumber++)
{
	w3dDevice->g_pd3dDevice->SetTexture(GameShaders->GetTextureIndexForShader(PixelMeshRendering, 0),MeshList[MeshIndex].Shaders[MeshList[MeshIndex].MeshShadingInfo[ShaderNumber].ShaderIndex].TextureHolder );
	w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, PollysRendered*3, MeshList[MeshIndex].MeshShadingInfo[ShaderNumber].NumPollysWithShader );
	
	PollysRendered+=MeshList[MeshIndex].MeshShadingInfo[ShaderNumber].NumPollysWithShader;
	
}
}



//TempMesh = TempMesh->NextMesh;
//}


	
	}
	}
/*
void cMesh::RenderOneMesh()
{
	if(Mesh.Mesh.NumFrames != 0)
	{
		SetAnimBones();
		//setFrame(0);
		BuildJoints();
	}
//
ComputeNormalsForAnimatedMeshes();

//PrintJointCoordsToFile();

CalculateVertices();
Vector TempVector;
Vector TempVec;

w3dDevice->g_pd3dDevice->SetPixelShader(GameShaders->PixelShaderMeshRendering);
w3dDevice->g_pd3dDevice->SetVertexShader(GameShaders->VertexShaderMeshRendering);
w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclarationMeshRendering);

	int k=0;
	int vertexIndex;
	int uvIndex;
	 MESH_CUSTOMVERTEX* pVertices;
//WORD* pIndex2;
Meshes * TempMesh = &Mesh.Mesh;




	wCamera->SetWorldMatrix(&TempMesh->MeshMatrix);
GameShaders->MatHandle = GameShaders->VertexShaderMeshRenderingConstantTable->GetConstantByName(0, "mat_mvp");
GameShaders->VertexShaderMeshRenderingConstantTable->SetMatrix(w3dDevice->g_pd3dDevice, GameShaders->MatHandle, wCamera->GetWorldViewProjectionCombined());
if(FAILED(w3dDevice->g_pd3dDevice->SetStreamSource( 0, TempMesh->MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) )))
{
	MessageBoxA(0,"Failed setstream source",&TempMesh->name[0],MB_OK);
}
//w3dDevice->g_pd3dDevice->SetIndices(TempMesh->MeshIndexBuffer);
//w3dDevice->g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,TempMesh->NumPollys*3,0,TempMesh->NumPollys);
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,2);
	






	
	
	}

*/
/*
void cMesh::RenderOnlyFrontCulledPollys()
{

	
int NumPollysFrontCulled=0;
	if(Mesh.Mesh.NumFrames != 0)
	{
		SetAnimBones();
		//setFrame(0);
		BuildJoints();
	}
//
ComputeNormalsForAnimatedMeshes();
ProcessAllPollysToFindFrontCulledPollys();
//PrintJointCoordsToFile();

CalculateVertices();
Vector TempVector;
Vector TempVec;
if(!ErrorLoadingMesh)
{
w3dDevice->g_pd3dDevice->SetPixelShader(GameShaders->PixelShaderMeshRendering);
w3dDevice->g_pd3dDevice->SetVertexShader(GameShaders->VertexShaderMeshRendering);
w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclarationMeshRendering);

	int k=0;
	int vertexIndex;
	int uvIndex;
	 MESH_CUSTOMVERTEX* pVertices;
//WORD* pIndex2;
Meshes * TempMesh = &Mesh.Mesh;
for(int i = 0; i!= Mesh.Mesh.NumMeshes;i++)
{
if(!TempMesh->ConnectedToBone)
{
	
  // TempMesh-> MeshVertexBuffer->Lock( 0, TempMesh->NumVertices * sizeof( MESH_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	//for(int i = 0; i!= TempMesh->NumPollys; i++)
	//{
		
	//	vertexIndex = TempMesh->Polygons[i].Polly[0].VertexIndex;

	//	pVertices[k].x =   TempMesh->MeshVertices[vertexIndex].VertexX;
	//	pVertices[k].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
	//	pVertices[k].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
	



	//	vertexIndex = TempMesh->Polygons[i].Polly[1].VertexIndex;
	//	pVertices[k+1].x =    TempMesh->MeshVertices[vertexIndex].VertexX;
	//	pVertices[k+1].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
	//	pVertices[k+1].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
		

	//	vertexIndex = TempMesh->Polygons[i].Polly[2].VertexIndex;
	//	pVertices[k+2].x =    TempMesh->MeshVertices[vertexIndex].VertexX;
	//	pVertices[k+2].y =    TempMesh->MeshVertices[vertexIndex].VertexY;
	//	pVertices[k+2].z =    TempMesh->MeshVertices[vertexIndex].VertexZ;
		

	//	k+=3;
		
	//}
	//TempMesh->MeshVertexBuffer->Unlock();
	//

	wCamera->SetWorldMatrix(&TempMesh->MeshMatrix);
GameShaders->MatHandle = GameShaders->VertexShaderMeshRenderingConstantTable->GetConstantByName(0, "mat_mvp");
GameShaders->VertexShaderMeshRenderingConstantTable->SetMatrix(w3dDevice->g_pd3dDevice, GameShaders->MatHandle, wCamera->GetWorldViewProjectionCombined());
if(FAILED(w3dDevice->g_pd3dDevice->SetStreamSource( 0, TempMesh->MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) )))
{
	MessageBoxA(0,"Failed setstream source",&TempMesh->name[0],MB_OK);
}
//w3dDevice->g_pd3dDevice->SetIndices(TempMesh->MeshIndexBuffer);
//w3dDevice->g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,TempMesh->NumPollys*3,0,TempMesh->NumPollys);
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,TempMesh->NumPollys); //TempMesh->NumPollys
}else
	{

k=0;
	TempMesh-> MeshVertexBuffer->Lock( 0, TempMesh->NumVertices * sizeof( MESH_CUSTOMVERTEX ), ( void** )&pVertices, 0 );
	NumPollysFrontCulled=0;
	TempMesh->NumTrianglesInVertexBufferForVolume=0;
	GoThrowAllFrontCulledPollys(TempMesh->FirstFrontCulled,TempMesh);
	int PollyNum=TempMesh->FirstFrontCulled;
	while(PollyNum!=65535)
	{

		pVertices[k].x =   TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[0].VertexIndex].X;
		pVertices[k].y =   TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[0].VertexIndex].Y;
		pVertices[k].z =   TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[0].VertexIndex].Z;
		
		pVertices[k+1].x =  TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[1].VertexIndex].X;
		pVertices[k+1].y =	TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[1].VertexIndex].Y;
		pVertices[k+1].z =  TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[1].VertexIndex].Z;

		pVertices[k+2].x =  TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[2].VertexIndex].X;
		pVertices[k+2].y =	TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[2].VertexIndex].Y;
		pVertices[k+2].z =  TempMesh->MeshVerticesForRendering[TempMesh->Polygons[PollyNum].PollyVertices[2].VertexIndex].Z;
		NumPollysFrontCulled++;
		k+=3;
		
		PollyNum = TempMesh->Polygons[PollyNum].NextFrontCulled;
	}
	TempMesh->MeshVertexBuffer->Unlock();

//	wCamera->SetZeroMeshMatrix();
	//wCamera->SetZeroMeshMatrix();
wCamera->SetIdentityWorldMatrix();
GameShaders->MatHandle = GameShaders->VertexShaderMeshRenderingConstantTable->GetConstantByName(0, "mat_mvp");
GameShaders->VertexShaderMeshRenderingConstantTable->SetMatrix(w3dDevice->g_pd3dDevice, GameShaders->MatHandle, wCamera->GetWorldViewProjectionCombined());
w3dDevice->g_pd3dDevice->SetStreamSource( 0, TempMesh->MeshVertexBuffer, 0, sizeof( MESH_CUSTOMVERTEX ) );
//w3dDevice->g_pd3dDevice->SetIndices(TempMesh->MeshIndexBuffer);
w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, NumPollysFrontCulled );
	}



TempMesh = TempMesh->NextMesh;
}


	
	}
	}


*/

cMesh::~cMesh()
{
	/*
	if(MeshList)
	{
		delete [] MeshList;
		MeshList = 0;
	}
	*/
	/*Meshes * TempMesh = &Mesh.Mesh;
Meshes * LastMesh = NULL;
	int CounterDeletedMeshes=0;
	delete [] Mesh.Mesh.MeshShadingInfo;
if(MeshList[MeshIndex].BaseFrame) delete [] MeshList[MeshIndex].BaseFrame;
	if(Mesh.Mesh.NumFrames)
	{
//XXX		delete [] Mesh.Mesh.Frames;
	//xxx	delete [] Mesh.Mesh.FrameTimes;
	}
	if(Mesh.Mesh.NumHierarhy)
//XXX		delete [] Mesh.Mesh.Hierarhy;

	
	if(Mesh.Mesh.WeightsBufferSize)
//xxx		delete [] Mesh.Mesh.WeightsBuffer;
delete [] MeshFileData;
if(Mesh.Mesh.NumShaders)
//xxx delete [] Mesh.Mesh.Shaders;


for(int i = 0; i!= Mesh.Mesh.NumMeshes;i++) // Удаляем всё от всех мешей
{

	//if(TempMesh->VertexBufferForShadowVolume);
//	TempMesh->VertexBufferForShadowVolume->Release();
	if(TempMesh->ConnectedToBone)
	{
		if(TempMesh->ComputedNormals)
		delete [] TempMesh->ComputedNormals;

	}
	//if(TempMesh->LightmapForThisMesh)
	//	delete [] TempMesh->LightmapForThisMesh;
		//if(TempMesh->MeshIndexBuffer)
	//TempMesh->MeshIndexBuffer->Release();
	//TempMesh->MeshIndexBuffer = NULL;
	if(TempMesh->AdjencyBuffer)
		delete [] TempMesh->AdjencyBuffer;
		if(Mesh.Mesh.NavMesh)
	{
//		delete [] TempMesh->VertexAdjency;
	}

	if(TempMesh->MeshVertexBuffer)
	TempMesh->MeshVertexBuffer->Release();
	TempMesh->MeshVertexBuffer = NULL;

if(TempMesh->MeshVertices)
delete [] TempMesh->MeshVertices;
if(Mesh.Mesh.NavMesh) // Если эта переменная поменяет свое место тут тоже надо поправить
delete [] TempMesh->NavMeshVertices;

//delete [] TempMesh->DirToLight; //////////////////////  Подумать надо ли это убрать
if(TempMesh->ConnectedToBone)
{
delete [] TempMesh->MeshVerticesForRendering;
}
// To Do Сделать удаление вертексов от всех мешей, а не только первого
if(TempMesh->Normals)
delete [] TempMesh->Normals;
 // To Do Сделать удаление Нормалей от всех мешей, а не только первого
if(TempMesh->MeshUVs) // Если есть Uvs
for(int i = 0 ; i!=TempMesh->NumUVs; i++)
	{
		delete [] TempMesh->MeshUVs[i].Uvs;
	}
delete [] TempMesh->MeshUVs; // Перед удадением MeshUVs надо удалить его содержимое
//for(int i = 0 ; i!=TempMesh->NumPollys; i++)
	//{
		//for(int j = 0 ; j!=3 ; j++)
		//{
		//	delete [] TempMesh->Polygons[i].Polly[j].UVIndex;
		//}
		
	//}
if(Mesh.Mesh.NavMesh)
	{
		delete [] TempMesh->NavMeshPolygons; //Сделать удаление поликов от всех мешей а не только от первого.
}else
{
	delete [] TempMesh->Polygons; //Сделать удаление поликов от всех мешей а не только от первого.
}

LastMesh = TempMesh;
TempMesh = TempMesh->NextMesh;
if(CounterDeletedMeshes)  //Первый меш удалять не надо
delete LastMesh;

CounterDeletedMeshes++;
}
*/
}


#define NO_ADJENCY -1
void cMesh::GenerateAdjencyForMesh(unsigned int MeshIndex)
{
	int IntegerOnlyForDebugDeleteMe;
	int i;
	int Result;
	ListValues * TempValue;
	ListValues * LastValue;
	MeshList[MeshIndex].AdjencyBuffer = new AdjencyInfo[MeshList[MeshIndex].NumPollys];
	if(MeshList[MeshIndex].Num4AnglePollys)
	{
		CON->ErrorMessage("GenerateAdjencyForMesh: Error.  \"%s\" has 4 angle pollys", &MeshList[MeshIndex].name[0] );
		return;
	}
	for(i = 0; i!= MeshList[MeshIndex].Num3AnglePollys;i++)
		{
			MeshList[MeshIndex].AdjencyBuffer[i].Edge1 = NO_ADJENCY;
			MeshList[MeshIndex].AdjencyBuffer[i].Edge2 = NO_ADJENCY;
			MeshList[MeshIndex].AdjencyBuffer[i].Edge3 = NO_ADJENCY;
		}



	ConnectedListIntegers * PollyIndexes = new ConnectedListIntegers[MeshList[MeshIndex].NumVertices]; //Сюда для каждого вертекса запишем номер полигона к которому он принадлежит
	
	for(i = 0; i!= MeshList[MeshIndex].NumPollys;i++)
	{
		PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].AddValue(i);
		PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].AddValue(i);
		PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].AddValue(i);
	}

	for(i = 0; i!= MeshList[MeshIndex].NumVertices;i++)
	{
		IntegerOnlyForDebugDeleteMe = PollyIndexes[i].NumElements;
	}


	for(i = 0; i!= MeshList[MeshIndex].NumPollys;i++)
	{

		// Между 0 и 1
		TempValue = PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&MeshList[MeshIndex].name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					MeshList[MeshIndex].AdjencyBuffer[i].Edge1 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&MeshList[MeshIndex].name[0],MB_OK);
						}
				}
			}
			TempValue = TempValue->NextValue;
		}

// Между 1 и 2
		TempValue = PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[1].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&MeshList[MeshIndex].name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					MeshList[MeshIndex].AdjencyBuffer[i].Edge2 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&MeshList[MeshIndex].name[0],MB_OK);
						}
				}
			}
			TempValue = TempValue->NextValue;
		}

		// Между 2 и 3
		TempValue = PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[2].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = PollyIndexes[MeshList[MeshIndex].Polygons[i].PollyVertices[0].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&MeshList[MeshIndex].name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					MeshList[MeshIndex].AdjencyBuffer[i].Edge3 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&MeshList[MeshIndex].name[0],MB_OK);
						}
					}
			}
			TempValue = TempValue->NextValue;
		}
		//PollyIndexes[TempMesh->Polygons[i].Polly[1].VertexIndex].AddValue(i);
		//PollyIndexes[TempMesh->Polygons[i].Polly[2].VertexIndex].AddValue(i);
	}

for(i = 0; i!= MeshList[MeshIndex].NumVertices;i++)
{
	TempValue = PollyIndexes[i].List;
		for(int EnumElements = 0; EnumElements!=PollyIndexes[i].NumElements; EnumElements++)
		{
			LastValue = TempValue;
			TempValue = TempValue->NextValue;
			delete LastValue;
		}
}
//delete [] PollyIndexes;
//PollyIndexes = NULL;

	
}
/*
void cMesh::GenerateAdjencyForNavMesh(Meshes * TempMesh)
{
	int IntegerOnlyForDebugDeleteMe;
	int i;
	int Result;
	VListValues * TempValue;
	TempMesh->AdjencyBuffer = new AdjencyInfo[TempMesh->NumPollys];

		for(i = 0; i!= TempMesh->NumPollys;i++)
		{
			TempMesh->AdjencyBuffer[i].Edge1 = NO_ADJENCY;
			TempMesh->AdjencyBuffer[i].Edge2 = NO_ADJENCY;
			TempMesh->AdjencyBuffer[i].Edge3 = NO_ADJENCY;
		}



	TempMesh->VertexAdjency = new ConnectedListVertexAdjency[TempMesh->NumVertices]; //Сюда для каждого вертекса запишем номер полигона к которому он принадлежит
	
	for(i = 0; i!= TempMesh->NumPollys;i++)
	{
		TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[0].VertexIndex].AddValue(i,0);
		TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[1].VertexIndex].AddValue(i,1);
		TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[2].VertexIndex].AddValue(i,2);
	}
TempMesh->VertexAdjency[1].List[1].VertexNum;
	for(i = 0; i!= TempMesh->NumVertices;i++)
	{
		IntegerOnlyForDebugDeleteMe = TempMesh->VertexAdjency[i].NumElements;
	}


	for(i = 0; i!= TempMesh->NumPollys;i++)
	{

		// Между 0 и 1
		TempValue = TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[0].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[0].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[1].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&TempMesh->name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					TempMesh->AdjencyBuffer[i].Edge1 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&TempMesh->name[0],MB_OK);
						}
				}
			}
			TempValue = TempValue->NextValue;
		}

// Между 1 и 2
		TempValue = TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[1].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[1].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[2].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&TempMesh->name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					TempMesh->AdjencyBuffer[i].Edge2 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&TempMesh->name[0],MB_OK);
						}
				}
			}
			TempValue = TempValue->NextValue;
		}

		// Между 2 и 3
		TempValue = TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[2].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[2].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = TempMesh->VertexAdjency[TempMesh->Polygons[i].PollyVertices[0].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&TempMesh->name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					TempMesh->AdjencyBuffer[i].Edge3 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&TempMesh->name[0],MB_OK);
						}
					}
			}
			TempValue = TempValue->NextValue;
		}
		//PollyIndexes[TempMesh->Polygons[i].Polly[1].VertexIndex].AddValue(i);
		//PollyIndexes[TempMesh->Polygons[i].Polly[2].VertexIndex].AddValue(i);
	}



	
}
*/
void cMesh::GenerateAdjencyForNavMesh(unsigned int MeshIndex)
{
D3DXVECTOR3 VecDown =  D3DXVECTOR3(0, -1.0f, 0);
	//unsigned int debug;
	int i, j;
	int Result;
	VListValues * TempValue;
	VListValues * LastValue;
	MeshList[MeshIndex].AdjencyBuffer = new AdjencyInfo[MeshList[MeshIndex].Num4AnglePollys];
	MeshList[MeshIndex].PollygonEdgeNormals = new PollyEdgeNormals[MeshList[MeshIndex].Num4AnglePollys];
		for(i = 0; i!= MeshList[MeshIndex].Num4AnglePollys;i++)
		{
			MeshList[MeshIndex].AdjencyBuffer[i].Edge1 = NO_ADJENCY;
			MeshList[MeshIndex].AdjencyBuffer[i].Edge2 = NO_ADJENCY;
			MeshList[MeshIndex].AdjencyBuffer[i].Edge3 = NO_ADJENCY;
			MeshList[MeshIndex].AdjencyBuffer[i].Edge4 = NO_ADJENCY;
		}

	MeshList[MeshIndex].VertexAdjency = new ConnectedListVertexAdjency[MeshList[MeshIndex].NumVertices]; //Сюда для каждого вертекса запишем номер полигона к которому он принадлежит
	
	
	for(i = 0; i!= MeshList[MeshIndex].Num4AnglePollys;i++)
	{
		MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].AddValue(i,0);
		MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].AddValue(i,1);
		MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].AddValue(i,2);
		MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].AddValue(i,3);
	}
	//TempMesh->VertexAdjency[0].NumElements;
	 
VListValues * TempElement;
	for(i = 0; i!= MeshList[MeshIndex].NumVertices;i++)
	{
		MeshList[MeshIndex].VertexAdjency[i].Massive = new VListValuesNoNext[MeshList[MeshIndex].VertexAdjency[i].NumElements];
		TempElement = MeshList[MeshIndex].VertexAdjency[i].List;
		for(j = 0; j!= MeshList[MeshIndex].VertexAdjency[i].NumElements;j++)
		{
			MeshList[MeshIndex].VertexAdjency[i].Massive[j].IntegerValue = TempElement->IntegerValue;
			MeshList[MeshIndex].VertexAdjency[i].Massive[j].VertexNum = TempElement->VertexNum;

			TempElement=TempElement->NextValue;
		}
	}
//TempMesh->VertexAdjency[1].List[1].VertexNum;
//debug = TempMesh->VertexAdjency[1].Massive[2].VertexNum;



	for(i = 0; i!= MeshList[MeshIndex].Num4AnglePollys;i++)
	{
// Посчитаем нормали для граней фейса.  Это нужно для проверки пересечения луча с непроходимой областью.(не нужно. проверка пересечения работает без этих нормалей. но удалять не буду.  может пригодится где-нибудь)
D3DXVec3Cross(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge1Normal, &D3DXVECTOR3(MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].VertexX-MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].VertexX, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].VertexY - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].VertexY, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].VertexZ - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].VertexZ), &VecDown );
D3DXVec3Normalize(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge1Normal,&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge1Normal);

D3DXVec3Cross(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge2Normal, &D3DXVECTOR3(MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].VertexX-MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].VertexX, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].VertexY - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].VertexY, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].VertexZ - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].VertexZ), &VecDown );
D3DXVec3Normalize(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge2Normal,&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge2Normal);

D3DXVec3Cross(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge3Normal, &D3DXVECTOR3(MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].VertexX-MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].VertexX, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].VertexY - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].VertexY, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].VertexZ - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].VertexZ), &VecDown );
D3DXVec3Normalize(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge3Normal,&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge3Normal);

D3DXVec3Cross(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge4Normal, &D3DXVECTOR3(MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].VertexX-MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].VertexX, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].VertexY - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].VertexY, MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].VertexZ - MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].VertexZ), &VecDown );
D3DXVec3Normalize(&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge4Normal,&MeshList[MeshIndex].PollygonEdgeNormals[i].Edge4Normal);


		// Между 0 и 1
		TempValue = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&MeshList[MeshIndex].name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					MeshList[MeshIndex].AdjencyBuffer[i].Edge1 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&MeshList[MeshIndex].name[0],MB_OK);
						}
				}
			}
			TempValue = TempValue->NextValue;
		}

// Между 1 и 2
		TempValue = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[1].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&MeshList[MeshIndex].name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					MeshList[MeshIndex].AdjencyBuffer[i].Edge2 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&MeshList[MeshIndex].name[0],MB_OK);
						}
				}
			}
			TempValue = TempValue->NextValue;
		}

		// Между 2 и 3
		TempValue = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[2].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&MeshList[MeshIndex].name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					MeshList[MeshIndex].AdjencyBuffer[i].Edge3 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&MeshList[MeshIndex].name[0],MB_OK);
						}
					}
			}
			TempValue = TempValue->NextValue;
		}
		//PollyIndexes[TempMesh->Polygons[i].Polly[1].VertexIndex].AddValue(i);
		//PollyIndexes[TempMesh->Polygons[i].Polly[2].VertexIndex].AddValue(i);

		// Между 3 и 0
		TempValue = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].List;
		for(int EnumElements = 0; EnumElements!=MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[3].VertexIndex].NumElements; EnumElements++)
		{
			if(TempValue->IntegerValue != i) // пропускаем текущий полигон.  само собой он там есть.  Нас интересуют все кроме текущего
			{
				Result = MeshList[MeshIndex].VertexAdjency[MeshList[MeshIndex].NavMeshPolygons[i].PollyVertices[0].VertexIndex].FindValue(TempValue->IntegerValue);
				if(Result > 1)
				{ // У одной грани больше 2х полигонов.  Выводим ошибку об этом
					MessageBoxA(0,"Critical error:  Edge have more than 2 pollys.\n Geometry is not correct",&MeshList[MeshIndex].name[0],MB_OK);
					return;
				}
				if(Result)
				{// Нашли грань сохраним ее
					MeshList[MeshIndex].AdjencyBuffer[i].Edge4 = TempValue->IntegerValue;
					if(TempValue->IntegerValue == NO_ADJENCY)
						{
							MessageBoxA(0,"Invalid NO_ADJENCY CONSTANT",&MeshList[MeshIndex].name[0],MB_OK);
						}
					}
			}
			TempValue = TempValue->NextValue;
		}
	}



for(i = 0; i!= MeshList[MeshIndex].NumVertices;i++)
{
	TempValue = MeshList[MeshIndex].VertexAdjency[i].List;
		for(int EnumElements = 0; EnumElements!=MeshList[MeshIndex].VertexAdjency[i].NumElements; EnumElements++)
		{
			LastValue = TempValue;
			TempValue = TempValue->NextValue;
			delete LastValue;
		}
}



}

void cMesh::GenerateAdjency(unsigned int MeshIndex)
{

//for(int i = 0; i!= MeshList[MeshIndex].NumMeshes;i++)
//{
if(MeshList[MeshIndex].NavMesh) // Можно убрать эту проверку если завести отдельную функцию GenerateAdjencyForMesh()
	{
		
		GenerateAdjencyForNavMesh(MeshIndex);
	}else
	{
		GenerateAdjencyForMesh(MeshIndex);
	}
//TempMesh = TempMesh->NextMesh;
//}

}