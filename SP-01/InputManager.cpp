#include "InputManager.h"
#include "stdio.h"
#include "C_XInput.h"

C_XInput* Player1 = nullptr;
bool bInputs[MAX_INPUTS];
float fAxis[MAX_AXIS];
bool bHoldingXinput[MAX_INPUTS];
bool bXinputConnected;

void XinputTriggerControl(bool BeforeInputs);
void InitInputManager()
{
	InitInput();
	Player1 = new C_XInput(1);
	for (int i = 0; i < MAX_INPUTS; bInputs[i] = false, i++);
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
}

void UpdateInputManager()
{
	UpdateInput();
	Player1->UpdateXinput();
	XinputTriggerControl(true);
	bXinputConnected = Player1->IsConnected();

	bInputs[INPUT_FORWARD] = GetKeyPress(VK_W);
	bInputs[INPUT_BACKWARD] = GetKeyPress(VK_S);
	
	bInputs[INPUT_LEFT] = GetKeyPress(VK_A);
	bInputs[INPUT_RIGHT] = GetKeyPress(VK_D);

	bInputs[INPUT_LOCKON] = GetKeyPress(VK_SHIFT);

	bInputs[INPUT_JUMP] = GetKeyTrigger(VK_K) || GetKeyTrigger(VK_SPACE) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A && !bHoldingXinput[INPUT_JUMP]);

	bInputs[INPUT_SHOOT] = GetMouseButton(MOUSEBUTTON_L) || GetKeyPress(VK_J) ||
		(bXinputConnected && Player1->GetState().Gamepad.bLeftTrigger > 0);
	bInputs[INPUT_AIM] = GetKeyPress(VK_CONTROL) || GetKeyPress(VK_J) ||
							(bXinputConnected && Player1->GetState().Gamepad.bRightTrigger>0);

	bInputs[INPUT_SWORD] = GetMouseTrigger(MOUSEBUTTON_L) || GetKeyTrigger(VK_I) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y && !bHoldingXinput[INPUT_SWORD]);
	bInputs[INPUT_KICK] = GetMouseTrigger(MOUSEBUTTON_R) || GetKeyTrigger(VK_L) || (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B && !bHoldingXinput[INPUT_KICK]);

	bInputs[INPUT_DEBUG_AIM_ON] = GetKeyTrigger(VK_1);
	bInputs[INPUT_DEBUG_CONFIRM] = GetKeyTrigger(VK_2);
	bInputs[INPUT_SCALEUP_X] = GetKeyPress(VK_L);
	bInputs[INPUT_SCALEDOWN_X] = GetKeyPress(VK_J);
	bInputs[INPUT_SCALEUP_Y] = GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_I);
	bInputs[INPUT_SCALEDOWN_Y] = GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_K);
	bInputs[INPUT_SCALEUP_Z] = !GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_I);
	bInputs[INPUT_SCALEDOWN_Z] = !GetKeyPress(VK_LSHIFT) && GetKeyPress(VK_K);
	bInputs[INPUT_SWITCH_DEBUGOBJ] = GetKeyTrigger(VK_O);
	bInputs[INPUT_SWITCH_DEBUGOBJALT] = GetKeyTrigger(VK_U);
	bInputs[INPUT_SAVE_LEVEL] = GetKeyPress(VK_LCONTROL) && GetKeyTrigger(VK_S);
	bInputs[INPUT_DEBUGAIM_DELETE] = GetKeyTrigger(VK_BACK);
	bInputs[INPUT_DEBUGAIM_ZOOMIN] = GetKeyPress(VK_NUMPAD8);
	bInputs[INPUT_DEBUGAIM_ZOOMOUT] = GetKeyPress(VK_NUMPAD2);

	bool bUsingKeyBoard = false;
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
		
	if (GetKeyPress(VK_W))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_VERTICAL] = 1;
	}

	if (GetKeyPress(VK_S))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_VERTICAL] = -1;
	}

	if (GetKeyPress(VK_D))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = 1;
	}

	if (GetKeyPress(VK_A))
	{
		bUsingKeyBoard = true;
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = -1;
	}

	if (GetKeyPress(VK_LEFT) || GetKeyPress(VK_Q))
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_HORIZONTAL] = -1;
	}
	if (GetKeyPress(VK_RIGHT) || GetKeyPress(VK_E))
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_HORIZONTAL] = 1;
	}
	if (GetKeyPress(VK_UP))
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_VERTICAL] = 1;
	}
	if (GetKeyPress(VK_DOWN))
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_VERTICAL] = -1;
	}
	if (!bUsingKeyBoard && bXinputConnected) {
		fAxis[CAMERA_AXIS_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbRX / 32876.7f + 0.023543f;
		fAxis[CAMERA_AXIS_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbRY / 32876.7f - 0.017848f + 0.010000;
		fAxis[MOVEMENT_AXIS_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbLX / 32767;
		fAxis[MOVEMENT_AXIS_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbLY / 32767;
	}
	
	XinputTriggerControl(false);

	//printf("%f\n", fAxis[MOVEMENT_AXIS_VERTICAL]);
}

void EndInputManager()
{
	UninitInput();
}

float GetAxis(int nAxis)
{
	if(fAxis[nAxis] > 0.05f || fAxis[nAxis] < -0.05f)
		return fAxis[nAxis];
	return 0;
}

bool GetInput(int nInput)
{
	return bInputs[nInput];
}

void VibrateXinput(float vivbrationL, float VibrationR)
{
	if (!bXinputConnected)
		return;
	Player1->Vibrate(vivbrationL, VibrationR, 10);
}

void XinputTriggerControl(bool BeforeInputs)
{
	if (BeforeInputs) {
		if (bHoldingXinput[INPUT_SWORD] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y)) bHoldingXinput[INPUT_SWORD] = false;
		if (bHoldingXinput[INPUT_KICK] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B)) bHoldingXinput[INPUT_KICK] = false;
		if (bHoldingXinput[INPUT_JUMP] && !(Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)) bHoldingXinput[INPUT_JUMP] = false;

	}
	else {
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_Y) bHoldingXinput[INPUT_SWORD] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_B) bHoldingXinput[INPUT_KICK] = true;
		if (Player1->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A) bHoldingXinput[INPUT_JUMP] = true;
	}
}
