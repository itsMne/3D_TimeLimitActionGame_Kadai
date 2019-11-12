#include "SceneManager.h"
#include "DXWindow3D.h"
#include "debugproc.h"
#include "Camera3D.h"
#include "Light3D.h"
#include "Field3D.h"
#include "Player3D.h"
ID3D11Device* g_pDevice = nullptr;
Camera3D* SceneCamera = nullptr;
Light3D* SceneLight = nullptr;
Player3D* HelloModel = nullptr;
Field3D* HelloField = nullptr;
DXWindow3D* MainWindow = nullptr;
GameObject3D* HelloShadow = nullptr;
HRESULT InitScene()
{
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	SceneCamera = new Camera3D(true);
	SceneLight = new Light3D(true);
	HelloModel = new Player3D();
	HelloField = new Field3D();
	HelloShadow = new GameObject3D(GO_MAX);
	HelloShadow->SetParent(HelloModel);
	HRESULT	hr;
	g_pDevice = GetDevice();
	hr = HelloField->InitField(SceneLight, "data/texture/field000.jpg");
	hr = InitDebugProc();
	SceneCamera->SetFocalPointGO(HelloModel);
	return hr;
}

void UpdateScene()
{
	// �f�o�b�O������\���X�V
	UpdateDebugProc();

	// �f�o�b�O������ݒ�
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", GetMainWindowFPS());

	// �����X�V
	SceneLight->Update();

	// �J�����X�V
	SceneCamera->Update();

	// ���f���X�V
	HelloModel->Update();

	// �t�B�[���h�X�V
	HelloField->UpdateField();

	HelloShadow->Update();
}

void DrawScene()
{
	DXWindow3D* pMainWindow = GetMainWindow();
	// Z�o�b�t�@�L��
	SetZBuffer(true);

	// �O�ʃJ�����O (FBX�͕\�������]���邽��)
	ID3D11DeviceContext* pDeviceContext = pMainWindow->GetDeviceContext();
	pDeviceContext->RSSetState(pMainWindow->GetRasterizerState(1));

	// ���f���`��
	HelloModel->Draw();

	// �w�ʃJ�����O (�ʏ�͕\�ʂ̂ݕ`��)
	pDeviceContext->RSSetState(pMainWindow->GetRasterizerState(2));

	// �t�B�[���h�`��
	HelloField->DrawField();

	HelloShadow->Draw();
	// Z�o�b�t�@����
	SetZBuffer(false);

	// �f�o�b�O������\��
	DrawDebugProc();
}

void EndScene()
{
	// �t�B�[���h�I������
	HelloField->UninitField();
	// ���f���\���I������
	HelloModel->End();

	// �����I������
	SceneLight->End();

	HelloShadow->End();
	// �J�����I������
	if(SceneCamera)
		SceneCamera->End();

	// �f�o�b�O������\���I������
	UninitDebugProc();
}
