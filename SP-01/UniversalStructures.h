#pragma once
#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <d3d11.h>
#include <DirectXMath.h>



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x){(x)->Release();x=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if(x){delete(x);x=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) {if(x){delete[](x);x=nullptr;}}
#endif

#define SCREEN_WIDTH	(1280)				// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(720)				// �E�C���h�E�̍���
#define SCREEN_CENTER_X	(SCREEN_WIDTH/2)	// �E�C���h�E�̒��S�w���W
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT/2)	// �E�C���h�E�̒��S�x���W
#define NUM_VERTEX 4
#define CULLMODE_NONE	0					// �J�����O���Ȃ�
#define CULLMODE_CW		1					// �O�ʃJ�����O
#define CULLMODE_CCW	2					// �w�ʃJ�����O
using namespace DirectX;
//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// ���_�t�H�[�}�b�g( ���_���W[2D] / ���ˌ� / �e�N�X�`�����W )
typedef struct {
	XMFLOAT3 vtx;		// ���_���W
	XMFLOAT4 diffuse;	// �g�U���ˌ�
	XMFLOAT2 tex;		// �e�N�X�`�����W
} VERTEX_2D;

typedef struct UV 
{
	float U, V;
};

typedef struct Box
{
	float PositionX=0, PositionY=0, PositionZ=0;
	float SizeX=0, SizeY=0, SizeZ=0;
};

bool IsInCollision3D(Box a, Box b);
// ���_�t�H�[�}�b�g( ���_���W[3D] / �@���x�N�g�� / ���ˌ� / �e�N�X�`�����W )
typedef struct {
	XMFLOAT3 vtx;		// ���_���W
	XMFLOAT3 nor;		// �@���x�N�g��
	XMFLOAT4 diffuse;	// �g�U���ˌ�
	XMFLOAT2 tex;		// �e�N�X�`�����W
} VERTEX_3D;
