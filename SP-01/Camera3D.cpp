//*****************************************************************************
// Camera3D.cpp
// �J�������Ǘ�����
// Author : Mane
//*****************************************************************************
#include "Camera3D.h"	
#include "input.h"
#include "Model3D.h"
#include "GameObject3D.h"
#include "InputManager.h"
#include <stdio.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
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

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Camera3D* MainCamera = nullptr;
bool bFocalPointIsGameObject = false;

//*****************************************************************************
// �R���X�g���N�^�֐�
//*****************************************************************************
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

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
HRESULT Camera3D::Init()
{
	g_posCameraP = XMFLOAT3(POS_CAMERA_P_X, POS_CAMERA_P_Y, POS_CAMERA_P_Z);
	g_posCameraR = XMFLOAT3(POS_CAMERA_R_X, POS_CAMERA_R_Y, POS_CAMERA_R_Z);
	g_vecCameraU = XMFLOAT3(0, 1, 0);
	g_rotCamera = XMFLOAT3(0, 0, 0);
	ShakeForce = XMFLOAT2(0, 0);
	vEye = REGULAR_OBJECT_DISTANCE;
	vEye = REGULAR_OBJECT_DISTANCE;
	vsOffset = { 0,0,0 };
	float fVecX, fVecZ;
	fVecX = g_posCameraP.x - g_posCameraR.x;
	fVecZ = g_posCameraP.z - g_posCameraR.z;
	g_fLengthInterval = sqrtf(fVecX * fVecX + fVecZ * fVecZ);
	fAcceleration = 0;
	fAttackZoom = 0;
	fLockOnZoom = 0;
	fLockOnOffset = 0;
	return S_OK;
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Camera3D::Update()
{
	if (fAttackZoom < 0)
		fAttackZoom = 0;
	//printf("ZOOMFRAMES %f\n", fAttackZoom);
	if (fAttackZoomFrames <= 0) {
		if (fAttackZoom > 0)
			fAttackZoom-=2;
	}
	else {
		if(fAttackZoom == fMaxAttackZoom)
			fAttackZoomFrames--;
		if (fAttackZoom < fMaxAttackZoom)
			fAttackZoom+=2;
		if (fAttackZoom > fMaxAttackZoom)
			fAttackZoom = fMaxAttackZoom;
	}
	if (FocalPoint && !bFocalPointIsGameObject) {
		Model3D* FocusPoint = (Model3D*)FocalPoint;
		//���_
		const XMFLOAT3 vLookAt = XMFLOAT3(0, 25, 0);//�����_
		XMMATRIX mtxWorld = XMLoadFloat4x4(FocusPoint->GetModelWorld());

		//���_
		XMStoreFloat3(&g_posCameraP, XMVector3TransformCoord(XMLoadFloat3(&vEye), mtxWorld));

		//�����_
		XMStoreFloat3(&g_posCameraR, XMVector3TransformCoord(XMLoadFloat3(&vLookAt), mtxWorld));
		
		g_rotCamera = FocusPoint->GetRotation();
	}
	else if (FocalPoint && bFocalPointIsGameObject)
	{
		GameObject3D* FocusPoint = ((GameObject3D*)FocalPoint);
		//Model3D* FocusPoint = ((GameObject3D*)FocalPoint)->GetModel();
		XMFLOAT3 vLookAt = XMFLOAT3(vEye.x, vEye.y, 0);//�����_
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
		vEye.z += vsOffset.z + fLockOnZoom;
		vLookAt.x += vsOffset.x;
		vLookAt.y += vsOffset.y;
		XMFLOAT3 vFinalEye = vEye;
		if (--nShakeFrames <= 0)
		{
			ShakeForce = { 0,0 };
			nShakeFrames = 0;
		}
		else {
			if (nShakeFrames % 4 == 0)
			{
				ShakeForce.x*=-1;
				ShakeForce.y*=-1;
			}
		}
		vFinalEye.x += ShakeForce.x;
		vFinalEye.y += ShakeForce.y;
		vFinalEye.z += fAttackZoom;
		//���_
		XMStoreFloat3(&g_posCameraP, XMVector3TransformCoord(XMLoadFloat3(&vFinalEye), mtxWorld));

		//�����_
		XMStoreFloat3(&g_posCameraR, XMVector3TransformCoord(XMLoadFloat3(&vLookAt), mtxWorld));

		g_rotCamera = FocusPoint->GetRotation();
	}
	SetCamera();
}

//*****************************************************************************
//Uninit�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Camera3D::End()
{
	//�Ȃ�
}

//*****************************************************************************
//SetCamera�֐�
//���݂̃t���[���ɃJ�����̋��_�Ɖ�]��ݒ肷��
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
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

//*****************************************************************************
//GetCameraPos�֐�
//�J�����̋��_��߂�
//�����Fvoid
//�߁FXMFLOAT3&
//*****************************************************************************
XMFLOAT3& Camera3D::GetCameraPos()
{
	return g_posCameraP;
}

//*****************************************************************************
//GetCameraAngle�֐�
//�J�����̉�]��߂�
//�����Fvoid
//�߁FXMFLOAT3&
//*****************************************************************************
XMFLOAT3& Camera3D::GetCameraAngle(void)
{
	return g_rotCamera;
}

//*****************************************************************************
//GetViewMatrix�֐�
//�J�����̃r���[�}�g���b�N�X��߂�
//�����Fvoid
//�߁FXMFLOAT4X4&
//*****************************************************************************
XMFLOAT4X4 & Camera3D::GetViewMatrix()
{
	return g_mtxView;
}

//*****************************************************************************
//GetProjMatrix�֐�
//�J�����̃v���W�F�N�V�����}�g���b�N�X��߂�
//�����Fvoid
//�߁FXMFLOAT4X4&
//*****************************************************************************
XMFLOAT4X4 & Camera3D::GetProjMatrix()
{
	return g_mtxProjection;
}

//*****************************************************************************
//GetForwardCameraVector�֐�
//�J�����̃t�H�[���[�h�x�N�g����߂�
//�����Fvoid
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 Camera3D::GetForwardCameraVector()
{
	float pitch = g_rotCamera.y;
	float yaw = g_rotCamera.y;
	return { cosf(pitch)*cosf(yaw),cosf(pitch)*sinf(yaw),sinf(pitch) };
}

//*****************************************************************************
//SetFocalPoint�֐�
//�J�����̏œ_��ݒ肷��
//�����Fvoid*
//�߁Fvoid
//*****************************************************************************
void Camera3D::SetFocalPoint(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = false;
}

//*****************************************************************************
//SetFocalPointGO�֐�
//�J�����̏œ_��ݒ肷��
//�����Fvoid*
//�߁Fvoid
//*****************************************************************************
void Camera3D::SetFocalPointGO(void * newFocalPoint)
{
	FocalPoint = newFocalPoint;
	bFocalPointIsGameObject = true;
}

//*****************************************************************************
//ZoomIn�֐�
//�J�����̃Y�[���A�E�g�iZ���W�j��ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Camera3D::ZoomIn(float inc)
{
	vsOffset.z += inc;
}

//*****************************************************************************
//ResetOffset�֐�
//�J�����̃Y�[���I�t�Z�b�g�����Z�b�g����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Camera3D::ResetOffset()
{
	vsOffset = { 0,0,0 };
}

//*****************************************************************************
//SetZoomLock�֐�
//�J�����̃Y�[���A�E�g�iZ���W�j��ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Camera3D::SetZoomLock(float flock)
{
	fLockOnZoom = flock;
}

//*****************************************************************************
//SetYOffsetLock�֐�
//�J�����̃Y�[���A�E�g�iY���W�j��ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Camera3D::SetYOffsetLock(float OffsetLockOnY)
{
	fLockOnOffset = OffsetLockOnY;
}

//*****************************************************************************
//SetAttackZoom�֐�
//�U������̎��ɃJ�����̃Y�[���A�E�g�iZ���W�j��ݒ肷��
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Camera3D::SetAttackZoom(float Zoom, float AttackZoomFrames)
{
	fAttackZoomFrames = AttackZoomFrames;
	fMaxAttackZoom = Zoom;
}

//*****************************************************************************
//ShakeCamera�֐�
//�J�����ɗh�点��
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Camera3D::SetShake(float fShakeForce, float Frames)
{
	ShakeForce = {fShakeForce, fShakeForce};
	nShakeFrames = Frames;
}

//*****************************************************************************
//GetMainCamera�֐�
//���C���J������߂�
//�����Fvoid
//�߁FCamera3D*
//*****************************************************************************
Camera3D * GetMainCamera()
{
	return MainCamera;
}
