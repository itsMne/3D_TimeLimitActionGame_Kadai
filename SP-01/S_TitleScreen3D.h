#pragma once
#include "S_Scene3D.h"
#include "UIManager2D.h"
class SceneTitleScreen3D :
	public Scene3D
{
private:
	UIObject2D* Logo;
public:
	SceneTitleScreen3D();
	~SceneTitleScreen3D();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
};

