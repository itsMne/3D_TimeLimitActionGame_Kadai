#include "S_InGame3D.h"
#include "debugproc.h"
#include "Field3D.h"
#include "Cube3D.h"
#include "Wall3D.h"
#include "Enemy3D.h"
#include "Billboard2D.h"


SceneInGame3D* pCurrentGame = nullptr;
Enemy3D* HelloEnemy = nullptr;

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
	Floors->Load("Level_Floors", GO_FLOOR);
	Walls->Load("Level_Walls", GO_WALL);
	hr = InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer);
	pUI = new InGameUI2D();

	HelloEnemy = new Enemy3D();
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
	// �f�o�b�O������\���X�V
	UpdateDebugProc();

	// �f�o�b�O������ݒ�
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", GetMainWindowFPS());

	// ���f���X�V
	pPlayer->Update();

	// �t�B�[���h�X�V
	Floors->Update();

	//HelloShadow->Update();

	pUI->Update();

	pSkybox->Update();
	
	Walls->Update();

	HelloEnemy->Update();
	return SCENE_IN_GAME;
}

void SceneInGame3D::Draw()
{
	// Z�o�b�t�@�L��
	SetZBuffer(true);

	// �O�ʃJ�����O (FBX�͕\�������]���邽��)
	//if(!pDeviceContext)
	//	pDeviceContext = MainWindow->GetDeviceContext();
	//pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));

	SetCullMode(CULLMODE_NONE);
	// ���f���`��
	
	pPlayer->Draw();
	SetCullMode(CULLMODE_CCW);
	HelloEnemy->Draw();
	// �w�ʃJ�����O (�ʏ�͕\�ʂ̂ݕ`��)
	
	SetCullMode(CULLMODE_NONE);
	Walls->Draw();
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

void SceneInGame3D::End()
{
	Scene3D::End();
	// �t�B�[���h�I������
	SAFE_DELETE(Floors);
	SAFE_DELETE(Walls);
	// ���f���\���I������
	SAFE_DELETE(pPlayer);
	// �f�o�b�O������\���I������
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
	default:
		break;
	}
	return nullptr;
}

SceneInGame3D * GetCurrentGame()
{
	return pCurrentGame;
}
