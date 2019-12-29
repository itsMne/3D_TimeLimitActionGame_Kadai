#include "Sphere3D.h"
#include "main.h"
#include "Texture.h"
#include "Camera3D.h"
#define USE_16_SLASHES true
#define SKYBOX_SIZE 800
//*****************************************************************************
// �}�N����`
//*****************************************************************************


Sphere3D::Sphere3D(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath)
{
	Init(pos, rot, nSlice, nStack, fRadius, szPath);
	fRotSpeed = { 0,0,0 };
	bIsSkybox = false;
}

Sphere3D::Sphere3D(const char * szPath)
{
#if USE_16_SLASHES
	Init({ 0,50,100 }, { 0,0,0 }, 16, 8, SKYBOX_SIZE, szPath);
#else
	Init({ 0,50,100 }, { 0,0,0 }, 32, 16, SKYBOX_SIZE, szPath);
#endif
	bisUnlit = true;
	bNoCull = true;
	fRotSpeed = { 0,0.001f,0 };
	bIsSkybox = true;
}


Sphere3D::~Sphere3D()
{
}

HRESULT Sphere3D::Init(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath)
{
	Mesh3D::Init();
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// �e�N�X�`�� �}�g���b�N�X������
	XMStoreFloat4x4(&pMesh->mtxTexture, XMMatrixIdentity());

	// �ʒu�A�����̏����ݒ�
	pMesh->Position = pos;
	pMesh->Rotation = rot;

	// �e�N�X�`���̓ǂݍ���
	hr = CreateTextureFromFile(pDevice,					// �f�o�C�X�ւ̃|�C���^
		szPath,		// �t�@�C���̖��O
		&pMesh->pTexture);	// �ǂݍ��ރ������[
	if (FAILED(hr))
		return hr;

	// ���_���̐ݒ�
	pMesh->nNumVertex = (nSlice + 1) * (nSlice + 1);

	// �C���f�b�N�X���̐ݒ�
	pMesh->nNumIndex = (nSlice + 1) * 2 * nStack + (nStack - 1) * 2;


	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	VERTEX_3D* pVertexWk = new VERTEX_3D[pMesh->nNumVertex];

	// �I�u�W�F�N�g�̃C���f�b�N�X�o�b�t�@�𐶐�
	int* pIndexWk = new int[pMesh->nNumIndex];

	// ���_�o�b�t�@�̒��g�𖄂߂�
	VERTEX_3D* pVtx = pVertexWk;
	const float texSizeX = 1.0f / nSlice;
	const float texSizeZ = 1.0f / nStack;


	for (int nCntVtxY = 0; nCntVtxY < nStack + 1; ++nCntVtxY) {

		for (int nCntVtxX = 0; nCntVtxX < nSlice + 1; ++nCntVtxX) {
			// ���_���W�̐ݒ�
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.x = cosf(nCntVtxX * XM_PI / (nSlice / 2)) * fRadius * sinf(nCntVtxY * XM_PI / nStack);
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.y = sinf(((nStack / 2) - nCntVtxY) * XM_PI / nStack)*fRadius;
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.z = sinf(nCntVtxX * XM_PI / (nSlice / 2)) * fRadius * sinf(nCntVtxY * XM_PI / nStack);

			// �@���̐ݒ�
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].nor = XMFLOAT3(0.0f, 1.0, 0.0f);

			// ���ˌ��̐ݒ�
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].tex.x = texSizeX * nCntVtxX;
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].tex.y = texSizeZ * nCntVtxY;
		}
	}

	// �C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
	int* pIdx = pIndexWk;

	int nCntIdx = 0;
	for (int nCntVtxY = 0; nCntVtxY < nStack; ++nCntVtxY) {
		if (nCntVtxY > 0) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			pIdx[nCntIdx] = (nCntVtxY + 1) * (nSlice + 1);
			++nCntIdx;
		}

		for (int nCntVtxX = 0; nCntVtxX < nSlice + 1; ++nCntVtxX) {
			pIdx[nCntIdx] = (nCntVtxY + 1) * (nSlice + 1) + nCntVtxX;
			++nCntIdx;
			pIdx[nCntIdx] = nCntVtxY * (nSlice + 1) + nCntVtxX;
			++nCntIdx;
		}

		if (nCntVtxY < nStack - 1) {
			// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
			pIdx[nCntIdx] = nCntVtxY * (nSlice + 1) + nSlice;
			++nCntIdx;
		}
	}

	// ���_�o�b�t�@/�C���f�b�N�X �o�b�t�@����
	hr = MakeMeshVertex(pDevice, pMesh, pVertexWk, pIndexWk);

	// �ꎞ�z����
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

void Sphere3D::Uninit(void)
{
	Mesh3D::Uninit();
}

void Sphere3D::Update(void)
{
	GameObject3D::Update();
	Mesh3D::Update();
	if (!pMesh)
		return;
	if (!bIsSkybox)
		return;
	pMesh->Rotation.x += fRotSpeed.x;
	pMesh->Rotation.y += fRotSpeed.y;
	pMesh->Rotation.z += fRotSpeed.z;
	Camera3D* pCam = GetMainCamera();
	if (!pCam)
		return;
	pMesh->Position = pCam->GetCameraPos();
	pMesh->Position.y -= 10;
}

void Sphere3D::Draw(void)
{
	GameObject3D::Draw();
	Mesh3D::Draw(GetDeviceContext());
}

void Sphere3D::SetUnlit(bool isUnlit)
{
	bisUnlit = isUnlit;
}