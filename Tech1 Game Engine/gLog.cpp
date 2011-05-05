/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Common.h"


gLog * wLog;
void gLog::addMessage(WCHAR * message, ... )
{
	WCHAR * WriteString = new WCHAR[1024];
	va_list		argptr;
	va_start (argptr, message);
	vswprintf (WriteString, message, argptr);
	va_end (argptr);

FILE *file = fopen( log_file_name, "a" );
fputs( "\n", file );
fputws( WriteString, file );
fclose( file );
delete  WriteString;
}
gLog::gLog()
{
FILE *file = fopen( log_file_name, "w" ); 
fputs("----Tech1 game engine инициализирован---", file );
fclose( file );
}

gLog::~gLog()
{
FILE *file = fopen( log_file_name, "a" ); 
fputs("\n---Конец лога---", file );
fclose( file );
}