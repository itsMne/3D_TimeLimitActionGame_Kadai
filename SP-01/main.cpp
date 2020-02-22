//*****************************************************************************
// メイン
//*****************************************************************************
#include "main.h"
#include "DXWindow3D.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
DXWindow3D* pDXWindow = nullptr;

//*****************************************************************************
// メイン関数
//*****************************************************************************
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	pDXWindow = new DXWindow3D(hInstance, hPrevInstance, lpCmdLine, nCmdShow, true);
	pDXWindow->InitDXWindow();
	while(pDXWindow->UpdateDXWindow());
	delete(pDXWindow);
	pDXWindow = nullptr;
	return 0;
}

//*****************************************************************************
//GetGameWindow関数
//ウインドウズのアドレスを戻す
//引数：void
//戻：DXWindow3D*
//*****************************************************************************
DXWindow3D * GetGameWindow()
{
	return pDXWindow;
}

//*****************************************************************************
//GetMainWnd関数
//HWNDを戻す
//引数：void
//戻：HWND
//*****************************************************************************
HWND GetMainWnd()
{
	if (pDXWindow)
		return pDXWindow->GetMainWnd();
	return nullptr;
}

//*****************************************************************************
//GetInstance関数
//インスタンスを戻す
//引数：void
//戻：HINSTANCE
//*****************************************************************************
HINSTANCE GetInstance()
{
	if (pDXWindow)
		return pDXWindow->GetInstance();
	return nullptr;
}

//*****************************************************************************
//GetDevice関数
//デバイスのアドレスを戻す
//引数：void
//戻：ID3D11Device*
//*****************************************************************************
ID3D11Device * GetDevice()
{
	if (pDXWindow)
		return pDXWindow->GetDevice();
	return nullptr;
}

//*****************************************************************************
//GetDeviceContext関数
//デバイスコンテクストのアドレスを戻す
//引数：void
//戻：ID3D11Device*
//*****************************************************************************
ID3D11DeviceContext * GetDeviceContext()
{
	if (pDXWindow)
		return pDXWindow->GetDeviceContext();
	return nullptr;
}

//*****************************************************************************
//SetZBuffer関数
//Zバッファーを設定する
//引数：bool
//戻：void
//*****************************************************************************
void SetZBuffer(bool bEnable)
{
	if (pDXWindow)
		return pDXWindow->SetZBuffer(bEnable);
}

//*****************************************************************************
//SetZWrite関数
//Zバッファー（Writer）を設定する
//引数：bool
//戻：void
//*****************************************************************************
void SetZWrite(bool bEnable)
{
	if (pDXWindow)
		return pDXWindow->SetZWrite(bEnable);
}

//*****************************************************************************
//SetBlendState関数
//ブレンダーを設定する
//引数：bool
//戻：void
//*****************************************************************************
void SetBlendState(bool bAdd)
{
	if (pDXWindow)
		return pDXWindow->SetBlendState(bAdd);
}
