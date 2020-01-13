#include "Enemy3D.h"
#include "S_InGame3D.h"
#include "RankManager.h"
#include "UniversalStructures.h"
#define MODEL_PATH "data/model/Enemy.fbx"
#define MAX_GRAVITY_FORCE 5.5f
#define ATTACK_HIT_DAMAGE 1
#define MAX_DIZZY_FRAMES 60*3
#define UNLIT_FRAMES_AFTER_HIT 5
#define BULLET_DAMAGE 1
#define INITIAL_HP 250
#define MAX_NUM_ENEMIES_FOLLOWING_PLAYER 4

Enemy3D* FollowingPlayer[MAX_NUM_ENEMIES_FOLLOWING_PLAYER] = {nullptr};
int nMaxFollowingPlayer = MAX_NUM_ENEMIES_FOLLOWING_PLAYER;
bool bInfiniteSpawn = true;
enum ENEMY_STATES
{
	ENEMY_IDLE,
	ENEMY_MOVING,
	ENEMY_ATTACKING_UP,
	ENEMY_ATTACKING_DOWN,
	ENEMY_DIZZY_STATE,
	ENEMY_SENDOFF,
	ENEMY_SENDUP,
	ENEMY_DAMAGED,
	ENEMY_DEAD,
	MAX_ENEMY_STATES,
};
enum ENEMY_ANIMATIONS
{
	IDLE_ANIM,
	ATTACKA_ANIM,
	ATTACKB_ANIM,
	DASHING_ANIM,
	DAMAGEDA_ANIM,
	DAMAGEDB_ANIM,
	DAMAGEDUP_ANIM,
	FALLDAMAGE_ANIM,
	DEAD_ANIM,
	SENDOFF_ANIM,
	DIZZY_ANIM,
	MAX_ANIM
};
float nEnemyAnimationSpeed[MAX_ANIM] = {
	1,//IDLE_ANIM,
	2.75f,//ATTACKA_ANIM,
	2.75f,//ATTACKB_ANIM,
	1,//DASHING_ANIM,
	4,//DAMAGEDA_ANIM,
	5,//DAMAGEDB_ANIM,
	1,//DAMAGEDUP_ANIM,
	1,//FALLDAMAGE_ANIM
	1,//DEAD_ANIM,
	1, //SENDOFF_ANIM,
	3,//DIZZY
};		
S_InGame3D* pGame = nullptr;
Enemy3D::Enemy3D(): GameObject3D(GO_ENEMY)
{
	Init();
}


Enemy3D::~Enemy3D()
{
	GameObject3D::~GameObject3D();
	pGame = nullptr;
	for (int i = 0; i < MAX_NUM_ENEMIES_FOLLOWING_PLAYER; i++)
	{
		if (this == FollowingPlayer[i])
			FollowingPlayer[i] = nullptr;
	}
	End();
}

void Enemy3D::Init()
{
	Position = { 0,0,0 };
	Scale = { 1,1,1 };
	Rotation = { 0,0,0 };
	pGameFloors = nullptr;
	pLastAttackPlaying = nullptr;
	pPlayerPointer = nullptr;
	pUIManager = nullptr;
	pFloor = nullptr;
	InitModel(ENEMY_WARRIOR_MODEL_PATH);
	//Model->SetPosition({0,-10,0});
	Model->SetScale(0.79f);
	Hitbox = { 0,10.5f,0, 5,11,5 };
	nState = ENEMY_IDLE;
	nFaceCooldown = 0;
	fDeadAcceleration = 0;
	nIdleFramesCount=nIdleFramesWait = 60;
	fSpeed = 1.5f;
	fY_force = 0;
	nSendOffFrames = 0;

	fSpeed = 0;//del
	bSetDamageA = true;
	nDizzyFrames = 0;
	fSendOffPower = 0;
	fSendOffAcceleration = 0;
	nSendOffFrames = 0;
	nSendUpGravityCancelling = 0;
	nUnlitFrames = 0;
	fMaxHP = fHP = INITIAL_HP;
	SkullMark = new GameObject3D(GO_SKULLWARNING);
	HeartMark = new GameObject3D(GO_ENEMYHEARTMARK);
	bIsOnLockOn = false;
	bUse = true;
}

