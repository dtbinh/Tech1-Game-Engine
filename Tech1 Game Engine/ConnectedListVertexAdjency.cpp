/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "ConnectedListVertexAdjency.h"

#include <iostream>

void ConnectedListVertexAdjency::AddValue(int ValueToAdd, char VertexNumber)
{
VListValues * TempValue;
	if(NumElements) // Если уже есть элементы
	{
		TempValue = new VListValues;
		TempValue->IntegerValue = ValueToAdd;
		TempValue->VertexNum = VertexNumber;
		TempValue->NextValue = List;
		List = TempValue;

	}else
	{ // если нет
		List = new VListValues;
		List->IntegerValue = ValueToAdd;
		List->VertexNum = VertexNumber;
	}
NumElements++;
}

ConnectedListVertexAdjency::ConnectedListVertexAdjency()
{
NumElements = 0;
List = 0;
}

void ConnectedListVertexAdjency::PrintAllElements()
{
VListValues * TempValue = List;
	for(int i = 0;  i!= NumElements; i++)
	{
		std::cout << TempValue->IntegerValue << "\n";
		TempValue = TempValue->NextValue;
	}
}

int ConnectedListVertexAdjency::FindValue(int ValueToFind)
{
	int ThisValueFoundTimes = 0;
VListValues * TempValue = List;
	for(int i = 0;  i!= NumElements; i++)
	{
		//std::cout << TempValue->IntegerValue << "\n";
		if(TempValue->IntegerValue == ValueToFind)
		ThisValueFoundTimes++;
		TempValue = TempValue->NextValue;
	}

	return ThisValueFoundTimes;
}

ConnectedListVertexAdjency::~ConnectedListVertexAdjency()
{
VListValues * TempValue = List;
VListValues * LastValue;
	for(int i = 0;  i!= NumElements; i++)
	{
		LastValue = TempValue;
		TempValue = TempValue->NextValue;
		delete LastValue;
	}
}