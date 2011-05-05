/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
extern g3dDevice * w3dDevice;
extern gTerrain * wTerrain;
extern Console * CON;
Shaders * GameShaders;

bool Shaders::LoadShaderFromFile(char * ShaderFileName, unsigned int NewShaderIndex, char * ShaderVersion, unsigned int NumTexturesInShader)
{
	LPD3DXBUFFER BufferErrors;
	char FileName[MAX_STRING_SIZE];
	ShadersList[NewShaderIndex].Code = NULL;
	ShadersList[NewShaderIndex].ConstantTable = NULL;
	ShadersList[NewShaderIndex].PixelShader = NULL;
	ShadersList[NewShaderIndex].VertexShader = NULL;

	if((ShaderVersion[0] == 'v') && (ShaderVersion[1] == 's'))
	{
		ShadersList[NewShaderIndex].ShaderType = SHADER_TYPE_VERTEX_SHADER;
	}else if((ShaderVersion[0] == 'p') && (ShaderVersion[1] == 's'))
	{
		ShadersList[NewShaderIndex].ShaderType = SHADER_TYPE_PIXEL_SHADER;
	}else
	{
		CON->ErrorMessage("Error loading \"%s\". Invalid  shader version \"%s\"", ShaderFileName, ShaderVersion);
		return false;
	}

	sprintf_s(FileName,GameShaderPath);
	strcat_s(FileName,ShaderFileName);
	if( FAILED(D3DXCompileShaderFromFileA(FileName, NULL, NULL, "main", ShaderVersion, D3DXSHADER_USE_LEGACY_D3DX9_31_DLL, &ShadersList[NewShaderIndex].Code, &BufferErrors, &ShadersList[NewShaderIndex].ConstantTable )))
	{
		char * ErrMsg = Str("Fatal Error: Error While Compiling Shader \"%s\"", ShaderFileName);
		char ErrorMessage[MAX_STRING_SIZE];
		sprintf_s(ErrorMessage,ErrMsg);
		strcat_s(ErrorMessage,FileName);
		strcat_s(ErrorMessage, "\nError Message:\n");
		if((char*)BufferErrors == NULL) strcat_s(ErrorMessage,"no message" );
		else {char * ErMSG = (char*)BufferErrors->GetBufferPointer();strcat_s(ErrorMessage,ErMSG );}
		CON->ErrorMessage(ErrorMessage);
	}
	if(ShadersList[NewShaderIndex].ShaderType == SHADER_TYPE_VERTEX_SHADER)
	{
		if( FAILED(w3dDevice->g_pd3dDevice->CreateVertexShader( (DWORD*)ShadersList[NewShaderIndex].Code->GetBufferPointer(), &ShadersList[NewShaderIndex].VertexShader )))
		{
			CON->ErrorMessage("Unable create vertex shader \"%s\"", ShaderFileName);
			return false;
		}
	}else
	{
		if( FAILED(w3dDevice->g_pd3dDevice->CreatePixelShader( (DWORD*)ShadersList[NewShaderIndex].Code->GetBufferPointer(), &ShadersList[NewShaderIndex].PixelShader )))
		{
			CON->ErrorMessage("Unable create pixel shader \"%s\"", ShaderFileName);
			return false;
		}
	}
SaveTextureIndexesForShader(NewShaderIndex, NumTexturesInShader);
	return true;
}

void Shaders::SetShader(unsigned int ShaderIndex)
{
	if(ShadersList[ShaderIndex].ShaderType == SHADER_TYPE_VERTEX_SHADER)
	{
		w3dDevice->g_pd3dDevice->SetVertexShader(ShadersList[ShaderIndex].VertexShader);
	}else
	{
		w3dDevice->g_pd3dDevice->SetPixelShader(ShadersList[ShaderIndex].PixelShader);
	}
}

void Shaders::SetMatrixForShader(unsigned int ShaderIndex, char * MatrixName, D3DXMATRIXA16 * Matrix)
{
	D3DXHANDLE MatHandle;
	MatHandle = ShadersList[ShaderIndex].ConstantTable->GetConstantByName(0, MatrixName);
	ShadersList[ShaderIndex].ConstantTable->SetMatrix(w3dDevice->g_pd3dDevice, MatHandle, Matrix);
}

void Shaders::SetFloatForShader(unsigned int ShaderIndex, char * FloatName, float FloatToSet)
{
	D3DXHANDLE FloatHandle;
	FloatHandle = ShadersList[ShaderIndex].ConstantTable->GetConstantByName(0, FloatName);
	ShadersList[ShaderIndex].ConstantTable->SetFloat(w3dDevice->g_pd3dDevice, FloatHandle, FloatToSet);
}

DWORD Shaders::GetTextureIndexForShader(unsigned int ShaderIndex, unsigned int TextureNum)
{
return ShadersList[ShaderIndex].ShaderTextures[TextureNum];
}

void Shaders::SaveTextureIndexesForShader(unsigned int ShaderIndex, unsigned int NumTexturesInShader)
{
D3DXHANDLE TexHandle;
D3DXCONSTANT_DESC TextureDesc;
UINT count;
int i;

for(i=0 ; i!= NumTexturesInShader ; i++ )
{
	TexHandle = ShadersList[ShaderIndex].ConstantTable->GetConstantByName(0, Str("tex%d", i));
	ShadersList[ShaderIndex].ConstantTable->GetConstantDesc(TexHandle, &TextureDesc,  &count);
	ShadersList[ShaderIndex].ShaderTextures[i] = TextureDesc.RegisterIndex;
}
}