void Enemy3D::Update()
{
	if (!bUse)
		return;
	GameObject3D::Update();
	if (!pGame)
	{
		pGame = GetCurrentGame();
		if (!pGame)
			return;	
	}
	if (!pPlayerPointer)
		pPlayerPointer = GetPlayer3D();
	if (!pGameFloors)
		pGameFloors = pGame->GetList(GO_FLOOR);

	if (--nUnlitFrames > 0)
		bIsUnlit = true;
	else {
		nUnlitFrames = 0;
		bIsUnlit = false;
	}
	GravityControl();
	Player3D* pPlayer = (Player3D*)pPlayerPointer;
	XMFLOAT3 xm3PlayerRotation = pPlayer->GetRotation();
	XMFLOAT3 xm3PlayerPosition = pPlayer->GetRotation();
	if (pPlayer->GetState() == PLAYER_FELL)
		return;
	if (nState == ENEMY_DEAD)
	{
		for (int i = 0; i < MAX_NUM_ENEMIES_FOLLOWING_PLAYER; i++)
		{
			if (this == FollowingPlayer[i])
				FollowingPlayer[i] = nullptr;
		}
		Model->RotateAroundY(1);
		SetScale(Scale.x - fDeadAcceleration);
		if (Scale.x == 0 || Scale.y == 0 || Scale.z == 0) {
			bUse = false;
			if (bInfiniteSpawn) {
				Init();
				Position = InitialPosition;
			}
		}
		fDeadAcceleration+=0.00125f;
		bIsUnlit = true;
		return;
	}
	float AttackHitboxDistance;
	int nAttackFrame = 0;
	if (nState != ENEMY_IDLE)
		nIdleFramesCount = 0;
	int atkState=0;
	if (nState != ENEMY_DIZZY_STATE)
		nDizzyFrames = 0;
	if (nState != ENEMY_SENDUP)
		nSendUpGravityCancelling = 0;
	if (!pUIManager)
	{
		if (pGame)
			pUIManager = pGame->GetUIManager();
	}
	if (pLastAttackPlaying)
	{
		if (pLastAttackPlaying->Animation == SLIDEKICK && pPlayer->GetModel()->GetCurrentAnimation()==SLIDEKICK)
		{
			Position.y = pPlayer->GetPosition().y;
		}
	}
	if (Position.y < pPlayer->GetBottom())
		fHP = 0;
	bIsOnLockOn = pPlayer->GetLockedOnEnemy() == this;
	if (fHP < 0)
		fHP = 0;
	if (HeartMark)
	{
		if (!bIsOnLockOn)
			HeartMark->GetModel()->SetScale(0);
		else
		{
			HeartMark->Update();
			HeartMark->SetPosition({ Position.x - sinf(XM_PI + Model->GetRotation().y) * 15, Position.y + 12.5f,  Position.z - cosf(XM_PI + Model->GetRotation().y) * 15 });
			HeartMark->GetModel()->SetRotation(Model->GetRotation());
			HeartMark->GetModel()->RotateAroundY(XM_PI / 2);
			HeartMark->SetScale(fHP / fMaxHP);
		}
	}
	switch (nState)
	{
	case ENEMY_IDLE:
		SetEnemyAnimation(IDLE_ANIM);
		if (pFloor) {
			fY_force = 0;
			while (IsInCollision3D(pFloor->GetHitbox(), GetHitbox()))
				Position.y += 0.1f;
			Position.y -= 0.1f;
		}
		for (int i = 0; i < nMaxFollowingPlayer; i++)
		{
			if (FollowingPlayer[i] == nullptr || FollowingPlayer[i] == this)
			{
				FollowingPlayer[i] = this;
				if (pPlayer->GetFloor() == pFloor && ++nIdleFramesCount > nIdleFramesWait)
					nState = ENEMY_MOVING;
				break;
			}
		}

		break;
	case ENEMY_MOVING:
		FacePlayer();
		SetEnemyAnimation(DASHING_ANIM);
		MoveTowardPos(pPlayer->GetPosition(), fSpeed);
		if (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_BODY))) 
		{
			atkState = rand() % 101;
			if(atkState>50)
				nState = ENEMY_ATTACKING_UP;
			else
				nState = ENEMY_ATTACKING_DOWN;
		}
		break;
	case ENEMY_SENDUP:
		if (fY_force > 0 && nSendUpGravityCancelling < 40)
		{
			fY_force = 0;
			pLastAttackPlaying = nullptr;
			nSendUpGravityCancelling++;
		}
		if (pFloor) {
			while (IsInCollision3D(pFloor->GetHitbox(), GetHitbox()))
				Position.y += 0.1f;
			Position.y -= 0.1f;
		}
		if (pFloor)
			nState = ENEMY_IDLE;
		break;
	case ENEMY_ATTACKING_UP:
		FacePlayer();
		if (SkullMark) {
			SkullMark->SetPosition({ Position.x - sinf(XM_PI + Model->GetRotation().y) * 10, Position.y+12.5f,  Position.z - cosf(XM_PI + Model->GetRotation().y) * 10 });
			SkullMark->Update();
		}
		SetEnemyAnimation(ATTACKA_ANIM);
		AttackHitboxDistance = 10;
		hbAttackHitbox = { Position.x-sinf(XM_PI + Model->GetRotation().y) * AttackHitboxDistance,Position.y+9.5f,Position.z-cosf(XM_PI + Model->GetRotation().y) * AttackHitboxDistance,7.5f,7,12.5f };
		nAttackFrame = Model->GetCurrentFrame();
		if (nAttackFrame > 450 && nAttackFrame < 486) {
			if (IsInCollision3D(pPlayer->GetHitboxPlayer(PLAYER_HB_BODY), hbAttackHitbox))
			{
				if (pPlayer->GetState() == PLAYER_DODGE_DOWN)
				{
					if (nAttackFrame > 478) {
						SetEnemyAnimation(DIZZY_ANIM);
						nState = ENEMY_DIZZY_STATE;
						GetMainCamera()->SetAttackZoom(40, 40);
						if (pUIManager)
							pUIManager->SetAura();
					}
				}
				else
				{
					pPlayer->SetDamage(ATTACK_HIT_DAMAGE);
				}
			}
		}
		if (Model->GetLoops() > 0)
			nState = ENEMY_IDLE;
		break;
	case ENEMY_ATTACKING_DOWN:
		FacePlayer();
		if (SkullMark) {
			SkullMark->SetPosition({ Position.x - sinf(XM_PI + Model->GetRotation().y) * 10, Position.y,  Position.z - cosf(XM_PI + Model->GetRotation().y) * 10 });
			SkullMark->Update();
		}
		SetEnemyAnimation(ATTACKB_ANIM);
		AttackHitboxDistance = 10;
		hbAttackHitbox = { Position.x - sinf(XM_PI + Model->GetRotation().y) * AttackHitboxDistance,Position.y + 9.5f,Position.z - cosf(XM_PI + Model->GetRotation().y) * AttackHitboxDistance,7.5f,7,12.5f };
		nAttackFrame = Model->GetCurrentFrame();
		if (nAttackFrame > 716 && nAttackFrame < 740) {
			if (IsInCollision3D(pPlayer->GetHitboxPlayer(PLAYER_HB_BODY), hbAttackHitbox))
			{
				if (pPlayer->GetState() == PLAYER_DODGE_UP)
				{
					if (nAttackFrame > 729) {
						SetEnemyAnimation(DIZZY_ANIM);
						nState = ENEMY_DIZZY_STATE;
						GetMainCamera()->SetAttackZoom(40, 40);
						if (pUIManager)
							pUIManager->SetAura();
					}
				}
				else
				{
					pPlayer->SetDamage(ATTACK_HIT_DAMAGE);
				}
			}
		}
		if (Model->GetLoops() > 0)
			nState = ENEMY_IDLE;
		break;
	case ENEMY_DAMAGED:
		//SetEnemyAnimation(DAMAGEDA_ANIM);
		if (Model->GetLoops() > 0) {
			nState = ENEMY_IDLE;
			pLastAttackPlaying = nullptr;
		}
		break;
	case ENEMY_DIZZY_STATE:
		if (Model->GetLoops() > 0)
			SetEnemyAnimation(DIZZY_ANIM);
		if (++nDizzyFrames > MAX_DIZZY_FRAMES)
			nState = ENEMY_IDLE;
		break;
	case ENEMY_SENDOFF:
		if (fSendOffAcceleration < fSendOffPower)
			fSendOffAcceleration+=3;
		else {
			fSendOffAcceleration = fSendOffPower;
			if (--nSendOffFrames <= 0)
				nState = ENEMY_IDLE;
		}
		Position.x -= -sinf(XM_PI + Model->GetRotation().y) * fSendOffAcceleration; 
		Position.z -= -cosf(XM_PI + Model->GetRotation().y) * fSendOffAcceleration;
		break;
	default:
		break;
	}
	PlayerCollision();
	PlayerAttackCollision();
	GameObject3D** pBullets = pPlayer->GetBullets();
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!pBullets[i])
			continue;
		if (!pBullets[i]->IsInUse())
			continue;
		if (IsInCollision3D(pBullets[i]->GetHitbox(), GetHitbox()))
		{
			fHP -= BULLET_DAMAGE;
			if (fHP <= 0)
				fHP = 1;
			pBullets[i]->SetUse(false);
			nUnlitFrames = 2;
			nGravityCancellingFrames = 10;
		}
	}
	if (fHP <= 0)
		nState = ENEMY_DEAD;
}

