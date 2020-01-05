#pragma once
#include "S_Scene3D.h"
#include "Player3D.h"
#include "Light3D.h"
#include "UIManager2D.h"
#include "Sphere3D.h"

class SceneInGame3D :
	public Scene3D
{
private:
	
	ID3D11Device* g_pDevice;
	Player3D* pPlayer;
	InGameUI2D* pUI;
	Sphere3D* pSkybox;
	Go_List* Floors;
	Go_List* Walls;
	Go_List* Enemies;
public:
	SceneInGame3D();
	~SceneInGame3D();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
	Go_List* GetList(int Type);
};

SceneInGame3D* GetCurrentGame();