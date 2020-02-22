//*****************************************************************************
// Billboard2D.cpp
// �r�[���{�[�h���Ǘ�����
// Author : Mane
//*****************************************************************************
#include "Billboard2D.h"
#include "Texture.h"
#include "Mesh3D.h"
#include "Camera3D.h"
#include "input.h"
#include "debugproc.h"
#include "Light3D.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE_TREE		(3.00f)							// �ړ����x

//*****************************************************************************
//�R���X�g���N�^�֐�
//*****************************************************************************
Billboard2D::Billboard2D(const char* szpath): Mesh3D()
{
	pTexture = nullptr;
	Init(szpath);
}

Billboard2D::Billboard2D(ID3D11ShaderResourceView * texture): Mesh3D()
{
	pTexture = texture;
	Init("");
}


Billboard2D::~Billboard2D()
{
	End();
}

//*****************************************************************************
//Init�֐�
//�������֐�
//�����Fvoid
//�߁FHRESULT
//*****************************************************************************
HRESULT Billboard2D::Init(const char* szpath)
{
	ID3D11Device* pDevice = GetDevice();
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	MakeVertex(pDevice);

	// �e�N�X�`���̓ǂݍ���
	
	if (!pTexture) {
		printf("%s\n", szpath);
		CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
			szpath,		// �t�@�C���̖��O
			&(pMesh->pTexture));
	}
	else
	{
		pMesh->pTexture = pTexture;
	}
	XMStoreFloat4x4(&pMesh->mtxTexture, XMMatrixIdentity());

	// �}�e���A���̐ݒ�
	g_nAlpha = 0;
	fWidth = 10;
	fHeight = 10;
	bUse = false;

	g_bAlphaTest = false;
	g_nAlpha = 0x0;
	g_bInTree = false;
	uv = { 0,0 };
	nAnimeCount = 3;
	SetBillboard(XMFLOAT3(0.0f, 0.0f, -10.0f), 60.0f, 90.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	nFrameX = 1;
	nFrameY = 1;
	bUse = true;
	return S_OK;
}

//*****************************************************************************
//Update�֐�
//�ύX�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Billboard2D::Update()
{
	Camera3D* camera = GetMainCamera();
	Rotation= camera->GetCameraAngle();
	if (++nAnimeCount >= 3) {
		++uv.U;
		if (uv.U >= (float)nFrameX) {
			//// �e�폜
			//nIdxShadow = -1;
			// �e�폜
			//bUse = false;
			//continue;
			//nAnimeIdx = 0;
			uv.U = 0;
			uv.V++;
			if (uv.V >= (float)nFrameY)
			{
				uv.V = 0;
				if(bSetFalseAfterAnimation)
				bUse = false;
			}
		}
		nAnimeCount = 0;
	}
}

//*****************************************************************************
//Draw�֐�
//�����_�����O�֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Billboard2D::Draw()
{
	if (!bUse)
		return;
	GetMainLight()->SetLightEnable(false);
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxScale, mtxRotate, mtxTranslate;



	if (!GetMainCamera())
		return;
	XMFLOAT4X4& mtxView = GetMainCamera()->GetViewMatrix();

	g_bInTree = true;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();
	XMStoreFloat4x4(&this->mtxWorld, mtxWorld);

	// �|���S���𐳖ʂɌ�����
	this->mtxWorld._11 = mtxView._11;
	this->mtxWorld._12 = mtxView._21;
	this->mtxWorld._13 = mtxView._31;
	this->mtxWorld._21 = mtxView._12;
	this->mtxWorld._22 = mtxView._22;
	this->mtxWorld._23 = mtxView._32;
	this->mtxWorld._31 = mtxView._13;
	this->mtxWorld._32 = mtxView._23;
	this->mtxWorld._33 = mtxView._33;

	// �X�P�[���𔽉f
	mtxScale = XMMatrixScaling(fWidth,
		fHeight, 1.0f);
	mtxWorld = XMMatrixMultiply(mtxScale, mtxWorld);
	mtxRotate = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRotate);

	
	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(Position.x,
		Position.y,
		Position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	XMStoreFloat4x4(&this->mtxWorld, mtxWorld);
	pMesh->mtxWorld = this->mtxWorld;

	// ���b�V���̕`��
	XMMATRIX mtxTex = XMMatrixScaling(1.0f / (float)nFrameX, 1.0f / (float)nFrameY, 0.0f);
	mtxTex *= XMMatrixTranslation((float)uv.U / (float)nFrameX, (float)uv.V / (float)nFrameY, 0.0f);
	XMStoreFloat4x4(&pMesh->mtxTexture, mtxTex);


	Mesh3D::Draw(pDeviceContext);
	g_bInTree = false;
	GetMainLight()->SetLightEnable(true);
}

//*****************************************************************************
//End�֐�
//�I���֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Billboard2D::End()
{
	// ���b�V���̊J��
	SAFE_RELEASE(pMesh->pTexture);
	ReleaseMesh();
}

//*****************************************************************************
//SetBillboard�֐�
//�r�[���{�[�h��ݒ肷��
//�����FXMFLOAT3, float, float, XMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetBillboard(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 color)
{
	pos = pos;
	bUse = true;
	// ���_���W�̐ݒ�
	SetVertex(fWidth, fHeight);
	// ���_�J���[�̐ݒ�
	SetColor(color);
}

