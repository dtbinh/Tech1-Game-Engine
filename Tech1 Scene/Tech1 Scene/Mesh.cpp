/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include <stdlib.h>
#include <crtdbg.h>
#include "Mesh.h"
#include "FileIO.h"
//#include "ScriptReader.h"



cMesh::cMesh()
{
	
Mesh.WeightsBufferSize = 0;
Mesh.NumShaders = 0;
Mesh.Mesh.MeshUVs = NULL;
Mesh.Mesh.MeshVertices = NULL;
Mesh.NumMeshes = 0;
Mesh.Mesh.Normals = NULL;
Mesh.Mesh.NextMesh = NULL;
ErrorLoadingMesh = false;
//int FileSize = GetFileSizez(MeshFilePath);
//if(!FileSize) ErrorLoadingMesh = true;
//Files File;
//File.OpenFile(MeshFilePath, "rb");
//MeshFileData = new char[FileSize+1];
//File.ReadFile(MeshFileData, FileSize);
//MeshFileData[FileSize]=0;
//File.CloseFile();
//ScriptReader MeshScript(MeshFileData, &Mesh, true);

}

cMesh::~cMesh()
{
	int CounterDeletedMeshes=0;
	if(Mesh.NumFrames)
	{
		delete [] Mesh.Frames;
		delete [] Mesh.FrameTimes;
	}
	if(Mesh.NumHierarhy)
		delete [] Mesh.Hierarhy;
	
	if(Mesh.WeightsBufferSize)
		delete [] Mesh.WeightsBuffer;
delete [] MeshFileData;
if(Mesh.NumShaders)
delete [] Mesh.Shaders;
Meshes * TempMesh = &Mesh.Mesh;
Meshes * LastMesh = NULL;
for(int i = 0; i!= Mesh.NumMeshes;i++) // Удаляем всё от всех мешей
{

if(TempMesh->MeshVertices)
delete [] TempMesh->MeshVertices;
 // To Do Сделать удаление вертексов от всех мешей, а не только первого
if(TempMesh->Normals)
delete [] TempMesh->Normals;
 // To Do Сделать удаление Нормалей от всех мешей, а не только первого
if(TempMesh->MeshUVs) // Если есть Uvs
for(int i = 0 ; i!=TempMesh->NumUVs; i++)
	{
		delete [] TempMesh->MeshUVs[i].Uvs;
	}
delete [] TempMesh->MeshUVs; // Перед удадением MeshUVs надо удалить его содержимое
for(int i = 0 ; i!=TempMesh->NumPollys; i++)
	{
		for(int j = 0 ; j!=TempMesh->Polygons[i].VertexCount ; j++)
		{
			delete [] TempMesh->Polygons[i].Polly[j].UVIndex;
		}
		
	}
delete [] TempMesh->Polygons; //Сделать удаление поликов от всех мешей а не только от первого.
LastMesh = TempMesh;
TempMesh = TempMesh->NextMesh;
if(CounterDeletedMeshes)  //Первый меш удалять не надо
delete LastMesh;

CounterDeletedMeshes++;
}

}