/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"

extern cMesh * Mesh;  // Обязательно добавить возможность работы с несколькими мешами а не одним
BinaryHeap * BHeap;

//extern Varray VertexArray[VERTEX_ARRAY_SIZE];
BinaryHeap::BinaryHeap()
{
NumElementsInHeap = 0;
OneMesh * MeshList = Mesh->GetMeshes();
}

void BinaryHeap::PrintAllElements()
{/*
	{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data,"--------------------\n");
	fputs((const char *)&data,file);
	fclose(file);
	}
	int i;
	for(i = 1; i != NumElementsInHeap+1; i++)
	{
		//std::cout << "Cost : " << Heap[i].Fcost << " index : " << Heap[i].IndexInVertexArray << "\n";
		
		
		{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data,"Print int heap  Index %d  g %f h %f f %f  bheap index %d parrent index %d vertex array index %d\n", Heap[i].IndexInVertexArray,Mesh->Mesh.Mesh.NavMeshVertices[Heap[i].IndexInVertexArray].G,Mesh->Mesh.Mesh.NavMeshVertices[Heap[i].IndexInVertexArray].H,Mesh->Mesh.Mesh.NavMeshVertices[Heap[i].IndexInVertexArray].F,  Mesh->Mesh.Mesh.NavMeshVertices[Heap[i].IndexInVertexArray].IndexInBinaryHeap,Mesh->Mesh.Mesh.NavMeshVertices[Heap[i].IndexInVertexArray].ParrentIndex, Heap[i].IndexInVertexArray);
	fputs((const char *)&data,file);
	fclose(file);
	}

		
	}
	{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data,"--------------------\n");
	fputs((const char *)&data,file);
	fclose(file);
	}
	*/
}


void BinaryHeap::AddElementInHeap(unsigned int MeshIndex, unsigned int ElementIndex)
{
	
	/*
														{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data,"Add element  Index %d  G %f H %f F %f\n", ElementIndex,Mesh->Mesh.Mesh.NavMeshVertices[ElementIndex].G,Mesh->Mesh.Mesh.NavMeshVertices[ElementIndex].H,Mesh->Mesh.Mesh.NavMeshVertices[ElementIndex].F);
	fputs((const char *)&data,file);
	fclose(file);
	}
*/
	ElementBHeap  TempIttem; 

	unsigned int CurrentIttemToCheck = ++NumElementsInHeap, HalfSize;
	Heap[NumElementsInHeap].IndexInVertexArray = ElementIndex;
	Heap[NumElementsInHeap].Fcost = MeshList[MeshIndex].NavMeshVertices[ElementIndex].F; // Обязательно добавить возможность работы с несколькими мешами а не одним

		while(CurrentIttemToCheck != 1)
		{
			HalfSize = CurrentIttemToCheck/2;
			if(Heap[CurrentIttemToCheck].Fcost <= Heap[HalfSize].Fcost)
			  {			
					   TempIttem = Heap[HalfSize];
					   Heap[HalfSize] = Heap[CurrentIttemToCheck];
					  MeshList[MeshIndex].NavMeshVertices[ Heap[HalfSize].IndexInVertexArray].IndexInBinaryHeap = HalfSize;
					   Heap[CurrentIttemToCheck] = TempIttem;
					 MeshList[MeshIndex].NavMeshVertices[ TempIttem.IndexInVertexArray].IndexInBinaryHeap = CurrentIttemToCheck;
					   CurrentIttemToCheck = HalfSize;

			  }else
			  {
				 
				 break;
			  }
		}
	 MeshList[MeshIndex].NavMeshVertices[ElementIndex].IndexInBinaryHeap = CurrentIttemToCheck;
//	  PrintAllElements();
}
/*
void BinaryHeap::AddElementInHeap(unsigned int ElementIndex)
{
	ElementBHeap  TempIttem; 
	unsigned int CurrentIttemToCheck = ++NumElementsInHeap, HalfSize;
	Heap[NumElementsInHeap].IndexInVertexArray = ElementIndex;
	Heap[NumElementsInHeap].Fcost = Mesh->Mesh.Mesh.NavMeshVertices[ElementIndex].F; // Обязательно добавить возможность работы с несколькими мешами а не одним

		while(CurrentIttemToCheck != 1)
		{
			HalfSize = CurrentIttemToCheck/2;
			if(Heap[CurrentIttemToCheck].Fcost <= Heap[HalfSize].Fcost)
			  {
					   TempIttem = Heap[HalfSize];
					   Heap[HalfSize] = Heap[CurrentIttemToCheck];
					   Heap[CurrentIttemToCheck] = TempIttem;
					   CurrentIttemToCheck = HalfSize;

			  }else
			  {
				  break;
			  }
		}
	
}
*/
//todo Вернуть Heap[1]

