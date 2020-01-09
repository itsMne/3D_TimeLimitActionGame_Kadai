//=============================================================================
//
// �|���S������ [polygon.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "Polygon2D.h"
#include "Shader.h"
#include "Texture.h"

int InitedPolygons = 0;

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �V�F�[�_�ɓn���l
struct SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ���[���h�ϊ��s��(�]�u�s��)
	XMMATRIX	mView;		// �r���[�ϊ��s��(�]�u�s��)
	XMMATRIX	mProj;		// �ˉe�ϊ��s��(�]�u�s��)
	XMMATRIX	mTex;		// �e�N�X�`���ϊ��s��(�]�u�s��)
};

Polygon2D::Polygon2D()
{
	g_pSamplerState = nullptr;
	g_pVertexBuffer = nullptr;
	g_pConstantBuffer = nullptr;
	g_pPixelShader = nullptr;
	g_pInputLayout = nullptr;
	g_pVertexShader = nullptr;
	InitPolygon(GetDevice());
	gpTexture = nullptr;
	SetTexture(gpTexture);
	InitedPolygons++;
}

Polygon2D::Polygon2D(const char * TexturePath)
{
	g_pSamplerState = nullptr;
	g_pVertexBuffer = nullptr;
	g_pConstantBuffer = nullptr;
	g_pPixelShader = nullptr;
	g_pInputLayout = nullptr;
	g_pVertexShader = nullptr;
	InitPolygon(GetDevice());
	HRESULT hr = CreateTextureFromFile(GetDevice(), TexturePath, &gpTexture);
	if (FAILED(hr))
		printf("���s����\n");
	SetTexture(gpTexture);
	InitedPolygons++;
}

