/*
Copyright (C) 2011 Tech1 Game Engine.
*/

#include "common.h"

QUAD * Quad;
extern g3dDevice * w3dDevice;
extern Console * CON;
QUAD::QUAD()
{
HRESULT hr = S_OK;

    hr = w3dDevice->g_pd3dDevice->CreateVertexBuffer(4 * sizeof(SMVertex), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &pVB, NULL);
    if(FAILED(hr))
       CON->ErrorMessage("QUAD::QUAD(): CreateVertexBuffer Failed");

    SMVertex* pVData;
    hr = pVB->Lock(0, 0, (void**)&pVData, 0);
    if(FAILED(hr))
         CON->ErrorMessage("QUAD::QUAD(): pVB->Lock Failed");
    float value = SMQUAD_SIZE;
    pVData[0].x  = -value; pVData[0].y  = value; pVData[0].z  = -1.0f;
	pVData[0].u = 0.0f;
	pVData[0].v = 1.0f;
    pVData[1].x  = value;  pVData[1].y  = value; pVData[1].z  = -1.0f;
	pVData[1].u = 1.0f;
	pVData[1].v = 1.0f;
    pVData[2].x  = -value; pVData[2].y  = -value; pVData[2].z  = -1.0f;
	pVData[2].u = 0.0f;
	pVData[2].v = 0.0f;
    pVData[3].x  = value;  pVData[3].y  = -value; pVData[3].z  = -1.0f;
	pVData[3].u = 1.0f;
	pVData[3].v = 0.0f;

    hr = pVB->Unlock();
    if(FAILED(hr))
        CON->ErrorMessage("QUAD::QUAD(): Unlock() Failed");

    hr = w3dDevice->g_pd3dDevice->CreateIndexBuffer(4 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &pIB, NULL);
    if(FAILED(hr))
        CON->ErrorMessage("QUAD::QUAD(): CreateIndexBuffer Failed");

    WORD* pIData;
    hr = pIB->Lock(0, 0, (void**)&pIData, 0);
    if(FAILED(hr))
        CON->ErrorMessage("QUAD::QUAD(): pIB->Lock Failed");
    //it's a strip
    pIData[0] = 0;
    pIData[1] = 1;
    pIData[2] = 2;
    pIData[3] = 3;
    hr = pIB->Unlock();
	if(FAILED(hr))
        CON->ErrorMessage("QUAD::QUAD(): pIB->Unlock() Failed");
    dwNumFaces = 2;
    dwNumVerts = 4;
    primType = D3DPT_TRIANGLESTRIP;

    //quad doesn't get scaled / translated
    scaleVec = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
    transVec = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


}
void QUAD::Render()
{
 
    HRESULT hr = S_OK;

    hr = w3dDevice->g_pd3dDevice->SetStreamSource(0, pVB, 0, sizeof(SMVertex));
    if (FAILED(hr))
       CON->ErrorMessage("QUAD::Render(): SetStreamSource Failed");

    //set index buffer
    hr = w3dDevice->g_pd3dDevice->SetIndices(pIB);
    if(FAILED(hr))
        CON->ErrorMessage("QUAD::Render(): SetIndices Failed");


    
     if(FAILED(w3dDevice->g_pd3dDevice->DrawIndexedPrimitive(primType, 0, 0, dwNumVerts, 0, dwNumFaces)))
      CON->ErrorMessage("QUAD::Render(): DrawIndexedPrimitive Failed");

}
QUAD::~QUAD()
{

}