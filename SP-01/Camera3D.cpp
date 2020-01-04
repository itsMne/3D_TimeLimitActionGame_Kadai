#include "Camera3D.h"	
#include "input.h"
#include "Model3D.h"
#include "GameObject3D.h"
#include "InputManager.h"
#include <stdio.h>
#define POS_CAMERA_P_X		(0.0f)
#define POS_CAMERA_P_Y		(100.0f)
#define POS_CAMERA_P_Z		(-200.0f)

#define POS_CAMERA_R_X		(0.0f)
#define POS_CAMERA_R_Y		(0.0f)
#define POS_CAMERA_R_Z		(0.0f)

#define VIEW_ANGLE			(XMConvertToRadians(45.0f))
#define VIEW_ASPECT			((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT)
#define VIEW_NEAR			(10.0f)
#define VIEW_FAR			(1000.0f)
#define VALUE_MOVE_CAMERA	(2.0f)	
#define VALUE_ROTATE_CAMERA	(XM_PI*0.01f)	
#define REGULAR_OBJECT_DISTANCE {0, 25, -142}
#define AIM_OBJECT_DISTANCE {20, 35, -40}
#define AIM_OBJECT_DISTANCE {10, 35, -40}
Camera3D* MainCamera = nullptr;
bool bFocalPointIsGameObject = false;
Camera3D::Camera3D(): FocalPoint(nullptr)
{
	MainCamera = this;
	Init();
}

Camera3D::Camera3D(bool bisMainCamera) : FocalPoint(nullptr)
{
	if (bisMainCamera)
		MainCamera = this;
	Init();
}


Camera3D::~Camera3D()
{
	End();
}

HRESULT Camera3D::Init()
{
	g_posCameraP = XMFLOAT3(POS_CAMERA_P_X, POS_CAMERA_P_Y, POS_CAMERA_P_Z);
	g_posCameraR = XMFLOAT3(POS_CAMERA_R_X, POS_CAMERA_R_Y, POS_CAMERA_R_Z);
	g_vecCameraU = XMFLOAT3(0, 1, 0);
	g_rotCamera = XMFLOAT3(0, 0, 0);
	vEye = REGULAR_OBJECT_DISTANCE;
	vEye = REGULAR_OBJECT_DISTANCE;
	vsOffset = { 0,0,0 };
	float fVecX, fVecZ;
	fVecX = g_posCameraP.x - g_posCameraR.x;
	fVecZ = g_posCameraP.z - g_posCameraR.z;
	g_fLengthInterval = sqrtf(fVecX * fVecX + fVecZ * fVecZ);
	fAcceleration = 0;
	return S_OK;
}

