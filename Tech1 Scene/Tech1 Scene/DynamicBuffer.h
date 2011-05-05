#ifndef _DYNAMIC_BUFFER_
#define _DYNAMIC_BUFFER_

class DynamicBuffer
{
public:
char * Buffer;
unsigned int BufferLengh;
DynamicBuffer();
~DynamicBuffer();
void AddData(char * StringToAdd);
void AddChar(char Leter);
void DynamicBuffer::AddZero();
};

#endif