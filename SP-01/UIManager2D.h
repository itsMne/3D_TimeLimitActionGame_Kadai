#ifndef	UIMANAGER2D_H
#define UIMANAGER2D_H
#include "Polygon2D.h"
#include "Player3D.h"
enum UI_TYPE
{
	UI_HEART,
	UI_AIM,
	UI_LOGO,
	UI_ATKZOOM,
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
public:
	UIObject2D();
	UIObject2D(int nUI_Type);
	~UIObject2D();

	void Init();
	void Update();
	void RegularUVAnimation();
	void Draw();
	void UIHeartDrawControl();
	void End();
	void SetActiveFrames(int Frames);
};

class InGameUI2D
{
private:
	UIObject2D* pPlayerHealth;
	UIObject2D* pPlayerAim;
	UIObject2D* pAtkEffect;
public:
	InGameUI2D();
	~InGameUI2D();
	void Init();
	void Update();
	void Draw();
	void End();
	void ActivateAtkEffect(int Frames);
};
#endif