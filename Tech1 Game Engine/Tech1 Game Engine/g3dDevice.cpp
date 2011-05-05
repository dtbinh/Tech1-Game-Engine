/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "g3dDevice.h"

g3dDevice * w3dDevice;
extern gWindow * wWindow;
g3dDevice::g3dDevice()
{
		g_pD3D = NULL;
		g_pd3dDevice = NULL;



		g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
      


    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
	if(wWindow->WindowMode) d3dpp.Windowed = TRUE;
	else d3dpp.Windowed = FALSE;


	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; 
	if(wWindow->WindowMode)
	{
		d3dpp.BackBufferWidth = (wWindow->WindowClientAreaEndPosX - 1) - wWindow->WindowClientAreaStartPosX;
		d3dpp.BackBufferHeight = (wWindow->WindowClientAreaEndPosY - 1) - wWindow->WindowClientAreaStartPosY;
	}else
	{
		d3dpp.BackBufferWidth = wWindow->ScreenWidth;    // set the width of the buffer
		d3dpp.BackBufferHeight = wWindow->ScreenHeight;    // set the height of the buffer
	}
	
	d3dpp.BackBufferCount = 1;
	//d3dpp.Flags						= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,wWindow-> hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice );


g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,D3DCULL_CW );
  
g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

   g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
}

g3dDevice::~g3dDevice()
{


    if( w3dDevice->g_pd3dDevice != NULL )
        w3dDevice->g_pd3dDevice->Release();

    if( w3dDevice->g_pD3D != NULL )
        w3dDevice->g_pD3D->Release();
		g_pD3D = NULL;
		g_pd3dDevice = NULL;
		
}