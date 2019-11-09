//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "camera.h"
#include "input.h"
#include "model.h"
#include <stdio.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define POS_CAMERA_P_X		(0.0f)
#define POS_CAMERA_P_Y		(100.0f)
#define POS_CAMERA_P_Z		(-200.0f)

#define POS_CAMERA_R_X		(0.0f)
#define POS_CAMERA_R_Y		(0.0f)
#define POS_CAMERA_R_Z		(0.0f)

#define VIEW_ANGLE			(XMConvertToRadians(45.0f))
#define VIEW_ASPECT			((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT)
#define VIEW_NEAR			(10.0f)
#define VIEW_FAR			(1000.0f)
#define VALUE_MOVE_CAMERA	(2.0f)	
#define VALUE_ROTATE_CAMERA	(XM_PI*0.01f)	
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void SetCamera(void);					// カメラの設定処理

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static XMFLOAT3			g_posCameraP;
static XMFLOAT3			g_posCameraR;
static XMFLOAT3			g_vecCameraU;
static XMFLOAT3			g_rotCamera;
static float				g_fLengthInterval;
static XMFLOAT4X4			g_mtxProjection;
static XMFLOAT4X4			g_mtxView;

//=============================================================================
// カメラの初期化処理
//=============================================================================
HRESULT InitCamera(void)
{
	g_posCameraP = XMFLOAT3(POS_CAMERA_P_X, POS_CAMERA_P_Y, POS_CAMERA_P_Z);
	g_posCameraR = XMFLOAT3(POS_CAMERA_R_X, POS_CAMERA_R_Y, POS_CAMERA_R_Z);
	g_vecCameraU = XMFLOAT3(0,1,0);
	g_rotCamera = XMFLOAT3(0,0,0);

	float fVecX, fVecZ;
	fVecX = g_posCameraP.x - g_posCameraR.x;
	fVecZ = g_posCameraP.z - g_posCameraR.z;
	g_fLengthInterval = sqrtf(fVecX * fVecX + fVecZ * fVecZ);
	return S_OK;
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{
	// (なし)
}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	const XMFLOAT3 vEye = XMFLOAT3(0, 16, -442);//視点
	const XMFLOAT3 vLookAt = XMFLOAT3(0, 0, 158);//注視点

	XMMATRIX mtxWorld = XMLoadFloat4x4(GetModelWorld());

	//視点
	XMStoreFloat3(&g_posCameraP, XMVector3TransformCoord(XMLoadFloat3(&vEye), mtxWorld));

	//注視点
	XMStoreFloat3(&g_posCameraR, XMVector3TransformCoord(XMLoadFloat3(&vLookAt), mtxWorld));



	/*if (GetKeyPress(VK_A))
	{
		if (GetKeyPress(VK_W))
		{
			g_posCameraP.x -= cosf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
			g_posCameraP.z += sinf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
		}
		else if (GetKeyPress(VK_S))
		{
			g_posCameraP.x -= cosf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
			g_posCameraP.z += sinf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
		}
		else 
		{
			g_posCameraP.x -= cosf(g_rotCamera.y) * VALUE_MOVE_CAMERA;
			g_posCameraP.z += sinf(g_rotCamera.y) * VALUE_MOVE_CAMERA;
		}
		g_posCameraP.x = g_posCameraP.x + sinf(g_rotCamera.y) * g_fLengthInterval;
		g_posCameraP.z = g_posCameraP.z + cosf(g_rotCamera.y) * g_fLengthInterval;
	}*/
	g_rotCamera = GetRotation();
	SetCamera();
}

//=============================================================================
// カメラの設定処理
//=============================================================================
void SetCamera(void)
{
	XMStoreFloat4x4(&g_mtxView, XMMatrixLookAtLH(
	XMVectorSet(g_posCameraP.x, g_posCameraP.y, g_posCameraP.z, 1),
	XMVectorSet(g_posCameraR.x, g_posCameraR.y, g_posCameraR.z, 1),
	XMVectorSet(g_vecCameraU.x, g_vecCameraU.y, g_vecCameraU.z, 0)
	)
	);
	XMStoreFloat4x4(&g_mtxProjection, XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR, VIEW_FAR));
}

//=============================================================================
// カメラの位置の取得
//=============================================================================
XMFLOAT3& GetCameraPos(void)
{
	return g_posCameraP;
}

//=============================================================================
// カメラの向きの取得
//=============================================================================
XMFLOAT3& GetCameraAngle(void)
{
	return g_rotCamera;
}

//=============================================================================
// ビューマトリックスの取得
//=============================================================================
XMFLOAT4X4& GetViewMatrix(void)
{
	return g_mtxView;
}

//=============================================================================
// プロジェクションマトリックスの取得
//=============================================================================
XMFLOAT4X4& GetProjMatrix(void)
{
	return g_mtxProjection;
}

XMFLOAT3 GetForwardCameraVector()
{
	float pitch = g_rotCamera.y;
	float yaw = g_rotCamera.y;
	printf("PITCH: %f\n YAW: %f\n FORWARD: {%f, %f, %f}\n", pitch, yaw, cosf(pitch)*cosf(yaw), cosf(pitch)*sinf(yaw), sinf(pitch));
	return { cosf(pitch)*cosf(yaw),cosf(pitch)*sinf(yaw),sinf(pitch) };
}
