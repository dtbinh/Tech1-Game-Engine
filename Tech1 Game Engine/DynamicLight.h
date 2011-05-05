#ifndef _DYNAMIC_LIGHT_
#define _DYNAMIC_LIGHT_
#include "common.h"
#define DYNAMIC_LIGHTMAP_SIZE 16

struct surface {
		float vertices[4][3];
		float matrix[9];

		float s_dist, t_dist;
	};
static float light_pos[3] = { 0.0f, 0.0f, 0.0f };
	static float light_color[3] = { 1.0f, 1.0f, 1.0f };

class DynamicLight
{
//void BuildDynamicLights( int NumMeshes);
	 DynamicLight();
	~DynamicLight();
};

#endif