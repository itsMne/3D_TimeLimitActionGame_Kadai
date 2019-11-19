#pragma once

#include "main.h"
#include "Mesh3D.h"
#include "UniversalStructures.h"



class Billboard2D: 
	public Mesh3D
{
private:
	MATERIAL						g_material;			// マテリアル
	bool							g_bAlphaTest;		// アルファテストON/OFF
	bool							g_bInTree;			// 木処理中
	XMFLOAT4 Color;			// 色
	XMFLOAT4X4 mtxWorld;	// ワールドマトリックス
	XMFLOAT3 Position;			// 位置
	XMFLOAT3 Rotation;			// 位置

	UV			uv;	// フレームNo.
	int			nAnimeCount;	// カウンタ
	int			nFrameX, nFrameY;
	float fWidth;			// 幅
	float fHeight;			// 高さ
	bool bUse;				// 使用しているかどうか
public:
	Billboard2D(const char* szpath);
	~Billboard2D();
	HRESULT Init(const char* szpath);
	void Update();
	void Draw();
	void End();
	void SetBillboard(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);
	HRESULT MakeVertex(ID3D11Device* pDevice);
	void SetVertex(float fWidth, float fHeight);
	void SetColor(XMFLOAT4 col);
	void SetPosition(XMFLOAT3 newPosition);
	void SetWidth(float newWidth);
	void SetHeight(float newHeight);
	void SetUVFrames(int nX, int nY);
};