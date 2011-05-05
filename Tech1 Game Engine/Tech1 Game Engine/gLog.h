/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Common.h"
#ifndef G_LOG
#define G_LOG
#define log_file_name  "Log.txt"
class gLog
{
public:
void addMessage(WCHAR * message, ... );
gLog();
~gLog();
};

#endif