#ifndef	PLAYER3D_H
#define PLAYER3D_H
#include "GameObject3D.h"
#include "Cube3D.h"
#include "Field3D.h"
#define MAX_BULLETS 40
#define MAX_PLAYER_INPUT 8
enum EPLAYER_STATE
{
	PLAYER_IDLE_STATE=0,
	PLAYER_MOVING_STATE,
	MAX_PLAYER_STATE
};
enum EPLAYER_HITBOXES
{
	PLAYER_HB_FEET=0,
	PLAYER_HB_MAX
};
enum AirMove
{
	BOTH_MOVES = 0,
	GROUND_MOVE = 1,
	AIR_MOVE = -1
};
typedef struct PLAYER_ATTACK_MOVE
{
	char	Input[MAX_PLAYER_INPUT];
	int		Animation;
	bool	ResetInputs;
	AirMove eAirMove;
	int UnlockFrame;
};
class Player3D :
	public GameObject3D
{
private:
	int nState;
	bool bSwitcheToAimingState;
	//‰ñ•œ‚ÉŠÖ‚·‚é
	int nCurrentHealth;
	int nMaxHealth;
	//‰e‚ÉŠÖ‚·‚é
	GameObject3D* mShadow;
	//’e‚ÉŠÖ‚·‚é
	GameObject3D* goBullets[MAX_BULLETS];
	int nShootCooldown;
	//ƒŒƒ“ƒ_ƒŠƒ“ƒO‚Ìˆ×‚É
	ID3D11DeviceContext* pDeviceContext;
	DXWindow3D*			 pCurrentWindow;
	Box Hitboxes[PLAYER_HB_MAX];
	Cube3D* pVisualHitboxes[PLAYER_HB_MAX];
	Field3D* pFloor;
	float fY_force;
	char szInputs[MAX_PLAYER_INPUT + 1];
	int nInputTimer;
	PLAYER_ATTACK_MOVE* CurrentAttackPlaying;
public:
	Player3D();
	Player3D(Light3D* Light);
	~Player3D();

	void Init();
	void Update();
	void Jump(float jumpforce);
	void GravityControl();
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
	Box GetHitboxPlayer(int hb);
	void SetFloor(Field3D* Floor);
	bool IsOnTheFloor();
	void AddInput(char A);
	char GetLastInputInserted();
	void ResetInputs();
	void Attack(const char * atkInput);
	void Attack(const char * atkInput, int recursions);
	
};

Player3D*  GetMainPlayer3D();
#endif