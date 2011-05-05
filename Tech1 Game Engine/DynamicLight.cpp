#include "DynamicLight.h"

/*
void DynamicLight::BuildDynamicLights(Meshes * RootMesh, int NumMeshes)
{
Meshes * TempMesh = RootMesh;
float s_dist, t_dist;
float TempVector1[3];
float TempVector2[3];
for(int i = 0; i!= NumMeshes;i++)
	{
		if(TempMesh->ConnectedToBone)
		{

			for(int j = 0; j!= TempMesh->NumPollys; j++)
			{
				 VectorSubstract( &TempMesh->MeshVertices[TempMesh->Polygons[j].PollyVertices[1].VertexIndex].VertexX, &TempMesh->MeshVertices[TempMesh->Polygons[j].PollyVertices[0].VertexIndex].VertexX,&TempVector1[0]);
				 VectorSubstract( &TempMesh->MeshVertices[TempMesh->Polygons[j].PollyVertices[2].VertexIndex].VertexX, &TempMesh->MeshVertices[TempMesh->Polygons[j].PollyVertices[0].VertexIndex].VertexX,&TempVector2[0]);
				 //sy = TempMesh->MeshVertices[TempMesh->Polygons[j].Polly[2].VertexIndex].VertexY - TempMesh->MeshVertices[TempMesh->Polygons[j].Polly[0].VertexIndex].VertexY;
				s_dist = sqrt(dot_product(&TempVector1[0],&TempVector1[0]));
				t_dist = sqrt(dot_product(&TempVector1[1],&TempVector1[1]));

			}
		}
		RootMesh = RootMesh->NextMesh;
	}
}
*/
DynamicLight::DynamicLight()
{

}

DynamicLight::~DynamicLight()
{

}