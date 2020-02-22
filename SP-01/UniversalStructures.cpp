//*****************************************************************************
// UniversalStructures.cpp
// ゲームの構造体と変数を管理する
// Author : Mane
//*****************************************************************************
#include "UniversalStructures.h"

//*****************************************************************************
//IsInCollision3D関数
//当たり判定を確認する
//引数：Box, Box
//戻：bool
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
//CompareXmfloat3関数
//ベクトルの中身は同じを確認する
//引数：XMFLOAT3, XMFLOAT3
//戻：bool
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
//GetAngle関数
//角度を戻す
//引数：XMFLOAT3, XMFLOAT3
//戻：float
//*****************************************************************************
float GetAngle(XMFLOAT3 a, XMFLOAT3 b)
{
	return (float)atan2(a.y, a.x) - atan2(b.y, b.x);
}

//*****************************************************************************
//DotProduct関数
//・プロダクトを計算する
//引数：XMFLOAT3, XMFLOAT3
//戻：float
//*****************************************************************************
float DotProduct(XMFLOAT3 a, XMFLOAT3 b)
{
	return (a.x*b.x + a.y*b.y + a.z*b.z);
}

//*****************************************************************************
//MultiplyVector関数
//ベクトルを掛け算する
//引数：XMFLOAT3, XMFLOAT3
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 MultiplyVector(XMFLOAT3 a, XMFLOAT3 b)
{
	return { a.x*b.x, a.y*b.y, a.z*b.z };
}

//*****************************************************************************
//MultiplyVector関数
//ベクトルを総計する
//引数：XMFLOAT3, XMFLOAT3
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 SumVector(XMFLOAT3 a, XMFLOAT3 b)
{
	return { a.x+b.x, a.y+b.y, a.z+b.z };
}

//*****************************************************************************
//GetForwardVector関数
//フォーワードベクトルを戻す
//引数：XMFLOAT3
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 GetForwardVector(XMFLOAT3 Rot)
{
	return {(float)(cos(Rot.x) * cos(Rot.y)), (float)(cos(Rot.x) * sin(Rot.y)), (float)sin(Rot.x)};
}

//*****************************************************************************
//GetVectorDifference関数
//ベクトルを減算する
//引数：XMFLOAT3, XMFLOAT3
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 GetVectorDifference(XMFLOAT3 a, XMFLOAT3 b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

//*****************************************************************************
//NormalizeVector関数
//ノーマルベクトルを戻す
//引数：XMFLOAT3
//戻：XMFLOAT3
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
//MultiplyVectorByFloat関数
//数で掛け算する
//引数：XMFLOAT3, float
//戻：XMFLOAT3
//*****************************************************************************
XMFLOAT3 MultiplyVectorByFloat(XMFLOAT3 a, float flot)
{
	return { a.x*flot, a.y*flot, a.z*flot };
}
