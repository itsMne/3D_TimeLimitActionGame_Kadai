#pragma once
#include "Polygon2D.h"
#include "Player3D.h"
enum UI_TYPE
{
	UI_HEART,
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
public:
	UIObject2D();
	UIObject2D(int nUI_Type);
	~UIObject2D();

	void Init();
	void Update();
	void RegularUVAnimation();
	void Draw();
	void UIHeartControl();
	void End();
};

