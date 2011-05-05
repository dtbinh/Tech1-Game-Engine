
#include "DynamicBuffer.h"
#include "Str.h"

#ifndef NULL
#define NULL 0
#endif
void DynamicBuffer::AddChar(char Leter)
{
char * Text;
Text = new char[2];
Text[0] = Leter;
Text[1] = 0;
AddData(Text);
delete [] Text;
}
void DynamicBuffer::AddZero()
{
	AddData(" ");
	Buffer[BufferLengh] = 0;

}
void DynamicBuffer::AddData(char * StringToAdd)
{

int NewBufferLengh;
char * NewBuffer;
if(!BufferLengh)
	{
		BufferLengh = GetStringLengh(StringToAdd);
		Buffer = new char[BufferLengh];
		AddStringToBuffer(StringToAdd , Buffer, 0, BufferLengh);
		
	}else
		{
			NewBufferLengh = GetStringLengh(StringToAdd);
			NewBuffer = new char[NewBufferLengh + BufferLengh];
			AddStringToBuffer(Buffer , NewBuffer, 0, BufferLengh);
			AddStringToBuffer(StringToAdd , NewBuffer, BufferLengh, NewBufferLengh);
			delete Buffer;
			Buffer = NewBuffer;
			BufferLengh = NewBufferLengh + BufferLengh;
		}

}

DynamicBuffer::DynamicBuffer()
{
Buffer = NULL;
BufferLengh = 0;
}
DynamicBuffer::~DynamicBuffer()
{
	if(!Buffer)
delete [] Buffer;
BufferLengh = 0;
}