//*****************************************************************************
// S_TitleScreen3D.cpp
// タイトルスクリーンを管理する
// Author : Mane
//*****************************************************************************
#include "S_TitleScreen3D.h"
#include "InputManager.h"
#include "Sound.h"

//*****************************************************************************
//コンストラクタ関数
//*****************************************************************************
S_TitleScreen3D::S_TitleScreen3D() :Scene3D(true)
{
	Init();
}


S_TitleScreen3D::~S_TitleScreen3D()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void S_TitleScreen3D::Init()
{
	MainWindow->SetWindowColor(0.8, 0.8, 0.8);
	pSceneLight->GetLight();
	Logo = new UIObject2D(UI_LOGO);
	PlaySoundGame(SOUND_LABEL_TITLE);
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：eSceneType
//*****************************************************************************
eSceneType S_TitleScreen3D::Update()
{
	Scene3D::Update();
	if (Logo)
		Logo->Update();
	if (GetInput(INPUT_PAUSE)|| GetInput(INPUT_SWORD)|| GetInput(INPUT_JUMP)) {
		return SCENE_MENU;
	}
	return SCENE_TITLE_SCREEN;
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void S_TitleScreen3D::Draw()
{
	SetZBuffer(true);//3D
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(2));
	SetZBuffer(false);//2D
	if(Logo)
		Logo->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void S_TitleScreen3D::End()
{
	SAFE_DELETE(Logo);
}
