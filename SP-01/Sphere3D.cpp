#include "Sphere3D.h"
#include "main.h"
#include "Texture.h"
#include "Camera3D.h"
#define USE_16_SLASHES true
#define SKYBOX_SIZE 800
//*****************************************************************************
// マクロ定義
//*****************************************************************************


Sphere3D::Sphere3D(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath)
{
	Init(pos, rot, nSlice, nStack, fRadius, szPath);
	fRotSpeed = { 0,0,0 };
}

Sphere3D::Sphere3D(const char * szPath)
{
#if USE_16_SLASHES
	Init({ 0,50,100 }, { 0,0,3.18f }, 16, 8, SKYBOX_SIZE, szPath);
#else
	Init({ 0,50,100 }, { 0,0,3.18f }, 32, 16, SKYBOX_SIZE, szPath);
#endif
	bisUnlit = true;
	bNoCull = true;
	fRotSpeed = { 0,0.001f,0 };
}


Sphere3D::~Sphere3D()
{
}

HRESULT Sphere3D::Init(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath)
{
	Mesh3D::Init();
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr;

	// テクスチャ マトリックス初期化
	XMStoreFloat4x4(&pMesh->mtxTexture, XMMatrixIdentity());

	// 位置、向きの初期設定
	pMesh->pos = pos;
	pMesh->rot = rot;

	// テクスチャの読み込み
	hr = CreateTextureFromFile(pDevice,					// デバイスへのポインタ
		szPath,		// ファイルの名前
		&pMesh->pTexture);	// 読み込むメモリー
	if (FAILED(hr))
		return hr;

	// 頂点数の設定
	pMesh->nNumVertex = (nSlice + 1) * (nSlice + 1);

	// インデックス数の設定
	pMesh->nNumIndex = (nSlice + 1) * 2 * nStack + (nStack - 1) * 2;


	// オブジェクトの頂点バッファを生成
	VERTEX_3D* pVertexWk = new VERTEX_3D[pMesh->nNumVertex];

	// オブジェクトのインデックスバッファを生成
	int* pIndexWk = new int[pMesh->nNumIndex];

	// 頂点バッファの中身を埋める
	VERTEX_3D* pVtx = pVertexWk;
	const float texSizeX = 1.0f / nSlice;
	const float texSizeZ = 1.0f / nStack;


	for (int nCntVtxY = 0; nCntVtxY < nStack + 1; ++nCntVtxY) {

		for (int nCntVtxX = 0; nCntVtxX < nSlice + 1; ++nCntVtxX) {
			// 頂点座標の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.x = cosf(nCntVtxX * XM_PI / (nSlice / 2)) * fRadius * sinf(nCntVtxY * XM_PI / nStack);
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.y = sinf(((nStack / 2) - nCntVtxY) * XM_PI / nStack)*fRadius;
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].vtx.z = sinf(nCntVtxX * XM_PI / (nSlice / 2)) * fRadius * sinf(nCntVtxY * XM_PI / nStack);

			// 法線の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].nor = XMFLOAT3(0.0f, 1.0, 0.0f);

			// 反射光の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].tex.x = texSizeX * nCntVtxX;
			pVtx[nCntVtxY * (nSlice + 1) + nCntVtxX].tex.y = texSizeZ * nCntVtxY;
		}
	}

	// インデックスバッファの中身を埋める
	int* pIdx = pIndexWk;

	int nCntIdx = 0;
	for (int nCntVtxY = 0; nCntVtxY < nStack; ++nCntVtxY) {
		if (nCntVtxY > 0) {
			// 縮退ポリゴンのためのダブりの設定
			pIdx[nCntIdx] = (nCntVtxY + 1) * (nSlice + 1);
			++nCntIdx;
		}

		for (int nCntVtxX = 0; nCntVtxX < nSlice + 1; ++nCntVtxX) {
			pIdx[nCntIdx] = (nCntVtxY + 1) * (nSlice + 1) + nCntVtxX;
			++nCntIdx;
			pIdx[nCntIdx] = nCntVtxY * (nSlice + 1) + nCntVtxX;
			++nCntIdx;
		}

		if (nCntVtxY < nStack - 1) {
			// 縮退ポリゴンのためのダブりの設定
			pIdx[nCntIdx] = nCntVtxY * (nSlice + 1) + nSlice;
			++nCntIdx;
		}
	}

	// 頂点バッファ/インデックス バッファ生成
	hr = MakeMeshVertex(pDevice, pMesh, pVertexWk, pIndexWk);

	// 一時配列解放
	delete[] pIndexWk;
	delete[] pVertexWk;

	return hr;
}

void Sphere3D::Uninit(void)
{
	Mesh3D::Uninit();
}

void Sphere3D::Update(void)
{
	Mesh3D::Update();
	if (!pMesh)
		return;
	pMesh->rot.x += fRotSpeed.x;
	pMesh->rot.y += fRotSpeed.y;
	pMesh->rot.z += fRotSpeed.z;
	Camera3D* pCam = GetMainCamera();
	if (!pCam)
		return;
	pMesh->pos = pCam->GetCameraPos();
}

void Sphere3D::Draw(void)
{

	Mesh3D::Draw(GetDeviceContext());
}
