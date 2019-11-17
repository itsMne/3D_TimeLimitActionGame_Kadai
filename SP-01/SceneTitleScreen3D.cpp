#include "SceneTitleScreen3D.h"



SceneTitleScreen3D::SceneTitleScreen3D() :Scene3D(true)
{
	pLogo = new GameObject3D(GO_TITLE_LOGO);
	pSceneCamera->SetFocalPointGO(pLogo);
	MainWindow->SetWindowColor(1, 1, 1);
	pLogo->GetModel()->SetPosition({ 0,25,0 });
	pLogo->GetModel()->SetRotation({0,-XM_PI/2, 0});
	pLogo->GetModel()->SetScale(2);
}


SceneTitleScreen3D::~SceneTitleScreen3D()
{
	End();
}

void SceneTitleScreen3D::Init()
{
}

void SceneTitleScreen3D::Update()
{
	Scene3D::Update();
	if (pLogo)
		pLogo->Update();
}

void SceneTitleScreen3D::Draw()
{
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));
	if (pLogo)
		pLogo->Draw();
}

void SceneTitleScreen3D::End()
{
}
