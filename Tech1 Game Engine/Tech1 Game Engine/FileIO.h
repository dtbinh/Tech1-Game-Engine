/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include <stdio.h>
#ifndef _FILEIO_
#define _FILEIO_
class Files
{
private:
	FILE *file;
public:
	char * FileToWorkWith;
	Files();
	~Files();
	void OpenFile(char * FileName, char * mode); // mode = "w" - Для записи в начало. "a" добавить   . mode = "r" - Для чтения     "r+b"  для чтения и записи
	void ReadFile(char * Data, unsigned int NumDataToRead);
	
	void PutDataToFile (void * Data, int Count);
	void CloseFile();
};
unsigned int GetFileSizez(char * FileName);

#endif