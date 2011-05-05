/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
extern g3dDevice * w3dDevice;
extern  gWindow * wWindow;
extern Shaders * GameShaders;
gTerrain * wTerrain;
extern gLog * wLog;
extern gCamera * wCamera;
gTerrain::gTerrain(int XCells, int YCells, float CellSize, float OffsetX, float OffsetY, float OffsetZ)
{
	int i;
	Map = new MapTile[XCells*YCells];
	for(i = 0; i != (XCells * YCells);i++)
	{
		Map[i].texturenum=0;
		Map[i].free = true;
	}

	//---------------
Map[1].texturenum = 1;
Map[2].texturenum = 1;
	//-------------
g_pVB = NULL;
for(i = 0; i != MaxTerrainTextures; i++)
{
g_pTexture[i] = NULL;
}
TerrainXCells = XCells;
TerrainYCells = YCells;
int XNumVertex, YNumVertex;
TERRAINCUSTOMVERTEX * TerrainMemoryVertex = new TERRAINCUSTOMVERTEX[((XCells*2)+2)*(YCells)];
for(YNumVertex = 0; YNumVertex != (YCells*2); YNumVertex+=2)
{
	for(XNumVertex = 0; XNumVertex != (XCells+1)*2; XNumVertex+=2)
	{
		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))+1].x = OffsetX+(CellSize*((float)XNumVertex)/2.0f);
		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))+1].y = OffsetY+(CellSize*(float)(YNumVertex/2));
		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))+1].z = OffsetZ;
	//	TerrainMemoryVertex[XNumVertex+(YNumVertex*(XCells+1)*2)].color = 0xff00ffff;

		//float x = (float)XNumVertex/(((float)XCells+1.0f)*2.0f);

TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))+1].tu =(((((float)XNumVertex)/2.0f)/(float)XCells))*2.0f;
		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))+1].tv = ((1.0f-((((float)YNumVertex+2.0f)/((float)YCells*2.0f))-((2.0f)/((float)YCells*2.0f)))))*2.0f;

TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))+1].tu1 =(((((float)XNumVertex)/2.0f)/(float)XCells));
		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))+1].tv1 = ((1.0f-((((float)YNumVertex+2.0f)/((float)YCells*2.0f))-((2.0f)/((float)YCells*2.0f)))));

		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))].x = OffsetX+(CellSize*((float)XNumVertex)/2.0f);
		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))].y = OffsetY+(CellSize*(float)(YNumVertex/2)+(CellSize));
		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))].z = OffsetZ;

		TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))].tu = ((((float)XNumVertex)/2.0f)/(float)XCells)*2.0f;
			TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))].tv =(1.0f-(((float)YNumVertex+2.0f)/((float)YCells*2.0f)))*2.0f;
	
					TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))].tu1 = ((((float)XNumVertex)/2.0f)/(float)XCells);
			TerrainMemoryVertex[(XNumVertex+((YNumVertex/2)*(XCells+1)*2))].tv1 =(1.0f-(((float)YNumVertex+2.0f)/((float)YCells*2.0f)));
			//if(YNumVertex!=(YCells-1))
		//TerrainMemoryVertex[(XNumVertex+(YNumVertex*(XCells+1)*2))+1].tv = (1.0f-(((float)YNumVertex+1.0f) / (float)(YCells-1)))+(((((float)1.0f) / (float)(YCells-1))));
		//else
		//	TerrainMemoryVertex[(XNumVertex+(YNumVertex*(XCells+1)*2))+1].tv =((float)YNumVertex+2.0f)/((float)YCells*2.0f);
	
		//TerrainMemoryVertex[(XNumVertex+(YNumVertex*(XCells+1)*2))+1].color = 0xff0000ff;
	}
}



   w3dDevice->g_pd3dDevice->CreateVertexBuffer(((XCells*2)+2)*(YCells) * sizeof( TERRAINCUSTOMVERTEX ),
                                                  D3DUSAGE_WRITEONLY, D3DFVF_TERRAINCUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL );



    VOID* pVertices;
    g_pVB->Lock( 0, ((XCells*2)+2)*(YCells) * sizeof( TERRAINCUSTOMVERTEX ), ( void** )&pVertices, 0 );
    memcpy( pVertices, TerrainMemoryVertex, ((XCells*2)+2)*(YCells) * sizeof( TERRAINCUSTOMVERTEX ) );
   
	g_pVB->Unlock();

delete TerrainMemoryVertex;



int * IndexBuffer = new int[((XCells*2)+2)*YCells];


for(YNumVertex = 0; YNumVertex != YCells; YNumVertex++)
{
	for(XNumVertex = 0; XNumVertex != ((XCells*2)+2); XNumVertex++)
	{
		IndexBuffer[XNumVertex+(YNumVertex*XCells)] = XNumVertex+(YNumVertex*XCells);
	}

}

