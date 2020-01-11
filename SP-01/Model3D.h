#ifndef	MODEL3D
#define MODEL3D
#include "DXWindow3D.h"
#include "Camera3D.h"
#include "FbxModel.h"
#include "Light3D.h"
//#include "Camera3D.h"
enum MyEnum
{
	ENEMY_WARRIOR_MODEL_PATH,
	WALL_MODEL_PATH,
	//SPIKE_MODEL,
	MAX_PRELOADED_MODELS,
};

class Model3D
{
private:
	CFbxModel*		g_pModel;		// FBXモデルへのポインタ
	XMFLOAT3		Position;		// 現在の位置
	XMFLOAT3		Rotation;		// 現在の向き
	XMFLOAT3		Scale;		// 現在の向き
	XMFLOAT4X4	g_mtxWorld;		// ワールドマトリックス
	bool bIsMoving = false;
	Camera3D* pMainCamera;
	float fFrame;
	int nFramCount;
	int nAnimationFrameSlowness;
	int nCurrentAnimation;
	int nInitialFrame;
	int nFinalFrame;
	float fAnimationSkipFrame;
	void* GameObjectParent;
	Light3D* pLight;
	int nCountLoop;
	bool bLoop;
	bool bPreLoadedModel;
public:
	Model3D(void* Parent, const char*ModelPath);
	Model3D(void* Parent, int ModelPath);
	~Model3D();
	HRESULT InitModel(const char*ModelPath);
	HRESULT InitModel(int ModelPath);
	void UninitModel(void);
	void UpdateModel(void);
	void SwitchAnimation(int nNewAnimNum, int FrameSlowness, float nAnimationSkipFrame);
	void DrawModel(void);
	void AnimationControl();
	XMFLOAT4X4 *GetModelWorld();
	XMFLOAT3 GetRotation();
	XMFLOAT3 GetPosition();
	void SetScale(float newScale);
	int GetNumberOfAnimations();
	void SetParent(void* newParent);
	void RotateAroundX(float x);
	void RotateAroundY(float y);
	void TranslateModel(XMFLOAT3 translation);
	void SetRotation(XMFLOAT3 rot);
	void SetPosition(XMFLOAT3 pos);
	void SetRotationX(float rotx);
	void SetRotationY(float roty);
	void SetLight(Light3D* newLight);
	int GetCurrentAnimation();
	int GetCurrentFrameOfAnimation();
	int GetEndFrameOfCurrentAnimation();
	int GetCurrentFrame();
	int GetLoops();
	void SetFrameOfAnimation(float Frame);
	void SetLoop(bool loopable);
};
void UninitPreloadedModels();
#endif
