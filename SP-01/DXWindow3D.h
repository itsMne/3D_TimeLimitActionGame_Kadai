#pragma once
#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "model.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "polygon.h"
#include "debugproc.h"
#include "field.h"

#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>
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
	MSG msg;
	CFbxModel*				g_pModel;				// メッシュ
	int							g_nCountFPS;			// FPSカウンタ
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
public:
	DXWindow3D(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
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
	void ActivateConsole();
};

