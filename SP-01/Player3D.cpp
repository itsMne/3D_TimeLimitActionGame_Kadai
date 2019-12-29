#include "Player3D.h"
#include "InputManager.h"
#include "debugproc.h"
#include "Texture.h"
#define PLAYER_MODEL_PATH "data/model/NinaModel.fbx"
#define PLAYER_SPEED	1.95f					// 移動速度
#define ROTATION_SPEED	XM_PI*0.02f			// 回転速度
#define PLAYER_SCALE	0.5f
#define BULLET_COOLDOWN 5.0f
#define INITIAL_HEALTH 3
#define GRAVITY_FORCE  0.35f
#define JUMP_FORCE  6
#define MAX_GRAVITY_FORCE 5.5f
#define MAX_INPUT_TIMER 25
#define MAX_ATTACKS 10
#define MAX_FLOWER_TIMER 15
#define DEBUG_ANIMATION_FRAME false
Player3D* pMainPlayer3D = nullptr;

enum PLAYER_ANIMATIONS
{
	ANIMATION_IDLE = 0,//アイドル
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
	FALLING,
	MAX_ANIMATIONS
};

PLAYER_ATTACK_MOVE stAllMoves[MAX_ATTACKS] =
{
	{"A",	 BASIC_CHAIN_A,   false, GROUND_MOVE, 490	},
	{"AA",	 BASIC_CHAIN_B,   false, GROUND_MOVE, 630	},
	{"AAA",	 BASIC_CHAIN_C,   false, GROUND_MOVE, 780	},
	{"AAAA", BASIC_CHAIN_D,	  false, GROUND_MOVE, 950	},
	{"AAAAA",BASIC_CHAIN_E,   true,  GROUND_MOVE, 1098	},
	{"A",	 AIRCOMBOA,       false, AIR_MOVE,	  1403	},
	{"AA",	 AIRCOMBOB,       false, AIR_MOVE,	  1535	},
	{"AAA",	 AIRCOMBOC,       false, AIR_MOVE,	  1669	},
	{"AAAA", AIRCOMBOD,       false, AIR_MOVE,	  1820	},
	{"AAAAA",AIRCOMBOE,       true, AIR_MOVE,	  2000	},
};
float fAnimationSpeeds[MAX_ANIMATIONS] =//アニメーションの速さ
{
	{2},//IDLE
	{3},//WALKING
	{1},//AIMING
	{3.8f},//BASIC_CHAIN_A
	{3.15f},//BASIC_CHAIN_B
	{3.25f},//BASIC_CHAIN_C
	{3.25f},//BASIC_CHAIN_D
	{3.25f},//BASIC_CHAIN_E
	{1},//SLIDE,
	{1},//SLIDEKICK,
	{2.2f},//AIRCOMBOA,
	{2.3f},//AIRCOMBOB,
	{2.3f},//AIRCOMBOC,
	{2.3f},//AIRCOMBOD,
	{2.8f},//AIRCOMBOE,
	{2},//JUMP
	{1},
	{1},
	{1},
	{1},
	{1},//SLOWWALK
	{1},//FALLING
};

Player3D::Player3D() :GameObject3D(GetMainLight(), PLAYER_MODEL_PATH, GO_PLAYER)
{
	Init();
	pLight = GetMainLight();
}

Player3D::Player3D(Light3D * Light) :GameObject3D(Light, PLAYER_MODEL_PATH, GO_PLAYER)
{
	Init();
}


Player3D::~Player3D()
{
	End();
}

void Player3D::Init()
{
	mShadow = nullptr;
	pFloor = nullptr;
	fAtkAcceleration = 0;
	CurrentAttackPlaying = nullptr;
	bSwitcheToAimingState = false; 
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = nullptr;
	}

	strcpy(szInputs, "********");
	SetScale(PLAYER_SCALE);
	pMainPlayer3D = this;
	nState = PLAYER_IDLE_STATE;
	nMaxHealth = nCurrentHealth = INITIAL_HEALTH;
	nType = GO_PLAYER;
	nShootCooldown = 0;
	fY_force = 0;
	pDeviceContext = GetDeviceContext();
	pCurrentWindow = GetMainWindow();
	mShadow = new GameObject3D("data/model/Shadow.fbx", GO_SHADOW);
	mShadow->SetRotation({ 0,0,0 });
	mShadow->SetParent(this);
	pDebugAim = new DebugAim();
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = new GameObject3D(GO_BULLET);
		goBullets[i]->SetUse(false);
		goBullets[i]->SetHitbox({ 0,0,0,2,2,2 });
	}
	InitPlayerHitboxes();
	InitFlowers();
}
void Player3D::InitPlayerHitboxes()
{
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = nullptr;
	}
	Hitboxes[PLAYER_HB_FEET] = { 0,5,0,5,5,5 };
