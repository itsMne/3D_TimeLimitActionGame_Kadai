#include "S_InGame3D.h"
#include "debugproc.h"
#include "Field3D.h"
#include "Cube3D.h"
#include "Billboard2D.h"




Field3D* HelloField;
Cube3D* HelloCube;


SceneInGame3D::SceneInGame3D() :Scene3D(true)
{
	MainWindow = GetMainWindow();
	MainWindow->SetWindowColor255(150, 71, 89);
	pPlayer = new Player3D();
	HelloField = new Field3D();
	pSkybox = new Sphere3D("data/texture/Skybox.tga");
	HRESULT	hr;
	g_pDevice = GetDevice();
	hr = HelloField->Init(pSceneLight, "data/texture/field000.jpg");
	HelloField->SetTextureSubdivisions(3); 
	HelloField->SetScale(100);
	hr = InitDebugProc();
	pSceneCamera->SetFocalPointGO(pPlayer);
	pUI = new InGameUI2D();

	

	HelloCube = new Cube3D();
	HelloCube->Init("data/texture/hbox.tga");
	HelloCube->SetScale(10);
}


SceneInGame3D::~SceneInGame3D()
{
	End();
}

void SceneInGame3D::Init()
{
}

eSceneType SceneInGame3D::Update()
{
	Scene3D::Update();
	// デバッグ文字列表示更新
	UpdateDebugProc();

	// デバッグ文字列設定
	StartDebugProc();
	PrintDebugProc("FPS:%d\n\n", GetMainWindowFPS());

	// モデル更新
	pPlayer->Update();

	// フィールド更新
	HelloField->UpdateField();

	//HelloShadow->Update();

	pUI->Update();

	pSkybox->Update();
	
	 
	HelloCube->Update();

	return SCENE_IN_GAME;
}

void SceneInGame3D::Draw()
{
	// Zバッファ有効
	SetZBuffer(true);

	// 前面カリング (FBXは表裏が反転するため)
	if(!pDeviceContext)
		pDeviceContext = MainWindow->GetDeviceContext();
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(1));

	SetCullMode(CULLMODE_NONE);
	// モデル描画
	pPlayer->Draw();
	SetCullMode(CULLMODE_CCW);

	// 背面カリング (通常は表面のみ描画)
	pDeviceContext->RSSetState(MainWindow->GetRasterizerState(2));

	pSkybox->Draw();
	// フィールド描画
	HelloField->DrawField();
	SetCullMode(CULLMODE_NONE);
	HelloCube->Draw();
	SetCullMode(CULLMODE_CCW);
	// Zバッファ無効
	SetZBuffer(false);

	// デバッグ文字列表示
	DrawDebugProc();
	
	
	pUI->Draw();
}

void SceneInGame3D::End()
{
	Scene3D::End();
	// フィールド終了処理
	SAFE_DELETE(HelloField);
	// モデル表示終了処理
	SAFE_DELETE(pPlayer);
	// デバッグ文字列表示終了処理
	UninitDebugProc();

	SAFE_DELETE(pUI);

	//SAFE_DELETE(HelloBill);
}