void Camera3D::Update()
{
	
	
	if (FocalPoint && !bFocalPointIsGameObject) {
		Model3D* FocusPoint = (Model3D*)FocalPoint;
		//視点
		const XMFLOAT3 vLookAt = XMFLOAT3(0, 25, 0);//注視点
		XMMATRIX mtxWorld = XMLoadFloat4x4(FocusPoint->GetModelWorld());

		//視点
		XMStoreFloat3(&g_posCameraP, XMVector3TransformCoord(XMLoadFloat3(&vEye), mtxWorld));

		//注視点
		XMStoreFloat3(&g_posCameraR, XMVector3TransformCoord(XMLoadFloat3(&vLookAt), mtxWorld));
		
		g_rotCamera = FocusPoint->GetRotation();
	}
	else if (FocalPoint && bFocalPointIsGameObject)
	{
		GameObject3D* FocusPoint = ((GameObject3D*)FocalPoint);
		//Model3D* FocusPoint = ((GameObject3D*)FocalPoint)->GetModel();
		XMFLOAT3 vLookAt = XMFLOAT3(vEye.x, vEye.y, 0);//注視点
		if (FocusPoint->GetType() == GO_PLAYER && GetInput(INPUT_AIM))
		{
			XMFLOAT3 v_NEye = AIM_OBJECT_DISTANCE;
			fAcceleration++;
			if (vEye.z < v_NEye.z)
				vEye.z += fAcceleration;
			if (vEye.z > v_NEye.z) 
				vEye.z = v_NEye.z;
			if (vEye.x < v_NEye.x)
				vEye.x += fAcceleration;
			if (vEye.x > v_NEye.x) 
				vEye.x = v_NEye.x;
			if (vEye.y < v_NEye.y)
				vEye.y += fAcceleration;
			if (vEye.y > v_NEye.y)
				vEye.y = v_NEye.y;
			if (vEye.y == v_NEye.y && vEye.z == v_NEye.z && vEye.x == v_NEye.x)
				fAcceleration = 0;
		}
		else if (FocusPoint->GetType() == GO_PLAYER && !GetInput(INPUT_AIM)) {
			XMFLOAT3 v_NEye = REGULAR_OBJECT_DISTANCE;
			fAcceleration+=2;
			if (vEye.z > v_NEye.z)
				vEye.z -= fAcceleration;
			if (vEye.z < v_NEye.z)
				vEye.z = v_NEye.z;

			if (vEye.x > v_NEye.x)
				vEye.x -= fAcceleration;
			if (vEye.x <  v_NEye.x)
				vEye.x =  v_NEye.x;
			if (vEye.y > v_NEye.y)
				vEye.y -= fAcceleration;
			if (vEye.y <  v_NEye.y)
				vEye.y =  v_NEye.y;
			if (vEye.y == v_NEye.y && vEye.z == v_NEye.z && vEye.x == v_NEye.x)
				fAcceleration = 0;
			
		}
		if(!FocusPoint->GetType() == GO_PLAYER)
			vEye = REGULAR_OBJECT_DISTANCE;
		XMMATRIX mtxWorld = XMLoadFloat4x4(FocusPoint->GetModelWorld());
		vEye.x += vsOffset.x;
		vEye.y += vsOffset.y;
		vEye.z += vsOffset.z;

		vLookAt.x += vsOffset.x;
		vLookAt.y += vsOffset.y;

		//視点
		XMStoreFloat3(&g_posCameraP, XMVector3TransformCoord(XMLoadFloat3(&vEye), mtxWorld));

		//注視点
		XMStoreFloat3(&g_posCameraR, XMVector3TransformCoord(XMLoadFloat3(&vLookAt), mtxWorld));

		g_rotCamera = FocusPoint->GetRotation();
	}

	/*
	
	if (GetKeyPress(VK_A))
	{
		if (GetKeyPress(VK_W))
		{
			g_posCameraP.x -= cosf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
			g_posCameraP.z += sinf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
		}
		else if (GetKeyPress(VK_S))
		{
			g_posCameraP.x -= cosf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
			g_posCameraP.z += sinf(g_rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_CAMERA;
		}
		else
		{
			g_posCameraP.x -= cosf(g_rotCamera.y) * VALUE_MOVE_CAMERA;
			g_posCameraP.z += sinf(g_rotCamera.y) * VALUE_MOVE_CAMERA;
		}

		//g_posCameraP.x = g_posCameraP.x + sinf(g_rotCamera.y) * g_fLengthInterval;
		//g_posCameraP.z = g_posCameraP.z + cosf(g_rotCamera.y) * g_fLengthInterval;
	}
	if (GetKeyPress(VK_Y))
	{
		g_posCameraP.y += VALUE_MOVE_CAMERA;
	}
	if (GetKeyPress(VK_N))
	{
		g_posCameraP.y -= VALUE_MOVE_CAMERA;
	}
	if (GetKeyPress(VK_Q))
	{
		g_rotCamera.y -= VALUE_ROTATE_CAMERA;
		if (g_rotCamera.y < -XM_PI)
		{
			g_rotCamera.y += XM_2PI;
		}
		g_posCameraR.x = g_posCameraP.x + sinf(g_rotCamera.y) * g_fLengthInterval;
		g_posCameraR.x = g_posCameraP.z + cosf(g_rotCamera.y) * g_fLengthInterval;
	}
	if (GetKeyPress(VK_E))
	{
		g_rotCamera.y += VALUE_ROTATE_CAMERA;
		if (g_rotCamera.y > XM_PI)
		{
			g_rotCamera.y -= XM_2PI;
		}
		g_posCameraR.x = g_posCameraP.x + sinf(g_rotCamera.y) * g_fLengthInterval;
		g_posCameraR.x = g_posCameraP.z + cosf(g_rotCamera.y) * g_fLengthInterval;
	}
	if (GetKeyPress(VK_T))
	{
		g_posCameraR.y += VALUE_MOVE_CAMERA;
	}
	if (GetKeyPress(VK_B))
	{
		g_posCameraR.y -= VALUE_MOVE_CAMERA;
	}
	if (GetKeyTrigger(VK_SPACE))
	{
		g_posCameraP = XMFLOAT3(POS_CAMERA_P_X, POS_CAMERA_P_Y, POS_CAMERA_P_Z);
		g_posCameraR = XMFLOAT3(POS_CAMERA_R_X, POS_CAMERA_R_Y, POS_CAMERA_R_Z);
		g_rotCamera = XMFLOAT3(0, 0, 0);
	}
	*/

	SetCamera();
}

void Camera3D::End()
{
}

void Camera3D::SetCamera()
{
	XMStoreFloat4x4(&g_mtxView, XMMatrixLookAtLH(
		XMVectorSet(g_posCameraP.x, g_posCameraP.y, g_posCameraP.z, 1),
		XMVectorSet(g_posCameraR.x, g_posCameraR.y, g_posCameraR.z, 1),
		XMVectorSet(g_vecCameraU.x, g_vecCameraU.y, g_vecCameraU.z, 0)
	)
	);
	XMStoreFloat4x4(&g_mtxProjection, XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR, VIEW_FAR));
}

XMFLOAT3& Camera3D::GetCameraPos()
{
	return g_posCameraP;
}

XMFLOAT3& Camera3D::GetCameraAngle(void)
{
	return g_rotCamera;
}

XMFLOAT4X4 & Camera3D::GetViewMatrix()
{
	return g_mtxView;
}

XMFLOAT4X4 & Camera3D::GetProjMatrix()
{
	return g_mtxProjection;
}

XMFLOAT3 Camera3D::GetForwardCameraVector()
{
	float pitch = g_rotCamera.y;
	float yaw = g_rotCamera.y;
	return { cosf(pitch)*cosf(yaw),cosf(pitch)*sinf(yaw),sinf(pitch) };
}

void Camera3D::SetFocalPoint(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = false;
}

void Camera3D::SetFocalPointGO(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = true;
}

void Camera3D::ZoomIn(float inc)
{
	vsOffset.z += inc;
}

void Camera3D::ResetOffset()
{
	vsOffset = { 0,0,0 };
}

Camera3D * GetMainCamera()
{
	return MainCamera;
}
