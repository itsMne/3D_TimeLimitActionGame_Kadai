#ifndef	PLAYER3D_H
#define PLAYER3D_H
#include "GameObject3D.h"
#include "Cube3D.h"
#include "Field3D.h"
#include "Billboard2D.h"
#include "DebugAim.h"
#define MAX_BULLETS 40
#define MAX_PLAYER_INPUT 8
#define	MAX_FLOWERS	20

enum PLAYER_ANIMATIONS
{
	ANIMATION_IDLE = 0,//ÉAÉCÉhÉã
	ANIMATION_WALKING,//ìÆÇ≠ 
	ANIMATION_AIMING,//ë_Ç§
	BASIC_CHAIN_A,
	BASIC_CHAIN_B,
	BASIC_CHAIN_C,
	BASIC_CHAIN_D,
	BASIC_CHAIN_E,
	SLIDE,
	SLIDEKICK,
	AIRCOMBOA,
	AIRCOMBOB,
	AIRCOMBOC,
	AIRCOMBOD,
	AIRCOMBOE,
	JUMP,
	AIR_IDLE,
	ROULETTE,
	STAB_BLOCK,
	AIR_STAB_BLOCK,
	SLOWWALK,
	FALLING,
	DODGEUP,
	DODGEDOWN,
	KICKA,
	KICKB,
	KICKC,
	KICK_DOWN,
	DAMAGEA,
	DAMAGEB,
	DAMAGEC,
	AIRKICKA,
	AIRKICKB,
	AIRKICKC,
	REDHOTKICKDOWN,
	MAX_ANIMATIONS
};

enum EPLAYER_STATE
{
	PLAYER_IDLE_STATE=0,
	PLAYER_MOVING_STATE,
	PLAYER_ATTACKING_STATE,
	PLAYER_DODGE_DOWN,
	PLAYER_DODGE_UP,
	PLAYER_DAMAGED,
	MAX_PLAYER_STATE
};
enum EPLAYER_HITBOXES
{
	PLAYER_HB_FEET=0,
	PLAYER_HB_BODY,
	PLAYER_HB_HEAD,
	PLAYER_HB_FRONT,
	PLAYER_HB_BACK,
	PLAYER_HB_LEFT,
	PLAYER_HB_RIGHT,
	PLAYER_HB_ATTACK,
	PLAYER_HB_LOCK,
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
	//âÒïúÇ…ä÷Ç∑ÇÈ
	int nCurrentHealth;
	int nMaxHealth;
	//âeÇ…ä÷Ç∑ÇÈ
	GameObject3D* mShadow;
	//íeÇ…ä÷Ç∑ÇÈ
	GameObject3D* goBullets[MAX_BULLETS];
	int nShootCooldown;
	//ÉåÉìÉ_ÉäÉìÉOÇÃà◊Ç…
	ID3D11DeviceContext* pDeviceContext;
	DXWindow3D*			 pCurrentWindow;
	Box Hitboxes[PLAYER_HB_MAX];
	Cube3D* pVisualHitboxes[PLAYER_HB_MAX];
	GameObject3D* pFloor;
	float fY_force;
	//çUåÇÇ…ä÷ÇµÇƒ
	float fAtkAcceleration;
	char szInputs[MAX_PLAYER_INPUT + 1];
	int nInputTimer;
	PLAYER_ATTACK_MOVE* pCurrentAttackPlaying;
	Billboard2D* FlowersTemp[MAX_FLOWERS];
	ID3D11ShaderResourceView* pFlowerTexture;
	DebugAim* pDebugAim;
	bool DebugAimOn;
	void* pCurrentGame;
	GameObject3D* pLockedOnEnemy;
	int nRecoveryFrames;
	float fAnalogLockInput;
	bool bIsLockedBackwards;
	bool bIsLokedForward;
	int nCancellingGravityFrames;
public:
	Player3D();
	Player3D(Light3D* Light);
	~Player3D();

	void Init();
	void InitPlayerHitboxes();
	void Update();
	void AttackInputsControl();
	void PlayerAttackingControl();
	void Jump(float jumpforce);
	void GravityControl(bool bCountAttackState);
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
	void SetFloor(GameObject3D* Floor);
	bool IsOnTheFloor();
	void AddInput(char A);
	char GetLastInputInserted();
	void ResetInputs();
	void Attack(const char * atkInput);
	void Attack(const char * atkInput, int recursions);
	void InitFlowers();
	void UpdateFlowers();
	void DrawFlowers();
	void SetFlower(XMFLOAT3 Pos);
	float GetYForce();
	void LockModelToObject(GameObject3D* lock);
	void LockPlayerToObject(GameObject3D* lock);
	GameObject3D* GetFloor();
	PLAYER_ATTACK_MOVE* GetCurrentAttack();
	int GetState();
	void SetDamage(int nDamage);
};

Player3D*  GetPlayer3D();


#endif