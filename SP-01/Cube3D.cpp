//=============================================================================
//
// �n�ʏ��� [field.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "Cube3D.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera3D.h"
#include "UniversalStructures.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define	TEXTURE_FIELD	L"data/texture/field000.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
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


Cube3D::Cube3D()
{
	pSceneLight = nullptr;
	nTextureSubDivisions = 1;
	bIsPlane = false;
}

Cube3D::Cube3D(bool IsPlane)
{
	pSceneLight = nullptr;
	nTextureSubDivisions = 1;
	bIsPlane = IsPlane;
}


Cube3D::~Cube3D()
{
	End();
}


//=============================================================================
// ����������
//=============================================================================
HRESULT Cube3D::Init(const char* TexturePath)
{
	SetLight(GetMainLight());
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
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = { 1,1,1 };
	// �}�e���A���̏����ݒ�
	g_Kd = M_DIFFUSE;
	g_Ka = M_AMBIENT;
	g_Ks = M_SPECULAR;
	g_fPower = 0.0f;
	g_Ke = M_EMISSIVE;

	// �e�N�X�`���̓ǂݍ���
	hr = CreateTextureFromFile(pDevice,			// �f�o�C�X�ւ̃|�C���^
		TexturePath,	// �t�@�C���̖��O
		&g_pTexture);	// �ǂݍ��ރ������[
	if (FAILED(hr))
		return hr;

	// ���_���̍쐬
	hr = MakeVertex(pDevice);
	return hr;
}

//=============================================================================
// �I������
//=============================================================================
void Cube3D::End(void)
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
void Cube3D::Update(void)
{
	
}

