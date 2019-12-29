#include "DebugAim.h"
#include "InputManager.h"
#define DEBUGAIM_MODEL_PATH "data/model/DebugAim.fbx"
#define DA_SPEED 1.95f
#define ROTATION_SPEED	XM_PI*0.02f			// 回転速度

DebugAim::DebugAim(): GameObject3D()
{
	nType = GO_DEBUGAIM;
	Init();
}


DebugAim::~DebugAim()
{
}

void DebugAim::Init()
{
	InitModel(DEBUGAIM_MODEL_PATH);
	SetScale(0.5f);
	Model->SetScale(0.25f);
	Model->SetRotationY(3.14f*0.5f);
}

void DebugAim::Update()
{
	if (!pMainCamera) {
		pMainCamera = GetMainCamera();
		return;
	}
	GameObject3D::Update();
	DebugAimControl();
}

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

void DebugAim::Draw()
{
	GameObject3D::Draw();
}

void DebugAim::End()
{
}
