/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"

cTexture * Texture;
extern g3dDevice * w3dDevice;
// MaxTexturesInCurrentScene  количество текстур в сцене
cTexture::cTexture(unsigned int MaxTexturesInCurrentScene)
{
g_pTexture = new LPDIRECT3DTEXTURE9[MaxTexturesInCurrentScene];
NumTextures = 0;
}

//¬озвращает индекс загруженной текстуры
int cTexture::LoadTexture(char * TextureFileName)
{
	unsigned int FileSize =  GetFileSizez(Str("Data\\Textures\\%s" , TextureFileName));
	if(FileSize == 0xFFFFFFFF)
	{	
		MessageBoxA(0, Str("Error loading texture \"%s\" :: File does not exists. ",  TextureFileName)  ,"Error", MB_OK);
		return-1;
	}
if( FAILED( D3DXCreateTextureFromFileA( w3dDevice->g_pd3dDevice, Str("Data\\Textures\\%s" , TextureFileName), &g_pTexture[NumTextures] ) ) )
    {
		MessageBoxA(0, Str("Error loading texture \"%s\" :: File is corrupted.",  TextureFileName)  ,"Error", MB_OK);
		return -1;
	}
NumTextures++;
return (NumTextures-1);
}

cTexture::~cTexture()
{
delete [] g_pTexture;
g_pTexture = NULL;
}