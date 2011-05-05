/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef G_PRINT_INFO
#define G_PRINT_INFO
#define MAX_STRING_SIZE 1024

class gPrintInfo
{
public:
	RECT abc;
	RECT abc2;
	D3DXFONT_DESC lf;
	ID3DXFont* font;
	WCHAR * Console_Output;
	gPrintInfo();
	~gPrintInfo();
	void Print(WCHAR * TextToPrint, ... );
	void Render();
};
#endif