#if USE_HITBOX
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = new Cube3D();
		pVisualHitboxes[i]->Init("data/texture/hbox.tga");
		pVisualHitboxes[i]->SetScale({ Hitboxes[i].SizeX, Hitboxes[i].SizeY, Hitboxes[i].SizeZ });
		pVisualHitboxes[i]->SetPosition({ Hitboxes[i].PositionX,Hitboxes[i].PositionY,Hitboxes[i].PositionZ });
	}
#endif
	DebugAimOn = false;
}
void Player3D::Update()
{
	GameObject3D::Update();
	if (!pMainCamera) {
		pMainCamera = GetMainCamera();
		return;
	}
	if (GetInput(INPUT_DEBUG_AIM_ON))
	{
		if (!DebugAimOn) {
			pMainCamera->SetFocalPointGO(pDebugAim);
			DebugAimOn = true;
		}
		else 
		{
			pMainCamera->SetFocalPointGO(this);
			DebugAimOn = false;
		}
	}
	if (DebugAimOn)
	{
		pDebugAim->Update();
		return;
	}
	pDebugAim->SetPosition({ Position.x, Position.y+10, Position.z });
	AttackInputsControl();
	GravityControl(true);

	if (GetInput(INPUT_JUMP) && IsOnTheFloor())
	{
		while (IsInCollision3D(pFloor->GetHitbox(), GetHitboxPlayer(PLAYER_HB_FEET)))
			Position.y++;
		ResetInputs();
		Jump(JUMP_FORCE);
	}
	static int NumTest = 0;
	switch (nState)
	{
	case PLAYER_IDLE_STATE:
		if (IsOnTheFloor())
			SetPlayerAnimation(ANIMATION_IDLE);
		else {
			if(fY_force<0)
				SetPlayerAnimation(AIR_IDLE);
			else
				SetPlayerAnimation(FALLING);
		}
		if (GetInput(INPUT_FORWARD) || GetInput(INPUT_BACKWARD) || GetInput(INPUT_RIGHT) || GetInput(INPUT_LEFT) || GetInput(INPUT_AIM)
			|| GetAxis(MOVEMENT_AXIS_VERTICAL) != 0 || GetAxis(MOVEMENT_AXIS_HORIZONTAL) != 0)
			nState = PLAYER_MOVING_STATE;
		break;
	case PLAYER_MOVING_STATE:
		MoveControl();
		break;
	case PLAYER_ATTACKING_STATE:
		if (!CurrentAttackPlaying) {
			nState = PLAYER_IDLE_STATE;
			break;
		}
		PlayerAttackingControl();
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
	UpdateFlowers();
}

void Player3D::AttackInputsControl()
{
	if (CurrentAttackPlaying)
	{
		if (nState == PLAYER_ATTACKING_STATE && Model->GetCurrentFrame() < CurrentAttackPlaying->UnlockFrame)
			return;
	}
	if (strcmp(szInputs, "********")) {
		if (++nInputTimer > MAX_INPUT_TIMER)
		{
			if (GetLastInputInserted() != 'P')
			{
				nInputTimer = (MAX_INPUT_TIMER / 2);
				AddInput('P');
			}
			else {
				ResetInputs();
			}
			nInputTimer = 0;
		}
	}

	if (GetInput(INPUT_ATTACK) && !IsPlayerAiming())
	{
		nInputTimer = 0;
		AddInput('A');
		Attack(szInputs);
	}
}

void Player3D::PlayerAttackingControl()
{
	XMFLOAT3 ModelRot;
	ModelRot = Model->GetRotation();
	SetPlayerAnimation(CurrentAttackPlaying->Animation);
	int nCurrentFrame = Model->GetCurrentFrame();
	static float AttackDistanceAcceleration = 0;
	
	switch (CurrentAttackPlaying->Animation)
	{
	case BASIC_CHAIN_A:
		GravityControl(false);
		AttackDistanceAcceleration = 0.5f;
		if (nCurrentFrame > 430 && nCurrentFrame < 460) {
			if (fAtkAcceleration<2.5f)
				fAtkAcceleration += AttackDistanceAcceleration;
		}
		else
		{
			if(fAtkAcceleration>0)
				fAtkAcceleration -= AttackDistanceAcceleration;
			AttackDistanceAcceleration = 0;
		}										 
		Position.x -= sinf(XM_PI + ModelRot.y) * fAtkAcceleration;
		Position.z -= cosf(XM_PI + ModelRot.y) * fAtkAcceleration;
		break;
	case BASIC_CHAIN_B:
		GravityControl(false);
		AttackDistanceAcceleration = 0.5f;
		if (nCurrentFrame > 576 && nCurrentFrame < 590) {
			if (fAtkAcceleration < 4.5f)
				fAtkAcceleration += AttackDistanceAcceleration;
		}
		else
		{
			if (fAtkAcceleration > 0)
				fAtkAcceleration -= AttackDistanceAcceleration;
			AttackDistanceAcceleration = 0;
		}
		Position.x -= sinf(XM_PI + ModelRot.y) * fAtkAcceleration;
		Position.z -= cosf(XM_PI + ModelRot.y) * fAtkAcceleration;
		break;
	case BASIC_CHAIN_C:
		GravityControl(false);
		AttackDistanceAcceleration = 0.75f;
		if (nCurrentFrame > 755 && nCurrentFrame < 775) {
			if (fAtkAcceleration < 5.0f)
				fAtkAcceleration += AttackDistanceAcceleration;
		}
		else
		{
			if (fAtkAcceleration > 0)
				fAtkAcceleration -= AttackDistanceAcceleration;
			AttackDistanceAcceleration = 0;
		}
		Position.x -= sinf(XM_PI + ModelRot.y) * fAtkAcceleration;
		Position.z -= cosf(XM_PI + ModelRot.y) * fAtkAcceleration;
	
		break;
	default:
		break;
	}
	if (Model->GetLoops() >= 1) {
		CurrentAttackPlaying = nullptr;
		nState = PLAYER_IDLE_STATE;
		Update();
		return;
	}
#if DEBUG_ANIMATION_FRAME
	printf("FRAME: %d\n", Model->GetCurrentFrame());
#endif
}

void Player3D::Jump(float jumpforce)
{
	fY_force = -jumpforce;
	pFloor = nullptr;
	nState = PLAYER_IDLE_STATE;
	CurrentAttackPlaying = nullptr;
}

void Player3D::GravityControl(bool bCountAttackState)
{
	if (nState == PLAYER_ATTACKING_STATE && bCountAttackState)
		return;
	if (pFloor) {
		bool bCurrentfloorcol = IsInCollision3D(pFloor->GetHitbox(), GetHitboxPlayer(PLAYER_HB_FEET));
		if (!bCurrentfloorcol) {

			pFloor = nullptr;
			fY_force = 0;
		}
		return;
	}
	if (fY_force < 0) {
		fY_force += GRAVITY_FORCE;
	}
	else {
		if (!IsPlayerAiming())
			fY_force += GRAVITY_FORCE;
		else
			fY_force += GRAVITY_FORCE * 0.025f;
	}
	if (fY_force > MAX_GRAVITY_FORCE)
		fY_force = MAX_GRAVITY_FORCE;
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
	//スティック
	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
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
		Model->SetRotationY(nModelRotation + Rotation.y);
		if (!GetInput(INPUT_AIM)) {
			if (IsOnTheFloor()) {
				if (fVerticalAxis < 0.2f && fHorizontalAxis < 0.2f &&
					fVerticalAxis > -0.2f && fHorizontalAxis > -0.2f)
					SetPlayerAnimation(SLOWWALK);
				else
					SetPlayerAnimation(ANIMATION_WALKING);
			}
			else
			{
				if (fY_force < 0)
					SetPlayerAnimation(AIR_IDLE);
				else
					SetPlayerAnimation(FALLING);
			}
		}
	}
	static int nFlowerTimer = 0;
	if (++nFlowerTimer > MAX_FLOWER_TIMER && IsOnTheFloor()) {
		SetFlower({ Position.x,Position.y-1.2f,Position.z });
		nFlowerTimer = 0;
	}
}

