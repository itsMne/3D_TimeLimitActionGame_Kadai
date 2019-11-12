#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H
#include "input.h"
enum EINPUTS
{
	INPUT_FORWARD = 0,
	INPUT_BACKWARD,
	INPUT_RIGHT,
	INPUT_LEFT,
	INPUT_SHOOT,
	INPUT_AIM,
	MAX_INPUTS
};
void InitInputManager();
void UpdateInputManager();
void EndInputManager();
bool GetInput(int nInput);
#endif