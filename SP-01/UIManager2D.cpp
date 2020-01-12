#include "UIManager2D.h"
#include "S_InGame3D.h"
#include "Texture.h"
#define HEART_SIZE	75
#define AIM_SIZE	125
#define HEART_TATE true
UIObject2D::UIObject2D()
{
	Init();
}

UIObject2D::UIObject2D(int nUI_Type)
{
	Init();
	nType = nUI_Type;
	switch (nUI_Type)
	{
	case UI_HEART:
		CreateTextureFromFile(GetDevice(), "data/texture/HeartUI.tga", &gpTexture);
		Position.x = -(SCREEN_WIDTH / 2)+50;
		Position.y = (SCREEN_HEIGHT / 2)-50;
		x2Frame.x = 1.0f/5.0f;
		x2Frame.y = 1.0f/4.0f;
		uv.U = 0;
		uv.V = 0;
		SetPolygonSize(HEART_SIZE, HEART_SIZE);
		nAnimationSpeed = 5;
		SetTexture(gpTexture);
		break;
	case UI_AIM:
		CreateTextureFromFile(GetDevice(), "data/texture/AimUI.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		SetPolygonSize(AIM_SIZE, AIM_SIZE);
		SetTexture(gpTexture);
		break;
	case UI_LOGO:
		CreateTextureFromFile(GetDevice(), "data/texture/LogoUI.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1.0f/8.0f;
		x2Frame.y = 1.0f/8.0f;
		uv.U = 0;
		uv.V = 0;
		nAnimationSpeed = 1;
		SetPolygonSize(768, 432);
		SetTexture(gpTexture);
		break;
	case UI_ATKZOOM:
		CreateTextureFromFile(GetDevice(), "data/texture/AtkEffect.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1.0f / 1.0f;
		x2Frame.y = 1.0f / 1.0f;
		uv.U = 0;
		uv.V = 0;
		SetPolygonSize(1280, 1280);
		SetPolygonAlpha(0.5f);
		SetTexture(gpTexture);
		break;
	case UI_AURA:
		CreateTextureFromFile(GetDevice(), "data/texture/AuraEffect.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		SetPolygonSize(0, 0);
		SetTexture(gpTexture);
		break;
	case UI_GAMEOVER_SCREEN:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/GameOverScreen.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1.0f / 2.0f;
		x2Frame.y = 1.0f / 10.0f;
		uv.U = 0;
		uv.V = 0;
		SetPolygonSize(1280, 720);
		SetPolygonAlpha(1);
		SetTexture(gpTexture);
		nAnimationSpeed = 3;
		break;
	case UI_SCORE:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/NumbersUI.tga", &gpTexture);
		SetPolygonSize(18, 25);
		Position.x = SCREEN_WIDTH / 2 - (25 * 8);
		Position.y = SCREEN_HEIGHT / 2 - 25;
		x2Frame.x = 1 / 10.0f;
		x2Frame.y = 1 / 10.0f;
		uv = { 0,0 };
		nScore = 0;
		SetTexture(gpTexture);
		nAnimationSpeed = 3;
		break;
	default:
		break;
	}
}


UIObject2D::~UIObject2D()
{
	End();
}

void UIObject2D::Init()
{
	nType = UI_TYPE_MAX;
	nAnimationTimer = 0;
	nGameOverFrames = 0;
	fAcceleration = 0;
	nAnimationSpeed = 0;
	bHeartActive = true;
	bIsInUse = false;
	nCurrent_hearts = 1;
	nMax_Hearts = 1;
	pPlayer = nullptr;
	nFramesActive = 0;
}

void UIObject2D::Update()
{
	UpdatePolygon();
	switch (nType)
	{
	case UI_HEART:
		if (!pPlayer)
			pPlayer = GetPlayer3D();
		nCurrent_hearts = pPlayer->GetCurrentHealth();
		nMax_Hearts = pPlayer->GetMaxHealth();
		RegularUVAnimation();
		break;
	case UI_AIM:
		Rotation.z++;
		break;
	case UI_LOGO:
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			uv.U++;
			if (uv.U == 8)
			{
				uv.U = 0;
				uv.V++;
				if (uv.V == 8) {
					uv.V = 6;
					uv.U = 3;
				}
			}
		}
		if (uv.V == 6)
			nAnimationSpeed = 3;
		break;
	case UI_ATKZOOM:
		if (nFramesActive > 0)
			nFramesActive--;
		Rotation.z+=20;
		break;
	case UI_AURA:
		if (!bIsInUse)
			return;
		fAcceleration+=7.5f;
		Scale.x+=fAcceleration;
		Scale.y+=fAcceleration;
		Rotation.z += 5;
		if (Scale.x > 1500)
			bIsInUse = false;
		break;
	case UI_GAMEOVER_SCREEN:
		if (!bIsInUse)
			return;
		if (++nGameOverFrames > 600)
			nGameOverFrames = 600;
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			uv.U++;
			if (uv.U == 2)
			{
				uv.U = 0;
				uv.V++;
				if (uv.V == 10) {
					uv.V = 8;
					//uv.V = 0;//del
					uv.U = 0;
				}
			}
		}
		break;
	case UI_SCORE:
		nScore = GetScore();
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			if (++uv.V == 10)
				uv.V = 0;
		}
		break;
	default:
		break;
	}
	
}

void UIObject2D::RegularUVAnimation()
{
	if (++nAnimationTimer > nAnimationSpeed) {
		nAnimationTimer = 0;
		uv.U++;
		if (uv.U == x2Frame.x)
		{
			uv.U = 0;
			uv.V++;
			if (uv.V == x2Frame.y)
				uv.V = 0;
		}
	}
}

void UIObject2D::Draw()
{
	switch (nType)
	{
	case UI_HEART:
		UIHeartDrawControl();
		break;
	case UI_AIM:
		if (!pPlayer) 
			pPlayer = GetPlayer3D();
		else
			if(pPlayer->IsPlayerAiming())
				DrawPolygon(GetDeviceContext());
		break;
	case UI_LOGO:
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / 8.0f, uv.V / 8.0f);
		DrawPolygon(GetDeviceContext());
		break;
	case UI_ATKZOOM:
		if (nFramesActive > 0)
			DrawPolygon(GetDeviceContext());
		break;
	case UI_AURA:
		if (!bIsInUse)
			return;
		DrawPolygon(GetDeviceContext());
		break;
	case UI_GAMEOVER_SCREEN:
		if (!bIsInUse)
			return;
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / 2.0f, uv.V / 10.0f);
		DrawPolygon(GetDeviceContext());
		break;
	case UI_SCORE:
		DrawNumber();
		break;
	default:
		DrawPolygon(GetDeviceContext());
		break;
	}
	
}

