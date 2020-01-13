#include "DXWindow3D.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Sound.h"
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#pragma comment(lib, "winmm")
#pragma comment(lib, "imm32")
#pragma comment(lib, "d3d11")
#define CLASS_NAME		_T("AppClass")		// ウインドウのクラス名
#define WINDOW_NAME		_T("カメラ処理")	// ウインドウのキャプション名

DXWindow3D* pMainWindow = nullptr;
bool bGameIsEnd;

DXWindow3D::DXWindow3D(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow, bool bIsMainWindow)
{
	bGameIsEnd = false;
	fR = 0;
	fG = 0;
	fB = 0;
	if (bIsMainWindow)
		pMainWindow = this;
	ActivateConsole();
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）
	g_pDevice = nullptr;
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		DXWindow3D::DXWndProc,
		0,
		0,
		hInstance,
		nullptr,
		LoadCursor(nullptr, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		nullptr,
		CLASS_NAME,
		nullptr
	};

	// COM初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
		MessageBoxW(NULL, L"COMの初期化に失敗しました。", L"error", MB_OK);
		return;
	}

	// インスタンス ハンドル保存
	g_hInst = hInstance;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域サイズからウィンドウ サイズ算出
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION
		| WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	DWORD dwExStyle = 0;
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRectEx(&rc, dwStyle, FALSE, dwExStyle);

	// ウィンドウの作成
	g_hWnd = CreateWindowEx(dwExStyle,
		CLASS_NAME,
		WINDOW_NAME,
		dwStyle,
		CW_USEDEFAULT,		// ウィンドウの左座標
		CW_USEDEFAULT,		// ウィンドウの上座標
		rc.right - rc.left,	// ウィンドウ横幅
		rc.bottom - rc.top,	// ウィンドウ縦幅
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	// フレームカウント初期化
	timeBeginPeriod(1);				// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	// DirectXの初期化(ウィンドウを作成してから行う)
	if (FAILED(Init(g_hWnd, true))) {
		return;
	}
}


DXWindow3D::~DXWindow3D()
{
	Uninit();
}

//=============================================================================
// バックバッファ生成
//=============================================================================

HRESULT DXWindow3D::CreateBackBuffer(void)
{
	// レンダーターゲットビュー生成
	ID3D11Texture2D* pBackBuffer = nullptr;
	g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	g_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	// Zバッファ用テクスチャ生成
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = SCREEN_WIDTH;
	td.Height = SCREEN_HEIGHT;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	HRESULT hr = g_pDevice->CreateTexture2D(&td, nullptr, &g_pDepthStencilTexture);
	if (FAILED(hr)) {
		return hr;
	}

	// Zバッファターゲットビュー生成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = g_pDevice->CreateDepthStencilView(g_pDepthStencilTexture,
		&dsvd, &g_pDepthStencilView);
	if (FAILED(hr)) {
		return hr;
	}

	// 各ターゲットビューをレンダーターゲットに設定
	g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (float)SCREEN_WIDTH;
	vp.Height = (float)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pDeviceContext->RSSetViewports(1, &vp);

	return S_OK;
}

HRESULT DXWindow3D::Init(HWND hWnd, BOOL bWindow)
{
	hr = S_OK;

	// デバイス、スワップチェーンの作成
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Width = SCREEN_WIDTH;
	scd.BufferDesc.Height = SCREEN_HEIGHT;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.Windowed = bWindow;

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, 0, featureLevels, _countof(featureLevels), D3D11_SDK_VERSION, &scd,
		&g_pSwapChain, &g_pDevice, nullptr, &g_pDeviceContext);
	if (FAILED(hr)) {
		return hr;
	}

	// バックバッファ生成
	hr = CreateBackBuffer();
	if (FAILED(hr)) {
		return hr;
	}

	// ラスタライズ設定
	D3D11_RASTERIZER_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE;	// カリング無し(両面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[0]);
	rd.CullMode = D3D11_CULL_FRONT;	// 前面カリング(裏面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[1]);
	rd.CullMode = D3D11_CULL_BACK;	// 背面カリング(表面描画)
	g_pDevice->CreateRasterizerState(&rd, &g_pRs[2]);
	g_pDeviceContext->RSSetState(g_pRs[2]);

	// ブレンド ステート生成 (アルファ ブレンド用)
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[0]);
	// ブレンド ステート生成 (加算合成用)
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	g_pDevice->CreateBlendState(&BlendDesc, &g_pBlendState[1]);
	SetBlendState(false);

	// 深度ステンシルステート生成
	CD3D11_DEFAULT def;
	CD3D11_DEPTH_STENCIL_DESC dsd(def);
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	g_pDevice->CreateDepthStencilState(&dsd, &g_pDSS);
	InitSound(hWnd);
	return hr;
}

