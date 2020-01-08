#include "S_TitleScreen3D.h"
#include "InputManager.h"

SceneTitleScreen3D::SceneTitleScreen3D() :Scene3D(true)
{
	MainWindow->SetWindowColor(0.8, 0.8, 0.8);
	pSceneLight->GetLight();
	Logo = new UIObject2D(UI_LOGO);
}


SceneTitleScreen3D::~SceneTitleScreen3D()
{
	End();
}

void SceneTitleScreen3D::Init()
{
}

eSceneType SceneTitleScreen3D::Update()
{
	Scene3D::Update();
	if (Logo)
		Logo->Update();
	if (GetInput(INPUT_JUMP))
		return SCENE_IN_GAME;
	return SCENE_TITLE_SCREEN;
}

void SceneTitleScreen3D::Draw()
{
	SetZBuffer(true);//3D
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(2));
	SetZBuffer(false);//2D
	if(Logo)
		Logo->Draw();
}

void SceneTitleScreen3D::End()
{
}
