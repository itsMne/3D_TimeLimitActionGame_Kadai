//=============================================================================
//
// 地面処理 [field.cpp]
// Author : HIROHIKO HAMAYA
//
//=============================================================================
#include "Cube3D.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera3D.h"
#include "UniversalStructures.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define	TEXTURE_FIELD	L"data/texture/field000.jpg"	// 読み込むテクスチャファイル名
#define M_DIFFUSE		XMFLOAT4(1.0f,1.0f,1.0f,1.0f)
#define M_SPECULAR		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
#define M_AMBIENT		XMFLOAT4(0.0f,0.0f,0.0f,1.0f)
#define M_EMISSIVE		XMFLOAT4(0.0f,0.0f,0.0f,0.0f)
//*****************************************************************************
// 構造体定義
//*****************************************************************************
// シェーダに渡す値
struct SHADER_GLOBAL {
	XMMATRIX	mWVP;		// ワールド×ビュー×射影行列(転置行列)
	XMMATRIX	mW;			// ワールド行列(転置行列)
	XMMATRIX	mTex;		// テクスチャ行列(転置行列)
};
struct SHADER_GLOBAL2 {
	XMVECTOR	vEye;		// 視点座標
	// 光源
	XMVECTOR	vLightDir;	// 光源方向
	XMVECTOR	vLa;		// 光源色(アンビエント)
	XMVECTOR	vLd;		// 光源色(ディフューズ)
	XMVECTOR	vLs;		// 光源色(スペキュラ)
	// マテリアル
	XMVECTOR	vAmbient;	// アンビエント色(+テクスチャ有無)
	XMVECTOR	vDiffuse;	// ディフューズ色
	XMVECTOR	vSpecular;	// スペキュラ色(+スペキュラ強度)
	XMVECTOR	vEmissive;	// エミッシブ色
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static HRESULT MakeVertexField(ID3D11Device* pDevice);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
ID3D11Buffer* pIndexBuffer = nullptr;

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
// 初期化処理
//=============================================================================
HRESULT Cube3D::Init(Light3D* SceneLight, const char* TexturePath)
{
	SetLight(SceneLight);
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// シェーダ初期化
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

	// 定数バッファ生成
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

	// テクスチャ サンプラ生成
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

	// 位置、向きの初期設定
	Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_rotField = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Scale = { 1,1,1 };
	// マテリアルの初期設定
	g_Kd = M_DIFFUSE;
	g_Ka = M_AMBIENT;
	g_Ks = M_SPECULAR;
	g_fPower = 0.0f;
	g_Ke = M_EMISSIVE;

	// テクスチャの読み込み
	hr = CreateTextureFromFile(pDevice,			// デバイスへのポインタ
		TexturePath,	// ファイルの名前
		&g_pTexture);	// 読み込むメモリー
	if (FAILED(hr))
		return hr;

	// 頂点情報の作成
	hr = MakeVertex(pDevice);
	return hr;
}

//=============================================================================
// 終了処理
//=============================================================================
void Cube3D::End(void)
{
	// テクスチャ解放
	SAFE_RELEASE(g_pTexture);
	// テクスチャ サンプラの開放
	SAFE_RELEASE(g_pSamplerState);
	// 頂点バッファの解放
	SAFE_RELEASE(g_pVertexBuffer);
	// 定数バッファの解放
	for (int i = 0; i < _countof(g_pConstantBuffer); ++i) {
		SAFE_RELEASE(g_pConstantBuffer[i]);
	}
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
void Cube3D::Update(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void Cube3D::Draw(void)
{
	Camera3D* pMainCamera = GetMainCamera();
	if (!pMainCamera)
	{
		printf("メインカメラがありません\n");
		return;
	}
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	//大きさ
	mtxScale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);
	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_rotField.x, g_rotField.y, g_rotField.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(Position.x, Position.y, Position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	XMStoreFloat4x4(&g_mtxWorldField, mtxWorld);

	pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
	pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
	pDeviceContext->IASetInputLayout(g_pInputLayout);

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

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
		printf("光がありません\n");

	cb2.vDiffuse = XMLoadFloat4(&g_Kd);
	cb2.vAmbient = XMVectorSet(g_Ka.x, g_Ka.y, g_Ka.z,
		(g_pTexture != nullptr) ? 1.f : 0.f);
	cb2.vSpecular = XMVectorSet(g_Ks.x, g_Ks.y, g_Ks.z, g_fPower);
	cb2.vEmissive = XMLoadFloat4(&g_Ke);
	pDeviceContext->UpdateSubresource(g_pConstantBuffer[1], 0, nullptr, &cb2, 0, 0);
	pDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer[1]);

	// プリミティブ形状をセット
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ポリゴンの描画
	//pDeviceContext->DrawIndexed(6, 0, 4);
	if(bIsPlane)
		pDeviceContext->Draw(NUM_VERTEX, 0);
	else
		pDeviceContext->Draw(NUM_VERTEX_CUBE, 0);

}

void Cube3D::SetLight(Light3D * SceneLight)
{
	pSceneLight = SceneLight;
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT Cube3D::MakeVertex(ID3D11Device* pDevice)
{
	// 頂点座標の設定
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
		g_vertexWk[0].vtx = XMFLOAT3(-1.0f, -1.0f, 1.0f);
		g_vertexWk[1].vtx = XMFLOAT3(1.0f, -1.0f, 1.0f);
		g_vertexWk[2].vtx = XMFLOAT3(-1.0f, 1.0f, 1.0f);
		g_vertexWk[3].vtx = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_vertexWk[4].vtx = XMFLOAT3(1.0f, -1.0f, 1.0f);
		g_vertexWk[5].vtx = XMFLOAT3(1.0f, -1.0f, -1.0f);
		g_vertexWk[6].vtx = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_vertexWk[7].vtx = XMFLOAT3(1.0f, 1.0f, -1.0f);
		g_vertexWk[8].vtx = XMFLOAT3(1.0f, -1.0f, -1.0f);
		g_vertexWk[9].vtx = XMFLOAT3(-1.0f, -1.0f, -1.0f);
		g_vertexWk[10].vtx = XMFLOAT3(1.0f, 1.0f, -1.0f);
		g_vertexWk[11].vtx = XMFLOAT3(-1.0f, 1.0f, -1.0f);
		g_vertexWk[12].vtx = XMFLOAT3(-1.0f, -1.0f, -1.0f);
		g_vertexWk[13].vtx = XMFLOAT3(-1.0f, -1.0f, 1.0f);
		g_vertexWk[14].vtx = XMFLOAT3(-1.0f, 1.0f, -1.0f);
		g_vertexWk[15].vtx = XMFLOAT3(-1.0f, 1.0f, 1.0f);
		g_vertexWk[16].vtx = XMFLOAT3(-1.0f, -1.0f, -1.0f);
		g_vertexWk[17].vtx = XMFLOAT3(1.0f, -1.0f, -1.0f);
		g_vertexWk[18].vtx = XMFLOAT3(-1.0f, -1.0f, 1.0f);
		g_vertexWk[19].vtx = XMFLOAT3(1.0f, -1.0f, 1.0f);
		g_vertexWk[20].vtx = XMFLOAT3(-1.0f, 1.0f, 1.0f);
		g_vertexWk[21].vtx = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_vertexWk[22].vtx = XMFLOAT3(-1.0f, 1.0f, -1.0f);
		g_vertexWk[23].vtx = XMFLOAT3(1.0f, 1.0f, -1.0f);

		for (int i = 0; i < NUM_VERTEX_CUBE; i++)
		{
			switch (i)
			{
			case 0:case 1:case 2:case 3://front
				g_vertexWk[i].nor = XMFLOAT3(0.0f, 0.0f, 1.0f);
				break;
			case 4:case 5:case 6:case 7://Right
				g_vertexWk[i].nor = XMFLOAT3(1.0f, 0.0f, 0.0f);
				break;
			case 8:case 9:case 10:case 11://Back
				g_vertexWk[i].nor = XMFLOAT3(0.0f, 0.0f, -1.0f);
				break;
			case 12:case 13:case 14:case 15://left
				g_vertexWk[i].nor = XMFLOAT3(-1.0f, 0.0f, 0.0f);
				break;
			case 16:case 17:case 18:case 19://bottom
				g_vertexWk[i].nor = XMFLOAT3(0.0f, -1.0f, 0.0f);
				break;
			case 20:case 21:case 22:case 23://top
				g_vertexWk[i].nor = XMFLOAT3(0.0f, 1.0f, 0.0f);
				break;
			default:
				break;
			}
		}
	}
	// 拡散反射光の設定
	g_vertexWk[0].diffuse = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	g_vertexWk[1].diffuse = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	g_vertexWk[2].diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	g_vertexWk[3].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	
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
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
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
	GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	return hr;
}

void Cube3D::SetPosition(XMFLOAT3 newPos)
{
	Position = newPos;
}

void Cube3D::SetRotation(XMFLOAT3 newRot)
{
	g_rotField = newRot;
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
