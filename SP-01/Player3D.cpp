#include "Player3D.h"
#include "InputManager.h"
#include "debugproc.h"
#include "Texture.h"
#include "Enemy3D.h"
#include "S_InGame3D.h"
#define PLAYER_MODEL_PATH "data/model/NinaModel.fbx"
#define PLAYER_SPEED	1.95f					// 移動速度
#define ROTATION_SPEED	XM_PI*0.02f			// 回転速度
#define PLAYER_SCALE	0.5f
#define BULLET_COOLDOWN 5.0f
#define INITIAL_HEALTH 3
#define JUMP_FORCE  6
#define MAX_GRAVITY_FORCE 5.5f
#define MAX_INPUT_TIMER 25
#define MAX_ATTACKS 24
#define MAX_FLOWER_TIMER 15
#define DEBUG_ANIMATION_FRAME false
#define SHOW_PLAYER_HITBOX false
#define DEBUG_ADD_INPUTS false
#define DEBUG_ATTACK_INPUTS false
#define DEBUG_ANALOG_INPUTS false
#define DEBUG_FOUND_ATTACKS false
Player3D* pMainPlayer3D = nullptr;



PLAYER_ATTACK_MOVE stAllMoves[MAX_ATTACKS] =
{
	{"A",	  BASIC_CHAIN_A,   false, GROUND_MOVE, 490	},
	{"AA",	  BASIC_CHAIN_B,   false, GROUND_MOVE, 630	},
	{"AAA",	  BASIC_CHAIN_C,   false, GROUND_MOVE, 780	},
	{"AAAA",  BASIC_CHAIN_D,	  false, GROUND_MOVE, 950	},
	{"AAAAA", BASIC_CHAIN_E,   true,  GROUND_MOVE, 1098	},
	{"A",	  AIRCOMBOA,       false, AIR_MOVE,	  1403	},
	{"AA",	  AIRCOMBOB,       false, AIR_MOVE,	  1535	},
	{"AAA",	  AIRCOMBOC,       false, AIR_MOVE,	  1669	},
	{"AAAA",  AIRCOMBOD,       false, AIR_MOVE,	  1820	},
	{"AAAAA", AIRCOMBOE,       true, AIR_MOVE,	  2000	},

	{"K"  ,   KICKA,      false, GROUND_MOVE,	  2981	},
	{"KK" ,   KICKB,      false, GROUND_MOVE,	  3041	},
	{"KKK",   KICKC,      true,  GROUND_MOVE,	  3102	},
	{"AAAK",  KICKC,      true,     GROUND_MOVE,  3102	},
	{"AAK",   SLIDEKICK,      true,  GROUND_MOVE,  1319	},
	{"K",	  AIRKICKA,      false,  AIR_MOVE,	      3495	},
	{"KK",	  AIRKICKB,      false,  AIR_MOVE,	      3592	},
	{"KKK",	  AIRKICKC,      true,  AIR_MOVE,	      3660	},
	//向こうインプット
	{"BA",	  BASIC_CHAIN_A,   false, GROUND_MOVE, 490	},//TEMP
	{"FA",	  BASIC_CHAIN_A,   false, GROUND_MOVE, 490	},//TEMP
	{"FK"  ,   SLIDE,      true, GROUND_MOVE,	  1164	},
	{"BK"  ,   SLIDEKICK,      false, GROUND_MOVE,	  1319	},
	//{"BFK"  ,   KICKC,      false, GROUND_MOVE,	  3102	},
	{"FK",	  RED_HOT_KICK_DOWN,      true,  AIR_MOVE,	      3176	},//TEMP
	{"BK",	  KICK_DOWN,      true,  AIR_MOVE,	      3176	},//TEMP
};
float fAnimationSpeeds[MAX_ANIMATIONS] =//アニメーションの速さ
{
	{2},//IDLE
	{3},//WALKING
	{1},//AIMING
	{4.2f},//BASIC_CHAIN_A
	{3.55f},//BASIC_CHAIN_B
	{3.75f},//BASIC_CHAIN_C
	{3.75f},//BASIC_CHAIN_D
	{3.75f},//BASIC_CHAIN_E
	{2},//SLIDE,
	{2.95f},//SLIDEKICK,
	{2.7f},//AIRCOMBOA,
	{2.7f},//AIRCOMBOB,
	{2.7f},//AIRCOMBOC,
	{2.7f},//AIRCOMBOD,
	{3.2f},//AIRCOMBOE,
	{2},//JUMP
	{1},
	{1},
	{1},
	{1},
	{1},//SLOWWALK
	{1},//FALLING
	{3.5f},//DODGEUP
	{3.5f},//DODGEDOWN
	{0.85f},//KICK
	{0.85f},//KICK
	{0.85f},//KICK
	{1},//KICKDOWN
	{1},//DAMAGEA,
	{1},//DAMAGEB,
	{1.5f},//DAMAGEC,
	{1.5f},//,
	{1.5f},//,
	{1.5f},//,
	{1.5f},//,
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
	GameObject3D::~GameObject3D();
}

