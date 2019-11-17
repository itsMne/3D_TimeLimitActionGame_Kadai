#pragma once
#include "Scene3D.h"
#include "GameObject3D.h"
class SceneTitleScreen3D :
	public Scene3D
{
private:
	GameObject3D* pLogo;
public:
	SceneTitleScreen3D();
	~SceneTitleScreen3D();
	void Init();
	void Update();
	void Draw();
	void End();
};

