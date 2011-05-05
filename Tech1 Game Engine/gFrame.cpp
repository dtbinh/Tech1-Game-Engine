/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
extern g3dDevice * w3dDevice;
extern  gWindow * wWindow;
extern gTerrain * wTerrain;
extern gCamera * wCamera;
extern gLog * wLog;
extern gDirectInput * wDirectInput;
extern gVariables wVariables;
extern gPrintInfo * wPrintInfo;
extern cMesh * Mesh;
extern gTime * wTime;
extern RendererSceleton * RScel;
extern EditorGeometry * EDgeom;
extern gPrintInfo * wPrintInfo;
extern ObjectPicking * wObjectPicking;
extern PathFind * PathFinder;
extern Scene * Sce;
//extern XPSM * XPSMShadow;
extern unsigned int VStart; // Только для дебага. Удалить
extern unsigned int VEnd;// Только для дебага. Удалить
D3DXVECTOR3 LS;  //// Только для дебага. Удалить
D3DXVECTOR3 LE;  //// Только для дебага. Удалить
extern Game * aGame;
void gMainFrame()
{

	switch (wVariables.GameState)
	{
		case GMAE_STARTUP:
		GameStartupFrame();
		return;
		
		

		case GAME_MAIN_MENU:
		GameMainMenuFrame();
		return;

		case GAME_PLAY_MODE:
		GamePlayFrame();
		return;

		case GAME_EDIT_MODE:
		GameEditFrame();
		return;

		case GAME_SHUTDOWN:
		ShutDownFrame();
		return;
	}


}

void RenderGamePlayMode()
{
RenderGameEditMode();
}

VOID RenderGameEditMode()
{
unsigned int VertexIndex;
aGame->Tick();
OneMesh * MeshList = Mesh->GetMeshes();
    w3dDevice->g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET| D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB( 100, 100, 100 ), 1.0f, 0 );
//w3dDevice->g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
  //                      0x000000ff, 1.0f, 0L);

    if( SUCCEEDED( w3dDevice->g_pd3dDevice->BeginScene() ) )
    {
//w3dDevice->g_pd3dDevice->SetTransform( D3DTS_VIEW, &wCamera->matView );
		//wCamera->Calculate_Camera();
		
//wTerrain->Render();
Sce->Render();
/////////////////////////////////////////Mesh->RenderAnimatedMesh(7, 0);
//Mesh->RenderOnlyFrontCulledPollys();
//Mesh->RenderOneMesh();
//EDgeom->Render();
//RenderSiluete();
//RScel->RenderSkeleton();
//RScel->RenderNormals();
//RScel->render(&RScel->LineStart,&RScel->LineEnd);

if(PathFinder->LastVertexAddedToClosedListIndex != MESH_MAX_VERTICES)
{
VertexIndex = PathFinder->LastVertexAddedToClosedListIndex;
for (;;)
{
	RScel->LineStart = *(D3DXVECTOR3 *)&MeshList[0].NavMeshVertices[VertexIndex].VertexX;
VertexIndex = MeshList[0].NavMeshVertices[VertexIndex].ParrentIndex;
if(VertexIndex == MESH_MAX_VERTICES) break;
RScel->LineEnd = *(D3DXVECTOR3 *)&MeshList[0].NavMeshVertices[VertexIndex].VertexX;
RScel->render(&RScel->LineStart,&RScel->LineEnd);
}
}


//RScel->LineStart = *(D3DXVECTOR3 *)&Mesh->Mesh.Mesh.NavMeshVertices[VStart].VertexX;// Только для дебага. Удалить
//RScel->LineEnd = *(D3DXVECTOR3 *)&Mesh->Mesh.Mesh.NavMeshVertices[VEnd].VertexX;// Только для дебага. Удалить
RScel->render(&RScel->LineStart,&RScel->LineEnd);

RScel->LineStart = LS;// Только для дебага. Удалить
RScel->LineEnd = LE;// Только для дебага. Удалить
RScel->render(&RScel->LineStart,&RScel->LineEnd);
//	D3DXVECTOR3 End(31.49f, -1.62f, 25.46f);
//D3DXVECTOR3 Start(-16.93f, 0.0f, -19.58f);
//RScel->LineStart = Start;
//RScel->LineEnd = End;
//RScel->render(&RScel->LineStart,&RScel->LineEnd);

//D3DXVECTOR3(TempMesh->NavMeshVertices[TempMesh->Polygons[i].PollyVertices[1].VertexIndex].VertexX - TempMesh->NavMeshVertices[TempMesh->Polygons[i].PollyVertices[0].VertexIndex].VertexX, TempMesh->NavMeshVertices[TempMesh->Polygons[i].PollyVertices[1].VertexIndex].VertexY - TempMesh->NavMeshVertices[TempMesh->Polygons[i].PollyVertices[0].VertexIndex].VertexY, TempMesh->NavMeshVertices[TempMesh->Polygons[i].PollyVertices[1].VertexIndex].VertexZ - TempMesh->NavMeshVertices[TempMesh->Polygons[i].PollyVertices[0].VertexIndex].VertexZ), &));
//RScel->LineStart = *(D3DXVECTOR3 *)&Mesh->Mesh.Mesh.NavMeshVertices[Mesh->Mesh.Mesh.Polygons[0].PollyVertices[0].VertexIndex].VertexX;
//float Vertex0_X = ;
//float Vertex0_Y = ;
//float Vertex0_Z = ;

//float Vertex1_X = ;
//float Vertex1_Y = ;
//float Vertex1_Z =;

//float Vertex2_X = ;
//float Vertex2_Y = ;
//float Vertex2_Z =;

//float Vertex3_X = ;
//float Vertex3_Y = ;
//float Vertex3_Z =;






//D3DXVECTOR3 Vec2 = ;
//D3DXVECTOR3 Vec3 = ;
//D3DXVECTOR3 Vec4 = ;





//RScel->LineEnd = Vec1ForNormal ;  //Mesh->Mesh.Mesh.PollygonEdgeNormals[0].Edge1Normal;

//RScel->LineEnd = Edge2Normal + RScel->LineStart;
//RScel->LineEnd = Vec2 + RScel->LineStart;
//RScel->LineEnd = Edge4Normal + RScel->LineStart;
//RScel->render(&RScel->LineStart,&RScel->LineEnd);

wPrintInfo->Render();
w3dDevice->g_pd3dDevice->EndScene();
    }


    w3dDevice->g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	//wPrintInfo->Console_Output[0] = 0;
}


