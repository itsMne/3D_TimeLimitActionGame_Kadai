#include "Player3D.h"
#include "InputManager.h"
#include "debugproc.h"
#define PLAYER_MODEL_PATH "data/model/NinaModel.fbx"
#define PLAYER_SPEED	2.5f					// 移動速度
#define ROTATION_SPEED	XM_PI*0.02f			// 回転速度
#define PLAYER_SCALE	0.5f
#define BULLET_COOLDOWN 5.0f
#define INITIAL_HEALTH 3
#define USE_HITBOX true
#define GRAVITY_FORCE  0.35f
#define JUMP_FORCE  6
Player3D* pMainPlayer3D = nullptr;
enum PLAYER_ANIMATIONS
{
	ANIMATION_IDLE=0,//アイドル
	ANIMATION_WALKING,//動く 
	ANIMATION_AIMING,//狙う
	BASIC_CHAIN_A,
	BASIC_CHAIN_B,
	BASIC_CHAIN_C,
	BASIC_CHAIN_D,
	BASIC_CHAIN_E,
	SLIDE,
	SLIDEKICK,
	AIRCOMBOA,
	AIRCOMBOB,
	AIRCOMBOC,
	AIRCOMBOD,
	AIRCOMBOE,
	JUMP,
	AIR_IDLE,
	ROULETTE,
	STAB_BLOCK,
	AIR_STAB_BLOCK,
	SLOWWALK,
	MAX_ANIMATIONS
};
int nAnimationSpeeds[MAX_ANIMATIONS] =//アニメーションの速さ
{
	{2},//IDLE
	{4},//WALKING
	{1},//AIMING
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},
	{1},//SLOWWALK
};
Player3D::Player3D():GameObject3D(GetMainLight(), PLAYER_MODEL_PATH, GO_PLAYER)
{
	mShadow = nullptr;
	pFloor = nullptr;
	bSwitcheToAimingState = false;
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = nullptr;
	}
	Init();
}

Player3D::Player3D(Light3D * Light):GameObject3D(Light, PLAYER_MODEL_PATH, GO_PLAYER)
{
	Init();
}


Player3D::~Player3D()
{
	End();
}

void Player3D::Init()
{
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = nullptr;
	}
	SetScale(PLAYER_SCALE);
	pMainPlayer3D = this;
	nState = PLAYER_IDLE_STATE;
	nMaxHealth = nCurrentHealth = INITIAL_HEALTH;
	//printf("%f\n", GetModel()->GetPosition().y);
	nType = GO_PLAYER;
	nShootCooldown = 0;
	fY_force = 0;
	pDeviceContext = GetDeviceContext();
	pCurrentWindow = GetMainWindow();
	mShadow = new GameObject3D("data/model/Shadow.fbx", GO_SHADOW);
	mShadow->SetRotation({ 0,0,0 });
	mShadow->SetParent(this);
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = new GameObject3D(GO_BULLET);
		goBullets[i]->SetUse(false);
		goBullets[i]->SetHitbox({ 0,0,0,2,2,2 });
	}
	Hitboxes[PLAYER_HB_FEET] = { 0,5,0,5,3,5 };
#if USE_HITBOX
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = new Cube3D();
		pVisualHitboxes[i]->Init("data/texture/hbox.tga");
		pVisualHitboxes[i]->SetScale({ Hitboxes[i].SizeX, Hitboxes[i].SizeY, Hitboxes[i].SizeZ });
		pVisualHitboxes[i]->SetPosition({ Hitboxes[i].PositionX,Hitboxes[i].PositionY,Hitboxes[i].PositionZ });
	}
#endif
}

