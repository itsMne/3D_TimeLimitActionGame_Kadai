#include "GameObject3D.h"
#define BULLET_SPEED 30
#define PRINT_HITBOX false
int nInitedGameObjects = 0;

GameObject3D::GameObject3D()
{
	nType = GO_MAX;
	Model = nullptr;
}

GameObject3D::GameObject3D(int Type)
{
	
	Init();
	switch (Type)
	{
	case GO_SHADOW:
		Model = new Model3D(this, "data/model/Shadow.fbx");
		break;
	case GO_TITLE_LOGO:
		Model = new Model3D(this, "data/model/LogoV3.fbx");
		break;
	case GO_BULLET:
		Model = new Model3D(this, "data/model/Bullet.fbx");
		/*pEffect2D = new Billboard2D("data/texture/explosion000.png");
		pEffect2D->SetColor({ 1, 1, 1, 1 });
		pEffect2D->SetUVFrames(8, 1);*/
		nCurrentRasterizer = 1;
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
	Model = new Model3D(this, ModelPath);
	nInitedGameObjects++;
	nType = Type;
}

GameObject3D::GameObject3D(Light3D* light, const char * ModelPath, int Type)
{
	Init();
	Model = new Model3D(this, ModelPath);
	Model->SetLight(light);
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
	Model = nullptr;
	pEffect2D = nullptr;
	pVisualHitbox = nullptr;
	Hitbox = { 0,0,0,0,0,0 };
#if PRINT_HITBOX
	pVisualHitbox = new Cube3D();
	pVisualHitbox->Init("data/texture/hbox.tga");
	pVisualHitbox->SetScale({ Hitbox.SizeX, Hitbox.SizeY, Hitbox.SizeZ });
	pVisualHitbox->SetPosition({ Hitbox.PositionX,Hitbox.PositionY,Hitbox.PositionZ });
#endif
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
		Position.y += sinf(XM_PI + Rotation.x) * BULLET_SPEED;
		nUseCounterFrame++;
		if (nUseCounterFrame > 200)
		{
			nUseCounterFrame = 0;
			bUse = false;
		}

		Model->SetRotation(Rotation);
		break;
	case GO_SHADOW:
		if (!p_goParent)
			return;
		XMFLOAT3 ParentPosition = p_goParent->GetPosition();
		Position.x = ParentPosition.x;
		Position.z = ParentPosition.z;
		break;
	case GO_TITLE_LOGO:
		if (Model->GetCurrentAnimation() == 0 && Model->GetCurrentFrameOfAnimation() == 148)
			Model->SwitchAnimation(1, 0, 1);
		break;
	default:
		break;
	}
	if(pEffect2D)
		pEffect2D->Update();
}

void GameObject3D::Draw()
{
	if (!bUse)
		return;
	switch (nType)
	{
	case GO_BULLET:
		if (Model)
			Model->DrawModel();
		Draw2DEffect();
		break;
	default:
		if (Model)
			Model->DrawModel();
		break;
	}
	

#if PRINT_HITBOX
	if (pVisualHitbox) {
		Box ThisHitbox = GetHitbox();
		pVisualHitbox->SetScale({ ThisHitbox.SizeX, ThisHitbox.SizeY, ThisHitbox.SizeZ });
		pVisualHitbox->SetPosition({ ThisHitbox.PositionX,ThisHitbox.PositionY,ThisHitbox.PositionZ });
		pVisualHitbox->Draw();
	}
#endif
}

void GameObject3D::Draw2DEffect()
{
	SetCullMode(CULLMODE_NONE);
	if (pEffect2D)
		pEffect2D->Draw();
	SetCullMode(CULLMODE_CCW);
}

void GameObject3D::End()
{
	SAFE_DELETE(Model);
	SAFE_DELETE(pEffect2D);
#if PRINT_HITBOX
	SAFE_DELETE(pVisualHitbox);
#endif
}

XMFLOAT3 GameObject3D::GetPosition()
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
	if(Model)
		Model->SetRotation(rot);
}

void GameObject3D::RotateAroundX(float x)
{
	Rotation.x -= x;
	if (Rotation.x < -XM_PI) {
		Rotation.x += XM_2PI;
	}
}

void GameObject3D::RotateAroundY(float y)
{
	Rotation.y -= y;
	if (Rotation.y < -XM_PI) {
		Rotation.y += XM_2PI;
	}
}

void GameObject3D::RotateAroundZ(float z)
{
	Rotation.z -= z;
	if (Rotation.z < -XM_PI) {
		Rotation.z += XM_2PI;
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

void GameObject3D::TranslateOnY(float translation)
{
	Position.y += translation;
}

bool GameObject3D::IsInUse()
{
	return bUse;
}

int GameObject3D::GetType()
{
	return nType;
}

Box GameObject3D::GetHitbox()
{
	return { Hitbox.PositionX + Position.x,Hitbox.PositionY + Position.y,Hitbox.PositionZ + Position.z, Hitbox.SizeX, Hitbox.SizeY, Hitbox.SizeZ };
}

void GameObject3D::SetHitbox(Box newHitbox)
{
	Hitbox = newHitbox;
}
