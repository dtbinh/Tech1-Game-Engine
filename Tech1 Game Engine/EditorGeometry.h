/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _EDITOR_GEOMETRY_
#define _EDITOR_GEOMETRY_
#include "common.h"
#define D3DFVF_EDCUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEX1)

#define CUBESIZE 5.0f

struct EDGEOMETRUCUSTOMVERTEX
{
    FLOAT x, y, z; // Pos
	FLOAT tu, tv; // Tex coord
};


class EditorGeometry
{
public:

void Render();
LPDIRECT3DVERTEXBUFFER9 EDVB;
	EditorGeometry::EditorGeometry();
	EditorGeometry::~EditorGeometry();

};
#endif