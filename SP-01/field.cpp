//=============================================================================
//
// �n�ʏ��� [field.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "field.h"
#include "camera.h"
#include "light.h"
#include "Shader.h"
#include "Texture.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_FIELD	L"data/texture/field000.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����

#define M_DIFFUSE		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define M_AMBIENT		XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �V�F�[�_�ɓn���l
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ���[���h�~�r���[�~�ˉe�s��(�]�u�s��)
	XMMATRIX	mW;			// ���[���h�s��(�]�u�s��)
	XMMATRIX	mTex;		// �e�N�X�`���s��(�]�u�s��)
};
struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// ���_���W
	// ����
	XMVECTOR	vLightDir;	// ��������
	XMVECTOR	vLa;		// �����F(�A���r�G���g)
	XMVECTOR	vLd;		// �����F(�f�B�t���[�Y)
	XMVECTOR	vLs;		// �����F(�X�y�L����)
	// �}�e���A��
	XMVECTOR	vAmbient;	// �A���r�G���g�F(+�e�N�X�`���L��)
	XMVECTOR	vDiffuse;	// �f�B�t���[�Y�F
	XMVECTOR	vSpecular;	// �X�y�L�����F(+�X�y�L�������x)
	XMVECTOR	vEmissive;	// �G�~�b�V�u�F
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static HRESULT MakeVertexField(ID3D11Device* pDevice);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11ShaderResourceView*	g_pTexture;				// �e�N�X�`���ւ̃|�C���^
static VERTEX_3D					g_vertexWk[NUM_VERTEX];	// ���_���i�[���[�N

static ID3D11Buffer*				g_pConstantBuffer[2];	// �萔�o�b�t�@
static ID3D11Buffer*				g_pVertexBuffer;		// ���_�o�b�t�@
static ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
static ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
static ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
static ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_

static XMFLOAT4X4					g_mtxWorldField;		// ���[���h�}�g���b�N�X
static XMFLOAT4X4					g_mtxTexture;			// �e�N�X�`���}�g���b�N�X
static XMFLOAT3						g_posField;				// ���݂̈ʒu
static XMFLOAT3						g_rotField;				// ���݂̌���

// �}�e���A��
static XMFLOAT4						g_Ka;		// �A���r�G���g
static XMFLOAT4						g_Kd;		// �f�B�t���[�Y
static XMFLOAT4						g_Ks;		// �X�y�L����
static float						g_fPower;	// �X�y�L�������x
static XMFLOAT4						g_Ke;		// �G�~�b�V�u

//=============================================================================
// ����������
//=============================================================================
HRESULT InitField(void)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// �V�F�[�_������
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("Vertex", "Pixel",
		&g_pVertexShader, &g_pInputLayout, &g_pPixelShader, layout, _countof(layout));
	if (FAILED(hr)) {
		return hr;
	}

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[0]);
	if (FAILED(hr)) return hr;
	bd.ByteWidth = sizeof(SHADER_GLOBAL2);
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer[1]);
	if (FAILED(hr)) return hr;

	// �e�N�X�`�� �T���v������
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	XMStoreFloat4x4(&g_mtxTexture, XMMatrixIdentity());

	// �ʒu�A�����̏����ݒ�
	g_posField = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_rotField = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// �}�e���A���̏����ݒ�
	g_Kd = M_DIFFUSE;
	g_Ka = M_AMBIENT;
	g_Ks = M_SPECULAR;
	g_fPower = 0.0f;
	g_Ke = M_EMISSIVE;

	// �e�N�X�`���̓ǂݍ���
	hr = CreateTextureFromFile(pDevice,			// �f�o�C�X�ւ̃|�C���^
							   TEXTURE_FIELD,	// �t�@�C���̖��O
							   &g_pTexture);	// �ǂݍ��ރ������[
	if (FAILED(hr))
		return hr;

	// ���_���̍쐬
	hr = MakeVertexField(pDevice);

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void UninitField(void)
{
	// �e�N�X�`�����
	SAFE_RELEASE(g_pTexture);
	// �e�N�X�`�� �T���v���̊J��
	SAFE_RELEASE(g_pSamplerState);
	// ���_�o�b�t�@�̉��
	SAFE_RELEASE(g_pVertexBuffer);
	// �萔�o�b�t�@�̉��
	for (int i = 0; i < _countof(g_pConstantBuffer); ++i) {
		SAFE_RELEASE(g_pConstantBuffer[i]);
	}
	// �s�N�Z���V�F�[�_���
	SAFE_RELEASE(g_pPixelShader);
	// ���_�t�H�[�}�b�g���
	SAFE_RELEASE(g_pInputLayout);
	// ���_�V�F�[�_���
	SAFE_RELEASE(g_pVertexShader);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateField(void)
{
	// (�������Ȃ�)
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawField(void)
{
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();


	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(g_posField.x, g_posField.y, g_posField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	XMStoreFloat4x4(&g_mtxWorldField, mtxWorld);

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &g_pTexture);

	SHADER_GLOBAL cb;
	cb.mWVP = XMMatrixTranspose(mtxWorld *
		XMLoadFloat4x4(&GetViewMatrix()) * XMLoadFloat4x4(&GetProjMatrix()));
	cb.mW = XMMatrixTranspose(mtxWorld);
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mtxTexture));
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);
	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&GetCameraPos());
	CFbxLight& light = GetLight();
	cb2.vLightDir = XMVectorSet(light.m_direction.x, light.m_direction.y, light.m_direction.z, 0.f);
	cb2.vLa = XMLoadFloat4(&light.m_ambient);
	cb2.vLd = XMLoadFloat4(&light.m_diffuse);
	cb2.vLs = XMLoadFloat4(&light.m_specular);
	cb2.vDiffuse = XMLoadFloat4(&g_Kd);
	cb2.vAmbient = XMVectorSet(g_Ka.x, g_Ka.y, g_Ka.z,
		(g_pTexture != nullptr) ? 1.f : 0.f);
	cb2.vSpecular = XMVectorSet(g_Ks.x, g_Ks.y, g_Ks.z, g_fPower);
	cb2.vEmissive = XMLoadFloat4(&g_Ke);
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer[1]);

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �|���S���̕`��
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT MakeVertexField(ID3D11Device* pDevice)
{
	// ���_���W�̐ݒ�
	g_vertexWk[0].vtx = XMFLOAT3(-100.0f, 0.0f,  100.0f);
	g_vertexWk[1].vtx = XMFLOAT3( 100.0f, 0.0f,  100.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-100.0f, 0.0f, -100.0f);
	g_vertexWk[3].vtx = XMFLOAT3( 100.0f, 0.0f, -100.0f);

	// �@���x�N�g���̐ݒ�
	g_vertexWk[0].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_vertexWk[1].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_vertexWk[2].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
	g_vertexWk[3].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// �g�U���ˌ��̐ݒ�
	g_vertexWk[0].diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	g_vertexWk[1].diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	g_vertexWk[2].diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	g_vertexWk[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	g_vertexWk[0].tex = XMFLOAT2(0.0f, 0.0f);
	g_vertexWk[1].tex = XMFLOAT2(1.0f, 0.0f);
	g_vertexWk[2].tex = XMFLOAT2(0.0f, 1.0f);
	g_vertexWk[3].tex = XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(g_vertexWk);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = &g_vertexWk[0];

	HRESULT hr = pDevice->CreateBuffer(&vbd, &initData, &g_pVertexBuffer);

	return hr;
}
