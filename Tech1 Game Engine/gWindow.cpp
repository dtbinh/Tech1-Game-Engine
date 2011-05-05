/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "gWindow.h"

extern g3dDevice * w3dDevice;
extern gVariables wVariables;
gWindow * wWindow;
extern gPrintInfo * wPrintInfo;
extern RendererSceleton * RScel;
extern ObjectPicking * wObjectPicking;
extern PathFind * PathFinder;
gWindow::gWindow()
{
WindowMode = true;
ScreenWidth = 800;
ScreenHeight = 600;
    wc.cbSize =  sizeof(WNDCLASSEX);
	wc.style  = CS_CLASSDC;
	wc.lpfnWndProc = MsgProc;
	wc.cbClsExtra = 0L;
	wc.cbWndExtra = 0L;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon     = NULL;
	wc.hCursor   = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = GameName;
	wc.hIconSm       = NULL ;

    RegisterClassEx( &wc );
ScreenSizeWidth  = GetSystemMetrics(SM_CXSCREEN);
	ScreenSizeHeight = GetSystemMetrics(SM_CYSCREEN);
	WindowStartPosX  = ScreenSizeWidth/10;
	WindowStartPosY  = ScreenSizeHeight/10;
	WindowEndPosX    = ScreenSizeWidth-((ScreenSizeWidth/10)*2);
	WindowEndPosY    = ScreenSizeHeight-((ScreenSizeHeight/10)*2);
	RECT RectAngle;
	RectAngle.left = WindowStartPosX;
	RectAngle.top  = WindowStartPosY;
	RectAngle.right = WindowEndPosX;
	RectAngle.bottom = WindowEndPosY;
if(!WindowMode)
	{
    hWnd = CreateWindow( GameName , GameName ,WS_EX_TOPMOST | WS_POPUP, 0, 0, ScreenWidth, ScreenHeight,GetDesktopWindow(), NULL, wc.hInstance, NULL );
	
	}else
	{
		hWnd = CreateWindow( GameName , GameName , WS_OVERLAPPEDWINDOW, WindowStartPosX, WindowStartPosY, WindowEndPosX, WindowEndPosY, GetDesktopWindow(), NULL, wc.hInstance, NULL );
	}


GetClientRect(hWnd,&RectAngle);
ClientAreaSizeWidth = RectAngle.right;
ClientAreaSizeHeight = RectAngle.bottom;
WindowClientAreaEndPosX = GetSystemMetrics(SM_CXSIZEFRAME)+WindowStartPosX +RectAngle.right -1;
WindowClientAreaStartPosX = GetSystemMetrics(SM_CXSIZEFRAME)+WindowStartPosX;

WindowClientAreaStartPosY = GetSystemMetrics(SM_CYCAPTION) + WindowStartPosY  + GetSystemMetrics(SM_CYSIZEFRAME);
WindowClientAreaEndPosY = WindowClientAreaStartPosY + RectAngle.bottom-1;//GetSystemMetrics(SM_CYMENU);
WindowCenterPosX = WindowClientAreaStartPosX+((WindowClientAreaEndPosX - (WindowClientAreaStartPosX-1))/2);
WindowCenterPosY = WindowClientAreaStartPosY+((WindowClientAreaEndPosY - (WindowClientAreaStartPosY-1))/2);
SetCursorPos(WindowCenterPosX,WindowCenterPosY);
	if(WindowMode)
	{
MoveWindow(hWnd,WindowStartPosX,WindowStartPosY,WindowEndPosX,WindowEndPosY,0);
	//WindowSizeX =WindowClientAreaEndPosX - WindowClientAreaStartPosX;
	//WindowSizeY = WindowClientAreaEndPosY - WindowClientAreaStartPosY;

	}


	 ShowWindow( hWnd, SW_SHOWDEFAULT );
     UpdateWindow( hWnd );
}
gWindow::~gWindow()
{
	

	UnregisterClass((LPCWSTR)GameName, wc.hInstance );
}

int LastSelectedPolly=-1;
D3DXVECTOR3 LastIntersectionPos;
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{bool NotCount = false;
    switch( msg )
    {
        case WM_DESTROY:
            wVariables.GameState = GAME_SHUTDOWN;
           
           break;

		case WM_MOVE:
        wVariables.HaveToCalculateDimensions = true;
        break;

		case WM_SIZE:
        wVariables.HaveToCalculateDimensions = true;
        break;
		case WM_LBUTTONDOWN:
			if(wObjectPicking->Select())
			{
				swprintf(wPrintInfo->Console_Output,L"Intersection detected PollyIndex = %d", wObjectPicking->IntersectedPollyIndex);
				if(LastSelectedPolly!=-1)
				{
						//RScel->LineStart = wObjectPicking->LineStart;
						//RScel->LineEnd = LastIntersectionPos;
					NotCount = true;
					DWORD StartSeartchTime = timeGetTime(); 
					PathFinder->RayTraceVec3(0, &LastIntersectionPos, &wObjectPicking->LineStart , (unsigned int)LastSelectedPolly, (unsigned int)wObjectPicking->IntersectedPollyIndex);
					swprintf(wPrintInfo->Console_Output,L"Path search time = %d", timeGetTime() - StartSeartchTime);
						//PathFinder->FindPath(&wObjectPicking->LineStart, &LastIntersectionPos, LastSelectedPolly, wObjectPicking->IntersectedPollyIndex);
					//swprintf(wPrintInfo->Console_Output,L"Path search time = %d", timeGetTime() - StartSeartchTime);
				
				}else
				{
						//RScel->LineStart = wObjectPicking->LineStart;
						//RScel->LineEnd = wObjectPicking->IntersectionPos;
				}
				if(!NotCount)
				{
					LastSelectedPolly = wObjectPicking->IntersectedPollyIndex;
					LastIntersectionPos = wObjectPicking->LineStart;
				
				}	
			}

        break;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

void gWindow::CalculateDimensions()
{

RECT rect;
GetWindowRect(hWnd, &rect);
wVariables.HaveToCalculateDimensions = false;
WindowStartPosX = rect.left;
WindowStartPosY = rect.top;
WindowEndPosX = rect.right - rect.left;
WindowEndPosY = rect.bottom - rect.top;



GetClientRect(hWnd,&rect);
ClientAreaSizeWidth = rect.right;
ClientAreaSizeHeight = rect.bottom;
WindowClientAreaEndPosX = GetSystemMetrics(SM_CXSIZEFRAME)+WindowStartPosX +rect.right -1;
WindowClientAreaStartPosX = GetSystemMetrics(SM_CXSIZEFRAME)+WindowStartPosX;

WindowClientAreaStartPosY = GetSystemMetrics(SM_CYCAPTION) + WindowStartPosY  + GetSystemMetrics(SM_CYSIZEFRAME);
WindowClientAreaEndPosY = WindowClientAreaStartPosY + rect.bottom-1;//GetSystemMetrics(SM_CYMENU);
WindowCenterPosX = WindowClientAreaStartPosX+((WindowClientAreaEndPosX - (WindowClientAreaStartPosX-1))/2);
WindowCenterPosY = WindowClientAreaStartPosY+((WindowClientAreaEndPosY - (WindowClientAreaStartPosY-1))/2);
//GetWindowRect(hWnd, &rect);
//wPrintInfo->Print(L"Окно Left %d Right %d Top %d Bottom %d", rect.left, rect.right,rect.top,rect.bottom);
}