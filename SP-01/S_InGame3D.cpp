#include "S_InGame3D.h"
#include "debugproc.h"
#include "Field3D.h"
#include "InputManager.h"
#include "RankManager.h"
#include "Billboard2D.h"


S_InGame3D* pCurrentGame = nullptr;
int nScore;
S_InGame3D::S_InGame3D() :Scene3D(true)
{
	pCurrentGame = this;
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	pPlayer = new Player3D();
	pPlayer->SetPosition({ 2.578626f, 119.499969f, -138.668900f });
	
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

	nScore = 0;
	RankManager::Init();
	//Enemies->AddEnemy({ 0,0,0 });
}


S_InGame3D::~S_InGame3D()
{
	printf("ENDING INGAME SCENE\n");
	UninitPreloadedModels();
	End();
	pCurrentGame = nullptr;
}

void S_InGame3D::Init()
{
}

eSceneType S_InGame3D::Update()
{
	if (pUI->GetGameOverFrames() > 120)
	{
		if (GetInput(INPUT_JUMP))
			return SCENE_TITLE_SCREEN;
		pUI->Update();
		return SCENE_IN_GAME;
	}
	Scene3D::Update();
	// デバッグ文字列表示更新
	UpdateDebugProc();

	// デバッグ文字列設定
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", GetMainWindowFPS());

	RankManager::Update();
	// モデル更新
	pPlayer->Update();

	// フィールド更新
	Floors->Update();

	pUI->Update();

	pSkybox->Update();
	
	Walls->Update();

	Enemies->Update();
	if (pPlayer->IsPlayerDead())
		pUI->ActivateGameOver();
	return SCENE_IN_GAME;
}

void S_InGame3D::Draw()
{
	// Zバッファ有効
	SetZBuffer(true);

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

void S_InGame3D::End()
{
	Scene3D::End();
	// フィールド終了処理
	SAFE_DELETE(Floors);
	SAFE_DELETE(Enemies);
	SAFE_DELETE(Walls);
	// モデル表示終了処理
	SAFE_DELETE(pPlayer);
	// デバッグ文字列表示終了処理
	UninitDebugProc();

	SAFE_DELETE(pUI);

	//SAFE_DELETE(HelloBill);
}

Go_List * S_InGame3D::GetList(int Type)
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

void S_InGame3D::SetAtkEffect(int frames)
{
	pUI->ActivateAtkEffect(frames);
}

InGameUI2D * S_InGame3D::GetUIManager()
{
	return pUI;
}

S_InGame3D * GetCurrentGame()
{
	return pCurrentGame;
}

int GetScore()
{
	return nScore;
}