void Enemy3D::PlayerAttackCollision()
{
	Player3D* pPlayer = (Player3D*)pPlayerPointer;
	PLAYER_ATTACK_MOVE* pCurrentAtk = nullptr;
	if (IsInCollision3D(pPlayer->GetHitboxPlayer(PLAYER_HB_ATTACK), GetHitbox()))
	{
		pCurrentAtk = pPlayer->GetCurrentAttack();
		if (!pCurrentAtk)
			return;
		if (pLastAttackPlaying)
		{
			if (pLastAttackPlaying->Animation == pCurrentAtk->Animation)
				return;
		}
		pLastAttackPlaying = pCurrentAtk;
		Position.x;
		Position.y;
		pPlayer->LockModelToObject(this);
	}
	else {
		return;
	}
	if (nState != ENEMY_DIZZY_STATE)
		nState = ENEMY_DAMAGED;
	else
		if (pUIManager)
			pUIManager->SetAura(HEART_AURA_TEXTURE);
	else
		nDizzyFrames -= 10;
	nUnlitFrames = UNLIT_FRAMES_AFTER_HIT;
	GetMainCamera()->SetAttackZoom(30, 40);
	GetMainCamera()->SetShake(2.5f, 8);
	GetCurrentGame()->SetAtkEffect(30);
	XMFLOAT3 modelRot;

	switch (pLastAttackPlaying->Animation)
	{
	case SLIDEKICK: case RED_HOT_KICK_DOWN:
		AddMoveToRankMeter(pLastAttackPlaying->Animation, 45);
		fHP -= 9;
		pFloor = nullptr;
		SetEnemyAnimation(DAMAGEDUP_ANIM);
		nFaceCooldown = 0;
		FacePlayer();
		modelRot = pPlayer->GetModel()->GetRotation();
		nGravityCancellingFrames = 15;
		Position.x -= sinf(XM_PI + modelRot.y) * 1.0f;
		Position.z -= cosf(XM_PI + modelRot.y) * 1.0f;
		AddScoreWithRank(10);
		if (pLastAttackPlaying->Animation == RED_HOT_KICK_DOWN)
			pPlayer->CancelAttack();
		break;
	case AIRCOMBOE: case AIRKICKC:
		AddMoveToRankMeter(pLastAttackPlaying->Animation, 45);
		fHP -= 11;
		SetEnemyAnimation(DAMAGEDUP_ANIM);
		modelRot = pPlayer->GetModel()->GetRotation();
		nGravityCancellingFrames = 0;
		if (pLastAttackPlaying->Animation == AIRKICKC)
			nGravityCancellingFrames = 65;
		fY_force -= -12;
		nState = ENEMY_SENDOFF;
		if (nState == ENEMY_DIZZY_STATE)
		{
			fSendOffPower = 20;
			fSendOffAcceleration = 0;
			nSendOffFrames = 10;
			SetEnemyAnimation(SENDOFF_ANIM);
		}
		else {
			fSendOffPower = 20;
			fSendOffAcceleration = 0;
			nSendOffFrames = 2;
		}
		AddScoreWithRank(12);
		pLastAttackPlaying = nullptr;
		break;
	case KICK_DOWN:
		AddMoveToRankMeter(pLastAttackPlaying->Animation, 35);
		fHP -= 10;
		SetEnemyAnimation(DAMAGEDUP_ANIM);
		nFaceCooldown = 0;
		FacePlayer();
		nState = ENEMY_SENDUP;
		modelRot = pPlayer->GetModel()->GetRotation();
		Position.x -= sinf(XM_PI + modelRot.y) * 5;
		Position.z -= cosf(XM_PI + modelRot.y) * 5;
		if (pFloor) {
			while (IsInCollision3D(pFloor->GetHitbox(), GetHitbox()))
				Position.y += 0.1f;
			pFloor = nullptr;
			Position.y += 5;
			fY_force += -6;
		}
		else {
			nGravityCancellingFrames = 0;
			fY_force -= -12;
			nState = ENEMY_IDLE;
			Position.x -= sinf(XM_PI + modelRot.y) * 1.5f;
			Position.z -= cosf(XM_PI + modelRot.y) * 1.5f;
			pLastAttackPlaying = nullptr;
		}
		AddScoreWithRank(3);
		break;
	case KICKC:
		AddMoveToRankMeter(pLastAttackPlaying->Animation, 45);
		fHP -= 11;
		if (bSetDamageA) {
			SetEnemyAnimation(DAMAGEDA_ANIM);
			bSetDamageA = false;
		}
		else {
			SetEnemyAnimation(DAMAGEDB_ANIM);
			bSetDamageA = true;
		}
		nFaceCooldown = 0;
		FacePlayer();

		if (nState == ENEMY_DIZZY_STATE)
		{
			fSendOffPower = 20;
			fSendOffAcceleration = 0;
			nSendOffFrames = 20;
			SetEnemyAnimation(SENDOFF_ANIM);
		}
		else {
			fSendOffPower = 20;
			fSendOffAcceleration = 0;
			nSendOffFrames = 2;
		}
		nState = ENEMY_SENDOFF;
		modelRot = pPlayer->GetModel()->GetRotation();
		Position.x -= sinf(XM_PI + modelRot.y) * 5;
		Position.z -= cosf(XM_PI + modelRot.y) * 5;
		AddScoreWithRank(10);
		break;
	case BASIC_CHAIN_E:
		AddMoveToRankMeter(pLastAttackPlaying->Animation, 45);
		fHP -= 11;
		if (bSetDamageA) {
			SetEnemyAnimation(DAMAGEDA_ANIM);
			bSetDamageA = false;
		}
		else {
			SetEnemyAnimation(DAMAGEDB_ANIM);
			bSetDamageA = true;
		}
		nFaceCooldown = 0;
		FacePlayer();
		modelRot = pPlayer->GetModel()->GetRotation();

		if (nState == ENEMY_DIZZY_STATE)
		{
			fSendOffPower = 20;
			fSendOffAcceleration = 0;
			nSendOffFrames = 10;
			SetEnemyAnimation(SENDOFF_ANIM);
		}
		else {
			fSendOffPower = 20;
			fSendOffAcceleration = 0;
			nSendOffFrames = 2;
		}
		nState = ENEMY_SENDOFF;
		AddScoreWithRank(10);
		break;
	default:
		AddMoveToRankMeter(pLastAttackPlaying->Animation, 30);
		AddScoreWithRank(2);
		fHP -= 5;
		if (bSetDamageA) {
			SetEnemyAnimation(DAMAGEDA_ANIM);
			bSetDamageA = false;
		}
		else {
			SetEnemyAnimation(DAMAGEDB_ANIM);
			bSetDamageA = true;
		}
		nFaceCooldown = 0;
		FacePlayer();
		modelRot = pPlayer->GetModel()->GetRotation();
		if (pFloor) {
			Position.x -= sinf(XM_PI + modelRot.y) * 5.0f;
			Position.z -= cosf(XM_PI + modelRot.y) * 5.0f;
		}
		else {
			nGravityCancellingFrames = 15;
			if (pLastAttackPlaying->Animation == AIRKICKB)
				nGravityCancellingFrames = 30;
			Position.x -= sinf(XM_PI + modelRot.y) * 0.5f;
			Position.z -= cosf(XM_PI + modelRot.y) * 0.5f;
		}
		if (pLastAttackPlaying->Animation == SLIDE)
			pPlayer->GetModel()->SetFrameOfAnimation(1167);
		break;
	}
}

