#include "SceneManager.h"
#include "SceneInGame3D.h"
#include "SceneTitleScreen3D.h"
Scene3D* pCurrentScene = nullptr;
HRESULT InitScene()
{
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
