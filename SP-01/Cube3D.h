#pragma once
#include "main.h"
#include "UniversalStructures.h"
#include "Light3D.h"
#define NUM_VERTEX_CUBE 24
class Cube3D
{
private:
	ID3D11ShaderResourceView*	g_pTexture;				// �e�N�X�`���ւ̃|�C���^
	VERTEX_3D					g_vertexWk[NUM_VERTEX_CUBE];	// ���_���i�[���[�N

	ID3D11Buffer*				g_pConstantBuffer[2];	// �萔�o�b�t�@
	ID3D11Buffer*				g_pVertexBuffer;		// ���_�o�b�t�@
	ID3D11SamplerState*			g_pSamplerState;		// �e�N�X�`�� �T���v��
	ID3D11VertexShader*			g_pVertexShader;		// ���_�V�F�[�_
	ID3D11InputLayout*			g_pInputLayout;			// ���_�t�H�[�}�b�g
	ID3D11PixelShader*			g_pPixelShader;			// �s�N�Z���V�F�[�_

	XMFLOAT4X4					g_mtxWorldField;		// ���[���h�}�g���b�N�X
	XMFLOAT4X4					g_mtxTexture;			// �e�N�X�`���}�g���b�N�X
	XMFLOAT3					Position;				// ���݂̈ʒu
	XMFLOAT3					Rotation;				// ���݂̌���
	XMFLOAT3					Scale;				// ���݂̌���

	// �}�e���A��
	XMFLOAT4						g_Ka;		// �A���r�G���g
	XMFLOAT4						g_Kd;		// �f�B�t���[�Y
	XMFLOAT4						g_Ks;		// �X�y�L����
	float						g_fPower;	// �X�y�L�������x
	XMFLOAT4						g_Ke;		// �G�~�b�V�u
	Light3D*					pSceneLight;
	int							nTextureSubDivisions;
	bool bIsPlane;
public:
	Cube3D();
	Cube3D(bool IsPlane);
	~Cube3D();
	HRESULT Init(const char* TexturePath);
	void End(void);
	void Update(void);
	void Draw(void);
	void SetLight(Light3D* SceneLight);
	HRESULT MakeVertex(ID3D11Device* pDevice);
	void SetPosition(XMFLOAT3 newPos);
	void SetRotation(XMFLOAT3 newRot);
	void SetScale(float newScale);
	void SetScale(XMFLOAT3 newScale);
	void SetTextureSubdivisions(int newSubs);
	Box GetHitbox();
	void SetAsPlane(bool);
};