void UIObject2D::DrawNumber()
{
	XMFLOAT3 x2OriginalPosition;
	int nNumberToPrint;
	int nDividerByTen;
	int nDigits = 0;
	int	nMaxDigits = 7;
	x2OriginalPosition = Position;
	nNumberToPrint = nScore;
	for (nDividerByTen = 1; nNumberToPrint > 0; nNumberToPrint /= 10, nDividerByTen *= 10, nDigits++);
	nDividerByTen /= 10;
	if (nScore < 10)
	{
		for (int i = 0; i < nMaxDigits; i++)
		{
			uv.U = 0;
			SetPolygonFrameSize(x2Frame.x, x2Frame.y);
			SetPolygonUV(uv.U / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
			DrawPolygon(GetDeviceContext());
			Position.x += 27 * 0.75f;
		}
		uv.U = nScore;
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
		DrawPolygon(GetDeviceContext());
	}
	else {
		for (int i = 0; i < nMaxDigits - nDigits + 1; i++)
		{
			uv.U = 0;
			SetPolygonFrameSize(x2Frame.x, x2Frame.y);
			SetPolygonUV(uv.U / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
			DrawPolygon(GetDeviceContext());
			Position.x += 27 * 0.75f;
		}
		while (nDividerByTen > 0)
		{
			nNumberToPrint = nScore / nDividerByTen % 10;
			SetPolygonFrameSize(x2Frame.x, x2Frame.y);
			SetPolygonUV(nNumberToPrint / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
			DrawPolygon(GetDeviceContext());
			Position.x += 27 * 0.75f;
			nDividerByTen /= 10;
		}
	}
	SetPolygonPos(x2OriginalPosition.x, x2OriginalPosition.y);
}

void UIObject2D::UIHeartDrawControl()
{
	int nHeartsToShow = 1;
	if (nHeartsToShow > nMax_Hearts)
		return;
	XMFLOAT3 ogHeartPos = Position;
	for (int i = 0; i < nMax_Hearts; i++, nHeartsToShow++)
	{
		bHeartActive = nHeartsToShow <= nCurrent_hearts;
		if (bHeartActive)
		{
			SetPolygonSize(HEART_SIZE, HEART_SIZE);
			SetColor(1, 1, 1);
		}
		else
		{
			SetPolygonSize(HEART_SIZE*0.75f, HEART_SIZE*0.75f);
			SetColor(0.25, 0.25, 0.25);
		}
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / 5.0f, uv.V / 4.0f);
		DrawPolygon(GetDeviceContext());
		if(HEART_TATE)
			Position.y -= HEART_SIZE * 0.75f;
		else
			Position.x -= HEART_SIZE * 0.75f;
	}
	Position = ogHeartPos;
}

void UIObject2D::End()
{
}
void UIObject2D::SetActiveFrames(int Frames)
{
	nFramesActive=Frames;
}
bool UIObject2D::GetUse()
{
	return bIsInUse;
}
void UIObject2D::SetAura()
{
	bIsInUse = true;
	Scale = { 0,0,0 };
	fAcceleration = 0;
}

void UIObject2D::SetUse(bool use)
{
	bIsInUse = use;
}

int UIObject2D::GetGameOverFrames()
{
	return nGameOverFrames;
}
 //UIマネージャー
InGameUI2D::InGameUI2D()
{
	Init();
}

InGameUI2D::~InGameUI2D()
{
	End();
}

void InGameUI2D::Init()
{
	pPlayerHealth = new UIObject2D(UI_AIM);
	pPlayerAim = new UIObject2D(UI_HEART);
	pAtkEffect = new UIObject2D(UI_ATKZOOM);
	for (int i = 0; i < MAX_AURA; i++)
		pAuraEffects[i]=new UIObject2D(UI_AURA);
	pGameOverScreen = new UIObject2D(UI_GAMEOVER_SCREEN);
	pScore = new UIObject2D(UI_SCORE);
}

void InGameUI2D::Update()
{
	if (pPlayerHealth)
		pPlayerHealth->Update();
	if (pPlayerAim)
		pPlayerAim->Update();
	if (pAtkEffect)
		pAtkEffect->Update();
	for (int i = 0; i < MAX_AURA; i++)
		if(pAuraEffects[i])pAuraEffects[i]->Update();
	if (pGameOverScreen)
		pGameOverScreen->Update();
	if (pScore)
		pScore->Update();
}

void InGameUI2D::Draw()
{
	if (pPlayerHealth)
		pPlayerHealth->Draw();
	if (pPlayerAim)
		pPlayerAim->Draw();
	if (pAtkEffect)
		pAtkEffect->Draw();
	for (int i = 0; i < MAX_AURA; i++)
		if (pAuraEffects[i])pAuraEffects[i]->Draw();
	if (pGameOverScreen)
		pGameOverScreen->Draw();
	if (pScore)
		pScore->Draw();
}

void InGameUI2D::End()
{
	SAFE_DELETE(pPlayerHealth);
	SAFE_DELETE(pPlayerAim);
	for (int i = 0; i < MAX_AURA; i++)
		SAFE_DELETE(pAuraEffects[i]);
	SAFE_DELETE(pAtkEffect);
	SAFE_DELETE(pGameOverScreen);
	SAFE_DELETE(pScore);
}

void InGameUI2D::ActivateAtkEffect(int Frames)
{
	if (pAtkEffect)
		pAtkEffect->SetActiveFrames(Frames);
}

void InGameUI2D::ActivateGameOver()
{
	if (pGameOverScreen)
		pGameOverScreen->SetUse(true);
}

void InGameUI2D::SetAura()
{
	for (int i = 0; i < MAX_AURA; i++)
	{
		if (!pAuraEffects[i])
			continue;
		if (pAuraEffects[i]->GetUse())
			continue;
		pAuraEffects[i]->SetAura();
	}
}

int InGameUI2D::GetGameOverFrames()
{
	if (pGameOverScreen)
		return pGameOverScreen->GetGameOverFrames();
	return 0;
}
