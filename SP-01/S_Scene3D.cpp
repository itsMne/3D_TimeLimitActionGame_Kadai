//*****************************************************************************
// S_Scene3D.cpp
// シーンのベースを管理する(親)
// Author : Mane
//*****************************************************************************
#include "S_Scene3D.h"

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
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

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：eSceneType
//*****************************************************************************
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

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Scene3D::End()
{
	// 光源終了処理
	SAFE_DELETE(pSceneLight);
	// カメラ終了処理
	SAFE_DELETE(pSceneCamera);
}
