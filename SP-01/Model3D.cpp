//*****************************************************************************
// Model3D.cpp
// 3Dモデルの管理
// Author : Mane
//*****************************************************************************
#include "Model3D.h"
#include "main.h"
#include "FbxModel.h"
#include "input.h"
#include "Camera3D.h"
#include "GameObject3D.h"
#include "S_InGame3D.h"
#include "UniversalStructures.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DEBUG_SHOW_INITED_MODEL_PATH false
#define MAX_LOOPS	9000

//*****************************************************************************
// グローバル変数
//*****************************************************************************
char ModelPaths[MAX_PRELOADED_MODELS][256] = 
{
	"data/model/Enemy.fbx",
	"data/model/Cube2.fbx",
	"data/model/SkullV2.fbx",
	"data/model/EnemyHeart.fbx"
};
CFbxModel* Models[MAX_PRELOADED_MODELS] = { nullptr };

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
Model3D::Model3D(void* pParent, const char*ModelPath)
{
	pMainCamera = nullptr;
	nFramCount = 0;
	nCurrentAnimation = 0;
	nInitialFrame = 0;
	nFinalFrame = 0;
	nAnimationFrameSlowness = 0;
	fAnimationSkipFrame = 1;
	GameObjectParent = nullptr;
	GameObjectParent = pParent;
	InitModel(ModelPath);
	bLoop = true;
}

Model3D::Model3D(void * Parent, int ModelPath)
{
	pMainCamera = nullptr;
	nFramCount = 0;
	nCurrentAnimation = 0;
	nInitialFrame = 0;
	nFinalFrame = 0;
	nAnimationFrameSlowness = 0;
	fAnimationSkipFrame = 1;
	GameObjectParent = nullptr;
	GameObjectParent = Parent;
	InitModel(ModelPath);
	bLoop = true;
}


Model3D::~Model3D()
{
	UninitModel();
}

//*****************************************************************************
//InitModel関数
//初期化関数
//引数：const char*
//戻：void
//*****************************************************************************
HRESULT Model3D::InitModel(const char* ModelPath)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	pMainCamera = GetMainCamera();
	// 位置、向きの初期設定
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float ModelScale = 1.0f;
	Scale = XMFLOAT3(ModelScale, ModelScale, ModelScale);
	// FBXファイルの読み込み
	g_pModel = new CFbxModel();
	pLight = GetMainLight();
	fFrame = g_pModel->GetInitialAnimFrame();
	nFramCount = nCountLoop = 0;
	if (!pMainCamera)
	{
		printf("メインカメラがありません\n");
		return S_OK;
	}
	hr = g_pModel->Init(pDevice, pDeviceContext, ModelPath);
#if DEBUG_SHOW_INITED_MODEL_PATH
	printf("%s\n", ModelPath);	
#endif
	if (SUCCEEDED(hr)) {
		g_pModel->SetCamera(pMainCamera->GetCameraPos());
		if (pLight)
			g_pModel->SetLight(pLight->GetLight());
	}
	bPreLoadedModel = false;
	return hr;
}


//*****************************************************************************
//InitModel関数
//初期化関数
//引数：int
//戻：void
//*****************************************************************************
HRESULT Model3D::InitModel(int ModelPath)
{
	if (ModelPath > MAX_PRELOADED_MODELS || ModelPath < 0)
		return S_OK;
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	pMainCamera = GetMainCamera();
	if (!pMainCamera)
	{
		printf("メインカメラがありません\n");
		return S_OK;
	}
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float ModelScale = 1.0f;
	Scale = XMFLOAT3(ModelScale, ModelScale, ModelScale);
	// FBXファイルの読み込み
	if (!Models[ModelPath]) {
		Models[ModelPath] = new CFbxModel();
		Light3D* pLight = GetMainLight();
		fFrame = Models[ModelPath]->GetInitialAnimFrame();
		Models[ModelPath]->SetCamera(pMainCamera->GetCameraPos());
		Models[ModelPath]->Init(pDevice, pDeviceContext, ModelPaths[ModelPath]);
		g_pModel = Models[ModelPath];
		SetLight(pLight);
	}
	else {
		g_pModel = Models[ModelPath];
	}
	bPreLoadedModel = true;
	return S_OK;
}

//*****************************************************************************
//UninitModel関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Model3D::UninitModel(void)
{
	// FBXファイルの解放
	if (!bPreLoadedModel)
		SAFE_DELETE(g_pModel);
}

//*****************************************************************************
//UpdateModel関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Model3D::UpdateModel(void)
{
	//なし
}

