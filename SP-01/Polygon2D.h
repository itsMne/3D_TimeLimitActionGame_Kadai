#ifndef	 POLYGON_H
#define  POLYGON_H
#include "main.h"
#include "UniversalStructures.h"
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//HRESULT InitPolygon(ID3D11Device* pDevice);

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// シェーダに渡す値

enum Direction
{
	RIGHT_DIR = 1,
	LEFT_DIR = -1
};

class Polygon2D
{
private:
	bool bPolygonInited;
protected:
	ID3D11ShaderResourceView*	g_pTexture;				// テクスチャへのポインタ

	VERTEX_2D					g_vertexWk[NUM_VERTEX];	// 頂点情報格納ワーク

	XMFLOAT3						g_posPolygon;			// ポリゴンの移動量
	XMFLOAT3						g_rotPolygon;			// ポリゴンの回転量
	XMFLOAT3						g_sizPolygon;			// ポリゴンのサイズ
	XMFLOAT4						g_colPolygon;			// ポリゴンの頂点カラー
	bool							g_bInvalidate;			// 頂点データ更新フラグ

	XMFLOAT2						g_posTexFrame;			// UV座標
	XMFLOAT2						g_sizTexFrame;			// テクスチャ抽出サイズ

	ID3D11Buffer*				g_pConstantBuffer;		// 定数バッファ
	ID3D11Buffer*				g_pVertexBuffer;		// 頂点バッファ
	ID3D11SamplerState*			g_pSamplerState;		// テクスチャ サンプラ
	ID3D11VertexShader*			g_pVertexShader;		// 頂点シェーダ
	ID3D11InputLayout*			g_pInputLayout;			// 頂点フォーマット
	ID3D11PixelShader*			g_pPixelShader;			// ピクセルシェーダ

	XMFLOAT4X4					g_mProj;				// 射影変換行列
	XMFLOAT4X4					g_mView;				// ビュー変換行列
	XMFLOAT4X4					g_mWorld;				// ワールド変換行列
	XMFLOAT4X4					g_mTex;					// テクスチャ変換行列

public:
	ID3D11ShaderResourceView * gpTexture;
	Polygon2D();
	Polygon2D(const char * TexturePath);
	~Polygon2D();
	HRESULT InitPolygon(ID3D11Device* pDevice);

	void UninitPolygon();
	void UpdatePolygon();
	void DrawPolygon(ID3D11DeviceContext* pDeviceContext);
	void SetPolygonTexture(ID3D11ShaderResourceView* pTexture);
	void SetPolygonPos(float fX, float fY);
	void SetPolygonSize(float fScaleX, float fScaleY);
	void SetPolygonAngle(float fAngle);
	void SetPolygonUV(float fU, float fV);
	void SetPolygonFrameSize(float fWidth, float fHeight);
	void SetPolygonColor(float fRed, float fGreen, float fBlue);
	void SetPolygonAlpha(float fAlpha);
	HRESULT MakeVertexPolygon(ID3D11Device* pDevice);
	void SetVertexPolygon(void);
};
#endif