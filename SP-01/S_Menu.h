#pragma once
#include "S_Scene3D.h"
class S_Menu :
	public Scene3D
{
public:
	S_Menu();
	~S_Menu();
	void Init();
	eSceneType Update();
	void Draw();
	void End();
};

