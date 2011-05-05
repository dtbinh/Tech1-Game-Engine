#ifndef _STRING_DYNAMIC_BUFFER_
#define _STRING_DYNAMIC_BUFFER_


struct MemoryBuffer
{
MemoryBuffer * Next;
char * Text;
int size;
};

class StringDynamicBuffer
{
public:
MemoryBuffer * Buffer;
MemoryBuffer * LastUnit;
unsigned int BufferLengh;
void PrintAllUnitsToFile(char *File);
StringDynamicBuffer();
~StringDynamicBuffer();
void AddData(char * StringToAdd);
void AddChar(char Leter);
void AddZero();
};

#endif