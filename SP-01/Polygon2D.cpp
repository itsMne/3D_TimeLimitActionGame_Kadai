#include "Polygon2D.h"
#include "Shader.h"
#include "Texture.h"

int InitedPolygons = 0;

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWorld;		// ワールド変換行列(転置行列)
	XMMATRIX	mView;		// ビュー変換行列(転置行列)
	XMMATRIX	mProj;		// 射影変換行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ変換行列(転置行列)
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
	SetTexture(gpTexture);
	InitedPolygons++;
}

Polygon2D::~Polygon2D()
{
	UninitPolygon();
	InitedPolygons--;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Polygon2D::InitPolygon(ID3D11Device* pDevice)
{
	HRESULT hr = S_OK;

	// シェーダ初期化
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

	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SHADER_GLOBAL);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = pDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	if (FAILED(hr)) return hr;

	// テクスチャ サンプラ生成
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

	// 変換行列初期化
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

	// 頂点情報の作成
	hr = MakeVertexPolygon(pDevice);

	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void Polygon2D::UninitPolygon(void)
{
	// テクスチャ サンプラの開放
	SAFE_RELEASE(g_pSamplerState);
	// 頂点バッファの解放
	SAFE_RELEASE(g_pVertexBuffer);
	// 定数バッファの解放
	SAFE_RELEASE(g_pConstantBuffer);
	// ピクセルシェーダ解放
	SAFE_RELEASE(g_pPixelShader);
	// 頂点フォーマット解放
	SAFE_RELEASE(g_pInputLayout);
	// 頂点シェーダ解放
	SAFE_RELEASE(g_pVertexShader);
}

//=============================================================================
// 更新処理
//=============================================================================
void Polygon2D::UpdatePolygon(void)
{
	// (何もしない)
}

//=============================================================================
// 描画処理
//=============================================================================
void Polygon2D::DrawPolygon(ID3D11DeviceContext* pDeviceContext)
{
	// 拡縮
	XMMATRIX mWorld = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	// 回転
	mWorld *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(Rotation.x),
		XMConvertToRadians(Rotation.y), XMConvertToRadians(Rotation.z));
	// 移動
	mWorld *= XMMatrixTranslation(Position.x, Position.y, Position.z);
	// ワールド マトリックスに設定
	XMStoreFloat4x4(&g_mWorld, mWorld);

	if (g_pTexture) {
		// 拡縮
		mWorld = XMMatrixScaling(x2Frame.x, x2Frame.y, 1.0f);
		// 移動
		mWorld *= XMMatrixTranslation(x2UV.x, x2UV.y, 0.0f);
		// テクスチャ マトリックスに設定
		XMStoreFloat4x4(&g_mTex, mWorld);
	}
	else {
		// テクスチャ無し
		g_mTex._44 = 0.0f;
	}

	// 頂点バッファ更新
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

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	pDeviceContext->Draw(NUM_VERTEX, 0);
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT Polygon2D::MakeVertexPolygon(ID3D11Device* pDevice)
{
	// 頂点座標の設定
	g_vertexWk[0].vtx = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	g_vertexWk[1].vtx = XMFLOAT3(0.5f, 0.5f, 0.0f);
	g_vertexWk[2].vtx = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	g_vertexWk[3].vtx = XMFLOAT3(0.5f, -0.5f, 0.0f);

	// 拡散反射光の設定
	g_vertexWk[0].diffuse = g_colPolygon;
	g_vertexWk[1].diffuse = g_colPolygon;
	g_vertexWk[2].diffuse = g_colPolygon;
	g_vertexWk[3].diffuse = g_colPolygon;

	// テクスチャ座標の設定
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
// 頂点座標の設定
//=============================================================================
void Polygon2D::SetVertexPolygon(void)
{
	if (g_bInvalidate) {
		//頂点バッファの中身を埋める
		ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
		HRESULT hr = S_OK;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		D3D11_MAPPED_SUBRESOURCE msr;
		hr = pDeviceContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		if (SUCCEEDED(hr)) {
			// 拡散反射光の設定
			g_vertexWk[0].diffuse = g_colPolygon;
			g_vertexWk[1].diffuse = g_colPolygon;
			g_vertexWk[2].diffuse = g_colPolygon;
			g_vertexWk[3].diffuse = g_colPolygon;
			// 頂点データを上書き
			memcpy_s(msr.pData, sizeof(g_vertexWk), g_vertexWk, sizeof(g_vertexWk));
			// 頂点データをアンロックする
			pDeviceContext->Unmap(g_pVertexBuffer, 0);
			// フラグをクリア
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
// テクスチャの設定
//=============================================================================
void Polygon2D::SetTexture(ID3D11ShaderResourceView* pTexture)
{
	g_pTexture = pTexture;
	g_mTex._44 = (g_pTexture) ? 1.0f : 0.0f;
}

//=============================================================================
// 表示座標の設定
//=============================================================================
void Polygon2D::SetPolygonPos(float fX, float fY)
{
	Position.x = fX;
	Position.y = fY;
}

void Polygon2D::SetPolygonPosY(float fY)
{
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
// 表示サイズの設定
//=============================================================================
void Polygon2D::SetPolygonSize(float fScaleX, float fScaleY)
{
	Scale.x = fScaleX;
	Scale.y = fScaleY;
}

//=============================================================================
// 表示角度の設定(単位:度)
//=============================================================================
void Polygon2D::SetPolygonAngle(float fAngle)
{
	Rotation.z = fAngle;
}

//=============================================================================
// 左上テクスチャ座標の設定 (0.0≦fU＜1.0, 0.0≦fV＜1.0)
//=============================================================================
void Polygon2D::SetPolygonUV(float fU, float fV)
{
	x2UV.x = fU;
	x2UV.y = fV;
}

//=============================================================================
// テクスチャフレームサイズの設定 (0.0＜fWidth≦1.0, 0.0＜fHeight≦1.0)
//=============================================================================
void Polygon2D::SetPolygonFrameSize(float fWidth, float fHeight)
{
	x2Frame.x = fWidth;
	x2Frame.y = fHeight;
}

//=============================================================================
// 頂点カラーの設定
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


