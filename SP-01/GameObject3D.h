#pragma once
#include "UniversalStructures.h"
#include "Model3D.h"
#include "Light3D.h"

enum eGameObjectTypes
{
	GO_PLAYER=0,
	GO_BULLET,
	GO_SHADOW,
	GO_TITLE_LOGO,
	GO_FLOOR,
	GO_DEBUGAIM,
	GO_CUBE,
	GO_WALL,
	GO_ENEMY,
	GO_SKULLWARNING,
	GO_ENEMYHEARTMARK,
	GO_TUTORIALSIGN,
	GO_MAX
};
class GameObject3D
{
private:
	int nPauseFrames;
	//自動で動いているオブジェクトなら
	bool bMoveable;
	bool bGoToStartPos;
	XMFLOAT3 x3MoveStartPos;
	XMFLOAT3 x3MoveEndPos;
	int nDelayFramesBetweenStops;
	float fAutoSpeed;
	float fTutorialSignSpeed;
	int nSpinningFrames;
protected:
	Model3D* Model;
	XMFLOAT3 Rotation;
	XMFLOAT3 Position;
	XMFLOAT3 InitialPosition;
	XMFLOAT3 Scale;
	Camera3D* pMainCamera;
	XMFLOAT4X4 WorldMatrix;
	int nType;
	bool bUse;
	bool bIsUnlit;
	//影に関する
	GameObject3D* p_goParent;
	int nUseCounterFrame;
	Box Hitbox;
	void* pVisualHitbox;
	int nCurrentRasterizer;
	Light3D* pLight;
	bool ScaleUp;
	float fAcceleration;
public:
	GameObject3D();
	GameObject3D(int nType);
	GameObject3D(const char* ModelPath, int nType);
	GameObject3D(Light3D* light, const char* ModelPath, int nType);
	virtual ~GameObject3D();
	virtual void Init();
	virtual void Update();
	void TutorialSignControl();
	virtual void Draw();
	virtual void End();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetScale();
	Model3D* GetModel();
	void SetScale(float fScale);
	void SetScale(XMFLOAT3 fScale);
	void SetRotation(XMFLOAT3 fRot);
	XMFLOAT4X4* GetModelWorld();
	void SetPosition(XMFLOAT3 newPos);
	void SetPosition(XMFLOAT3 newPos, bool bInitial);
	void SetModelRotation(XMFLOAT3 rot);
	void RotateAroundX(float x);
	void RotateAroundY(float y);
	void RotateAroundZ(float z);
	void SetUse(bool newUse);
	void InitModel(const char* szPath);
	void InitModel(int szPath);
	void SetParent(GameObject3D* pNewParent);
	void TranslateX(float translation);
	void TranslateY(float translation);
	void TranslateZ(float translation);
	bool IsInUse();
	int GetType();
	Box GetHitbox();
	void SetHitbox(Box);
	//自動で動いているオブジェクトなら
	void AutomaticMovementControl();
	bool MoveToPos(float fSpeed, XMFLOAT3 Destination);
	void SetMovement(XMFLOAT3 Start, XMFLOAT3 End, float Speed, int nDelayFrames);
	bool IsMoveableObject();
	XMFLOAT3 GetMoveStartPosition();
	XMFLOAT3 GetMoveEndPosition();
	XMFLOAT3 GetForward();
	XMFLOAT3 GetModelForward();
	XMFLOAT3* GetScaleAdd();
	bool MoveTowardPos(XMFLOAT3 Destination, float Speed);
	void PauseObject(int pauseFrames);
	float GetMoveSpeed();
	int GetDelayFrames();
};



//*****************************************************************************
// クラス-リスト
//*****************************************************************************
typedef struct go_node {//リストの管理の為に、この構造体が要る
	GameObject3D* Object;
	go_node *next;
};

typedef struct GameObjectContainer {
	XMFLOAT3 Pos;
	XMFLOAT3 Scale;
	char texpath[256];
	bool bMoveable = false;
	XMFLOAT3 MoveStartPos;
	XMFLOAT3 MoveEndPos;
	float Speed; 
	int DelayFrames;
};

class Go_List
{
private:
	int nObjectCount;
public:
	go_node * HeadNode;
	Go_List();
	~Go_List();
	int GetNumberOfObjects();
	GameObject3D* AddField(XMFLOAT3 newPosition, XMFLOAT3 newScale, const char* TexturePath);
	GameObject3D* AddField(XMFLOAT3 newPosition, XMFLOAT3 newScale, const char* TexturePath, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames);
	GameObject3D* AddWall(XMFLOAT3 newPosition, XMFLOAT3 newScale);
	GameObject3D* AddWall(XMFLOAT3 newPosition, XMFLOAT3 newScale, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames);
	//GameObject3D* AddItem(XMFLOAT3 newPosition, int nType);
	//GameObject3D* AddItem(XMFLOAT3 newPosition, int nType, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End);
	//GameObject3D* AddSpike(XMFLOAT3 newPosition, int SpikesX, int SpikesY, bool binvisible);
	//GameObject3D* AddSpike(XMFLOAT3 newPosition, int SpikesX, int SpikesY, bool binvisible, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End);
	//GameObject3D* AddMisc(XMFLOAT3 newPosition, int nType);
	//GameObject3D* AddMisc(XMFLOAT3 newPosition, int nType, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End);
	//GameObject3D* AddMirror(XMFLOAT3 newPosition, XMFLOAT3 Destination);
	//GameObject3D* AddMirror(XMFLOAT3 newPosition, XMFLOAT3 Destination, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End);
	GameObject3D* AddEnemy(XMFLOAT3 newPosition);
	GameObject3D* AddEnemy(XMFLOAT3 newPosition, bool Moveable, XMFLOAT3 Start, XMFLOAT3 End, float Speed, int DelayFrames);
	GameObject3D* CheckCollision(Box hb);
	GameObject3D * CheckCollision(Box hb, bool bIgnoreUnused);
	void DeleteLastPosObject();
	void DeleteObject(GameObject3D*);
	void Update();
	void Draw();
	void End();
	void SaveFields(const char* szFilename);
	void SaveWalls(const char* szFilename);
	void SaveEnemies(const char* szFilename);
	//void SaveItems(const char* szFilename);
	//void SaveSpikes(const char* szFilename);
	//void SaveMisc(const char* szFilename);
	//void SaveMirrors(const char* szFilename);
	//void SaveEnemies(const char* szFilename);
	void Load(const char* szFilename, int nType);
};
