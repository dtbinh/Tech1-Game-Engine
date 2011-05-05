/*
Copyright (C) 2011 Tech1 Game Engine.
*/
// Todo  для одинаковых частей кода сделать отдельные функции или defin'ы
#include "common.h"
#include <cmath>
PathFind * PathFinder;
extern cMesh * Mesh;
extern RendererSceleton * RScel;
extern BinaryHeap * BHeap;
unsigned int VStart; // Только для дебага. Удалить
unsigned int VEnd;// Только для дебага. Удалить
extern D3DXVECTOR3 LS;  //// Только для дебага. Удалить
extern D3DXVECTOR3 LE;  //// Только для дебага. Удалить

PathFind::PathFind()
{
	MeshList = Mesh->GetMeshes();
ControlValue = 0;
LastVertexAddedToClosedListIndex = MESH_MAX_VERTICES;
}

/*
void PathFind::FindPath(D3DXVECTOR3 * PathEnd, D3DXVECTOR3 *  PathStart, int PollyStart, int PollyEnd)
{ // Эта функция будет вызыватся только для NavMesh поэтому работаем только с ТфмMeshVertices
//1.  Ищем какой вертекс на стартовом полигоне ближе к цели.

int i;
int SelectedVertex;
float fLength = MAX_PATH_LENGH;
float fTempLengh;
D3DXVECTOR3 TempVector;
	for(i = 0 ; i!=3; i++)
	{
		TempVector = D3DXVECTOR3( PathEnd->x - MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexX, PathEnd->y - MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexY, PathEnd->z - MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexZ);
		fTempLengh = D3DXVec3Dot(&TempVector , &TempVector );//Длинна вектора в квадрате без корня быстрей
		if(fTempLengh < fLength) 
		{
			RScel->LineStart = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexX;
			RScel->LineEnd = *PathEnd;
			SelectedVertex = MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex; // Этот вертекс ближе к цели
			fLength = fTempLengh;
		}
	}


}
*/

void PathFind::RayTraceVec3(unsigned int MeshIndex, D3DXVECTOR3 * VertexToStart, D3DXVECTOR3 * VertexToEnd, unsigned int PollyStart,unsigned int PollyEnd)
{
	if(PollyStart == PollyEnd)
	{
				// достигли последнего полигона.
				return;
	}
	unsigned int VertexEnd1 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[0].VertexIndex;
	unsigned int VertexEnd2 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[1].VertexIndex;
	unsigned int VertexEnd3 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[2].VertexIndex;
	unsigned int VertexEnd4 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[3].VertexIndex;
bool WasOnPlane;
D3DXVECTOR3 Result;
float DistanceToPlane[4];
float percentage;
float linelength;
unsigned int i,  CurrentVertexToCheckIndexIndex;
unsigned char Vert[2][4]; // два массива.  для back и forward вертексов  0[..] спереди 1[..]сзади
unsigned char VertOnPlane[2];
unsigned char VertOnPlaneCount=0;
unsigned char ForwardVerticesCount;
unsigned char BackwardVerticesCount;
unsigned char j;
D3DXVECTOR3 * PointOnPlane =VertexToStart;
D3DXVECTOR3 Direction;
float result;
D3DXVECTOR3 VecDown =  D3DXVECTOR3(0, -1.0f, 0);
D3DXVECTOR3 * VecToCheck;
//VStart = VertexToStart;
//VEnd = VertexToEnd;
D3DXVECTOR3  Ray = *VertexToEnd-*VertexToStart;

//float RayLengh = D3DXVec3Length(&Ray);


D3DXVECTOR3  RayNormal;
D3DXVec3Cross(&RayNormal, &Ray, &VecDown );
D3DXVec3Normalize(&RayNormal,&RayNormal);
//RayNormal /=RayLengh;

//////////
/////////
LS =  *VertexToStart;
LE = LS + RayNormal;

LS =  *VertexToStart;
LE = *VertexToEnd;
//return;  ///!!!!!!!!!!!!!!!!!!!!!
//////////
/////////
LS =  *VertexToStart;
LE = LS + (Ray*0.015);
float mmnnX = LE.x;
float mmnnY = LE.y;
float mmnnZ = LE.z;
//return;  ///!!!!!!!!!!!!!!!!!!!!!

bool LastVertexWasOnPlane=true;
unsigned int CurrentPollyToCheck = PollyStart; // нельзя чтоб был -1 поэтому ставим любое число кроме -1
unsigned int VertexIndexWorkingWith; //= VertexToStart;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
unsigned int LastVertexCrossed;//=VertexToStart;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

VListValuesNoNext * CurrentVertexAdjencyPollys;
unsigned char LastEdgeNum;  // Грань которую пересекли в прошлый раз относительно текцщего полигона
unsigned int LastPollyIndex; // Индекс полигона с которым работали в прошлый раз

ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
for(j = 0; j!=4; j++) // 4 количество вертексов в  полигоне
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[j].VertexIndex;

			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// спереди
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// сзади
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// Вертекс на плоскости
VertOnPlane[VertOnPlaneCount]=j;
VertOnPlaneCount++;
				
			}
		}
