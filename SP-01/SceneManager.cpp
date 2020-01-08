#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_TitleScreen3D.h"
#include "TransitionControl.h"
Scene3D* pCurrentScene = nullptr;
int nCurrentScene = SCENE_TITLE_SCREEN;
int nNextScene = SCENE_TITLE_SCREEN;
int nTransitionInUse = TRANSITION_NONE;
HRESULT InitScene()
{
	TransitionControl::Init();
	switch (nCurrentScene)
	{
	case SCENE_TITLE_SCREEN:
		pCurrentScene = new SceneTitleScreen3D();
		break;
	case SCENE_IN_GAME:
		pCurrentScene = new SceneInGame3D();
		break;
	default:
		break;
	}
	
	return S_OK;
}

void UpdateScene()
{
	if (nNextScene != nCurrentScene || !IsTransitionComplete()) {
		TransitionControl::Update();
		if(nNextScene != nCurrentScene)
			return;
	}
	if (pCurrentScene) {
		nNextScene = pCurrentScene->Update();
		if (nNextScene != nCurrentScene)
			TransitionForNewScene(true);
	}
}

void ChangeScene()
{
	nCurrentScene = nNextScene;
	InitScene();
}

void DrawScene()
{
	
	if (pCurrentScene)
		pCurrentScene->Draw();
	TransitionControl::Draw();
}

void EndScene()
{
	SAFE_DELETE(pCurrentScene);
}

void TransitionForNewScene(bool bTransitionIn)
{
	SetTransition(bTransitionIn, STICKER_TRANSITION);
	nTransitionInUse = STICKER_TRANSITION;
}
