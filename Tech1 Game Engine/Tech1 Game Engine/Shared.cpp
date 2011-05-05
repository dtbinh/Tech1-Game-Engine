/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
char * Str(char * str, ...)
{
	static char  String[256];
	va_list		argptr;
	va_start (argptr, str);
	vsprintf (String, str, argptr);
	va_end (argptr);
	return String;
}

char * GetStr(char * string, unsigned int size)
{
	static char  String[1024];
	memcpy(String, string, size);
	String[size] = 0;
	return String;
}