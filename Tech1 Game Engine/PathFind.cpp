/*
Copyright (C) 2011 Tech1 Game Engine.
*/
// Todo  ��� ���������� ������ ���� ������� ��������� ������� ��� defin'�
#include "common.h"
#include <cmath>
PathFind * PathFinder;
extern cMesh * Mesh;
extern RendererSceleton * RScel;
extern BinaryHeap * BHeap;
unsigned int VStart; // ������ ��� ������. �������
unsigned int VEnd;// ������ ��� ������. �������
extern D3DXVECTOR3 LS;  //// ������ ��� ������. �������
extern D3DXVECTOR3 LE;  //// ������ ��� ������. �������

PathFind::PathFind()
{
	MeshList = Mesh->GetMeshes();
ControlValue = 0;
LastVertexAddedToClosedListIndex = MESH_MAX_VERTICES;
}

/*
void PathFind::FindPath(D3DXVECTOR3 * PathEnd, D3DXVECTOR3 *  PathStart, int PollyStart, int PollyEnd)
{ // ��� ������� ����� ��������� ������ ��� NavMesh ������� �������� ������ � ���MeshVertices
//1.  ���� ����� ������� �� ��������� �������� ����� � ����.

int i;
int SelectedVertex;
float fLength = MAX_PATH_LENGH;
float fTempLengh;
D3DXVECTOR3 TempVector;
	for(i = 0 ; i!=3; i++)
	{
		TempVector = D3DXVECTOR3( PathEnd->x - MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexX, PathEnd->y - MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexY, PathEnd->z - MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexZ);
		fTempLengh = D3DXVec3Dot(&TempVector , &TempVector );//������ ������� � �������� ��� ����� �������
		if(fTempLengh < fLength) 
		{
			RScel->LineStart = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex ].VertexX;
			RScel->LineEnd = *PathEnd;
			SelectedVertex = MeshList[MeshIndex].Polygons[PollyStart].PollyVertices[i].VertexIndex; // ���� ������� ����� � ����
			fLength = fTempLengh;
		}
	}


}
*/

void PathFind::RayTraceVec3(unsigned int MeshIndex, D3DXVECTOR3 * VertexToStart, D3DXVECTOR3 * VertexToEnd, unsigned int PollyStart,unsigned int PollyEnd)
{
	if(PollyStart == PollyEnd)
	{
				// �������� ���������� ��������.
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
unsigned char Vert[2][4]; // ��� �������.  ��� back � forward ���������  0[..] ������� 1[..]�����
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
unsigned int CurrentPollyToCheck = PollyStart; // ������ ���� ��� -1 ������� ������ ����� ����� ����� -1
unsigned int VertexIndexWorkingWith; //= VertexToStart;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
unsigned int LastVertexCrossed;//=VertexToStart;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

VListValuesNoNext * CurrentVertexAdjencyPollys;
unsigned char LastEdgeNum;  // ����� ������� ��������� � ������� ��� ������������ �������� ��������
unsigned int LastPollyIndex; // ������ �������� � ������� �������� � ������� ���

ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
for(j = 0; j!=4; j++) // 4 ���������� ��������� �  ��������
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[j].VertexIndex;

			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// �������
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// �����
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// ������� �� ���������
VertOnPlane[VertOnPlaneCount]=j;
VertOnPlaneCount++;
				
			}
		}
if(VertOnPlaneCount==2)
{// ��������� ����� �� ��������� ������ �� VertexToEnd
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
		    // ���������  1
			//onPlane == 3 �����
				
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
pp++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
	
}




				pp++;
			break;

			case 1:
		    // ���������  2
			//onPlane == 0 �����
				LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
pp++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
	
}
			break;
		
			case 2:
			// ���������  3 
			// onPlane == 1 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[2] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
pp++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3;
LastVertexWasOnPlane = false;
	
}

				pp++;
			break;

			case 3:
			// ���������  4 
			// onPlane == 2 �����
				LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
