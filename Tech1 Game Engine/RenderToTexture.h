/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _RTT_
#define _RTT_
#include <d3dx9.h>
class cRenderToTexture
{
public:
	//int iWidth;
    //int iHeight;
	//D3DFORMAT DepthFormat;
	//D3DFORMAT ColorFormat;

	LPDIRECT3DTEXTURE9 m_pTexture;
	LPDIRECT3DSURFACE9 m_pSurface;
	LPDIRECT3DTEXTURE9 m_pDSTexture;
	LPDIRECT3DSURFACE9 m_pDSSurface;

	D3DVIEWPORT9 m_Viewport;

  LPDIRECT3DSURFACE9 m_pOldDSSurface;
  LPDIRECT3DSURFACE9 m_pOldRenderTarget;
  D3DVIEWPORT9 m_OldViewport;

	cRenderToTexture::cRenderToTexture();
	cRenderToTexture::~cRenderToTexture();
void	cRenderToTexture::EnableRenderToTexture();
void	cRenderToTexture::DisableRenderToTexture();
void	Create(int iWidth, int iHeight, D3DFORMAT DepthFormat, D3DFORMAT ColorFormat);
};

#endif