void Enemy3D::PlayerCollision()
{
	Player3D* pPlayer = (Player3D*)pPlayerPointer;
	bool bCollision = false;
	if (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FRONT))) {
		while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FRONT)))
		{
			pPlayer->TranslateZ(-0.1f);
		}
		TranslateZ(0.1f);
		bCollision = true;
	}
	if (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_BACK))) {
		while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_BACK)))
		{
			pPlayer->TranslateZ(0.1f);
		}
		TranslateZ(-0.1f);
		bCollision = true;
	}
	if ((IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_LEFT)))) {
		while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_LEFT)))
		{
			pPlayer->TranslateX(0.1f);
		}
		TranslateX(-0.1f);
	}
	if ((IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_RIGHT)))) {
		while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_RIGHT)))
		{
			pPlayer->TranslateX(-0.1f);
		}
		TranslateX(0.1f);
		bCollision = true;
	}
	if (nState == ENEMY_MOVING && bCollision) {
		int atkState = rand() % 101;
		if (atkState > 50)
			nState = ENEMY_ATTACKING_UP;
		else
			nState = ENEMY_ATTACKING_DOWN;
	}
}

void Enemy3D::SetEnemyAnimation(int Animation)
{
	Model->SwitchAnimation(Animation, 0, nEnemyAnimationSpeed[Animation]);
}

