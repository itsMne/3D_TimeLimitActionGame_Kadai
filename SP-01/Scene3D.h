#pragma once
#include "DXWindow3D.h"
#include "Camera3D.h"
#include "Light3D.h"
class Scene3D
{
protected:
	Camera3D* pSceneCamera;
	DXWindow3D* MainWindow = nullptr;
	Light3D* pSceneLight = nullptr;
	ID3D11DeviceContext* pDeviceContext;
public:
	Scene3D(bool);
	~Scene3D();
	virtual void Init() = 0;
	virtual void Update();
	virtual void Draw() = 0;
	virtual void End();
};