void Player3D::SetPlayerAnimation(int Animation)
{
	Model->SwitchAnimation(Animation, 0, fAnimationSpeeds[Animation]);
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
		CurrentAttackPlaying = nullptr;
		bSwitcheToAimingState = true;

	}
	if (GetInput(INPUT_SHOOT) && GetInput(INPUT_AIM) && ++nShootCooldown >= BULLET_COOLDOWN) {
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
			float fYOffset = 14 * (sinf(Rotation.x)*cosf(XM_PI + Rotation.x) + (cosf(Rotation.x)*sinf(XM_PI + Rotation.x)) + 1);
			XMFLOAT3 BulPos = { Position.x + sinf(rotCamera.y) * 15, Position.y + fYOffset, Position.z + cosf(rotCamera.y) * 15 };
			goBullets[i]->SetPosition(BulPos);
			BulPos.y -= 2;
			BulPos.x += 4;
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
	if(DebugAimOn)
		pDebugAim->Draw();
	DrawFlowers();
	
	GameObject3D::Draw();
	pDeviceContext->RSSetState(pCurrentWindow->GetRasterizerState(2));
	if (mShadow)
		mShadow->Draw();
	pLight->SetLightEnable(false);
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!goBullets[i])
			continue;
		goBullets[i]->Draw();
	}
	pLight->SetLightEnable(true);
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
	return { Hitboxes[hb].PositionX + Position.x, Hitboxes[hb].PositionY + Position.y,Hitboxes[hb].PositionZ + Position.z, Hitboxes[hb].SizeX,Hitboxes[hb].SizeY,Hitboxes[hb].SizeZ };
}

