#include "S_InGame3D.h"
#include "debugproc.h"
#include "Field3D.h"
#include "InputManager.h"
#include "RankManager.h"
#include "S_Ranking.h"
#include "Billboard2D.h"
#include "Sound.h"
#define MAX_TIME 150

S_InGame3D* pCurrentGame = nullptr;
int nScore;
int nScoreToAdd;
bool bPauseGame;

S_InGame3D::S_InGame3D() :Scene3D(true)
{
	pCurrentGame = this;
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	pPlayer = new Player3D();
	pPlayer->SetPosition({ 2.578626f, 119.499969f, -138.668900f });
	pPlayer->SetBottom(-650);
	pSkybox = new Sphere3D("data/texture/Skybox.tga");
	HRESULT	hr;
	g_pDevice = GetDevice();

	Floors = new Go_List();
	Walls = new Go_List();
	Enemies = new Go_List();
	Floors->Load("Level_Floors", GO_FLOOR);
	Walls->Load("Level_Walls", GO_WALL);
	Enemies->Load("Level_Enemies", GO_ENEMY);
	TutorialSign = new GameObject3D(GO_TUTORIALSIGN);
	hr = InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer);
	pUI = new InGameUI2D();

	nTimer = MAX_TIME;
	nFrameCounter = 0;
	nScoreToAdd = nScore = 0;
	RankManager::Init();
	bUseTimer = false;
	bGameOverActivated = false;
	PlaySoundGame(SOUND_LABEL_TUTORIAL);
	//Enemies->AddEnemy({ 0,0,0 });
	bPauseGame = false;
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
	if (bPauseGame)
	{
		if (GetInput(INPUT_PAUSE)) {
			bPauseGame = false;
			PlaySoundGame(SOUND_LABEL_SE_REMOVESTICKER);
		}
		return SCENE_IN_GAME;
	}
	if (GetInput(INPUT_PAUSE)) {
		bPauseGame = true;
		PlaySoundGame(SOUND_LABEL_SE_PASTESTICKER);
	}
	if (pUI->GetGameOverFrames() > 120)
	{
		if (GetInput(INPUT_JUMP) || GetInput(INPUT_SWORD))
			return SCENE_TITLE_SCREEN;
		pUI->Update();
		return SCENE_IN_GAME;
	}
	if (bGameOverActivated) {
		pPlayer->Update();
		pUI->Update();
		return SCENE_IN_GAME;
	}
	Scene3D::Update();
	if (pPlayer->GetPosition().y < 69.200058f && !bUseTimer)
	{
		bUseTimer = true;
		pUI->DeactivateTutorial();
		StopSound();
		PlaySoundGame(SOUND_LABEL_SA_GAME);
	}
	if (nScoreToAdd > 0)
	{
		nScore += 1;
		nScoreToAdd -= 1;
	}
	if(bUseTimer)
	if (++nFrameCounter >= 60)
	{
		nFrameCounter = 0;
		nTimer--;
		if (nTimer < 0)
			nTimer = 0;
	}
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

	if(!bUseTimer)
		TutorialSign->Update();

	Enemies->Update();
	if (pPlayer->IsPlayerDead() || nTimer <= 0) {
		pUI->ActivateGameOver();
		AddScoreToTopRankings(nScore + nScoreToAdd, MODE_SCORE_ATTACK);
		nScore += nScoreToAdd;
		nScoreToAdd = 0;
		bGameOverActivated = true;
	}
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
	if (!bUseTimer)
		TutorialSign->Draw();
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

bool S_InGame3D::TimeIsUp()
{
	return (bUseTimer && nTimer <= 0);
}

int S_InGame3D::GetTimer()
{
	return nTimer;
}

S_InGame3D * GetCurrentGame()
{
	return pCurrentGame;
}

int GetScore()
{
	return nScore;
}

void AddScoreWithRank(int add)
{
	nScoreToAdd += add*GetRank();
}
void AddScore(int add)
{
	nScoreToAdd += add;
}

int GetTimer()
{
	if(pCurrentGame)
		return pCurrentGame->GetTimer();
	return 0;
}

bool IsGamePaused()
{
	return bPauseGame;
}
