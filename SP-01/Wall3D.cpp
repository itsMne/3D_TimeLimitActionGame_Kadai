#include "Wall3D.h"
#include "Player3D.h"
#define MODEL_PATH "data/model/Cube2.fbx"
Player3D* pPlayer = nullptr;
Wall3D::Wall3D(): GameObject3D()
{
	nType = GO_WALL;
	strcpy(szTexturePath, "np");
	Init();
}


Wall3D::~Wall3D()
{
	End();
	pPlayer = nullptr;
}

void Wall3D::Init()
{
	InitModel(MODEL_PATH);
	Model->SetScale(0.009765625f);
	SetScale(1);
	
}

void Wall3D::Update()
{
	Hitbox = { 0,0,0,Scale.x,Scale.y,Scale.z };
	GameObject3D::Update();
	if (!pPlayer) {
		pPlayer = GetPlayer3D();
		if (!pPlayer)
			return;
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FEET)) && !pPlayer->GetFloor())
	{
		pPlayer->TranslateY(0.1f);
		if (!IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FEET))) {
			pPlayer->TranslateY(-0.1f);
			pPlayer->SetFloor(this);
			return;
		}
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FRONT)))
	{
		pPlayer->TranslateZ(-0.1f);
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_BACK)))
	{
		pPlayer->TranslateZ(0.1f);
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_LEFT)))
	{
		pPlayer->TranslateX(0.1f);
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_RIGHT)))
	{
		pPlayer->TranslateX(-0.1f);
	}

}

void Wall3D::Draw()
{
	GameObject3D::Draw();
}

void Wall3D::End()
{
	GameObject3D::End();
}


char * Wall3D::GetTexturePath()
{
	return szTexturePath;
}
