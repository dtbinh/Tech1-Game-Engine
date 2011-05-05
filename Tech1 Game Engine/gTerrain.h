/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef G_TERRAIN
#define G_TERRAIN
#define MaxTerrainTextures 10
struct TERRAINCUSTOMVERTEX
{
    FLOAT x, y, z;
   // DWORD color;
	FLOAT tu, tv;
	FLOAT tu1, tv1;
};

struct MapTile
{
char texturenum;
bool free;

};

#define D3DFVF_TERRAINCUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEX2)
class gTerrain
{
public:
	LPDIRECT3DVERTEXBUFFER9 g_pVB;
	LPDIRECT3DINDEXBUFFER9 TerrainIndexBuffer;
	LPDIRECT3DTEXTURE9   g_pTexture[MaxTerrainTextures];
	MapTile * Map;
	int TerrainXCells;
	int TerrainYCells;
	void Render();
	void LoadTexture(int Stage, WCHAR * TextureFileName);
	gTerrain(int XCells, int YCells, float CellSize, float OffsetX, float OffsetY, float OffsetZ);
	~gTerrain();
};
#endif