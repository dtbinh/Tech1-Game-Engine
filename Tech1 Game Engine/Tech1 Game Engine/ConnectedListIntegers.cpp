/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "ConnectedListIntegers.h"

#include <iostream>

void ConnectedListIntegers::AddValue(int ValueToAdd)
{
ListValues * TempValue;
	if(NumElements) // Если уже есть элементы
	{
		TempValue = new ListValues;
		TempValue->IntegerValue = ValueToAdd;
		TempValue->NextValue = List;
		List = TempValue;

	}else
	{ // если нет
		List = new ListValues;
		List->IntegerValue = ValueToAdd;
	}
NumElements++;
}

ConnectedListIntegers::ConnectedListIntegers()
{
NumElements = 0;
}

void ConnectedListIntegers::PrintAllElements()
{
ListValues * TempValue = List;
	for(int i = 0;  i!= NumElements; i++)
	{
		std::cout << TempValue->IntegerValue << "\n";
		TempValue = TempValue->NextValue;
	}
}

int ConnectedListIntegers::FindValue(int ValueToFind)
{
	int ThisValueFoundTimes = 0;
ListValues * TempValue = List;
	for(int i = 0;  i!= NumElements; i++)
	{
		//std::cout << TempValue->IntegerValue << "\n";
		if(TempValue->IntegerValue == ValueToFind)
		ThisValueFoundTimes++;
		TempValue = TempValue->NextValue;
	}

	return ThisValueFoundTimes;
}

ConnectedListIntegers::~ConnectedListIntegers()
{
ListValues * TempValue = List;
ListValues * LastValue;
	for(int i = 0;  i!= NumElements; i++)
	{
		LastValue = TempValue;
		TempValue = TempValue->NextValue;
		delete LastValue;
	}
}