//*****************************************************************************
// SceneManager.h
// シーンを管理する
// Author : Mane
//*****************************************************************************
#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_Menu.h"
#include "S_TitleScreen3D.h"
#include "S_Ranking.h"
#include "TransitionControl.h"

//*****************************************************************************
//グローバル変数
//*****************************************************************************
Scene3D* pCurrentScene = nullptr;
int nCurrentScene = SCENE_TITLE_SCREEN;
int nNextScene = SCENE_TITLE_SCREEN;
int nTransitionInUse = TRANSITION_NONE;

//*****************************************************************************
//InitScene関数
//初期化関数
//引数：void
//戻：HRESULT
//*****************************************************************************
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
	case SCENE_RANKING:
		pCurrentScene = new S_Ranking();
		break;
	default:
		break;
	}
	
	return S_OK;
}

//*****************************************************************************
//UpdateScene関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
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

//*****************************************************************************
//ChangeScene関数
//シーンを変える
//引数：void
//戻：void
//*****************************************************************************
void ChangeScene()
{
	nCurrentScene = nNextScene;
	InitScene();
}

//*****************************************************************************
//DrawScene関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DrawScene()
{
	
	if (pCurrentScene)
		pCurrentScene->Draw();
	TransitionControl::Draw();
}

//*****************************************************************************
//EndScene関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void EndScene()
{
	SAFE_DELETE(pCurrentScene);
}

//*****************************************************************************
//TransitionForNewScene関数
//シーンを変える状態を管理する
//引数：bool
//戻：void
//*****************************************************************************
void TransitionForNewScene(bool bTransitionIn)
{
	switch (nNextScene)
	{
	case SCENE_TITLE_SCREEN: case SCENE_IN_GAME:
		SetTransition(bTransitionIn, STICKER_TRANSITION);
		nTransitionInUse = STICKER_TRANSITION;
		break;
	case SCENE_MENU: case SCENE_RANKING:
		SetTransition(bTransitionIn, SLASH_TRANSITION);
		nTransitionInUse = STICKER_TRANSITION;
		break;
	default:
		break;
	}
}