/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"
extern g3dDevice * w3dDevice;
extern cTexture * Texture;

cMaterial * Material;
	
cMaterial::cMaterial()
{
	FirstMaterial	= NULL;
	LastMaterial	= NULL;
}

cMaterial::~cMaterial()
{
	
}

OneMaterial * cMaterial::LoadMaterial(char * FileName)
{
	unsigned int FileSize =  GetFileSizez(Str("Data\\Materials\\%s.mtl" , FileName));
	if(FileSize == 0xFFFFFFFF)
	{	
		MessageBoxA(0, Str("Warning: Error loading materal \"%s\" :: File does not exists. ",  FileName)  ,"Warning", MB_OK);
		return 0;
	}


	//MessageBoxA(0,FileName,"LoadingMaterial", MB_OK);
	if(LastMaterial == NULL)
	{
		FirstMaterial	=	new OneMaterial;
		LastMaterial	=	FirstMaterial;
		FirstMaterial->MaterialName = new char[strlen(FileName)+1];
		memcpy(&FirstMaterial->MaterialName[0], FileName, strlen(FileName)+1);
		memcpy(&CurrentLoadingMaterial[0], FileName, strlen(FileName)+1);
		FirstMaterial->CompiledMaterial = CompileMaterial(Str("Data\\Materials\\%s.mtl" , FileName), FileSize, FirstMaterial);
		return FirstMaterial;
	}else
	{

	}
	
}

void * cMaterial::CompileMaterial(char * FileName, unsigned int FileSize, OneMaterial * LoadingMaterial)
{
unsigned int  CompiledMaterialSize = 0;
unsigned int  RunOnceCompiledSize = 0;
LastTokenStart = 0;
LastTokenStartReadPos = 0;
LastTokenSize = 0;
if(FileSize == 0) return 0;
char * Data;
Files File;
File.OpenFile(FileName, "rb");
Data = new char[FileSize+1];
File.ReadFile(Data, FileSize);
Data[FileSize]=0;
File.CloseFile();
Compile(Data, false, LoadingMaterial, CompiledMaterialSize, RunOnceCompiledSize);
CompiledData = new unsigned char [CompiledMaterialSize];

LastTokenStart = 0;
LastTokenStartReadPos = 0;
LastTokenSize = 0;
Compile(Data, true, LoadingMaterial,CompiledMaterialSize, RunOnceCompiledSize);
return CompiledData;
}

