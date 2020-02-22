//*****************************************************************************
// TransitionControl.h
//*****************************************************************************
#pragma once

//*****************************************************************************
// �G�i��
//*****************************************************************************
enum TRANSITIONS
{
	STICKER_TRANSITION,
	SLASH_TRANSITION,
	TRANSITION_NONE
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
namespace TransitionControl
{
	void Init();
	void Update();
	void Draw();
	void End();
}
void SetTransition(bool TransitionIn, int TransitionType);
bool IsTransitionComplete();