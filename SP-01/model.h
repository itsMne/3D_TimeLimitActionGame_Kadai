//=============================================================================
//
// ���f������ [model.h]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitModel(void);
void UninitModel(void);
void UpdateModel(void);
void DrawModel(void);
XMFLOAT4X4 *GetModelWorld();
XMFLOAT3 GetRotation();