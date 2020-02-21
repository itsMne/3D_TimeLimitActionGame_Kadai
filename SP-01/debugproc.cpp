//*****************************************************************************
// debugproc.cpp
// �f�o�b�O�\���̊Ǘ�
// Author : Mane
//*****************************************************************************
#include <stdarg.h>
#include <string.h>
#include "debugproc.h"
#include "Polygon2D.h"
#include "Texture.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TEXTURE_FILENAME	L"data/TEXTURE/PressStart2Ph.tga"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define FONT_WIDTH			16
#define FONT_HEIGHT			16

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static char							g_szDebug[8192] = { '\0' };	// �f�o�b�O���
static bool							g_bHiragana = false;		// �������t���O
Polygon2D* DebugProcPolygon = nullptr;

//*****************************************************************************
//InitDebugProc�֐�
//�f�o�b�O�v���b�N�̏����������֐�
//�����Fvoid
//�߁FHRESULT
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
//UninitDebugProc�֐�
//�f�o�b�O�v���b�N�̏����̏I�������֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void UninitDebugProc(void)
{
	// �e�N�X�`����@
	DebugProcPolygon->UninitPolygon();
	delete(DebugProcPolygon);
}

//*****************************************************************************
//UpdateDebugProc�֐�
//�f�o�b�O�v���b�N�̏����̍X�V�����֐�
//�����Fvoid
//�߁Fvoid
//*****************************************************************************
void UpdateDebugProc(void)
{
	// (�������Ȃ�)
}

//*****************************************************************************
//DrawDebugProc�֐�
//�f�o�b�O�v���b�N�����̕`�揈���֐�
//�����Fvoid
//�߁Fvoid
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
	// �e�N�X�`���ݒ�����ɖ߂�
	DebugProcPolygon->SetColor(1.0f, 1.0f, 1.0f);
	DebugProcPolygon->SetPolygonAlpha(1.0f);
	DebugProcPolygon->SetPolygonUV(0.0f, 0.0f);
	DebugProcPolygon->SetPolygonFrameSize(1.0f, 1.0f);
}

//*****************************************************************************
//StartDebugProc�֐�
//�f�o�b�O�v���b�N�̓o�^�J�n�֐�
//�����Fbool
//�߁Fvoid
//*****************************************************************************
void StartDebugProc(bool hiragana)
{
	SetHiragana(hiragana);
	g_szDebug[0] = '\0';
}

//*****************************************************************************
//PrintDebugProc�֐�
//�f�o�b�O�v���b�N�̓o�^�֐�
//�����Fconst char *fmt, ...
//�߁Fvoid
//*****************************************************************************
void PrintDebugProc(const char *fmt, ...)
{
	va_list list;
	char aBuf[256];

	va_start(list, fmt);
	_vsprintf_p(aBuf, sizeof(aBuf), fmt, list);
	va_end(list);

	// �A��
	if ((strlen(g_szDebug) + strlen(aBuf)) < sizeof(g_szDebug) - 1) {
		strcat_s(g_szDebug, sizeof(g_szDebug), aBuf);
	}
}

//*****************************************************************************
//SetHiragana�֐�
//�Ђ炪�Ȃɂ���֐�
//�����Fbool
//�߁Fbool
//*****************************************************************************
bool SetHiragana(bool hiragana)
{
	bool prev = g_bHiragana;
	g_bHiragana = hiragana;
	return prev;
}
