/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
extern g3dDevice * w3dDevice;
gPrintInfo * wPrintInfo;
gPrintInfo::gPrintInfo()
{
ZeroMemory(&lf, sizeof(LOGFONT));
	lf.Height    = 16;
	lf.Width     = 9;
	lf.Weight    = 100; 
	lf.Italic    = false;
	//lf.Underline = false;
	//lf.StrikeOut = false;
	lf.CharSet   = DEFAULT_CHARSET;
	strcpy((char *)&lf.FaceName, (char *)"Times New Roman");

	D3DXCreateFontIndirect(w3dDevice->g_pd3dDevice, &lf, &font);

	abc2.top = 0;
	abc2.left = 0;
	abc2.right = 700;
	abc2.bottom = 480;

	abc.top = 21;
	abc.left = 0;
	abc.right =7500;
	abc.bottom = 480;
	Console_Output =  new WCHAR[MAX_STRING_SIZE];
	Console_Output[0]=0;
}

void gPrintInfo::Print(WCHAR * TextToPrint, ... )
{
	va_list		argptr;
	va_start (argptr, TextToPrint);
	vswprintf (Console_Output, TextToPrint, argptr);
	va_end (argptr);
	//Console_Output[0]=0; ///!!!!!!!!
}

gPrintInfo::~gPrintInfo()
{
font->Release();

}


void gPrintInfo::Render()
{
font->DrawText(NULL,
		Console_Output,
		-1,
		&abc,
		DT_TOP | DT_LEFT,
		0xffFFFF00);

}