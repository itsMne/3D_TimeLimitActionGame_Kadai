#pragma once
#include "UniversalStructures.h"
#include "Model3D.h"
#include "Light3D.h"
#include "Cube3D.h"
#include "Billboard2D.h"

enum eGameObjectTypes
{
	GO_PLAYER=0,
	GO_BULLET,
	GO_SHADOW,
	GO_TITLE_LOGO,
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
	//‰e‚ÉŠÖ‚·‚é
	GameObject3D* p_goParent;
	int nUseCounterFrame;
	Box Hitbox;
	Cube3D* pVisualHitbox;
	Billboard2D* pEffect2D;
	int nCurrentRasterizer;
	Light3D* pLight;
public:
	GameObject3D();
	GameObject3D(int nType);
	GameObject3D(const char* ModelPath, int nType);
	GameObject3D(Light3D* light, const char* ModelPath, int nType);
	~GameObject3D();
	virtual void Init();
	virtual void Update();
	virtual void Draw();
	virtual void Draw2DEffect();
	virtual void End();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();
	Model3D* GetModel();
	void SetScale(float fScale);
	void SetRotation(XMFLOAT3 fRot);
	XMFLOAT4X4* GetModelWorld();
	void SetPosition(XMFLOAT3 newPos);
	void SetModelRotation(XMFLOAT3 rot);
	void RotateAroundX(float x);
	void RotateAroundY(float y);
	void RotateAroundZ(float z);
	void SetUse(bool newUse);
	void SetParent(GameObject3D* pNewParent);
	void TranslateOnY(float translation);
	bool IsInUse();
	int GetType();
	Box GetHitbox();
	void SetHitbox(Box);
};

