#include "S_Menu.h"
#include "Texture.h"
#include "DXWindow3D.h"
#include "InputManager.h"
#include "Sound.h"


S_Menu::S_Menu() :Scene3D(false)
{
	Init();
}


S_Menu::~S_Menu()
{
}

void S_Menu::Init()
{
	MainWindow->SetWindowColor(0.58, 0.58, 0.78);
	pSceneLight->GetLight();
	nCurrentSelection = OPTION_SCORE_ATTACK;
	CreateTextureFromFile(GetDevice(), "data/texture/UI/Decoration.tga", &g_pTexture);
	XMFLOAT2 Pos = { -SCREEN_WIDTH / 2+50, SCREEN_HEIGHT/2 -50 }; 
	bool bSpin = true;
	bool bSpinY = true;
	for (int i = 0; i < MAX_DECORATIONS; i++)
	{
		pDecorations[i] = new Polygon2D();
		pDecorations[i]->SetPolygonSize(100, 100);
		pDecorations[i]->SetTexture(g_pTexture);
		pDecorations[i]->SetPolygonAlpha(0.25f);
		if (bSpin) {
			pDecorations[i]->RotateAroundY(180);
			bSpin = false;
		}
		else {
			bSpin = true;
		}
		pDecorations[i]->SetPolygonPos(Pos.x, Pos.y);
		Pos.x += 100;
		if (Pos.x > 800)
		{
			if (bSpinY) {
				bSpin = false;
				bSpinY = false;
			}
			else {
				bSpin = true;
				bSpinY = true;
			}
			Pos.x = -SCREEN_WIDTH / 2 + 50;
			Pos.y -= 100;
		}
	}
	pAim = new Polygon2D("data/texture/UI/MenuAimUI.tga");
	pAim->SetPolygonSize(783*2.2f / 2, 720*2.2f / 2);
	pAim->SetPolygonAlpha(0.75f);
	pAim2 = new Polygon2D("data/texture/UI/MenuAimUI.tga");
	pAim2->SetPolygonSize(783*2.2f / 1.65f, 720*2.2f / 1.65f);
	pAim2->SetPolygonAlpha(0.75f);
	pMenuOption[OPTION_SCORE_ATTACK]= new Polygon2D("data/texture/UI/ScoreAtkUI.tga");
	pMenuOption[OPTION_EXIT]= new Polygon2D("data/texture/UI/OwariUI.tga");
	
	for (int i = 0; i < MAX_OPTIONS; i++)
	{
		pMenuOption[i]->SetPolygonAlpha(0.75f);
		pMenuOption[i]->SetPolygonSize(165, 718);
		pMenuOption[i]->SetPolygonPos(0 + (550 * i), 0, true);
		fAccelerator[i] =  0;

	}
	PlaySoundGame(SOUND_LABEL_MENU);
}

eSceneType S_Menu::Update()
{
	for (int i = 0; i < MAX_DECORATIONS; i++) {
		pDecorations[i]->RotateAroundY(1);
		pDecorations[i]->Translate({ 2,-2 });
		if (pDecorations[i]->GetPolygonPos().x > SCREEN_WIDTH / 2 + 50)
			pDecorations[i]->Translate({ -(SCREEN_WIDTH + 100),0 });
		if (pDecorations[i]->GetPolygonPos().y < -SCREEN_HEIGHT / 2 - 100) {
			pDecorations[i]->Translate({ 0,SCREEN_HEIGHT + 200 });
			pDecorations[i]->RotateAroundY(180);
		}
	}
	pAim->RotateAroundZ(1);
	pAim2->RotateAroundZ(-1);
	static bool ScaleUp = true;
	static float fScale=0;
	if (ScaleUp)
		fScale+=0.5f;
	else
		fScale-=0.5f;
	if (fScale > 20)
		ScaleUp = false;
	else if (fScale < 0)
		ScaleUp = true;
	bool isMoving = false;
	for (int i = 0; i < MAX_OPTIONS; i++) {
		
		pMenuOption[i]->SetPolygonSize(165, 718);
		XMFLOAT2 InitialPosition = pMenuOption[i]->GetPolygonInitialPos();
		XMFLOAT2 CurrentPosition = pMenuOption[i]->GetPolygonPos();

		if (CurrentPosition.x > InitialPosition.x - nCurrentSelection * 550)
		{
			fAccelerator[i] += 1.5f;
			isMoving = true;
			pMenuOption[i]->Translate({ -fAccelerator[i], 0 });
			CurrentPosition = pMenuOption[i]->GetPolygonPos();
			if (CurrentPosition.x < InitialPosition.x - nCurrentSelection * 550) {
				pMenuOption[i]->SetPolygonPos(InitialPosition.x - nCurrentSelection * 550, 0);
				fAccelerator[i] = 0;
				isMoving = false;
			}
		}
		else if (CurrentPosition.x < InitialPosition.x - nCurrentSelection * 550)
		{
			isMoving = true;
			fAccelerator[i] += 1.5f;
			pMenuOption[i]->Translate({ fAccelerator[i], 0 });
			CurrentPosition = pMenuOption[i]->GetPolygonPos();
			if (CurrentPosition.x > InitialPosition.x - nCurrentSelection * 550) {
				fAccelerator[i] = 0;
				pMenuOption[i]->SetPolygonPos(InitialPosition.x - nCurrentSelection * 550, 0);
				isMoving = false;
			}
		}
	}
	pMenuOption[nCurrentSelection]->SetPolygonSize(165+ fScale, 718+ fScale);
	if (GetInput(INPUT_MENU_LEFT) && !isMoving)
		nCurrentSelection--;
	else if (GetInput(INPUT_MENU_RIGHT) && !isMoving)
		nCurrentSelection++;
	if (nCurrentSelection >= MAX_OPTIONS)
		nCurrentSelection = 0;
	else if (nCurrentSelection < 0)
		nCurrentSelection = MAX_OPTIONS - 1;
	if (GetInput(INPUT_JUMP) && !isMoving)
	{
		switch (nCurrentSelection)
		{
		case OPTION_SCORE_ATTACK:
			StopSound();
			return SCENE_IN_GAME;
			break;
		case OPTION_EXIT:
			StopSound();
			EndCurrentGame();
			break;
		default:
			break;
		}
	}
	return SCENE_MENU;
}

void S_Menu::Draw()
{
	for (int i = 0; i < MAX_DECORATIONS; i++)
		pDecorations[i]->DrawPolygon(GetDeviceContext());
	pAim->DrawPolygon(GetDeviceContext());
	pAim2->DrawPolygon(GetDeviceContext());
	for (int i = 0; i < MAX_OPTIONS; i++)
		pMenuOption[i]->DrawPolygon(GetDeviceContext());
}

void S_Menu::End()
{
}
