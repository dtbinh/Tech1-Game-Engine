/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "EditorGeometry.h"
extern g3dDevice * w3dDevice;
extern gCamera * wCamera;
extern Shaders * GameShaders;
extern float ShadowVolumeLightPos[3];
EditorGeometry * EDgeom;

EditorGeometry::EditorGeometry()
{ 
	EDGEOMETRUCUSTOMVERTEX q_Vertices[] =
    {
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2] ,0.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2],1.0f, 0.0f},
		{-(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2],0.0f, 1.0f},

		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1],(CUBESIZE/2)+ ShadowVolumeLightPos[2] ,1.0f, 0.0f},
		{-(CUBESIZE/2)+ ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ ShadowVolumeLightPos[2] ,1.0f, 1.0f},
		{-(CUBESIZE/2)+ ShadowVolumeLightPos[0],-(CUBESIZE/2)+ShadowVolumeLightPos[1]+ CUBESIZE, (CUBESIZE/2)+ ShadowVolumeLightPos[2],0.0f, 1.0f},


		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ ShadowVolumeLightPos[1],(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE ,0.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE,0.0f, 1.0f},
		{-(CUBESIZE/2)+ ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ ShadowVolumeLightPos[1],(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE,1.0f, 0.0f},

		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE ,0.0f, 1.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 1.0f},


		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2] ,0.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1],(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE,0.0f, 1.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2],1.0f, 0.0f},

		{-(CUBESIZE/2)+ ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1],(CUBESIZE/2)+ShadowVolumeLightPos[2] ,1.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE ,0.0f, 1.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1],(CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 1.0f},

		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2] ,0.0f, 0.0f},
		{-(CUBESIZE/2)+ ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2],1.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ShadowVolumeLightPos[1]+ CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE,0.0f, 1.0f},

		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2] ,1.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 1.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE ,0.0f, 1.0f},


		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2] ,0.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2],0.0f, 1.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE,1.0f, 0.0f},

		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0],-(CUBESIZE/2)+ ShadowVolumeLightPos[1]+CUBESIZE, (CUBESIZE/2)+ShadowVolumeLightPos[2] ,0.0f, 1.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0], -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 1.0f},

		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2] ,0.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ ShadowVolumeLightPos[1], (CUBESIZE/2)+ShadowVolumeLightPos[2]-CUBESIZE,1.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE,-(CUBESIZE/2)+ ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ShadowVolumeLightPos[2],0.0f, 1.0f},

		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1],(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 0.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ ShadowVolumeLightPos[2]-CUBESIZE ,1.0f, 1.0f},
		{ -(CUBESIZE/2)+ShadowVolumeLightPos[0]+CUBESIZE, -(CUBESIZE/2)+ShadowVolumeLightPos[1]+CUBESIZE,(CUBESIZE/2)+ ShadowVolumeLightPos[2] ,0.0f, 1.0f},
	
        
    };

	
w3dDevice->g_pd3dDevice->CreateVertexBuffer( 36 * sizeof( EDGEOMETRUCUSTOMVERTEX ),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_EDCUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &EDVB, NULL );

    VOID* pVertices;
    EDVB->Lock( 0, 36 * sizeof( TERRAINCUSTOMVERTEX ), ( void** )&pVertices, 0 );
    memcpy( pVertices, (void *)&q_Vertices, 36 * sizeof( EDGEOMETRUCUSTOMVERTEX ) );
   
	EDVB->Unlock();
	
}

void EditorGeometry::Render()
{
GameShaders->SetShader(VertexMeshRendering);
GameShaders->SetShader(PixelMeshRendering);
w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclarationMeshRendering);

//wCamera->SetZeroMeshMatrix();
wCamera->SetIdentityWorldMatrix();
GameShaders->SetMatrixForShader(VertexMeshRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());

w3dDevice->g_pd3dDevice->SetStreamSource( 0, EDVB, 0, sizeof( EDGEOMETRUCUSTOMVERTEX ));
w3dDevice->g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,12);
}

EditorGeometry::~EditorGeometry()
{

}