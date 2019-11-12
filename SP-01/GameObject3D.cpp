#include "GameObject3D.h"
#define BULLET_SPEED 5

int nInitedGameObjects = 0;

GameObject3D::GameObject3D()
{
	nType = GO_MAX;
}

GameObject3D::GameObject3D(int Type)
{
	
	Init();
	switch (Type)
	{
	case GO_SHADOW:
		Model = new Model3D();
		Model->InitModel(GetMainLight(), "data/model/Shadow.fbx");
		Model->SetParent(this);
		break;
	default:
		break;
	}

	nInitedGameObjects++;
	nType = Type;
}

GameObject3D::GameObject3D(const char * ModelPath, int Type)
{
	Init();
	Model = new Model3D();
	Model->InitModel(GetMainLight(), ModelPath);
	Model->SetParent(this);
	nInitedGameObjects++;
	nType = Type;
}

GameObject3D::GameObject3D(Light3D* light, const char * ModelPath, int Type)
{
	Init();
	Model = new Model3D();
	Model->InitModel(light, ModelPath);
	Model->SetParent(this);
	nInitedGameObjects++;
	nType = Type;
}


GameObject3D::~GameObject3D()
{
	nInitedGameObjects--;
	End();
}

void GameObject3D::Init()
{
	Rotation = { 0,0,0 };
	Position = { 0,0,0 };
	Scale = { 1,1,1 };
	pMainCamera = GetMainCamera();
	bUse = true;
	nUseCounterFrame = 0;
	p_goParent = nullptr;
}

void GameObject3D::Update()
{
	if (!bUse)
		return;
	if (Model)
		Model->UpdateModel();
	switch (nType)
	{
	case GO_BULLET:
		Position.x -= sinf(XM_PI + Rotation.y) * BULLET_SPEED;
		Position.z -= cosf(XM_PI + Rotation.y) * BULLET_SPEED;
		nUseCounterFrame++;
		if (nUseCounterFrame > 200)
		{
			nUseCounterFrame = 0;
			bUse = false;
		}
		break;
	case GO_SHADOW:
		if (!p_goParent)
			return;
		XMFLOAT3 ParentPosition = p_goParent->GetLocation();
		Position.x = ParentPosition.x;
		Position.z = ParentPosition.z;
		break;
	default:
		break;
	}
}

void GameObject3D::Draw()
{
	if (!bUse)
		return;
	if (Model) 
		Model->DrawModel();
	/*if (nType == GO_SHADOW && Model && bUse)
		printf("hello\n");*/
}

void GameObject3D::End()
{
	if (Model)
		Model->UninitModel();
}

XMFLOAT3 GameObject3D::GetLocation()
{
	return Position;
}

XMFLOAT3 GameObject3D::GetRotation()
{
	return Rotation;
}

XMFLOAT3 GameObject3D::GetScale()
{
	return Scale;
}

Model3D * GameObject3D::GetModel()
{
	return Model;
}

void GameObject3D::SetScale(float fScale)
{
	Scale = { fScale, fScale, fScale };
}

void GameObject3D::SetRotation(XMFLOAT3 fRot)
{
	Rotation = fRot;
}

XMFLOAT4X4* GameObject3D::GetModelWorld()
{
	XMMATRIX mtxWorld, mtxRot, mtxTranslate, mtxScale;
	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	mtxScale = XMMatrixScaling(Scale.x, Scale.y, Scale.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScale);

	mtxRot = XMMatrixRotationRollPitchYaw(Rotation.x, Rotation.y, Rotation.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	mtxTranslate = XMMatrixTranslation(Position.x, Position.y, Position.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
	
	XMStoreFloat4x4(&WorldMatrix, mtxWorld);
	
	// ワールドマトリックスの設定
	return &WorldMatrix;

}

void GameObject3D::SetPosition(XMFLOAT3 newPos)
{
	Position = newPos;
}

void GameObject3D::SetModelRotation(XMFLOAT3 rot)
{
	Model->SetRotation(rot);
}

void GameObject3D::RotateAroundY(float y)
{
	Rotation.y -= y;
	if (Rotation.y < -XM_PI) {
		Rotation.y += XM_2PI;
	}
}

void GameObject3D::SetUse(bool newUse)
{
	bUse = newUse;
}

void GameObject3D::SetParent(GameObject3D * pNewParent)
{
	p_goParent = pNewParent;
}

bool GameObject3D::IsInUse()
{
	return bUse;
}

int GameObject3D::GetType()
{
	return nType;
}
