#include "Player3D.h"
#include "InputManager.h"
#include "debugproc.h"
#define PLAYER_MODEL_PATH "data/model/NinaModel.fbx"
#define PLAYER_SPEED	2.5f					// �ړ����x
#define ROTATION_SPEED	XM_PI*0.02f			// ��]���x
#define PLAYER_SCALE	0.5f
#define BULLET_COOLDOWN 5.0f
#define INITIAL_HEALTH 3
#define USE_HITBOX true
#define GRAVITY_FORCE  0.35f
#define JUMP_FORCE  6
#define MAX_GRAVITY_FORCE 5.5f
#define MAX_INPUT_TIMER 60
#define MAX_ATTACKS 5
Player3D* pMainPlayer3D = nullptr;
enum PLAYER_ANIMATIONS
{
	ANIMATION_IDLE=0,//�A�C�h��
	ANIMATION_WALKING,//���� 
	ANIMATION_AIMING,//�_��
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

PLAYER_ATTACK_MOVE stAllMoves[MAX_ATTACKS] =
{
	{"A",  BASIC_CHAIN_A,   false, GROUND_MOVE },
	{"AA", BASIC_CHAIN_B,   false, GROUND_MOVE },
	{"AAA",BASIC_CHAIN_C,   false, GROUND_MOVE },
	{"AAAA",BASIC_CHAIN_D,  false, GROUND_MOVE },
	{"AAAAA",BASIC_CHAIN_E, true,  GROUND_MOVE },
};
int nAnimationSpeeds[MAX_ANIMATIONS] =//�A�j���[�V�����̑���
{
	{2},//IDLE
	{3},//WALKING
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
	{3},//AIR_IDLE
	{1},
	{1},
	{1},
	{1},
	{1},//SLOWWALK
};

Player3D::Player3D():GameObject3D(GetMainLight(), PLAYER_MODEL_PATH, GO_PLAYER)
{

	Init();
	pLight = GetMainLight();
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
	mShadow = nullptr;
	pFloor = nullptr;
	CurrentAttackPlaying = nullptr;
	bSwitcheToAimingState = false;
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = nullptr;
	}
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = nullptr;
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
	
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = new GameObject3D(GO_BULLET);
		goBullets[i]->SetUse(false);
		goBullets[i]->SetHitbox({ 0,0,0,2,2,2 });
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
}

void Player3D::Update()
{
	//printf("%s\n", szInputs);
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
	if (GetInput(INPUT_ATTACK))
	{
		nInputTimer = 0;
		AddInput('A');
	}
	GameObject3D::Update();
	GravityControl();
	// �J�����̌����擾
	if (!pMainCamera)
		pMainCamera = GetMainCamera();
	if (GetInput(INPUT_JUMP) && IsOnTheFloor() && !IsPlayerAiming())
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
		if(IsOnTheFloor())
			SetPlayerAnimation(ANIMATION_IDLE);
		else
			SetPlayerAnimation(AIR_IDLE);
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

void Player3D::Jump(float jumpforce)
{
	fY_force = -jumpforce;
	pFloor = nullptr;
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
	if (fY_force < 0) {
		fY_force += GRAVITY_FORCE;
	}
	else {
		if(!IsPlayerAiming())
			fY_force += GRAVITY_FORCE;
		else
			fY_force += GRAVITY_FORCE*0.025f;
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
	//�X�e�B�b�N�F

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
			if (IsOnTheFloor()) {
				if (fVerticalAxis < 0.2f && fHorizontalAxis < 0.2f &&
					fVerticalAxis > -0.2f && fHorizontalAxis > -0.2f)
					SetPlayerAnimation(SLOWWALK);
				else
					SetPlayerAnimation(ANIMATION_WALKING);
			}else
				SetPlayerAnimation(AIR_IDLE);
		}
	}
	if (GetKeyPress(VK_RETURN)) {
		// ���Z�b�g
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
	//printf("trying: %s\n", atkInput);
	if (recursions <= 0)
		return;
	for (int i = 0; i < MAX_ATTACKS; i++)
	{
		/*if (!strcmp(stAllMoves[i].Input, atkInput))
		{
			//printf("i: %s  atk: %s\n", stAllMoves[i].Input, atkInput);
			printf("FOUND AT: %d\n", i);
			return;
		}*/
	}
	char szAtkInput[MAX_PLAYER_INPUT + 1];
	int i = 0;
	for (i = 1; i < MAX_PLAYER_INPUT && atkInput[i] != '\0'; szAtkInput[i - 1] = atkInput[i], i++);
	szAtkInput[i - 1] = '\0';
	Attack(szAtkInput, recursions - 1);
}

Player3D * GetMainPlayer3D()
{
	return pMainPlayer3D;
}
