/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "Common.h"
#include <dinput.h>
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>

#ifndef G_DIRECTINPUT
#define G_DIRECTINPUT
#include <dinput.h>
class gDirectInput
{
public:
int CursorPosXClientAreaCoordSys;
int CursorPosYClientAreaCoordSys;
	DWORD LastTimePressedKeyboard;
LPDIRECTINPUTDEVICE8 dinmouse;
DIMOUSESTATE mousestate;
bool Keys[256];
LPDIRECTINPUT8 GameDirectInput;
LPDIRECTINPUTDEVICE8 KeyboardDevice, MouseDevice;
void ProcessInputGameEditMode();
void ProcessInputGamePlayMode();
void InitMouse();
void InitKeyboard();
gDirectInput();
~gDirectInput();
};
#endif