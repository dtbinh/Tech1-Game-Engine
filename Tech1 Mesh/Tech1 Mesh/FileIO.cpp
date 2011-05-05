/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "FileIO.h"

void Files::CloseFile()
{
fclose( file );
FileToWorkWith = NULL;
}
void Files::OpenFile(char * FileName)
{
FileToWorkWith = FileName;
file = fopen( FileToWorkWith, "w" );
}
void Files::PutDataToFile (void * Data, int Count)
{
//fputs( Data, file );
fwrite(Data, 1, Count, file);
}


void Files::ReadFile(char * Data, unsigned int NumDataToRead)
{
fread((void *)Data,1,NumDataToRead, file);
}
unsigned int GetFileSize(char * FileName)
{
FILE *fin;
fin = fopen(FileName, "rb");
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