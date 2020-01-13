#ifndef RANKMANAGER_H
#define RANKMANAGER_H

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_STYLE_RANK			 5
#define MAX_MOVES				 4
#define MAX_FRAMES_FOR_RANKINGUP 90

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
namespace RankManager
{
	void Init();
	void Update();
};
void AddMoveToRankMeter(int MoveToAdd, int nMoveStrength);
void AddFreezeRankFrames(int nFramesToAdd);
void ResetRanks();
int  GetRank();
int	 GetRankFrames();
#endif  //RANKMANAGER_H