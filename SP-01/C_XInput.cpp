//*****************************************************************************
// C_XInput.cpp
// XINPUTを管理する
// Author : Mane
//*****************************************************************************
#include "C_XInput.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define VIBRATION_BY_DEFAULT true

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
C_XInput::C_XInput(int playerNumber)
{
	nControllerNumber = playerNumber - 1;
	nVibrationTime = 0;
}

C_XInput::~C_XInput()
{
}

//*****************************************************************************
//UpdateXinput関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void C_XInput::UpdateXinput()
{
	if (--nVibrationTime <= 0)
	{
		nVibrationTime = 0;
		if (nCurrentVibrationLeft > 0 || nCurrentVibrationRight > 0)
			Vibrate(0, 0, 1);
	}
	UseVibration = VIBRATION_BY_DEFAULT;
}

//*****************************************************************************
//GetState関数
//XINPUTの状態を戻す
//引数：void
//戻：XINPUT_STATE
//*****************************************************************************
XINPUT_STATE C_XInput::GetState()
{
	ZeroMemory(&xiControllerState, sizeof(XINPUT_STATE));
	XInputGetState(nControllerNumber, &xiControllerState);
	return xiControllerState;
}

//*****************************************************************************
//IsConnected関数
//XBOXコントローラーは繋がってることを確認する
//引数：void
//戻：bool
//*****************************************************************************
bool C_XInput::IsConnected()
{
	ZeroMemory(&xiControllerState, sizeof(XINPUT_STATE));
	DWORD Result = XInputGetState(nControllerNumber, &xiControllerState);
	if (Result == ERROR_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//*****************************************************************************
//Vibrate関数
//XBOXコントローラーの揺れをコントロールする
//引数：int, int, int
//戻：void
//*****************************************************************************
void C_XInput::Vibrate(int leftVal, int rightVal, int VibrationTime)
{
	if (!UseVibration)
		return;
	if (nCurrentVibrationLeft == leftVal || nCurrentVibrationLeft == rightVal)
		return;
	if (nVibrationTime > 0)
	{
		XINPUT_VIBRATION Vibration;
		ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));
	}
	nVibrationTime = VibrationTime;
	nCurrentVibrationLeft = leftVal;
	nCurrentVibrationRight = rightVal;

	XINPUT_VIBRATION Vibration;
	ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

	Vibration.wLeftMotorSpeed = leftVal;
	Vibration.wRightMotorSpeed = rightVal;

	XInputSetState(nControllerNumber, &Vibration);
}
