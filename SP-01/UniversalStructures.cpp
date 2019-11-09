#include "UniversalStructures.h"

XMFLOAT3 GetForwardVector(XMFLOAT3 Rotation)
{
	
	float pitch = Rotation.x * (XM_PI/180);
	float yaw = Rotation.y* (XM_PI / 180);
	float offset = 1;
	printf("PITCH: %f\n YAW: %f\n FORWARD: {%f, %f, %f}\n", pitch, yaw, offset*sinf(yaw)*cosf(pitch), offset*(-sinf(pitch)), cosf(yaw)*cosf(pitch));
	return { offset*sinf(yaw)*cosf(pitch),offset*(-sinf(pitch)),cosf(yaw)*cosf(pitch) };
}
