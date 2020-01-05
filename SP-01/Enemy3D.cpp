#include "Enemy3D.h"
#include "S_InGame3D.h"
#include "UniversalStructures.h"
#define MODEL_PATH "data/model/Enemy.fbx"
#define MAX_GRAVITY_FORCE 5.5f
enum ENEMY_STATES
{
	ENEMY_IDLE,
	ENEMY_MOVING,
	ENEMY_ATTACKING_UP,
	ENEMY_ATTACKING_DOWN,
	ENEMY_DAMAGED
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
	MAX_ANIM
};
float nEnemyAnimationSpeed[MAX_ANIM] = {
	1,//IDLE_ANIM,
	2.5f,//ATTACKA_ANIM,
	2.5f,//ATTACKB_ANIM,
	1,//DASHING_ANIM,
	5,//DAMAGEDA_ANIM,
	5,//DAMAGEDB_ANIM,
	1,//DAMAGEDUP_ANIM,
	1,//FALLDAMAGE_ANIM
	1,//DEAD_ANIM,
	1 //SENDOFF_ANIM,
};		
SceneInGame3D* pGame = nullptr;
Enemy3D::Enemy3D(): GameObject3D(GO_ENEMY)
{
	Init();
}


Enemy3D::~Enemy3D()
{
	pGame = nullptr;
	End();
}

void Enemy3D::Init()
{
	pGameFloors = nullptr;
	pLastAttackPlaying = nullptr;
	pPlayerPointer = nullptr;
	pFloor = nullptr;
	InitModel(MODEL_PATH);
	Model->SetScale(0.79f);
	Hitbox = { 0,10.5f,0, 5,11,5 };
	nState = ENEMY_IDLE;
	nFaceCooldown = 0;
	nIdleFramesCount=nIdleFramesWait = 60;
	fSpeed = 1;
	//fSpeed = 0;//del
	bSetDamageA = true;
}

void Enemy3D::Update()
{
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
	GravityControl();
	Player3D* pPlayer = (Player3D*)pPlayerPointer;
	XMFLOAT3 xm3PlayerRotation = pPlayer->GetRotation();
	XMFLOAT3 xm3PlayerPosition = pPlayer->GetRotation();
	if (nState != ENEMY_IDLE)
		nIdleFramesCount = 0;
	int atkState=0;
	switch (nState)
	{
	case ENEMY_IDLE:
		SetEnemyAnimation(IDLE_ANIM);
		if (pPlayer->GetFloor() == pFloor && ++nIdleFramesCount> nIdleFramesWait)
			nState = ENEMY_MOVING;
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
	case ENEMY_ATTACKING_UP:
		FacePlayer();
		SetEnemyAnimation(ATTACKA_ANIM);
		if (Model->GetLoops() > 0)
			nState = ENEMY_IDLE;
		break;
	case ENEMY_ATTACKING_DOWN:
		FacePlayer();
		SetEnemyAnimation(ATTACKB_ANIM);
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
	default:
		break;
	}
	PlayerCollision();

	PlayerAttackCollision();
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
	}
	else {
		return;
	}
	nState = ENEMY_DAMAGED;
	XMFLOAT3 modelRot;
	switch (pLastAttackPlaying->Animation)
	{
	default:
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
		Position = pPlayer->GetPosition();
		modelRot = Model->GetRotation();
		Position.x += sinf(XM_PI + modelRot.y) * 15;
		Position.z += cosf(XM_PI + modelRot.y) * 15;
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
	GameObject3D::Draw();
}

void Enemy3D::End()
{
	GameObject3D::End();
}

void Enemy3D::GravityControl()
{
	if (!pGameFloors)
		return;
	if (nState == ENEMY_DAMAGED)
		return;
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
