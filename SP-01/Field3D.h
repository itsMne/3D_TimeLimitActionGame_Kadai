#pragma once
#include "main.h"
#include "UniversalStructures.h"
#include "GameObject3D.h"
#include "Light3D.h"

class Field3D: public GameObject3D
{
private:
	ID3D11ShaderResourceView*	g_pTexture;				// �e�N�X�`���ւ̃|�C���^
	VERTEX_3D					g_vertexWk[NUM_VERTEX];	// ���_���i�[���[�N

	ID3D11Buffer*				g_pConstantBuffer[2];	// �萔�o�b�t�@
	ID3D11Buffer*				g_pVertexBuffer;		// ���_�o�b�t�@
	ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
	ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
	ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
	ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_

	XMFLOAT4X4					g_mtxWorldField;		// ���[���h�}�g���b�N�X
	XMFLOAT4X4					g_mtxTexture;			// �e�N�X�`���}�g���b�N�X
	//XMFLOAT3					Position;				// ���݂̈ʒu
	//XMFLOAT3					Rotation;				// ���݂̌���
	//XMFLOAT3					Scale;				// ���݂̌���

	// �}�e���A��
	XMFLOAT4						g_Ka;		// �A���r�G���g
	XMFLOAT4						g_Kd;		// �f�B�t���[�Y
	XMFLOAT4						g_Ks;		// �X�y�L����
	float						g_fPower;	// �X�y�L�������x
	XMFLOAT4						g_Ke;		// �G�~�b�V�u
	Light3D*					pSceneLight;
	int							nTextureSubDivisions;
	ID3D11Buffer* mVertexBuffer;

	// index buffer, only 6 indexes
	ID3D11Buffer* mIndexBuffer;
public:
	Field3D();
	~Field3D();
	HRESULT InitField(Light3D* SceneLight, const char* TexturePath);
	void UninitField(void);
	void UpdateField(void);
	void DrawField(void);
	void SetFieldLight(Light3D* SceneLight);
	HRESULT MakeVertexField(ID3D11Device* pDevice);
	void SetPosition(XMFLOAT3 newPos);
	void SetRotation(XMFLOAT3 newRot);
	void SetScale(float newScale);
	void SetTextureSubdivisions(int newSubs);
	Box GetHitbox();
};

