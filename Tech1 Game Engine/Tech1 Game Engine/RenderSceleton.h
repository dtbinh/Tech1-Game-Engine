/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef _RENDER_SCELETON_
#define _RENDER_SCELETON_
#define D3DFVF_RENDSCELETONCUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

struct RENDSCELETONCUSTOMVERTEX
{
     FLOAT x, y, z;
	FLOAT tu, tv;
};
class RendererSceleton
{
	OneMesh * MeshList;
	
public:
D3DXVECTOR3 LineStart;
D3DXVECTOR3 LineEnd;
LPDIRECT3DVERTEXBUFFER9 ScelVertexBuffer;
RendererSceleton();
~RendererSceleton();
void render(D3DXVECTOR3 * StartPos, D3DXVECTOR3 * EndPos);
void RenderSkeleton(unsigned int MeshIndex, unsigned int AnimationIndex);
void RenderNormals();

};


#endif