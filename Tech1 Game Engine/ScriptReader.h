/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Mesh.h"

#ifndef _SCRIPT_READER_
#define _SCRIPT_READER_



enum Expecting
{
 BIG_OE_SMALL_LETERS = 1,
 NUMBERS,
 SPACEBAR_OR_TAB,
 OPEN_BRACKET,
 CLOSE_BRACKET,
 KOMA_DOT,
 BIG_OR_SMALL_LETERS_AND_NUMBERS,
 END_EOF,
 DOT
};

class ScriptReader
{
	OneMesh * MeshList;
public:
bool LoadingNavMesh; // Загружаем НавМеш
void WeightsLoader(unsigned int MeshIndex);
bool MeshLoader(unsigned int MeshIndex );
void ShadersLoader(unsigned int MeshIndex);
bool ProcessMeshScript(unsigned int MeshIndex);
//int Token(char * Buffer, int  StartPos, int & type);
//char * GetNextToken(int & type);
//char * GetNextTokenSkipSpacebar(int & type);
void HierarhyLoader(unsigned int AnimationIndex);
void FramesLoader(unsigned int AnimationIndex);
void LoadMesh(unsigned int MeshIndex, char * Data, bool PoocMesh , bool NavMesh);
void LoadAnimation(unsigned int AnimationIndex, char * Data);
bool ProcessAnimationScript(unsigned int AnimationIndex);
ScriptReader();
~ScriptReader();
int VerticesLoaded;//  To do Удалить эту переменную   она только для статистики во время разработки
int LastTokenStart;
int LastTokenStartReadPos;
int LastTokenSize;
char * ScriptData;
bool ErrorLoadingScript;
};



#endif;