unsigned int BinaryHeap::RemoveCheapestElement(unsigned int MeshIndex)
{


//PrintAllElements();

	unsigned int ReturnValue = Heap[1].IndexInVertexArray;
	unsigned int v = 1, u, FirstChildrenIndex, SecondChildrenIndex;
	ElementBHeap  TempIttem;
	Heap[1] = Heap[NumElementsInHeap];
	MeshList[MeshIndex].NavMeshVertices[ Heap[1].IndexInVertexArray].IndexInBinaryHeap = 1;
	NumElementsInHeap--;

	for(;;)
	{
		u = v;
		FirstChildrenIndex = 2*u;
		SecondChildrenIndex = FirstChildrenIndex+1;
		if ( (SecondChildrenIndex) <= NumElementsInHeap )
		{
			if (Heap[u].Fcost >= Heap[FirstChildrenIndex].Fcost) v = FirstChildrenIndex;
			if (Heap[v].Fcost >= Heap[SecondChildrenIndex].Fcost)  v = SecondChildrenIndex;
		}else if(FirstChildrenIndex <= NumElementsInHeap)
		{
			if(Heap[u].Fcost >= Heap[FirstChildrenIndex].Fcost) v = FirstChildrenIndex;
		}

		if (u != v)
		{
			TempIttem = Heap[u];
			Heap[u] = Heap[v];
			MeshList[MeshIndex].NavMeshVertices[ Heap[u].IndexInVertexArray].IndexInBinaryHeap = u;
			Heap[v] = TempIttem;
			MeshList[MeshIndex].NavMeshVertices[ TempIttem.IndexInVertexArray].IndexInBinaryHeap = v;
		}else
		{
/*																		{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data,"Remove element  Index %d  G %f H %f F %f\n", ReturnValue,Mesh->Mesh.Mesh.NavMeshVertices[ReturnValue].G,Mesh->Mesh.Mesh.NavMeshVertices[ReturnValue].H,Mesh->Mesh.Mesh.NavMeshVertices[ReturnValue].F);
	fputs((const char *)&data,file);
	fclose(file);
	}
	*/
			return ReturnValue;
		}
	}
}

/*
unsigned int BinaryHeap::RemoveCheapestElement()
{
	unsigned int ReturnValue = Heap[1].IndexInVertexArray;
	unsigned int v = 1, u, FirstChildrenIndex, SecondChildrenIndex;
	ElementBHeap  TempIttem;
	Heap[1] = Heap[NumElementsInHeap];
	NumElementsInHeap--;

	for(;;)
	{
		u = v;
		FirstChildrenIndex = 2*u;
		SecondChildrenIndex = FirstChildrenIndex+1;
		if ( (SecondChildrenIndex) <= NumElementsInHeap )
		{
			if (Heap[u].Fcost >= Heap[FirstChildrenIndex].Fcost) v = FirstChildrenIndex;
			if (Heap[v].Fcost >= Heap[SecondChildrenIndex].Fcost)  v = SecondChildrenIndex;
		}else if(FirstChildrenIndex <= NumElementsInHeap)
		{
			if(Heap[u].Fcost >= Heap[FirstChildrenIndex].Fcost) v = FirstChildrenIndex;
		}

		if (u != v)
		{
			TempIttem = Heap[u];
			Heap[u] = Heap[v];
			Heap[v] = TempIttem;
		}else
		{
			return ReturnValue;
		}
	}
}
*/

void BinaryHeap::ChangeIttemCost(unsigned int MeshIndex, unsigned int ElementIndex)
{/*
																		{
	char data[2048];
	FILE * file;
	file = fopen( "c:\\testqq2.txt", "a+" );
	sprintf(data,"Change cost  Index %d  G %f H %f F %f\n", ElementIndex,Mesh->Mesh.Mesh.NavMeshVertices[ElementIndex].G,Mesh->Mesh.Mesh.NavMeshVertices[ElementIndex].H,Mesh->Mesh.Mesh.NavMeshVertices[ElementIndex].F);
	fputs((const char *)&data,file);
	fclose(file);
	}
	*/
ElementBHeap  TempIttem; 
	unsigned int IndexInBinaryHeap;
	unsigned int CurrentIttemToCheck =  MeshList[MeshIndex].NavMeshVertices[ElementIndex].IndexInBinaryHeap, HalfSize;
	Heap[CurrentIttemToCheck].IndexInVertexArray = ElementIndex;
	
	Heap[CurrentIttemToCheck].Fcost = MeshList[MeshIndex].NavMeshVertices[ ElementIndex ].F;

		while(CurrentIttemToCheck != 1)
		{
			HalfSize = CurrentIttemToCheck/2;
			if(Heap[CurrentIttemToCheck].Fcost <= Heap[HalfSize].Fcost)
			  {			
					   TempIttem = Heap[HalfSize];
					   Heap[HalfSize] = Heap[CurrentIttemToCheck];
					   MeshList[MeshIndex].NavMeshVertices[ Heap[HalfSize].IndexInVertexArray].IndexInBinaryHeap = HalfSize;
					   Heap[CurrentIttemToCheck] = TempIttem;
					  MeshList[MeshIndex].NavMeshVertices[ TempIttem.IndexInVertexArray].IndexInBinaryHeap = CurrentIttemToCheck;
					   CurrentIttemToCheck = HalfSize;

			  }else
			  {
				 break;
			  }
		}
	MeshList[MeshIndex].NavMeshVertices[ElementIndex].IndexInBinaryHeap = CurrentIttemToCheck;
}
void BinaryHeap::Reset()
{
NumElementsInHeap = 0;
}
BinaryHeap::~BinaryHeap()
{

}