if(VertOnPlaneCount==2)
{// Проверяем какой из вертексов дальше от VertexToEnd
	LastVertexCrossed =MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[VertOnPlane[0]].VertexIndex;
	VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[LastVertexCrossed].VertexX;
	result = D3DXVec3Length(&(*VertexToEnd - *VecToCheck));
	VertexIndexWorkingWith = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[VertOnPlane[1]].VertexIndex;
	VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[VertexIndexWorkingWith].VertexX;
	
	if( D3DXVec3Length(&(*VertexToEnd - *VecToCheck)) > result)
	{//use LastVertexCrossed

CurrentVertexToCheckIndexIndex = LastVertexCrossed ;
LastVertexCrossed = VertexIndexWorkingWith;
VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
	}

}
else if(VertOnPlaneCount==1)
{ int pp=100;
	if(ForwardVerticesCount == 1)
	{
		switch(Vert[0][0])
		{
			case 0:
		    // пересекли  1
			//onPlane == 3 сзади
				
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
pp++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
	
}




				pp++;
			break;

			case 1:
		    // пересекли  2
			//onPlane == 0 сзади
				LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
pp++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
	
}
			break;
		
			case 2:
			// пересекли  3 
			// onPlane == 1 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[2] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
pp++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3;
LastVertexWasOnPlane = false;
	
}

				pp++;
			break;

			case 3:
			// пересекли  4 
			// onPlane == 2 сзади
				LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
pp++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4;
LastVertexWasOnPlane = false;
	
}
			break;
		}
	}else{//BackwardVerticesCount == 1
		switch(Vert[1][0])
		{
			case 0:
		    // пересекли  4
			//onPlane == 1 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex;
				Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					
					// Теперь проверяем кто ближе  Ray или (VertexToEnd - Точка пересечения)


if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
pp++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4;
LastVertexWasOnPlane = false;
	
}
				
			break;

			case 1:
		    // пересекли  1
			//onPlane == 2 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе //?????????????????????????????????
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
}
			break;

			case 2:
		    // пересекли  2
			//onPlane == 3 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
pp++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
}
			break;

			case 3:
		    // пересекли  3
			//onPlane == 0 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
pp++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3;
LastVertexWasOnPlane = false;
}
			break;
		}
	}
	
	
//LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[VertOnPlane[0]].VertexIndex;
//VertexIndexWorkingWith = LastVertexCrossed;
//if(VertOnPlane[0] == 

}else
{//VertOnPlaneCount==0
	if(ForwardVerticesCount == BackwardVerticesCount)
	{// 2 спереди 2 сзади
switch(Vert[0][0])// какой вертекс спереди
		{
			case 0: // если это 0й вертекс
				if(Vert[0][1]==1)
				{ // Пересекли 2 и 4
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (ближе)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 спереди 2 сзади

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[0] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4; // Возможен ли этот вариант?????????????????
}else
{ // 2 спереди 4 сзади

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;

}






				
					
				}else if(Vert[0][1]==3)
				{ // пересекли 1 и 3
					
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (ближе)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);


if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 спереди 2 сзади

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[0] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3; // Возможен ли этот вариант?????????????????
}else
{ // 2 спереди 4 сзади

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;

}
					
				}
			break;
			
			case 1: // если это 1й вертекс
				if(Vert[0][1]==0)
				{ // пересекли 2 4

MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else if(Vert[0][1]==2)
				{ // пересекли 1  3
					
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (ближе)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);





if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 спереди 2 сзади

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[2] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3; // Возможен ли этот вариант????????????????? Возможен!!!
}else
{ // 2 спереди 4 сзади

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;

}
					
				}
			break;
			
			case 2: // если это 2й вертекс
				if(Vert[0][1]==1)
				{ // 1 3
					
int loading = 100;
loading ++;
					
				}else if(Vert[0][1]==3)
				{ // пересекли 2 4
					
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (ближе)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 спереди 2 сзади

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[0] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4;
}else
{ // 2 спереди 4 сзади

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;// Возможен ли этот вариант?????????????????

}
					
					
				}
			break;
			
			case 3: // если это 3й вертекс
				if(Vert[0][1]==0)
				{ // пересекли 1  3
					
					int loading = 100;
					loading ++;
					
					
				}else if(Vert[0][1]==2)
				{ // пересекли 2  4
					
					int loading = 100;
					loading ++;
					
				}
			break;
		}
	}else
	{// 3 с одной стороны 1 с другой
		if(ForwardVerticesCount > BackwardVerticesCount)
		{ //BackwardVerticesCount == 1
		int fdffsdf=100;
			fdffsdf++;

			switch(Vert[1][0])
		{
			case 0:
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex;
				Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					
					// Теперь проверяем кто ближе  Ray или (VertexToEnd - Точка пересечения)


if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4;
LastVertexWasOnPlane = false;
	
}
				
			break;

			case 1:
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
}
			break;

			case 2:

				// пересекли  2
			//onPlane == 3 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
}
			break;

			case 3:
		    // пересекли  3
			//onPlane == 0 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3;
