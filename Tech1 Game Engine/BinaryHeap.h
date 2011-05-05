/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _BINARY_HEAP_
#define _BINARY_HEAP_
#include "Mesh.h"
#define BINARY_HEAP_SIZE 10000

struct ElementBHeap
{
unsigned int IndexInVertexArray;
float Fcost;
};
class BinaryHeap // В BinaryHeap у нас будут индексы  вершин потому что поиск пути оперирует вершинами.
{
	OneMesh * MeshList;
public:
	ElementBHeap Heap[BINARY_HEAP_SIZE];
	unsigned int NumElementsInHeap;
	BinaryHeap();
	~BinaryHeap();
	void AddElementInHeap(unsigned int MeshIndex, unsigned int ElementIndex);
	unsigned int RemoveCheapestElement(unsigned int MeshIndex);
	void Reset();
	void PrintAllElements();
	void ChangeIttemCost(unsigned int MeshIndex, unsigned int ElementIndex);
};

#endif