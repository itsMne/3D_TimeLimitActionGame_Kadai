#include "InputManager.h"

bool bInputs[MAX_INPUTS];
int fAxis[MAX_AXIS];
void InitInputManager()
{
	InitInput();
	for (int i = 0; i < MAX_INPUTS; i++)
	{
		bInputs[i] = false;
	}
	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
}

void UpdateInputManager()
{
	UpdateInput();
	
	bInputs[INPUT_FORWARD] = GetKeyPress(VK_W);
	bInputs[INPUT_BACKWARD] = GetKeyPress(VK_S);
	
	bInputs[INPUT_LEFT] = GetKeyPress(VK_A);
	bInputs[INPUT_RIGHT] = GetKeyPress(VK_D);
	bInputs[INPUT_SHOOT] =  GetMouseButton(MOUSEBUTTON_L);
	bInputs[INPUT_AIM] =  GetMouseButton(MOUSEBUTTON_R);

	for (int i = 0; i < MAX_AXIS; fAxis[i] = 0, i++);
		
	if (GetKeyPress(VK_LEFT))
		fAxis[CAMERA_AXIS_HORIZONTAL] = -1;
	if (GetKeyPress(VK_RIGHT))
		fAxis[CAMERA_AXIS_HORIZONTAL] = 1;
	if (GetKeyPress(VK_UP))
		fAxis[CAMERA_AXIS_VERTICAL] = -1;
	if (GetKeyPress(VK_DOWN))
		fAxis[CAMERA_AXIS_VERTICAL] = 1;
}

void EndInputManager()
{
	UninitInput();

}

float GetAxis(int nAxis)
{
	return fAxis[nAxis];
}

bool GetInput(int nInput)
{
	return bInputs[nInput];
}


