/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Time.h"
gTime * wTime;

gTime::gTime()
{
	StartTime = timeGetTime();
}

void gTime::ProcessTime()
{
	time = timeGetTime();
}

gTime::~gTime()
{

}

DWORD gTime::GetTimeFromStart()
{
	return time - StartTime;
}