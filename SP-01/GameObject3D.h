#pragma once
#include "UniversalStructures.h"
#include "Model3D.h"
#include "Light3D.h"

enum GameObjectTypes
{
	GO_PLAYER=0,
	GO_BULLET,
	GO_MAX
};
class GameObject3D
{
protected:
	Model3D* Model;
	XMFLOAT3 Rotation;
	XMFLOAT3 Position;
	XMFLOAT3 Scale;
	Camera3D* pMainCamera;
	XMFLOAT4X4 WorldMatrix;
	int nType;
	bool bUse;
	//’e‚ÉŠÖ‚·‚é
	int nUseCounterFrame;
public:
	GameObject3D();
	GameObject3D(const char* ModelPath, int nType);
	GameObject3D(Light3D* light, const char* ModelPath, int nType);
	~GameObject3D();
	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void End();
	XMFLOAT3 GetLocation();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();
	Model3D* GetModel();
	void SetScale(float fScale);
	void SetRotation(XMFLOAT3 fRot);
	XMFLOAT4X4* GetModelWorld();
	void SetPosition(XMFLOAT3 newPos);
	void SetModelRotation(XMFLOAT3 rot);
	void RotateAroundY(float y);
	void SetUse(bool newUse);
	bool IsInUse();
	int GetType();
};

