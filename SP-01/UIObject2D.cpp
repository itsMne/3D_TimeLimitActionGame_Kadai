#include "UIObject2D.h"
#include "Texture.h"
#define HEART_SIZE	75
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
			pPlayer = GetMainPlayer3D();
		nCurrent_hearts = pPlayer->GetCurrentHealth();
		nMax_Hearts = pPlayer->GetMaxHealth();

		break;
	default:
		break;
	}
	RegularUVAnimation();
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
		UIHeartControl();
		break;
	default:
		DrawPolygon(GetDeviceContext());
		break;
	}
	
}

void UIObject2D::UIHeartControl()
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
