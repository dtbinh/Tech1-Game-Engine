/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "ConnectedListStrings.h"
#include <stdio.h>
#include <memory>
#include <iostream>
#include "Str.h"
using namespace std;
ConnectedListStrings::ConnectedListStrings()
{
ListOfStrings = NULL;
NumUnits = 0;
}

void ConnectedListStrings::AddUnit(char * String)
{	OneUnitInConnectedListStrings * TempUnit;
	if(!ListOfStrings)  // ≈сли не существует ни одного то добавл€ем в начало
	{
		ListOfStrings = new OneUnitInConnectedListStrings;
		ListOfStrings->Next = NULL;
		memcpy(&ListOfStrings->Unit[0],String,GetStringLengh(String)+1);
		NumUnits++;
	}else
	{
	TempUnit = new OneUnitInConnectedListStrings;
	memcpy(&TempUnit->Unit[0],String,GetStringLengh(String)+1);
	TempUnit->Next = ListOfStrings;
	ListOfStrings = TempUnit;  // Ќовую строку добавим в начало списка
	NumUnits++;
	}
}
void ConnectedListStrings::PrintAllUnits()
{
	OneUnitInConnectedListStrings * TempUnit = ListOfStrings;
		for(int i = 0; i!=NumUnits; i++)
			{
				//cout << TempUnit->Unit<< endl;
				TempUnit = TempUnit->Next;
			}
}
bool ConnectedListStrings::CheckIsItemInList(char * String)
{
	int SearchStrLengh = GetStringLengh(String);

OneUnitInConnectedListStrings * TempUnit = ListOfStrings;
		for(int i = 0; i!=NumUnits; i++)
			{
				//cout << TempUnit->Unit<< endl;
				if(CompareStringMaya(String,&TempUnit->Unit[0],SearchStrLengh,GetStringLengh(&TempUnit->Unit[0]))) return true;
				TempUnit = TempUnit->Next;
			}
return false;
}
ConnectedListStrings::~ConnectedListStrings()
{
	OneUnitInConnectedListStrings * TempUnit = ListOfStrings;
		for(int i = 0; i!=NumUnits; i++)
			{
				//cout << ListOfStrings->Unit;
				ListOfStrings = ListOfStrings->Next;
				delete TempUnit;
				TempUnit = ListOfStrings;
			}
}