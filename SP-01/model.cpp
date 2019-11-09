//=============================================================================
//
// モデル処理 [model.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "model.h"
#include "main.h"
#include "FbxModel.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "UniversalStructures.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODEL_CAR			"data/model/car000.fbx"	// 読み込むモデルファイル名
#define MODEL_CAR			"data/model/PrototypeSamurai.fbx"	// 読み込むモデルファイル名
#define VALUE_MOVE_MODEL	(5.0f)					// 移動速度
#define VALUE_ROTATE_MODEL	(XM_PI*0.02f)			// 回転速度

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CFbxModel*	g_pModel;		// FBXモデルへのポインタ

static XMFLOAT3		g_posModel;		// 現在の位置
static XMFLOAT3		g_rotModel;		// 現在の向き
static XMFLOAT3		g_ScaleModel;		// 現在の向き
float ScaleTest;
static XMFLOAT4X4	g_mtxWorld;		// ワールドマトリックス
bool bIsMoving = false;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitModel(void)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// 位置、向きの初期設定
	g_posModel = XMFLOAT3(0.0f, 10.0f, 0.0f);
	g_rotModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	ScaleTest = 0.1f;
	g_ScaleModel = XMFLOAT3(ScaleTest, ScaleTest, ScaleTest);
	// FBXファイルの読み込み
	g_pModel = new CFbxModel();
	hr = g_pModel->Init(pDevice, pDeviceContext, MODEL_CAR);
	if (SUCCEEDED(hr)) {
		g_pModel->SetCamera(GetCameraPos());
		g_pModel->SetLight(GetLight());
	}
	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitModel(void)
{
	// FBXファイルの解放
	SAFE_DELETE(g_pModel);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateModel(void)
{
	XMFLOAT3 rotCamera;

	// カメラの向き取得
	rotCamera = GetCameraAngle();
	g_pModel->SetAnimStack(2);
	if (GetKeyPress(VK_LEFT)) {
		if (GetKeyPress(VK_UP)) {
			// 左前移動
			g_posModel.x -= sinf(rotCamera.y + XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y + XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else if (GetKeyPress(VK_DOWN)) {
			// 左後移動
			g_posModel.x -= sinf(rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else {
			// 左移動
			g_posModel.x -= sinf(rotCamera.y + XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y + XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
	}
	else if (GetKeyPress(VK_RIGHT)) {
		if (GetKeyPress(VK_UP)) {
			// 右前移動
			g_posModel.x -= sinf(rotCamera.y - XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y - XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else if (GetKeyPress(VK_DOWN)) {
			// 右後移動
			g_posModel.x -= sinf(rotCamera.y - XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y - XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else {
			// 右移動
			g_posModel.x -= sinf(rotCamera.y - XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y - XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
	}
	else if (GetKeyPress(VK_UP)) {
		// 前移動
		g_posModel.x -= sinf(XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;
		g_posModel.z -= cosf(XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;
		g_pModel->SetAnimStack(1);
		//g_posModel.x += GetForwardVector(g_rotModel).x*VALUE_MOVE_MODEL;
		//g_posModel.y += GetForwardVector(g_rotModel).y*VALUE_MOVE_MODEL;
		//g_posModel.z += GetForwardVector(g_rotModel).z*VALUE_MOVE_MODEL;
	}
	else if (GetKeyPress(VK_DOWN)) {
		// 後移動
		g_posModel.x -= sinf(rotCamera.y) * VALUE_MOVE_MODEL;
		g_posModel.z -= cosf(rotCamera.y) * VALUE_MOVE_MODEL;
		g_pModel->SetAnimStack(1);
	}

	if (GetKeyPress(VK_LSHIFT)) {
		// 左回転
		g_rotModel.y -= VALUE_ROTATE_MODEL;
		if (g_rotModel.y < -XM_PI) {
			g_rotModel.y += XM_2PI;
			g_pModel->SetAnimStack(1);
		}
	}
	if (GetKeyPress(VK_RSHIFT)) {
		// 右回転
		g_rotModel.y += VALUE_ROTATE_MODEL;
		if (g_rotModel.y > XM_PI) {
			g_rotModel.y -= XM_2PI;
			g_pModel->SetAnimStack(1);
		}
	}

	if (GetKeyPress(VK_RETURN)) {
		// リセット
		g_posModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_rotModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	PrintDebugProc("[ｸﾙﾏ ｲﾁ : (%f, %f, %f)]\n", g_posModel.x, g_posModel.y, g_posModel.z);
	PrintDebugProc("[ｸﾙﾏ ﾑｷ : (%f)]\n", XMConvertToDegrees(g_rotModel.y));
	PrintDebugProc("\n");

	PrintDebugProc("*** ｸﾙﾏ ｿｳｻ ***\n");
	PrintDebugProc("ﾏｴ   ｲﾄﾞｳ : \x1e\n");//↑
	PrintDebugProc("ｳｼﾛ  ｲﾄﾞｳ : \x1f\n");//↓
	PrintDebugProc("ﾋﾀﾞﾘ ｲﾄﾞｳ : \x1d\n");//←
	PrintDebugProc("ﾐｷﾞ  ｲﾄﾞｳ : \x1c\n");//→
	PrintDebugProc("ﾋﾀﾞﾘ ｾﾝｶｲ : LSHIFT\n");
	PrintDebugProc("ﾐｷﾞ  ｾﾝｶｲ : RSHIFT\n");
	PrintDebugProc("\n");
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawModel(void)
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//サイズ
	mtxScale = XMMatrixScaling(g_ScaleModel.x, g_ScaleModel.y, g_ScaleModel.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotModel.x, g_rotModel.y, g_rotModel.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_posModel.x, g_posModel.y, g_posModel.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);



	// ワールドマトリックスの設定
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);


	// FBXファイル表示
	static int nFrame = g_pModel->GetInitialAnimFrame();

	//g_pModel->SetAnimStack(0);
	//printf("FRAMES: %d\n", nFrame);
	if (++nFrame >= g_pModel->GetMaxAnimFrame()-5) nFrame = g_pModel->GetInitialAnimFrame();
	g_pModel->SetAnimFrame(nFrame);
	SetZWrite(true);
	g_pModel->Render(g_mtxWorld, GetViewMatrix(), GetProjMatrix(), eOpacityOnly);
	SetZWrite(false);
	g_pModel->Render(g_mtxWorld, GetViewMatrix(), GetProjMatrix(), eTransparentOnly);
}

XMFLOAT4X4 * GetModelWorld()
{
	return &g_mtxWorld;
}

XMFLOAT3 GetRotation()
{
	return g_rotModel;
}
