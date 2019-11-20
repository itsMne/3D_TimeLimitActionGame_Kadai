#pragma once
#include "main.h"
#include "UniversalStructures.h"
#include "Light3D.h"
#define NUM_VERTEX_CUBE 24
class Cube3D
{
private:
	ID3D11ShaderResourceView*	g_pTexture;				// テクスチャへのポインタ
	VERTEX_3D					g_vertexWk[NUM_VERTEX_CUBE];	// 頂点情報格納ワーク

	ID3D11Buffer*				g_pConstantBuffer[2];	// 定数バッファ
	ID3D11Buffer*				g_pVertexBuffer;		// 頂点バッファ
	ID3D11SamplerState*			g_pSamplerState;		// テクスチャ サンプラ
	ID3D11VertexShader*			g_pVertexShader;		// 頂点シェーダ
	ID3D11InputLayout*			g_pInputLayout;			// 頂点フォーマット
	ID3D11PixelShader*			g_pPixelShader;			// ピクセルシェーダ

	XMFLOAT4X4					g_mtxWorldField;		// ワールドマトリックス
	XMFLOAT4X4					g_mtxTexture;			// テクスチャマトリックス
	XMFLOAT3					Position;				// 現在の位置
	XMFLOAT3					Rotation;				// 現在の向き
	XMFLOAT3					Scale;				// 現在の向き

	// マテリアル
	XMFLOAT4						g_Ka;		// アンビエント
	XMFLOAT4						g_Kd;		// ディフューズ
	XMFLOAT4						g_Ks;		// スペキュラ
	float						g_fPower;	// スペキュラ強度
	XMFLOAT4						g_Ke;		// エミッシブ
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

