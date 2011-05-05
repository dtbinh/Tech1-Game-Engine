/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _OBJECT_PICKING_
#define _OBJECT_PICKING_
#include "common.h"
class ObjectPicking
{
OneMesh * MeshList;
public:

		int IntersectedPollyIndex;

	D3DXVECTOR3 LineStart;
	D3DXVECTOR3 LineEnd;

	D3DXVECTOR3 IntersectionPos;
	bool MGM_GetIntersect (D3DXVECTOR3 * linestart,D3DXVECTOR3 * lineend,D3DXVECTOR3 * vertex,D3DXVECTOR3 * normal, D3DXVECTOR3 * intersection, float *distance);
	ObjectPicking();
	bool Select();
	bool CheckIntersectionWithMesh(unsigned int MeshIndex, cMesh * MeshesForCheckIntersection, D3DXVECTOR3 * linestart, D3DXVECTOR3 * lineend);
	~ObjectPicking();
	bool MGM_HitPolygon (D3DXVECTOR3 * plane_intersection , PollyVertex * PollyVertexIndexes, MeshVertex * MeshVertices);
	bool MGM_NavHitPolygon (D3DXVECTOR3 * plane_intersection , PollyVertex * PollyVertexIndexes, NavMeshVertex * MeshVertices);
};
#endif