//*****************************************************************************
//SwitchAnimation関数
//アニメーションを変える
//引数：int, int, float
//戻：void
//*****************************************************************************
void Model3D::SwitchAnimation(int nNewAnimNum, int FrameSlowness, float AnimationFrameSkip)
{
	if (nCurrentAnimation == nNewAnimNum) {
		if(nAnimationFrameSlowness != FrameSlowness)
			nAnimationFrameSlowness = FrameSlowness;
		if (fAnimationSkipFrame != AnimationFrameSkip)
			fAnimationSkipFrame = AnimationFrameSkip;
		return;
	}
	nCountLoop = 0;
	nAnimationFrameSlowness = FrameSlowness;
	nCurrentAnimation = nNewAnimNum;
	g_pModel->SetAnimStack(nNewAnimNum);
	nInitialFrame = fFrame = g_pModel->GetInitialAnimFrame();
	nFinalFrame = g_pModel->GetMaxAnimFrame();
	fAnimationSkipFrame = AnimationFrameSkip;
	nFramCount = 0;
}

//*****************************************************************************
//DrawModel関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Model3D::DrawModel(void)
{
	if (!pMainCamera)
	{
		printf("メインカメラがありません\n");
		return;
	}

	GameObject3D* goParent = (GameObject3D*)GameObjectParent;//親のポインターを使う
	XMFLOAT3 ParentPos = { 0,0,0 };
	XMFLOAT3 ParentScale = { 1,1,1 };//親の拠点と親の大きさ
	if (goParent) {
		ParentPos = goParent->GetPosition();
		ParentScale = goParent->GetScale();
	}
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//サイズ
	mtxScale = XMMatrixScaling(Scale.x * ParentScale.x, Scale.y * ParentScale.y, Scale.z * ParentScale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// 移動を反映
	mtxTranslate = XMMatrixTranslation(Position.x + ParentPos.x, Position.y + ParentPos.y, Position.z + ParentPos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);



	// ワールドマトリックスの設定
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);

	g_pModel->SetAnimStack(nCurrentAnimation);
	AnimationControl();
	SetZWrite(true);
	g_pModel->Render(g_mtxWorld, pMainCamera->GetViewMatrix(), pMainCamera->GetProjMatrix(), eOpacityOnly);
	SetZWrite(false);
	g_pModel->Render(g_mtxWorld, pMainCamera->GetViewMatrix(), pMainCamera->GetProjMatrix(), eTransparentOnly);
}

//*****************************************************************************
//AnimationControl関数
//アニメーションを管理する
//引数：void
//戻：void
//*****************************************************************************
void Model3D::AnimationControl()
{
	if (IsGamePaused())
	{
		g_pModel->SetAnimFrame((int)fFrame);
		return;
	}
	if (++nFramCount >= nAnimationFrameSlowness) {
		nFramCount = 0;
		fFrame += fAnimationSkipFrame;
		if (fFrame >= nFinalFrame) {
			if (++nCountLoop > MAX_LOOPS)
				nCountLoop = MAX_LOOPS;
			if(bLoop)
				fFrame = nInitialFrame;
		}
		g_pModel->SetAnimFrame((int)fFrame);
	}
}

//*****************************************************************************
//GetModelWorld関数
//ワールドマトリックスのアドレスを戻す
//引数：void
//戻：XMFLOAT4X4*
//*****************************************************************************
XMFLOAT4X4 * Model3D::GetModelWorld()
{
	return &g_mtxWorld;
}

//*****************************************************************************
//GetRotation関数
//回転を戻すする
//引数：void
//戻：XMFLOAT4X4*
//*****************************************************************************
XMFLOAT3 Model3D::GetRotation()
{
	return Rotation;
}

//*****************************************************************************
//GetPosition関数
//拠点を戻すする
//引数：void
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 Model3D::GetPosition()
{
	return Position;
}

//*****************************************************************************
//SetScale関数
//大きさのアドレスを戻す
//引数：void
//戻：float
//*****************************************************************************
XMFLOAT3 * Model3D::GetScaleAdd()
{
	return &Scale;
}

//*****************************************************************************
//SetScale関数
//大きさを設定する
//引数：float
//戻：void
//*****************************************************************************
void Model3D::SetScale(float newScale)
{
	if (newScale < 0)
	{
		Scale = XMFLOAT3(0, 0, 0);
		return;
	}
	Scale = XMFLOAT3(newScale, newScale, newScale);
}

//*****************************************************************************
//SetScale関数
//大きさを設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Model3D::SetScale(XMFLOAT3 newScale)
{
	Scale = newScale;
}