LastVertexWasOnPlane = false;
}
			break;

		}
		}else
		{// ForwardVerticesCount == 1
				int fdffsdf=100;
			fdffsdf++;

			switch(Vert[0][0])
		{
			case 0:
		    // пересекли  1
			//onPlane == 3 сзади
				LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
	
}
			break;

			case 1:
		    // пересекли  2
			//onPlane == 0 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);

if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
	
}
			break;
		
			case 2:
			// пересекли  3 
			// onPlane == 1 сзади

LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[2] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3;
LastVertexWasOnPlane = false;
}
			break;

			case 3:
			// пересекли  4 
			// onPlane == 2 сзади
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ближе
fdffsdf++;
}else
{// Точка пересечения ближе
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4;
LastVertexWasOnPlane = false;
	
}
			break;
		}

		}

	}
}

//return;
for(int NumIterationsOnlyForDebug = 0; NumIterationsOnlyForDebug!=100;NumIterationsOnlyForDebug++)
{
	
	if(CurrentPollyToCheck == -1)
	{
		// луч пересек непроходимое пространство
return;
	}

	if(CurrentPollyToCheck == PollyEnd)
	{
				// достигли последнего полигона.
				return;
	}
		if(LastVertexWasOnPlane)
		{
			if((VertexIndexWorkingWith == VertexEnd1)||(VertexIndexWorkingWith == VertexEnd2)||(VertexIndexWorkingWith == VertexEnd3)||(VertexIndexWorkingWith == VertexEnd4))
			{
				// достигли последнего вертекса.
				return;
			}
for(i=0; i!=MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].NumElements; i++) // Проходим по всем полигонам к которым принадлежит текущий вертекс
{// Добавить если текущий вертекс не из NavMeshVertices - после предыдущего сплита
// ToDo  остановить цикл .NumElements - i  когда нашли пересечение
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
CurrentVertexAdjencyPollys = &MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].Massive[i];
CurrentPollyToCheck = CurrentVertexAdjencyPollys->IntegerValue;
if(CurrentPollyToCheck == LastPollyIndex) continue;

WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 количество вертексов в  полигоне
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;
			if((CurrentVertexToCheckIndexIndex == VertexIndexWorkingWith)||(CurrentVertexToCheckIndexIndex==LastVertexCrossed)) continue; // Пропускаем текущий вертекс.
			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// спереди
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// сзади
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// Вертекс на плоскости
				// Используем текущий вертекс.
				LastVertexWasOnPlane = true;
				LastVertexCrossed = VertexIndexWorkingWith;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				WasOnPlane = true;
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				break;
			}
		}
if(WasOnPlane) break;
if((!ForwardVerticesCount)||(!BackwardVerticesCount)) continue; // Если полигон не пересечен  пропускаем дальнейшие проверки
if(ForwardVerticesCount==1)
{
if(BackwardVerticesCount==1) continue;
}

