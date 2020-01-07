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

Light3D* MainLightSource = nullptr;
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

HRESULT Light3D::Init(void)
{
	XMFLOAT3 vecDir;
	//XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&LIGHT0_DIR)));
	//g_UnivLight.m_direction = LIGHT0_DIR;


	/*g_UnivLight.m_diffuse = LIGHT0_DIFFUSE;
	g_UnivLight.m_ambient = LIGHT0_AMBIENT;
	g_UnivLight.m_specular = LIGHT0_SPECULAR;
	vecDir = LIGHT0_DIR;
	XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));*/

	//XMFLOAT3 vecDir;

	// 拡散光
	g_UnivLight.m_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// 環境光
	g_UnivLight.m_ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// ライトの方向の設定
	vecDir = LIGHT0_DIR;
	XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
	return S_OK;
}

void Light3D::End(void)
{
}

void Light3D::Update(void)
{
	
}

CFbxLight* Light3D::GetLight(void)
{
	return &g_UnivLight;
}

void Light3D::SetDiffuse(XMFLOAT4 newDif)
{
	g_UnivLight.m_diffuse = newDif;
}

void Light3D::SetSpecular(XMFLOAT4 newSpec)
{
	g_UnivLight.m_specular = newSpec;
}

void Light3D::SetAmbient(XMFLOAT4 newAmb)
{
	g_UnivLight.m_ambient = newAmb;
}

void Light3D::SetDirection(XMFLOAT3 newDir)
{
	g_UnivLight.m_direction = newDir;
}

Light3D * GetMainLight()
{
	return MainLightSource;
}

//=============================================================================
// ライトの有効化/無効化
//=============================================================================
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