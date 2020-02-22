//*****************************************************************************
// Light3D.cpp
// 光の管理
// Author: Mane
//*****************************************************************************
#include "Light3D.h"
#include "InputManager.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define LIGHT0_DIFFUSE XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_DIFFUSE XMFLOAT4(1.0f, 0, 0, 1.0f)
#define LIGHT0_AMBIENT XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_AMBIENT XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
#define LIGHT0_SPECULAR XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_SPECULAR XMFLOAT4(1.0f, .0.0f, 0.0f, 1.0f)
#define LIGHT0_DIR XMFLOAT3(0.0f, -1.0f, 1.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Light3D* MainLightSource = nullptr;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Light3D::Light3D()
{
	Init();
}

Light3D::Light3D(bool bMainLight)
{
	if (bMainLight)
		MainLightSource = this;
	Init();
}


Light3D::~Light3D()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
HRESULT Light3D::Init(void)
{
	XMFLOAT3 vecDir;

	// 拡散光
	g_UnivLight.m_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// 環境光
	g_UnivLight.m_ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// ライトの方向の設定
	vecDir = LIGHT0_DIR;
	XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
	return S_OK;
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Light3D::End(void)
{
	//無し
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Light3D::Update(void)
{
	//無し
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
CFbxLight* Light3D::GetLight(void)
{
	return &g_UnivLight;
}

//*****************************************************************************
//SetDiffuse関数
//拡散光の設定
//引数：XMFLOAT4
//戻：void
//*****************************************************************************
void Light3D::SetDiffuse(XMFLOAT4 newDif)
{
	g_UnivLight.m_diffuse = newDif;
}

//*****************************************************************************
//SetAmbient関数
//スペキュラーの設定
//引数：XMFLOAT4
//戻：void
//*****************************************************************************
void Light3D::SetSpecular(XMFLOAT4 newSpec)
{
	g_UnivLight.m_specular = newSpec;
}

//*****************************************************************************
//SetAmbient関数
//アンビエントライトの設定
//引数：XMFLOAT4
//戻：void
//*****************************************************************************
void Light3D::SetAmbient(XMFLOAT4 newAmb)
{
	g_UnivLight.m_ambient = newAmb;
}

//*****************************************************************************
//SetDirection関数
//方向を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Light3D::SetDirection(XMFLOAT3 newDir)
{
	g_UnivLight.m_direction = newDir;
}

//*****************************************************************************
//GetMainLight関数
//ライトのアドレスを戻す
//引数：void
//戻：Light3D*
//*****************************************************************************
Light3D * GetMainLight()
{
	return MainLightSource;
}

//*****************************************************************************
//SetLightEnable関数
//光を有効する
//引数：bool
//戻：void
//*****************************************************************************
void  Light3D::SetLightEnable(bool bEnable)
{
	if (bEnable) {
		XMFLOAT3 vecDir = LIGHT0_DIR;
		XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
	}
	else {
		g_UnivLight.m_direction = XMFLOAT3(0, 0, 0);
	}
}