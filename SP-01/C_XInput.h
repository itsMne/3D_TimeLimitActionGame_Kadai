//*****************************************************************************
// C_Xinput.h
//*****************************************************************************
#ifndef	C_XINPUT_H
#define C_XINPUT_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <XInput.h>
#pragma comment(lib, "XInput.lib")
class C_XInput
{
private:
	XINPUT_STATE xiControllerState;
	int nControllerNumber;
	int nVibrationTime;
	int nCurrentVibrationLeft;
	int nCurrentVibrationRight;
	bool UseVibration;
public:
	C_XInput(int playerNumber);
	~C_XInput();
	void UpdateXinput();
	XINPUT_STATE GetState();
	bool IsConnected();
	void Vibrate(int leftVal, int rightVal, int VibrationTime);
};
#endif