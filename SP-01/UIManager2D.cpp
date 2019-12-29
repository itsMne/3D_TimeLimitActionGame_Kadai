#include "UIManager2D.h"
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
	nAnimationSpeed = 0;
	bHeartActive = true;
	nCurrent_hearts = 1;
	nMax_Hearts = 1;
	pPlayer = nullptr;
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
	default:
		DrawPolygon(GetDeviceContext());
		break;
	}
	
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
}

void InGameUI2D::Update()
{
	if (pPlayerHealth)
		pPlayerHealth->Update();
	if (pPlayerAim)
		pPlayerAim->Update();
}

void InGameUI2D::Draw()
{
	if (pPlayerHealth)
		pPlayerHealth->Draw();
	if (pPlayerAim)
		pPlayerAim->Draw();
}

void InGameUI2D::End()
{
	SAFE_DELETE(pPlayerHealth);
	SAFE_DELETE(pPlayerAim);
}
