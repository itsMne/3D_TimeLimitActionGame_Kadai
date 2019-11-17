#include "Scene3D.h"



Scene3D::Scene3D(bool Main)
{
	pSceneCamera = new Camera3D(Main);
	MainWindow = GetMainWindow();
	pSceneLight = new Light3D(Main);
	if (MainWindow)
		pDeviceContext = MainWindow->GetDeviceContext();
	else
		pDeviceContext = nullptr;
}


Scene3D::~Scene3D()
{
}

void Scene3D::Update()
{
	// �����X�V
	if(pSceneLight)
		pSceneLight->Update();

	// �J�����X�V
	if(pSceneCamera)
		pSceneCamera->Update();
}

void Scene3D::End()
{
	// �����I������
	SAFE_DELETE(pSceneLight);

	// �J�����I������
	SAFE_DELETE(pSceneCamera);
}
