#pragma once

enum TRANSITIONS
{
	STICKER_TRANSITION,
	TRANSITION_NONE
};

namespace TransitionControl
{
	void Init();
	void Update();
	void Draw();
	void End();
}

void SetTransition(bool TransitionIn, int TransitionType);
bool IsTransitionComplete();