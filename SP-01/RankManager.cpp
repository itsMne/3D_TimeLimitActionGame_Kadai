//*****************************************************************************
// RankManager.cpp
// ランクを管理する
// Author : Mane
//*****************************************************************************
#include "RankManager.h"
#include "Player3D.h"
#include <stdio.h>
#include <windows.h>

//*****************************************************************************
// エナム定義
//*****************************************************************************
enum ADDED_TO_RANK_RESULT
{
	MOVE_ADDED,
	MOVE_REPEATED,
	MOVE_NOT_ADDED = -1,
	MAX_RESULT
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
int		nCurrentRank;
int		MovesInMeter[MAX_MOVES];
int		RepeatedMoves[MAX_MOVES * 2];
int		AddResult;
int		nMoveStrength;
float	fRankFrames;
int		nFramesForFreezingRank;

//*****************************************************************************
//Init関数
//初期化の関数
//引数：void
//戻：void
//*****************************************************************************
void RankManager::Init()
{
	nCurrentRank = 1;
	AddResult = MOVE_NOT_ADDED;
	fRankFrames = 0;
	nFramesForFreezingRank = 0;
	nMoveStrength = 0;
	for (int i = 0; i < MAX_MOVES; i++)
	{
		MovesInMeter[i] = -1;
		RepeatedMoves[i] = -1;
	}
}

//*****************************************************************************
//Update関数
//アップデートの関数
//引数：void
//戻：void
//*****************************************************************************
void RankManager::Update()
{
	Player3D* pPlayer = GetPlayer3D();
	if (!pPlayer)
		return;
	if (AddResult == MOVE_ADDED) {
		fRankFrames += nMoveStrength;
		nFramesForFreezingRank = 15;
	}
	else if (AddResult == MOVE_REPEATED)
	{
		nFramesForFreezingRank = 10;
	}
	if (fRankFrames > MAX_FRAMES_FOR_RANKINGUP*nCurrentRank)
	{
		nCurrentRank++;
		if (nCurrentRank > MAX_STYLE_RANK) {
			nCurrentRank = MAX_STYLE_RANK;
			fRankFrames = MAX_FRAMES_FOR_RANKINGUP * nCurrentRank;
		}
	}
	else if (fRankFrames == 0) {
		nCurrentRank--;
		if (nCurrentRank < 1)
			nCurrentRank = 1;
		fRankFrames = 50;
		nFramesForFreezingRank = 5;
	}
	if (nFramesForFreezingRank > 20)
		nFramesForFreezingRank = 20;
	if (nFramesForFreezingRank <= 0) {
		if (fRankFrames > 0) {
			fRankFrames--;
			if (nCurrentRank >= 4)
				fRankFrames -= 0.5f;
		}
	}
	else
	{
		nFramesForFreezingRank--;
	}
	if (fRankFrames < 0)
		fRankFrames = 0;
	AddResult = MOVE_NOT_ADDED;
}

//*****************************************************************************
//AddMoveToRankMeter関数
//もらった動作でランクを計算する
//引数：int, int
//戻：void
//*****************************************************************************
void AddMoveToRankMeter(int nMoveToAdd, int MoveStrength)
{
	if (AddResult != MOVE_NOT_ADDED)
		return;
	//printf("%s, L%d: %d\n", __FILE__, __LINE__, nMoveToAdd);
	nMoveStrength = MoveStrength;
	int countSameMove = 0;
	for (int i = 0; i < MAX_MOVES; i++)
	{
		if (MovesInMeter[i] == nMoveToAdd)
			countSameMove++;
	}
	if (countSameMove <= 1) {
		for (int i = MAX_MOVES - 1; i >= 0; i--)
		{
			MovesInMeter[i] = MovesInMeter[i - 1];
		}
		MovesInMeter[0] = nMoveToAdd;
		AddResult = MOVE_ADDED;
		return;
	}
	countSameMove = 0;
	for (int i = 0; i < MAX_MOVES * 2; i++)
	{
		if (RepeatedMoves[i] == nMoveToAdd)
			countSameMove++;
	}
	if (countSameMove <= 8)
	{
		for (int i = (MAX_MOVES * 2) - 1; i >= 0; i--)
		{
			RepeatedMoves[i] = RepeatedMoves[i - 1];
		}
		RepeatedMoves[0] = nMoveToAdd;
		AddResult = MOVE_REPEATED;
		return;
	}
	AddResult = MOVE_NOT_ADDED;
}

//*****************************************************************************
//AddFreezeRankFrames関数
//もらった数字フレームのでランクを止まる
//引数：int
//戻：void
//*****************************************************************************
void AddFreezeRankFrames(int nFramesToAdd)
{
	nFramesForFreezingRank += nFramesToAdd;
}

//*****************************************************************************
//ResetRanks関数
//ランクをリセットする
//引数：void
//戻：void
//*****************************************************************************
void ResetRanks()
{
	nCurrentRank = 1;
	AddResult = MOVE_NOT_ADDED;
	fRankFrames = 0;
	nFramesForFreezingRank = 0;
	nMoveStrength = 0;
	for (int i = 0; i < MAX_MOVES; i++)
	{
		MovesInMeter[i] = -1;
		RepeatedMoves[i] = -1;
	}
}

//*****************************************************************************
//GetRank関数
//ランクを戻す
//引数：void
//戻：int
//*****************************************************************************
int GetRank()
{
	return nCurrentRank;
}

//*****************************************************************************
//GetRankFrames関数
//ランクのフレームカウンターを戻す
//引数：void
//戻：int
//*****************************************************************************
int GetRankFrames()
{
	return fRankFrames;
}
