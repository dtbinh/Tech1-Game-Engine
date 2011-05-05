#include "StringDynamicBuffer.h"
#include "Str.h"
#include "FileIO.h"
//#include <iostream>
#ifndef NULL
#define NULL 0
#endif

//using namespace std;
void StringDynamicBuffer::AddChar(char Leter)
{
char * Text;
Text = new char[2];
Text[0] = Leter;
Text[1] = 0;
AddData(Text);
delete [] Text;
}
void StringDynamicBuffer::AddZero()
{
	//AddData(" ");
	//Buffer[BufferLengh] = 0;

}





void StringDynamicBuffer::PrintAllUnitsToFile(char *File)
{
	Files file;
	file.OpenFile(File);
	
	MemoryBuffer * EnumObject = Buffer;
	while(EnumObject)
	{
		//cout << EnumObject->Text << endl;
		file.PutDataToFile(EnumObject->Text,EnumObject->size);
		EnumObject = EnumObject->Next;
	}
	file.CloseFile();
}

void StringDynamicBuffer::AddData(char * StringToAdd)
{
	if(!Buffer)
	{
		Buffer = new MemoryBuffer;
		Buffer->Next = NULL;
		Buffer->size = GetStringLengh(StringToAdd);
		Buffer->Text = new char[Buffer->size+1];
		AddStringToBuffer(StringToAdd,Buffer->Text,0,Buffer->size+1);
		BufferLengh++;
		LastUnit = Buffer;
	}else
	{
		LastUnit->Next = new MemoryBuffer;
		LastUnit->Next->Next = NULL;
		LastUnit->Next->size = GetStringLengh(StringToAdd);
		LastUnit->Next->Text = new char[LastUnit->Next->size+1];
		AddStringToBuffer(StringToAdd,LastUnit->Next->Text,0,LastUnit->Next->size+1);
		BufferLengh++;
		LastUnit = LastUnit->Next;
	}


	

}

StringDynamicBuffer::StringDynamicBuffer()
{
Buffer = NULL;
BufferLengh = 0;
LastUnit = NULL;
}
StringDynamicBuffer::~StringDynamicBuffer()
{
	MemoryBuffer * LastObject = NULL;
	while(Buffer)
	{
		LastObject = Buffer;
		Buffer = Buffer->Next;
		delete [] LastObject->Text;
		delete LastObject;
		
	}
		
		Buffer = NULL;
}