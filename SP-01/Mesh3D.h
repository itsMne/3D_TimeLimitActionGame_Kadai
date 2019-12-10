#pragma once
#include "DXWindow3D.h"

//**************************************
// �\���̒�`
//**************************************
typedef struct {
	XMFLOAT4	Diffuse;		// Diffuse color RGBA
	XMFLOAT4	Ambient;		// Ambient color RGB
	XMFLOAT4	Specular;		// Specular 'shininess'
	XMFLOAT4	Emissive;		// Emissive color RGB
	float		Power;			// Sharpness if specular highlight
} MATERIAL;
//*****************************************************************************
// �萔��`
//*****************************************************************************
enum ePrimitiveType {
	PT_UNDEFINED = 0,
	PT_POINT,
	PT_LINE,
	PT_LINESTRIP,
	PT_TRIANGLE,
	PT_TRIANGLESTRIP,

	MAX_PRIMITIVETYPE
};

typedef struct {
	ID3D11Buffer* pVertexBuffer;			// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
	ID3D11Buffer* pIndexBuffer;				// �C���f�b�N�X�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

	XMFLOAT4X4 mtxWorld;					// ���[���h�}�g���b�N�X
	XMFLOAT3 pos;							// �|���S���\���ʒu�̒��S���W
	XMFLOAT3 rot;							// �|���S���̉�]�p
	int nNumVertex;							// �����_��	
	int nNumIndex;							// ���C���f�b�N�X��

	XMFLOAT4X4 mtxTexture;					// �e�N�X�`�� �}�g���b�N�X
	ID3D11ShaderResourceView* pTexture;		// �e�N�X�`��

	ePrimitiveType primitiveType;			// �v���~�e�B�u�^

	MATERIAL* pMaterial = nullptr;					// �}�e���A��
} MESH;

class Mesh3D
{
protected:

	ID3D11Buffer*				g_pConstantBuffer[2];	// �萔�o�b�t�@
	ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
	ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
	ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
	ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_
	MESH* pMesh;
	// �}�e���A��
	MATERIAL						g_material;
	int							g_nAlpha;			// �A���t�@�e�X�g��臒l
	bool bisUnlit;
	bool bNoCull;
public:
	Mesh3D();
	~Mesh3D();
	virtual HRESULT Init();
	virtual void Update();
	void Draw(ID3D11DeviceContext* pDeviceContext);
	virtual void Uninit();
	float GetAlphaTestValue(void);
	HRESULT MakeMeshVertex(ID3D11Device* pDevice, MESH* pMesh,
		VERTEX_3D vertexWk[], int indexWk[]);
	void ReleaseMesh();
	void SetNoCull(bool);
	void SetUnlit(bool);
};


