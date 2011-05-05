/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#include "SpecularBumpMapping.h"
extern g3dDevice * w3dDevice;
SpecularBumpMapping::SpecularBumpMapping()
{



QUADVERTEX q_Vertices[] =
    {
		{ 0.0f, 0.0f, -1.0f ,0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 10.0f, 0.0f, -1.0f,0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 10.0f, -1.0f,0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },

		{ 10.0f, 0.0f, -1.0f ,0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ 10.0f, 10.0f, -1.0f ,0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f },
		{ 0.0f, 10.0f, -1.0f ,0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
	
        
    };



D3DVERTEXELEMENT9 decl[]=
{
//stream, offset, type, method, semantic type
{0,0, D3DDECLTYPE_FLOAT4,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
{0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,  0},
{0,24,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0},
{0,32,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TANGENT, 0},
D3DDECL_END()
};
w3dDevice->g_pd3dDevice->CreateVertexDeclaration(decl,&VertexDeclaration);
D3DXCreateMesh(2,6,D3DXMESH_SYSTEMMEM,decl,w3dDevice->g_pd3dDevice,&MeshQuad);
D3DXComputeNormals(MeshQuad,NULL);
D3DXComputeTangent(MeshQuad,0,0,0,TRUE,NULL);

LPVOID * p_Vertices=NULL;
MeshQuad->LockVertexBuffer(0,p_Vertices);
memcpy(p_Vertices,&q_Vertices,sizeof(q_Vertices));
MeshQuad->UnlockVertexBuffer();

}

SpecularBumpMapping::~SpecularBumpMapping()
{

}