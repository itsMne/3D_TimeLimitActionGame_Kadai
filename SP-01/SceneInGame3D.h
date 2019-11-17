#pragma once
#include "Scene3D.h"
#include "Player3D.h"
#include "Light3D.h"
#include "UIManager2D.h"

class SceneInGame3D :
	public Scene3D
{
private:
	
	ID3D11Device* g_pDevice;
	
	Player3D* pPlayer;

	InGameUI2D* pUI;
public:
	SceneInGame3D();
	~SceneInGame3D();
	void Init();
	void Update();
	void Draw();
	void End();
};

