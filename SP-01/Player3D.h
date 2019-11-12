#pragma once
#include "GameObject3D.h"
#define MAX_BULLETS 40
enum EPLAYER_STATE
{
	PLAYER_IDLE_STATE=0,
	PLAYER_MOVING_STATE,
	MAX_PLAYER_STATE
};

class Player3D :
	public GameObject3D
{
private:
	GameObject3D* goBullets[MAX_BULLETS];
	GameObject3D* mShadow;
	int nShootCooldown;
	//�����_�����O�ׂ̈�
	ID3D11DeviceContext* pDeviceContext;
	DXWindow3D*			 pCurrentWindow;
public:
	Player3D();
	Player3D(Light3D* Light);
	~Player3D();

	void Init();
	void Update();
	void PlayerShadowControl();
	void PlayerBulletsControl();
	void Draw();
	void End();
};

