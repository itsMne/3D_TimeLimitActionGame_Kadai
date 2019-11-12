#include "Model3D.h"

#include "main.h"
#include "FbxModel.h"
#include "input.h"
#include "Camera3D.h"
#include "GameObject3D.h"
#include "UniversalStructures.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************



Model3D::Model3D()
{
	pMainCamera = nullptr;
	nFramCount = 0;
	nAnimationFrameSpeed = 5;
	GameObjectParent = nullptr;
}


Model3D::~Model3D()
{
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Model3D::InitModel(Light3D* SceneLight, const char* ModelPath)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	pMainCamera = GetMainCamera();
	// 位置、向きの初期設定
	g_posModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_rotModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float ModelScale = 1.0f;
	g_ScaleModel = XMFLOAT3(ModelScale, ModelScale, ModelScale);
	// FBXファイルの読み込み
	g_pModel = new CFbxModel();
	Light3D* pLight = SceneLight;
	nFrame = g_pModel->GetInitialAnimFrame();
	if (!pMainCamera)
	{
		printf("メインカメラがありません\n");
		return S_OK;
	}
	hr = g_pModel->Init(pDevice, pDeviceContext, ModelPath);
	if (SUCCEEDED(hr)) {
		g_pModel->SetCamera(pMainCamera->GetCameraPos());
		if (pLight)
			g_pModel->SetLight(pLight->GetLight());
	}
	return hr;
}


//=============================================================================
// 終了処理
//=============================================================================
void Model3D::UninitModel(void)
{
	// FBXファイルの解放
	SAFE_DELETE(g_pModel);
}

//=============================================================================
// 更新処理
//=============================================================================
void Model3D::UpdateModel(void)
{
	
}

void Model3D::SwitchAnimation(int nNewAnimNum, int FrameSlowness)
{
	if (g_pModel->GetCurrentAnimation() == nNewAnimNum)
		return;
	nAnimationFrameSpeed = FrameSlowness;
	g_pModel->SetAnimStack(nNewAnimNum);
	nFrame = g_pModel->GetInitialAnimFrame();
	nFramCount = 0;
}

//=============================================================================
// 描画処理
//=============================================================================
void Model3D::DrawModel(void)
{
	if (!pMainCamera)
	{
		printf("メインカメラがありません\n");
		return;
	}
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	GameObject3D* Parent = (GameObject3D*)GameObjectParent;
	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//サイズ
	if (!Parent) {
		mtxScale = XMMatrixScaling(g_ScaleModel.x, g_ScaleModel.y, g_ScaleModel.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
	}
	else {
		XMFLOAT3 ParentScale = Parent->GetScale();
		mtxScale = XMMatrixScaling(g_ScaleModel.x * ParentScale.x, g_ScaleModel.y * ParentScale.y, g_ScaleModel.z * ParentScale.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
	}
	// 回転を反映
	if (!Parent) {
		mtxRot = XMMatrixRotationRollPitchYaw(g_rotModel.x, g_rotModel.y, g_rotModel.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	}
	else {
		XMFLOAT3 ParentRotation = Parent->GetRotation();
		mtxRot = XMMatrixRotationRollPitchYaw(ParentRotation.x+g_rotModel.x, ParentRotation.y + g_rotModel.y, ParentRotation.z + g_rotModel.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
	}

	if (!Parent) {
		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_posModel.x, g_posModel.y, g_posModel.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	}
	else {
		XMFLOAT3 ParentLocation = Parent->GetLocation();
		mtxTranslate = XMMatrixTranslation(g_posModel.x + ParentLocation.x, g_posModel.y + ParentLocation.y, g_posModel.z + ParentLocation.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	}


	// ワールドマトリックスの設定
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);


	AnimationControl();
	SetZWrite(true);
	g_pModel->Render(g_mtxWorld, pMainCamera->GetViewMatrix(), pMainCamera->GetProjMatrix(), eOpacityOnly);
	SetZWrite(false);
	g_pModel->Render(g_mtxWorld, pMainCamera->GetViewMatrix(), pMainCamera->GetProjMatrix(), eTransparentOnly);
}

void Model3D::AnimationControl()
{
	if (++nFramCount >= nAnimationFrameSpeed) {
		nFramCount = 0;
		if (++nFrame >= g_pModel->GetMaxAnimFrame()) nFrame = g_pModel->GetInitialAnimFrame();
		g_pModel->SetAnimFrame(nFrame);
	}
}

XMFLOAT4X4 * Model3D::GetModelWorld()
{
	return &g_mtxWorld;
}

XMFLOAT3 Model3D::GetRotation()
{
	return g_rotModel;
}

XMFLOAT3 Model3D::GetPosition()
{
	return g_posModel;
}

void Model3D::SetScale(float newScale)
{
	g_ScaleModel = XMFLOAT3(newScale, newScale, newScale);
}

int Model3D::GetNumberOfAnimations()
{
	return g_pModel->GetMaxNumberOfAnimations();;
}

void Model3D::SetParent(void * newParent)
{
	GameObjectParent = newParent;
}

void Model3D::RotateAroundX(float x)
{
	g_rotModel.x -= x;
	if (g_rotModel.x < -XM_PI) {
		g_rotModel.x += XM_2PI;
	}
}

void Model3D::RotateAroundY(float y)
{
	g_rotModel.y -= y;
	if (g_rotModel.y < -XM_PI) {
		g_rotModel.y += XM_2PI;
	}
	//printf("%f\n", g_rotModel.y);
}

void Model3D::TranslateModel(XMFLOAT3 translation)
{
	g_posModel.x += translation.x;
	g_posModel.y += translation.y;
	g_posModel.y += translation.y;
}

void Model3D::SetRotation(XMFLOAT3 rot)
{
	g_rotModel=rot;
}
