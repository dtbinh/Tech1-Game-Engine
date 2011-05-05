/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "ObjectPicking.h"
#include "common.h"
#include "Mesh.h"
extern gDirectInput * wDirectInput;
extern gPrintInfo * wPrintInfo;
extern gWindow * wWindow;
extern gCamera * wCamera;
extern g3dDevice * w3dDevice;
extern cMesh * Mesh;
ObjectPicking * wObjectPicking;
D3DXVECTOR3 TriangleVertex;

ObjectPicking::ObjectPicking()
{
	MeshList = Mesh->GetMeshes();
}
bool ObjectPicking::Select()
{
D3DXMATRIX matView;
float det;
D3DXVECTOR3 cameraspace;

float NMouseX=1.0f - (2.0f*((float)wDirectInput->CursorPosXClientAreaCoordSys/(float)(wWindow->ClientAreaSizeWidth)));
float NMouseY=1.0f - (2.0f*((float)wDirectInput->CursorPosYClientAreaCoordSys/(float)(wWindow->ClientAreaSizeHeight)));
cameraspace.y=(NMouseY*tan(wCamera->FOV / 2.0f));
cameraspace.x=((-NMouseX*wCamera->AspectRatio)*tan(wCamera->FOV/2.0f));
cameraspace.z=1.0;

LineEnd.x=cameraspace.x*200.0f; // (200=length of ray)
LineEnd.y=cameraspace.y*200.0f;
LineEnd.z=-200.0f; 
//swprintf(wPrintInfo->Console_Output,L"Real numbers:\n   %f %f",NMouseX ,NMouseY );
	
matView = wCamera->ViewMatrix;
D3DXMatrixInverse(&matView,&det,&matView);
D3DXVec3TransformCoord(&LineEnd,&LineEnd,&matView); 
LineStart=wCamera->CameraPos;
if(CheckIntersectionWithMesh(0, Mesh,&LineStart,&LineEnd))
return true;
return false;
}
bool ObjectPicking::CheckIntersectionWithMesh(unsigned int MeshIndex,cMesh * MeshesForCheckIntersection, D3DXVECTOR3 * linestart, D3DXVECTOR3 * lineend)
{
D3DXVECTOR3 IntersectionVertex;
int i, n;
//Meshes * TempMesh = &MeshesForCheckIntersection->Mesh.Mesh;
int VertexIndex;
int NormalIndex;
D3DXVECTOR3 IntersectionPlace;
bool WasIntersection = false;
float dist;
float ShortestDistance=10000.0; 
//for(i = 0; i!= MeshesForCheckIntersection->Mesh.Mesh.NumMeshes;i++) // Удаляем всё от всех мешей
//{
		for(n = 0; n != MeshList[MeshIndex].NumPollys; n++)
		{
				if(MeshList[MeshIndex].NavMesh)
			{
			VertexIndex = MeshList[MeshIndex].NavMeshPolygons[n].PollyVertices[0].VertexIndex;
			NormalIndex = MeshList[MeshIndex].NavMeshPolygons[n].FaceNormalIndex;
			}else
			{
			VertexIndex = MeshList[MeshIndex].Polygons[n].PollyVertices[0].VertexIndex;
			NormalIndex = MeshList[MeshIndex].Polygons[n].FaceNormalIndex;
			}
			//float NorX = MeshList[MeshIndex].Normals[NormalIndex].NormalX;
			//float NorY = MeshList[MeshIndex].Normals[NormalIndex].NormalY;
			//float NorZ = MeshList[MeshIndex].Normals[NormalIndex].NormalZ;
			if(MeshList[MeshIndex].NavMesh)
			{

					if(MGM_GetIntersect(linestart, lineend, (D3DXVECTOR3 *)&MeshList[MeshIndex].NavMeshVertices[VertexIndex].VertexX, (D3DXVECTOR3 *)&MeshList[MeshIndex].Normals[NormalIndex].NormalX,&IntersectionPlace,&dist))
					{//(D3DXVECTOR3 * IntersectionPlace , PollyVertex * PollyVertexIndexes, MeshVertex * MeshVertices)
				

					if(MGM_NavHitPolygon(&IntersectionPlace, MeshList[MeshIndex].NavMeshPolygons[n].PollyVertices,MeshList[MeshIndex].NavMeshVertices))
					{
							if (dist<ShortestDistance) {
							ShortestDistance=dist;
							IntersectionVertex = IntersectionPlace;
							IntersectionPos = TriangleVertex;
							IntersectedPollyIndex = n;
							}
						WasIntersection = true;
		
					}
					}



				
			}else
			{
					if(MGM_GetIntersect(linestart, lineend, (D3DXVECTOR3 *)&MeshList[MeshIndex].MeshVertices[VertexIndex].VertexX, (D3DXVECTOR3 *)&MeshList[MeshIndex].Normals[NormalIndex].NormalX,&IntersectionPlace,&dist))
					{//(D3DXVECTOR3 * IntersectionPlace , PollyVertex * PollyVertexIndexes, MeshVertex * MeshVertices)
				

					if(MGM_HitPolygon(&IntersectionPlace, MeshList[MeshIndex].Polygons[n].PollyVertices,MeshList[MeshIndex].MeshVertices))
					{
							if (dist<ShortestDistance) {
							ShortestDistance=dist;
							IntersectionVertex = IntersectionPlace;
							IntersectionPos = TriangleVertex;
							IntersectedPollyIndex = n;
							}
						WasIntersection = true;
		
					}
					}
			}

		}
		//TempMesh = TempMesh->NextMesh;
	//}
		if(WasIntersection)
		{
			//swprintf(wPrintInfo->Console_Output,L"Intersection detected PollyIndex = %d", IntersectedPollyIndex);



				LineStart = IntersectionVertex;
			return true;

	

		}else
		{
				//swprintf(wPrintInfo->Console_Output,L"No intersection");
				return false;
		}

		}
