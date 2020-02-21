#include "S_Ranking.h"
#include "InputManager.h"
#include "Sound.h"
#include "Texture.h"
#define SCORE_FILENAMEPATH "data/RankingScores.bin"

int nTopRanks[MAX_MODES][5];
S_Ranking::S_Ranking(): Scene3D(false)
{
	MainWindow->SetWindowColor(0.78, 0.58, 0.58);
	CreateTextureFromFile(GetDevice(), "data/texture/UI/DecorationAlt.tga", &g_pTexture);
	for (int i = 0; i < MAX_MODES; i++)
	{
		for (int j = 0; j < MAX_TOP_RANKS; j++)
		{
			nTopRanks[i][j] = 0;
		}
	}
	FILE *pFile = fopen(SCORE_FILENAMEPATH, "rb");
	printf("スコアデータを読み込みます。\n");
	if (pFile)
	{
		fread(nTopRanks, sizeof(int[MAX_MODES][MAX_TOP_RANKS]), 1, pFile);
	}
	fclose(pFile);
	XMFLOAT2 Pos = { -SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 - 50 };
	bool bSpin = true;
	bool bSpinY = true;
	for (int i = 0; i < MAX_DECORATIONS; i++)
	{
		pDecorations[i] = new Polygon2D();
		pDecorations[i]->SetPolygonSize(100, 100);
		pDecorations[i]->SetTexture(g_pTexture);
		pDecorations[i]->SetPolygonAlpha(0.25f);
		if (bSpin) {
			pDecorations[i]->RotateAroundY(180);
			bSpin = false;
		}
		else {
			bSpin = true;
		}
		pDecorations[i]->SetPolygonPos(Pos.x, Pos.y);
		Pos.x += 100;
		if (Pos.x > 800)
		{
			if (bSpinY) {
				bSpin = false;
				bSpinY = false;
			}
			else {
				bSpin = true;
				bSpinY = true;
			}
			Pos.x = -SCREEN_WIDTH / 2 + 50;
			Pos.y -= 100;
		}
	}
	nFrameCounter = 0;
	for (int i = 0; i < MAX_TOP_RANKS; i++)
	{
		UIResults[i] = new UIObject2D(UI_RESULT_SCREEN_NUMBER);
		UIResults[i]->SetPolygonPosY((SCREEN_HEIGHT / 2) - (120*(i+1)));
	}
	PlaySoundGame(SOUND_LABEL_RANKING);
}


S_Ranking::~S_Ranking()
{
	for (int i = 0; i < MAX_DECORATIONS; i++)
		SAFE_DELETE(pDecorations[i]);
	SAFE_RELEASE(g_pTexture);
	for (int i = 0; i < MAX_TOP_RANKS; i++)
		SAFE_DELETE(UIResults[i]);
}

void S_Ranking::Init()
{
}

eSceneType S_Ranking::Update()
{
	nFrameCounter++;
	for (int i = 0; i < MAX_DECORATIONS; i++) {
		pDecorations[i]->RotateAroundY(1);
		pDecorations[i]->Translate({ 2,-2 });
		if (pDecorations[i]->GetPolygonPos().x > SCREEN_WIDTH / 2 + 50)
			pDecorations[i]->Translate({ -(SCREEN_WIDTH + 100),0 });
		if (pDecorations[i]->GetPolygonPos().y < -SCREEN_HEIGHT / 2 - 100) {
			pDecorations[i]->Translate({ 0,SCREEN_HEIGHT + 200 });
			pDecorations[i]->RotateAroundY(180);
		}
	}
	for (int i = 0; i < MAX_TOP_RANKS; i++)
	{
		UIResults[i]->SetNum(nTopRanks[0][i]);
	}
	if ((GetInput(INPUT_SWORD) || GetInput(INPUT_JUMP)) && nFrameCounter>60)
	{
		return SCENE_MENU;
	}
	return SCENE_RANKING;
}

void S_Ranking::Draw()
{
	for (int i = 0; i < MAX_DECORATIONS; i++)
		pDecorations[i]->DrawPolygon(GetDeviceContext());
	for (int i = 0; i < MAX_TOP_RANKS; i++)
		if (UIResults[i])UIResults[i]->Draw();
}

void S_Ranking::End()
{
}

void AddScoreToTopRankings(int ScoreToAdd, int Level)
{
	FILE *pFile = fopen(SCORE_FILENAMEPATH, "rb");
	printf("スコアデータを読み込みます。\n");
	if (pFile)
	{
		fread(nTopRanks, sizeof(int[MAX_MODES][MAX_TOP_RANKS]), 1, pFile);
	}
	fclose(pFile);
	for (int i = 0; i < MAX_TOP_RANKS; i++)
	{
		if (nTopRanks[Level][i] < ScoreToAdd) {
			for (int j = MAX_TOP_RANKS - 1; j > i; j--)
			{
				nTopRanks[Level][j] = nTopRanks[Level][j - 1];
			}
			nTopRanks[Level][i] = ScoreToAdd;
			FILE *pFile = NULL;
			pFile = fopen(SCORE_FILENAMEPATH, "wb");
			if (pFile) {
				fwrite(nTopRanks, sizeof(int[MAX_MODES][MAX_TOP_RANKS]), 1, pFile);
				printf("データを書きだします: SCORE: %d LEVEL: %d。\n", nTopRanks[Level][i], Level);
			}
			fclose(pFile);
			return;
		}
	}
}

