//*****************************************************************************
// S_Ranking.h
//*****************************************************************************
#pragma once
#include "S_Scene3D.h"
#include "Polygon2D.h"
#include "UIManager2D.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_TOP_RANKS 5

//*****************************************************************************
// �N���X
//*****************************************************************************
class S_Ranking :
	public Scene3D
{
private:
	Polygon2D* pDecorations[MAX_DECORATIONS];
	ID3D11ShaderResourceView*	g_pTexture;
	int nFrameCounter;
	UIObject2D* UIResults[MAX_TOP_RANKS];

public:
	S_Ranking();
	~S_Ranking();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void AddScoreToTopRankings(int ScoreToAdd, int Level);