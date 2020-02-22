//*****************************************************************************
// UniversalStructures.cpp
// �Q�[���̍\���̂ƕϐ����Ǘ�����
// Author : Mane
//*****************************************************************************
#include "UniversalStructures.h"

//*****************************************************************************
//IsInCollision3D�֐�
//�����蔻����m�F����
//�����FBox, Box
//�߁Fbool
//*****************************************************************************
bool IsInCollision3D(Box a, Box b)
{
	//check the X axis
	float comX = a.PositionX - b.PositionX;
	if (comX < 0)
		comX *= -1;
	float comY = a.PositionY - b.PositionY;
	if (comY < 0)
		comY *= -1;
	float comZ = a.PositionZ - b.PositionZ;
	if (comZ < 0)
		comZ *= -1;
	if (comX < a.SizeX + b.SizeX)
	{
		//check the Y axis
		if (comY < a.SizeY + b.SizeY)
		{
			//check the Z axis
			if (comZ < a.SizeZ + b.SizeZ)
			{
				return true;
			}
		}
	}
	return false;
}

//*****************************************************************************
//CompareXmfloat3�֐�
//�x�N�g���̒��g�͓������m�F����
//�����FXMFLOAT3, XMFLOAT3
//�߁Fbool
//*****************************************************************************
bool CompareXmfloat3(XMFLOAT3 a, XMFLOAT3 b)
{
	if (a.x != b.x)
		return false;
	if (a.y != b.y)
		return false;
	if (a.z != b.z)
		return false;
	return true;
}

//*****************************************************************************
//GetAngle�֐�
//�p�x��߂�
//�����FXMFLOAT3, XMFLOAT3
//�߁Ffloat
//*****************************************************************************
float GetAngle(XMFLOAT3 a, XMFLOAT3 b)
{
	return (float)atan2(a.y, a.x) - atan2(b.y, b.x);
}

//*****************************************************************************
//DotProduct�֐�
//�E�v���_�N�g���v�Z����
//�����FXMFLOAT3, XMFLOAT3
//�߁Ffloat
//*****************************************************************************
float DotProduct(XMFLOAT3 a, XMFLOAT3 b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

//*****************************************************************************
//MultiplyVector�֐�
//�x�N�g�����|���Z����
//�����FXMFLOAT3, XMFLOAT3
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 MultiplyVector(XMFLOAT3 a, XMFLOAT3 b)
{
	return { a.x*b.x, a.y*b.y, a.z*b.z };
}

//*****************************************************************************
//MultiplyVector�֐�
//�x�N�g���𑍌v����
//�����FXMFLOAT3, XMFLOAT3
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 SumVector(XMFLOAT3 a, XMFLOAT3 b)
{
	return { a.x+b.x, a.y+b.y, a.z+b.z };
}

//*****************************************************************************
//GetForwardVector�֐�
//�t�H�[���[�h�x�N�g����߂�
//�����FXMFLOAT3
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 GetForwardVector(XMFLOAT3 Rot)
{
	return {(float)(cos(Rot.x) * cos(Rot.y)), (float)(cos(Rot.x) * sin(Rot.y)), (float)sin(Rot.x)};
}

//*****************************************************************************
//GetVectorDifference�֐�
//�x�N�g�������Z����
//�����FXMFLOAT3, XMFLOAT3
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 GetVectorDifference(XMFLOAT3 a, XMFLOAT3 b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

//*****************************************************************************
//NormalizeVector�֐�
//�m�[�}���x�N�g����߂�
//�����FXMFLOAT3
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 NormalizeVector(XMFLOAT3 v)
{
	XMFLOAT3 p = v;
	float w = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	p.x /= w;
	p.y /= w;
	p.z /= w;
	return p;
}

//*****************************************************************************
//MultiplyVectorByFloat�֐�
//���Ŋ|���Z����
//�����FXMFLOAT3, float
//�߁FXMFLOAT3
//*****************************************************************************
XMFLOAT3 MultiplyVectorByFloat(XMFLOAT3 a, float flot)
{
	return { a.x*flot, a.y*flot, a.z*flot };
}
