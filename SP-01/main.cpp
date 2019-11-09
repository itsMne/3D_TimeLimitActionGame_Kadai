#include "main.h"
#include "DXWindow3D.h"


DXWindow3D* pDXWindow = nullptr;


//=============================================================================
// ƒƒCƒ“ŠÖ”
//=============================================================================
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

	pDXWindow = new DXWindow3D(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	pDXWindow->InitDXWindow();
	while(pDXWindow->UpdateDXWindow());
	delete(pDXWindow);
	pDXWindow = nullptr;
	return 0;
}

DXWindow3D * GetGameWindow()
{
	return pDXWindow;
}

HWND GetMainWnd()
{
	if (pDXWindow)
		return pDXWindow->GetMainWnd();
	return nullptr;
}

HINSTANCE GetInstance()
{
	if (pDXWindow)
		return pDXWindow->GetInstance();
	return nullptr;
}

ID3D11Device * GetDevice()
{
	if (pDXWindow)
		return pDXWindow->GetDevice();
	return nullptr;
}

ID3D11DeviceContext * GetDeviceContext()
{
	if (pDXWindow)
		return pDXWindow->GetDeviceContext();
	return nullptr;
}

void SetZBuffer(bool bEnable)
{
	if (pDXWindow)
		return pDXWindow->SetZBuffer(bEnable);
}

void SetZWrite(bool bEnable)
{
	if (pDXWindow)
		return pDXWindow->SetZWrite(bEnable);
}

void SetBlendState(bool bAdd)
{
	if (pDXWindow)
		return pDXWindow->SetBlendState(bAdd);
}
