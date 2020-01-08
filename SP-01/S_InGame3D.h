#pragma once
#include "S_Scene3D.h"
#include "Player3D.h"
#include "Light3D.h"
#include "UIManager2D.h"
#include "Sphere3D.h"

class S_InGame3D :
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
	S_InGame3D();
	~S_InGame3D();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
	Go_List* GetList(int Type);
	void SetAtkEffect(int frames);
	InGameUI2D* GetUIManager();
};

S_InGame3D* GetCurrentGame();