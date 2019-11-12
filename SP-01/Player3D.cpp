#include "Player3D.h"
#include "InputManager.h"
#include "debugproc.h"
#define PLAYER_MODEL_PATH "data/model/NinaModel.fbx"
#define PLAYER_SPEED	2.5f					// 移動速度
#define ROTATION_SPEED	XM_PI*0.02f			// 回転速度
#define PLAYER_SCALE	0.5f
#define BULLET_COOLDOWN 5.0f

Player3D::Player3D():GameObject3D(GetMainLight(), PLAYER_MODEL_PATH, GO_PLAYER)
{
	mShadow = nullptr;
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = nullptr;
	}
	Init();
}

Player3D::Player3D(Light3D * Light):GameObject3D(Light, PLAYER_MODEL_PATH, GO_PLAYER)
{
	Init();
}


Player3D::~Player3D()
{
}

void Player3D::Init()
{
	SetScale(PLAYER_SCALE);
	printf("%f\n", GetModel()->GetPosition().y);
	nType = GO_PLAYER;
	nShootCooldown = 0;
	pDeviceContext = GetDeviceContext();
	pCurrentWindow = GetMainWindow();
	mShadow = new GameObject3D("data/model/Shadow.fbx", GO_SHADOW);
	mShadow->SetRotation({ 0,0,0 });
	mShadow->SetParent(this);
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		goBullets[i] = new GameObject3D("data/model/Bullet.fbx", GO_BULLET);
		goBullets[i]->SetUse(false);
	}
}

