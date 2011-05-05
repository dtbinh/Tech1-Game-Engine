/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#define _CRTDBG_MAPALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include "FileIO.h"

void Files::CloseFile()
{
fclose( file );
FileToWorkWith = NULL;
}
void Files::OpenFile(char * FileName, char * mode)
{
FileToWorkWith = FileName;
file = fopen( FileToWorkWith, mode );
}
void Files::PutDataToFile (void * Data, int Count)
{
//fputs( Data, file );
fwrite(Data, 1, Count, file);
}


void Files::ReadFile(char * Data, unsigned int NumDataToRead)
{
fread((void *)Data,NumDataToRead,1, file);
}
unsigned GetFileSizez(char * FileName)
{
FILE *fin;
fin = fopen(FileName, "rb");
if(fin == 0) return 4294967295;
unsigned int size;
fseek(fin, 0, SEEK_END);
size = ftell(fin);
fseek(fin, 0, SEEK_SET);
fclose( fin );
return size;
}

Files::Files()
{
FileToWorkWith = NULL;
}


Files::~Files()
{
if(FileToWorkWith)
fclose( file );
}