void Enemy3D::FacePlayer()
{
	if (--nFaceCooldown > 0)
		return;
	nFaceCooldown = 2;
	Player3D* pPlayer = (Player3D*)pPlayerPointer;
	XMFLOAT3 a;
	XMFLOAT3 calc = GetVectorDifference(pPlayer->GetPosition(), Position);
	a.x = sin(GetRotation().y);
	a.y = sin(GetRotation().x);
	a.z = cos(GetRotation().y);
	XMFLOAT3 b = NormalizeVector(calc);
	XMVECTOR dot = XMVector3Dot(XMLoadFloat3(&a), XMLoadFloat3(&b));


	float rotationAngle = (float)acos(XMVectorGetX(dot));
	rotationAngle = ceilf(rotationAngle * 10) / 10;
	if (pFloor == pPlayer->GetFloor())
	{
		if (pPlayer->GetPosition().x<Position.x)
			Model->SetRotationY(-rotationAngle);
		else
			Model->SetRotationY(rotationAngle);

	}
}

void Enemy3D::Draw()
{
	if (!bUse)
		return;
	GameObject3D::Draw();
	SetCullMode(CULLMODE_CW);
	if (HeartMark && bIsOnLockOn)
		HeartMark->Draw();
	if (SkullMark && (nState == ENEMY_ATTACKING_UP || nState == ENEMY_ATTACKING_DOWN))
		SkullMark->Draw();

}

