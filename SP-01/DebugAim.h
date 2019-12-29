#pragma once
#include "GameObject3D.h"

class DebugAim :
	public GameObject3D
{
private:
	int nTypeObj;
	GameObject3D* DA_Obj;
	GameObject3D* pPlayer;
	XMFLOAT3 x3dAScale;
	void* pCGame;
	Go_List* pFloors;
public:
	DebugAim();
	~DebugAim();
	void Init();
	void Update();
	void ScaleControl();
	void DebugAimControl();
	void Draw();
	void End();
};

