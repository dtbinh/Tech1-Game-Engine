/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _STR_
#define _STR_
int GetStringLengh(char * String);
void AddStringToBuffer(char * String, char * Buffer, int AdressToStart, int CharsToAdd);
char * ConvertIntegerToText(unsigned int Integer);
void AddStringToBufferAndInvert(char * String, char * Buffer, int AdressToStart, int CharsToAdd);
char * ConvertIntegerToTextAfterDot(unsigned int Integer);
bool CompareStringMaya(char * Str1, char * Str2, unsigned int Str1Size, unsigned int Str2Size);
#endif