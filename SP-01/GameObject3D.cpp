#include "GameObject3D.h"
#include "Cube3D.h"
#include "Enemy3D.h"
#include "Field3D.h"
#include "S_InGame3D.h"
#include "Player3D.h"
#include "Sound.h"

#define BULLET_SPEED 30
#define PRINT_HITBOX false

int nInitedGameObjects = 0;

GameObject3D::GameObject3D()
{
	nType = GO_MAX;
	Model = nullptr;
	Init();
}

GameObject3D::GameObject3D(int Type)
{
	nType = Type;
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
		Hitbox = { 0,0,0,2.5f,2.5f,7 };
		nCurrentRasterizer = 1;
		break;
	case GO_SKULLWARNING:
		Model = new Model3D(this, SKULL_MODEL_PATH);
		Model->SetScale(2);
		SetScale(0.25f);
		bIsUnlit = true;
		break;
	case GO_ENEMYHEARTMARK:
		Model = new Model3D(this, HEART_ENEMY_MODEL);
		Model->SetScale(1);
		SetScale(1);
		bIsUnlit = true;
		break;
	case GO_TUTORIALSIGN:
		Model = new Model3D(this, "data/model/TutorialModels/TutSign.fbx");
		Hitbox = { 0,5,0,4.5f,12.5f,4.5f };
		Position = { -1.321372f, 122.699966f, -130.018921f };
		nCurrentRasterizer = 1;
		bIsUnlit = true;
		nSpinningFrames = 0;
		fTutorialSignSpeed = 0.05f;
		bUse = true;
		break;
	default:
		break;
	}
	nInitedGameObjects++;
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
	pLight = light;
}


GameObject3D::~GameObject3D()
{
	nInitedGameObjects--;
	End();
}

void GameObject3D::Init()
{
	Rotation = { 0,0,0 };
	InitialPosition = Position = { 0,0,0 };
	Scale = { 1,1,1 };
	pMainCamera = GetMainCamera();
	bUse = true;
	ScaleUp = true;
	bIsUnlit = false;
	nUseCounterFrame = 0;
	fAcceleration = 0;
	p_goParent = nullptr;
	Model = nullptr;
	pVisualHitbox = nullptr;
	pLight = nullptr;
	Hitbox = { 0,0,0,0,0,0 };
#if PRINT_HITBOX
	if (nType != GO_CUBE) {
		pVisualHitbox = new Cube3D();
		((Cube3D*)pVisualHitbox)->Init("data/texture/hbox.tga");
		((Cube3D*)pVisualHitbox)->SetScale({ Hitbox.SizeX, Hitbox.SizeY, Hitbox.SizeZ });
		((Cube3D*)pVisualHitbox)->SetPosition({ Hitbox.PositionX,Hitbox.PositionY,Hitbox.PositionZ });
	}
#endif
}

void GameObject3D::Update()
{
	if (!bUse)
		return;
	if (Model)
		Model->UpdateModel();
	XMFLOAT3* ModelScale;
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
	case GO_SKULLWARNING:
		Model->SetRotation(GetMainCamera()->GetCameraAngle());
		Model->RotateAroundY(XM_PI/2);
		
		if (ScaleUp)
		{
			Scale.x += 0.0125f;
			Scale.y += 0.0125f;
			Scale.z += 0.0125f;
			if (Scale.x > 0.17f)
				ScaleUp = false;
		}
		else {
			Scale.x -= 0.0125f;
			Scale.y -= 0.0125f;
			Scale.z -= 0.0125f;

			if (Scale.x < 0.07f)
				ScaleUp = true;
		}
		break;
	case GO_ENEMYHEARTMARK:
		if (!Model)
			break;
		ModelScale = Model->GetScaleAdd();
		if (ModelScale->x < 0.80f)
			fAcceleration += 0.0125f;
		else
			fAcceleration = 0;
		if (ScaleUp)
		{
			ModelScale->x += 0.0125f+fAcceleration;
			ModelScale->y += 0.0125f+fAcceleration;
			ModelScale->z += 0.0125f+fAcceleration;
			if (ModelScale->x > 1.17f)
				ScaleUp = false;
		}
		else {
			ModelScale->x -= 0.0125f+fAcceleration;
			ModelScale->y -= 0.0125f+fAcceleration;
			ModelScale->z -= 0.0125f+fAcceleration;

			if (ModelScale->x < 0.87f)
				ScaleUp = true;
		}
		break;
	case GO_TUTORIALSIGN:
		TutorialSignControl();
		break;
	default:
		break;
	}
}

