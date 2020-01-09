#pragma once
#include "S_Scene3D.h"
#include "Polygon2D.h"
#define MAX_DECORATIONS 127-1

enum MENU_OPTIONS
{
	OPTION_SCORE_ATTACK = 0,
	OPTION_EXIT,
	MAX_OPTIONS
};

class S_Menu :
	public Scene3D
{
private:
	Polygon2D* pDecorations[MAX_DECORATIONS];
	ID3D11ShaderResourceView*	g_pTexture;
	Polygon2D* pAim;
	Polygon2D* pAim2;
	Polygon2D* pMenuOption[MAX_OPTIONS];
	int nCurrentSelection;
	float fAccelerator[MAX_OPTIONS];
public:
	S_Menu();
	~S_Menu();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
};

