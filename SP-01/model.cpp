//=============================================================================
//
// ���f������ [model.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "model.h"
#include "main.h"
#include "FbxModel.h"
#include "input.h"
#include "camera.h"
#include "light.h"
#include "UniversalStructures.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MODEL_CAR			"data/model/car000.fbx"	// �ǂݍ��ރ��f���t�@�C����
#define MODEL_CAR			"data/model/PrototypeSamurai.fbx"	// �ǂݍ��ރ��f���t�@�C����
#define VALUE_MOVE_MODEL	(5.0f)					// �ړ����x
#define VALUE_ROTATE_MODEL	(XM_PI*0.02f)			// ��]���x

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static CFbxModel*	g_pModel;		// FBX���f���ւ̃|�C���^

static XMFLOAT3		g_posModel;		// ���݂̈ʒu
static XMFLOAT3		g_rotModel;		// ���݂̌���
static XMFLOAT3		g_ScaleModel;		// ���݂̌���
float ScaleTest;
static XMFLOAT4X4	g_mtxWorld;		// ���[���h�}�g���b�N�X
bool bIsMoving = false;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitModel(void)
{
	HRESULT hr = S_OK;
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();

	// �ʒu�A�����̏����ݒ�
	g_posModel = XMFLOAT3(0.0f, 10.0f, 0.0f);
	g_rotModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	ScaleTest = 0.1f;
	g_ScaleModel = XMFLOAT3(ScaleTest, ScaleTest, ScaleTest);
	// FBX�t�@�C���̓ǂݍ���
	g_pModel = new CFbxModel();
	hr = g_pModel->Init(pDevice, pDeviceContext, MODEL_CAR);
	if (SUCCEEDED(hr)) {
		g_pModel->SetCamera(GetCameraPos());
		g_pModel->SetLight(GetLight());
	}
	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitModel(void)
{
	// FBX�t�@�C���̉��
	SAFE_DELETE(g_pModel);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateModel(void)
{
	XMFLOAT3 rotCamera;

	// �J�����̌����擾
	rotCamera = GetCameraAngle();
	g_pModel->SetAnimStack(2);
	if (GetKeyPress(VK_LEFT)) {
		if (GetKeyPress(VK_UP)) {
			// ���O�ړ�
			g_posModel.x -= sinf(rotCamera.y + XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y + XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else if (GetKeyPress(VK_DOWN)) {
			// ����ړ�
			g_posModel.x -= sinf(rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y + XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else {
			// ���ړ�
			g_posModel.x -= sinf(rotCamera.y + XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y + XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
	}
	else if (GetKeyPress(VK_RIGHT)) {
		if (GetKeyPress(VK_UP)) {
			// �E�O�ړ�
			g_posModel.x -= sinf(rotCamera.y - XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y - XM_PI * 0.75f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else if (GetKeyPress(VK_DOWN)) {
			// �E��ړ�
			g_posModel.x -= sinf(rotCamera.y - XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y - XM_PI * 0.25f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
		else {
			// �E�ړ�
			g_posModel.x -= sinf(rotCamera.y - XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_posModel.z -= cosf(rotCamera.y - XM_PI * 0.50f) * VALUE_MOVE_MODEL;
			g_pModel->SetAnimStack(1);
		}
	}
	else if (GetKeyPress(VK_UP)) {
		// �O�ړ�
		g_posModel.x -= sinf(XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;
		g_posModel.z -= cosf(XM_PI + rotCamera.y) * VALUE_MOVE_MODEL;
		g_pModel->SetAnimStack(1);
		//g_posModel.x += GetForwardVector(g_rotModel).x*VALUE_MOVE_MODEL;
		//g_posModel.y += GetForwardVector(g_rotModel).y*VALUE_MOVE_MODEL;
		//g_posModel.z += GetForwardVector(g_rotModel).z*VALUE_MOVE_MODEL;
	}
	else if (GetKeyPress(VK_DOWN)) {
		// ��ړ�
		g_posModel.x -= sinf(rotCamera.y) * VALUE_MOVE_MODEL;
		g_posModel.z -= cosf(rotCamera.y) * VALUE_MOVE_MODEL;
		g_pModel->SetAnimStack(1);
	}

	if (GetKeyPress(VK_LSHIFT)) {
		// ����]
		g_rotModel.y -= VALUE_ROTATE_MODEL;
		if (g_rotModel.y < -XM_PI) {
			g_rotModel.y += XM_2PI;
			g_pModel->SetAnimStack(1);
		}
	}
	if (GetKeyPress(VK_RSHIFT)) {
		// �E��]
		g_rotModel.y += VALUE_ROTATE_MODEL;
		if (g_rotModel.y > XM_PI) {
			g_rotModel.y -= XM_2PI;
			g_pModel->SetAnimStack(1);
		}
	}

	if (GetKeyPress(VK_RETURN)) {
		// ���Z�b�g
		g_posModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_rotModel = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	PrintDebugProc("[��� �� : (%f, %f, %f)]\n", g_posModel.x, g_posModel.y, g_posModel.z);
	PrintDebugProc("[��� ѷ : (%f)]\n", XMConvertToDegrees(g_rotModel.y));
	PrintDebugProc("\n");

	PrintDebugProc("*** ��� ��� ***\n");
	PrintDebugProc("ϴ   ��޳ : \x1e\n");//��
	PrintDebugProc("���  ��޳ : \x1f\n");//��
	PrintDebugProc("���� ��޳ : \x1d\n");//��
	PrintDebugProc("з�  ��޳ : \x1c\n");//��
	PrintDebugProc("���� �ݶ� : LSHIFT\n");
	PrintDebugProc("з�  �ݶ� : RSHIFT\n");
	PrintDebugProc("\n");
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawModel(void)
{
	ID3D11Device* pDevice = GetDevice();
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	//�T�C�Y
	mtxScale = XMMatrixScaling(g_ScaleModel.x, g_ScaleModel.y, g_ScaleModel.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotModel.x, g_rotModel.y, g_rotModel.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);



	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_posModel.x, g_posModel.y, g_posModel.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);



	// ���[���h�}�g���b�N�X�̐ݒ�
	XMStoreFloat4x4(&g_mtxWorld, mtxWorld);


	// FBX�t�@�C���\��
	static int nFrame = g_pModel->GetInitialAnimFrame();

	//g_pModel->SetAnimStack(0);
	//printf("FRAMES: %d\n", nFrame);
	if (++nFrame >= g_pModel->GetMaxAnimFrame()-5) nFrame = g_pModel->GetInitialAnimFrame();
	g_pModel->SetAnimFrame(nFrame);
	SetZWrite(true);
	g_pModel->Render(g_mtxWorld, GetViewMatrix(), GetProjMatrix(), eOpacityOnly);
	SetZWrite(false);
	g_pModel->Render(g_mtxWorld, GetViewMatrix(), GetProjMatrix(), eTransparentOnly);
}

XMFLOAT4X4 * GetModelWorld()
{
	return &g_mtxWorld;
}

XMFLOAT3 GetRotation()
{
	return g_rotModel;
}
