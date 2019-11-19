#pragma once

#include "UniversalStructures.h"
class DXWindow3D
{
private:
	HWND						g_hWnd;					// ���C�� �E�B���h�E �n���h��
	HINSTANCE					g_hInst;				// �C���X�^���X �n���h��
	HRESULT						hr;
	ID3D11Device*				g_pDevice;				// �f�o�C�X
	ID3D11DeviceContext*		g_pDeviceContext;		// �f�o�C�X �R���e�L�X�g
	IDXGISwapChain*				g_pSwapChain;			// �X���b�v�`�F�[��
	ID3D11RenderTargetView*		g_pRenderTargetView;	// �t���[���o�b�t�@
	ID3D11Texture2D*			g_pDepthStencilTexture;	// Z�o�b�t�@�p������
	ID3D11DepthStencilView*		g_pDepthStencilView;	// Z�o�b�t�@
	UINT						g_uSyncInterval = 0;	// �������� (0:��, 1:�L)
	ID3D11RasterizerState*		g_pRs[3];				// ���X�^���C�U �X�e�[�g
	ID3D11BlendState*			g_pBlendState[2];		// �u�����h �X�e�[�g
	ID3D11DepthStencilState*	g_pDSS;					// Z�o�b�t�@�X�V���Ȃ�
	MSG							msg;
	int							g_nCountFPS;			// FPS�J�E���^
	float						fR, fG, fB;				//�E�C���h�E�Y�̐F
	DWORD						dwExecLastTime;			//FPS�̃^�C�}�[�i���s�̍Ō�J�E���^�[�j
	DWORD						dwFPSLastTime;			//FPS�̃^�C�}�[�iFPS�ϐ��̍Ō�J�E���^�[�j
	DWORD						dwCurrentTime;
	DWORD						dwFrameCount;
public:
	DXWindow3D(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow, bool bIsMainWindow);
	~DXWindow3D();
	HRESULT CreateBackBuffer(void);
	HRESULT Init(HWND hWnd, BOOL bWindow);
	void InitDXWindow();
	void Uninit(void);
	bool UpdateDXWindow();
	void Update(void);
	void Draw(void);
	HWND GetMainWnd();
	HINSTANCE GetInstance();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	void SetZBuffer(bool bEnable);
	void SetZWrite(bool bEnable);
	void SetBlendState(bool bAdd);
	void ReleaseBackBuffer();
	static int DX_OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
	static LRESULT CALLBACK DXWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void SetWindowColor(float R, float G, float B);
	void SetWindowColor255(int R, int G, int B);
	XMFLOAT3 GetWindowColor();
	void ActivateConsole();
	int GetFPS();
	ID3D11RasterizerState* GetRasterizerState(int num);
	void SetCull(int nCullMode);
};

DXWindow3D* GetMainWindow();
int GetMainWindowFPS();
void SetCullMode(int nCullMode);