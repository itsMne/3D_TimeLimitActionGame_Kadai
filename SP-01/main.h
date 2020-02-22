//*****************************************************************************
// Main.h
//*****************************************************************************
#pragma once
#include "UniversalStructures.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HWND GetMainWnd();
HINSTANCE GetInstance();
ID3D11Device* GetDevice();
ID3D11DeviceContext* GetDeviceContext();
void SetZBuffer(bool bEnable);
void SetZWrite(bool bEnable);
void SetBlendState(bool bAdd);
