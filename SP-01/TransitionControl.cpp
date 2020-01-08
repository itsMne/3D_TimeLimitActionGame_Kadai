#include "TransitionControl.h"
#include "stdio.h"
#include "Texture.h"
#include "DXWindow3D.h"
#include "SceneManager.h"
#include "Polygon2D.h"
#define MAX_TRANSITIONEFFECTS 10
#define MAX_BG 4

Polygon2D* pBackground = nullptr;
Polygon2D* ptEffects[MAX_TRANSITIONEFFECTS] = {nullptr};
bool bPasteEffect[MAX_TRANSITIONEFFECTS+1];
ID3D11ShaderResourceView* pBGTextures[MAX_BG] = { nullptr };
bool bTCInited = false;
int nTransition = TRANSITION_NONE;
bool bTransitionIn;
bool bTransitionComplete = true;
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

}

void TransitionControl::Update()
{
	static int nFrameCoolOffSticker = 0;
	if (nTransition == TRANSITION_NONE) {
		nFrameCoolOffSticker = 0;
		return;
	}
	
	switch (nTransition)
	{
	case STICKER_TRANSITION:
		if (++nFrameCoolOffSticker > 10)
		{
			nFrameCoolOffSticker = 0;
			if (bTransitionIn) {
				for (int i = 0; i < MAX_TRANSITIONEFFECTS + 1; i++)
				{
					if (!bPasteEffect[i])
					{
						bPasteEffect[i] = true;
						if(i== MAX_TRANSITIONEFFECTS)
							pBackground->SetTexture(pBGTextures[rand() % MAX_BG]);
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
						bPasteEffect[i] = false;
						return;
					}
				}
				bTransitionComplete = true;
				//bTransitionIn = true;//del
			}
		}
		break;
	default:
		break;
	}
}

void TransitionControl::Draw()
{
	if (nTransition == TRANSITION_NONE)
		return;
	SetZBuffer(false);
	if (pBackground && bPasteEffect[MAX_TRANSITIONEFFECTS])
		pBackground->DrawPolygon(GetDeviceContext());
	for (int i = 0; i < MAX_TRANSITIONEFFECTS; i++) {
		if (ptEffects[i] && bPasteEffect[i])
			ptEffects[i]->DrawPolygon(GetDeviceContext());
	}
}

void TransitionControl::End()
{
}

void SetTransition(bool TransitionIn, int TransitionType)
{
	bTransitionComplete = false;
	bTransitionIn = TransitionIn;
	nTransition = TransitionType;
}

bool IsTransitionComplete()
{
	return bTransitionComplete;
}