Polygon2D::~Polygon2D()
{
	UninitPolygon();
	InitedPolygons--;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Polygon2D::InitPolygon(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	// �V�F�[�_������
	static const D3D11_INPUT_ELEMENT_DESC layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	hr = LoadShader("Vertex2D", "Pixel2D",
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
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	if (FAILED(hr)) return hr;

	// �e�N�X�`�� �T���v������
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = pDevice->CreateSamplerState(&sd, &g_pSamplerState);
	if (FAILED(hr)) {
		return hr;
	}

	// �ϊ��s�񏉊���
	XMStoreFloat4x4(&g_mWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&g_mView, XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&g_mProj,
		XMMatrixOrthographicLH(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 100.0f));
	XMStoreFloat4x4(&g_mTex, XMMatrixIdentity());
	g_mTex._44 = 0.0f;

	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_colPolygon = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	g_bInvalidate = false;

	x2UV = XMFLOAT2(0.0f, 0.0f);
	x2Frame = XMFLOAT2(1.0f, 1.0f);

	// ���_���̍쐬
	hr = MakeVertexPolygon(pDevice);

	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void Polygon2D::UninitPolygon(void)
{
	// �e�N�X�`�� �T���v���̊J��
	SAFE_RELEASE(g_pSamplerState);
	// ���_�o�b�t�@�̉��
	SAFE_RELEASE(g_pVertexBuffer);
	// �萔�o�b�t�@�̉��
	SAFE_RELEASE(g_pConstantBuffer);
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
void Polygon2D::UpdatePolygon(void)
{
	// (�������Ȃ�)
}

//=============================================================================
// �`�揈��
//=============================================================================
void Polygon2D::DrawPolygon(ID3D11DeviceContext* pDeviceContext)
{
	// �g�k
	XMMATRIX mWorld = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	// ��]
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// �ړ�
	mWorld *= XMMatrixTranslation(Position.x, Position.y, Position.z);
	// ���[���h �}�g���b�N�X�ɐݒ�
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (g_pTexture) {
		// �g�k
		mWorld = XMMatrixScaling(x2Frame.x, x2Frame.y, 1.0f);
		// �ړ�
		mWorld *= XMMatrixTranslation(x2UV.x, x2UV.y, 0.0f);
		// �e�N�X�`�� �}�g���b�N�X�ɐݒ�
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// �e�N�X�`������
		g_mTex._44 = 0.0f;
	}

	// ���_�o�b�t�@�X�V
	SetVertexPolygon();

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_2D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &g_pTexture);

	SHADER_GLOBAL cb;
	cb.mProj = XMMatrixTranspose(XMLoadFloat4x4(&g_mProj));
	cb.mView = XMMatrixTranspose(XMLoadFloat4x4(&g_mView));
	cb.mWorld = XMMatrixTranspose(XMLoadFloat4x4(&g_mWorld));
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mTex));
	pDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	pDeviceContext->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �|���S���̕`��
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT Polygon2D::MakeVertexPolygon(ID3D11Device* pDevice)
{
	// ���_���W�̐ݒ�
	g_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	g_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	g_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);

	// �g�U���ˌ��̐ݒ�
	g_vertexWk[0].diffuse = g_colPolygon;
	g_vertexWk[1].diffuse = g_colPolygon;
	g_vertexWk[2].diffuse = g_colPolygon;
	g_vertexWk[3].diffuse = g_colPolygon;

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

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void Polygon2D::SetVertexPolygon(void)
{
	if (g_bInvalidate) {
		//���_�o�b�t�@�̒��g�𖄂߂�
		ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
		HRESULT hr = S_OK;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		hr = pDeviceContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		if (SUCCEEDED(hr)) {
			// �g�U���ˌ��̐ݒ�
			g_vertexWk[0].diffuse = g_colPolygon;
			g_vertexWk[1].diffuse = g_colPolygon;
			g_vertexWk[2].diffuse = g_colPolygon;
			g_vertexWk[3].diffuse = g_colPolygon;
			// ���_�f�[�^���㏑��
			memcpy_s(msr.pData, sizeof(g_vertexWk), g_vertexWk, sizeof(g_vertexWk));
			// ���_�f�[�^���A�����b�N����
			pDeviceContext->Unmap(g_pVertexBuffer, 0);
			// �t���O���N���A
			g_bInvalidate = false;
		}
	}
}

void Polygon2D::Translate(XMFLOAT2 tr)
{
	Position.x += tr.x;
	Position.y += tr.y;
}

XMFLOAT2 Polygon2D::GetPolygonSize()
{
	return { Scale.x, Scale.y };
}

XMFLOAT2 Polygon2D::GetPolygonPos()
{
	return { Position.x,Position.y };
}

XMFLOAT2 Polygon2D::GetPolygonInitialPos()
{
	return { InitPosition.x,InitPosition.y };
}

void Polygon2D::bScaleUp(float scal)
{
	Scale.x += scal;
	Scale.y += scal;
}

//=============================================================================
// �e�N�X�`���̐ݒ�
//=============================================================================
void Polygon2D::SetTexture(ID3D11ShaderResourceView* pTexture)
{
	g_pTexture = pTexture;
	g_mTex._44 = (g_pTexture) ? 1.0f : 0.0f;
}

//=============================================================================
// �\�����W�̐ݒ�
//=============================================================================
void Polygon2D::SetPolygonPos(float fX, float fY)
{
	Position.x = fX;
	Position.y = fY;
}

void Polygon2D::SetPolygonPos(float fX, float fY, bool IsInit)
{
	Position.x = fX;
	Position.y = fY;
	if (IsInit)
		InitPosition = Position;
}

//=============================================================================
// �\���T�C�Y�̐ݒ�
//=============================================================================
void Polygon2D::SetPolygonSize(float fScaleX, float fScaleY)
{
	Scale.x = fScaleX;
	Scale.y = fScaleY;
}

//=============================================================================
// �\���p�x�̐ݒ�(�P��:�x)
//=============================================================================
void Polygon2D::SetPolygonAngle(float fAngle)
{
	Rotation.z = fAngle;
}

//=============================================================================
// ����e�N�X�`�����W�̐ݒ� (0.0��fU��1.0, 0.0��fV��1.0)
//=============================================================================
void Polygon2D::SetPolygonUV(float fU, float fV)
{
	x2UV.x = fU;
	x2UV.y = fV;
}

//=============================================================================
// �e�N�X�`���t���[���T�C�Y�̐ݒ� (0.0��fWidth��1.0, 0.0��fHeight��1.0)
//=============================================================================
void Polygon2D::SetPolygonFrameSize(float fWidth, float fHeight)
{
	x2Frame.x = fWidth;
	x2Frame.y = fHeight;
}

//=============================================================================
// ���_�J���[�̐ݒ�
//=============================================================================
void Polygon2D::SetColor(float fRed, float fGreen, float fBlue)
{
	if (fRed != g_colPolygon.x || fGreen != g_colPolygon.y || fBlue != g_colPolygon.z) {
		g_colPolygon.x = fRed;
		g_colPolygon.y = fGreen;
		g_colPolygon.z = fBlue;
		g_bInvalidate = true;
	}
}

void Polygon2D::SetPolygonAlpha(float fAlpha)
{
	if (fAlpha != g_colPolygon.w) {
		g_colPolygon.w = fAlpha;
		g_bInvalidate = true;
	}
}

void Polygon2D::RotateAroundY(float frot)
{
	Rotation.y += frot;
	if (Rotation.y >= 360)
		Rotation.y -= 360;
}

void Polygon2D::RotateAroundZ(float frot)
{
	Rotation.z += frot;
	if (Rotation.z >= 360)
		Rotation.z -= 360;
}


