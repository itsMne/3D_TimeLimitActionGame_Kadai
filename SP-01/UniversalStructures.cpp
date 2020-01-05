#include "UniversalStructures.h"



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

float GetAngle(XMFLOAT3 a, XMFLOAT3 b)
{
	return (float)atan2(a.y, a.x) - atan2(b.y, b.x);
}

XMFLOAT3 GetForwardVector(XMFLOAT3 Rot)
{
	return {(float)(cos(Rot.x) * cos(Rot.y)), (float)(cos(Rot.x) * sin(Rot.y)), (float)sin(Rot.x)};
}

XMFLOAT3 GetVectorDifference(XMFLOAT3 a, XMFLOAT3 b)
{
	return { a.x - b.x, a.y - b.y, a.z - b.z };
}

XMFLOAT3 NormalizeVector(XMFLOAT3 v)
{
	XMFLOAT3 p = v;
	float w = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	p.x /= w;
	p.y /= w;
	p.z /= w;
	return p;
}