//*****************************************************************************
// S_Scene3D.h
//*****************************************************************************
#pragma once
#include "DXWindow3D.h"
#include "Camera3D.h"
#include "Light3D.h"

//*****************************************************************************
// エナム
//*****************************************************************************
enum eSceneType
{
	SCENE_TITLE_SCREEN = 0,
	SCENE_MENU,
	SCENE_IN_GAME,
	SCENE_RANKING,
	MAX_SCENES
};

//*****************************************************************************
// クラス
//*****************************************************************************
class Scene3D
{
protected:
	Camera3D* pSceneCamera;
	DXWindow3D* MainWindow = nullptr;
	Light3D* pSceneLight = nullptr;
	ID3D11DeviceContext* pDeviceContext;
public:
	Scene3D(bool);
	virtual ~Scene3D();
	virtual void Init() = 0;
	virtual eSceneType Update();
	virtual void Draw() = 0;
	virtual void End();
};

