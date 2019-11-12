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
	XMFLOAT3		g_posModel;		// ���݂̈ʒu
	XMFLOAT3		g_rotModel;		// ���݂̌���
	XMFLOAT3		g_ScaleModel;		// ���݂̌���
	XMFLOAT4X4	g_mtxWorld;		// ���[���h�}�g���b�N�X
	bool bIsMoving = false;
	Camera3D* pMainCamera;
	int nFrame;
	int nFramCount;
	int nAnimationFrameSpeed;
	void* GameObjectParent;
public:
	Model3D();
	~Model3D();
	HRESULT InitModel(Light3D* SceneLight, const char*ModelPath);
	void UninitModel(void);
	void UpdateModel(void);
	void SwitchAnimation(int nNewAnimNum, int FrameSlowness);
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
};
#endif
