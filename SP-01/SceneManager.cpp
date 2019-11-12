#include "SceneManager.h"
#include "DXWindow3D.h"
#include "debugproc.h"
#include "Camera3D.h"
#include "Light3D.h"
#include "Field3D.h"
#include "Player3D.h"
ID3D11Device* g_pDevice = nullptr;
Camera3D* SceneCamera = nullptr;
Light3D* SceneLight = nullptr;
Player3D* HelloModel = nullptr;
Field3D* HelloField = nullptr;
DXWindow3D* MainWindow = nullptr;
GameObject3D* HelloShadow = nullptr;
HRESULT InitScene()
{
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	SceneCamera = new Camera3D(true);
	SceneLight = new Light3D(true);
	HelloModel = new Player3D();
	HelloField = new Field3D();
	HelloShadow = new GameObject3D(GO_MAX);
	HelloShadow->SetParent(HelloModel);
	HRESULT	hr;
	g_pDevice = GetDevice();
	hr = HelloField->InitField(SceneLight, "data/texture/field000.jpg");
	hr = InitDebugProc();
	SceneCamera->SetFocalPointGO(HelloModel);
	return hr;
}

void UpdateScene()
{
	// デバッグ文字列表示更新
	UpdateDebugProc();

	// デバッグ文字列設定
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", GetMainWindowFPS());

	// 光源更新
	SceneLight->Update();

	// カメラ更新
	SceneCamera->Update();

	// モデル更新
	HelloModel->Update();

	// フィールド更新
	HelloField->UpdateField();

	HelloShadow->Update();
}

void DrawScene()
{
	DXWindow3D* pMainWindow = GetMainWindow();
	// Zバッファ有効
	SetZBuffer(true);

	// 前面カリング (FBXは表裏が反転するため)
	ID3D11DeviceContext* pDeviceContext = pMainWindow->GetDeviceContext();
	pDeviceContext->RSSetState(pMainWindow->GetRasterizerState(1));

	// モデル描画
	HelloModel->Draw();

	// 背面カリング (通常は表面のみ描画)
	pDeviceContext->RSSetState(pMainWindow->GetRasterizerState(2));

	// フィールド描画
	HelloField->DrawField();

	HelloShadow->Draw();
	// Zバッファ無効
	SetZBuffer(false);

	// デバッグ文字列表示
	DrawDebugProc();
}

void EndScene()
{
	// フィールド終了処理
	HelloField->UninitField();
	// モデル表示終了処理
	HelloModel->End();

	// 光源終了処理
	SceneLight->End();

	HelloShadow->End();
	// カメラ終了処理
	if(SceneCamera)
		SceneCamera->End();

	// デバッグ文字列表示終了処理
	UninitDebugProc();
}