if(ForwardVerticesCount > BackwardVerticesCount)
	{ // Сзади 1 
		 // какой вертекс сзади
		
			if(Vert[1][0] == 0) // если это 0й вертекс
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 1ю грань
				MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);	// Возможен ли этот вариант. Поставить брейкпоинт для проверки
				}else  // Если в прошлый раз пересекли 2й вертекс полигона то сейчас пересекли 4ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					LastVertexWasOnPlane = false;
					break;
				}
			
				}
			
			if (Vert[1][0] == 1) // если это 1й вертекс
				{	
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 1ю грань
					// 0 - 1
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					// работает правильно
				//= Result
				LastPollyIndex = CurrentPollyToCheck;
				CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				LastVertexWasOnPlane = false;
				break;
				}else  // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 2ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
				}
			
			if(Vert[1][0] == 2) // если это 2й вертекс
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // Если в прошлый раз пересекли 2й вертекс полигона то сейчас пересекли 3ю грань
				//	Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
				//	linelength=D3DXVec3Dot(&Direction,&RayNormal);
				//	percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
				//	LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
				//	LastPollyIndex = CurrentPollyToCheck;
				//	CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
				//	LastVertexWasOnPlane = false;
			MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
			//continue;
				}else  // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 2ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
				//	LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
					LastVertexWasOnPlane = false;
					break;
				}
			
			}
			
			if (Vert[1][0] == 3) // если это 3й вертекс
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 4ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			}
		//switch




		
	}else if(ForwardVerticesCount < BackwardVerticesCount)
	{ // спереди 1
		if(Vert[0][0] == 0) // какой вертекс сзади
		{
			
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 1ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
					LastVertexWasOnPlane = false;
					break;
				}else  // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 4ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
		}
			
			if(Vert[0][0] == 1) // если это 1й вертекс
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 1ю грань
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 2ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
				//	LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
					LastVertexWasOnPlane = false;
					break;
				}
			
			}
			
			if(Vert[0][0] == 2) // если это 2й вертекс
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // Если в прошлый раз пересекли 2й вертекс полигона то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 2ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			
				}
			
			if(Vert[0][0] == 3) // если это 3й вертекс
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 4ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
				//	LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					LastVertexWasOnPlane = false;
					break;
				}
			}
			
		
		
	}
	
}// num elements
if(!WasOnPlane)
{
if((!ForwardVerticesCount)||(!BackwardVerticesCount))
{
// Не было ни одного пересечения - луч проходит через непроходимое пространство
return;
}
}
}else
	{// В прошлый раз вертекс был на Edge
		/////
		/////
		/////
		/////
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
//CurrentPollyToCheck  Полигон который будем проверять

//LastPollyIndex Номер полигона с которым работали в прошлый раз

if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1 == LastPollyIndex) LastEdgeNum = 1;//LastEdgeNum = номер предыдущей грани которую пересекли
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2 == LastPollyIndex) LastEdgeNum = 2;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3 == LastPollyIndex) LastEdgeNum = 3;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4 == LastPollyIndex) LastEdgeNum = 4;
WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 количество вертексов в  полигоне
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;

			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// спереди
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// сзади
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// Вертекс на плоскости
				// Используем текущий вертекс.
				LastVertexWasOnPlane = true;
				WasOnPlane = true;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				// todo останавливаем дальнейшие проверки
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				LastPollyIndex = CurrentPollyToCheck;
				break;
			}
	}
	if(WasOnPlane) continue;
		if(ForwardVerticesCount > BackwardVerticesCount)
	{ // Сзади 1 
		switch(Vert[1][0]) // какой вертекс сзади
		{
			case 0: // если это 0й вертекс
				if(LastEdgeNum == 4)
				{ // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 1ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 4ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
				}
			break;
			
			case 1: // если это 1й вертекс
				if(LastEdgeNum == 1)
				{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 2ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 1ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				}
			break;
			
			case 2: // если это 2й вертекс
				if(LastEdgeNum == 2)
				{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 2ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
				}
			break;
			
			case 3: // если это 3й вертекс
				if(LastEdgeNum == 3)
				{ // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 3ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}else if(ForwardVerticesCount < BackwardVerticesCount)
	{ // спереди 1
		switch(Vert[0][0]) // какой вертекс сзади
		{
			case 0: // если это 0й вертекс
				if(LastEdgeNum == 4)
				{ // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 1ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				}else  // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 4ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 1: // если это 1й вертекс
				if(LastEdgeNum == 1)
				{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 2ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
				}else  // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 1ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 2: // если это 2й вертекс
				if(LastEdgeNum == 2)
				{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
				}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 2ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 3: // если это 3й вертекс
				if(LastEdgeNum == 3)
				{ // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 4ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
				}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 3ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}else
	{// 2 вертекса спереди и 2 сзади
		
		switch(Vert[0][0])// какой вертекс спереди
		{
			case 0: // если это 0й вертекс
				if(Vert[0][1]==1)
				{ // Пересекли 2 и 4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
					}
					
				}else if(Vert[0][1]==3)
				{ // пересекли 1 и 3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
					}
				}
			break;
			
			case 1: // если это 1й вертекс
				if(Vert[0][1]==0)
				{ // пересекли 2 4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==2)
				{ // пересекли 1  3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}
			break;
			
			case 2: // если это 2й вертекс
				if(Vert[0][1]==1)
				{ // 1 3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==3)
				{ // пересекли 2 4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					} 
					
				}
			break;
			
			case 3: // если это 3й вертекс
				if(Vert[0][1]==0)
				{ // пересекли 1  3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
					
				}else if(Vert[0][1]==2)
				{ // пересекли 2  4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					} 
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}
	}
}
}


void PathFind::RayTrace(unsigned int MeshIndex, unsigned int VertexToStart, unsigned int VertexToEnd, unsigned int PollyStart,unsigned int PollyEnd)
{ // 43, 321
	unsigned int VertexEnd1 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[0].VertexIndex;
	unsigned int VertexEnd2 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[1].VertexIndex;
	unsigned int VertexEnd3 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[2].VertexIndex;
	unsigned int VertexEnd4 =  MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[3].VertexIndex;
bool WasOnPlane;
D3DXVECTOR3 Result;
float DistanceToPlane[4];
float percentage;
float linelength;
unsigned int i,  CurrentVertexToCheckIndexIndex;
unsigned char Vert[2][4]; // два массива.  для back и forward вертексов  0[..] спереди 1[..]сзади
unsigned char ForwardVerticesCount;
unsigned char BackwardVerticesCount;
unsigned char j;
D3DXVECTOR3 * PointOnPlane =(D3DXVECTOR3 *) &MeshList[MeshIndex].NavMeshVertices[VertexToStart].VertexX;
D3DXVECTOR3 Direction;
float result;
D3DXVECTOR3 VecDown =  D3DXVECTOR3(0, -1.0f, 0);
D3DXVECTOR3 * VecToCheck;
VStart = VertexToStart;
VEnd = VertexToEnd;
D3DXVECTOR3  RayNormal;
D3DXVec3Cross(&RayNormal, &D3DXVECTOR3(MeshList[MeshIndex].NavMeshVertices[VertexToEnd].VertexX-MeshList[MeshIndex].NavMeshVertices[VertexToStart].VertexX, MeshList[MeshIndex].NavMeshVertices[VertexToEnd].VertexY - MeshList[MeshIndex].NavMeshVertices[VertexToStart].VertexY, MeshList[MeshIndex].NavMeshVertices[VertexToEnd].VertexZ - MeshList[MeshIndex].NavMeshVertices[VertexToStart].VertexZ), &VecDown );
D3DXVec3Normalize(&RayNormal,&RayNormal);

//////////
/////////
LS =  D3DXVECTOR3(MeshList[MeshIndex].NavMeshVertices[VertexToStart].VertexX,MeshList[MeshIndex].NavMeshVertices[VertexToStart].VertexY,MeshList[MeshIndex].NavMeshVertices[VertexToStart].VertexZ);
LE = LS + RayNormal;
//return;  ///!!!!!!!!!!!!!!!!!!!!!
//////////
/////////


bool LastVertexWasOnPlane=true;
unsigned int CurrentPollyToCheck = 0; // нельзя чтоб был -1 поэтому ставим любое число кроме -1
unsigned int VertexIndexWorkingWith = VertexToStart;
unsigned int LastVertexCrossed=VertexToStart;
VListValuesNoNext * CurrentVertexAdjencyPollys;
unsigned char LastEdgeNum;  // Грань которую пересекли в прошлый раз относительно текцщего полигона
unsigned int LastPollyIndex; // Индекс полигона с которым работали в прошлый раз

for(int NumIterationsOnlyForDebug = 0; NumIterationsOnlyForDebug!=100;NumIterationsOnlyForDebug++)
{
	if (VertexIndexWorkingWith == 224)
{
int llkkk = 100;
llkkk++;
}
	if(CurrentPollyToCheck == -1)
	{
		// луч пересек непроходимое пространство
return;
	}

	if(CurrentPollyToCheck == PollyEnd)
	{
				// достигли последнего полигона.
				return;
	}
		if(LastVertexWasOnPlane)
		{
			if((VertexIndexWorkingWith == VertexEnd1)||(VertexIndexWorkingWith == VertexEnd2)||(VertexIndexWorkingWith == VertexEnd3)||(VertexIndexWorkingWith == VertexEnd4))
			{
				// достигли последнего вертекса.
				return;
			}
for(i=0; i!=MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].NumElements; i++) // Проходим по всем полигонам к которым принадлежит текущий вертекс
{// Добавить если текущий вертекс не из NavMeshVertices - после предыдущего сплита
// ToDo  остановить цикл .NumElements - i  когда нашли пересечение
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
CurrentVertexAdjencyPollys = &MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].Massive[i];
CurrentPollyToCheck = CurrentVertexAdjencyPollys->IntegerValue;

WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 количество вертексов в  полигоне
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;
			if((CurrentVertexToCheckIndexIndex == VertexIndexWorkingWith)||(CurrentVertexToCheckIndexIndex==LastVertexCrossed)) continue; // Пропускаем текущий вертекс.
			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// спереди
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// сзади
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// Вертекс на плоскости
				// Используем текущий вертекс.
				LastVertexWasOnPlane = true;
				LastVertexCrossed = VertexIndexWorkingWith;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				WasOnPlane = true;
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				break;
			}
		}
if(WasOnPlane) break;
if((!ForwardVerticesCount)||(!BackwardVerticesCount)) continue; // Если полигон не пересечен  пропускаем дальнейшие проверки
if(ForwardVerticesCount==1)
{
if(BackwardVerticesCount==1) continue;
}

if(ForwardVerticesCount > BackwardVerticesCount)
	{ // Сзади 1 
		 // какой вертекс сзади
		
			if(Vert[1][0] == 0) // если это 0й вертекс
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 1ю грань
				MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);	// Возможен ли этот вариант. Поставить брейкпоинт для проверки
				}else  // Если в прошлый раз пересекли 2й вертекс полигона то сейчас пересекли 4ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					LastVertexWasOnPlane = false;
					break;
				}
			
				}
			
			if (Vert[1][0] == 1) // если это 1й вертекс
				{	
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 1ю грань
					// 0 - 1
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					// работает правильно
				//= Result
				LastPollyIndex = CurrentPollyToCheck;
				CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				LastVertexWasOnPlane = false;
				break;
				}else  // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 2ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
				}
			
			if(Vert[1][0] == 2) // если это 2й вертекс
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // Если в прошлый раз пересекли 2й вертекс полигона то сейчас пересекли 3ю грань
				MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 2ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
				//	LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
					LastVertexWasOnPlane = false;
					break;
				}
			
			}
			
			if (Vert[1][0] == 3) // если это 3й вертекс
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 4ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			}
		//switch




		
	}else if(ForwardVerticesCount < BackwardVerticesCount)
	{ // спереди 1
		if(Vert[0][0] == 0) // какой вертекс сзади
		{
			
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 1ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
					LastVertexWasOnPlane = false;
					break;
				}else  // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 4ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
		}
			
			if(Vert[0][0] == 1) // если это 1й вертекс
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 1ю грань
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 2ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
				//	LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
					LastVertexWasOnPlane = false;
					break;
				}
			
			}
			
			if(Vert[0][0] == 2) // если это 2й вертекс
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // Если в прошлый раз пересекли 2й вертекс полигона то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // Если в прошлый раз пересекли 4й вертекс полигона то сейчас пересекли 2ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			
				}
			
			if(Vert[0][0] == 3) // если это 3й вертекс
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // Если в прошлый раз пересекли 1й вертекс полигона то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 3й вертекс полигона то сейчас пересекли 4ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
				//	LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					LastVertexWasOnPlane = false;
					break;
				}
			}
			
		
		
	}
	
}// num elements
if(!WasOnPlane)
{
if((!ForwardVerticesCount)||(!BackwardVerticesCount))
{
// Не было ни одного пересечения - луч проходит через непроходимое пространство
return;
}
}
}else
	{// В прошлый раз вертекс был на Edge
		/////
		/////
		/////
		/////
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
//CurrentPollyToCheck  Полигон который будем проверять

//LastPollyIndex Номер полигона с которым работали в прошлый раз

if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1 == LastPollyIndex) LastEdgeNum = 1;//LastEdgeNum = номер предыдущей грани которую пересекли
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2 == LastPollyIndex) LastEdgeNum = 2;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3 == LastPollyIndex) LastEdgeNum = 3;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4 == LastPollyIndex) LastEdgeNum = 4;
WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 количество вертексов в  полигоне
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;

			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// спереди
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// сзади
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// Вертекс на плоскости
				// Используем текущий вертекс.
				LastVertexWasOnPlane = true;
				WasOnPlane = true;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				// todo останавливаем дальнейшие проверки
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				break;
			}
	}
	if(WasOnPlane) continue;
		if(ForwardVerticesCount > BackwardVerticesCount)
	{ // Сзади 1 
		switch(Vert[1][0]) // какой вертекс сзади
		{
			case 0: // если это 0й вертекс
				if(LastEdgeNum == 4)
				{ // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 1ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 4ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
				}
			break;
			
			case 1: // если это 1й вертекс
				if(LastEdgeNum == 1)
				{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 2ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 1ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				}
			break;
			
			case 2: // если это 2й вертекс
				if(LastEdgeNum == 2)
				{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 2ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
				}
			break;
			
			case 3: // если это 3й вертекс
				if(LastEdgeNum == 3)
				{ // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 3ю грань
				{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}else if(ForwardVerticesCount < BackwardVerticesCount)
	{ // спереди 1
		switch(Vert[0][0]) // какой вертекс сзади
		{
			case 0: // если это 0й вертекс
				if(LastEdgeNum == 4)
				{ // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 1ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				}else  // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 4ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 1: // если это 1й вертекс
				if(LastEdgeNum == 1)
				{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 2ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
				}else  // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 1ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 2: // если это 2й вертекс
				if(LastEdgeNum == 2)
				{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
				}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 2ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 3: // если это 3й вертекс
				if(LastEdgeNum == 3)
				{ // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 4ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
				}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 3ю грань
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}else
	{// 2 вертекса спереди и 2 сзади
		
		switch(Vert[0][0])// какой вертекс спереди
		{
			case 0: // если это 0й вертекс
				if(Vert[0][1]==1)
				{ // Пересекли 2 и 4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
					}
					
				}else if(Vert[0][1]==3)
				{ // пересекли 1 и 3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
					}
				}
			break;
			
			case 1: // если это 1й вертекс
				if(Vert[0][1]==0)
				{ // пересекли 2 4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==2)
				{ // пересекли 1  3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}
			break;
			
			case 2: // если это 2й вертекс
				if(Vert[0][1]==1)
				{ // 1 3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==3)
				{ // пересекли 2 4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					} 
					
				}
			break;
			
			case 3: // если это 3й вертекс
				if(Vert[0][1]==0)
				{ // пересекли 1  3
					if(LastEdgeNum == 1)
					{ // Если в прошлый раз пересекли 1ю грань то сейчас пересекли 3ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 3ю грань то сейчас пересекли 1ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
					
				}else if(Vert[0][1]==2)
				{ // пересекли 2  4
					if(LastEdgeNum == 2)
					{ // Если в прошлый раз пересекли 2ю грань то сейчас пересекли 4ю грань
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // Если в прошлый раз пересекли 4ю грань то сейчас пересекли 2ю грань
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					} 
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}
	}
}


}
void PathFind::FindPath(unsigned int MeshIndex, D3DXVECTOR3 * PathEnd, D3DXVECTOR3 *  PathStart, int PollyStart, int PollyEnd)
{ // Эта функция будет вызыватся только для NavMesh поэтому работаем только с NavMeshVertices
//1.  Ищем какой вертекс на стартовом полигоне ближе к цели.
//RayTrace(16, 272, 15, 240);
//RayTraceVec3((D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,240, 15);
//RayTraceVec3( &D3DXVECTOR3(-11.612207f,0.0f,11.648356f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255);
//RayTraceVec3( &D3DXVECTOR3(-11.612207f,0.0f,11.648356f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX,255, 0);
//RayTraceVec3( (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX,255, 0);


//RayTraceVec3( &D3DXVECTOR3(-11.612207f,0.0f,11.648356f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255); ok // 2 спереди 2 сзади
//RayTraceVec3(  &D3DXVECTOR3(11.685318f,0.0f,-11.685318f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX, 255, 0); ok // 2 спереди 2 сзади
//RayTraceVec3(  (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240);
//RayTraceVec3(  &D3DXVECTOR3(11.739948f,0.0f,11.590860f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240); ok// 2 спереди 2 сзади
//RayTraceVec3(  &D3DXVECTOR3(-11.726042f,0.0f,-11.580701f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,  240,15); ok// 2 спереди 2 сзади

	
//RayTraceVec3( (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255);
	//RayTraceVec3( &D3DXVECTOR3(-11.652724f,0.0f,11.641801f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255); //// 2 спереди 2 сзади
	//&D3DXVECTOR3(-11.652724f,0.0f,11.641801f)



//RayTraceVec3(  &D3DXVECTOR3(11.695940f,0.0f,-11.664730f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX, 255, 0);
//RayTraceVec3( &D3DXVECTOR3(-11.695940f,0.0f,11.664730f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255); // ok 3 c одной стороны  1 с другой

//RayTraceVec3( &D3DXVECTOR3(11.695940f,0.0f,-11.664730f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX, 255, 0); // ok 3 c одной стороны  1 с другой


//RayTraceVec3(  &D3DXVECTOR3(11.695940f,0.0f,11.664730f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240); ok

//RayTraceVec3(  (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240);
RayTraceVec3( MeshIndex,  &D3DXVECTOR3(-11.695940f,0.0f,-11.664730f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX, 240, 15);
return;
float TempG;
	BHeap->Reset();
int i;
D3DXVECTOR3 TempVector;
unsigned int VertexIndex;
unsigned int VertexToCheck; // Желательно убрать эту переменную 
float VertexX, VertexY, VertexZ;
float PathSX = PathStart->x;
float PathSY = PathStart->y;
float PathSZ = PathStart->z;

float PathEX = PathEnd->x;
float PathEY = PathEnd->y;
float PathEZ = PathEnd->z;

unsigned int EndVertex1 = MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[0].VertexIndex;
unsigned int EndVertex2 = MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[1].VertexIndex;
unsigned int EndVertex3 = MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[2].VertexIndex;
unsigned int EndVertex4 = MeshList[MeshIndex].NavMeshPolygons[PollyEnd].PollyVertices[3].VertexIndex;
	for(i = 0 ; i!=4; i++) // Добавим 4 вертекса от стартового полигона в OpenList
	{
		VertexIndex = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[i].VertexIndex;
		
		VertexX = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX; // Подумать, может всетаки убрать эти переменные
		VertexY = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY;
		VertexZ = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ;


		

		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G  =abs(VertexX - PathStart->x)+ abs(VertexY- PathStart->y) + abs(VertexZ- PathStart->z);//Длинна вектора в квадрате, без корня быстрей
		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].H = abs(PathEnd->x - VertexX) + abs(PathEnd->y - VertexY)+ abs(PathEnd->z - VertexZ);
		
		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].F  = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].H;// F = G + H
// Теперь добавим в OpenList этот вертекс.
		BHeap->AddElementInHeap(MeshIndex, VertexIndex);
		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexStatus = 1; // Метка что занесен в OpenList
	}

	for(;;)
	{
		
	//1. Выбераем клетку с наименьшей стоимостью
	// Удаляем ее из OpenList
	VertexIndex = BHeap->RemoveCheapestElement(MeshIndex);
	
	// Заносим ее в ClosedList
	MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexStatus = 2; // 2 = ClosedList
	MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].ParrentIndex;
	LastVertexAddedToClosedListIndex = VertexIndex;
if((VertexIndex == EndVertex1)||(VertexIndex == EndVertex2)||(VertexIndex == EndVertex3)||(VertexIndex == EndVertex4)) break;
	// Работаем с соседними клетками
	//
	ControlValue++;
	if(ControlValue == 0xffffffff) // Протестировать - поставить например 5 или 10

	{
		for(i = 0; i != MeshList[MeshIndex].NumVertices; i++)
		{
			MeshList[MeshIndex].NavMeshVertices[i].ThisTimeCalculated = 0;
		}
		ControlValue = 1;
	}
	for(i=0; i!=MeshList[MeshIndex].VertexAdjency[VertexIndex].NumElements; i++) // Проходим по всем полигонам к которым принадлежит текущий вертекс
	{
		
		switch(MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].VertexNum)
		{
			case 0: // Берем 2й и 1й 3й вертексы


			 //  обработаем  2й вертекс	
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[2].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								//if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue)
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						//TempVector = D3DXVECTOR3( , MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
						//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						//TempVector = D3DXVECTOR3(, MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //  Терерь обработаем  1й вертекс
 


VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[1].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //  обработаем  3й вертекс	
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[3].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }
				break;
			case 1:// Берем 2й и 0й 3й вертексы
			

			 //  обработаем  2й вертекс	
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[2].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }
//0Й
 

			
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[0].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //3Й
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[3].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

			break;
			case 2:// Берем 0й и 1й 3й вертексы
		VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[0].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //1й
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[1].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //3Й
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[3].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }
			break;

			case 3:// Берем 0й и 1й 2й вертексы
		VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[0].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //1й
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[1].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //2Й
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[2].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue индекс полигона к которому принадлежит этот вертекс
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // Если клетка уже обрабатывалась(при текущей проверке соседних клеток) игнорируем ее.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// Если клетка в закрытом списке игнорируем ее.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // вертексы  Closed List игнорируем.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // если не в Open List
					{
						//Делаем текущую клетку родительской для этой клетки.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// Расчитываем F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // добавляем в Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // Вертекс уже в OpenList
												//проверяем, не дешевле ли будет путь через эту клетку. Для сравнения используем стоимость G. Более низкая стоимость G указывает на то, что путь будет дешевле.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//Длинна вектора в квадрате, без корня быстрей//  ToDo Заменить на эвристику
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//Mеняем родителя клетки на текущую клетку
							//и пересчитываем для нее стоимости G и F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // добавляем в Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  в Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }
			break;

			
		};
	}
	}
}


PathFind::~PathFind()
{

}