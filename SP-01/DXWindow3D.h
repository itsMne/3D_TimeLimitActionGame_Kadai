//*****************************************************************************
// DXWindow3D.h
//*****************************************************************************
#pragma once
#include "UniversalStructures.h"

//*****************************************************************************
// クラス
//*****************************************************************************
class DXWindow3D
{
private:
	HWND						g_hWnd;					// メイン ウィンドウ ハンドル
	HINSTANCE					g_hInst;				// インスタンス ハンドル
	HRESULT						hr;
	ID3D11Device*				g_pDevice;				// デバイス
	ID3D11DeviceContext*		g_pDeviceContext;		// デバイス コンテキスト
	IDXGISwapChain*				g_pSwapChain;			// スワップチェーン
	ID3D11RenderTargetView*		g_pRenderTargetView;	// フレームバッファ
	ID3D11Texture2D*			g_pDepthStencilTexture;	// Zバッファ用メモリ
	ID3D11DepthStencilView*		g_pDepthStencilView;	// Zバッファ
	UINT						g_uSyncInterval = 0;	// 垂直同期 (0:無, 1:有)
	ID3D11RasterizerState*		g_pRs[3];				// ラスタライザ ステート
	ID3D11BlendState*			g_pBlendState[2];		// ブレンド ステート
	ID3D11DepthStencilState*	g_pDSS;					// Zバッファ更新しない
	MSG							msg;
	int							g_nCountFPS;			// FPSカウンタ
	float						fR, fG, fB;				//ウインドウズの色
	DWORD						dwExecLastTime;			//FPSのタイマー（実行の最後カウンター）
	DWORD						dwFPSLastTime;			//FPSのタイマー（FPS変数の最後カウンター）
	DWORD						dwCurrentTime;
	DWORD						dwFrameCount;
	float						fMouseWheelMove;
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
	BOOL OnMouseWheel(HWND hWnd, UINT nFlags, short zDelta, POINTS pt);
	static LRESULT CALLBACK DXWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void SetWindowColor(float R, float G, float B);
	void SetWindowColor255(int R, int G, int B);
	XMFLOAT3 GetWindowColor();
	void ActivateConsole();
	int GetFPS();
	ID3D11RasterizerState* GetRasterizerState(int num);
	void SetCull(int nCullMode);
	float GetMouseWheel();
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
DXWindow3D* GetMainWindow();
int GetMainWindowFPS();
void SetCullMode(int nCullMode);
void EndCurrentGame();
BOOL OnMouseWheelDX(HWND hWnd, UINT nFlags, short zDelta, POINTS pt);
float GetMouseWheelMove();