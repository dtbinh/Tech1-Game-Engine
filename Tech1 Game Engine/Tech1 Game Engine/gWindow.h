/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef G_WINDOW
#define G_WINDOW
class gWindow
{
public:
	bool WindowMode;
	int ScreenWidth;
	int ScreenHeight;
	int ScreenSizeWidth;
	int ScreenSizeHeight;
	unsigned int WindowStartPosX;
	unsigned int WindowStartPosY;
	unsigned int WindowEndPosX;
	unsigned int WindowEndPosY;

	unsigned int WindowClientAreaStartPosX; // 132
	unsigned int WindowClientAreaStartPosY; // 110
	unsigned int WindowClientAreaEndPosX;  // 1148 - 1
	unsigned int WindowClientAreaEndPosY;  // 716-1
	unsigned int ClientAreaSizeWidth;
	unsigned int ClientAreaSizeHeight;


	unsigned int WindowCenterPosX;
	unsigned int WindowCenterPosY;
	WNDCLASSEX wc;
	HWND hWnd;
	gWindow();
	~gWindow();
	void CalculateDimensions();
};
#endif