//*****************************************************************************
//GetNumberOfAnimations関数
//アニメーションの数を戻す
//引数：void
//戻：int
//*****************************************************************************
int Model3D::GetNumberOfAnimations()
{
	return g_pModel->GetMaxNumberOfAnimations();
}

//*****************************************************************************
//SetParent関数
//親を設定する
//引数：void*
//戻：void
//*****************************************************************************
void Model3D::SetParent(void * newParent)
{
	GameObjectParent = newParent;
}

//*****************************************************************************
//RotateAroundX関数
//X座標に回転する
//引数：float
//戻：void
//*****************************************************************************
void Model3D::RotateAroundX(float x)
{
	Rotation.x -= x;
	if (Rotation.x < -XM_PI) {
		Rotation.x += XM_2PI;
	}
}

//*****************************************************************************
//RotateAroundX関数
//Y座標に回転する
//引数：float
//戻：void
//*****************************************************************************
void Model3D::RotateAroundY(float y)
{
	Rotation.y -= y;
	if (Rotation.y < -XM_PI) {
		Rotation.y = XM_2PI;
	}
}

//*****************************************************************************
//TranslateModel関数
//モデルに動かせる
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Model3D::TranslateModel(XMFLOAT3 translation)
{
	Position.x += translation.x;
	Position.y += translation.y;
	Position.y += translation.y;
}

//*****************************************************************************
//SetRotation関数
//回転を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Model3D::SetRotation(XMFLOAT3 rot)
{
	Rotation=rot;
}

//*****************************************************************************
//SetPosition関数
//拠点を設定する
//引数：XMFLOAT3
//戻：void
//*****************************************************************************
void Model3D::SetPosition(XMFLOAT3 pos)
{
	Position = pos;
}

//*****************************************************************************
//SetRotationX関数
//回転を設定する(X座標)
//引数：float
//戻：void
//*****************************************************************************
void Model3D::SetRotationX(float rotx)
{
	Rotation.x = rotx;
}

//*****************************************************************************
//SetRotationY関数
//回転を設定する(Y座標)
//引数：float
//戻：void
//*****************************************************************************
void Model3D::SetRotationY(float roty)
{
	Rotation.y = roty;
}

//*****************************************************************************
//SetLight関数
//光を設定する
//引数：Light3D*
//戻：void
//*****************************************************************************
void Model3D::SetLight(Light3D * newLight)
{
	if(g_pModel && newLight)
		g_pModel->SetLight(newLight->GetLight());
}

//*****************************************************************************
//GetCurrentAnimation関数
//現在のアニメーションを戻す
//引数：void
//戻：int
//*****************************************************************************
int Model3D::GetCurrentAnimation()
{
	return nCurrentAnimation;
}

//*****************************************************************************
//GetCurrentFrameOfAnimation関数
//現在のアニメーションのフレームを戻す
//引数：void
//戻：int
//*****************************************************************************
int Model3D::GetCurrentFrameOfAnimation()
{
	return (int)fFrame;
}

//*****************************************************************************
//GetEndFrameOfCurrentAnimation関数
//現在のアニメーションの最後のフレームを戻す
//引数：void
//戻：int
//*****************************************************************************
int Model3D::GetEndFrameOfCurrentAnimation()
{
	return nFinalFrame;
}

//*****************************************************************************
//GetCurrentFrame関数
//現在のアニメーションのフレームを戻す
//引数：void
//戻：int
//*****************************************************************************
int Model3D::GetCurrentFrame()
{
	if (g_pModel)
		return fFrame;
	return 0;
}

//*****************************************************************************
//GetLoops関数
//アニメーションのループを戻す
//引数：void
//戻：int
//*****************************************************************************
int Model3D::GetLoops()
{
	return nCountLoop;
}

//*****************************************************************************
//SetFrameOfAnimation関数
//アニメーションのフレームを設定する
//引数：float
//戻：void
//*****************************************************************************
void Model3D::SetFrameOfAnimation(float Frame)
{
	fFrame=Frame;
}

//*****************************************************************************
//SetLoop関数
//ループが出来ることを設定する
//引数：bool
//戻：void
//*****************************************************************************
void Model3D::SetLoop(bool loopable)
{
	bLoop = loopable;
}

//*****************************************************************************
//UninitPreloadedModels関数
//モデルを終了する
//引数：void
//戻：void
//*****************************************************************************
void UninitPreloadedModels()
{
	for (int i = 0; i < MAX_PRELOADED_MODELS; i++)
		SAFE_DELETE(Models[i]);
	
}