void GameObject3D::TutorialSignControl()
{
	Model->RotateAroundY(fTutorialSignSpeed);
	Player3D* pPlayer = GetPlayer3D();
	if (nSpinningFrames > 0)
	{
		nSpinningFrames--;
		fTutorialSignSpeed = 0.05f*8;
	}
	else {
		fTutorialSignSpeed = 0.05f;
		if (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_ATTACK)))
		{
			GetMainCamera()->SetAttackZoom(40, 40);
			GetCurrentGame()->SetAtkEffect(30);
			nSpinningFrames = 25;
			if (!pPlayer->IsLastAttackExecutedASwordAttack())
				PlaySoundGame(SOUND_LABEL_SE_HIT);
			else
				PlaySoundGame(SOUND_LABEL_SE_SWORD);
			GetCurrentGame()->GetUIManager()->SetNextTutorialMessage();

		}
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FEET)) && !pPlayer->GetFloor() && pPlayer->GetYForce() > 0)
	{
		pPlayer->TranslateY(0.1f);
		if (!IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FEET))) {
			pPlayer->TranslateY(-0.1f);
			pPlayer->SetFloor(this);
			return;
	}
}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_FRONT)))
	{
		pPlayer->TranslateZ(-0.1f);
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_BACK)))
	{
		pPlayer->TranslateZ(0.1f);
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_LEFT)))
	{
		pPlayer->TranslateX(0.1f);
	}
	while (IsInCollision3D(GetHitbox(), pPlayer->GetHitboxPlayer(PLAYER_HB_RIGHT)))
	{
		pPlayer->TranslateX(-0.1f);
	}
}

void GameObject3D::Draw()
{
#if PRINT_HITBOX
	if (pVisualHitbox) {
		Box ThisHitbox = GetHitbox();
		((Cube3D*)pVisualHitbox)->SetScale({ ThisHitbox.SizeX, ThisHitbox.SizeY, ThisHitbox.SizeZ });
		((Cube3D*)pVisualHitbox)->SetPosition({ ThisHitbox.PositionX,ThisHitbox.PositionY,ThisHitbox.PositionZ });
		((Cube3D*)pVisualHitbox)->Draw();
	}
#endif
	GetDeviceContext()->RSSetState(GetMainWindow()->GetRasterizerState(1));
	if (!bUse)
		return;
	if (bIsUnlit)
		GetMainLight()->SetLightEnable(false);
	switch (nType)
	{
	case GO_BULLET:
		if (Model)
			Model->DrawModel();
		break;
	case GO_DEBUGAIM:
		if (Model)
			Model->DrawModel();
		break;
	default:
		if (Model)
			Model->DrawModel();
		break;
	}
	GetMainLight()->SetLightEnable(true);


}


void GameObject3D::End()
{
	SAFE_DELETE(Model);
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
	if (fScale < 0) 
	{
		Scale = { 0,0,0 };
		return;
	}
	Scale = { fScale, fScale, fScale };
}

