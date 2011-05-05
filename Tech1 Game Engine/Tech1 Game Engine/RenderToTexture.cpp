/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "RenderToTexture.h"
#include "common.h"
extern g3dDevice * w3dDevice;
cRenderToTexture * RenderToTexture;
cRenderToTexture::cRenderToTexture()
{
  m_pTexture=NULL;
  m_pSurface=NULL;
  m_pDSSurface=NULL;
  m_pOldDSSurface=NULL;
  m_pOldRenderTarget=NULL;
  m_pDSTexture=NULL;
}

cRenderToTexture::~cRenderToTexture()
{

}


void cRenderToTexture::EnableRenderToTexture()
{
  w3dDevice->g_pd3dDevice->GetViewport(&m_OldViewport);
  w3dDevice->g_pd3dDevice->GetRenderTarget(0,&m_pOldRenderTarget);
  w3dDevice->g_pd3dDevice->GetDepthStencilSurface(&m_pOldDSSurface);

  w3dDevice->g_pd3dDevice->SetViewport(&m_Viewport);
  w3dDevice->g_pd3dDevice->SetRenderTarget(0,m_pSurface);
  w3dDevice->g_pd3dDevice->SetDepthStencilSurface(m_pDSSurface);
}

void cRenderToTexture::DisableRenderToTexture()
{
 w3dDevice->g_pd3dDevice->SetDepthStencilSurface(m_pOldDSSurface);

  // releasing is necessary due to reference counting
  if(m_pOldDSSurface!=NULL) {
    m_pOldDSSurface->Release();
    m_pOldDSSurface=NULL;
  }

  // Restore old render target
  //
  w3dDevice->g_pd3dDevice->SetRenderTarget(0,m_pOldRenderTarget);

  // releasing is necessary due to reference counting
  if(m_pOldRenderTarget!=NULL) {
    m_pOldRenderTarget->Release();
    m_pOldRenderTarget=NULL;
  }

  // Restore old viewport
  //
  w3dDevice->g_pd3dDevice->SetViewport(&m_OldViewport);
}

void cRenderToTexture::Create(int iWidth, int iHeight, D3DFORMAT DepthFormat, D3DFORMAT ColorFormat)
{
if(FAILED(w3dDevice->g_pd3dDevice->CreateTexture(iWidth,iHeight,1,D3DUSAGE_RENDERTARGET,ColorFormat,D3DPOOL_DEFAULT,&m_pTexture,NULL)))
  {
    MessageBox(NULL,TEXT("Creating render texture failed!"),TEXT("Error!"),MB_OK);
   
  }

  if(FAILED( m_pTexture->GetSurfaceLevel(0,&m_pSurface))) {
    MessageBox(NULL,TEXT("GetSurfaceLevel failed!"),TEXT("Error!"),MB_OK);
  }


  
    if(FAILED(w3dDevice->g_pd3dDevice->CreateTexture(iWidth,iHeight,1,D3DUSAGE_DEPTHSTENCIL,DepthFormat,D3DPOOL_DEFAULT,&m_pDSTexture,NULL))) {
      MessageBox(NULL,TEXT("CreateTexture for depth stencil failed!"),TEXT("Error!"),MB_OK);
    }


    if(FAILED(m_pDSTexture->GetSurfaceLevel(0,&m_pDSSurface))) {
      MessageBox(NULL,TEXT("GetSurfaceLevel for depth stencil texture failed!"),TEXT("Error!"),MB_OK);
    }

  m_Viewport.X = 0;
  m_Viewport.Y = 0;
  m_Viewport.Width  = iWidth;
  m_Viewport.Height = iHeight;
  m_Viewport.MinZ = 0.0f;
  m_Viewport.MaxZ = 1.0f;


}