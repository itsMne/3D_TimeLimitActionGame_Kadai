//*****************************************************************************
// S_InGame3D.cpp
// �Q�[���̃V�[�����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "S_InGame3D.h"
#include "debugproc.h"
#include "Field3D.h"
#include "InputManager.h"
#include "RankManager.h"
#include "S_Ranking.h"
#include "Billboard2D.h"
#include "Sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_TIME 150

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
S_InGame3D* pCurrentGame = nullptr;
int nScore;
int nScoreToAdd;
bool bPauseGame;

//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
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

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void S_InGame3D::Init()
{
	//�Ȃ�
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁FeSceneType
//*****************************************************************************
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
	// �f�o�b�O������\���X�V
	UpdateDebugProc();

	// �f�o�b�O������ݒ�
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", GetMainWindowFPS());

	RankManager::Update();
	// ���f���X�V
	pPlayer->Update();

	// �t�B�[���h�X�V
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

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void S_InGame3D::Draw()
{
	// Z�o�b�t�@�L��
	SetZBuffer(true);

	SetCullMode(CULLMODE_NONE);
	// ���f���`��
	
	pPlayer->Draw();
	Enemies->Draw();
	SetCullMode(CULLMODE_CCW);
	
	// �w�ʃJ�����O (�ʏ�͕\�ʂ̂ݕ`��)
	
	SetCullMode(CULLMODE_NONE);
	Walls->Draw();
	if (!bUseTimer)
		TutorialSign->Draw();
	SetCullMode(CULLMODE_CCW);

	pSkybox->Draw();
	// �t�B�[���h�`��
	SetCullMode(CULLMODE_NONE);
	Floors->Draw();
	SetCullMode(CULLMODE_CCW);
	// Z�o�b�t�@����
	SetZBuffer(false);

	// �f�o�b�O������\��
	DrawDebugProc();
	
	
	pUI->Draw();
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void S_InGame3D::End()
{
	Scene3D::End();
	// �t�B�[���h�I������
	SAFE_DELETE(Floors);
	SAFE_DELETE(Enemies);
	SAFE_DELETE(Walls);
	// ���f���\���I������
	SAFE_DELETE(pPlayer);
	SAFE_DELETE(pUI);
}

//*****************************************************************************
//GetList�֐�
//�I�u�W�F�N�g�̃��X�g�̃A�h���X��߂�
//�����Fint
//�߁FGo_List*
//*****************************************************************************
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

//*****************************************************************************
//SetAtkEffect�֐�
//�U������̃G�t�F�N�g��L������
//�����Fint
//�߁Fvoid
//*****************************************************************************
void S_InGame3D::SetAtkEffect(int frames)
{
	pUI->ActivateAtkEffect(frames);
}

//*****************************************************************************
//GetUIManager�֐�
//UI�}�l�[�W���[�̃A�h���X��߂�
//�����Fvoid
//�߁FInGameUI2D*
//*****************************************************************************
InGameUI2D * S_InGame3D::GetUIManager()
{
	return pUI;
}

//*****************************************************************************
//TimeIsUp�֐�
//�^�C�}�[�̏�Ԃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool S_InGame3D::TimeIsUp()
{
	return (bUseTimer && nTimer <= 0);
}

//*****************************************************************************
//GetTimer�֐�
//�^�C�}�[��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int S_InGame3D::GetTimer()
{
	return nTimer;
}

//*****************************************************************************
//GetCurrentGame�֐�
//���݂̃V�[���Q�[���̃A�h���X��߂�
//�����Fvoid
//�߁FS_InGame3D*
//*****************************************************************************
S_InGame3D * GetCurrentGame()
{
	return pCurrentGame;
}

//*****************************************************************************
//GetScore�֐�
//�X�R�A��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GetScore()
{
	return nScore;
}

//*****************************************************************************
//AddScoreWithRank�֐�
//�����N�ŃX�R�A�𑝂₷
//�����Fint
//�߁Fvoid
//*****************************************************************************
void AddScoreWithRank(int add)
{
	nScoreToAdd += add*GetRank();
}

//*****************************************************************************
//AddScore�֐�
//�X�R�A�𑝂₷
//�����Fint
//�߁Fvoid
//*****************************************************************************
void AddScore(int add)
{
	nScoreToAdd += add;
}

//*****************************************************************************
//GetTimer�֐�
//�^�C�}�[��߂�
//�����Fvoid
//�߁Fint
//*****************************************************************************
int GetTimer()
{
	if(pCurrentGame)
		return pCurrentGame->GetTimer();
	return 0;
}

//*****************************************************************************
//IsGamePaused�֐�
//�|�[�X����Ă����Ԃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool IsGamePaused()
{
	return bPauseGame;
}
