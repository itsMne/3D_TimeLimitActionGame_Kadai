#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_Menu.h"
#include "S_TitleScreen3D.h"
#include "TransitionControl.h"
Scene3D* pCurrentScene = nullptr;
int nCurrentScene = SCENE_TITLE_SCREEN;
int nNextScene = SCENE_TITLE_SCREEN;
int nTransitionInUse = TRANSITION_NONE;
HRESULT InitScene()
{
	TransitionControl::Init();
	if (pCurrentScene)
		SAFE_DELETE(pCurrentScene);
	switch (nCurrentScene)
	{
	case SCENE_TITLE_SCREEN:
		pCurrentScene = new S_TitleScreen3D();
		break;
	case SCENE_MENU:
		pCurrentScene = new S_Menu();
		break;
	case SCENE_IN_GAME:
		pCurrentScene = new S_InGame3D();
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
	switch (nNextScene)
	{
	case SCENE_TITLE_SCREEN: case SCENE_IN_GAME:
		SetTransition(bTransitionIn, STICKER_TRANSITION);
		nTransitionInUse = STICKER_TRANSITION;
		break;
	case SCENE_MENU:
		SetTransition(bTransitionIn, SLASH_TRANSITION);
		nTransitionInUse = STICKER_TRANSITION;
		break;
	default:
		break;
	}

}