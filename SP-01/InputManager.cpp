#include "InputManager.h"
#include "stdio.h"
#include "C_XInput.h"

C_XInput* Player1 = nullptr;
bool bInputs[MAX_INPUTS];
float fAxis[MAX_AXIS];
bool bXinputConnected;
void InitInputManager()
{
	InitInput();
	Player1 = new C_XInput(1);
	for (int i = 0; i < MAX_INPUTS; i++)
	{
		bInputs[i] = false;
	}
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
}

void UpdateInputManager()
{
	UpdateInput();
	Player1->UpdateXinput();
	bXinputConnected = Player1->IsConnected();

	bInputs[INPUT_FORWARD] = GetKeyPress(VK_W);
	bInputs[INPUT_BACKWARD] = GetKeyPress(VK_S);
	
	bInputs[INPUT_LEFT] = GetKeyPress(VK_A);
	bInputs[INPUT_RIGHT] = GetKeyPress(VK_D);
	bInputs[INPUT_SHOOT] =  GetMouseButton(MOUSEBUTTON_L) ||
		(bXinputConnected && Player1->GetState().Gamepad.bLeftTrigger > 0);
	bInputs[INPUT_AIM] =  GetMouseButton(MOUSEBUTTON_R) || 
							(bXinputConnected && Player1->GetState().Gamepad.bRightTrigger>0);
	//printf("%f\n", (float)Player1->GetState().Gamepad.sThumbRX /32876.7 + 0.01f);
	bool bUsingKeyBoard = false;
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
		
	if (GetKeyPress(VK_LEFT)) 
	{
		bUsingKeyBoard = true;
		fAxis[CAMERA_AXIS_HORIZONTAL] = -1;
	}
	if (GetKeyPress(VK_RIGHT))
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
		fAxis[CAMERA_AXIS_HORIZONTAL] = (float)Player1->GetState().Gamepad.sThumbRX / 32876.7f + 0.023543f;// +0.0113543f;
		fAxis[CAMERA_AXIS_VERTICAL] = (float)Player1->GetState().Gamepad.sThumbRY / 32876.7f - 0.017848f+ 0.010000;
	}
	
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