pp++;
}else
{// ����� ����������� �����
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
		    // ���������  4
			//onPlane == 1 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex;
				Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					
					// ������ ��������� ��� �����  Ray ��� (VertexToEnd - ����� �����������)


if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
pp++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4;
LastVertexWasOnPlane = false;
	
}
				
			break;

			case 1:
		    // ���������  1
			//onPlane == 2 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart ����� //?????????????????????????????????
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
}
			break;

			case 2:
		    // ���������  2
			//onPlane == 3 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
pp++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
}
			break;

			case 3:
		    // ���������  3
			//onPlane == 0 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
pp++;
}else
{// ����� ����������� �����
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
	{// 2 ������� 2 �����
switch(Vert[0][0])// ����� ������� �������
		{
			case 0: // ���� ��� 0� �������
				if(Vert[0][1]==1)
				{ // ��������� 2 � 4
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (�����)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 ������� 2 �����

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[0] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4; // �������� �� ���� �������?????????????????
}else
{ // 2 ������� 4 �����

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;

}






				
					
				}else if(Vert[0][1]==3)
				{ // ��������� 1 � 3
					
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (�����)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);


if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 ������� 2 �����

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[0] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3; // �������� �� ���� �������?????????????????
}else
{ // 2 ������� 4 �����

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;

}
					
				}
			break;
			
			case 1: // ���� ��� 1� �������
				if(Vert[0][1]==0)
				{ // ��������� 2 4

MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else if(Vert[0][1]==2)
				{ // ��������� 1  3
					
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (�����)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);





if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 ������� 2 �����

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[2] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3; // �������� �� ���� �������????????????????? ��������!!!
}else
{ // 2 ������� 4 �����

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;

}
					
				}
			break;
			
			case 2: // ���� ��� 2� �������
				if(Vert[0][1]==1)
				{ // 1 3
					
int loading = 100;
loading ++;
					
				}else if(Vert[0][1]==3)
				{ // ��������� 2 4
					
LastPollyIndex = PollyStart;
LastVertexWasOnPlane = false;
//2 ok (�����)
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{// 4 ������� 2 �����

//Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
//linelength=D3DXVec3Dot(&Direction,&RayNormal);
//percentage = DistanceToPlane[0] / linelength;
//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge4;
}else
{ // 2 ������� 4 �����

CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;// �������� �� ���� �������?????????????????

}
					
					
				}
			break;
			
			case 3: // ���� ��� 3� �������
				if(Vert[0][1]==0)
				{ // ��������� 1  3
					
					int loading = 100;
					loading ++;
					
					
				}else if(Vert[0][1]==2)
				{ // ��������� 2  4
					
					int loading = 100;
					loading ++;
					
				}
			break;
		}
	}else
	{// 3 � ����� ������� 1 � ������
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
					
					// ������ ��������� ��� �����  Ray ��� (VertexToEnd - ����� �����������)


if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
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
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
}
			break;

			case 2:

				// ���������  2
			//onPlane == 3 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
}
			break;

			case 3:
		    // ���������  3
			//onPlane == 0 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
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
		    // ���������  1
			//onPlane == 3 �����
				LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[0] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge1;
LastVertexWasOnPlane = false;
	
}
			break;

			case 1:
		    // ���������  2
			//onPlane == 0 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[1] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);

if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge2;
LastVertexWasOnPlane = false;
	
}
			break;
		
			case 2:
			// ���������  3 
			// onPlane == 1 �����

LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[1].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[2] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
LastPollyIndex = PollyStart;
CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[PollyStart].Edge3;
LastVertexWasOnPlane = false;
}
			break;

			case 3:
			// ���������  4 
			// onPlane == 2 �����
