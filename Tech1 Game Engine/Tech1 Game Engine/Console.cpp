/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
Console * CON;
Console::Console()
{

}

Console::~Console()
{

}



void Console::ErrorMessage(char * message, ... )
{
	char  ErrorMsg[256];
	va_list		argptr;
	va_start (argptr, message);
	vsprintf (&ErrorMsg[0], message, argptr);
	va_end (argptr);

MessageBoxA(0, &ErrorMsg[0], "Critical error.", MB_OK);
}