//*****************************************************************************
// UIManager2D.h
//*****************************************************************************
#ifndef	UIMANAGER2D_H
#define UIMANAGER2D_H
#define MAX_AURA 5
#include "Polygon2D.h"
#include "Player3D.h"

//*****************************************************************************
// エナム
//*****************************************************************************
enum AURA_EFFECTS
{
	NORMAL_AURA_TEXTURE,
	DARK_AURA_TEXTURE,
	HEART_AURA_TEXTURE,
	MAX_AURA_TEXTURE
};
enum UI_TYPE
{
	UI_HEART,
	UI_AIM,
	UI_LOGO,
	UI_ATKZOOM,
	UI_AURA,
	UI_GAMEOVER_SCREEN,
	UI_SCORE,
	UI_RANK,
	UI_TIMER,
	UI_RESULT_SCREEN_NUMBER,
	UI_SCORETEXT,
	UI_TUTORIAL_MESSAGE,
	UI_PAUSE,
	UI_PAUSE_BLACK,
	UI_TYPE_MAX
};
enum TUTORIAL_MESSAGES
{
	TUTM_ATTACK,
	TUTM_ATTACK2,
	TUTM_MOVE,
	TUTM_EVADE,
	TUTM_LOCKON,
	TUTM_JUMP,
	TUTM_GAMESTART,
	TUTM_MISC1,
	TUTM_MISC2,
	TUTM_MISC3,
	TUTM_MISC4,
	TUTM_MAX
};

//*****************************************************************************
// クラス
//*****************************************************************************
class UIObject2D :
	public Polygon2D
{
private:
	int nType;
	int nAnimationTimer;
	int nAnimationSpeed;
	int nMax_Hearts;
	int nCurrent_hearts;
	bool bHeartActive;
	Player3D* pPlayer;
	UV uv;
	int nFramesActive;
	float fAcceleration;
	bool bIsInUse;
	bool bSpin;
	int nGameOverFrames;
	int nNum;
	
public:
	UIObject2D();
	UIObject2D(int nUI_Type);
	~UIObject2D();

	void Init();
	void Update();
	void RegularUVAnimation();
	void Draw();
	void DrawNumber();
	void UIHeartDrawControl();
	void End();
	void SetActiveFrames(int Frames);
	bool GetUse();
	void SetAura();
	void SetUse(bool use);
	int GetGameOverFrames();
	void SetSpin(bool spin);
	void SetNum(int num);
	void SetTutorialMessage(int num);
};
class InGameUI2D
{
private:
	UIObject2D* pPlayerHealth;
	UIObject2D* pPlayerAim;
	UIObject2D* pAtkEffect;
	UIObject2D* pAuraEffects[MAX_AURA];
	UIObject2D* pGameOverScreen;
	UIObject2D* pScore;
	UIObject2D* pTimer;
	UIObject2D* pRankMeter;
	UIObject2D* pScoreText;
	UIObject2D* pTutorialMessage;
	UIObject2D* pPause[2];
	int nCurrentTutorialMessage;
	bool bUseTutorialMessage;

public:
	InGameUI2D();
	~InGameUI2D();
	void Init();
	void Update();
	void Draw();
	void End();
	void ActivateAtkEffect(int Frames);
	void ActivateGameOver();
	void SetAura();
	void SetAura(int Texture);
	int GetGameOverFrames();
	void SetNextTutorialMessage();
	void DeactivateTutorial();
};

#endif