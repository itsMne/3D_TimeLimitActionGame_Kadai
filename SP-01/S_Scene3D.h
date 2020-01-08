#pragma once
#include "DXWindow3D.h"
#include "Camera3D.h"
#include "Light3D.h"
enum eSceneType
{
	SCENE_TITLE_SCREEN = 0,
	SCENE_MENU,
	SCENE_IN_GAME,
	MAX_SCENES
};
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
	virtual eSceneType Update();
	virtual void Draw() = 0;
	virtual void End();
};

