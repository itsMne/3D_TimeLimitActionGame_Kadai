#include "S_TitleScreen3D.h"
#include "InputManager.h"

S_TitleScreen3D::S_TitleScreen3D() :Scene3D(true)
{
	MainWindow->SetWindowColor(0.8, 0.8, 0.8);
	pSceneLight->GetLight();
	Logo = new UIObject2D(UI_LOGO);
}


S_TitleScreen3D::~S_TitleScreen3D()
{
	End();
}

void S_TitleScreen3D::Init()
{
}

eSceneType S_TitleScreen3D::Update()
{
	Scene3D::Update();
	if (Logo)
		Logo->Update();
	if (GetInput(INPUT_JUMP))
		return SCENE_MENU;
	return SCENE_TITLE_SCREEN;
}

void S_TitleScreen3D::Draw()
{
	SetZBuffer(true);//3D
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(2));
	SetZBuffer(false);//2D
	if(Logo)
		Logo->Draw();
}

void S_TitleScreen3D::End()
{
}
