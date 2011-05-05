/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
#ifndef _SHADERS
#define _SHADERS
#define GameShaderPath 		  "Data\\Shader\\"
#define VertexShaderFinalRenderingFileName      "VertexShaderFinalRendering.txt"
#define PixelShaderFinalRenderingFileName      "PixelShaderFinalRendering.txt"
#define VertexShaderMeshRenderingFileName     "VertexShaderMeshRendering.txt"
#define PixelShaderMeshRenderingFileName	 "PixelShaderMeshRendering.txt"

#define PixelShaderMeshRendering2FileName	 "PixelShaderMeshRendering2.txt"

#define ShadowVolumeSilueteVertexShaderFileName     "ShadowVolumeSilueteVertexShader.txt"
#define ShadowVolumeSiluetePixelShaderFileName	 "ShadowVolumeSiluetePixelShader.txt"

#define VertexShaderPSSM     "VSRenderToShadowMap.txt"
#define PixelShaderPSSM     "PSRenderToShadowMap.txt"

#define VertexShaderPSSMFinal    "PSSM VS FINAL.txt"
#define PixelShaderPSSMFinal     "PSSM PS FINAL.txt"

#define ShadowMapVS     "ShadowMapVS.txt"
#define ShadowMapPS     "ShadowMapPS.txt"

#define QuadShader     "QuadShader.txt"
//#define PixelShaderMeshRenderingFileName	 "PixelShaderMeshRendering.txt"


#define VertexShadowVolume 0
#define PixelShadowVolume 1

#define VertexFinalRendering 2
#define PixelFinalRendering 3

#define VertexMeshRendering 4
#define PixelMeshRendering 5

#define VertexPSSM 6
#define PixelPSSM 7

#define PixelMeshRendering2 8

#define VertexPSSMFinal 9
#define PixelPSSMFinal 10

#define ShadowMapVSIndex 11
#define ShadowMapPSIndex 12

#define QuadShaderIndex 13
enum ShaderTypeEnum
{
SHADER_TYPE_VERTEX_SHADER,
SHADER_TYPE_PIXEL_SHADER
};

#define MAX_TEXTURE_SAMPLES_IN_SHADER 4
struct OneShader
{
LPD3DXBUFFER Code; // убрать отсюда
LPD3DXCONSTANTTABLE ConstantTable;
ShaderTypeEnum ShaderType;
LPDIRECT3DPIXELSHADER9 PixelShader;
LPDIRECT3DVERTEXSHADER9 VertexShader;
DWORD ShaderTextures[MAX_TEXTURE_SAMPLES_IN_SHADER];
unsigned int NumTexturesInShader;
};

class Shaders
{
public:
OneShader * ShadersList;
unsigned int NumShaders;

bool ShadersAllocMemory(unsigned int NumMemory);
bool LoadShaderFromFile(char * ShaderFileName, unsigned int NewShaderIndex, char * ShaderVersion, unsigned int NumTexturesInShader);
DWORD GetTextureIndexForShader(unsigned int ShaderIndex, unsigned int TextureNum);
void SaveTextureIndexesForShader(unsigned int ShaderIndex, unsigned int NumTexturesInShader);
void SetMatrixForShader(unsigned int ShaderIndex, char * MatrixName, D3DXMATRIXA16 * Matrix);
void SetShader(unsigned int ShaderIndex);
void SetFloatForShader(unsigned int ShaderIndex, char * FloatName, float FloatToSet);
IDirect3DVertexDeclaration9* VertexDeclaration;
IDirect3DVertexDeclaration9* VertexDeclarationMeshRendering;
Shaders();
~Shaders();

};
#endif