void Player3D::Init()
{
	mShadow = nullptr;
	pFloor = nullptr;
	pLockedOnEnemy = nullptr;
	pCurrentGame = nullptr;
	fAtkAcceleration = 0;
	nFramesDead = 0;
	nCancellingGravityFrames = 0;
	fBottom = 0;
	pCurrentAttackPlaying = nullptr;
	bSwitcheToAimingState = false; 
	bIsLockedBackwards = false;
	bIsLokedForward = false;
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = nullptr;
	}
	fAnalogLockInput = 0;
	strcpy(szInputs, "********");
	SetScale(PLAYER_SCALE);
	pMainPlayer3D = this;
	nState = PLAYER_IDLE_STATE;
	nMaxHealth = nCurrentHealth = INITIAL_HEALTH;
	nType = GO_PLAYER;
	nShootCooldown = 0;
	fTeleportAcceleration = fDamageAcceleration = 0;
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
	nRecoveryFrames = 0;
	InitPlayerHitboxes();
	InitFlowers();
}
void Player3D::InitPlayerHitboxes()
{
	for (int i = 0; i < PLAYER_HB_MAX; i++)
	{
		pVisualHitboxes[i] = nullptr;
		Hitboxes[i] = { 0 };
	}
	Hitboxes[PLAYER_HB_FEET] = { 0,1.5f,0,1.5f,1.5f,1.5f };
	Hitboxes[PLAYER_HB_ATTACK] = { 0,0,0,0,0,0 };
	Hitboxes[PLAYER_HB_BODY] = { 0,9.5f,0,2.5f,7,2.5f };
	Hitboxes[PLAYER_HB_HEAD] = { 0,18.5f,0,1.5f,1.5f,1.5f };
	Hitboxes[PLAYER_HB_FRONT] = { 0,9.5f,1.5f, 2.5f,7,1.5f };
	Hitboxes[PLAYER_HB_BACK] = {  0,9.5f,-1.5f,2.5f,7,1.5f };
	Hitboxes[PLAYER_HB_LEFT] = { -3,9.5f,0,1.5f,7,1.5f };
	Hitboxes[PLAYER_HB_RIGHT] = { 3,9.5f,0,1.5f,7,1.5f };
	Hitboxes[PLAYER_HB_LOCK] = { 0,9.5f,0,22.5f,7,1.5f };
#if SHOW_HITBOX && SHOW_PLAYER_HITBOX
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
	if (nState == PLAYER_FELL)
	{
		if (!CompareXmfloat3(Position, InitialPosition))
		{
			if (!CompareXmfloat3(*(Model->GetScaleAdd()), { 0,0,0 }))
			{
				fDamageAcceleration += 0.05f;
				Model->SetScale(Scale.x - fDamageAcceleration);

			}
			else {
				if (nCurrentHealth <= 0)
				{
					nState = PLAYER_DAMAGED;
					SetPlayerAnimation(DAMAGEC);
					return;
				}
				fDamageAcceleration = 0;
				fTeleportAcceleration += 0.5f;
				MoveToPos(fTeleportAcceleration, InitialPosition);
			}
		}
		else {
			if (!pCurrentGame)
				pCurrentGame = GetCurrentGame();
			S_InGame3D* pGame = (S_InGame3D*)pCurrentGame;
			if (!CompareXmfloat3(*(Model->GetScaleAdd()), { 1,1,1 }))
			{
				fDamageAcceleration += 0.05f;
				Model->SetScale(Scale.x + fDamageAcceleration);
				if (Scale.x > 1)
					Model->SetScale(1);
				if (CompareXmfloat3(*(Model->GetScaleAdd()), { 1,1,1 }))
					pGame->GetUIManager()->SetAura(DARK_AURA_TEXTURE);
			}
			else {
				nState = PLAYER_IDLE_STATE;
			}
		}
		SetPlayerAnimation(DAMAGEA);
		return;
	}
	if (nCurrentHealth <= 0)
	{
		if (Model->GetCurrentAnimation() == DAMAGEC)
		{
			if (Model->GetCurrentFrame() >= 3407) {
				Model->SetFrameOfAnimation(3407);
				nFramesDead++;
				return;
			}
			
		}
	}
	if (!pMainCamera) {
		pMainCamera = GetMainCamera();
		return;
	}
	if (nRecoveryFrames > 0)
		nRecoveryFrames--;
	GameObject3D::Update();

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
			pMainCamera->ResetOffset();
		}
	}

	if (DebugAimOn)
	{
		pDebugAim->Update();
		return;
	}
	
	if (nState == PLAYER_DAMAGED)
	{
		XMFLOAT3 ModelRot = Model->GetRotation();
		int nDamageAnimationFrame = Model->GetCurrentFrame();
		int nSpeedD = (nDamageAnimationFrame - 3321);
		if (nDamageAnimationFrame < 3321 || nDamageAnimationFrame > 3400)
			nSpeedD = 1;
		Position.x -= -sinf(XM_PI + ModelRot.y) * 0.05f*nSpeedD;
		Position.z -= -cosf(XM_PI + ModelRot.y) * 0.05f*nSpeedD;
		if (nDamageAnimationFrame < 3344)
			Model->SwitchAnimation(DAMAGEC, 0, fAnimationSpeeds[DAMAGEC]*4);
		else
			Model->SwitchAnimation(DAMAGEC, 0, fAnimationSpeeds[DAMAGEC]);
		if (Model->GetLoops() > 0)
			nState = PLAYER_IDLE_STATE;
		return;
	}
	if (!pCurrentGame)
		pCurrentGame = GetCurrentGame();
	S_InGame3D* pGame = (S_InGame3D*)pCurrentGame;
	if (GetInput(INPUT_LOCKON) && !pLockedOnEnemy && pGame && !GetInput(INPUT_AIM))
	{
		XMFLOAT3 ModelRot = GetRotation();
		Hitboxes[PLAYER_HB_LOCK].PositionX = -sinf(XM_PI + ModelRot.y) * 25;
		Hitboxes[PLAYER_HB_LOCK].PositionZ = -cosf(XM_PI + ModelRot.y) * 25;
		while (++Hitboxes[PLAYER_HB_LOCK].SizeZ < 150) {
			pLockedOnEnemy = pGame->GetList(GO_ENEMY)->CheckCollision(GetHitboxPlayer(PLAYER_HB_LOCK), true);
			if (pLockedOnEnemy) {
				//((Enemy3D*)pLockedOnEnemy)->LockEnemyToObject(this);
				break;
			}
		}
		Hitboxes[PLAYER_HB_LOCK].SizeZ = 1;
	}
	if (fBottom != 0)
	{
		if (Position.y < fBottom) {
			nState = PLAYER_FELL;
			pGame->GetUIManager()->SetAura(DARK_AURA_TEXTURE);
			nCurrentHealth--;
		}
	}
	if (!GetInput(INPUT_LOCKON) && pLockedOnEnemy) {
		pLockedOnEnemy = nullptr;
		//pMainCamera->SetFocalPointGO(this);
		pMainCamera->SetZoomLock(0);
		pMainCamera->SetYOffsetLock(0);
	}
	LockModelToObject(pLockedOnEnemy);
	if (pLockedOnEnemy)
	{
		//pMainCamera->SetFocalPointGO(pLockedOnEnemy);
		pMainCamera->SetYOffsetLock(-10);
		float flock = 80 - (pLockedOnEnemy->GetPosition().z - Position.z) - -(pLockedOnEnemy->GetPosition().y - Position.y);
		//printf("%f\n", flock);
		if(flock<0)
			pMainCamera->SetZoomLock(flock);
		else
			pMainCamera->SetZoomLock(0);
		if (flock < -150 || GetInput(INPUT_AIM) || !pLockedOnEnemy->IsInUse()) {
			pLockedOnEnemy = nullptr;
			pMainCamera->SetZoomLock(0);
			pMainCamera->SetYOffsetLock(0);
		}
	}


	pDebugAim->SetPosition({ Position.x, Position.y+10, Position.z });
	AttackInputsControl();
	bIsLockedBackwards = false;
	bIsLokedForward = false;
	GravityControl(true);
	if (!IsPlayerAiming() && nState != PLAYER_DODGE_UP && nState != PLAYER_DODGE_DOWN)
	{
		if (GetAxis(CAMERA_AXIS_VERTICAL) >= 0.7f)
			nState = PLAYER_DODGE_UP;
		if (GetAxis(CAMERA_AXIS_VERTICAL) <= -0.7)
			nState = PLAYER_DODGE_DOWN;
	}
	if (GetInput(INPUT_JUMP) && IsOnTheFloor())
	{
		while (IsInCollision3D(pFloor->GetHitbox(), GetHitboxPlayer(PLAYER_HB_FEET)))
			Position.y++;
		ResetInputs();
		Jump(JUMP_FORCE);
	}
	static int NumTest = 0;
	Hitboxes[PLAYER_HB_ATTACK] = { 0,0,0,0,0,0 };
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
		if (!pCurrentAttackPlaying) {
			nState = PLAYER_IDLE_STATE;
			break;
		}
		if(GetInput(INPUT_LOCKON))
			DirectionalInputsControl();
		PlayerAttackingControl();
		break;
	case PLAYER_DODGE_DOWN:
		SetPlayerAnimation(DODGEDOWN);
		if (GetAxis(CAMERA_AXIS_VERTICAL) <= -0.7f)
		{
			if (Model->GetCurrentFrameOfAnimation() > 2893 && Model->GetCurrentFrameOfAnimation() < 2900)
				Model->SetFrameOfAnimation(2890);
			break;
		}
		if(Model->GetLoops() > 0)
			nState = PLAYER_IDLE_STATE;
		break;
	case PLAYER_DODGE_UP:
		SetPlayerAnimation(DODGEUP);
		if (GetAxis(CAMERA_AXIS_VERTICAL) >= 0.7f && Model->GetCurrentFrameOfAnimation() < 2780)
		{
			if (Model->GetCurrentFrameOfAnimation() > 2775)
				Model->SetFrameOfAnimation(2771);
			break;
		}
		if (Model->GetLoops() > 0)
			nState = PLAYER_IDLE_STATE;
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
	if (pCurrentAttackPlaying)
	{
		if (nState == PLAYER_ATTACKING_STATE && Model->GetCurrentFrame() < pCurrentAttackPlaying->UnlockFrame)
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
				fAnalogLockInput = 0;
			}
			nInputTimer = 0;
		}
	}
	
	if (GetInput(INPUT_SWORD) && !IsPlayerAiming())
	{
		nInputTimer = 0;
		AddInput('A');
		Attack(szInputs);
	}
	if (GetInput(INPUT_KICK) && !IsPlayerAiming())
	{
		nInputTimer = 0;
		AddInput('K');
		Attack(szInputs);
	}
}

