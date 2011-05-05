/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _SPECULAR_BUMP_MAPPING_
#define _SPECULAR_BUMP_MAPPING_

	struct QUADVERTEX
{
    FLOAT x, y, z, w; // Pos
	FLOAT NormalX, NormalY, NormalZ; // Normal
	FLOAT tu, tv; // Tex coord
	FLOAT TangentX, TangentY, TangentZ; // Normal
};
class SpecularBumpMapping
{
SpecularBumpMapping();
~SpecularBumpMapping();
IDirect3DVertexDeclaration9* VertexDeclaration;
LPD3DXMESH MeshQuad;
};



#endif