void GameStartupFrame()
{
	GameStartup();
	SetCursorPos(wWindow->WindowCenterPosX,wWindow->WindowCenterPosY);
SwitchFromStartupToMainMenu();
}

void GameMainMenuFrame()
{
	//////////////
	D3DXVECTOR3 End(30.66f, 24.30f, -90.48f);
D3DXVECTOR3 Start(-90.80f, 28.29f, 55.97f);

	//PathFinder->FindPath(&End, &Start, 25, 249);
SwitchFromMainMenuToPlayMode();
}

void ShutDownFrame()
{
GameShutDown();
PostQuitMessage( 0 );
}
void GamePlayFrame() 
{
wTime->ProcessTime();
#ifdef IN_DEVELOPMENT
if(wVariables.AutoSwitchToEditMode)
SwitchFromPlayToEditMode();
wVariables.AutoSwitchToEditMode=false;
#endif
if(wVariables.HaveToCalculateDimensions || wWindow->WindowMode)
	{
		wWindow->CalculateDimensions();
		wVariables.HaveToCalculateDimensions = false;
	}

wDirectInput->ProcessInputGamePlayMode();


RenderGameEditMode();
}

void GameEditFrame()
{
	wTime->ProcessTime();
	if(wVariables.HaveToCalculateDimensions || wWindow->WindowMode)
	{
		wWindow->CalculateDimensions();
		wVariables.HaveToCalculateDimensions = false;
	}
wDirectInput->ProcessInputGameEditMode();


RenderGameEditMode();
}


void SwitchFromEditToPlayMode()
{
wVariables.GameState = GAME_PLAY_MODE;
wLog->addMessage(L"SwitchFromEditToPlayMode()");
}

void SwitchFromPlayToEditMode()
{
wVariables.GameState = GAME_EDIT_MODE;
wLog->addMessage(L"SwitchFromPlayToEditMode()");
}

void SwitchFromStartupToMainMenu()
{
wVariables.GameState = GAME_MAIN_MENU;
wLog->addMessage(L"SwitchFromStartupToMainMenu()");
}

void SwitchFromMainMenuToPlayMode()
{
wVariables.GameState = GAME_PLAY_MODE;
wLog->addMessage(L"SwitchFromMainMenuToPlayMode()");
}