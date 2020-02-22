//*****************************************************************************
// SceneManager.h
// �V�[�����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "SceneManager.h"
#include "S_InGame3D.h"
#include "S_Menu.h"
#include "S_TitleScreen3D.h"
#include "S_Ranking.h"
#include "TransitionControl.h"

//*****************************************************************************
//�O���[�o���ϐ�
//*****************************************************************************
Scene3D* pCurrentScene = nullptr;
int nCurrentScene = SCENE_TITLE_SCREEN;
int nNextScene = SCENE_TITLE_SCREEN;
int nTransitionInUse = TRANSITION_NONE;

//*****************************************************************************
//InitScene�֐�
//�������֐�
//�����Fvoid
//�߁FHRESULT
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
//UpdateScene�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
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
//ChangeScene�֐�
//�V�[����ς���
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void ChangeScene()
{
	nCurrentScene = nNextScene;
	InitScene();
}

//*****************************************************************************
//DrawScene�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void DrawScene()
{
	
	if (pCurrentScene)
		pCurrentScene->Draw();
	TransitionControl::Draw();
}

//*****************************************************************************
//EndScene�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void EndScene()
{
	SAFE_DELETE(pCurrentScene);
}

//*****************************************************************************
//TransitionForNewScene�֐�
//�V�[����ς����Ԃ��Ǘ�����
//�����Fbool
//�߁Fvoid
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