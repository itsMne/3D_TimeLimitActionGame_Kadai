//*****************************************************************************
// Sphere3D.h
//*****************************************************************************
#pragma once
#include "Mesh3D.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class Sphere3D :
	public Mesh3D
{
private:
	XMFLOAT3 fRotSpeed;
	bool bIsSkybox;
public:
	Sphere3D(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath);
	Sphere3D(const char* szPath);
	~Sphere3D();

	HRESULT Init(XMFLOAT3 pos, XMFLOAT3 rot, int nSlice, int nStack, float fRadius, const char* szPath);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetUnlit(bool isUnlit);
};

