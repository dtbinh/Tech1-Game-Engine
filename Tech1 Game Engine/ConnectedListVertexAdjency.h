/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _CONNECTED_LIST_VERTEX_ADJENCY_
#define _CONNECTED_LIST_VERTEX_ADJENCY_
struct VListValuesNoNext
{
unsigned int IntegerValue;
unsigned char VertexNum;
};
struct VListValues
{
unsigned int IntegerValue;
unsigned char VertexNum;
VListValues * NextValue;
};

class ConnectedListVertexAdjency
{
public:
VListValues * List;
VListValuesNoNext * Massive;
int NumElements;
ConnectedListVertexAdjency();
~ConnectedListVertexAdjency();
void AddValue(int ValueToAdd, char VertexNumber);
void PrintAllElements();
int FindValue(int ValueToFind);
};

#endif