bool ObjectPicking::MGM_GetIntersect (D3DXVECTOR3 * linestart,D3DXVECTOR3 * lineend,D3DXVECTOR3 * vertex,D3DXVECTOR3 * normal, D3DXVECTOR3 * intersection, float *distance)
{
D3DXVECTOR3 direction,L1;
float linelength,dist_from_plane,percentage;

direction.x=lineend->x-linestart->x; // вычислим вектор направления линий (зеленная линия на рисунке d)
direction.y=lineend->y-linestart->y;
direction.z=lineend->z-linestart->z;
linelength=D3DXVec3Dot(&direction,normal); // Это дает нам длину линии (голубая точка L3 + L4 на рисунке d)
if (fabsf(linelength) < 0.01) // проверяем, не равно ли 0
{
return false; //=0 означает, что линия паралельна плоскости и не может ее пересекать
}
L1.x=vertex->x-linestart->x; // calculate vector L1 (розовая линия на рис. d)
L1.y=vertex->y-linestart->y;
L1.z=vertex->z-linestart->z;
dist_from_plane=D3DXVec3Dot(&L1,normal); // дает дистанцию от плоскости (оранжевая линия L3 на рис. d)
percentage=dist_from_plane/linelength; // Как далеко пересечение линии в процентах от 0 до 1
if (percentage< 0.01 ) // Плоскость позади начала линии
{
return false;
}
else
if (percentage > 1.0) // Линия не достигает плоскости
{
return false;
}
*distance=percentage;
intersection->x=linestart->x+direction.x*percentage; // прибавляем процент линии к линии старта
intersection->y=linestart->y+direction.y*percentage;
intersection->z=linestart->z+direction.z*percentage;
//IntersectionPos= *intersection;
return true;
} 

