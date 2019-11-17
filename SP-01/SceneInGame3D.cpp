#include "SceneInGame3D.h"
#include "debugproc.h"
#include "Field3D.h"




Field3D* HelloField = nullptr;



SceneInGame3D::SceneInGame3D():Scene3D(true)
{
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	pPlayer = new Player3D();
	HelloField = new Field3D();
	//HelloShadow = new GameObject3D(GO_MAX);
	//HelloShadow->SetParent(pPlayer);
	HRESULT	hr;
	g_pDevice = GetDevice();
	hr = HelloField->InitField(pSceneLight, "data/texture/field000.jpg");
	HelloField->SetTextureSubdivisions(3);
	hr = InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer);
	pUI = new InGameUI2D();
}


SceneInGame3D::~SceneInGame3D()
{
	End();
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
	HelloField->UpdateField();

	//HelloShadow->Update();

	pUI->Update();

	return SCENE_IN_GAME;
}

void SceneInGame3D::Draw()
{
	// Z�o�b�t�@�L��
	SetZBuffer(true);

	// �O�ʃJ�����O (FBX�͕\�������]���邽��)
	if(!pDeviceContext)
		pDeviceContext = MainWindow->GetDeviceContext();
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));

	// ���f���`��
	pPlayer->Draw();

	// �w�ʃJ�����O (�ʏ�͕\�ʂ̂ݕ`��)
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(2));

	// �t�B�[���h�`��
	HelloField->DrawField();

	//HelloShadow->Draw();
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
	SAFE_DELETE(HelloField);
	// ���f���\���I������
	SAFE_DELETE(pPlayer);
	// �f�o�b�O������\���I������
	UninitDebugProc();

	SAFE_DELETE(pUI);
}