bool Shaders::ShadersAllocMemory(unsigned int NumMemory)
{
	if(ShadersList)
	{
		CON->ErrorMessage("ShadersAllocMemory: Unable to alloc memory. ShadersList != NULL");
		return false;
	}
	ShadersList = new OneShader[NumMemory];
	NumShaders = NumMemory;
	return true;
}
Shaders::Shaders()
{
	DWORD Index;
	ShadersList = NULL;
	NumShaders = NULL;
char FileName[MAX_STRING_SIZE];
VertexDeclaration = NULL;
VertexDeclarationMeshRendering = NULL;
//-------------------------
D3DVERTEXELEMENT9 VDeclaration[] = {
{ 0,  0, D3DDECLTYPE_FLOAT3   , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION ,    0 },
{ 0, 12, D3DDECLTYPE_FLOAT2   , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD ,    0 },
{ 0, 20, D3DDECLTYPE_FLOAT2   , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD ,    1 },
D3DDECL_END() };

D3DVERTEXELEMENT9 VDeclarationMeshRendering[] = {
{ 0,  0, D3DDECLTYPE_FLOAT3   , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION ,    0 },
{ 0, 12, D3DDECLTYPE_FLOAT2   , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD ,    0 },
D3DDECL_END() };






w3dDevice->g_pd3dDevice->CreateVertexDeclaration(VDeclaration, &VertexDeclaration);
w3dDevice->g_pd3dDevice->SetVertexDeclaration(VertexDeclaration);
w3dDevice->g_pd3dDevice->CreateVertexDeclaration(VDeclarationMeshRendering, &VertexDeclarationMeshRendering);

//RenderingDevice->g_pd3dDevice->CreateVertexDeclaration(VDeclaration, &VertexDeclaration);
//RenderingDevice->g_pd3dDevice->SetVertexDeclaration(VertexDeclaration);





ShadersAllocMemory(14);

LoadShaderFromFile(ShadowVolumeSilueteVertexShaderFileName, 0, "vs_1_1",0);
LoadShaderFromFile(ShadowVolumeSiluetePixelShaderFileName, 1, "ps_1_4", 0);
LoadShaderFromFile(VertexShaderFinalRenderingFileName, 2, "vs_1_1",0);
LoadShaderFromFile(PixelShaderFinalRenderingFileName, 3, "ps_1_4", 4);
LoadShaderFromFile(VertexShaderMeshRenderingFileName, 4, "vs_1_1", 0);
LoadShaderFromFile(PixelShaderMeshRenderingFileName, 5, "ps_1_4", 2);
LoadShaderFromFile(VertexShaderPSSM, 6, "vs_1_1", 0);
LoadShaderFromFile(PixelShaderPSSM, 7, "ps_2_0", 0);
LoadShaderFromFile(PixelShaderMeshRendering2FileName, 8, "ps_1_4", 0);
LoadShaderFromFile(VertexShaderPSSMFinal, 9, "vs_1_1", 0);
LoadShaderFromFile(PixelShaderPSSMFinal, 10, "ps_2_0", 2);
LoadShaderFromFile(ShadowMapVS, ShadowMapVSIndex, "vs_1_1", 0);
LoadShaderFromFile(ShadowMapPS, ShadowMapPSIndex, "ps_2_0", 0);
LoadShaderFromFile(QuadShader, QuadShaderIndex, "ps_2_0", 1);
Index = GetTextureIndexForShader(10, 0);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
w3dDevice->g_pd3dDevice->SetSamplerState(Index,D3DSAMP_ADDRESSU,D3DTADDRESS_BORDER);
w3dDevice->g_pd3dDevice->SetSamplerState(Index,D3DSAMP_ADDRESSV,D3DTADDRESS_BORDER);

if(wTerrain->g_pTexture[0])
{
Index = GetTextureIndexForShader(5, 0);
w3dDevice->g_pd3dDevice->SetTexture(Index, wTerrain->g_pTexture[0]);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

if(wTerrain->g_pTexture[1])
{
Index = GetTextureIndexForShader(5, 1);
w3dDevice->g_pd3dDevice->SetTexture(Index, wTerrain->g_pTexture[1]);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

if(wTerrain->g_pTexture[2])
{
Index = GetTextureIndexForShader(5, 2);
w3dDevice->g_pd3dDevice->SetTexture(Index, wTerrain->g_pTexture[2]);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

if(wTerrain->g_pTexture[3])
{
Index = GetTextureIndexForShader(5, 3);
w3dDevice->g_pd3dDevice->SetTexture(Index, wTerrain->g_pTexture[3]);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

if(wTerrain->g_pTexture[4])
{
Index = GetTextureIndexForShader(5, 4);
w3dDevice->g_pd3dDevice->SetTexture(Index, wTerrain->g_pTexture[4]);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
w3dDevice->g_pd3dDevice->SetSamplerState(Index, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

}

Shaders::~Shaders()
{
	int i;

	for(i = 0; i != NumShaders; i++)
	{
		ShadersList[i].Code->Release();
		ShadersList[i].ConstantTable->Release();
		if( ShadersList[i].ShaderType == SHADER_TYPE_VERTEX_SHADER) ShadersList[i].VertexShader->Release();
		else ShadersList[i].PixelShader->Release();
	}


	VertexDeclarationMeshRendering->Release();
	VertexDeclaration->Release();

}