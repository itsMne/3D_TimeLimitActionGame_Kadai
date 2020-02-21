#include "Model3D.h"
#include "main.h"
#include "FbxModel.h"
#include "input.h"
#include "Camera3D.h"
#include "GameObject3D.h"
#include "S_InGame3D.h"
#include "UniversalStructures.h"
#include "debugproc.h"
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

//=============================================================================
// 初期化処理
//=============================================================================
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
//=============================================================================
// 終了処理
//=============================================================================
void Model3D::UninitModel(void)
{
	// FBXファイルの解放
	if (!bPreLoadedModel)
		SAFE_DELETE(g_pModel);
}

//=============================================================================
// 更新処理
//=============================================================================
void Model3D::UpdateModel(void)
{
	
}

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

XMFLOAT4X4 * Model3D::GetModelWorld()
{
	return &g_mtxWorld;
}

XMFLOAT3 Model3D::GetRotation()
{
	return Rotation;
}

XMFLOAT3 Model3D::GetPosition()
{
	return Position;
}

XMFLOAT3 * Model3D::GetScaleAdd()
{
	return &Scale;
}

void Model3D::SetScale(float newScale)
{
	if (newScale < 0)
	{
		Scale = XMFLOAT3(0, 0, 0);
		return;
	}
	Scale = XMFLOAT3(newScale, newScale, newScale);
}
void Model3D::SetScale(XMFLOAT3 newScale)
{
	Scale = newScale;
}

int Model3D::GetNumberOfAnimations()
{
	return g_pModel->GetMaxNumberOfAnimations();
}

void Model3D::SetParent(void * newParent)
{
	GameObjectParent = newParent;
}

void Model3D::RotateAroundX(float x)
{
	Rotation.x -= x;
	if (Rotation.x < -XM_PI) {
		Rotation.x += XM_2PI;
	}
}

void Model3D::RotateAroundY(float y)
{
	Rotation.y -= y;
	if (Rotation.y < -XM_PI) {
		Rotation.y = XM_2PI;
	}
	//printf("%f\n", g_rotModel.y);
}

void Model3D::TranslateModel(XMFLOAT3 translation)
{
	Position.x += translation.x;
	Position.y += translation.y;
	Position.y += translation.y;
}

void Model3D::SetRotation(XMFLOAT3 rot)
{
	Rotation=rot;
}

void Model3D::SetPosition(XMFLOAT3 pos)
{
	Position = pos;
}

void Model3D::SetRotationX(float rotx)
{
	Rotation.x = rotx;
}

void Model3D::SetRotationY(float roty)
{
	Rotation.y = roty;
}

void Model3D::SetLight(Light3D * newLight)
{
	if(g_pModel && newLight)
		g_pModel->SetLight(newLight->GetLight());
}

int Model3D::GetCurrentAnimation()
{
	return nCurrentAnimation;
}

int Model3D::GetCurrentFrameOfAnimation()
{
	return (int)fFrame;
}

int Model3D::GetEndFrameOfCurrentAnimation()
{
	return nFinalFrame;
}

int Model3D::GetCurrentFrame()
{
	if (g_pModel)
		return fFrame;
	return 0;
}

int Model3D::GetLoops()
{
	return nCountLoop;
}

void Model3D::SetFrameOfAnimation(float Frame)
{
	fFrame=Frame;
}

void Model3D::SetLoop(bool loopable)
{
	bLoop = loopable;
}

void UninitPreloadedModels()
{
	for (int i = 0; i < MAX_PRELOADED_MODELS; i++)
		SAFE_DELETE(Models[i]);
	
}