void DXWindow3D::InitDXWindow()
{

	
	hr = InitScene();

	// 入力処理初期化
	InitInputManager();




}

void DXWindow3D::Uninit(void)
{
	EndScene();
	// 入力処理終了処理
	EndInputManager();



	// 深度ステンシルステート解放
	SAFE_RELEASE(g_pDSS);

	// ブレンド ステート解放
	for (int i = 0; i < _countof(g_pBlendState); ++i) {
		SAFE_RELEASE(g_pBlendState[i]);
	}

	// ラスタライザ ステート解放
	for (int i = 0; i < _countof(g_pRs); ++i) {
		SAFE_RELEASE(g_pRs[i]);
	}

	// バックバッファ解放
	ReleaseBackBuffer();

	// スワップチェーン解放
	SAFE_RELEASE(g_pSwapChain);

	// デバイス コンテキストの開放
	SAFE_RELEASE(g_pDeviceContext);

	// デバイスの開放
	SAFE_RELEASE(g_pDevice);

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, g_hInst);

	// COM終了処理
	CoUninitialize();

	timeEndPeriod(1);
}

bool DXWindow3D::UpdateDXWindow()
{
	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			// PostQuitMessage()が呼ばれたらループ終了
			return false;
		}
		else {
			// メッセージの翻訳とディスパッチ
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	else {
		dwCurrentTime = timeGetTime();
		if ((dwCurrentTime - dwFPSLastTime) >= 500) {	// 0.5秒ごとに実行
			g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
			dwFPSLastTime = dwCurrentTime;
			dwFrameCount = 0;
		}
		if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)) {
			dwExecLastTime = dwCurrentTime;
			// 更新処理
			Update();
			// 描画処理
			Draw();
			dwFrameCount++;
		}
		
	}
	
	return true;
}

void DXWindow3D::Update(void)
{
	// 入力処理更新
	UpdateInputManager();	// 必ずUpdate関数の先頭で実行.

	UpdateScene();
}

void DXWindow3D::Draw(void)
{
	// バックバッファ＆Ｚバッファのクリア
	float ClearColor[4] = { fR, fG, fB, 1.0f };
	g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, ClearColor);
	g_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Zバッファ有効
	DrawScene();

	// バックバッファとフロントバッファの入れ替え
	g_pSwapChain->Present(g_uSyncInterval, 0);
}

HWND DXWindow3D::GetMainWnd()
{
	return g_hWnd;
}

HINSTANCE DXWindow3D::GetInstance()
{
	return g_hInst;
}

ID3D11Device * DXWindow3D::GetDevice()
{
	return g_pDevice;
}

ID3D11DeviceContext * DXWindow3D::GetDeviceContext()
{
	return g_pDeviceContext;
}

void DXWindow3D::SetZBuffer(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS, 0);
}

void DXWindow3D::SetZWrite(bool bEnable)
{
	g_pDeviceContext->OMSetDepthStencilState((bEnable) ? nullptr : g_pDSS, 0);
}

void DXWindow3D::SetBlendState(bool bAdd)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_pDeviceContext->OMSetBlendState(g_pBlendState[(bAdd) ? 1 : 0], blendFactor, 0xffffffff);
}