void GameObject3D::SetScale(XMFLOAT3 fScale)
{
	Scale = fScale;
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

void GameObject3D::SetPosition(XMFLOAT3 newPos, bool bInitial)
{
	Position = newPos;
	if (bInitial)
		InitialPosition = Position;
}

void GameObject3D::SetModelRotation(XMFLOAT3 rot)
{
	if(Model)
		Model->SetRotation(rot);
}

void GameObject3D::RotateAroundX(float x)
{
	Rotation.x -= x;
	if (Rotation.x > XM_2PI) {
		Rotation.x -= XM_2PI;
	}
}

void GameObject3D::RotateAroundY(float y)
{
	Rotation.y -= y;
	if (Rotation.y > XM_2PI) {
		Rotation.y -= XM_2PI;
	}
}

void GameObject3D::RotateAroundZ(float z)
{
	Rotation.z -= z;
	if (Rotation.z > XM_2PI) {
		Rotation.z -= XM_2PI;
	}
}

void GameObject3D::SetUse(bool newUse)
{
	bUse = newUse;
}

void GameObject3D::InitModel(const char * szPath)
{
	if (Model)
		SAFE_DELETE(Model);
	Model = new Model3D(this, szPath);
}

void GameObject3D::InitModel(int szPath)
{
	if (Model)
		SAFE_DELETE(Model);
	Model = new Model3D(this, szPath);
}

void GameObject3D::SetParent(GameObject3D * pNewParent)
{
	p_goParent = pNewParent;
}

void GameObject3D::TranslateX(float translation)
{
	Position.x += translation;
}

void GameObject3D::TranslateY(float translation)
{
	Position.y += translation;
}

void GameObject3D::TranslateZ(float translation)
{
	Position.z += translation;
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

void GameObject3D::AutomaticMovementControl()
{

	if (--nPauseFrames > 0)
		return;
	if (--nDelayFramesBetweenStops > 0)
		return;
	nPauseFrames = 0;
	XMFLOAT3 Destination = { 0,0,0 };
	//printf("%f\n", Position.z);
	float fDelay = x3MoveEndPos.z;

	if (bGoToStartPos)
	{
		Destination.x = x3MoveStartPos.x;
		Destination.y = x3MoveStartPos.y;
		Destination.z = x3MoveStartPos.z;
	}
	else {
		Destination.x = x3MoveEndPos.x;
		Destination.y = x3MoveEndPos.y;
		Destination.z = x3MoveEndPos.z;
	}

	bool bIsPlayerFloorOrCrawling = false;
	Player3D* pPlayer = GetPlayer3D();
	if (pPlayer) {
		if (pPlayer->GetFloor() == this)
			bIsPlayerFloorOrCrawling = true;
	}

	if (Position.x < Destination.x)
	{
		Position.x += fAutoSpeed;
		if (bIsPlayerFloorOrCrawling)
			pPlayer->TranslateX(fAutoSpeed);
		if (Position.x > Destination.x)
			Position.x = Destination.x;
	}
	else if (Position.x > Destination.x) {
		Position.x -= fAutoSpeed;
		if (bIsPlayerFloorOrCrawling)
			pPlayer->TranslateX(-fAutoSpeed);
		if (Position.x < Destination.x)
			Position.x = Destination.x;
	}

	if (Position.y < Destination.y)
	{
		Position.y += fAutoSpeed;
		if (bIsPlayerFloorOrCrawling)
			pPlayer->TranslateY(fAutoSpeed);
		if (Position.y > Destination.y)
			Position.y = Destination.y;
	}
	else if (Position.y > Destination.y) {
		Position.y -= fAutoSpeed;
		if (bIsPlayerFloorOrCrawling)
			pPlayer->TranslateY(-fAutoSpeed);
		if (Position.y < Destination.y)
			Position.y = Destination.y;
	}

	if (Position.z < Destination.z)
	{
		Position.z += fAutoSpeed;
		if (bIsPlayerFloorOrCrawling)
			pPlayer->TranslateZ(fAutoSpeed);
		if (Position.z > Destination.z)
			Position.z = Destination.z;
	}
	else if (Position.z > Destination.z) {
		Position.z -= fAutoSpeed;
		if (bIsPlayerFloorOrCrawling)
			pPlayer->TranslateZ(-fAutoSpeed);
		if (Position.z < Destination.z)
			Position.z = Destination.z;
	}
	if (Position.y == Destination.y && Position.x == Destination.x && Position.z == Destination.z)
	{
		if (bGoToStartPos)
			bGoToStartPos = false;
		else
			bGoToStartPos = true;
		//nDelayFramesBetweenStops = (int)x3MoveEndPos.z * 60;//FIX THIS
	}

}

bool GameObject3D::MoveToPos(float f_Speed, XMFLOAT3 vDestination)
{
	if (Position.x < vDestination.x)
	{
		Position.x += f_Speed;
		if (Position.x > vDestination.x)
			Position.x = vDestination.x;
	}
	else if (Position.x > vDestination.x) {
		Position.x -= f_Speed;
		if (Position.x < vDestination.x)
			Position.x = vDestination.x;
	}

	if (Position.y < vDestination.y)
	{
		Position.y += f_Speed;
		if (Position.y > vDestination.y)
			Position.y = vDestination.y;
	}
	else if (Position.y > vDestination.y) {
		Position.y -= f_Speed;
		if (Position.y < vDestination.y)
			Position.y = vDestination.y;
	}

	if (Position.z < vDestination.z)
	{
		Position.z += f_Speed;
		if (Position.z > vDestination.z)
			Position.z = vDestination.z;
	}
	else if (Position.z > vDestination.z) {
		Position.z -= f_Speed;
		if (Position.z < vDestination.z)
			Position.z = vDestination.z;
	}
	return CompareXmfloat3(Position, vDestination);
}

void GameObject3D::SetMovement(XMFLOAT3 Start, XMFLOAT3 End, float Speed, int delay)
{
	x3MoveStartPos = Start;
	x3MoveEndPos = End;
	bGoToStartPos = true;
	fAutoSpeed = Speed;
	nDelayFramesBetweenStops = delay;
	bMoveable = true;
}

bool GameObject3D::IsMoveableObject()
{
	return bMoveable;
}

XMFLOAT3 GameObject3D::GetMoveStartPosition()
{
	return x3MoveStartPos;
}

XMFLOAT3 GameObject3D::GetMoveEndPosition()
{
	return x3MoveEndPos;
}

XMFLOAT3 GameObject3D::GetForward()
{
	return GetForwardVector(Rotation);
}

XMFLOAT3 GameObject3D::GetModelForward()
{
	if (!Model)
		return{ 0,0,0 };
	return GetForwardVector(Model->GetRotation());
}

bool GameObject3D::MoveTowardPos(XMFLOAT3 Destination, float Speed)
{
	if (Position.x < Destination.x)
	{
		Position.x += Speed;
		if (Position.x > Destination.x)
			Position.x = Destination.x;
	}

	if (Position.x > Destination.x)
	{
		Position.x -= Speed;
		if (Position.x < Destination.x)
			Position.x = Destination.x;
	}

	if (Position.y < Destination.y)
	{
		Position.y += Speed;
		if (Position.y > Destination.y)
			Position.y = Destination.y;
	}

	if (Position.y > Destination.y)
	{
		Position.y -= Speed;
		if (Position.y < Destination.y)
			Position.y = Destination.y;
	}

	if (Position.z < Destination.z)
	{
		Position.z += Speed;
		if (Position.z > Destination.z)
			Position.z = Destination.z;
	}

	if (Position.z > Destination.z)
	{
		Position.z -= Speed;
		if (Position.z < Destination.z)
			Position.z = Destination.z;
	}
	return Position.z == Destination.z && Position.y == Destination.y && Position.x == Destination.x;
}

void GameObject3D::PauseObject(int pauseFrames)
{
	nPauseFrames = pauseFrames;
}

float GameObject3D::GetMoveSpeed()
{
	return fAutoSpeed;
}

int GameObject3D::GetDelayFrames()
{
	return nDelayFramesBetweenStops;
}

Go_List::Go_List()
{
	HeadNode = nullptr;
	nObjectCount = 0;
}

Go_List::~Go_List()
{
	End();
}

int Go_List::GetNumberOfObjects()
{
	return nObjectCount;
}

GameObject3D * Go_List::AddField(XMFLOAT3 newPosition, XMFLOAT3 newScale, const char * TexturePath)
{
	return AddField(newPosition, newScale, TexturePath, false, { 0,0,0 }, { 0,0,0 }, 0,0);
}

GameObject3D * Go_List::AddField(XMFLOAT3 newPosition, XMFLOAT3 newScale, const char * TexturePath, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Field3D(TexturePath);
		Field3D* ThisField = (Field3D*)(pWorkList->Object);
		ThisField->SetScale(newScale);
		ThisField->SetPosition(newPosition);
		if (Moveable)
			ThisField->SetMovement(Start, End, Speed, DelayFrames);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		nObjectCount++;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Field3D(TexturePath);
		Field3D* ThisField = (Field3D*)(HeadNode->Object);
		ThisField->SetScale(newScale);
		ThisField->SetPosition(newPosition);
		if (Moveable)
			ThisField->SetMovement(Start, End, Speed, DelayFrames);
		HeadNode->next = nullptr;
		nObjectCount++;
		return HeadNode->Object;
	}
}

GameObject3D * Go_List::AddWall(XMFLOAT3 newPosition, XMFLOAT3 newScale)
{
	return AddWall(newPosition, newScale, false, { 0,0,0 }, { 0,0,0 },0,0);
}

GameObject3D * Go_List::AddWall(XMFLOAT3 newPosition, XMFLOAT3 newScale, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Wall3D();
		Wall3D* thisWall = (Wall3D*)(pWorkList->Object);
		thisWall->SetScale(newScale);
		thisWall->SetPosition(newPosition);
		if (Moveable)
			thisWall->SetMovement(Start, End, Speed, DelayFrames);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		nObjectCount++;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Wall3D();
		Wall3D* thisWall = (Wall3D*)(HeadNode->Object);
		thisWall->SetScale(newScale);
		thisWall->SetPosition(newPosition);
		if (Moveable)
			thisWall->SetMovement(Start, End, Speed, DelayFrames);
		HeadNode->next = nullptr;
		nObjectCount++;
		return HeadNode->Object;
	}
}

