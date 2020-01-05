#include "Enemy3D.h"
#include "Player3D.h"
#include "S_InGame3D.h"
#include "UniversalStructures.h"
#define MODEL_PATH "data/model/Enemy.fbx"
#define MAX_GRAVITY_FORCE 5.5f
enum ENEMY_STATES
{
	IDLE,
	MOVING,
	ATTACKING,
	DAMAGED
};
enum ENEMY_ANIMATIONS
{

};
SceneInGame3D* pGame = nullptr;
Enemy3D::Enemy3D(): GameObject3D()
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
	pPlayerPointer = nullptr;
	pFloor = nullptr;
	InitModel(MODEL_PATH);
	Model->SetScale(0.79f);
	Hitbox = { 0,10.5f,0, 5,11,5 };
	DetectZone = { 0,10.5f,+180, 2.5f,2.5f,180*3 };
	nState = IDLE;
	nFaceCooldown = 0;
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
	FacePlayer();
	switch (nState)
	{
	case IDLE:
		//Model->SwitchAnimation();
		break;
	default:
		break;
	}
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
