//=============================================================================
//
// ライト処理 [light.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "light.h"
CFbxLight g_UnivLight;
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define LIGHT0_DIFFUSE XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_AMBIENT XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_SPECULAR XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_DIR XMFLOAT3(-1.0f, -1.0f, 1.0f)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************

//=============================================================================
// ライトの初期化処理
//=============================================================================
HRESULT InitLight(void)
{
	XMFLOAT3 vecDir;
	//XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&LIGHT0_DIR)));
	//g_UnivLight.m_direction = LIGHT0_DIR;
	g_UnivLight.m_diffuse = LIGHT0_DIFFUSE;
	g_UnivLight.m_ambient = LIGHT0_AMBIENT;
	//g_UnivLight.m_specular = LIGHT0_SPECULAR;
	vecDir = XMFLOAT3(0.0f, -1.0f, 1.0f);
	XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
	return S_OK;
}

//=============================================================================
// ライトの終了処理
//=============================================================================
void UninitLight(void)
{
}

//=============================================================================
// ライトの更新処理
//=============================================================================
void UpdateLight(void)
{
}

//=============================================================================
// ライトの取得
//=============================================================================
CFbxLight& GetLight(void)
{
	return g_UnivLight;
}
