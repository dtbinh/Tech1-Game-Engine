/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef G_DEVICE
#define G_DEVICE



class g3dDevice
{
public:

LPDIRECT3D9             g_pD3D;
LPDIRECT3DDEVICE9       g_pd3dDevice;

	g3dDevice();
	~g3dDevice();
};
#endif