//*****************************************************************************
// Camera3D.h
//*****************************************************************************
#pragma once

#include "DXWindow3D.h"
#include "UniversalStructures.h"
//*****************************************************************************
// クラス
//*****************************************************************************
class Camera3D
{
private:
	 XMFLOAT3			g_posCameraP;
	 XMFLOAT3			g_posCameraR;
	 XMFLOAT3			g_vecCameraU;
	 XMFLOAT3			g_rotCamera;
	 float				g_fLengthInterval;
	 XMFLOAT4X4			g_mtxProjection;
	 XMFLOAT4X4			g_mtxView;
	 void*				FocalPoint;
	 XMFLOAT3			vEye;
	 XMFLOAT3			vsOffset;
	 XMFLOAT2			ShakeForce;
	 float				fAcceleration;
	 float				fLockOnZoom;
	 float				fLockOnOffset;
	 float				fAttackZoom;
	 float				fMaxAttackZoom;
	 float				fAttackZoomFrames;
	 int				nShakeFrames;
public:
	Camera3D();
	Camera3D(bool bisMainCamera);
	~Camera3D();
	HRESULT Init();
	void Update();
	void End();
	void SetCamera();
	XMFLOAT3& GetCameraPos();
	XMFLOAT3& GetCameraAngle();
	XMFLOAT4X4& GetViewMatrix();
	XMFLOAT4X4& GetProjMatrix();
	XMFLOAT3 GetForwardCameraVector();
	void SetFocalPoint(void* newFocalPoint);
	void SetFocalPointGO(void* newFocalPoint);
	void ZoomIn(float inc);
	void ResetOffset();
	void SetZoomLock(float flock);
	void SetYOffsetLock(float OffsetLockOnY);
	void SetAttackZoom(float Zoom, float AttackZoomFrames);
	void SetShake(float ShakeForce, float Frames);
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
Camera3D* GetMainCamera();
