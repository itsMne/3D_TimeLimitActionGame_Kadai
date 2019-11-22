#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_TitleScreen3D.h"
Scene3D* pCurrentScene = nullptr;
HRESULT InitScene()
{
	//pCurrentScene = new SceneTitleScreen3D();
	pCurrentScene = new SceneInGame3D();
	
	return S_OK;
}

void UpdateScene()
{
	if (pCurrentScene)
		pCurrentScene->Update();
}

void DrawScene()
{
	if (pCurrentScene)
		pCurrentScene->Draw();
}

void EndScene()
{
	SAFE_DELETE(pCurrentScene);
}
