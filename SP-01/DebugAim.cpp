//*****************************************************************************
// DebugAim.h
// デバッグアイムを管理する
// Author : Mane
//*****************************************************************************
#include "DebugAim.h"
#include "S_InGame3D.h"
#include "Field3D.h"
#include "Light3D.h"
#include "Wall3D.h"
#include "Player3D.h"
#include "Enemy3D.h"
#include "Camera3D.h"
#include "InputManager.h"

//*****************************************************************************
// マクロ
//*****************************************************************************
#define DEBUGAIM_MODEL_PATH "data/model/DebugAim.fbx"
#define DA_SPEED 1.95f
#define ROTATION_SPEED	XM_PI*0.02f			// 回転速度
#define SCALE_VALUE 1
#define ZOOM_VALUE 1

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Camera3D* MainCam = nullptr;

//*****************************************************************************
// エナム
//*****************************************************************************
enum DebugAimType//作りたいオブジェクトの種類
{
	DA_DEBUG_AIM = 0,
	DA_FLOOR,
	DA_WALL,
	DA_ENEMIES,
	DA_MAX
};

//*****************************************************************************
//コンストラクタ関数
//*****************************************************************************
DebugAim::DebugAim(): GameObject3D()
{
	Init();
}


DebugAim::~DebugAim()
{
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::Init()
{
	InitModel(DEBUGAIM_MODEL_PATH);
	SetScale(0.5f);
	Model->SetScale(0.25f);
	Model->SetRotationY(3.14f*0.5f);
	DA_Obj = nullptr;
	pPlayer = nullptr;
	pCGame = nullptr;
	pFloors = nullptr;
	pWalls = nullptr;
	pEnemies = nullptr;
	nType = GO_DEBUGAIM;
	nTypeObj = DA_DEBUG_AIM;
	x3dAScale = { 1,1,1 };
	MainCam = GetMainCamera();
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::Update()
{
	if (!pMainCamera) {
		pMainCamera = GetMainCamera();
		if (!pMainCamera)
			return;
	}
	if (!pPlayer)
		pPlayer = GetPlayer3D();
	if (!pCGame) {
		pCGame = GetCurrentGame();
		if (!pCGame)
			return;
	}
	S_InGame3D* pGame = (S_InGame3D*)pCGame;
	GameObject3D::Update();
	DebugAimControl();
	ScaleControl();
	if (!pFloors)
		pFloors = pGame->GetList(GO_FLOOR);
	if (!pWalls)
		pWalls = pGame->GetList(GO_WALL);
	if (!pEnemies)
		pEnemies = pGame->GetList(GO_ENEMY);
	if (GetInput(INPUT_SAVE_LEVEL))
	{
		if(pFloors)
			pFloors->SaveFields("Level_Floors");		
		if(pWalls)
			pWalls->SaveWalls("Level_Walls");
		if (pEnemies)
			pEnemies->SaveEnemies("Level_Enemies");
	}
	if (GetInput(INPUT_SWITCH_DEBUGOBJ)) {
		nTypeObj++;
		SAFE_DELETE(DA_Obj);
	}
	if (GetInput(INPUT_SWITCH_DEBUGOBJALT)) {
		nTypeObj--;
		SAFE_DELETE(DA_Obj);
	}
	if (nTypeObj < 0)
		nTypeObj = DA_MAX - 1;
	if (nTypeObj >= DA_MAX)
		nTypeObj = 0;
	switch (nTypeObj)
	{
	case DA_DEBUG_AIM:
		if (GetInput(INPUT_DEBUG_CONFIRM) && pPlayer) {
			pPlayer->SetPosition(Position);
			printf("PLAYER POS: (%f, %f, %f)\n", Position.x, Position.y, Position.z);
		}
		SetScale(0.5f);
		break;
	case DA_FLOOR:
		if (!DA_Obj)
		{
			DA_Obj = new Field3D();
			((Field3D*)DA_Obj)->Init(GetMainLight(), "data/texture/field000.jpg");
			x3dAScale = { 10,10,10 };
		}
		else {
			DA_Obj->SetScale(x3dAScale);
			DA_Obj->SetPosition(Position);
			if(pFloors && GetInput(INPUT_DEBUG_CONFIRM))
				pFloors->AddField(Position, x3dAScale, "data/texture/field000.jpg");
			if (pFloors && GetInput(INPUT_DEBUGAIM_DELETE))
				pFloors->DeleteLastPosObject();
		}
		break;
	case DA_WALL:
		if (!DA_Obj)
		{
			DA_Obj = new Wall3D();
			x3dAScale = { 10,10,10 };
		}
		else {
			DA_Obj->SetScale(x3dAScale);
			DA_Obj->SetPosition(Position);
			if (pWalls && GetInput(INPUT_DEBUG_CONFIRM))
				pWalls->AddWall(Position, x3dAScale);
			if (pWalls && GetInput(INPUT_DEBUGAIM_DELETE))
				pWalls->DeleteLastPosObject();
		}
		break;
	case DA_ENEMIES:
		if (!DA_Obj)
		{
			DA_Obj = new Enemy3D();
		}
		else {
			DA_Obj->SetPosition(Position);
			if (pEnemies && GetInput(INPUT_DEBUG_CONFIRM))
				pEnemies->AddEnemy(Position);
			if (pEnemies && GetInput(INPUT_DEBUGAIM_DELETE))
				pEnemies->DeleteLastPosObject();
		}
		break;
	default:
		break;
	}
	if (DA_Obj)
		DA_Obj->Update();
}

//*****************************************************************************
//ScaleControl関数
//スケールの管理
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::ScaleControl()
{
	if (GetInput(INPUT_SCALEUP_X))
		x3dAScale.x += SCALE_VALUE;
	if (GetInput(INPUT_SCALEDOWN_X))
		x3dAScale.x -= SCALE_VALUE;
	if (GetInput(INPUT_SCALEUP_Y))
		x3dAScale.y += SCALE_VALUE;
	if (GetInput(INPUT_SCALEDOWN_Y))
		x3dAScale.y -= SCALE_VALUE;
	if (GetInput(INPUT_SCALEUP_Z))
		x3dAScale.z += SCALE_VALUE;
	if (GetInput(INPUT_SCALEDOWN_Z))
		x3dAScale.z -= SCALE_VALUE;
	if (x3dAScale.x <= 0)
		x3dAScale.x = 0.1f;
	if (x3dAScale.y <= 0)
		x3dAScale.y = 0.1f;
	if (x3dAScale.z <= 0)
		x3dAScale.z = 0.1f;
	if (!MainCam) {
		MainCam = GetMainCamera();
		if (!MainCam)
			return;
	}
	if (GetInput(INPUT_DEBUGAIM_ZOOMIN))
		MainCam->ZoomIn(ZOOM_VALUE);
	if (GetInput(INPUT_DEBUGAIM_ZOOMOUT))
		MainCam->ZoomIn(-ZOOM_VALUE);

}

//*****************************************************************************
//DebugAimControl関数
//デバッグアイムの拠点を管理する
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::DebugAimControl()
{
	if (GetAxis(CAMERA_AXIS_HORIZONTAL) != 0)
		RotateAroundY(-ROTATION_SPEED * GetAxis(CAMERA_AXIS_HORIZONTAL));
	if (GetAxis(CAMERA_AXIS_VERTICAL) != 0)
		TranslateY(DA_SPEED * GetAxis(CAMERA_AXIS_VERTICAL));
	XMFLOAT3 rotCamera;
	float fHorizontalAxis = GetAxis(MOVEMENT_AXIS_HORIZONTAL);
	float fVerticalAxis = GetAxis(MOVEMENT_AXIS_VERTICAL);
	rotCamera = pMainCamera->GetCameraAngle();
	//スティック
	float nModelRotation = -(atan2(fVerticalAxis, fHorizontalAxis) - 1.570796f);
	if (fVerticalAxis != 0) {

		Position.x -= sinf(XM_PI + rotCamera.y) * DA_SPEED * fVerticalAxis;
		Position.z -= cosf(XM_PI + rotCamera.y) * DA_SPEED * fVerticalAxis;
	}
	if (fHorizontalAxis != 0) {

		Position.x -= sinf(rotCamera.y - XM_PI * 0.50f) * DA_SPEED * fHorizontalAxis;
		Position.z -= cosf(rotCamera.y - XM_PI * 0.50f) * DA_SPEED * fHorizontalAxis;
	}
	Model->SetRotationY(rotCamera.y + (3.14f*0.5f));
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::Draw()
{
	if(!DA_Obj)
		GameObject3D::Draw();
	else {
		DA_Obj->Draw();
	}
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void DebugAim::End()
{
	SAFE_DELETE(DA_Obj);
}
