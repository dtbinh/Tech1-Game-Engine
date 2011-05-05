/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _BOUNDINGVOLUME_
#define _BOUNDINGVOLUME_
#include "common.h"
class BoundingVolume
{
public:
	BoundingVolume();
	void AddBoundingPlane();
	void AddBoundingBox();
	~BoundingVolume();
};
#endif