/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
gVariables wVariables; // Глобальная
gVariables::gVariables()
{
#ifdef IN_DEVELOPMENT
AutoSwitchToEditMode=true;
#endif
GameState = GMAE_STARTUP;
HaveToCalculateDimensions = false;

}


gVariables::~gVariables()
{



}