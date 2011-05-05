/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dinput8.lib")
extern gLog * wLog;
extern gWindow * wWindow;
gDirectInput * wDirectInput;
extern gCamera * wCamera;
extern gLog * wLog;
extern gVariables wVariables;
extern gTime * wTime;
extern gPrintInfo * wPrintInfo;
gDirectInput::gDirectInput(){

LastTimePressedKeyboard = wTime->time;
//	initialize directinput8
	if (DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&GameDirectInput, NULL) != DI_OK)
	{
		wLog->addMessage(L"Error:DirectInput8Create");
		return;
	}
InitKeyboard();
if(!wWindow->WindowMode)
InitMouse();



}
void gDirectInput::ProcessInputGameEditMode()
{


	
//GetCursorPos(&cursorpos);
	KeyboardDevice->Acquire();
if (KeyboardDevice->GetDeviceState(256, Keys) != DI_OK)
{
	//MessageBox(NULL,L"sdf",L"sdf",MB_OK);
	//KeyboardDevice->GetDeviceState(256, Keys);
}else
{

}

if (Keys[DIK_TAB])
{
if((wTime->time - LastTimePressedKeyboard)>300)
{
LastTimePressedKeyboard = wTime->time;
	SwitchFromEditToPlayMode();
wLog->addMessage(L"EditTab");
}
}

if (Keys[DIK_W])
{
	//wCamera->CameraMoveForward();
	wCamera->MoveForward();

}
if (Keys[DIK_S])
{
	//wCamera->CameraMoveForward();
	wCamera->MoveBackward();

}

if (Keys[DIK_A])
{
	//wCamera->CameraMoveForward();
	wCamera->StrifeLeft();

}
if (Keys[DIK_D])
{
	//wCamera->CameraMoveForward();
	wCamera->StrifeRight();

}
if (Keys[DIK_UP])
{
	wCamera->MoveForward();

}
if (Keys[DIK_DOWN])
{
	wCamera->MoveBackward();

}
if (Keys[DIK_LEFT])
{
	wCamera->StrifeLeft();

}
if (Keys[DIK_RIGHT])
{
	wCamera->StrifeRight();

}

POINT point;
	if(!wWindow->WindowMode)
	{
// process mouse
dinmouse->Acquire();
dinmouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mousestate);
	}else
	{
		GetCursorPos(&point);
		
		CursorPosXClientAreaCoordSys = point.x-wWindow->WindowClientAreaStartPosX; // To do сделать такойже расчет для full screen
		CursorPosYClientAreaCoordSys = point.y-wWindow->WindowClientAreaStartPosY;	
	}

if(wWindow->WindowMode)
{
wCamera->RotateCam(point.x-wWindow->WindowCenterPosX,point.y-wWindow->WindowCenterPosY);
SetCursorPos(wWindow->WindowCenterPosX,wWindow->WindowCenterPosY);
}else
{
wCamera->RotateCam(mousestate.lX,mousestate.lY);
}

}



#define DIMOUSE_LEFTBUTTON   0

void gDirectInput::ProcessInputGamePlayMode()
{


	
//GetCursorPos(&cursorpos);
	KeyboardDevice->Acquire();
if (KeyboardDevice->GetDeviceState(256, Keys) != DI_OK)
{
	//MessageBox(NULL,L"sdf",L"sdf",MB_OK);
	//KeyboardDevice->GetDeviceState(256, Keys);
}else
{

}

if (Keys[DIK_W])
{
	//wCamera->CameraMoveForward();
	wCamera->MoveForward();

}
if (Keys[DIK_TAB])
{
	if((wTime->time - LastTimePressedKeyboard)>300)
{
LastTimePressedKeyboard = wTime->time;
	
	wLog->addMessage(L"PlayTab");
SwitchFromPlayToEditMode();
	}
}
if (Keys[DIK_S])
{
	//wCamera->CameraMoveForward();
	wCamera->MoveBackward();

}

if (Keys[DIK_A])
{
	//wCamera->CameraMoveForward();
	wCamera->StrifeLeft();

}
if (Keys[DIK_D])
{
	//wCamera->CameraMoveForward();
	wCamera->StrifeRight();

}
if (Keys[DIK_UP])
{
	wCamera->MoveForward();
}
if (Keys[DIK_DOWN])
{
	wCamera->MoveBackward();

if (Keys[DIK_LEFT])
{
	wCamera->StrifeLeft();

}
if (Keys[DIK_RIGHT])
{
	wCamera->StrifeRight();

}




ZeroMemory(&mousestate,sizeof(mousestate));
POINT point;
if(!wWindow->WindowMode)
	{
// process mouse
dinmouse->Acquire();
dinmouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mousestate);
	}else
	{
		GetCursorPos(&point);
		
		CursorPosXClientAreaCoordSys = point.x-wWindow->WindowClientAreaStartPosX; // To do сделать такойже расчет для full screen
		CursorPosYClientAreaCoordSys = point.y-wWindow->WindowClientAreaStartPosY;	
		//wsprintf(wPrintInfo->Console_Output,L"%d  %d", CursorPosXClientAreaCoordSys,CursorPosYClientAreaCoordSys);
	
	}

if(wWindow->WindowMode)
{
wCamera->RotateCam(0,0);
}else
{
wCamera->RotateCam(0,0);
}

}
}
//mousestate.lY
gDirectInput::~gDirectInput()
{

}

void	gDirectInput::InitKeyboard()
{
if (GameDirectInput->CreateDevice(GUID_SysKeyboard, &KeyboardDevice, NULL) != DI_OK)
	{
		wLog->addMessage(L"Error:DirectInput->CreateDevice");
		return;
	}

	if (!KeyboardDevice)
	{
		wLog->addMessage(L"Error:KeyboardDevice==NULL");
		return;
	}
	
	if (KeyboardDevice->SetDataFormat(&c_dfDIKeyboard) != DI_OK) {
		KeyboardDevice->Release();
		KeyboardDevice = NULL;
		wLog->addMessage(L"Error:KeyboardDevice->SetDataFormat");
		return;
	}	
if (KeyboardDevice->SetCooperativeLevel(wWindow->hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) != DI_OK) {
		KeyboardDevice->Release();
		KeyboardDevice = NULL;
		wLog->addMessage(L"Error:KeyboardDevice->SetCooperativeLevel");
		return ;
	}
KeyboardDevice->Acquire();
}

void	gDirectInput::InitMouse()
{
GameDirectInput->CreateDevice(GUID_SysMouse, &dinmouse, NULL);
dinmouse->SetDataFormat(&c_dfDIMouse2);
dinmouse->SetCooperativeLevel(wWindow->hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
/*
		DIPROPDWORD  dipdw; 
	dipdw.diph.dwSize = sizeof(DIPROPDWORD); 
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	dipdw.diph.dwObj = 0; 
	dipdw.diph.dwHow = DIPH_DEVICE; 
	dipdw.dwData = DIPROPAXISMODE_ABS ; 

	dinmouse->SetProperty(DIPROP_AXISMODE , &dipdw.diph); 
	*/
}