GameObject3D * Go_List::AddEnemy(XMFLOAT3 newPosition)
{
	return AddEnemy(newPosition,false, { 0,0,0 }, { 0,0,0 }, 0, 0);
}

GameObject3D * Go_List::AddEnemy(XMFLOAT3 newPosition, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames)
{
	go_node* pPositionList = HeadNode;
	if (HeadNode != nullptr) {
		while (pPositionList->next != nullptr) {
			pPositionList = pPositionList->next;
		}
		go_node* pWorkList = new go_node();
		pWorkList->Object = new Enemy3D();
		Enemy3D* thisWall = (Enemy3D*)(pWorkList->Object);
		thisWall->SetPosition(newPosition, true);
		if (Moveable)
			thisWall->SetMovement(Start, End, Speed, DelayFrames);
		pWorkList->next = nullptr;
		pPositionList->next = pWorkList;
		nObjectCount++;
		return pWorkList->Object;
	}
	else {
		HeadNode = new go_node();
		HeadNode->Object = new Enemy3D();
		Enemy3D* thisWall = (Enemy3D*)(HeadNode->Object);
		thisWall->SetPosition(newPosition);
		if (Moveable)
			thisWall->SetMovement(Start, End, Speed, DelayFrames);
		HeadNode->next = nullptr;
		nObjectCount++;
		return HeadNode->Object;
	}
}