//*****************************************************************************
//MakeVertex�֐�
//�o�[�e�b�N�X�����
//�����FID3D11Device*
//�߁FHRESULT
//*****************************************************************************
HRESULT Billboard2D::MakeVertex(ID3D11Device * pDevice)
{
	pMesh->nNumVertex = 4;
	pMesh->nNumIndex = 4;

	// �I�u�W�F�N�g�̒��_�z��𐶐�
	VERTEX_3D* pVertexWk = new VERTEX_3D[pMesh->nNumVertex];

	// ���_�z��̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;

	// ���_���W�̐ݒ�
	pVtx[0].vtx = XMFLOAT3(-1.0f / 2, 0.0f, 0.0f);
	pVtx[1].vtx = XMFLOAT3(-1.0f / 2, 1.0f, 0.0f);
	pVtx[2].vtx = XMFLOAT3(1.0f / 2, 0.0f, 0.0f);
	pVtx[3].vtx = XMFLOAT3(1.0f / 2, 1.0f, 0.0f);

	// �@���̐ݒ�
	pVtx[0].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// ���ˌ��̐ݒ�
	pVtx[0].diffuse = Color;
	pVtx[1].diffuse = Color;
	pVtx[2].diffuse = Color;
	pVtx[3].diffuse = Color;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = XMFLOAT2(0.0f, 1.0f);
	pVtx[1].tex = XMFLOAT2(0.0f, 0.0f);
	pVtx[2].tex = XMFLOAT2(1.0f, 1.0f);
	pVtx[3].tex = XMFLOAT2(1.0f, 0.0f);

	// �C���f�b�N�X�z��𐶐�
	int* pIndexWk = new int[pMesh->nNumIndex];

	// �C���f�b�N�X�z��̒��g�𖄂߂�
	pIndexWk[0] = 0;
	pIndexWk[1] = 1;
	pIndexWk[2] = 2;
	pIndexWk[3] = 3;

	// ���_�o�b�t�@����
	HRESULT hr = MakeMeshVertex(pDevice, pMesh, pVertexWk, pIndexWk);
	// �ꎞ�z��̉��
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

//*****************************************************************************
//SetVertex�֐�
//�o�[�e�b�N�X��ݒ肷��
//�����Ffloat, float
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetVertex(float fWidth, float fHeight)
{
	this->fWidth = fWidth;
	this->fHeight = fHeight;
}

//*****************************************************************************
//SetColor�֐�
//�F��ݒ肷��
//�����FXMFLOAT4
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetColor(XMFLOAT4 col)
{
	this->Color = col;
	if (Color.x < 0)
		Color.x = 0;
	if (Color.y < 0)
		Color.y = 0;
	if (Color.z < 0)
		Color.z = 0;
	if (Color.w < 0)
		Color.w = 0;
}

//*****************************************************************************
//SetPosition�֐�
//���_��ݒ肷��
//�����FXMFLOAT3
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetPosition(XMFLOAT3 newPosition)
{
	Position = newPosition;
}

//*****************************************************************************
//SetWidth�֐�
//����ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetWidth(float newWidth)
{
	fWidth = newWidth;
}

//*****************************************************************************
//SetHeight�֐�
//������ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetHeight(float newHeight)
{
	fHeight = newHeight;
}

//*****************************************************************************
//SetUVFrames�֐�
//UV�t���[����ݒ肷��
//�����Fint, int
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetUVFrames(int nX, int nY)
{
	nFrameX = nX;
	nFrameY = nY;

}

//*****************************************************************************
//SetScale�֐�
//�傫����ݒ肷��
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetScale(float nScale)
{
	fHeight = nScale;
	fWidth = nScale;
}

//*****************************************************************************
//ScaleUp�֐�
//�傫���𑝂₷
//�����Ffloat
//�߁Fvoid
//*****************************************************************************
void Billboard2D::ScaleUp(float nS_Up)
{
	fHeight += nS_Up;
	fWidth  += nS_Up;
}

//*****************************************************************************
//GetAlpha�֐�
//�A���t�@��߂�
//�����Fvoid
//�߁Ffloat
//*****************************************************************************
float Billboard2D::GetAlpha()
{
	return Color.w;
}

//*****************************************************************************
//GetUse�֐�
//�g�����Ƃ��m�F����
//�����Fvoid
//�߁Fbool
//*****************************************************************************
bool Billboard2D::GetUse()
{
	return bUse;
}

//*****************************************************************************
//SetUse�֐�
//�g�����Ƃ�ݒ肷��
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetUse(bool use)
{
	bUse = use;
}

//*****************************************************************************
//ResetUV�֐�
//UV�����Z�b�g����
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void Billboard2D::ResetUV()
{
	uv = { 0,0 };
}

//*****************************************************************************
//SetUnusableAfterAnimation�֐�
//�A�j���[�V�������I�������A�I�u�W�F�N�g�̏�Ԃ��g��Ȃ����Ƃɂ���
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void Billboard2D::SetUnusableAfterAnimation(bool inv)
{
	bSetFalseAfterAnimation = inv;
}
