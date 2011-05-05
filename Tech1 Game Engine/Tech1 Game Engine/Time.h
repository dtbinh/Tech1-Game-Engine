/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef G_TIME
#define G_TIME

class gTime
{
public:
DWORD StartTime;
DWORD time;
gTime();
void ProcessTime();
DWORD GetTimeFromStart();
~gTime();
};

#endif