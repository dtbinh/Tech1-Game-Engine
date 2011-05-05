/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _SHADOW_
#define _SHADOW_
#define SILUETTE_LENGH 10.0f
struct SHADOW_CUSTOMVERTEX
{
    FLOAT x, y, z;
};
#define D3DFVF_SHADOW_CUSTOMVERTEX (D3DFVF_XYZ)

void ProcessAllPollysToFindFrontCulledPollys(unsigned int MeshIndex);
void GoThrowAllFrontCulledPollys(unsigned int MeshIndex, int FirstFrontCulled);
void RenderSiluete();
#endif