w3dDevice->g_pd3dDevice->CreateIndexBuffer(((XCells*2)+2)*YCells*sizeof(WORD),  D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &TerrainIndexBuffer, NULL);



WORD* pIndex2;


TerrainIndexBuffer->Lock(0,0,(void**)&pIndex2,0);

memcpy(pIndex2,IndexBuffer,((XCells*2)+2)*YCells*sizeof(WORD));
TerrainIndexBuffer->Unlock();
delete IndexBuffer;





}
void gTerrain::LoadTexture(int Stage, WCHAR * TextureFileName)
{
if( FAILED( D3DXCreateTextureFromFile( w3dDevice->g_pd3dDevice, TextureFileName, &g_pTexture[Stage] ) ) )
    {
		wLog->addMessage(L"Error load texture %s",TextureFileName);
	}
}

void gTerrain::Render()
{
       // D3DXMATRIXA16 matWorld;


    //UINT iTime = timeGetTime() % 1000;
   // FLOAT fAngle = iTime * ( 2.0f * D3DX_PI ) / 1000.0f;
   // D3DXMatrixRotationY( &matWorld, fAngle );
   // w3dDevice->g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );



//for(int i = 0; i != MaxTerrainTextures; i++)
//{
//if(g_pTexture[i])
w3dDevice->g_pd3dDevice->SetTexture(0,g_pTexture[0]);
//}
//wCamera->SetZeroMeshMatrix();
wCamera->SetIdentityWorldMatrix();
GameShaders->SetMatrixForShader(VertexFinalRendering, "mat_mvp", wCamera->GetWorldViewProjectionCombined());

        GameShaders->SetShader(VertexFinalRendering);
		GameShaders->SetShader(PixelFinalRendering);
		w3dDevice->g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( TERRAINCUSTOMVERTEX ) );
		w3dDevice->g_pd3dDevice->SetVertexDeclaration(GameShaders->VertexDeclaration);
       // w3dDevice->g_pd3dDevice->SetFVF( D3DFVF_TERRAINCUSTOMVERTEX );
		//w3dDevice->g_pd3dDevice->SetIndices(TerrainIndexBuffer);
		for(int i = 0; i != TerrainYCells; i++)
		{
        w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, i*((TerrainXCells * 2)+2), 2*TerrainXCells);
		}

//w3dDevice->g_pd3dDevice->SetPixelShader(NULL);
//w3dDevice->g_pd3dDevice->SetVertexShader(NULL);
}

gTerrain::~gTerrain()
{
	for(int i = 0; i != MaxTerrainTextures; i++)
{
if(g_pTexture[i])
g_pTexture[i]->Release();
}
	if( g_pVB != NULL )
        g_pVB->Release();
	g_pVB = NULL;
	if( TerrainIndexBuffer != NULL )
        TerrainIndexBuffer->Release();
	TerrainIndexBuffer = NULL;
	delete [] Map;
}


/*

#include "common.h"
extern g3dDevice * w3dDevice;
extern  gWindow * wWindow;
gTerrain * wTerrain;
gTerrain::gTerrain(int XCells, int YCells, float CellSize, float OffsetX, float OffsetY, float OffsetZ)
{

g_pVB = NULL;
TERRAINCUSTOMVERTEX g_Vertices[] =
    {
        { -1.0f,-1.0f, 0.0f, 0xffff0000, },
        {  1.0f,-1.0f, 0.0f, 0xff0000ff, },
        {  0.0f, 1.0f, 0.0f, 0xffffffff, },
    };


   w3dDevice->g_pd3dDevice->CreateVertexBuffer( 3 * sizeof( TERRAINCUSTOMVERTEX ),
                                                  0, D3DFVF_TERRAINCUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL );



    VOID* pVertices;
    g_pVB->Lock( 0, sizeof( g_Vertices ), ( void** )&pVertices, 0 );
    memcpy( pVertices, g_Vertices, sizeof( g_Vertices ) );
    g_pVB->Unlock();

}


void gTerrain::Render()
{
       // D3DXMATRIXA16 matWorld;


    //UINT iTime = timeGetTime() % 1000;
   // FLOAT fAngle = iTime * ( 2.0f * D3DX_PI ) / 1000.0f;
   // D3DXMatrixRotationY( &matWorld, fAngle );
   // w3dDevice->g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );






 


        w3dDevice->g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof( TERRAINCUSTOMVERTEX ) );
        w3dDevice->g_pd3dDevice->SetFVF( D3DFVF_TERRAINCUSTOMVERTEX );
        w3dDevice->g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
}

gTerrain::~gTerrain()
{
	if( g_pVB != NULL )
        g_pVB->Release();
	g_pVB = NULL;
}

*/