//=============================================================================
// �`�揈��
//=============================================================================
void Cube3D::Draw(void)
{
	Camera3D* pMainCamera = GetMainCamera();
	if (!pMainCamera)
	{
		printf("���C���J����������܂���\n");
		return;
	}
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	//�傫��
	mtxScale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
	// ��]�𔽉f
	mtxRot = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(Position.x, Position.y, Position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	XMStoreFloat4x4(&g_mtxWorldField, mtxWorld);

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &g_pTexture);

	SHADER_GLOBAL cb;

	cb.mWVP = XMMatrixTranspose(mtxWorld *
		XMLoadFloat4x4(&(pMainCamera->GetViewMatrix())) * XMLoadFloat4x4(&(pMainCamera->GetProjMatrix())));
	cb.mW = XMMatrixTranspose(mtxWorld);
	cb.mTex = XMMatrixTranspose(XMLoadFloat4x4(&g_mtxTexture));
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[0], 0, nullptr, &cb, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer[0]);
	SHADER_GLOBAL2 cb2;
	cb2.vEye = XMLoadFloat3(&(pMainCamera->GetCameraPos()));
	CFbxLight& light = *pSceneLight->GetLight();
	if (pSceneLight)
	{
		cb2.vLightDir = XMVectorSet(light.m_direction.x, light.m_direction.y, light.m_direction.z, 0.f);
		cb2.vLa = XMLoadFloat4(&light.m_ambient);
		cb2.vLd = XMLoadFloat4(&light.m_diffuse);
		cb2.vLs = XMLoadFloat4(&light.m_specular);
	}
	else
		printf("��������܂���\n");

	cb2.vDiffuse = XMLoadFloat4(&g_Kd);
	cb2.vAmbient = XMVectorSet(g_Ka.x, g_Ka.y, g_Ka.z,
		(g_pTexture != nullptr) ? 1.f : 0.f);
	cb2.vSpecular = XMVectorSet(g_Ks.x, g_Ks.y, g_Ks.z, g_fPower);
	cb2.vEmissive = XMLoadFloat4(&g_Ke);
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer[1]);

	// �v���~�e�B�u�`����Z�b�g
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �|���S���̕`��
	pDeviceContext->DrawIndexed(NUM_VERTEX_CUBE, 0,0);

}

void Cube3D::SetLight(Light3D * SceneLight)
{
	pSceneLight = SceneLight;
}

//=============================================================================
// ���_�̍쐬
//=============================================================================
HRESULT Cube3D::MakeVertex(ID3D11Device* pDevice)
{
	// ���_���W�̐ݒ�
	/*g_vertexWk[0].vtx = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	g_vertexWk[1].vtx = XMFLOAT3(1.0f, -1.0f, -1.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	g_vertexWk[3].vtx = XMFLOAT3(1.0f, 1.0f, -1.0f);

	g_vertexWk[4].vtx = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	g_vertexWk[5].vtx = XMFLOAT3(1.0f, -1.0f, 1.0f);
	g_vertexWk[6].vtx = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	g_vertexWk[7].vtx = XMFLOAT3(1.0f, 1.0f, 1.0f);
	
	g_CubeVertex[0].Position = nn::util:: XMFLOAT3(-1.0f, -1.0f, 1.0f);
	g_CubeVertex[1].Position = nn::util:: XMFLOAT3(1.0f, -1.0f, 1.0f);
	g_CubeVertex[2].Position = nn::util:: XMFLOAT3(-1.0f, 1.0f, 1.0f);
	g_CubeVertex[3].Position = nn::util:: XMFLOAT3(1.0f, 1.0f, 1.0f);							  							  
	g_CubeVertex[4].Position = nn::util:: XMFLOAT3(1.0f, -1.0f, 1.0f);
	g_CubeVertex[5].Position = nn::util:: XMFLOAT3(1.0f, -1.0f, -1.0f);
	g_CubeVertex[6].Position = nn::util:: XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_CubeVertex[7].Position = nn::util:: XMFLOAT3(1.0f, 1.0f, -1.0f);							
	g_CubeVertex[8].Position = nn::util:: XMFLOAT3(1.0f, -1.0f, -1.0f);
	g_CubeVertex[9].Position = nn::util:: XMFLOAT3(-1.0f, -1.0f, -1.0f);
	g_CubeVertex[10].Position = nn::util::XMFLOAT3(1.0f, 1.0f, -1.0f);
	g_CubeVertex[11].Position = nn::util::XMFLOAT3(-1.0f, 1.0f, -1.0f);							  
	g_CubeVertex[12].Position = nn::util::XMFLOAT3(-1.0f, -1.0f, -1.0f);
	g_CubeVertex[13].Position = nn::util::XMFLOAT3(-1.0f, -1.0f, 1.0f);
	g_CubeVertex[14].Position = nn::util::XMFLOAT3(-1.0f, 1.0f, -1.0f);
	g_CubeVertex[15].Position = nn::util::XMFLOAT3(-1.0f, 1.0f, 1.0f);							  
	g_CubeVertex[16].Position = nn::util::XMFLOAT3(-1.0f, -1.0f, -1.0f);
	g_CubeVertex[17].Position = nn::util::XMFLOAT3(1.0f, -1.0f, -1.0f);
	g_CubeVertex[18].Position = nn::util::XMFLOAT3(-1.0f, -1.0f, 1.0f);
	g_CubeVertex[19].Position = nn::util::XMFLOAT3(1.0f, -1.0f, 1.0f);							  
	g_CubeVertex[20].Position = nn::util::XMFLOAT3(-1.0f, 1.0f, 1.0f);
	g_CubeVertex[21].Position = nn::util::XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_CubeVertex[22].Position = nn::util::XMFLOAT3(-1.0f, 1.0f, -1.0f);
	g_CubeVertex[23].Position = nn::util::XMFLOAT3(1.0f, 1.0f, -1.0f);
	*/
	/*g_vertexWk[0].vtx = XMFLOAT3(-1.0f, 1.0f, -1.0f);
	g_vertexWk[1].vtx = XMFLOAT3(1.0f, 1.0f, -1.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-1.0f, -1.0f, -1.0f);
	g_vertexWk[3].vtx = XMFLOAT3(1.0f, -1.0f, -1.0f);
	g_vertexWk[4].vtx = XMFLOAT3(-1.0f, 1.0f, 1.0f);
	g_vertexWk[5].vtx = XMFLOAT3(1.0f, 1.0f, 1.0f);
	g_vertexWk[6].vtx = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	g_vertexWk[7].vtx = XMFLOAT3(1.0f, -1.0f, 1.0f);*/
	if (bIsPlane) {
		g_vertexWk[0].vtx = XMFLOAT3(-1.0f, -1.0f, -1.0f);
		g_vertexWk[1].vtx = XMFLOAT3(1.0f, -1.0f, -1.0f);
		g_vertexWk[2].vtx = XMFLOAT3(-1.0f, -1.0f, 1.0f);
		g_vertexWk[3].vtx = XMFLOAT3(1.0f, -1.0f, 1.0f);

		g_vertexWk[0].nor = XMFLOAT3(0.0f, +1.0f, 0.0f);
		g_vertexWk[1].nor = XMFLOAT3(0.0f, +1.0f, 0.0f);
		g_vertexWk[2].nor = XMFLOAT3(0.0f, +1.0f, 0.0f);
		g_vertexWk[3].nor = XMFLOAT3(0.0f, +1.0f, 0.0f);
		g_vertexWk[4].nor = XMFLOAT3(0.0f, +1.0f, 0.0f);
	}
	else {
		g_vertexWk[0].vtx = XMFLOAT3(-1.0f, -1.0f, -1.0f);
		g_vertexWk[1].vtx = XMFLOAT3(-1.0f,  1.0f, -1.0f);
		g_vertexWk[2].vtx = XMFLOAT3(1.0f,  1.0f, -1.0f );
		g_vertexWk[3].vtx = XMFLOAT3(1.0f, -1.0f, -1.0f ); 
		g_vertexWk[4].vtx = XMFLOAT3(-1.0f, -1.0f, 1.0f );
		g_vertexWk[5].vtx = XMFLOAT3(1.0f, -1.0f, 1.0f  );
		g_vertexWk[6].vtx = XMFLOAT3(1.0f,  1.0f, 1.0f  );
		g_vertexWk[7].vtx = XMFLOAT3(-1.0f,  1.0f, 1.0f ); 
		g_vertexWk[8].vtx = XMFLOAT3(-1.0f, 1.0f, -1.0f );
		g_vertexWk[9].vtx = XMFLOAT3(-1.0f, 1.0f,  1.0f );
		g_vertexWk[10].vtx =XMFLOAT3(1.0f, 1.0f,  1.0f  );
		g_vertexWk[11].vtx =XMFLOAT3(1.0f, 1.0f, -1.0f  );  
		g_vertexWk[12].vtx =XMFLOAT3(-1.0f, -1.0f, -1.0f);
		g_vertexWk[13].vtx =XMFLOAT3(1.0f, -1.0f, -1.0f );
		g_vertexWk[14].vtx =XMFLOAT3(1.0f, -1.0f,  1.0f );
		g_vertexWk[15].vtx =XMFLOAT3(-1.0f, -1.0f,  1.0f);
		g_vertexWk[16].vtx =XMFLOAT3(-1.0f, -1.0f,  1.0f);
		g_vertexWk[17].vtx =XMFLOAT3(-1.0f,  1.0f,  1.0f);
		g_vertexWk[18].vtx =XMFLOAT3(-1.0f,  1.0f, -1.0f);
		g_vertexWk[19].vtx =XMFLOAT3(-1.0f, -1.0f, -1.0f);
		g_vertexWk[20].vtx =XMFLOAT3(1.0f, -1.0f, -1.0f );
		g_vertexWk[21].vtx =XMFLOAT3(1.0f,  1.0f, -1.0f );
		g_vertexWk[22].vtx =XMFLOAT3(1.0f,  1.0f,  1.0f );
		g_vertexWk[23].vtx =XMFLOAT3(1.0f, -1.0f,  1.0f );

		/*
		        // Front Face
        XMFLOAT3(-1.0f, -1.0f, -1.0f),
        XMFLOAT3(-1.0f,  1.0f, -1.0f),
        XMFLOAT3(1.0f,  1.0f, -1.0f),
        XMFLOAT3(1.0f, -1.0f, -1.0f),       
        XMFLOAT3(-1.0f, -1.0f, 1.0f),
        XMFLOAT3(1.0f, -1.0f, 1.0f),
        XMFLOAT3(1.0f,  1.0f, 1.0f),
        XMFLOAT3(-1.0f,  1.0f, 1.0f),             
        XMFLOAT3(-1.0f, 1.0f, -1.0f),
        XMFLOAT3(-1.0f, 1.0f,  1.0f),
        XMFLOAT3(1.0f, 1.0f,  1.0f),
        XMFLOAT3(1.0f, 1.0f, -1.0f),          
        XMFLOAT3(-1.0f, -1.0f, -1.0f),
        XMFLOAT3(1.0f, -1.0f, -1.0f),
        XMFLOAT3(1.0f, -1.0f,  1.0f),
        XMFLOAT3(-1.0f, -1.0f,  1.0f),          
        XMFLOAT3(-1.0f, -1.0f,  1.0f),
        XMFLOAT3(-1.0f,  1.0f,  1.0f),
        XMFLOAT3(-1.0f,  1.0f, -1.0f),
        XMFLOAT3(-1.0f, -1.0f, -1.0f),         
        XMFLOAT3(1.0f, -1.0f, -1.0f),
        XMFLOAT3(1.0f,  1.0f, -1.0f),
        XMFLOAT3(1.0f,  1.0f,  1.0f),
        XMFLOAT3(1.0f, -1.0f,  1.0f),
		*/
		for (int i = 0; i < NUM_VERTEX_CUBE; i++)
		{
			switch (i)
			{
			case 0:case 1:case 2:case 3://front
				g_vertexWk[i].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
				break;
			case 4:case 5:case 6:case 7://Back
				g_vertexWk[i].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
				break;
			case 8:case 9:case 10:case 11://top
				g_vertexWk[i].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
				break;
			case 12:case 13:case 14:case 15://bottom
				g_vertexWk[i].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
				break;
			case 16:case 17:case 18:case 19://left
				g_vertexWk[i].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
				break;
			case 20:case 21:case 22:case 23://Right
				g_vertexWk[i].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
				break;
			default:
				break;
			}
		}
	}
	// �g�U���ˌ��̐ݒ�
	//g_vertexWk[0].diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	//g_vertexWk[1].diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//g_vertexWk[2].diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	for (int i = 0; i < NUM_VERTEX_CUBE; i++)
	{
		g_vertexWk[i].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	// �e�N�X�`�����W�̐ݒ�
	
	if (bIsPlane) {
		for (int i = 0; i < NUM_VERTEX_CUBE; i += 4)
		{
			g_vertexWk[i + 0].tex = XMFLOAT2(0.0f*nTextureSubDivisions, 0.0f*nTextureSubDivisions);
			g_vertexWk[i + 1].tex = XMFLOAT2(1.0f*nTextureSubDivisions, 0.0f*nTextureSubDivisions);
			g_vertexWk[i + 2].tex = XMFLOAT2(0.0f*nTextureSubDivisions, 1.0f*nTextureSubDivisions);
			g_vertexWk[i + 3].tex = XMFLOAT2(1.0f*nTextureSubDivisions, 1.0f*nTextureSubDivisions);
		}
	}
	else {
		for (int i = 0; i < NUM_VERTEX_CUBE; i += 4)
		{
			if (12 == i)
			{
				g_vertexWk[i + 3].tex = XMFLOAT2(0.0f, 0.0f);
				g_vertexWk[i + 1].tex = XMFLOAT2(0.0f, 1.0f);
				g_vertexWk[i + 2].tex = XMFLOAT2(1.0f, 0.0f);
				g_vertexWk[i + 0].tex = XMFLOAT2(1.0f, 1.0f);
			}
			else {
				g_vertexWk[i + 0].tex = XMFLOAT2(0.0f, 1.0f);
				g_vertexWk[i + 1].tex = XMFLOAT2(1.0f, 1.0f);
				g_vertexWk[i + 2].tex = XMFLOAT2(0.0f, 0.0f);
				g_vertexWk[i + 3].tex = XMFLOAT2(1.0f, 0.0f);
			}
		}
	}
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


	const unsigned short indices[] =
	{
				// Front Face
				0,  1,  2,
				0,  2,  3,

				// Back Face
				4,  5,  6,
				4,  6,  7,

				// Top Face
				8,  9, 10,
				8, 10, 11,

				// Bottom Face
				12, 13, 14,
				12, 14, 15,

				// Left Face
				16, 17, 18,
				16, 18, 19,

				// Right Face
				20, 21, 22,
				20, 22, 23
	};
	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = sizeof(indices);
	ibd.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices;
	//GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0u);
	//GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	hr = pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer);
	return hr;
}

void Cube3D::SetPosition(XMFLOAT3 newPos)
{
	Position = newPos;
}

void Cube3D::SetRotation(XMFLOAT3 newRot)
{
	Rotation = newRot;
}

void Cube3D::SetScale(float newScale)
{
	Scale = { newScale ,newScale ,newScale };
}

void Cube3D::SetScale(XMFLOAT3 newScale)
{
	Scale = newScale;
}

void Cube3D::SetTextureSubdivisions(int newSubs)
{
	nTextureSubDivisions = newSubs;
}

Box Cube3D::GetHitbox()
{
	return { Position.x, Position.y, Position.z, Scale.x, Scale.y, Scale.z };
}

void Cube3D::SetAsPlane(bool isPlane)
{
	bIsPlane = isPlane;
}
