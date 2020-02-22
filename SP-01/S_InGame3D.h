//*****************************************************************************
// S_InGame.h
//*****************************************************************************
#pragma once
#include "S_Scene3D.h"
#include "Player3D.h"
#include "Light3D.h"
#include "UIManager2D.h"
#include "Sphere3D.h"
#include "GameObject3D.h"

//*****************************************************************************
// クラス
//*****************************************************************************
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
	int nTimer;
	int nFrameCounter;
	bool bUseTimer;
	bool bGameOverActivated;
	GameObject3D* TutorialSign;
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
	bool TimeIsUp();
	int GetTimer();
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
S_InGame3D* GetCurrentGame();
int GetScore();
void AddScoreWithRank(int add);
void AddScore(int add);
int GetTimer();
bool IsGamePaused();