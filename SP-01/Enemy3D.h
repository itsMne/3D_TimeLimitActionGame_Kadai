#pragma once
#include "GameObject3D.h"
class Enemy3D :
	public GameObject3D
{
private:
	GameObject3D* pFloor;
	float fY_force;
	Go_List* pGameFloors;
	void* pPlayerPointer;
	Box DetectZone;
	int nState;
	int nFaceCooldown;
public:
	Enemy3D();
	~Enemy3D();
	void Init();
	void Update();
	void FacePlayer();
	void Draw();
	void End();
	void GravityControl();
};