void Enemy3D::End()
{
	GameObject3D::End();
}

void Enemy3D::GravityControl()
{
	if (!pGameFloors)
		return;
	if (nState == ENEMY_DAMAGED || nState == ENEMY_DIZZY_STATE)
		return;
	if (--nGravityCancellingFrames > 0)
		return;
	nGravityCancellingFrames = 0;
	if (pFloor) {
		bool bCurrentfloorcol = IsInCollision3D(pFloor->GetHitbox(), GetHitbox());
		if (!bCurrentfloorcol) {

			pFloor = nullptr;
			fY_force = 0;
		}
		return;
	}
	else {
		if(pGameFloors)
			pFloor = pGameFloors->CheckCollision(GetHitbox());
		if (pFloor) {
			while (IsInCollision3D(pFloor->GetHitbox(), GetHitbox()))
				Position.y += 0.1f;
			Position.y -= 0.1f;
		}
	}
	fY_force += GRAVITY_FORCE;
	if (fY_force > MAX_GRAVITY_FORCE)
		fY_force = MAX_GRAVITY_FORCE;
	Position.y -= fY_force;
}

void Enemy3D::LockEnemyToObject(GameObject3D * lock)
{
	if (!lock)
		return;
	XMFLOAT3 a;
	XMFLOAT3 calc = GetVectorDifference(lock->GetPosition(), Position);
	a.x = sin(GetRotation().y);
	a.y = sin(GetRotation().x);
	a.z = cos(GetRotation().y);
	XMFLOAT3 b = NormalizeVector(calc);
	XMVECTOR dot = XMVector3Dot(XMLoadFloat3(&a), XMLoadFloat3(&b));


	float rotationAngle = (float)acos(XMVectorGetX(dot));
	rotationAngle = ceilf(rotationAngle * 10) / 10;
	Rotation.y = -rotationAngle;
	if (lock->GetPosition().x < Position.x) {
		Rotation.y = rotationAngle;
		//Rotation = Model->GetRotation();
	}
	else {
		Rotation.y = -rotationAngle;
		//Rotation = Model->GetRotation();
	}
}