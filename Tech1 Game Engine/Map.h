/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _SCENE_MAP_
#define _SCENE_MAP_
#include "Mesh.h"

enum {TYPE_MESH, TYPE_BBOX, TYPE_NAVMESH};
class SceneMap
{
public:
	char * MapFolderName;
	char * MeshFolderName;
	char * MapFileData;

	char * CurrentMapFileName;
	unsigned int CurrentMapNumMeshesTotal;
	//unsigned int CurrentMapNumMeshes;
//	unsigned int CurrentMapNumBBOX;
	//unsigned int CurrentMapNumNavMesh;

	SceneMap();
	~SceneMap();
	void LoadMap(char * MapName);
	bool SceneMap::ParseMapData(char * Data, OneMesh ** MeshList);
	bool LoadMesh(char * Data, unsigned int & NumChildrensLoaded);

	int LastTokenSize;
	int LastTokenStartReadPos;
	int LastTokenStart;
};

#endif