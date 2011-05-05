/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _QUAD_
#define _QUAD_
#define SMQUAD_SIZE  100.0f

struct SMVertex
{
    float x, y, z;
   float u, v;
    enum FVF
    {
        FVF_Flags = D3DFVF_XYZ  | D3DFVF_TEX0
    };
};
class QUAD
{
public:

	LPDIRECT3DVERTEXBUFFER9 pVB;
    LPDIRECT3DINDEXBUFFER9 pIB;
    DWORD dwNumVerts;
    DWORD dwNumFaces;
    D3DPRIMITIVETYPE primType;
    D3DXVECTOR3 scaleVec;
    D3DXVECTOR3 transVec;

void QUAD::Render();
	QUAD::QUAD();
	QUAD::~QUAD();
};

#endif