//*****************************************************************************
// ���C��
//*****************************************************************************
#include "main.h"
#include "DXWindow3D.h"

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
DXWindow3D* pDXWindow = nullptr;

//*****************************************************************************
// ���C���֐�
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
//GetGameWindow�֐�
//�E�C���h�E�Y�̃A�h���X��߂�
//�����Fvoid
//�߁FDXWindow3D*
//*****************************************************************************
DXWindow3D * GetGameWindow()
{
	return pDXWindow;
}

//*****************************************************************************
//GetMainWnd�֐�
//HWND��߂�
//�����Fvoid
//�߁FHWND
//*****************************************************************************
HWND GetMainWnd()
{
	if (pDXWindow)
		return pDXWindow->GetMainWnd();
	return nullptr;
}

//*****************************************************************************
//GetInstance�֐�
//�C���X�^���X��߂�
//�����Fvoid
//�߁FHINSTANCE
//*****************************************************************************
HINSTANCE GetInstance()
{
	if (pDXWindow)
		return pDXWindow->GetInstance();
	return nullptr;
}

//*****************************************************************************
//GetDevice�֐�
//�f�o�C�X�̃A�h���X��߂�
//�����Fvoid
//�߁FID3D11Device*
//*****************************************************************************
ID3D11Device * GetDevice()
{
	if (pDXWindow)
		return pDXWindow->GetDevice();
	return nullptr;
}

//*****************************************************************************
//GetDeviceContext�֐�
//�f�o�C�X�R���e�N�X�g�̃A�h���X��߂�
//�����Fvoid
//�߁FID3D11Device*
//*****************************************************************************
ID3D11DeviceContext * GetDeviceContext()
{
	if (pDXWindow)
		return pDXWindow->GetDeviceContext();
	return nullptr;
}

//*****************************************************************************
//SetZBuffer�֐�
//Z�o�b�t�@�[��ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void SetZBuffer(bool bEnable)
{
	if (pDXWindow)
		return pDXWindow->SetZBuffer(bEnable);
}

//*****************************************************************************
//SetZWrite�֐�
//Z�o�b�t�@�[�iWriter�j��ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void SetZWrite(bool bEnable)
{
	if (pDXWindow)
		return pDXWindow->SetZWrite(bEnable);
}

//*****************************************************************************
//SetBlendState�֐�
//�u�����_�[��ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void SetBlendState(bool bAdd)
{
	if (pDXWindow)
		return pDXWindow->SetBlendState(bAdd);
}
