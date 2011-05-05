#include "common.h"
#ifndef G_CAMERA
#define G_CAMERA



class gCamera
{
public:
	//D3DXVECTOR3 CameraPosition;
	//D3DXVECTOR3 CameraLookAt;

D3DVIEWPORT9 viewport;
float CameraFOV;
float PickRange;
float AspectRatio;
//int MouseLastPosX;
//int MouseLastPosY;
int MouseSensivity;
int MouseWorldPosX;
int MouseWorldPosY;
D3DXMATRIXA16 matMonPirtor_ojection; //���������
		D3DXMATRIX TransMat;
D3DXMATRIXA16 matMVP;
D3DXMATRIXA16 RenderMat;
D3DXMATRIXA16 matWorldView; // ������� ���� Respawn

D3DXVECTOR3 Camera_Position_C; //Respawn pos
D3DXVECTOR3 Camera_Look_At_C;  //Respawn pos
D3DXVECTOR3 Camera_Up_C;       //Respawn pos
D3DXMATRIXA16 matWorldView_C; // ������� ���� Respawn

bool CameraChanged; //���� true, �� ����� ����������� ������
bool MoveF_B; //���� true �������� ������. ���� false �����
bool MoveL_R; //���� false �������� �� ����. //���� true �������� �� �����.
DWORD Moving_Forward_Or_Backward; // ���� �� NULL ������ ���� ������� ��� ����� �������� ����� ������ ��������� �������� ������ ��� �����.
DWORD Moving_Left_Or_Right; // ���� �� NULL ������ ���� Strife left ��� Strife right �������� ����� ������ ��������� �������� Strife.
DWORD Moving_Forward_Or_Backward_Stop;
DWORD Moving_Left_Or_Right_Stop;
unsigned int	MoveX;
unsigned int	MoveY;

void SetMeshMatrix(D3DXMATRIXA16 *  MeshMat);
void Player_Respawn(D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR3);
void Calculate_Camera(); // �����������(���������) ������
void ReCalcCamMoveFowardBackward(); // �������� ������
void RotateCam();
GamePOINTType GetCameraPosition();
gCamera(D3DXVECTOR3 Pos , D3DXVECTOR3 LookAt, D3DXVECTOR3 Up);
~gCamera();
void CameraMoveForward();
void CameraMoveBackward();
void CameraStrifeLeft();
void CameraStrifeRight();
void MoveStrightAread(int direction);
void RotateCam(int x,int y);
void SetZeroMeshMatrix();
};
#endif