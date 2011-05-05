/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "DoublesSorter.h"
#include <iostream>
#ifndef NULL
#define NULL 0
#endif
//using namespace std;

double DoublesSorter::PrintUnit(int UnitNo)
{
	int counter=1;
	DoublesOneUnit * EnumObject = Memory;
	while(EnumObject)
	{
		if(counter == UnitNo)
		{
			//cout << EnumObject->value << endl;
			return EnumObject->value;
		}
		counter++;
		EnumObject = EnumObject->Next;
	}
return -1.0;  // сюда никогда не попадем
}

void DoublesSorter::PrintAllUnits()
{
	DoublesOneUnit * EnumObject = Memory;
	while(EnumObject)
	{
		//cout << EnumObject->value << endl;
		EnumObject = EnumObject->Next;
	}
}
extern int onlyfortestCounter;
void DoublesSorter::AddValue(double Value)
{



	DoublesOneUnit * EnumObject = Memory;
	DoublesOneUnit * LastUnit	= NULL;
	if(!EnumObject)
	{
		Memory = new DoublesOneUnit;
		Memory->Next = NULL;
		Memory->value = Value;
		counter++;
		return;
	}

	while(EnumObject)
	{



		if(EnumObject->value == Value) 
		{



			return; // если элемент с таким значением уже существует, то не добавляем его.
		}
		if(Value < EnumObject->value)
			{
				
					if(LastUnit)
						{
							LastUnit->Next = new DoublesOneUnit;
							LastUnit->Next->value = Value;
							LastUnit->Next->Next = EnumObject;
							counter++;
							return;

						}else
							{
								LastUnit = new DoublesOneUnit;
								LastUnit->value = Value;
								LastUnit->Next = EnumObject;
								Memory = LastUnit;
								counter++;
								return;
							}
				
			}
			if(!EnumObject->Next)
			{
				EnumObject->Next = new DoublesOneUnit;
				EnumObject->Next->Next = NULL;
				EnumObject->Next->value = Value;
				counter++;
				return;
			}

			LastUnit = EnumObject;
			EnumObject = EnumObject->Next;
	}
}


DoublesSorter::DoublesSorter()
	{
		Memory = NULL;
		counter = 0;
	}


DoublesSorter::~DoublesSorter()
	{ 
		
	DoublesOneUnit * LastObject = NULL;
	while(Memory)
	{
		LastObject = Memory;
		Memory = Memory->Next;
		delete LastObject;
		
	}
		
		Memory = NULL;
	}