#pragma once
#include "main.h"
#include "FbxModel.h"
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
	CFbxLight& GetLight(void);
};

Light3D* GetMainLight();