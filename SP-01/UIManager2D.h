#ifndef	UIMANAGER2D_H
#define UIMANAGER2D_H
#define MAX_AURA 5
#include "Polygon2D.h"
#include "Player3D.h"
enum UI_TYPE
{
	UI_HEART,
	UI_AIM,
	UI_LOGO,
	UI_ATKZOOM,
	UI_AURA,
	UI_GAMEOVER_SCREEN,
	UI_SCORE,
	UI_TYPE_MAX
};
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
	int nGameOverFrames;
	int nScore;
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
	int GetGameOverFrames();
};

#endif