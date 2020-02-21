//*****************************************************************************
// debugproc.cpp
// デバッグ表示の管理
// Author : Mane
//*****************************************************************************
#include <stdarg.h>
#include <string.h>
#include "debugproc.h"
#include "Polygon2D.h"
#include "Texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_FILENAME	L"data/TEXTURE/PressStart2Ph.tga"	// 読み込むテクスチャファイル名
#define FONT_WIDTH			16
#define FONT_HEIGHT			16

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static char							g_szDebug[8192] = { '\0' };	// デバッグ情報
static bool							g_bHiragana = false;		// 平仮名フラグ
Polygon2D* DebugProcPolygon = nullptr;

//*****************************************************************************
//InitDebugProc関数
//デバッグプロックの初期化処理関数
//引数：void
//戻：HRESULT
//*****************************************************************************
HRESULT InitDebugProc(void)
{
	ID3D11Device* pDevice = GetDevice();
	HRESULT hr = S_OK;

	if (!DebugProcPolygon)
		DebugProcPolygon = new Polygon2D("data/TEXTURE/PressStart2Ph.tga");
	return hr;
}

//*****************************************************************************
//UninitDebugProc関数
//デバッグプロックの処理の終了処理関数
//引数：void
//戻：void
//*****************************************************************************
void UninitDebugProc(void)
{
	// テクスチャ解法
	DebugProcPolygon->UninitPolygon();
	delete(DebugProcPolygon);
}

//*****************************************************************************
//UpdateDebugProc関数
//デバッグプロックの処理の更新処理関数
//引数：void
//戻：void
//*****************************************************************************
void UpdateDebugProc(void)
{
	// (何もしない)
}

//*****************************************************************************
//DrawDebugProc関数
//デバッグプロック処理の描画処理関数
//引数：void
//戻：void
//*****************************************************************************
void DrawDebugProc(void)
{
	DebugProcPolygon->SetColor(0.75f, 0.5f, 1.0f);
	ID3D11DeviceContext* pDeviceContext = GetDeviceContext();
	XMFLOAT2 vPos(SCREEN_WIDTH * -0.5f + FONT_WIDTH * 0.5f,
		SCREEN_HEIGHT * 0.5f - FONT_HEIGHT * 0.5f);
	
	DebugProcPolygon->SetPolygonFrameSize(8.0f / 128.0f, 8.0f / 128.0f);
	DebugProcPolygon->SetPolygonSize(FONT_WIDTH, FONT_HEIGHT);
	for (char* pChr = &g_szDebug[0]; *pChr; ++pChr) {
		if (*pChr == '\n') {
			vPos.x = SCREEN_WIDTH * -0.5f + FONT_WIDTH * 0.5f;
			vPos.y -= FONT_HEIGHT;
			continue;
		}
		DebugProcPolygon->SetPolygonPos(vPos.x, vPos.y);
		int nChr = (BYTE)*pChr;
		if (g_bHiragana &&
			(nChr >= 0xa6 && nChr <= 0xaf || nChr >= 0xb1 && nChr <= 0xdd))
			nChr ^= 0x20;
		DebugProcPolygon->SetPolygonUV((nChr & 15) / 16.0f, (nChr >> 4) / 16.0f);
		DebugProcPolygon->UpdatePolygon();
		DebugProcPolygon->DrawPolygon(pDeviceContext);
		vPos.x += FONT_WIDTH;
	}
	// テクスチャ設定を元に戻す
	DebugProcPolygon->SetColor(1.0f, 1.0f, 1.0f);
	DebugProcPolygon->SetPolygonAlpha(1.0f);
	DebugProcPolygon->SetPolygonUV(0.0f, 0.0f);
	DebugProcPolygon->SetPolygonFrameSize(1.0f, 1.0f);
}

//*****************************************************************************
//StartDebugProc関数
//デバッグプロックの登録開始関数
//引数：bool
//戻：void
//*****************************************************************************
void StartDebugProc(bool hiragana)
{
	SetHiragana(hiragana);
	g_szDebug[0] = '\0';
}

//*****************************************************************************
//PrintDebugProc関数
//デバッグプロックの登録関数
//引数：const char *fmt, ...
//戻：void
//*****************************************************************************
void PrintDebugProc(const char *fmt, ...)
{
	va_list list;
	char aBuf[256];

	va_start(list, fmt);
	_vsprintf_p(aBuf, sizeof(aBuf), fmt, list);
	va_end(list);

	// 連結
	if ((strlen(g_szDebug) + strlen(aBuf)) < sizeof(g_szDebug) - 1) {
		strcat_s(g_szDebug, sizeof(g_szDebug), aBuf);
	}
}

//*****************************************************************************
//SetHiragana関数
//ひらがなにする関数
//引数：bool
//戻：bool
//*****************************************************************************
bool SetHiragana(bool hiragana)
{
	bool prev = g_bHiragana;
	g_bHiragana = hiragana;
	return prev;
}
