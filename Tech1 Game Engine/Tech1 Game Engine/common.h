/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef G_COMMON
#define G_COMMON
#define DIRECTINPUT_VERSION 0x0800
#define GameName L"Tech 1 v0.01"
#include <Windows.h>
#include <mmsystem.h>
#include <d3dx9.h>
#include <dinput.h>
#include <strsafe.h>
#define IN_DEVELOPMENT
	struct GamePOINTType
	{
		float x;
		float y;
	};
#pragma comment(lib,"d3dxof.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"winmm.lib")
#include "ConnectedListVertexAdjency.h"
#include "BinaryHeap.h"
#include "Camera.h"
#include "gWindow.h"
#include "g3dDevice.h"
#include "Vector.h"
#include "gTerrain.h"
#include "gShaders.h"
#include "ConnectedListIntegers.h"
#include "gDirectInput.h"
#include "gLog.h"
#include "gVariables.h"
#include "gPrintInfo.h"
#include "ScriptReader.h"
#include "Mesh.h"
#include "Time.h"
#include "RenderSceleton.h"

#include "DynamicLight.h"
#include "EditorGeometry.h"
#include "ShadowVolume.h"
#include "ObjectPicking.h"
#include "PathFind.h"
#include "Map.h"
#include "FileIO.h"
#include "Scene.h"
#include "BBox.h"
#include "Console.h"
#include "Mesh.h"
#include "BinaryHeap.h"
#include "Game.h"
#include "RenderToTexture.h"
#include "PSSM.h"
#include "Quad.h"
#include "Material.h"
#include "Texture.h"
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
VOID RenderGameEditMode();
void gMainFrame();
bool GameStartup();
void GameShutDown();
void GameStartupFrame();
void GameMainMenuFrame();
void GamePlayFrame();
void GameEditFrame();
void SwitchFromPlayToEditMode();
void SwitchFromStartupToMainMenu();
void SwitchFromMainMenuToPlayMode();
void ShutDownFrame();
void SwitchFromEditToPlayMode();
unsigned GetFileSizez(char * FileName);
char * GetNextTokenSkipSpacebar(char * Data, int & type,int & LastTokenSize, int & LastTokenStartReadPos, int & LastTokenStart);
char * GetNextToken(int & type, char * ScriptData,int & LastTokenSize, int & LastTokenStartReadPos, int & LastTokenStart);
bool CmpString(char * Str1, char * Str2, unsigned int Str1Size);
int Token(char * Buffer, int  StartPos, int & type, int & LastTokenStartReadPos, int & LastTokenStart);
int ConvertTextToNumberInteger(char * Text, int size, int & LastCurNum); // size - размер текста без 0
float ConvertTextToNumberFloat(char * Text, int size);
char * Str(char * str, ...);
char * GetStr(char * string, unsigned int size);

typedef void (*tRunScript)();
#endif