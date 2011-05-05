/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _TEXTURE_
#define _TEXTURE_



class cTexture
{
private:
	
	unsigned int NumTextures; // Количество загруженных с файлов текстур
public:
	LPDIRECT3DTEXTURE9 * g_pTexture;
	cTexture::cTexture(unsigned int MaxTexturesInCurrentScene);
	cTexture::~cTexture();
	
	int LoadTexture(char * TextureFileName);
};

#endif