void DXWindow3D::ReleaseBackBuffer()
{
	if (g_pDeviceContext) {
		g_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}
	SAFE_RELEASE(g_pDepthStencilView);
	SAFE_RELEASE(g_pDepthStencilTexture);
	SAFE_RELEASE(g_pRenderTargetView);
}

int DXWindow3D::DX_OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
	// クライアント領域サイズをSCREEN_WIDTH×SCREEN_HEIGHTに再設定.
	RECT rcClnt;
	GetClientRect(hWnd, &rcClnt);
	rcClnt.right -= rcClnt.left;
	rcClnt.bottom -= rcClnt.top;
	if (rcClnt.right != SCREEN_WIDTH || rcClnt.bottom != SCREEN_HEIGHT) {
		RECT rcWnd;
		GetWindowRect(hWnd, &rcWnd);
		SIZE sizeWnd;
		sizeWnd.cx = (rcWnd.right - rcWnd.left) - rcClnt.right + SCREEN_WIDTH;
		sizeWnd.cy = (rcWnd.bottom - rcWnd.top) - rcClnt.bottom + SCREEN_HEIGHT;
		SetWindowPos(hWnd, nullptr, 0, 0, sizeWnd.cx, sizeWnd.cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	}

	// IME無効化
	ImmAssociateContext(hWnd, nullptr);

	return 0;	// -1を返すとCreateWindow[Ex]が失敗する.
}

LRESULT DXWindow3D::DXWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:					//----- ウィンドウが生成された
		return DXWindow3D::DX_OnCreate(hWnd, (LPCREATESTRUCT)lParam);
	case WM_DESTROY:				//----- ウィンドウ破棄指示がきた
		PostQuitMessage(0);				// システムにスレッドの終了を要求
		break;
	case WM_KEYDOWN:				//----- キーボードが押された
		switch (wParam) {
		case VK_ESCAPE:					// [ESC]キーが押された
			PostMessage(hWnd, WM_CLOSE, 0, 0);	// [x]が押されたように振舞う
			return 0;
		}
		break;
	case WM_MENUCHAR:
		return MNC_CLOSE << 16;			// [Alt]+[Enter]時のBEEPを抑止
	default:
		break;
	}
	if (bGameIsEnd)
	{
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void DXWindow3D::SetWindowColor(float R, float G, float B)
{
	fR = R;
	fG = G;
	fB = B;
}

void DXWindow3D::SetWindowColor255(int R, int G, int B)
{
	fR = R/255.0f;
	fG = G/255.0f;
	fB = B/255.0f;
}

XMFLOAT3 DXWindow3D::GetWindowColor()
{
	return { fR, fG, fB };
}

void DXWindow3D::ActivateConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((long)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	printf("こんにちは！コンソールはONです！\n");
}

int DXWindow3D::GetFPS()
{
	return g_nCountFPS;
}

ID3D11RasterizerState * DXWindow3D::GetRasterizerState(int num)
{
	return g_pRs[num];
}

//=============================================================================
// カリング設定
//=============================================================================
void DXWindow3D::SetCull(int nCullMode)
{
	if (nCullMode >= 0 && nCullMode < _countof(g_pRs)) {
		GetDeviceContext()->RSSetState(g_pRs[nCullMode]);
	}
}

DXWindow3D * GetMainWindow()
{
	return pMainWindow;
}

int GetMainWindowFPS()
{
	if (!pMainWindow)
		return 0;
	return pMainWindow->GetFPS();
}

//=============================================================================
// カリング設定
//=============================================================================
void SetCullMode(int nCullMode)
{
	if (GetMainWindow())
	{
		GetMainWindow()->SetCull(nCullMode);
	}
}

//*****************************************************************************
//EndCurrentGame関数
//ゲームが終わる変数を管理する
//引数：void
//戻：void
//*****************************************************************************
void EndCurrentGame()
{
	bGameIsEnd = true;
}