LastVertexCrossed = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[2].VertexIndex;
Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX;
linelength=D3DXVec3Dot(&Direction,&RayNormal);
percentage = DistanceToPlane[3] / linelength;
LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
if( D3DXVec3Length(&Ray) < D3DXVec3Length(&(*VertexToEnd - (*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage)))))
{  // VertexToStart �����
fdffsdf++;
}else
{// ����� ����������� �����
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
		// ��� ������� ������������ ������������
return;
	}

	if(CurrentPollyToCheck == PollyEnd)
	{
				// �������� ���������� ��������.
				return;
	}
		if(LastVertexWasOnPlane)
		{
			if((VertexIndexWorkingWith == VertexEnd1)||(VertexIndexWorkingWith == VertexEnd2)||(VertexIndexWorkingWith == VertexEnd3)||(VertexIndexWorkingWith == VertexEnd4))
			{
				// �������� ���������� ��������.
				return;
			}
for(i=0; i!=MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].NumElements; i++) // �������� �� ���� ��������� � ������� ����������� ������� �������
{// �������� ���� ������� ������� �� �� NavMeshVertices - ����� ����������� ������
// ToDo  ���������� ���� .NumElements - i  ����� ����� �����������
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
CurrentVertexAdjencyPollys = &MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].Massive[i];
CurrentPollyToCheck = CurrentVertexAdjencyPollys->IntegerValue;
if(CurrentPollyToCheck == LastPollyIndex) continue;

WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 ���������� ��������� �  ��������
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;
			if((CurrentVertexToCheckIndexIndex == VertexIndexWorkingWith)||(CurrentVertexToCheckIndexIndex==LastVertexCrossed)) continue; // ���������� ������� �������.
			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// �������
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// �����
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// ������� �� ���������
				// ���������� ������� �������.
				LastVertexWasOnPlane = true;
				LastVertexCrossed = VertexIndexWorkingWith;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				WasOnPlane = true;
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				break;
			}
		}
if(WasOnPlane) break;
if((!ForwardVerticesCount)||(!BackwardVerticesCount)) continue; // ���� ������� �� ���������  ���������� ���������� ��������
if(ForwardVerticesCount==1)
{
if(BackwardVerticesCount==1) continue;
}

