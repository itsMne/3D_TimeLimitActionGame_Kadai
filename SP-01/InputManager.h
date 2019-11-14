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
enum EAXIS
{
	CAMERA_AXIS_HORIZONTAL,
	CAMERA_AXIS_VERTICAL,
	MAX_AXIS
};
void InitInputManager();
void UpdateInputManager();
void EndInputManager();
float GetAxis(int nAxis);
bool GetInput(int nInput);
void VibrateXinput(float vivbrationL, float VibrationR);
#endif