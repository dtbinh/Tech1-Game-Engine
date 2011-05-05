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
	void OpenFile(char * FileName);
	void ReadFile(char * Data, unsigned int NumDataToRead);
	
	void PutDataToFile (void * Data, int Count);
	void CloseFile();
};

unsigned int GetFileSize(char * FileName);
#endif