void Player3D::PlayerAttackingControl()
{
	XMFLOAT3 ModelRot;
	ModelRot = Model->GetRotation();
	SetPlayerAnimation(pCurrentAttackPlaying->Animation);
	int nCurrentFrame = Model->GetCurrentFrame();
	static float AttackDistanceAcceleration = 0;
	static int nCountFrame = 0;
	float AttackHitboxDistance = 10;
	switch (pCurrentAttackPlaying->Animation)
	{
	case AIRKICKA:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 3478 && nCurrentFrame < 3490)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case AIRKICKB:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 3549 && nCurrentFrame < 3563)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case AIRKICKC:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 3626 && nCurrentFrame < 3638)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case RED_HOT_KICK_DOWN:
		if (nCurrentFrame < 3669) {
			fY_force = JUMP_FORCE;
			break;
		}
		pFloor = nullptr;
		AttackHitboxDistance = 7;
		if (pLockedOnEnemy)
		{
			fY_force += GRAVITY_FORCE * 0.5f;
			if (fY_force > MAX_GRAVITY_FORCE*0.5f)
				fY_force = MAX_GRAVITY_FORCE*0.5f;
			Position.y -= fY_force;
			float MoveX = -sinf(XM_PI + ModelRot.y) * 17;
			float MoveZ = -cosf(XM_PI + ModelRot.y) * 17;
			Position.x += MoveX;
			Position.z += MoveZ;

			if (MoveX < 0 && Position.x < pLockedOnEnemy->GetPosition().x)
				Position.x = pLockedOnEnemy->GetPosition().x;
			else if (MoveX > 0 && Position.x > pLockedOnEnemy->GetPosition().x)
				Position.x = pLockedOnEnemy->GetPosition().x;
			if (MoveZ < 0 && Position.z < pLockedOnEnemy->GetPosition().z)
				Position.z = pLockedOnEnemy->GetPosition().z;
			else if (MoveZ > 0 && Position.z > pLockedOnEnemy->GetPosition().z)
				Position.z = pLockedOnEnemy->GetPosition().z;

		}
		else {
			fY_force += GRAVITY_FORCE;
			if (fY_force > MAX_GRAVITY_FORCE)
				fY_force = MAX_GRAVITY_FORCE;
			Position.y -= fY_force;
			Position.x -= sinf(XM_PI + ModelRot.y) * 17;
			Position.z -= cosf(XM_PI + ModelRot.y) * 17;
		}
		if (nCurrentFrame > 3740)
			Model->SetFrameOfAnimation(3674);
		Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case SLIDEKICK:
		if (nCurrentFrame < 1221) {
			fY_force = -JUMP_FORCE;
			break;
		}
		pFloor = nullptr;
		fY_force += GRAVITY_FORCE;
		if (fY_force > 0)
			fY_force = 0;
		Position.y -= fY_force;
		if (nCurrentFrame > 1311)
			fY_force = 0;
		AttackHitboxDistance = 7;
		Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		//nCancellingGravityFrames = 10;
		break;
	case SLIDE:
		if (nCurrentFrame > 1122 && nCurrentFrame < 1164)
		{
			Position.x -= sinf(XM_PI + ModelRot.y) * 6;
			Position.z -= cosf(XM_PI + ModelRot.y) * 6;
			AttackHitboxDistance = 5;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,6.5f };
		}
		break;
	case BASIC_CHAIN_A:
		GravityControl(false);
		AttackDistanceAcceleration = 0.125f;
		if (nCurrentFrame > 430 && nCurrentFrame < 460) {
			if (fAtkAcceleration<2.5f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
		AttackDistanceAcceleration = 0.125f;
		if (nCurrentFrame > 576 && nCurrentFrame < 590) {
			if (fAtkAcceleration < 4.5f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
		AttackDistanceAcceleration = 0.125f;
		if (nCurrentFrame > 755 && nCurrentFrame < 775) {
			if (fAtkAcceleration < 5.0f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
	case BASIC_CHAIN_D:
		GravityControl(false);
		AttackDistanceAcceleration = 0.0625f;
		if (nCurrentFrame > 855 && nCurrentFrame < 894) {
			if (fAtkAcceleration < 5.0f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
	case BASIC_CHAIN_E:
		GravityControl(false);
		AttackDistanceAcceleration = 0.0625f;
		if (nCurrentFrame > 1008 && nCurrentFrame < 1018) {
			if (fAtkAcceleration < 5.0f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
	case KICKA:
		GravityControl(false);
		AttackDistanceAcceleration = 0.125f;
		if (nCurrentFrame > 2966 && nCurrentFrame < 2973) {
			if (fAtkAcceleration < 5.0f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
	case KICKB:
		GravityControl(false);
		AttackDistanceAcceleration = 0.0625f;
		if (nCurrentFrame > 3014 && nCurrentFrame < 3030) {
			if (fAtkAcceleration < 5.0f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
	case KICKC:
		GravityControl(false);
		AttackDistanceAcceleration = 0.125f;
		if (nCurrentFrame > 3075 && nCurrentFrame < 3088) {
			if (fAtkAcceleration < 5.0f)
				fAtkAcceleration += AttackDistanceAcceleration;
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
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
	case KICK_DOWN:
		Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,9.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,8.5f };
		if (++nCountFrame>8)
			GravityControl(false);
		else
			fY_force = JUMP_FORCE;
		AttackDistanceAcceleration = 0.125f;
		if (nCurrentFrame > 3166) {
			Model->SetFrameOfAnimation(3116);
		}
		if (pFloor)
		{
			pCurrentAttackPlaying = nullptr;
			nState = PLAYER_IDLE_STATE;
			nCountFrame = 0;
			Update();
			return;
		}
		//Position.x -= sinf(XM_PI + ModelRot.y) * fAtkAcceleration;
		//Position.z -= cosf(XM_PI + ModelRot.y) * fAtkAcceleration;
		break;
	case AIRCOMBOA:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 1394 && nCurrentFrame < 1412)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case AIRCOMBOB:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 1520 && nCurrentFrame < 1538)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case AIRCOMBOC:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 1647 && nCurrentFrame < 1664)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case AIRCOMBOD:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 1792 && nCurrentFrame < 1806)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	case AIRCOMBOE:
		nCancellingGravityFrames = 10;
		if (nCurrentFrame > 1954 && nCurrentFrame < 1973)
			Hitboxes[PLAYER_HB_ATTACK] = { -sinf(XM_PI + ModelRot.y) * AttackHitboxDistance,11.5f,-cosf(XM_PI + ModelRot.y) * AttackHitboxDistance,7.5f,7,12.5f };
		break;
	default:
		break;
	}
	if (Model->GetLoops() >= 1) {
		CancelAttack();
		Update();
		return;
	}
#if DEBUG_ANIMATION_FRAME
	printf("FRAME: %d\n", Model->GetCurrentFrame());
#endif
}

void Player3D::CancelAttack()
{
	pCurrentAttackPlaying = nullptr;
	nState = PLAYER_IDLE_STATE;
}

void Player3D::Jump(float jumpforce)
{
	fY_force = -jumpforce;
	pFloor = nullptr;
	nState = PLAYER_IDLE_STATE;
	pCurrentAttackPlaying = nullptr;
}

void Player3D::GravityControl(bool bCountAttackState)
{
	if (nState == PLAYER_ATTACKING_STATE && bCountAttackState)
		return;
	if (--nCancellingGravityFrames > 0)
		return;
	nCancellingGravityFrames = 0;
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
	if (GetInput(INPUT_LOCKON))
		return;
	if (GetAxis(CAMERA_AXIS_HORIZONTAL) != 0)RotateAroundY(-ROTATION_SPEED * GetAxis(CAMERA_AXIS_HORIZONTAL));
	if (GetAxis(CAMERA_AXIS_VERTICAL) != 0 && IsPlayerAiming())
	{
		RotateAroundX(ROTATION_SPEED * GetAxis(CAMERA_AXIS_VERTICAL));
		if (Rotation.x > 0.816814f)
			Rotation.x = 0.816814f;
		if (Rotation.x < -0.628319f)
			Rotation.x = -0.628319f;
	}

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
	//printf("ANALOG ROT: %f\n", nModelRotation);
	//printf("MODEL ROT: %f\n", Model->GetRotation().y);

	if (GetInput(INPUT_LOCKON))
	{
		if (pLockedOnEnemy) {
			if (fVerticalAxis > 0.19f && pLockedOnEnemy->GetPosition().z < Position.z)
				fVerticalAxis = 0.19f;
			else if (fVerticalAxis < -0.19f && pLockedOnEnemy->GetPosition().z>Position.z)
				fVerticalAxis = -0.19f;
		}
		else {
			if (fVerticalAxis > 0.19f)
				fVerticalAxis = 0.19f;
			else if (fVerticalAxis < -0.19f)
				fVerticalAxis = -0.19f;
		}
		if (fHorizontalAxis > 0.19f)
			fHorizontalAxis = 0.19f;
		else if (fHorizontalAxis < -0.19f)
			fHorizontalAxis = -0.19f;
	}
	if (GetInput(INPUT_LOCKON) && pLockedOnEnemy)
	{
		if (fVerticalAxis > 0.19f)
			fVerticalAxis = 0.19f;
		else if (fVerticalAxis < -0.19f)
			fVerticalAxis = -0.19f;
	}
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
		if (!GetInput(INPUT_LOCKON))
			Model->SetRotationY(nModelRotation + Rotation.y);
		else
		{
			DirectionalInputsControl();
			
		}
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

void Player3D::DirectionalInputsControl()
{
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
	if (fHorizontalAxis == 0 && fVerticalAxis == 0)
		return;
	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	float abs = Model->GetRotation().y - nModelRotation - GetMainCamera()->GetCameraAngle().y;
	if (abs < 0)
		abs *= -1;
	float fDifferenceChangeInFrame = fAnalogLockInput - abs;
	if (fDifferenceChangeInFrame < 0)
		fDifferenceChangeInFrame *= -1;
#if DEBUG_ANALOG_INPUTS
	printf("DIF: %f\n", fAnalogLockInput);
#endif
	fAnalogLockInput = abs;
	if (fAnalogLockInput > XM_PI)
		fAnalogLockInput -= XM_2PI;
	if (fAnalogLockInput < 1)
	{
		bIsLokedForward = true;
	}
	else if (fAnalogLockInput > 2)
	{
		bIsLockedBackwards = true;
	}
	if (fDifferenceChangeInFrame > 1.5f)
	{
		//変化があった
		nInputTimer = (MAX_INPUT_TIMER / 2);
		if (fAnalogLockInput < 1)
		{
			AddInput('F');
		}
		else if (fAnalogLockInput>2)
		{
			AddInput('B');
		}
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
	if(pFloor)
		mShadow->SetPosition({ Position.x,pFloor->GetPosition().y,Position.z });
	else
		mShadow->SetPosition({ Position.x,1,Position.z });
}

void Player3D::PlayerBulletsControl()
{
	if (GetInput(INPUT_AIM))
	{
		GetModel()->SetRotation(Rotation);
		SetPlayerAnimation(ANIMATION_AIMING);
		pCurrentAttackPlaying = nullptr;
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
	if (nRecoveryFrames>0)
	{
		if (nRecoveryFrames % 2 == 0)
			return;
	}
#if SHOW_HITBOX && SHOW_PLAYER_HITBOX
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
	if(DebugAimOn)
		pDebugAim->Draw();
	DrawFlowers();
	
	GameObject3D::Draw();
	pDeviceContext->RSSetState(pCurrentWindow->GetRasterizerState(2));
	pLight->SetLightEnable(false);
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!goBullets[i])
			continue;
		goBullets[i]->Draw();
	}
	SetCullMode(CULLMODE_NONE);
	if (mShadow)
		mShadow->Draw();
	pLight->SetLightEnable(true);

	
}

void Player3D::End()
{
	pMainPlayer3D = nullptr;
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

void Player3D::SetFloor(GameObject3D* Floor)
{
	pFloor = Floor;
}

bool Player3D::IsOnTheFloor()
{
	return pFloor != nullptr;
}


void Player3D::AddInput(char A)
{
	if (A == 'P' && szInputs[0] == '*')
		return;
	if (A == 'P')
		fAnalogLockInput = 0;
	for (int i = 0; i < MAX_PLAYER_INPUT; i++)
	{
		if (szInputs[i] == '*') {
			szInputs[i] = A;
#if DEBUG_ADD_INPUTS
			printf("INPUTS: %s\n", szInputs);
#endif
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
#if DEBUG_ADD_INPUTS
			printf("INPUTS: %s\n", szInputs);
#endif
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

#if DEBUG_ATTACK_INPUTS
	printf("%s\n", atkInput);
#endif
	Attack(szAtkInput, MAX_PLAYER_INPUT);
}

void Player3D::Attack(const char * atkInput, int recursions)
{
	if (recursions <= 0)
		return;
	char AtkInputComp[MAX_PLAYER_INPUT + 1];
	strcpy(AtkInputComp, atkInput);
	static bool bLastCheck = false;
	if (recursions == MAX_PLAYER_INPUT)
		bLastCheck = false;
	if (bIsLockedBackwards && !strcmp(AtkInputComp, "A") && !bLastCheck) {
		strcpy(AtkInputComp, "BA");
		bLastCheck = true;
	}
	if (bIsLokedForward && !strcmp(AtkInputComp, "A") && !bLastCheck) {
		strcpy(AtkInputComp, "FA");
		bLastCheck = true;
	}
	if (bIsLockedBackwards && !strcmp(AtkInputComp, "K") && !bLastCheck) {
		strcpy(AtkInputComp, "BK");
		bLastCheck = true;
	}
	if (bIsLokedForward && !strcmp(AtkInputComp, "K") && !bLastCheck) {
		strcpy(AtkInputComp, "FK");
		bLastCheck = true;
	}

	for (int i = 0; i < MAX_ATTACKS; i++)
	{
		if (stAllMoves[i].eAirMove == GROUND_MOVE && !IsOnTheFloor()
			|| stAllMoves[i].eAirMove == AIR_MOVE && IsOnTheFloor())
			continue;
		if (!strcmp(stAllMoves[i].Input, AtkInputComp))
		{
			fAtkAcceleration = 0;
#if DEBUG_FOUND_ATTACKS
			printf("FOUND AT: %d\n", i);
#endif
			pCurrentAttackPlaying = &stAllMoves[i];
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

float Player3D::GetYForce()
{
	return fY_force;
}

void Player3D::LockModelToObject(GameObject3D * lock)
{
	if (!lock)
		return;
	if (pCurrentAttackPlaying)
	{
		if (pCurrentAttackPlaying->Animation == RED_HOT_KICK_DOWN)
			return;
	}
	XMFLOAT3 a;
	XMFLOAT3 calc = GetVectorDifference(lock->GetPosition(), Position);
	a.x = sin(GetRotation().y+ GetMainCamera()->GetCameraAngle().y);
	a.y = sin(GetRotation().x + GetMainCamera()->GetCameraAngle().x);
	a.z = cos(GetRotation().y+ GetMainCamera()->GetCameraAngle().y);
	XMFLOAT3 b = NormalizeVector(calc);
	XMVECTOR dot = XMVector3Dot(XMLoadFloat3(&a), XMLoadFloat3(&b));


	float rotationAngle = (float)acos(XMVectorGetX(dot));
	rotationAngle = ceilf(rotationAngle * 10) / 10;
	if (lock->GetPosition().x < Position.x) {
		Model->SetRotationY(-rotationAngle);
		//Rotation = Model->GetRotation();
	}
	else {
		Model->SetRotationY(rotationAngle);
		//Rotation = Model->GetRotation();
	}
}

void Player3D::LockPlayerToObject(GameObject3D * lock)
{
	if (!lock)
	return;
	XMFLOAT3 a;
	XMFLOAT3 calc = GetVectorDifference(lock->GetPosition(), Position);
	//calc.y = 0;
	a.x = sin(GetRotation().y);
	a.y = sin(GetRotation().x);
	a.z = 0;
	XMFLOAT3 b = NormalizeVector(calc);
	float dot = DotProduct(a, b);
	//XMVECTOR dot = XMVector3Dot(XMLoadFloat3(&a), XMLoadFloat3(&b));


	float rotationAngle = dot;
	rotationAngle = ceilf(rotationAngle * 10) / 10;
	//printf("%f\n", -dot);

	if (lock->GetPosition().x < Position.x) {
		Rotation.y = -rotationAngle;
		//Rotation = Model->GetRotation();
	}
	else {
		Rotation.y = rotationAngle;
		//Rotation = Model->GetRotation();
	}
}

GameObject3D * Player3D::GetFloor()
{
	return pFloor;
}

PLAYER_ATTACK_MOVE * Player3D::GetCurrentAttack()
{
	return pCurrentAttackPlaying;
}

int Player3D::GetState()
{
	return nState;
}

void Player3D::SetDamage(int nDamage)
{
	if (nState == PLAYER_DAMAGED)
		return;
	if (!pCurrentGame)
		pCurrentGame = GetCurrentGame();
	S_InGame3D* pGame = (S_InGame3D*)pCurrentGame;
	pGame->GetUIManager()->SetAura(DARK_AURA_TEXTURE);
	nState = PLAYER_DAMAGED;
	nCurrentHealth -= nDamage;
}

bool Player3D::IsPlayerDead()
{
	return nFramesDead>120;
}

GameObject3D ** Player3D::GetBullets()
{
	return goBullets;
}

GameObject3D * Player3D::GetLockedOnEnemy()
{
	return pLockedOnEnemy;
}

void Player3D::SetBottom(float bot)
{
	fBottom = bot;
}

Player3D * GetPlayer3D()
{
	return pMainPlayer3D;
}