//*****************************************************************************
// Sound.h
//*****************************************************************************
#ifndef _SOUND_H_
#define _SOUND_H_
#include "DXWindow3D.h"
#include <xaudio2.h>
// Windows7�̏ꍇ�͏�L���폜���Ĉȉ��ɒu������.
//#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_TITLE = 0,		// BGM0
	SOUND_LABEL_MENU,			// BGM0
	SOUND_LABEL_TUTORIAL,		// BGM0
	SOUND_LABEL_SA_GAME,		// BGM0
	SOUND_LABEL_GAMEOVER,		// BGM0
	SOUND_LABEL_RANKING,		// BGM0
	SOUND_LABEL_SE_HIT,			// ����SE
	SOUND_LABEL_SE_BIGHIT,			// ����SE
	SOUND_LABEL_SE_TRANSITIONSLASH,			// ����SE
	SOUND_LABEL_SE_PASTESTICKER,			// ����SE
	SOUND_LABEL_SE_REMOVESTICKER,			// ����SE
	SOUND_LABEL_SE_MIRRORBREAK,			// ����SE
	SOUND_LABEL_SE_SWING,			// ����SE
	SOUND_LABEL_SE_KICKSWING,			// ����SE
	SOUND_LABEL_SE_SWORD,			// ����SE
	SOUND_LABEL_SE_BIGSWORD,			// ����SE
	SOUND_LABEL_SE_DAMAGED,			// ����SE
	SOUND_LABEL_MAX,				// �o�^���̎����ݒ�
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void	UninitSound(void);
HRESULT PlaySoundGame(SOUND_LABEL label);
void	StopSound(SOUND_LABEL label);
void	StopSound(void);
void	SetVolume(float fVol);

#endif
