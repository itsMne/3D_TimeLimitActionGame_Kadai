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
