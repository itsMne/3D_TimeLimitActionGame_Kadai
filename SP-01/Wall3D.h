#pragma once
#include "GameObject3D.h"
class Wall3D :
	public GameObject3D
{
private:
	char szTexturePath[256];
public:
	Wall3D();
	~Wall3D();
	void Init();
	void Update();
	void Draw();
	void End();
	char * GetTexturePath();
};

