//*****************************************************************************
// Wall3D.cpp
// 壁の管理
// Author : Mane
//*****************************************************************************
#include "Wall3D.h"
#include "Player3D.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODEL_PATH "data/model/Cube2.fbx"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Player3D* pPlayer = nullptr;

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
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

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void Wall3D::Init()
{
	InitModel(WALL_MODEL_PATH);
	Model->SetScale(0.009765625f);
	SetScale(1);
	
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void Wall3D::Update()
{
	Hitbox = { 0,0,0,Scale.x,Scale.y,Scale.z };
	GameObject3D::Update();
	if (!pPlayer) {
		pPlayer = GetPlayer3D();
		if (!pPlayer)
			return;
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FEET)) && !pPlayer->GetFloor() && pPlayer->GetYForce()>0)
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

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void Wall3D::Draw()
{
	GameObject3D::Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void Wall3D::End()
{
	GameObject3D::End();
}

//*****************************************************************************
//GetTexturePath関数
//テクスチャのパスを戻す
//引数：void
//戻：char*
//*****************************************************************************
char * Wall3D::GetTexturePath()
{
	return szTexturePath;
}
