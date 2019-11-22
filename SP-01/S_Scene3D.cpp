#include "S_Scene3D.h"



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

eSceneType Scene3D::Update()
{
	// 光源更新
	if(pSceneLight)
		pSceneLight->Update();

	// カメラ更新
	if(pSceneCamera)
		pSceneCamera->Update();

	return MAX_SCENES;
}

void Scene3D::End()
{
	// 光源終了処理
	SAFE_DELETE(pSceneLight);

	// カメラ終了処理
	SAFE_DELETE(pSceneCamera);
}
