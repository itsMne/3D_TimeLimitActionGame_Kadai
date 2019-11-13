#pragma once
#include "Polygon2D.h"
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
	int pnMax_Hearts;
	int pnCurrent_hearts;
	bool bHeartActive;
	UV uv;
public:
	UIObject2D();
	UIObject2D(int nUI_Type);
	~UIObject2D();

	void Init();
	void Update();
	void Draw();
	void UIHeartControl();
	void End();
};