if(ForwardVerticesCount > BackwardVerticesCount)
	{ // ����� 1 
		 // ����� ������� �����
		
			if(Vert[1][0] == 0) // ���� ��� 0� �������
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 1� �����
				MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);	// �������� �� ���� �������. ��������� ���������� ��� ��������
				}else  // ���� � ������� ��� ��������� 2� ������� �������� �� ������ ��������� 4� �����
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
			
			if (Vert[1][0] == 1) // ���� ��� 1� �������
				{	
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 1� �����
					// 0 - 1
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					// �������� ���������
				//= Result
				LastPollyIndex = CurrentPollyToCheck;
				CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				LastVertexWasOnPlane = false;
				break;
				}else  // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 2� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
				}
			
			if(Vert[1][0] == 2) // ���� ��� 2� �������
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // ���� � ������� ��� ��������� 2� ������� �������� �� ������ ��������� 3� �����
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
				}else  // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 2� �����
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
			
			if (Vert[1][0] == 3) // ���� ��� 3� �������
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 4� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			}
		//switch




		
	}else if(ForwardVerticesCount < BackwardVerticesCount)
	{ // ������� 1
		if(Vert[0][0] == 0) // ����� ������� �����
		{
			
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 1� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
					LastVertexWasOnPlane = false;
					break;
				}else  // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 4� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
		}
			
			if(Vert[0][0] == 1) // ���� ��� 1� �������
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 1� �����
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 2� �����
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
			
			if(Vert[0][0] == 2) // ���� ��� 2� �������
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // ���� � ������� ��� ��������� 2� ������� �������� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 2� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			
				}
			
			if(Vert[0][0] == 3) // ���� ��� 3� �������
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 4� �����
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
// �� ���� �� ������ ����������� - ��� �������� ����� ������������ ������������
return;
}
}
}else
	{// � ������� ��� ������� ��� �� Edge
		/////
		/////
		/////
		/////
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
//CurrentPollyToCheck  ������� ������� ����� ���������

//LastPollyIndex ����� �������� � ������� �������� � ������� ���

if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1 == LastPollyIndex) LastEdgeNum = 1;//LastEdgeNum = ����� ���������� ����� ������� ���������
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2 == LastPollyIndex) LastEdgeNum = 2;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3 == LastPollyIndex) LastEdgeNum = 3;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4 == LastPollyIndex) LastEdgeNum = 4;
WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 ���������� ��������� �  ��������
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;

			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// �������
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// �����
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// ������� �� ���������
				// ���������� ������� �������.
				LastVertexWasOnPlane = true;
				WasOnPlane = true;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				// todo ������������� ���������� ��������
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				LastPollyIndex = CurrentPollyToCheck;
				break;
			}
	}
	if(WasOnPlane) continue;
		if(ForwardVerticesCount > BackwardVerticesCount)
	{ // ����� 1 
		switch(Vert[1][0]) // ����� ������� �����
		{
			case 0: // ���� ��� 0� �������
				if(LastEdgeNum == 4)
				{ // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 1� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 4� �����
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
			
			case 1: // ���� ��� 1� �������
				if(LastEdgeNum == 1)
				{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 2� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 1� �����
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
			
			case 2: // ���� ��� 2� �������
				if(LastEdgeNum == 2)
				{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 2� �����
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
			
			case 3: // ���� ��� 3� �������
				if(LastEdgeNum == 3)
				{ // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 3� �����
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
	{ // ������� 1
		switch(Vert[0][0]) // ����� ������� �����
		{
			case 0: // ���� ��� 0� �������
				if(LastEdgeNum == 4)
				{ // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 1� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				}else  // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 4� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 1: // ���� ��� 1� �������
				if(LastEdgeNum == 1)
				{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 2� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
				}else  // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 1� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 2: // ���� ��� 2� �������
				if(LastEdgeNum == 2)
				{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
				}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 2� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 3: // ���� ��� 3� �������
				if(LastEdgeNum == 3)
				{ // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 4� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
				}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 3� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}else
	{// 2 �������� ������� � 2 �����
		
		switch(Vert[0][0])// ����� ������� �������
		{
			case 0: // ���� ��� 0� �������
				if(Vert[0][1]==1)
				{ // ��������� 2 � 4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
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
				{ // ��������� 1 � 3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
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
			
			case 1: // ���� ��� 1� �������
				if(Vert[0][1]==0)
				{ // ��������� 2 4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==2)
				{ // ��������� 1  3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}
			break;
			
			case 2: // ���� ��� 2� �������
				if(Vert[0][1]==1)
				{ // 1 3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==3)
				{ // ��������� 2 4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					} 
					
				}
			break;
			
			case 3: // ���� ��� 3� �������
				if(Vert[0][1]==0)
				{ // ��������� 1  3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
					
				}else if(Vert[0][1]==2)
				{ // ��������� 2  4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
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
unsigned char Vert[2][4]; // ��� �������.  ��� back � forward ���������  0[..] ������� 1[..]�����
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
unsigned int CurrentPollyToCheck = 0; // ������ ���� ��� -1 ������� ������ ����� ����� ����� -1
unsigned int VertexIndexWorkingWith = VertexToStart;
unsigned int LastVertexCrossed=VertexToStart;
VListValuesNoNext * CurrentVertexAdjencyPollys;
unsigned char LastEdgeNum;  // ����� ������� ��������� � ������� ��� ������������ �������� ��������
unsigned int LastPollyIndex; // ������ �������� � ������� �������� � ������� ���

for(int NumIterationsOnlyForDebug = 0; NumIterationsOnlyForDebug!=100;NumIterationsOnlyForDebug++)
{
	if (VertexIndexWorkingWith == 224)
{
int llkkk = 100;
llkkk++;
}
	if(CurrentPollyToCheck == -1)
	{
		// ��� ������� ������������ ������������
return;
	}

	if(CurrentPollyToCheck == PollyEnd)
	{
				// �������� ���������� ��������.
				return;
	}
		if(LastVertexWasOnPlane)
		{
			if((VertexIndexWorkingWith == VertexEnd1)||(VertexIndexWorkingWith == VertexEnd2)||(VertexIndexWorkingWith == VertexEnd3)||(VertexIndexWorkingWith == VertexEnd4))
			{
				// �������� ���������� ��������.
				return;
			}
for(i=0; i!=MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].NumElements; i++) // �������� �� ���� ��������� � ������� ����������� ������� �������
{// �������� ���� ������� ������� �� �� NavMeshVertices - ����� ����������� ������
// ToDo  ���������� ���� .NumElements - i  ����� ����� �����������
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
CurrentVertexAdjencyPollys = &MeshList[MeshIndex].VertexAdjency[VertexIndexWorkingWith].Massive[i];
CurrentPollyToCheck = CurrentVertexAdjencyPollys->IntegerValue;

WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 ���������� ��������� �  ��������
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;
			if((CurrentVertexToCheckIndexIndex == VertexIndexWorkingWith)||(CurrentVertexToCheckIndexIndex==LastVertexCrossed)) continue; // ���������� ������� �������.
			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// �������
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// �����
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// ������� �� ���������
				// ���������� ������� �������.
				LastVertexWasOnPlane = true;
				LastVertexCrossed = VertexIndexWorkingWith;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				WasOnPlane = true;
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				break;
			}
		}
if(WasOnPlane) break;
if((!ForwardVerticesCount)||(!BackwardVerticesCount)) continue; // ���� ������� �� ���������  ���������� ���������� ��������
if(ForwardVerticesCount==1)
{
if(BackwardVerticesCount==1) continue;
}

if(ForwardVerticesCount > BackwardVerticesCount)
	{ // ����� 1 
		 // ����� ������� �����
		
			if(Vert[1][0] == 0) // ���� ��� 0� �������
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 1� �����
				MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);	// �������� �� ���� �������. ��������� ���������� ��� ��������
				}else  // ���� � ������� ��� ��������� 2� ������� �������� �� ������ ��������� 4� �����
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
			
			if (Vert[1][0] == 1) // ���� ��� 1� �������
				{	
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 1� �����
					// 0 - 1
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					// �������� ���������
				//= Result
				LastPollyIndex = CurrentPollyToCheck;
				CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				LastVertexWasOnPlane = false;
				break;
				}else  // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 2� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
				}
			
			if(Vert[1][0] == 2) // ���� ��� 2� �������
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // ���� � ������� ��� ��������� 2� ������� �������� �� ������ ��������� 3� �����
				MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 2� �����
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
			
			if (Vert[1][0] == 3) // ���� ��� 3� �������
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 4� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			}
		//switch




		
	}else if(ForwardVerticesCount < BackwardVerticesCount)
	{ // ������� 1
		if(Vert[0][0] == 0) // ����� ������� �����
		{
			
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex)
				{ // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 1� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
					LastVertexWasOnPlane = false;
					break;
				}else  // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 4� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
		}
			
			if(Vert[0][0] == 1) // ���� ��� 1� �������
			{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex)
				{ // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 1� �����
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 2� �����
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
			
			if(Vert[0][0] == 2) // ���� ��� 2� �������
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex)
				{ // ���� � ������� ��� ��������� 2� ������� �������� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					//LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					LastVertexWasOnPlane = false;
					break;
				}else  // ���� � ������� ��� ��������� 4� ������� �������� �� ������ ��������� 2� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			
				}
			
			if(Vert[0][0] == 3) // ���� ��� 3� �������
				{
				if(VertexIndexWorkingWith == MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex)
				{ // ���� � ������� ��� ��������� 1� ������� �������� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 3� ������� �������� �� ������ ��������� 4� �����
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
// �� ���� �� ������ ����������� - ��� �������� ����� ������������ ������������
return;
}
}
}else
	{// � ������� ��� ������� ��� �� Edge
		/////
		/////
		/////
		/////
ForwardVerticesCount = 0;
BackwardVerticesCount = 0;
//CurrentPollyToCheck  ������� ������� ����� ���������

//LastPollyIndex ����� �������� � ������� �������� � ������� ���

if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1 == LastPollyIndex) LastEdgeNum = 1;//LastEdgeNum = ����� ���������� ����� ������� ���������
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2 == LastPollyIndex) LastEdgeNum = 2;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3 == LastPollyIndex) LastEdgeNum = 3;
if(MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4 == LastPollyIndex) LastEdgeNum = 4;
WasOnPlane = false;
	for(j = 0; j!=4; j++) // 4 ���������� ��������� �  ��������
	{		
			CurrentVertexToCheckIndexIndex = MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[j].VertexIndex;

			VecToCheck = (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
			Direction=(*PointOnPlane)-(*VecToCheck);
			result = D3DXVec3Dot(&Direction,&RayNormal);
			DistanceToPlane[j] = result;
			if (result >= 0.001f)
			{
				// �������
				Vert[0][ForwardVerticesCount] = j;
				ForwardVerticesCount++;
			}
			else if (result <= -0.001f)
			{// �����
				Vert[1][BackwardVerticesCount] = j;
				BackwardVerticesCount++;
			}
			else
			{// ������� �� ���������
				// ���������� ������� �������.
				LastVertexWasOnPlane = true;
				WasOnPlane = true;
				VertexIndexWorkingWith = CurrentVertexToCheckIndexIndex;
				// todo ������������� ���������� ��������
				LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[CurrentVertexToCheckIndexIndex].VertexX;
				break;
			}
	}
	if(WasOnPlane) continue;
		if(ForwardVerticesCount > BackwardVerticesCount)
	{ // ����� 1 
		switch(Vert[1][0]) // ����� ������� �����
		{
			case 0: // ���� ��� 0� �������
				if(LastEdgeNum == 4)
				{ // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 1� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 4� �����
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
			
			case 1: // ���� ��� 1� �������
				if(LastEdgeNum == 1)
				{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 2� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 1� �����
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
			
			case 2: // ���� ��� 2� �������
				if(LastEdgeNum == 2)
				{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 2� �����
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
			
			case 3: // ���� ��� 3� �������
				if(LastEdgeNum == 3)
				{ // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 3� �����
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
	{ // ������� 1
		switch(Vert[0][0]) // ����� ������� �����
		{
			case 0: // ���� ��� 0� �������
				if(LastEdgeNum == 4)
				{ // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 1� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge1;
				}else  // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 4� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 1: // ���� ��� 1� �������
				if(LastEdgeNum == 1)
				{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 2� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[1] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[1].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge2;
				}else  // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 1� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 2: // ���� ��� 2� �������
				if(LastEdgeNum == 2)
				{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
				}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 2� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
			
			case 3: // ���� ��� 3� �������
				if(LastEdgeNum == 3)
				{ // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 4� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[3] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
				}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 3� �����
				{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
				}
			break;
		}
		LastVertexWasOnPlane = false;
	}else
	{// 2 �������� ������� � 2 �����
		
		switch(Vert[0][0])// ����� ������� �������
		{
			case 0: // ���� ��� 0� �������
				if(Vert[0][1]==1)
				{ // ��������� 2 � 4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
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
				{ // ��������� 1 � 3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
					MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
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
			
			case 1: // ���� ��� 1� �������
				if(Vert[0][1]==0)
				{ // ��������� 2 4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==2)
				{ // ��������� 1  3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[2] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[2].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge3;
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}
			break;
			
			case 2: // ���� ��� 2� �������
				if(Vert[0][1]==1)
				{ // 1 3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
				}else if(Vert[0][1]==3)
				{ // ��������� 2 4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
					Direction =*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX-*(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[3].VertexIndex].VertexX;
					linelength=D3DXVec3Dot(&Direction,&RayNormal);
					percentage = DistanceToPlane[0] / linelength;
					// 1 + direction * Percentage
					LE  = *(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[MeshList[MeshIndex].NavMeshPolygons[CurrentPollyToCheck].PollyVertices[0].VertexIndex].VertexX + (Direction*percentage);
					LastPollyIndex = CurrentPollyToCheck;
					CurrentPollyToCheck = MeshList[MeshIndex].AdjencyBuffer[CurrentPollyToCheck].Edge4;
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					} 
					
				}
			break;
			
			case 3: // ���� ��� 3� �������
				if(Vert[0][1]==0)
				{ // ��������� 1  3
					if(LastEdgeNum == 1)
					{ // ���� � ������� ��� ��������� 1� ����� �� ������ ��������� 3� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 3� ����� �� ������ ��������� 1� �����
					{
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}
					
				}else if(Vert[0][1]==2)
				{ // ��������� 2  4
					if(LastEdgeNum == 2)
					{ // ���� � ������� ��� ��������� 2� ����� �� ������ ��������� 4� �����
MessageBox(0,L"Tsddfsd",L"sdfsdf",MB_OK);
					}else  // ���� � ������� ��� ��������� 4� ����� �� ������ ��������� 2� �����
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
{ // ��� ������� ����� ��������� ������ ��� NavMesh ������� �������� ������ � NavMeshVertices
//1.  ���� ����� ������� �� ��������� �������� ����� � ����.
//RayTrace(16, 272, 15, 240);
//RayTraceVec3((D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,240, 15);
//RayTraceVec3( &D3DXVECTOR3(-11.612207f,0.0f,11.648356f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255);
//RayTraceVec3( &D3DXVECTOR3(-11.612207f,0.0f,11.648356f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX,255, 0);
//RayTraceVec3( (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX,255, 0);


//RayTraceVec3( &D3DXVECTOR3(-11.612207f,0.0f,11.648356f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255); ok // 2 ������� 2 �����
//RayTraceVec3(  &D3DXVECTOR3(11.685318f,0.0f,-11.685318f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX, 255, 0); ok // 2 ������� 2 �����
//RayTraceVec3(  (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240);
//RayTraceVec3(  &D3DXVECTOR3(11.739948f,0.0f,11.590860f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240); ok// 2 ������� 2 �����
//RayTraceVec3(  &D3DXVECTOR3(-11.726042f,0.0f,-11.580701f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,  240,15); ok// 2 ������� 2 �����

	
//RayTraceVec3( (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255);
	//RayTraceVec3( &D3DXVECTOR3(-11.652724f,0.0f,11.641801f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255); //// 2 ������� 2 �����
	//&D3DXVECTOR3(-11.652724f,0.0f,11.641801f)



//RayTraceVec3(  &D3DXVECTOR3(11.695940f,0.0f,-11.664730f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX, 255, 0);
//RayTraceVec3( &D3DXVECTOR3(-11.695940f,0.0f,11.664730f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 288 ].VertexX, 0, 255); // ok 3 c ����� �������  1 � ������

//RayTraceVec3( &D3DXVECTOR3(11.695940f,0.0f,-11.664730f) ,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 0 ].VertexX, 255, 0); // ok 3 c ����� �������  1 � ������


//RayTraceVec3(  &D3DXVECTOR3(11.695940f,0.0f,11.664730f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240); ok

//RayTraceVec3(  (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX,(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 272 ].VertexX, 15, 240);
RayTraceVec3( MeshIndex,  &D3DXVECTOR3(-11.695940f,0.0f,-11.664730f),(D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[ 16 ].VertexX, 240, 15);
return;
float TempG;
	BHeap->Reset();
int i;
D3DXVECTOR3 TempVector;
unsigned int VertexIndex;
unsigned int VertexToCheck; // ���������� ������ ��� ���������� 
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
	for(i = 0 ; i!=4; i++) // ������� 4 �������� �� ���������� �������� � OpenList
	{
		VertexIndex = MeshList[MeshIndex].NavMeshPolygons[PollyStart].PollyVertices[i].VertexIndex;
		
		VertexX = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX; // ��������, ����� ������� ������ ��� ����������
		VertexY = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY;
		VertexZ = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ;


		

		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G  =abs(VertexX - PathStart->x)+ abs(VertexY- PathStart->y) + abs(VertexZ- PathStart->z);//������ ������� � ��������, ��� ����� �������
		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].H = abs(PathEnd->x - VertexX) + abs(PathEnd->y - VertexY)+ abs(PathEnd->z - VertexZ);
		
		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].F  = MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].H;// F = G + H
// ������ ������� � OpenList ���� �������.
		BHeap->AddElementInHeap(MeshIndex, VertexIndex);
		MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexStatus = 1; // ����� ��� ������� � OpenList
	}

	for(;;)
	{
		
	//1. �������� ������ � ���������� ����������
	// ������� �� �� OpenList
	VertexIndex = BHeap->RemoveCheapestElement(MeshIndex);
	
	// ������� �� � ClosedList
	MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexStatus = 2; // 2 = ClosedList
	MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].ParrentIndex;
	LastVertexAddedToClosedListIndex = VertexIndex;
if((VertexIndex == EndVertex1)||(VertexIndex == EndVertex2)||(VertexIndex == EndVertex3)||(VertexIndex == EndVertex4)) break;
	// �������� � ��������� ��������
	//
	ControlValue++;
	if(ControlValue == 0xffffffff) // �������������� - ��������� �������� 5 ��� 10

	{
		for(i = 0; i != MeshList[MeshIndex].NumVertices; i++)
		{
			MeshList[MeshIndex].NavMeshVertices[i].ThisTimeCalculated = 0;
		}
		ControlValue = 1;
	}
	for(i=0; i!=MeshList[MeshIndex].VertexAdjency[VertexIndex].NumElements; i++) // �������� �� ���� ��������� � ������� ����������� ������� �������
	{
		
		switch(MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].VertexNum)
		{
			case 0: // ����� 2� � 1� 3� ��������


			 //  ����������  2� �������	
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[2].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								//if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue)
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						//TempVector = D3DXVECTOR3( , MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
						//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						//TempVector = D3DXVECTOR3(, MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //  ������ ����������  1� �������
 


VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[1].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //  ����������  3� �������	
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[3].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }
				break;
			case 1:// ����� 2� � 0� 3� ��������
			

			 //  ����������  2� �������	
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[2].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }
//0�
 

			
VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[0].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //3�
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[3].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

			break;
			case 2:// ����� 0� � 1� 3� ��������
		VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[0].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //1�
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[1].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //3�
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[3].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }
			break;

			case 3:// ����� 0� � 1� 2� ��������
		VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[0].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //1�
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[1].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
							BHeap->ChangeIttemCost(MeshIndex, VertexToCheck);
						}
					}
				}
 }

 //2�
 VertexToCheck = MeshList[MeshIndex].NavMeshPolygons[ MeshList[MeshIndex].VertexAdjency[VertexIndex].Massive[i].IntegerValue ].PollyVertices[2].VertexIndex; //MeshList[MeshIndex].VertexAdjency[VertexIndex].List[i].IntegerValue ������ �������� � �������� ����������� ���� �������
								
 if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated != ControlValue) // ���� ������ ��� ��������������(��� ������� �������� �������� ������) ���������� ��.
 {
	 MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ThisTimeCalculated = ControlValue;
// ���� ������ � �������� ������ ���������� ��.
				if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus != 2) // ��������  Closed List ����������.
				{
					if(MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus == 0) // ���� �� � Open List
					{
						//������ ������� ������ ������������ ��� ���� ������.
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;
						// ����������� F G H
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ) + MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						//TempVector = D3DXVECTOR3( , PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY, PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						//MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G = 
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H = abs(PathEnd->x -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX) +abs(PathEnd->y -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY) +abs(PathEnd->z -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ);
						MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
						 // ��������� � Open List
						MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
						BHeap->AddElementInHeap(MeshIndex, VertexToCheck);
						
					}else
					{ // ������� ��� � OpenList
												//���������, �� ������� �� ����� ���� ����� ��� ������. ��� ��������� ���������� ��������� G. ����� ������ ��������� G ��������� �� ��, ��� ���� ����� �������.
						TempG  = abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexX -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexX )+abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexY -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexY ) +abs(MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].VertexZ -  MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].VertexZ )+ MeshList[MeshIndex].NavMeshVertices[ VertexIndex ].G;//������ ������� � ��������, ��� ����� �������//  ToDo �������� �� ���������
						
						if(TempG < MeshList[MeshIndex].NavMeshVertices[VertexToCheck].G)
						{
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].ParrentIndex = VertexIndex;//M����� �������� ������ �� ������� ������
							//� ������������� ��� ��� ��������� G � F.
							
							 
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G  = TempG;
							MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].F = MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].H + MeshList[MeshIndex].NavMeshVertices[ VertexToCheck ].G; // F = H + G
							 // ��������� � Open List
							MeshList[MeshIndex].NavMeshVertices[VertexToCheck].VertexStatus = 1; //  � Open List
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