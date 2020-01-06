#pragma once
#include "GameObject3D.h"
#include "Player3D.h"
class Enemy3D :
	public GameObject3D
{
private:
	GameObject3D* pFloor;
	float fY_force;
	Go_List* pGameFloors;
	void* pPlayerPointer;
	int   nState;
	int   nFaceCooldown;
	int   nIdleFramesCount;
	int   nIdleFramesWait;
	float fSpeed;
	PLAYER_ATTACK_MOVE* pLastAttackPlaying;
	bool bSetDamageA;
	int nSendOffFrames;
	Box hbAttackHitbox;
public:
	Enemy3D();
	~Enemy3D();
	void Init();
	void Update();
	void PlayerAttackCollision();
	void PlayerCollision();
	void SetEnemyAnimation(int Animation);
	void FacePlayer();
	void Draw();
	void End();
	void GravityControl();
	void LockEnemyToObject(GameObject3D * lock);
};
