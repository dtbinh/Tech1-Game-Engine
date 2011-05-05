/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#ifndef _MATERIAL_
#define _MATERIAL_

struct OneMaterial
{
 char * MaterialName;// Material name == material fileame
 void * CompiledMaterial;
 unsigned int DiffuseTextureIndex;
 OneMaterial * NextMaterial;
};

class cMaterial
{
private:
	OneMaterial * FirstMaterial;
	OneMaterial * LastMaterial;
	// Render State Flags;
	static void MaterialSetTexture(unsigned int TextureIndex, DWORD * Stage);
	static void cMaterial::SamplerStateSet(D3DSAMPLERSTATETYPE Type, unsigned int Value, DWORD * TextureIndex);

	// Temporarys
	DWORD DWORDTemp;
public:
	
	cMaterial::cMaterial();
	cMaterial::~cMaterial();

	 OneMaterial * LoadMaterial(char * FileName);
	 void * CompileMaterial(char * FileName, unsigned int FileSize, OneMaterial * LoadingMaterial);
	 bool ProcessData(char * Data, bool Stage, OneMaterial * LoadingMaterial, unsigned int & MaterialCompiledSize,unsigned int & RunOnceCompiledSize );
	 bool Compile(char * Data, bool Stage, OneMaterial * LoadingMaterial, unsigned int & CompiledMaterialSize, unsigned int & RunOnceCompiledSize) ;
     

int LastTokenStartReadPos;
int LastTokenSize;
int LastTokenStart;
char CurrentLoadingMaterial[255];
unsigned char * CompiledData;
unsigned char * RunOnceCompiledData;
unsigned int NumBytesCompiled;
unsigned int RunOnceNumBytesCompiled;


// Temporarys
void SetDWORDTemporary(DWORD Value) { DWORDTemp = Value; }
};

#endif