#ifndef	PLAYER3D_H
#define PLAYER3D_H
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
	int nState;
	bool bSwitcheToAimingState;
	//�񕜂Ɋւ���
	int nCurrentHealth;
	int nMaxHealth;
	//�e�Ɋւ���
	GameObject3D* mShadow;
	//�e�Ɋւ���
	GameObject3D* goBullets[MAX_BULLETS];
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
	void PlayerCameraControl();
	void MoveControl();
	void SetPlayerAnimation(int Animation);
	void SetPlayerAnimation(int Animation, int Slowness);
	void PlayerShadowControl();
	void PlayerBulletsControl();
	void Draw();
	void End();
	int GetCurrentHealth();
	int GetMaxHealth();
	bool IsPlayerAiming();
};

Player3D*  GetMainPlayer3D();
#endif