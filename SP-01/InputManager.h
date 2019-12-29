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
	INPUT_JUMP,
	INPUT_ATTACK,
	INPUT_DEBUG_AIM_ON,
	INPUT_DEBUG_CONFIRM,
	INPUT_SCALEUP_X,
	INPUT_SCALEDOWN_X,
	INPUT_SCALEUP_Y,
	INPUT_SCALEDOWN_Y,
	INPUT_SCALEUP_Z,
	INPUT_SCALEDOWN_Z,
	INPUT_SWITCH_DEBUGOBJ,
	INPUT_SWITCH_DEBUGOBJALT,
	INPUT_SAVE_LEVEL,
	INPUT_DEBUGAIM_DELETE,
	MAX_INPUTS
};
enum EAXIS
{
	MOVEMENT_AXIS_HORIZONTAL,
	MOVEMENT_AXIS_VERTICAL,
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