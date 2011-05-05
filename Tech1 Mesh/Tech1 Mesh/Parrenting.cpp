
#include "Parrenting.h"
#include <windows.h>
#include <iostream>
Parenting::Parenting()
{
ParrentInfo = 0;
LastElementAdded = 0;
Num = 0;
}

Parenting::~Parenting()
{

}

void Parenting::PrintAllElements()
{
	std::cout<< "\n";
OneParrentInfoEx * TempElement = ParrentInfo;
Childs * TempChild;
while(TempElement!=0)
	{
		
		std::cout << &TempElement->MeshName[0]<< "\n";
		TempChild = TempElement->ChildData;

		while(TempChild!=0)
		{
			std::cout<< "	";
			std::cout << &TempChild->ParrentName[0];
			TempChild = TempChild->NextChild;
			std::cout << "\n";
		}
		
		TempElement=TempElement->NextParrentInfo;
	}
}

void Parenting::DeleteAllElements()
{
	OneParrentInfoEx * LastElement;
	
OneParrentInfoEx * TempElement = ParrentInfo;
Childs * TempChild;
Childs * LastCH;
LastElement = TempElement;
while(TempElement!=0)
	{
		LastElement = TempElement;
		TempChild = TempElement->ChildData;
LastCH = TempChild;
		while(TempChild!=0)
		{
			LastCH = TempChild;
			TempChild = TempChild->NextChild;
			delete LastCH;
		}
		TempElement=TempElement->NextParrentInfo;
		delete LastElement;
	}
ParrentInfo = 0;
LastElementAdded = 0;
Num = 0;
}

void Parenting::AddElement(char * Name)
{
OneParrentInfoEx * NewElement = new OneParrentInfoEx;

memcpy(&NewElement->MeshName[0], Name, strlen(Name)+1);
NewElement->NextParrentInfo = 0;
NewElement->ChildData = 0;
NewElement->LastChildAdded = 0;
NewElement->NumChilds = 0;
Num++;
if(LastElementAdded)
{
	LastElementAdded->NextParrentInfo = NewElement;
	LastElementAdded = NewElement;
}else
{
LastElementAdded = NewElement;
ParrentInfo = NewElement;
}
}

unsigned int Parenting::GetNumChilds(char * ElementName)
{
char ErrorMessage[255];
OneParrentInfoEx * TempElement = ParrentInfo;
while(TempElement!=0)
{
	if(!strcmp(ElementName, &TempElement->MeshName[0]))
	{
		return TempElement->NumChilds;
	}
	TempElement=TempElement->NextParrentInfo;
}

sprintf(&ErrorMessage[0], "GetNumChilds: unable to find element \"%s\"", ElementName);
MessageBoxA(0, &ErrorMessage[0], "Error", MB_OK);
return 0;
}
void Parenting::AddChild(char * ElementName, char * ChildName)
{
char ErrorMessage[255];
OneParrentInfoEx * TempElement = ParrentInfo;
Childs * NewChild;
while(TempElement!=0)
{
	if(!strcmp(ElementName, &TempElement->MeshName[0]))
	{
		TempElement->NumChilds++;
		NewChild = new Childs;
		memcpy(&NewChild->ParrentName[0], ChildName, strlen(ChildName)+1);
		NewChild->NextChild = 0;
		if(TempElement->LastChildAdded == 0)
		{
			TempElement->ChildData = NewChild;
			TempElement->LastChildAdded = NewChild;
		}else
		{
			TempElement->LastChildAdded->NextChild = NewChild;
			TempElement->LastChildAdded = NewChild;
		}
		return;
	}
	TempElement=TempElement->NextParrentInfo;
}
// Если не нашли элемент с таким именем добавим его
AddElement(ElementName);
AddChild(ElementName, ChildName);
}

unsigned int Parenting::GetNumElements()
{
return Num;
}

char * Parenting::GetElementNum(unsigned int ElementNum)
{
int i;
char ErrorMessage[255];
OneParrentInfoEx * TempElement = ParrentInfo;
	if(ElementNum>=Num)
	{
		sprintf(&ErrorMessage[0], "GetElementNum: unable to find element No:%d", ElementNum);
		MessageBoxA(0, &ErrorMessage[0], "Error", MB_OK);
		return "";
	}
for(i = 0; i!= ElementNum; i++)
{
	TempElement=TempElement->NextParrentInfo;
}	
return &TempElement->MeshName[0];
}


char * Parenting::GetChildNum(char * ElementName, unsigned int ChildNum)
{
	int i;
char ErrorMessage[255];
OneParrentInfoEx * TempElement = ParrentInfo;
Childs * TempChild;
while(TempElement!=0)
{
	if(!strcmp(ElementName, &TempElement->MeshName[0]))
	{
		if(ChildNum >= TempElement->NumChilds)
		{
		sprintf(&ErrorMessage[0], "GetChildNum: unable to find child No:%d", ChildNum);
		MessageBoxA(0, &ErrorMessage[0], "Error", MB_OK);
		return "";
		}

		TempChild = TempElement->ChildData;

		for(i = 0; i!= ChildNum; i++)
		{
			TempChild = TempChild->NextChild;
		}	
		return &TempChild->ParrentName[0];
	}
	TempElement=TempElement->NextParrentInfo;
}

sprintf(&ErrorMessage[0], "GetChildNum: unable to find element \"%s\"", ElementName);
MessageBoxA(0, &ErrorMessage[0], "Error", MB_OK);
return "";
}