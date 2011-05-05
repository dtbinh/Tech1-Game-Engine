/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _BONE_INDEXER_
#define _BONE_INDEXER_

int GetStrSize(char * Str);
struct BonesAndIndexes
{
char BoneName[255];
int Index;
BonesAndIndexes * NextBone;
};

class BoneIndexer
{
public:
int LastIndex;
BonesAndIndexes * bones;
BoneIndexer();
int AddBone(char * BoneName);
~BoneIndexer();
};

#endif