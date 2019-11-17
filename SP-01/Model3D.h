#ifndef	MODEL3D
#define MODEL3D
#include "DXWindow3D.h"
#include "Camera3D.h"
#include "FbxModel.h"
#include "Light3D.h"
//#include "Camera3D.h"
class Model3D
{
private:
	CFbxModel*		g_pModel;		// FBX���f���ւ̃|�C���^
	XMFLOAT3		Position;		// ���݂̈ʒu
	XMFLOAT3		Rotation;		// ���݂̌���
	XMFLOAT3		Scale;		// ���݂̌���
	XMFLOAT4X4	g_mtxWorld;		// ���[���h�}�g���b�N�X
	bool bIsMoving = false;
	Camera3D* pMainCamera;
	int nFrame;
	int nFramCount;
	int nAnimationFrameSlowness;
	int nAnimationSkipFrame;
	void* GameObjectParent;
public:
	Model3D(void* Parent, const char*ModelPath);
	~Model3D();
	HRESULT InitModel(const char*ModelPath);
	void UninitModel(void);
	void UpdateModel(void);
	void SwitchAnimation(int nNewAnimNum, int FrameSlowness, int nAnimationSkipFrame);
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
};
#endif