void Player3D::Update()
{
	GameObject3D::Update();
	GravityControl();
	// カメラの向き取得
	if (!pMainCamera)
		pMainCamera = GetMainCamera();
	if (GetInput(INPUT_JUMP) && IsOnTheFloor())
	{
		while (IsInCollision3D(pFloor->GetHitbox(), GetHitboxPlayer(PLAYER_HB_FEET)))
			Position.y++;
		fY_force = -JUMP_FORCE;
		pFloor = nullptr;
	}
	static int NumTest = 0;
	switch (nState)
	{
	case PLAYER_IDLE_STATE:
		SetPlayerAnimation(ANIMATION_IDLE);
		if (GetInput(INPUT_FORWARD) || GetInput(INPUT_BACKWARD) || GetInput(INPUT_RIGHT) || GetInput(INPUT_LEFT) || GetInput(INPUT_AIM) 
			|| GetAxis(MOVEMENT_AXIS_VERTICAL) != 0 || GetAxis(MOVEMENT_AXIS_HORIZONTAL) != 0)
			nState = PLAYER_MOVING_STATE;
		break;
	case PLAYER_MOVING_STATE:
		MoveControl();
		break;
	default:
		break;
	}

	if (bSwitcheToAimingState)
	{
		bSwitcheToAimingState = GetInput(INPUT_AIM);
		if (!bSwitcheToAimingState) {
			Rotation.x = 0;
			Model->SetRotationX(0);
		}
	}
	PlayerCameraControl();
	PlayerBulletsControl();
	PlayerShadowControl();
}

void Player3D::GravityControl()
{
	if (pFloor) {
		bool bCurrentfloorcol = IsInCollision3D(pFloor->GetHitbox(), GetHitboxPlayer(PLAYER_HB_FEET));
		if (!bCurrentfloorcol) {
			
			pFloor = nullptr;
			fY_force = 0;
		}
		return;
	}
	printf("%f\n", fY_force);
	fY_force += GRAVITY_FORCE;
	Position.y -= fY_force;
}

void Player3D::PlayerCameraControl()
{
	if (GetAxis(CAMERA_AXIS_HORIZONTAL) != 0)
		RotateAroundY(-ROTATION_SPEED * GetAxis(CAMERA_AXIS_HORIZONTAL));
	if (GetAxis(CAMERA_AXIS_VERTICAL) != 0)
		RotateAroundX(ROTATION_SPEED * GetAxis(CAMERA_AXIS_VERTICAL));

}

void Player3D::MoveControl()
{
	
	XMFLOAT3 rotCamera;
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
	rotCamera = pMainCamera->GetCameraAngle();
	if (!GetInput(INPUT_FORWARD) && !GetInput(INPUT_BACKWARD) && !GetInput(INPUT_RIGHT) && !GetInput(INPUT_LEFT) && !GetInput(INPUT_AIM)
		&& fVerticalAxis == 0 && fHorizontalAxis == 0) {
		nState = PLAYER_IDLE_STATE;
		return;
	}
	//スティック：

	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	//printf("%f\n", nModelRotetion);
	if (fVerticalAxis != 0) {
		
		Position.x -= sinf(XM_PI + rotCamera.y) * PLAYER_SPEED * fVerticalAxis;
		Position.z -= cosf(XM_PI + rotCamera.y) * PLAYER_SPEED * fVerticalAxis;
	}
	if (fHorizontalAxis != 0) {

		Position.x -= sinf(rotCamera.y - XM_PI * 0.50f) * PLAYER_SPEED * fHorizontalAxis;
		Position.z -= cosf(rotCamera.y - XM_PI * 0.50f) * PLAYER_SPEED * fHorizontalAxis;
	}
	if (fVerticalAxis != 0 || fHorizontalAxis != 0)
	{
		XMFLOAT3 x3CurrentModelRot = Model->GetRotation();
		Model->SetRotationY(nModelRotation+Rotation.y);
		if (!GetInput(INPUT_AIM)) {
			if (fVerticalAxis < 0.2f && fHorizontalAxis < 0.2f &&
				fVerticalAxis > -0.2f && fHorizontalAxis > -0.2f)
				SetPlayerAnimation(SLOWWALK);
			else
				SetPlayerAnimation(ANIMATION_WALKING);
		}
	}
	if (GetKeyPress(VK_RETURN)) {
		// リセット
		Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
}

void Player3D::SetPlayerAnimation(int Animation)
{
	Model->SwitchAnimation(Animation, 0, nAnimationSpeeds[Animation]);
}

void Player3D::SetPlayerAnimation(int Animation, int Speed)
{
	Model->SwitchAnimation(Animation, 0, Speed);
}

void Player3D::PlayerShadowControl()
{
	if (!mShadow)
		return;
	mShadow->Update();
	XMFLOAT3 ModelRot = Model->GetRotation();
	mShadow->SetRotation({ 0, ModelRot.y, 0 });
}

void Player3D::PlayerBulletsControl()
{
	if (GetInput(INPUT_AIM))
	{
		GetModel()->SetRotation(Rotation);
		SetPlayerAnimation(ANIMATION_AIMING);
		bSwitcheToAimingState = true;
		
	}
	if (GetInput(INPUT_SHOOT) && GetInput(INPUT_AIM) && ++nShootCooldown>= BULLET_COOLDOWN) {
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			if (!goBullets[i])
				continue;
			if (goBullets[i]->IsInUse())
				continue;
			XMFLOAT3 rotCamera;
			rotCamera = pMainCamera->GetCameraAngle();
			goBullets[i]->SetUse(true);
			goBullets[i]->SetRotation(Rotation);
			float fYOffset = 14*(sinf(Rotation.x)*cosf(XM_PI + Rotation.x) + (cosf(Rotation.x)*sinf(XM_PI + Rotation.x))+1);
			goBullets[i]->SetPosition({ Position.x+ sinf(rotCamera.y) *15, Position.y + fYOffset, Position.z+ cosf(rotCamera.y)*15 });
			//printf("%f\n", goBullets[i]->GetPosition().y);
			SetExplosion(goBullets[i]->GetPosition());
			nShootCooldown = 0;
			break;
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!goBullets[i])
			continue;
		goBullets[i]->Update();
	}
}