void Player3D::SetFloor(Field3D * Floor)
{
	pFloor = Floor;
}

bool Player3D::IsOnTheFloor()
{
	return pFloor != nullptr;
}


void Player3D::AddInput(char A)
{
	if (A == 'S' && szInputs[0] == '*')
		return;
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (szInputs[i] == '*') {
			szInputs[i] = A;
			return;
		}
	}
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (i != MAX_PLAYER_INPUT - 1)
		{
			szInputs[i] = szInputs[i + 1];
		}
		else {
			szInputs[i] = A;
		}
	}
}

char Player3D::GetLastInputInserted()
{
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (szInputs[i] == '*')
			return szInputs[i - 1];
	}
	return szInputs[MAX_PLAYER_INPUT - 1];
}

void Player3D::ResetInputs()
{
	strcpy(szInputs, "********");

}


void Player3D::Attack(const char * atkInput)
{
	char szAtkInput[MAX_PLAYER_INPUT + 1];
	int i = 0;
	for (i = 0; i < MAX_PLAYER_INPUT && atkInput[i] != '*'; szAtkInput[i] = atkInput[i], i++);
	szAtkInput[i] = '\0';
	Attack(szAtkInput, MAX_PLAYER_INPUT);
}

void Player3D::Attack(const char * atkInput, int recursions)
{
	if (recursions <= 0)
		return;
	for (int i = 0; i < MAX_ATTACKS; i++)
	{
		if (stAllMoves[i].eAirMove == GROUND_MOVE && !IsOnTheFloor()
			|| stAllMoves[i].eAirMove == AIR_MOVE && IsOnTheFloor())
			continue;
		if (!strcmp(stAllMoves[i].Input, atkInput))
		{
			fAtkAcceleration = 0;
			//printf("FOUND AT: %d\n", i);
			CurrentAttackPlaying = &stAllMoves[i];
			nState = PLAYER_ATTACKING_STATE;
			if (stAllMoves[i].ResetInputs)
				ResetInputs();
			return;
		}
	}
	char szAtkInput[MAX_PLAYER_INPUT + 1];
	int i = 0;
	for (i = 1; i < MAX_PLAYER_INPUT && atkInput[i] != '\0'; szAtkInput[i - 1] = atkInput[i], i++);
	szAtkInput[i - 1] = '\0';
	Attack(szAtkInput, recursions - 1);
}

void Player3D::InitFlowers()
{
	CreateTextureFromFile(GetDevice(), "data/texture/Flower.tga", &pFlowerTexture);
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		
		FlowersTemp[i] = new Billboard2D(pFlowerTexture);
		FlowersTemp[i]->SetColor({ 1, 1, 1, 1 });
		FlowersTemp[i]->SetUVFrames(20, 1);
		FlowersTemp[i]->SetColor({ 1, 1, 1, 1 });
		FlowersTemp[i]->SetVertex(10/1.5f,11/1.5f);
		FlowersTemp[i]->SetUse(false);
		FlowersTemp[i]->SetUnusableAfterAnimation(true);
	}
}

void Player3D::UpdateFlowers()
{
	static float ScaleAcceleration[MAX_FLOWERS] = { 0 };
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		if (!FlowersTemp[i])
			continue;
		if (!FlowersTemp[i]->GetUse()) {
			ScaleAcceleration[i] = 0;
			continue;
		}
		FlowersTemp[i]->Update();
	}
}

void Player3D::DrawFlowers()
{
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		if (FlowersTemp[i])
			FlowersTemp[i]->Draw();
	}
}

void Player3D::SetFlower(XMFLOAT3 Pos)
{
	for (int i = 0; i < MAX_FLOWERS; i++)
	{
		if (FlowersTemp[i]) {
			if (!FlowersTemp[i]->GetUse()) {
				FlowersTemp[i]->SetPosition(Pos);
				FlowersTemp[i]->ResetUV();
				FlowersTemp[i]->SetUse(true);
				return;
			}
		}
	}
}

Field3D * Player3D::GetFloor()
{
	return pFloor;
}

Player3D * GetPlayer3D()
{
	return pMainPlayer3D;
}