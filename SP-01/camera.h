//=============================================================================
//
// �J�������� [camera.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);

XMFLOAT3& GetCameraPos(void);
XMFLOAT3& GetCameraAngle(void);
XMFLOAT4X4& GetViewMatrix(void);
XMFLOAT4X4& GetProjMatrix(void);
XMFLOAT3 GetForwardCameraVector();