GameObject3D * Go_List::CheckCollision(Box hb)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (IsInCollision3D(pPositionList->Object->GetHitbox(), hb))
				return pPositionList->Object;
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

GameObject3D * Go_List::CheckCollision(Box hb, bool bIgnoreUnused)
{
	if (HeadNode == nullptr)
		return nullptr;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
		{
			if (bIgnoreUnused && pPositionList->Object->IsInUse()) {
				if (IsInCollision3D(pPositionList->Object->GetHitbox(), hb))
					return pPositionList->Object;
			}
		}
		pPositionList = pPositionList->next;
	}
	return nullptr;
}

void Go_List::DeleteLastPosObject()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode->next;
	go_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
	{
		if (PreviousPos->Object) {
			delete(PreviousPos->Object);
			PreviousPos->Object = nullptr;
			delete(PreviousPos);
			HeadNode = PreviousPos = nullptr;
			return;
		}
	}
	while (true) {
		if (pPositionList->next == nullptr)
			break;
		pPositionList = pPositionList->next;
		PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		PreviousPos->next = nullptr;
	}
	return;
}

void Go_List::DeleteObject(GameObject3D * pSearch)
{
	if (HeadNode == nullptr)
		return;
	if (HeadNode->Object == pSearch)
	{
		go_node* pPositionList = HeadNode;
		HeadNode = HeadNode->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	if (HeadNode->next->Object == pSearch)
	{
		go_node* pPositionList = HeadNode->next;
		HeadNode->next = HeadNode->next->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
		return;
	}
	go_node* pPositionList = HeadNode->next;
	go_node* PreviousPos = HeadNode;
	if (pPositionList == nullptr)
		return;
	while (true) {
		if (pPositionList->next == nullptr)
			return;
		if (pPositionList->Object == pSearch)
			break;
		pPositionList = pPositionList->next;
		PreviousPos = PreviousPos->next;
	}
	if (pPositionList->Object) {
		PreviousPos->next = pPositionList->next;
		delete(pPositionList->Object);
		pPositionList->Object = nullptr;
		delete(pPositionList);
		pPositionList = nullptr;
	}
	return;
}

void Go_List::Update()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr)
			pPositionList->Object->Update();
		pPositionList = pPositionList->next;
	}
}

