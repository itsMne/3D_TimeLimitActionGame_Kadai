#pragma once
#include "GameObject3D.h"
enum DebugAimType
{
	DA_DEBUG_AIM=0,
	DA_MAX
};
class DebugAim :
	public GameObject3D
{
private:
	int nType;
public:
	DebugAim();
	~DebugAim();
	void Init();
	void Update();
	void DebugAimControl();
	void Draw();
	void End();
};

