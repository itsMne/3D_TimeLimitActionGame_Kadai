// FBX�t�@�C���Ǎ�/�\�� (FbxModel.h)
//#define D3DCOMPILER
#pragma once
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <fbxsdk.h>
#ifdef D3DCOMPILER
#include <d3dcompiler.h>
#endif

// �}�N��
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{if(p){(p)->Release();(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{if(p){delete p;(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{if(p){delete[]p;(p)=nullptr;}}
#endif

#define MAX_REF_POLY	60		// �ő勤�L���_�Q�Ɛ�

// �萔
enum EByOpacity {
	eNoAffect = 0,		// �S��
	eOpacityOnly,		// �s�����̂�
	eTransparentOnly,	// �����̂�
};

// �}�e���A��
struct TFbxMaterial {
	DirectX::XMFLOAT4	Ka;		// �A���r�G���g+�e�N�X�`���L��
	DirectX::XMFLOAT4	Kd;		// �f�B�t���[�Y
	DirectX::XMFLOAT4	Ks;		// �X�y�L����+�X�y�L�������x
	DirectX::XMFLOAT4	Ke;		// �G�~�b�V�u
	ID3D11ShaderResourceView*	pTexture;		// �g�U�e�N�X�`��
	ID3D11ShaderResourceView*	pTexEmmisive;	// �����e�N�X�`��
	DWORD		dwNumFace;		// ���̃}�e���A���̃|���S����
	TFbxMaterial()
	{
		ZeroMemory(this, sizeof(TFbxMaterial));
		Kd = DirectX::XMFLOAT4(1, 1, 1, 1);
	}
	~TFbxMaterial()
	{
		SAFE_RELEASE(pTexture);
	}
};

// ���_���
struct TFbxVertex {
	DirectX::XMFLOAT3	vPos;	// ���_�ʒu
	DirectX::XMFLOAT3	vNorm;	// ���_�@��
	DirectX::XMFLOAT2	vTex;	// UV���W
	UINT		uBoneIndex[4];	// �{�[���ԍ�
	float		fBoneWeight[4];	// �{�[���d��
	TFbxVertex()
	{
		ZeroMemory(this, sizeof(TFbxVertex));
	}
};

// �{�[��
struct TBone {
	DirectX::XMFLOAT4X4 mBindPose;	// �����|�[�Y (�����ƕς��Ȃ�)
	DirectX::XMFLOAT4X4 mNewPose;	// ���݂̃|�[�Y (���̓s�x�ς��)

	TBone()
	{
		ZeroMemory(this, sizeof(TBone));
	}
};

struct TSkinInfo {
	int				nNumBone;
	TBone*			pBoneArray;
	FbxCluster**	ppCluster;
	TSkinInfo()
	{
		ZeroMemory(this, sizeof(TSkinInfo));
	}
};

// 1���_�̋��L (�ő�MAX_REF_POLY�|���S��)
struct TPolyTable {
	int nPolyIndex[MAX_REF_POLY];	// �|���S���ԍ� 
	int nIndex123[MAX_REF_POLY];	// 3�̒��_�̂����A���Ԗڂ�
	int nNumRef;					// �����Ă���|���S����

	TPolyTable()
	{
		ZeroMemory(this, sizeof(TPolyTable));
	}
};

//
// FBX���f������
//
class CFbxLight
{
public:
	CFbxLight();

	DirectX::XMFLOAT4 m_diffuse;
	DirectX::XMFLOAT4 m_ambient;
	DirectX::XMFLOAT4 m_specular;
	DirectX::XMFLOAT3 m_direction;
};

// ���b�V�� �N���X
class CFbxMesh
{
public:
	CFbxMesh();
	~CFbxMesh();

	CFbxMesh** m_ppChild;	// �����̎q�ւ̃|�C���^
	DWORD m_dwNumChild;		// �q�̐�

public:
	// �O���̃f�o�C�X�����
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11SamplerState* m_pSampleLinear;
	ID3D11Buffer* m_pConstantBuffer0;
	ID3D11Buffer* m_pConstantBuffer1;
	ID3D11Buffer* m_pConstantBufferBone;
	FbxNode* m_pFBXNode;						// FBX����p���s������o���ۂɎg��FBX�|�C���^
	DirectX::XMFLOAT4X4 m_mView;
	DirectX::XMFLOAT4X4 m_mProj;
	CFbxLight* m_pLight;
	DirectX::XMFLOAT3* m_pCamera;
	// �A�j���[�V�����֘A
	DirectX::XMFLOAT4X4 m_mParentOrientation;	// �e�̎p���s��
	DirectX::XMFLOAT4X4 m_mFBXOrientation;		// �����̎p���s�� (�e���猩�����Ύp��)
	DirectX::XMFLOAT4X4 m_mFinalWorld;			// �ŏI�I�ȃ��[���h�s�� (���̎p���Ń����_�����O����)

private:
	// ���b�V���֘A
	DWORD m_dwNumVert;
	DWORD m_dwNumFace;
	DWORD m_dwNumUV;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer** m_ppIndexBuffer;
	TFbxMaterial* m_pMaterial;
	DWORD m_dwNumMaterial;
	// �{�[��
	int m_nNumSkin;
	TSkinInfo* m_pBoneTable;
public:
	// ���\�b�h
	HRESULT CreateFromFBX(FbxMesh* pFbxMesh);
	void RenderMesh(EByOpacity byOpacity = eNoAffect);
	void SetNewPoseMatrices(int nFrame);

private:
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	HRESULT ReadSkinInfo(FbxMesh* pFbxMesh, TFbxVertex* pvVB, TPolyTable* PolyTable);
	DirectX::XMFLOAT4X4 GetCurrentPoseMatrix(int nSkin, int nIndex);
};

//
// FBX���f�� �N���X
//
class CFbxModel
{
public:
	CFbxModel();
	~CFbxModel();

	// ���\�b�h
	void Render(DirectX::XMFLOAT4X4& mWorld, DirectX::XMFLOAT4X4& mView, DirectX::XMFLOAT4X4& mProj, EByOpacity byOpacity = eNoAffect);
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LPCSTR pszFileName);
	void SetLight(CFbxLight& light);
	void SetCamera(DirectX::XMFLOAT3& vCamera);
	void SetAnimFrame(int nFrame);
	int GetMaxAnimFrame();
	int GetInitialAnimFrame();
	int GetCurrentAnimation();
	void SetAnimStack(int nAnimStack);
	int GetMaxNumberOfAnimations();
private:
	CFbxMesh* m_pRootMesh;
	// �O���̃f�o�C�X�����
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext *m_pDeviceContext;
	ID3D11SamplerState* m_pSampleLinear;
	ID3D11Buffer* m_pConstantBuffer0;
	ID3D11Buffer* m_pConstantBuffer1;
	ID3D11Buffer* m_pConstantBufferBone;
	ID3D11InputLayout* m_pVertexLayout;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	DirectX::XMFLOAT4X4 m_mView;
	DirectX::XMFLOAT4X4 m_mProj;
	DirectX::XMFLOAT4X4 m_mWorld;
	// FBX
	FbxManager *m_pSdkManager;
	FbxImporter* m_pImporter;
	FbxScene* m_pScene;
	DirectX::XMFLOAT4X4 m_mFinalWorld;//�ŏI�I�ȃ��[���h�s��i���̎p���Ń����_�����O����j
	CFbxLight m_light;
	DirectX::XMFLOAT3 m_vCamera;
	int nAnimationNumber;
private:
	// �A�j���[�V�����t���[��
	int m_nAnimFrame;
	FbxTime m_tStart;
	FbxTime m_tStop;
	int m_nAnimStack;
	FbxArray<FbxString*> m_strAnimStackName;

	HRESULT InitShader();
	HRESULT InitFBX(LPCSTR szFileName);
	HRESULT CreateFromFBX(LPCSTR szFileName);
	HRESULT LoadRecursive(FbxNode* pNode, CFbxMesh* pFBXMesh);
	void RenderMesh(CFbxMesh* pMesh, EByOpacity byOpacity);
	void RecursiveRender(CFbxMesh* pMesh, EByOpacity byOpacity);
	void SetNewPoseMatrices(CFbxMesh* pMesh, int nFrame);
	void DestroyMesh(CFbxMesh* pMesh);
};