void Go_List::Draw()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;

		if (pPositionList->Object != nullptr)
			pPositionList->Object->Draw();
		pPositionList = pPositionList->next;
	}
}

void Go_List::End()
{
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	go_node* pWork = nullptr;
	int Count = 0;
	while (true) {
		if (pPositionList == nullptr)
			break;
		if (pPositionList->Object != nullptr) {
			Count++;
			delete(pPositionList->Object);
			pPositionList->Object = nullptr;
		}
		pWork = pPositionList;
		pPositionList = pPositionList->next;
		if (pWork) {
			delete(pWork);
			pWork = nullptr;
		}
	}
	printf("GameObjects Deleted: %d\n", Count);
	nObjectCount = 0;
	HeadNode = nullptr;
}

void Go_List::SaveFields(const char * szFilename)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/levels/";
	strcat(szFinalfilename, szFilename);
	strcat(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy(szFinalfilename, "Default.bin");
	}
	pFile = fopen(szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;

		if (pPositionList->Object != nullptr)
		{
			if (pPositionList->Object->GetType() == GO_FLOOR)
			{
				GameObjectContainer field;
				Field3D* thisField = (Field3D*)pPositionList->Object;
				field.Pos = thisField->GetPosition();
				field.Scale = thisField->GetScale();
				field.bMoveable = thisField->IsMoveableObject();
				field.MoveStartPos = thisField->GetMoveStartPosition();
				field.MoveEndPos = thisField->GetMoveEndPosition();
				field.Speed = thisField->GetMoveSpeed();
				field.DelayFrames = thisField->GetDelayFrames();
				strcpy(field.texpath, thisField->GetTexturePath());
				fwrite(&field, sizeof(GameObjectContainer), 1, pFile);
			}
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	fclose(pFile);
}

void Go_List::SaveWalls(const char * szFilename)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/levels/";
	strcat(szFinalfilename, szFilename);
	strcat(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy(szFinalfilename, "Default.bin");
	}
	pFile = fopen(szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;

		if (pPositionList->Object != nullptr)
		{
			if (pPositionList->Object->GetType() == GO_WALL)
			{
				GameObjectContainer field;
				Wall3D* thisWall = (Wall3D*)pPositionList->Object;
				field.Pos = thisWall->GetPosition();
				field.Scale = thisWall->GetScale();
				field.bMoveable = thisWall->IsMoveableObject();
				field.MoveStartPos = thisWall->GetMoveStartPosition();
				field.MoveEndPos = thisWall->GetMoveEndPosition();
				field.Speed = thisWall->GetMoveSpeed();
				field.DelayFrames = thisWall->GetDelayFrames();
				strcpy(field.texpath, thisWall->GetTexturePath());
				fwrite(&field, sizeof(GameObjectContainer), 1, pFile);
			}
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	fclose(pFile);
}

void Go_List::SaveEnemies(const char * szFilename)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/levels/";
	strcat(szFinalfilename, szFilename);
	strcat(szFinalfilename, ".bin");
	if (strcmp(szFilename, "") == 0)
	{
		strcpy(szFinalfilename, "Default.bin");
	}
	pFile = fopen(szFinalfilename, "wb");
	if (HeadNode == nullptr)
		return;
	go_node* pPositionList = HeadNode;
	while (true) {

		if (pPositionList == nullptr)
			break;

		if (pPositionList->Object != nullptr)
		{
			if (pPositionList->Object->GetType() == GO_ENEMY)
			{
				GameObjectContainer field;
				Enemy3D* thisWall = (Enemy3D*)pPositionList->Object;
				field.Pos = thisWall->GetPosition();
				field.Scale = thisWall->GetScale();
				field.bMoveable = thisWall->IsMoveableObject();
				field.MoveStartPos = thisWall->GetMoveStartPosition();
				field.MoveEndPos = thisWall->GetMoveEndPosition();
				field.Speed = thisWall->GetMoveSpeed();
				field.DelayFrames = thisWall->GetDelayFrames();
				strcpy(field.texpath, "nt");
				fwrite(&field, sizeof(GameObjectContainer), 1, pFile);
			}
		}
		pPositionList = pPositionList->next;
	}
	printf("SAVED OK: %s\n", szFinalfilename);
	fclose(pFile);
}

void Go_List::Load(const char * szFilename, int nType)
{
	FILE *pFile;
	char szFinalfilename[256] = "data/levels/";
	strcat(szFinalfilename, szFilename);
	strcat(szFinalfilename, ".bin");
	if ((pFile = fopen(szFinalfilename, "rb")) == NULL)
	{
		printf_s("%sのファイルはない…\n", szFinalfilename);
		return;
	}
	GameObjectContainer* go_container = new GameObjectContainer();
	switch (nType)
	{
	case GO_FLOOR:
		while ((fread(go_container, sizeof(GameObjectContainer), 1, pFile)))
			AddField(go_container->Pos, go_container->Scale, go_container->texpath, go_container->bMoveable, go_container->MoveStartPos, go_container->MoveEndPos, go_container->Speed, go_container->DelayFrames);
		break;
	case GO_WALL:
		while ((fread(go_container, sizeof(GameObjectContainer), 1, pFile)))
			AddWall(go_container->Pos, go_container->Scale, go_container->bMoveable, go_container->MoveStartPos, go_container->MoveEndPos, go_container->Speed, go_container->DelayFrames);
		break;
	case GO_ENEMY:
		while ((fread(go_container, sizeof(GameObjectContainer), 1, pFile)))
			AddEnemy(go_container->Pos, go_container->bMoveable, go_container->MoveStartPos, go_container->MoveEndPos, go_container->Speed, go_container->DelayFrames);
		break;
	default:
		break;
	}
	delete(go_container);
	fclose(pFile);
}
