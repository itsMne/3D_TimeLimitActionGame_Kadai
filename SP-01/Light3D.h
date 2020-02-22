#pragma once
#include "main.h"
#include "FbxModel.h"

//*****************************************************************************
// ƒNƒ‰ƒX
//*****************************************************************************
class Light3D
{
private:
	CFbxLight g_UnivLight;
public:
	Light3D();
	Light3D(bool bMainLight);
	~Light3D();
	HRESULT Init(void);
	void End(void);
	void Update(void);
	CFbxLight* GetLight(void);
	void SetDiffuse(XMFLOAT4 newDif);
	void SetSpecular(XMFLOAT4 newSpec);
	void SetAmbient(XMFLOAT4 newAmb);
	void SetDirection(XMFLOAT3 newDir);
	void SetLightEnable(bool bEnable);
};

Light3D* GetMainLight();