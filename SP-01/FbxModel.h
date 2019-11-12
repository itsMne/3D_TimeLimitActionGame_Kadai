// FBXファイル読込/表示 (FbxModel.h)
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

// マクロ
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)			{if(p){(p)->Release();(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			{if(p){delete p;(p)=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	{if(p){delete[]p;(p)=nullptr;}}
#endif

#define MAX_REF_POLY	60		// 最大共有頂点参照数

// 定数
enum EByOpacity {
	eNoAffect = 0,		// 全て
	eOpacityOnly,		// 不透明のみ
	eTransparentOnly,	// 透明のみ
};

// マテリアル
struct TFbxMaterial {
	DirectX::XMFLOAT4	Ka;		// アンビエント+テクスチャ有無
	DirectX::XMFLOAT4	Kd;		// ディフューズ
	DirectX::XMFLOAT4	Ks;		// スペキュラ+スペキュラ強度
	DirectX::XMFLOAT4	Ke;		// エミッシブ
	ID3D11ShaderResourceView*	pTexture;		// 拡散テクスチャ
	ID3D11ShaderResourceView*	pTexEmmisive;	// 発光テクスチャ
	DWORD		dwNumFace;		// このマテリアルのポリゴン数
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

// 頂点情報
struct TFbxVertex {
	DirectX::XMFLOAT3	vPos;	// 頂点位置
	DirectX::XMFLOAT3	vNorm;	// 頂点法線
	DirectX::XMFLOAT2	vTex;	// UV座標
	UINT		uBoneIndex[4];	// ボーン番号
	float		fBoneWeight[4];	// ボーン重み
	TFbxVertex()
	{
		ZeroMemory(this, sizeof(TFbxVertex));
	}
};

// ボーン
struct TBone {
	DirectX::XMFLOAT4X4 mBindPose;	// 初期ポーズ (ずっと変わらない)
	DirectX::XMFLOAT4X4 mNewPose;	// 現在のポーズ (その都度変わる)

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

// 1頂点の共有 (最大MAX_REF_POLYポリゴン)
struct TPolyTable {
	int nPolyIndex[MAX_REF_POLY];	// ポリゴン番号 
	int nIndex123[MAX_REF_POLY];	// 3つの頂点のうち、何番目か
	int nNumRef;					// 属しているポリゴン数

	TPolyTable()
	{
		ZeroMemory(this, sizeof(TPolyTable));
	}
};

//
// FBXモデル光源
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

// メッシュ クラス
class CFbxMesh
{
public:
	CFbxMesh();
	~CFbxMesh();

	CFbxMesh** m_ppChild;	// 自分の子へのポインタ
	DWORD m_dwNumChild;		// 子の数

public:
	// 外部のデバイス等情報
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;
	ID3D11SamplerState* m_pSampleLinear;
	ID3D11Buffer* m_pConstantBuffer0;
	ID3D11Buffer* m_pConstantBuffer1;
	ID3D11Buffer* m_pConstantBufferBone;
	FbxNode* m_pFBXNode;						// FBXから姿勢行列を取り出す際に使うFBXポインタ
	DirectX::XMFLOAT4X4 m_mView;
	DirectX::XMFLOAT4X4 m_mProj;
	CFbxLight* m_pLight;
	DirectX::XMFLOAT3* m_pCamera;
	// アニメーション関連
	DirectX::XMFLOAT4X4 m_mParentOrientation;	// 親の姿勢行列
	DirectX::XMFLOAT4X4 m_mFBXOrientation;		// 自分の姿勢行列 (親から見た相対姿勢)
	DirectX::XMFLOAT4X4 m_mFinalWorld;			// 最終的なワールド行列 (この姿勢でレンダリングする)

private:
	// メッシュ関連
	DWORD m_dwNumVert;
	DWORD m_dwNumFace;
	DWORD m_dwNumUV;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer** m_ppIndexBuffer;
	TFbxMaterial* m_pMaterial;
	DWORD m_dwNumMaterial;
	// ボーン
	int m_nNumSkin;
	TSkinInfo* m_pBoneTable;
public:
	// メソッド
	HRESULT CreateFromFBX(FbxMesh* pFbxMesh);
	void RenderMesh(EByOpacity byOpacity = eNoAffect);
	void SetNewPoseMatrices(int nFrame);

private:
	HRESULT CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer);
	HRESULT ReadSkinInfo(FbxMesh* pFbxMesh, TFbxVertex* pvVB, TPolyTable* PolyTable);
	DirectX::XMFLOAT4X4 GetCurrentPoseMatrix(int nSkin, int nIndex);
};

//
// FBXモデル クラス
//
class CFbxModel
{
public:
	CFbxModel();
	~CFbxModel();

	// メソッド
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
	// 外部のデバイス等情報
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
	DirectX::XMFLOAT4X4 m_mFinalWorld;//最終的なワールド行列（この姿勢でレンダリングする）
	CFbxLight m_light;
	DirectX::XMFLOAT3 m_vCamera;
	int nAnimationNumber;
private:
	// アニメーションフレーム
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