bool ObjectPicking::MGM_NavHitPolygon (D3DXVECTOR3 * IntersectionPlace , PollyVertex * PollyVertexIndexes, NavMeshVertex * MeshVertices)
{
D3DXVECTOR3 Line0;
D3DXVECTOR3 Line1;
D3DXVECTOR3 Line2;
float total_angles,result,vlength;
//int a;
TriangleVertex =D3DXVECTOR3(MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexX,MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexY,MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexZ) ;

Line0.x=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line0.y=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexY-IntersectionPlace->y;
Line0.z=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexZ-IntersectionPlace->z;

D3DXVec3Normalize((D3DXVECTOR3*)&Line0,(D3DXVECTOR3*)&Line0);

Line1.x=MeshVertices[PollyVertexIndexes[1].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line1.y=MeshVertices[PollyVertexIndexes[1].VertexIndex].VertexY-IntersectionPlace->y;
Line1.z=MeshVertices[PollyVertexIndexes[1].VertexIndex].VertexZ-IntersectionPlace->z;
D3DXVec3Normalize((D3DXVECTOR3*)&Line1,(D3DXVECTOR3*)&Line1);

Line2.x=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line2.y=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexY-IntersectionPlace->y;
Line2.z=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexZ-IntersectionPlace->z;
D3DXVec3Normalize((D3DXVECTOR3*)&Line2,(D3DXVECTOR3*)&Line2);

total_angles=0.0; //начинаем считать эти углы

total_angles= total_angles+acos(D3DXVec3Dot(&Line0,&Line1)); // просчитываем углы между каждым вектором
total_angles= total_angles+acos(D3DXVec3Dot(&Line1,&Line2)); // просчитываем углы между каждым вектором
total_angles= total_angles+acos(D3DXVec3Dot(&Line2,&Line0)); // Последний вектор обработан вне цикла. ( L2 , L0 )

if (fabsf (total_angles-6.28)< 0.05) // пусть 6.28 радиана (360 градусов) дальше от total_angles и с допуском возле нуля попадем ...
{
//попали в многоугольник



return true;
}


//TriangleVertex =D3DXVECTOR3(MeshVertices[PollyVertexIndexes[3].VertexIndex].VertexX,MeshVertices[PollyVertexIndexes[3].VertexIndex].VertexY,MeshVertices[PollyVertexIndexes[3].VertexIndex].VertexZ) ;

Line0.x=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line0.y=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexY-IntersectionPlace->y;
Line0.z=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexZ-IntersectionPlace->z;

D3DXVec3Normalize((D3DXVECTOR3*)&Line0,(D3DXVECTOR3*)&Line0);

Line1.x=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line1.y=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexY-IntersectionPlace->y;
Line1.z=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexZ-IntersectionPlace->z;
D3DXVec3Normalize((D3DXVECTOR3*)&Line1,(D3DXVECTOR3*)&Line1);

Line2.x=MeshVertices[PollyVertexIndexes[3].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line2.y=MeshVertices[PollyVertexIndexes[3].VertexIndex].VertexY-IntersectionPlace->y;
Line2.z=MeshVertices[PollyVertexIndexes[3].VertexIndex].VertexZ-IntersectionPlace->z;
D3DXVec3Normalize((D3DXVECTOR3*)&Line2,(D3DXVECTOR3*)&Line2);

total_angles=0.0; //начинаем считать эти углы

total_angles= total_angles+acos(D3DXVec3Dot(&Line0,&Line1)); // просчитываем углы между каждым вектором
total_angles= total_angles+acos(D3DXVec3Dot(&Line1,&Line2)); // просчитываем углы между каждым вектором
total_angles= total_angles+acos(D3DXVec3Dot(&Line2,&Line0)); // Последний вектор обработан вне цикла. ( L2 , L0 )

if (fabsf (total_angles-6.28)< 0.05) // пусть 6.28 радиана (360 градусов) дальше от total_angles и с допуском возле нуля попадем ...
{
//попали в многоугольник



return true;
}

return false; //не попали в многоугольник
}


bool ObjectPicking::MGM_HitPolygon (D3DXVECTOR3 * IntersectionPlace , PollyVertex * PollyVertexIndexes, MeshVertex * MeshVertices)
{
D3DXVECTOR3 Line0;
D3DXVECTOR3 Line1;
D3DXVECTOR3 Line2;
float total_angles,result,vlength;
//int a;
TriangleVertex =D3DXVECTOR3(MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexX,MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexY,MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexZ) ;

Line0.x=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line0.y=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexY-IntersectionPlace->y;
Line0.z=MeshVertices[PollyVertexIndexes[0].VertexIndex].VertexZ-IntersectionPlace->z;

D3DXVec3Normalize((D3DXVECTOR3*)&Line0,(D3DXVECTOR3*)&Line0);

Line1.x=MeshVertices[PollyVertexIndexes[1].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line1.y=MeshVertices[PollyVertexIndexes[1].VertexIndex].VertexY-IntersectionPlace->y;
Line1.z=MeshVertices[PollyVertexIndexes[1].VertexIndex].VertexZ-IntersectionPlace->z;
D3DXVec3Normalize((D3DXVECTOR3*)&Line1,(D3DXVECTOR3*)&Line1);

Line2.x=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexX-IntersectionPlace->x; // получить вектор направления из пересечения с вершиной [a]
Line2.y=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexY-IntersectionPlace->y;
Line2.z=MeshVertices[PollyVertexIndexes[2].VertexIndex].VertexZ-IntersectionPlace->z;
D3DXVec3Normalize((D3DXVECTOR3*)&Line2,(D3DXVECTOR3*)&Line2);

total_angles=0.0; //начинаем считать эти углы

total_angles= total_angles+acos(D3DXVec3Dot(&Line0,&Line1)); // просчитываем углы между каждым вектором
total_angles= total_angles+acos(D3DXVec3Dot(&Line1,&Line2)); // просчитываем углы между каждым вектором
total_angles= total_angles+acos(D3DXVec3Dot(&Line2,&Line0)); // Последний вектор обработан вне цикла. ( L2 , L0 )

if (fabsf (total_angles-6.28)< 0.05) // пусть 6.28 радиана (360 градусов) дальше от total_angles и с допуском возле нуля попадем ...
{
//попали в многоугольник



return true;
}
return false; //не попали в многоугольник
}

ObjectPicking::~ObjectPicking()
{

}
