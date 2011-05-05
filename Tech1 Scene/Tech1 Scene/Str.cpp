/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Str.h"
#include "DynamicBuffer.h"

char * ConvertIntegerToTextAfterDot(unsigned int Integer)
{
	char * Buf;
DynamicBuffer DBuffer;
if (!Integer)
	{
		Buf = new char[2];
		Buf[0] = '0';
		Buf[1] = 0;
		return Buf;
	}else
	{
		while(Integer)
		{
			DBuffer.AddChar('0' + (char)(Integer % 10));
			Integer = Integer / 10;
		}
	}
	Buf = new char[DBuffer.BufferLengh + 1];
	AddStringToBufferAndInvert(DBuffer.Buffer,Buf,0,DBuffer.BufferLengh-1);
	Buf[DBuffer.BufferLengh-1] = 0;
	return Buf;
}

char * ConvertIntegerToText(unsigned int Integer)
{
	char * Buf;
DynamicBuffer DBuffer;
if (!Integer)
	{
		Buf = new char[2];
		Buf[0] = '0';
		Buf[1] = 0;
		return Buf;
	}else
	{
		
		while(Integer)
		{
			DBuffer.AddChar('0' + (char)(Integer % 10));
			Integer = Integer / 10;
		}
	}
	Buf = new char[DBuffer.BufferLengh + 1];
	AddStringToBufferAndInvert(DBuffer.Buffer,Buf,0,DBuffer.BufferLengh);
	Buf[DBuffer.BufferLengh] = 0;
	return Buf;
}

int GetStringLengh(char * String)
{
int i = 0;
while(String[i++])
{

}
return i-1;
}

bool CompareStringMaya(char * Str1, char * Str2, unsigned int Str1Size, unsigned int Str2Size)
{
if( Str1Size != Str2Size) return false;
	while(Str1Size)
	{
		if(*Str1 != *Str2) return false;
		Str1++;
		Str2++;
		Str1Size--;
	}
	return true;
}
void AddStringToBuffer(char * String, char * Buffer, int AdressToStart, int CharsToAdd)
{
int i;
for(i = 0; i!= CharsToAdd; i++)
	{
		Buffer[i+AdressToStart]=String[i];
	}
}

void AddStringToBufferAndInvert(char * String, char * Buffer, int AdressToStart, int CharsToAdd)
{
int i;
for(i = 0; i!= CharsToAdd; i++)
	{
		Buffer[((CharsToAdd+AdressToStart)-i)-1]=String[i];
	}
}