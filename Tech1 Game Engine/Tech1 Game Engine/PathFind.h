/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _PATH_FIND_
#define _PATH_FIND_
class PathFind
{
	OneMesh * MeshList;
public:
	unsigned int LastVertexAddedToClosedListIndex; // �� �������. ������
	unsigned int ControlValue; // ��� ���� ���� ��������� �� �������������� �� ��� �������� ����.
	// ������ ��� ����� ��������� �������� ��� ������������� �� 1
	PathFind();
	void FindPath(unsigned int MeshIndex, D3DXVECTOR3 * PathStart, D3DXVECTOR3 * PathEnd, int PollyStart, int PollyEnd);
	void RayTrace(unsigned int MeshIndex, unsigned int VertexToStart, unsigned int VertexToEnd, unsigned int PollyStart,unsigned int PollyEnd);
	~PathFind();
	void RayTraceVec3(unsigned int MeshIndex, D3DXVECTOR3 * VertexToStart, D3DXVECTOR3 * VertexToEnd, unsigned int PollyStart,unsigned int PollyEnd);
};
#endif