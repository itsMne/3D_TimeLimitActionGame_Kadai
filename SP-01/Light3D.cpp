//*****************************************************************************
// Light3D.cpp
// ���̊Ǘ�
// Author: Mane
//*****************************************************************************
#include "Light3D.h"
#include "InputManager.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define LIGHT0_DIFFUSE XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_DIFFUSE XMFLOAT4(1.0f, 0, 0, 1.0f)
#define LIGHT0_AMBIENT XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_AMBIENT XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
#define LIGHT0_SPECULAR XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define LIGHT0_SPECULAR XMFLOAT4(1.0f, .0.0f, 0.0f, 1.0f)
#define LIGHT0_DIR XMFLOAT3(0.0f, -1.0f, 1.0f)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Light3D* MainLightSource = nullptr;

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Light3D::Light3D()
{
	Init();
}

Light3D::Light3D(bool bMainLight)
{
	if (bMainLight)
		MainLightSource = this;
	Init();
}


Light3D::~Light3D()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
HRESULT Light3D::Init(void)
{
	XMFLOAT3 vecDir;

	// �g�U��
	g_UnivLight.m_diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// ����
	g_UnivLight.m_ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// ���C�g�̕����̐ݒ�
	vecDir = LIGHT0_DIR;
	XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
	return S_OK;
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Light3D::End(void)
{
	//����
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Light3D::Update(void)
{
	//����
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
CFbxLight* Light3D::GetLight(void)
{
	return &g_UnivLight;
}

//*****************************************************************************
//SetDiffuse�֐�
//�g�U���̐ݒ�
//�����FXMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Light3D::SetDiffuse(XMFLOAT4 newDif)
{
	g_UnivLight.m_diffuse = newDif;
}

//*****************************************************************************
//SetAmbient�֐�
//�X�y�L�����[�̐ݒ�
//�����FXMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Light3D::SetSpecular(XMFLOAT4 newSpec)
{
	g_UnivLight.m_specular = newSpec;
}

//*****************************************************************************
//SetAmbient�֐�
//�A���r�G���g���C�g�̐ݒ�
//�����FXMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Light3D::SetAmbient(XMFLOAT4 newAmb)
{
	g_UnivLight.m_ambient = newAmb;
}

//*****************************************************************************
//SetDirection�֐�
//������ݒ肷��
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void Light3D::SetDirection(XMFLOAT3 newDir)
{
	g_UnivLight.m_direction = newDir;
}

//*****************************************************************************
//GetMainLight�֐�
//���C�g�̃A�h���X��߂�
//�����Fvoid
//�߁FLight3D*
//*****************************************************************************
Light3D * GetMainLight()
{
	return MainLightSource;
}

//*****************************************************************************
//SetLightEnable�֐�
//����L������
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void  Light3D::SetLightEnable(bool bEnable)
{
	if (bEnable) {
		XMFLOAT3 vecDir = LIGHT0_DIR;
		XMStoreFloat3(&g_UnivLight.m_direction, XMVector3Normalize(XMLoadFloat3(&vecDir)));
	}
	else {
		g_UnivLight.m_direction = XMFLOAT3(0, 0, 0);
	}
}