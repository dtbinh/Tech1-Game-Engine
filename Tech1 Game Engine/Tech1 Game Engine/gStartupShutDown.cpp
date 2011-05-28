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
extern Shaders * GameShaders;
extern cMesh * Mesh;
extern gTime * wTime;
extern RendererSceleton * RScel;
extern EditorGeometry * EDgeom;
extern ObjectPicking * wObjectPicking;
extern PathFind * PathFinder;
extern ScriptReader * ScriptLoader;
extern SceneMap * Map;
extern BBox * Boxes;
extern Console * CON;
extern BinaryHeap * BHeap;
extern Scene * Sce;
extern Game * aGame;
//extern XPSM * XPSMShadow;
extern cPSSM * PSSM;
extern QUAD * Quad;
extern cRenderToTexture * RenderToTexture;
extern cMaterial * Material;
extern cTexture * Texture;
bool GameStartup()
{

CON = new Console;
wWindow = new gWindow;
w3dDevice = new g3dDevice;
Texture = new cTexture(10); // 10 Текстур в сцене
Sce = new Scene;
ScriptLoader = new ScriptReader;
Mesh = new cMesh;
	wTime = new gTime;
wLog = new gLog;

wDirectInput = new gDirectInput;


wPrintInfo = new gPrintInfo;
wTerrain = new gTerrain(16,16,1.0f,0.0f,0.0f,0.0f);
wCamera = new gCamera;

wCamera->SetRespawnPosition(&D3DXVECTOR3( 0.0f, 0.0f, -1.0f ),&D3DXVECTOR3( 0.0f, 0.0f, 1.0f ));

//wTerrain->LoadTexture(0,L"Data\\Textures\\Tex1.jpg");//Asphalt1.JPG  alyx_sheet.tga
//wTerrain->LoadTexture(1,L"Data\\Textures\\Tex2.jpg");

//Texture->LoadTexture("Tex2.jpg");
//wTerrain->LoadTexture(2,L"Textures\\dirt5.jpg");
//wTerrain->LoadTexture(3,L"Textures\\mask.bmp");

GameShaders = new Shaders;
Quad = new QUAD;

//XPSMShadow = new XPSM;
RenderToTexture = new cRenderToTexture;
PSSM = new cPSSM;
EDgeom = new EditorGeometry;
char * FileName = "c:\\Data.txt";
//Mesh->AnimAllocMemory(6);
Material = new cMaterial;
Map = new SceneMap;
Map->LoadMap("Level1.t1l");
//Mesh->LoadAnimationFromFile("Data\\Animations\\SRF.t1a",0);
//Mesh->LoadMeshFromFile("Data\\Meshes\\SRF.t1m", false);
Sce -> DlightsAllocMemory(1);

BHeap = new BinaryHeap;
PathFinder = new PathFind;
Boxes = new BBox;
RScel = new RendererSceleton;
wObjectPicking = new ObjectPicking;

aGame = new Game;



return true;
}

void GameShutDown()
{
//delete XPSMShadow;
delete RenderToTexture;
delete aGame;
delete Quad;
delete Map;
delete ScriptLoader;
delete PathFinder;
delete wObjectPicking;
delete EDgeom;
delete RScel;
delete Mesh;
delete GameShaders;
delete wCamera;
delete wTerrain;
delete wPrintInfo;
delete w3dDevice;
delete wDirectInput;
delete wWindow;
delete wLog;
delete wTime;
delete CON;
delete Sce;
}