//*****************************************************************************
// TransitionControl.cpp
// �V�[����ς��邱�Ƃ̊Ǘ�����
// Author : Mane
//*****************************************************************************
#include "TransitionControl.h"
#include "stdio.h"
#include "Texture.h"
#include "DXWindow3D.h"
#include "SceneManager.h"
#include "Polygon2D.h"
#include "Sound.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_TRANSITIONEFFECTS 10
#define MAX_BG 4

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Polygon2D* pBackground = nullptr;
Polygon2D* ptEffects[MAX_TRANSITIONEFFECTS] = {nullptr};
bool bPasteEffect[MAX_TRANSITIONEFFECTS+1];
ID3D11ShaderResourceView* pBGTextures[MAX_BG] = { nullptr };
bool bTCInited = false;
int nTransition = TRANSITION_NONE;
bool bTransitionIn;
bool bTransitionComplete = true;
Polygon2D* pSlash = nullptr;
XMFLOAT2 SlashUI;

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TransitionControl::Init()
{
	if (bTCInited)
		return;
	pBackground = new Polygon2D();
	pBackground->SetPolygonPos(0, 0);
	pBackground->SetPolygonSize(1280, 720);
	for (int i = 0; i < MAX_TRANSITIONEFFECTS; i++) {
		char szPath[256] = "data/texture/Transition/";
		char szsteck[2];
		itoa(i+1, szsteck, 10);
		strcat(szPath, szsteck);
		strcat(szPath, ".tga");
		printf("%s\n", szPath);
		ptEffects[i] = new Polygon2D(szPath);
		bPasteEffect[i] = false;
	}
	bTransitionIn = false;
	for (int i = 0; i < MAX_BG; i++)
	{
		char szPath[256] = "data/texture/Transition/BG";
		char szsteck[2];
		itoa(i+1, szsteck, 10);
		strcat(szPath, szsteck);
		strcat(szPath, ".tga");
		printf("%s\n", szPath);
		HRESULT hr = CreateTextureFromFile(GetDevice(), szPath, &pBGTextures[i]);

	}
	pBackground->SetTexture(pBGTextures[rand() % MAX_BG]);
	ptEffects[0]->SetPolygonSize(286, 302);
	ptEffects[0]->SetPolygonPos(-497.000000f, -209.000000f);
	ptEffects[1]->SetPolygonSize(349, 356);
	ptEffects[1]->SetPolygonPos(-466.000000f, 23.000000f);
	ptEffects[2]->SetPolygonSize(348, 328);
	ptEffects[2]->SetPolygonPos(-469.000000f, 199.000000f);
	ptEffects[3]->SetPolygonSize(478, 281);
	ptEffects[3]->SetPolygonPos(-200.000000f, 220.000000f);
	ptEffects[4]->SetPolygonSize(474, 209);
	ptEffects[4]->SetPolygonPos(140.000000f, 255.000000f);
	ptEffects[5]->SetPolygonSize(323, 314);
	ptEffects[5]->SetPolygonPos(486.000000f, 205.000000f);
	ptEffects[6]->SetPolygonSize(229, 272);
	ptEffects[6]->SetPolygonPos(528.000000, -71.000000);
	ptEffects[7]->SetPolygonSize(368, 243);
	ptEffects[7]->SetPolygonPos(456.000000, -257.000000);
	ptEffects[8]->SetPolygonSize(447, 173);
	ptEffects[8]->SetPolygonPos(109.000000, -273.000000);
	ptEffects[9]->SetPolygonSize(523, 256);
	ptEffects[9]->SetPolygonPos(-248.000000, -232.000000);
	bTCInited = true;
	if (!pSlash)
		pSlash = new Polygon2D("data/texture/Transition/Slash.tga");
	pSlash->SetPolygonPos(0, 0);
	SlashUI = { 0,0 };
	pSlash->SetPolygonSize(1280, 720);
	pSlash->SetPolygonUV(0, 0);
	pSlash->SetPolygonFrameSize(1.0f / 4.0f, 1.0f / 8.0f);
	pSlash->SetPolygonPos(0, 0);
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TransitionControl::Update()
{
	static int nFrameCoolOffSticker = 0;
	if (nTransition == TRANSITION_NONE) {
		nFrameCoolOffSticker = 0;
		return;
	}
	static int nFrameCounter = 0;
	switch (nTransition)
	{
	case STICKER_TRANSITION:
		if (++nFrameCoolOffSticker > 5)
		{
			nFrameCoolOffSticker = 0;
			if (bTransitionIn) {
				for (int i = 0; i < MAX_TRANSITIONEFFECTS + 1; i++)
				{
					if (!bPasteEffect[i])
					{
						bPasteEffect[i] = true;
						PlaySoundGame(SOUND_LABEL_SE_PASTESTICKER);
						if (i == MAX_TRANSITIONEFFECTS) {
							StopSound();
							PlaySoundGame(SOUND_LABEL_SE_PASTESTICKER);
							pBackground->SetTexture(pBGTextures[rand() % MAX_BG]);
						}
						return;
					}
				}
				bTransitionIn = false;
				ChangeScene();
			}
			else {
				for (int i = MAX_TRANSITIONEFFECTS + 1; i >= 0; i--)
				{
					if (bPasteEffect[i])
					{
						PlaySoundGame(SOUND_LABEL_SE_REMOVESTICKER);
						bPasteEffect[i] = false;
						return;
					}
				}
				bTransitionComplete = true;
				nTransition = TRANSITION_NONE;
			}
		}
		break;
	case SLASH_TRANSITION:
		if (++nFrameCounter >= 1)
		{
			nFrameCounter = 0;
			SlashUI.x++;
			if (SlashUI.x == 4) {
				SlashUI.y++;
				SlashUI.x = 0;
			}
			if (SlashUI.y == 4 && SlashUI.x == 0) {
				StopSound();
				PlaySoundGame(SOUND_LABEL_SE_TRANSITIONSLASH);
			}
			if (SlashUI.y == 4 && SlashUI.x == 2) {
				ChangeScene();
			}
			if (SlashUI.x == 3 && SlashUI.y == 7)
			{
				bTransitionComplete = true;
				nTransition = TRANSITION_NONE;
				SlashUI = { 0,0 };
			}
		}
		break;
	default:
		break;
	}
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TransitionControl::Draw()
{
	if (nTransition == TRANSITION_NONE)
		return;
	SetZBuffer(false);
	switch (nTransition)
	{
	case STICKER_TRANSITION:
		if (pBackground && bPasteEffect[MAX_TRANSITIONEFFECTS])
			pBackground->DrawPolygon(GetDeviceContext());
		for (int i = 0; i < MAX_TRANSITIONEFFECTS; i++) {
			if (ptEffects[i] && bPasteEffect[i])
				ptEffects[i]->DrawPolygon(GetDeviceContext());
		}
		break;
	case SLASH_TRANSITION:
		if (pSlash)
			pSlash->SetPolygonUV(SlashUI.x / 4.0f, SlashUI.y / 8.0f);
			pSlash->DrawPolygon(GetDeviceContext());
		break;
	}


}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void TransitionControl::End()
{
	//�Ȃ�
}

//*****************************************************************************
//SetTransition�֐�
//�V�[����ς��邱�Ƃ�ݒ肷��
//�����Fbool, int
//�߁Fvoid
//*****************************************************************************
void SetTransition(bool TransitionIn, int TransitionType)
{
	bTransitionComplete = false;
	bTransitionIn = TransitionIn;
	nTransition = TransitionType;
}

//*****************************************************************************
//IsTransitionComplete�֐�
//�V�[����ς��邱�Ƃ��I����������m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool IsTransitionComplete()
{
	return bTransitionComplete;
}
