/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef G_VARIABLES
#define G_VARIABLES

enum GAME_STATE
{
GMAE_STARTUP,
GAME_SHUTDOWN,
GAME_MAIN_MENU,
GAME_PLAY_MODE,
GAME_EDIT_MODE

};

class gVariables
{
public:
#ifdef IN_DEVELOPMENT
bool AutoSwitchToEditMode;
#endif
	GAME_STATE GameState;
	bool SwitchToEditMode;
	bool HaveToCalculateDimensions;
	gVariables();
	~gVariables();
};
#endif