void Player3D::Draw()
{
	pDeviceContext->RSSetState(pCurrentWindow->GetRasterizerState(1));
	GameObject3D::Draw();
	pDeviceContext->RSSetState(pCurrentWindow->GetRasterizerState(2));
	if(mShadow)
		mShadow->Draw();
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!goBullets[i])
			continue;
		goBullets[i]->Draw();
	}
#if USE_HITBOX
	GetMainLight()->SetLightEnable(false);
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		if (!pVisualHitboxes[i])continue;
		Box pHB = GetHitboxPlayer(i);
		pVisualHitboxes[i]->SetScale({ pHB.SizeX, pHB.SizeY, pHB.SizeZ });
		pVisualHitboxes[i]->SetPosition({ pHB.PositionX, pHB.PositionY, pHB.PositionZ });
		pVisualHitboxes[i]->Draw();
	}
	GetMainLight()->SetLightEnable(true);
#endif
}

void Player3D::End()
{
	GameObject3D::End();
	mShadow->End();
	for (int i = 0; i < MAX_BULLETS; i++)
		SAFE_DELETE(goBullets[i])
	for (int i = 0; i < PLAYER_HB_MAX; i++)
		SAFE_DELETE(pVisualHitboxes[i])

}

int Player3D::GetCurrentHealth()
{
	return nCurrentHealth;
}

int Player3D::GetMaxHealth()
{
	return nMaxHealth;
}

bool Player3D::IsPlayerAiming()
{
	return bSwitcheToAimingState;
}

Box Player3D::GetHitboxPlayer(int hb)
{
	return { Hitboxes[hb].PositionX + Position.x,Hitboxes[hb].PositionY + Position.y,Hitboxes[hb].PositionZ + Position.z, Hitboxes[hb].SizeX,Hitboxes[hb].SizeY,Hitboxes[hb].SizeZ };
}

void Player3D::SetFloor(Field3D * Floor)
{
	pFloor = Floor;
}

bool Player3D::IsOnTheFloor()
{
	return pFloor!=nullptr;
}

Player3D * GetMainPlayer3D()
{
	return pMainPlayer3D;
}