bool  cMaterial::Compile(char * Data, bool Stage, OneMaterial * LoadingMaterial, unsigned int & CompiledMaterialSize, unsigned int & RunOnceCompiledSize) 
{

if(Stage) NumBytesCompiled = 0;
if(Stage) RunOnceNumBytesCompiled = 0;
// Stage == false ѕервый проход - вернет размер откомпилированного файла.  
// true - второй проход - вернет откомпилированный файл (функцию)
int type = 0;
char * Token;
int LastCurNum;
for(;;)
{
	Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
	if(type == 0)
	{
		break;//cout << "Uncnown type" << endl;

	}
	if(type==BIG_OR_SMALL_LETERS_AND_NUMBERS)
	{
		
			if(CmpString(Token, "Material",LastTokenSize))
			{
					if(! ProcessData(Data, Stage, LoadingMaterial, CompiledMaterialSize, RunOnceCompiledSize) )
						{
							return false;
						}
					
			continue;
			}
	}

	if(type == END_EOF)
	{
		break;	
	}
}
CompiledMaterialSize++;// +ret
if(Stage)
{
CompiledData[NumBytesCompiled++] = 0xC3; // ret
}
return true;// ¬озвраша
}
bool cMaterial::ProcessData(char * Data, bool Stage, OneMaterial * LoadingMaterial, unsigned int & MaterialCompiledSize,unsigned int & RunOnceCompiledSize )
{
unsigned int FirstSamplerStateParameter;
unsigned int SecondSamplerStateParameter;
unsigned int TempInteger;
int type;
int LastCurNum;
char * Token;
Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
if(type == OPEN_BRACKET) // ƒолжен идти OPEN_BRACKET
{
		Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
		while(type != CLOSE_BRACKET)
		{
			if(type == END_EOF)
			{
				CON->ErrorMessage("Error loading material \"%s\". Unexpected end of file", &CurrentLoadingMaterial[0]);
				return false;
			}

			if(CmpString(Token, "Diffuse",LastTokenSize))
			{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading material \"%s\". Invalid diffuse texture parameter.", &CurrentLoadingMaterial[0]);
							return false;
						}

					if(!Stage)
					{
						
						LoadingMaterial->DiffuseTextureIndex =  Texture->LoadTexture(GetStr(Token, LastTokenSize));
						MaterialCompiledSize+=21;
					}else
					{


						CompiledData[NumBytesCompiled++] = 0xB8; // mov eax,
						TempInteger = (unsigned int)&DWORDTemp;
						CompiledData[NumBytesCompiled++] = (TempInteger >> 0); // адрес
						CompiledData[NumBytesCompiled++] = (TempInteger >> 8); // адрес
						CompiledData[NumBytesCompiled++] = (TempInteger >> 16); // адрес
						CompiledData[NumBytesCompiled++] = (TempInteger >> 24); // адрес
						CompiledData[NumBytesCompiled++] = 0x50; // push eax

						CompiledData[NumBytesCompiled++] = 0xB8; // mov eax,

						TempInteger = LoadingMaterial->DiffuseTextureIndex;
						CompiledData[NumBytesCompiled++] = (TempInteger >> 0); // индекс текстуры
						CompiledData[NumBytesCompiled++] = (TempInteger >> 8); // индекс текстуры
						CompiledData[NumBytesCompiled++] = (TempInteger >> 16);// индекс текстуры
						CompiledData[NumBytesCompiled++] = (TempInteger >> 24);// индекс текстуры
						CompiledData[NumBytesCompiled++] = 0x50; // push eax

						CompiledData[NumBytesCompiled++] = 0xB8; // mov eax,

						TempInteger = (unsigned int)&cMaterial::MaterialSetTexture;
						CompiledData[NumBytesCompiled++] = (TempInteger >> 0); // адрес
						CompiledData[NumBytesCompiled++] = (TempInteger >> 8); // адрес
						CompiledData[NumBytesCompiled++] = (TempInteger >> 16); // адрес
						CompiledData[NumBytesCompiled++] = (TempInteger >> 24); // адрес


						//_asm mov Return, eax;
						CompiledData[NumBytesCompiled++] = 0xFF; // call eax
						CompiledData[NumBytesCompiled++] = 0xD0; // call eax

						CompiledData[NumBytesCompiled++] = 0x58; // pop eax
						CompiledData[NumBytesCompiled++] = 0x58; // pop eax

						

					}
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}

						if(CmpString(Token, "SetDiffuseSamplerState",LastTokenSize))
						{
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading material \"%s\". Invalid  sampler state first parameter.", &CurrentLoadingMaterial[0]);
							return false;
						}
					
						// ƒелаем считывани€ на 1м и 2м проходах дл€ того чтоб
						// на первом проверить ощибки
						// на втором считать нужные параметры так как если SetRenerState больше чем 1 то
					    // эти параметры негде сохранить на первом проходе чтоб на 2м их считать
						if(CmpString(Token, "D3DSAMP_MAGFILTER",LastTokenSize))
						{
							FirstSamplerStateParameter = D3DSAMP_MAGFILTER;
						}else if(CmpString(Token, "D3DSAMP_MINFILTER",LastTokenSize))
						{
							FirstSamplerStateParameter = D3DSAMP_MINFILTER;
						}else if(CmpString(Token, "D3DSAMP_MIPFILTER",LastTokenSize))
						{
							FirstSamplerStateParameter = D3DSAMP_MIPFILTER;
						}else if(CmpString(Token, "D3DSAMP_ADDRESSU",LastTokenSize))
						{
							FirstSamplerStateParameter = D3DSAMP_ADDRESSU;
						}else if(CmpString(Token, "D3DSAMP_ADDRESSV",LastTokenSize))
						{
							FirstSamplerStateParameter = D3DSAMP_ADDRESSV;
						}else
						{
							CON->ErrorMessage("Error loading material \"%s\". Invalid sampler state first parameter.", &CurrentLoadingMaterial[0]);
							return false;
						}

						
						Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					if(type!=BIG_OR_SMALL_LETERS_AND_NUMBERS)
						{
							CON->ErrorMessage("Error loading material \"%s\". Invalid  sampler state second parameter.", &CurrentLoadingMaterial[0]);
							return false;
						}

						if(CmpString(Token, "D3DTEXF_LINEAR",LastTokenSize))
						{
							SecondSamplerStateParameter = D3DTEXF_LINEAR;
						}else if(CmpString(Token, "D3DTEXF_NONE",LastTokenSize))
						{
							SecondSamplerStateParameter = D3DTEXF_NONE;
						}else if(CmpString(Token, "D3DTADDRESS_BORDER",LastTokenSize))
						{
							SecondSamplerStateParameter = D3DTADDRESS_BORDER;
						}else
						{
							CON->ErrorMessage("Error loading material \"%s\". Invalid sampler state second parameter.", &CurrentLoadingMaterial[0]);
							return false;
						}
					

					if(!Stage)
					{
						RunOnceCompiledSize+=28;
					}else
					{


						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0xB8; // mov eax,
						TempInteger = (unsigned int)&DWORDTemp;
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 0); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 8); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 16); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 24); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0x50; // push eax

						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0xB8; // mov eax,
						TempInteger = SecondSamplerStateParameter;
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 0); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 8); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 16); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 24); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0x50; // push eax

						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0xB8; // mov eax,
						TempInteger = FirstSamplerStateParameter;
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 0); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 8); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 16); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 24); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0x50; // push eax


						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0xB8; // mov eax,
						TempInteger = (unsigned int)&cMaterial::SamplerStateSet;
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 0); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 8); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 16); // адрес
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = (TempInteger >> 24); // адрес

						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0xFF; // call eax
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0xD0; // call eax

						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0x58; // pop eax
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0x58; // pop eax
						RunOnceCompiledData[RunOnceNumBytesCompiled++] = 0x58; // pop eax

					}
					//w3dDevice->g_pd3dDevice->SetSamplerState(Index,FirstSamplerStateParameter,SecondSamplerStateParameter);
					Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
					continue;
				}


			// сюда попадаем если нет загрузчика
			Token = GetNextTokenSkipSpacebar(Data , type, LastTokenSize, LastTokenStartReadPos, LastTokenStart);
			
			//OnlyForDebug++;
			continue;

		}

	}else
	{ // ‘айл поврежден   To do: —делать вывод сообщени€ об этом
		CON->ErrorMessage("Error loading material \"%s\". Unexpected token.  Expected OpenBracket", &CurrentLoadingMaterial[0]);
	}

return true;
}

void cMaterial::MaterialSetTexture(unsigned int TextureIndex, DWORD * Stage)
{
	//MessageBoxA(0, "TestRenderStateCalled" ,"Warning", MB_OK);
	w3dDevice->g_pd3dDevice->SetTexture(*Stage, Texture->g_pTexture[TextureIndex]);
}


void cMaterial::SamplerStateSet(D3DSAMPLERSTATETYPE Type, unsigned int Value, DWORD * TextureIndex)
{
	w3dDevice->g_pd3dDevice->SetSamplerState(*TextureIndex, Type, Value);
}