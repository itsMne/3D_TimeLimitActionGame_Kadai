#include "S_InGame3D.h"
#include "debugproc.h"
#include "Field3D.h"
#include "Cube3D.h"
#include "Wall3D.h"
#include "Enemy3D.h"
#include "Billboard2D.h"


SceneInGame3D* pCurrentGame = nullptr;

SceneInGame3D::SceneInGame3D() :Scene3D(true)
{
	pCurrentGame = this;
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	pPlayer = new Player3D();
	
	pSkybox = new Sphere3D("data/texture/Skybox.tga");
	HRESULT	hr;
	g_pDevice = GetDevice();

	Floors = new Go_List();
	Walls = new Go_List();
	Enemies = new Go_List();
	Floors->Load("Level_Floors", GO_FLOOR);
	Walls->Load("Level_Walls", GO_WALL);
	Enemies->Load("Level_Enemies", GO_ENEMY);
	hr = InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer);
	pUI = new InGameUI2D();

	//Enemies->AddEnemy({ 0,0,0 });
}


SceneInGame3D::~SceneInGame3D()
{
	End();
	pCurrentGame = nullptr;
}

void SceneInGame3D::Init()
{
}

eSceneType SceneInGame3D::Update()
{
	Scene3D::Update();
	// デバッグ文字列表示更新
	UpdateDebugProc();

	// デバッグ文字列設定
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", GetMainWindowFPS());

	// モデル更新
	pPlayer->Update();

	// フィールド更新
	Floors->Update();

	//HelloShadow->Update();

	pUI->Update();

	pSkybox->Update();
	
	Walls->Update();

	Enemies->Update();
	return SCENE_IN_GAME;
}

void SceneInGame3D::Draw()
{
	// Zバッファ有効
	SetZBuffer(true);

	// 前面カリング (FBXは表裏が反転するため)
	//if(!pDeviceContext)
	//	pDeviceContext = MainWindow->GetDeviceContext();
	//pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));

	SetCullMode(CULLMODE_NONE);
	// モデル描画
	
	pPlayer->Draw();
	Enemies->Draw();
	SetCullMode(CULLMODE_CCW);
	
	// 背面カリング (通常は表面のみ描画)
	
	SetCullMode(CULLMODE_NONE);
	Walls->Draw();
	SetCullMode(CULLMODE_CCW);

	pSkybox->Draw();
	// フィールド描画
	SetCullMode(CULLMODE_NONE);
	Floors->Draw();
	SetCullMode(CULLMODE_CCW);
	// Zバッファ無効
	SetZBuffer(false);

	// デバッグ文字列表示
	DrawDebugProc();
	
	
	pUI->Draw();
}

void SceneInGame3D::End()
{
	Scene3D::End();
	// フィールド終了処理
	SAFE_DELETE(Floors);
	SAFE_DELETE(Walls);
	// モデル表示終了処理
	SAFE_DELETE(pPlayer);
	// デバッグ文字列表示終了処理
	UninitDebugProc();

	SAFE_DELETE(pUI);

	//SAFE_DELETE(HelloBill);
}

Go_List * SceneInGame3D::GetList(int Type)
{
	switch (Type)
	{
	case GO_FLOOR:
		return Floors;
	case GO_WALL:
		return Walls;
	case GO_ENEMY:
		return Enemies;
	default:
		break;
	}
	return nullptr;
}

void SceneInGame3D::SetAtkEffect(int frames)
{
	pUI->ActivateAtkEffect(frames);
}

InGameUI2D * SceneInGame3D::GetUIManager()
{
	return pUI;
}

SceneInGame3D * GetCurrentGame()
{
	return pCurrentGame;
}
