#include "InputManager.h"

bool bInputs[MAX_INPUTS];
void InitInputManager()
{
	InitInput();
	for (int i = 0; i < MAX_INPUTS; i++)
	{
		bInputs[i] = false;
	}
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
}

void EndInputManager()
{
	UninitInput();

}

bool GetInput(int nInput)
{
	return bInputs[nInput];
}