void Player3D::Update()
{
	GameObject3D::Update();
	XMFLOAT3 rotCamera;
	// カメラの向き取得
	if (!pMainCamera)
		pMainCamera = GetMainCamera();
	rotCamera = pMainCamera->GetCameraAngle();
	static int NumTest = 0;
	/*if (GetKeyTrigger(VK_SPACE))
		NumTest++;
	if (NumTest >= Model->GetNumberOfAnimations())
		NumTest = BULLET_COOLDOWN;*/
	if (!GetInput(INPUT_FORWARD) && !GetInput(INPUT_BACKWARD) && !GetInput(INPUT_RIGHT) && !GetInput(INPUT_LEFT) && !GetInput(INPUT_AIM))
		Model->SwitchAnimation(NumTest, 5);
	if (GetInput(INPUT_LEFT)) {
		if (GetInput(INPUT_FORWARD)) {
			// 左前移動
			Position.x -= sinf(rotCamera.y + XM_PI * 0.75f) * PLAYER_SPEED;
			Position.z -= cosf(rotCamera.y + XM_PI * 0.75f) * PLAYER_SPEED;
			Model->SwitchAnimation(1, 2);
			Model->SetRotation({ 0,-0.942f,0 });
		}
		else if (GetInput(INPUT_BACKWARD)) {
			// 左後移動
			Position.x -= sinf(rotCamera.y + XM_PI * 0.25f) * PLAYER_SPEED;
			Position.z -= cosf(rotCamera.y + XM_PI * 0.25f) * PLAYER_SPEED;
			Model->SwitchAnimation(1, 2);
			Model->SetRotation({ 0,-2.450f,0 });
		}
		else {
			// 左移動
			Position.x -= sinf(rotCamera.y + XM_PI * 0.50f) * PLAYER_SPEED;
			Position.z -= cosf(rotCamera.y + XM_PI * 0.50f) * PLAYER_SPEED;
			Model->SwitchAnimation(1, 2);
			Model->SetRotation({ 0,4.696f,0 });
		}
	}
	else if (GetInput(INPUT_RIGHT)) {
		if (GetInput(INPUT_FORWARD)) {
			// 右前移動
			Position.x -= sinf(rotCamera.y - XM_PI * 0.75f) * PLAYER_SPEED;
			Position.z -= cosf(rotCamera.y - XM_PI * 0.75f) * PLAYER_SPEED;
			Model->SwitchAnimation(1, 2);
			Model->SetRotation({ 0,0.942f,0 });
		}
		else if (GetInput(INPUT_BACKWARD)) {
			// 右後移動
			Position.x -= sinf(rotCamera.y - XM_PI * 0.25f) * PLAYER_SPEED;
			Position.z -= cosf(rotCamera.y - XM_PI * 0.25f) * PLAYER_SPEED;
			Model->SwitchAnimation(1, 2);
			Model->SetRotation({ 0,2.450f,0 });
		}
		else {
			// 右移動
			Position.x -= sinf(rotCamera.y - XM_PI * 0.50f) * PLAYER_SPEED;
			Position.z -= cosf(rotCamera.y - XM_PI * 0.50f) * PLAYER_SPEED;
			Model->SwitchAnimation(1, 2);
			Model->SetRotation({ 0,1.68f,0 });
		}
	}
	else if (GetInput(INPUT_FORWARD)) {
		// 前移動
		Position.x -= sinf(XM_PI + rotCamera.y) * PLAYER_SPEED;
		Position.z -= cosf(XM_PI + rotCamera.y) * PLAYER_SPEED;
		Model->SwitchAnimation(1, 2);
		Model->SetRotation({0,0,0});
	}
	else if (GetInput(INPUT_BACKWARD)) {
		// 後移動
		Position.x -= sinf(rotCamera.y) * PLAYER_SPEED;
		Position.z -= cosf(rotCamera.y) * PLAYER_SPEED;
		Model->SwitchAnimation(1, 2);
		Model->SetRotation({ 0,3.314f,0 });
	}

	if (GetKeyPress(VK_LSHIFT)) {
		// 左回転
		RotateAroundY(-ROTATION_SPEED);
		Model->RotateAroundY(ROTATION_SPEED);
	}
	if (GetKeyPress(VK_RSHIFT)) {
		// 右回転
		RotateAroundY(ROTATION_SPEED);
		Model->RotateAroundY(-ROTATION_SPEED);
	}

	if (GetKeyPress(VK_RETURN)) {
		// リセット
		Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	PrintDebugProc("[ｸﾙﾏ ｲﾁ : (%f, %f, %f)]\n", Position.x, Position.y, Position.z);
	PrintDebugProc("[ｸﾙﾏ ﾑｷ : (%f)]\n", XMConvertToDegrees(Rotation.y));
	PrintDebugProc("\n");

	PrintDebugProc("*** ｸﾙﾏ ｿｳｻ ***\n");
	PrintDebugProc("ﾏｴ   ｲﾄﾞｳ : \x1e\n");//↑
	PrintDebugProc("ｳｼﾛ  ｲﾄﾞｳ : \x1f\n");//↓
	PrintDebugProc("ﾋﾀﾞﾘ ｲﾄﾞｳ : \x1d\n");//←
	PrintDebugProc("ﾐｷﾞ  ｲﾄﾞｳ : \x1c\n");//→
	PrintDebugProc("ﾋﾀﾞﾘ ｾﾝｶｲ : LSHIFT\n");
	PrintDebugProc("ﾐｷﾞ  ｾﾝｶｲ : RSHIFT\n");
	PrintDebugProc("ROT : %f\n", sinf(XM_PI+rotCamera.y));
	PrintDebugProc("\n");

	PlayerBulletsControl();
	PlayerShadowControl();
}

void Player3D::PlayerShadowControl()
{
	if (!mShadow)
		return;
	mShadow->Update();
	mShadow->SetRotation(Model->GetRotation());
}

void Player3D::PlayerBulletsControl()
{
	if (GetInput(INPUT_AIM))
	{
		GetModel()->SetRotation({0,0,0});
		Model->SwitchAnimation(2, 5);
		
	}
	if (GetInput(INPUT_SHOOT) && GetInput(INPUT_AIM) && ++nShootCooldown>= BULLET_COOLDOWN) {
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			if (!goBullets[i])
				continue;
			if (goBullets[i]->IsInUse())
				continue;
			XMFLOAT3 rotCamera;
			rotCamera = pMainCamera->GetCameraAngle();
			goBullets[i]->SetUse(true);
			XMFLOAT3 ModelRotation = Rotation;
			goBullets[i]->SetRotation(ModelRotation);
			goBullets[i]->SetPosition({ Position.x+ sinf(rotCamera.y) *15, Position.y+15, Position.z+ cosf(rotCamera.y)*15 });
			nShootCooldown = 0;
			break;
		}
	}
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!goBullets[i])
			continue;
		goBullets[i]->Update();
	}
}

void Player3D::Draw()
{
	pDeviceContext->RSSetState(pCurrentWindow->GetRasterizerState(1));
	GameObject3D::Draw();
	pDeviceContext->RSSetState(pCurrentWindow->GetRasterizerState(2));
	if(mShadow)
		mShadow->Draw();
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!goBullets[i])
			continue;
		goBullets[i]->Draw();
	}
	
}

void Player3D::End()
{
	GameObject3D::End();
	mShadow->End();
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!goBullets[i])
			continue;
		delete(goBullets[i]);
		goBullets[i] = nullptr;

	}
}
