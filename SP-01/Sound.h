//*****************************************************************************
// Sound.h
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_
#include "DXWindow3D.h"
#include <xaudio2.h>
// Windows7の場合は上記を削除して以下に置き換え.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_TITLE = 0,		// BGM0
	SOUND_LABEL_MENU,			// BGM0
	SOUND_LABEL_TUTORIAL,		// BGM0
	SOUND_LABEL_SA_GAME,		// BGM0
	SOUND_LABEL_GAMEOVER,		// BGM0
	SOUND_LABEL_RANKING,		// BGM0
	SOUND_LABEL_SE_HIT,			// 殴るSE
	SOUND_LABEL_SE_BIGHIT,			// 殴るSE
	SOUND_LABEL_SE_TRANSITIONSLASH,			// 殴るSE
	SOUND_LABEL_SE_PASTESTICKER,			// 殴るSE
	SOUND_LABEL_SE_REMOVESTICKER,			// 殴るSE
	SOUND_LABEL_SE_MIRRORBREAK,			// 殴るSE
	SOUND_LABEL_SE_SWING,			// 殴るSE
	SOUND_LABEL_SE_KICKSWING,			// 殴るSE
	SOUND_LABEL_SE_SWORD,			// 殴るSE
	SOUND_LABEL_SE_BIGSWORD,			// 殴るSE
	SOUND_LABEL_SE_DAMAGED,			// 殴るSE
	SOUND_LABEL_MAX,				// 登録数の自動設定
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySoundGame(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
