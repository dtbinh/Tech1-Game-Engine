#include "BoneIndexer.h"
#include <iostream>




using namespace std;
#ifndef NULL
#define NULL 0
#endif


bool CompareString(char * Str1, char * Str2, unsigned int Str1Size, unsigned int Str2Size)
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

int GetStrSize(char * Str)
{
	int i=0;
	while(Str[i])
	{
		i++;
	}
	return i;
}

int BoneIndexer::AddBone(char * BoneName)
{
 //Сначало проходим весь список в поиске кости с таким именем
int NameSize = GetStrSize(BoneName);
//cout << "StringSize: " << NameSize << endl;
BonesAndIndexes * TempBone = bones;
for(; TempBone ; TempBone = TempBone->NextBone)
	{
		//cout << "Text" << endl;
		if(CompareString(TempBone->BoneName, BoneName,GetStrSize(TempBone->BoneName), NameSize))
		{
			return TempBone->Index;
		}
	}
// Если не нащли то создаем в списке кость с таким именем
TempBone = bones;
bones = new BonesAndIndexes;
bones->NextBone = TempBone;
memcpy(bones->BoneName, BoneName,NameSize+1);
bones->Index = ++LastIndex;
return bones->Index;
}


BoneIndexer::BoneIndexer()
{
bones = NULL;
LastIndex = -1;
}

BoneIndexer::~BoneIndexer()
{
BonesAndIndexes * TempBone = bones;
BonesAndIndexes * LastBone;
while(TempBone)
	{
		//cout << "Text" << endl;
		LastBone = TempBone;
		TempBone = TempBone->NextBone;
		delete LastBone;
	}
}