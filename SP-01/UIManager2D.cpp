//*****************************************************************************
// UIManager2D.cpp
// UIの管理
// Author : Mane
//*****************************************************************************
#include "UIManager2D.h"
#include "S_InGame3D.h"
#include "RankManager.h"
#include "Sound.h"
#include "Texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define HEART_SIZE	75
#define AIM_SIZE	125
#define HEART_TATE true

//*****************************************************************************
// グローバル変数
//*****************************************************************************
ID3D11ShaderResourceView*	AuraTexture[MAX_AURA_TEXTURE] = {nullptr};
ID3D11ShaderResourceView*	TutorialMessageTexture[TUTM_MAX] = {nullptr};

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
UIObject2D::UIObject2D()
{
	Init();
}

UIObject2D::UIObject2D(int nUI_Type)
{
	Init();
	nType = nUI_Type;
	switch (nUI_Type)
	{
	case UI_HEART:
		CreateTextureFromFile(GetDevice(), "data/texture/HeartUI.tga", &gpTexture);
		Position.x = -(SCREEN_WIDTH / 2)+50;
		Position.y = (SCREEN_HEIGHT / 2)-50;
		x2Frame.x = 1.0f/5.0f;
		x2Frame.y = 1.0f/4.0f;
		uv.U = 0;
		uv.V = 0;
		SetPolygonSize(HEART_SIZE, HEART_SIZE);
		nAnimationSpeed = 5;
		SetTexture(gpTexture);
		break;
	case UI_AIM:
		CreateTextureFromFile(GetDevice(), "data/texture/AimUI.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		SetPolygonSize(AIM_SIZE, AIM_SIZE);
		SetTexture(gpTexture);
		break;
	case UI_LOGO:
		CreateTextureFromFile(GetDevice(), "data/texture/LogoUI.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1.0f/8.0f;
		x2Frame.y = 1.0f/8.0f;
		uv.U = 0;
		uv.V = 0;
		nAnimationSpeed = 1;
		SetPolygonSize(768, 432);
		SetTexture(gpTexture);
		break;
	case UI_ATKZOOM:
		CreateTextureFromFile(GetDevice(), "data/texture/AtkEffect.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1.0f / 1.0f;
		x2Frame.y = 1.0f / 1.0f;
		uv.U = 0;
		uv.V = 0;
		SetPolygonSize(1280, 1280);
		SetPolygonAlpha(0.5f);
		SetTexture(gpTexture);
		break;
	case UI_AURA:
		if (!AuraTexture[NORMAL_AURA_TEXTURE])
			CreateTextureFromFile(GetDevice(), "data/texture/AuraEffect.tga", &AuraTexture[NORMAL_AURA_TEXTURE]);
		if (!AuraTexture[DARK_AURA_TEXTURE])
			CreateTextureFromFile(GetDevice(), "data/texture/AuraEffectAlt.tga", &AuraTexture[DARK_AURA_TEXTURE]);		
		if (!AuraTexture[HEART_AURA_TEXTURE])
			CreateTextureFromFile(GetDevice(), "data/texture/HeartAuraEffect.tga", &AuraTexture[HEART_AURA_TEXTURE]);
		Position.x = 0;
		Position.y = 0;
		SetPolygonSize(0, 0);
		SetTexture(AuraTexture[NORMAL_AURA_TEXTURE]);
		break;
	case UI_GAMEOVER_SCREEN:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/GameOverScreen.tga", &gpTexture);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1.0f / 2.0f;
		x2Frame.y = 1.0f / 10.0f;
		uv.U = 0;
		uv.V = 0;
		SetPolygonSize(1280, 720);
		SetPolygonAlpha(1);
		SetTexture(gpTexture);
		nAnimationSpeed = 3;
		break;
	case UI_SCORE:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/NumbersUI.tga", &gpTexture);
		SetPolygonSize(18, 25);
		Position.x = SCREEN_WIDTH / 2 - (25 * 8);
		Position.y = SCREEN_HEIGHT / 2 - 25;
		x2Frame.x = 1 / 10.0f;
		x2Frame.y = 1 / 10.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(gpTexture);
		nAnimationSpeed = 3;
		break;
	case UI_RANK:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/RankUI.tga", &gpTexture);
		SetPolygonSize(100, 75);
		Position.x = 550;
		Position.y = 280;
		x2Frame.x = 1 / 4.0f;
		x2Frame.y = 1 / 10.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(gpTexture);
		nAnimationSpeed = 3;
		break;
	case UI_TIMER:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/NumbersUI.tga", &gpTexture);
		SetPolygonSize(18, 25);
		Position.x = -25;
		Position.y = -(SCREEN_HEIGHT / 2 - 25);
		x2Frame.x = 1 / 10.0f;
		x2Frame.y = 1 / 10.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(gpTexture);
		nAnimationSpeed = 3;
		break;
	case UI_RESULT_SCREEN_NUMBER:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/GRS_GeneralRankingNumbers.tga", &gpTexture);
		SetPolygonSize(108, 110);
		Position.x = -(SCREEN_WIDTH/2)+(110*3.5f);
		Position.y = 0;
		x2Frame.x = 1 / 10.0f;
		x2Frame.y = 1 / 1.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(gpTexture);
		break;
	case UI_SCORETEXT:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/ScoreText.tga", &gpTexture);
		SetPolygonSize(25, 95);
		Position.x = (SCREEN_WIDTH / 2)-25;
		Position.y = (SCREEN_HEIGHT / 2) - (95/2 + 10);
		x2Frame.x = 1 / 1.0f;
		x2Frame.y = 1 / 1.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(gpTexture);
		break;
	case UI_TUTORIAL_MESSAGE:
		if (!TutorialMessageTexture[TUTM_ATTACK])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/TutMessage3.tga", &TutorialMessageTexture[TUTM_ATTACK]);
		if (!TutorialMessageTexture[TUTM_MOVE])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/TutMessage1.tga", &TutorialMessageTexture[TUTM_MOVE]);
		if (!TutorialMessageTexture[TUTM_EVADE])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/TutMessage2.tga", &TutorialMessageTexture[TUTM_EVADE]);
		if (!TutorialMessageTexture[TUTM_ATTACK2])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/TutMessage4.tga", &TutorialMessageTexture[TUTM_ATTACK2]);
		if (!TutorialMessageTexture[TUTM_LOCKON])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/TutMessage6.tga", &TutorialMessageTexture[TUTM_LOCKON]);
		if (!TutorialMessageTexture[TUTM_JUMP])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/TutMessage5.tga", &TutorialMessageTexture[TUTM_JUMP]);
		if (!TutorialMessageTexture[TUTM_GAMESTART])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/TutGameStart.tga", &TutorialMessageTexture[TUTM_GAMESTART]);
		if (!TutorialMessageTexture[TUTM_MISC1])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/Misc1.png", &TutorialMessageTexture[TUTM_MISC1]);
		if (!TutorialMessageTexture[TUTM_MISC2])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/Misc2.png", &TutorialMessageTexture[TUTM_MISC2]);
		if (!TutorialMessageTexture[TUTM_MISC3])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/Misc3.png", &TutorialMessageTexture[TUTM_MISC3]);
		if (!TutorialMessageTexture[TUTM_MISC4])
			CreateTextureFromFile(GetDevice(), "data/texture/TUT/Misc4.png", &TutorialMessageTexture[TUTM_MISC4]);
		SetPolygonSize(1280, 206);
		Position.x = 0;
		Position.y = (SCREEN_HEIGHT / 2) - (206 / 2);
		x2Frame.x = 1 / 1.0f;
		x2Frame.y = 1 / 1.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(TutorialMessageTexture[TUTM_ATTACK]);
		break;
	case UI_PAUSE:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/PAUSE.tga", &gpTexture);
		SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1 / 1.0f;
		x2Frame.y = 1 / 1.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(gpTexture);
		break;
	case UI_PAUSE_BLACK:
		CreateTextureFromFile(GetDevice(), "data/texture/UI/black.tga", &gpTexture);
		SetPolygonSize(SCREEN_WIDTH, SCREEN_HEIGHT);
		Position.x = 0;
		Position.y = 0;
		x2Frame.x = 1 / 1.0f;
		SetPolygonAlpha(0.65f);
		x2Frame.y = 1 / 1.0f;
		uv = { 0,0 };
		nNum = 0;
		SetTexture(gpTexture);
		break;
	default:
		break;
	}
}


UIObject2D::~UIObject2D()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::Init()
{
	nType = UI_TYPE_MAX;
	nAnimationTimer = 0;
	nGameOverFrames = 0;
	fAcceleration = 0;
	nAnimationSpeed = 0;
	bHeartActive = true;
	bSpin = false;
	bIsInUse = false;
	nCurrent_hearts = 1;
	nMax_Hearts = 1;
	pPlayer = nullptr;
	nFramesActive = 0;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::Update()
{
	UpdatePolygon();
	switch (nType)
	{
	case UI_HEART:
		if (!pPlayer)
			pPlayer = GetPlayer3D();
		nCurrent_hearts = pPlayer->GetCurrentHealth();
		nMax_Hearts = pPlayer->GetMaxHealth();
		RegularUVAnimation();
		break;
	case UI_AIM:
		Rotation.z++;
		break;
	case UI_LOGO:
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			uv.U++;
			if (uv.U == 8)
			{
				uv.U = 0;
				uv.V++;
				if (uv.V == 8) {
					uv.V = 6;
					uv.U = 3;
				}
			}
		}
		if (uv.V == 6)
			nAnimationSpeed = 3;
		break;
	case UI_ATKZOOM:
		if (nFramesActive > 0)
			nFramesActive--;
		Rotation.z+=20;
		break;
	case UI_AURA:
		if (!bIsInUse)
			return;
		fAcceleration+=7.5f;
		Scale.x+=fAcceleration;
		Scale.y+=fAcceleration;
		if (bSpin)
			Rotation.z += 5;
		else
			Rotation.z = 0;
		if (Scale.x > 1500)
			bIsInUse = false;
		break;
	case UI_GAMEOVER_SCREEN:
		if (!bIsInUse)
			return;
		if (++nGameOverFrames > 600)
			nGameOverFrames = 600;
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			uv.U++;
			if (uv.U == 2)
			{
				uv.U = 0;
				uv.V++;
				if (uv.V == 10) {
					uv.V = 8;
					//uv.V = 0;//del
					uv.U = 0;
				}
			}
		}
		break;
	case UI_SCORE:
		nNum = GetScore();
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			if (++uv.V == 10)
				uv.V = 0;
		}
		break;
	case UI_RANK:
		uv.U = GetRank()-2;
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			if (++uv.V == 10)
				uv.V = 0;
		}
		break;
	case UI_TIMER:
		nNum = GetTimer();
		if (++nAnimationTimer > nAnimationSpeed) {
			nAnimationTimer = 0;
			if (++uv.V == 10)
				uv.V = 0;
		}
		break;
	case UI_RESULT_SCREEN_NUMBER:
		break;
	default:
		break;
	}
	
}

//*****************************************************************************
//RegularUVAnimation関数
//UVアニメーションの管理
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::RegularUVAnimation()
{
	if (++nAnimationTimer > nAnimationSpeed) {
		nAnimationTimer = 0;
		uv.U++;
		if (uv.U == x2Frame.x)
		{
			uv.U = 0;
			uv.V++;
			if (uv.V == x2Frame.y)
				uv.V = 0;
		}
	}
}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::Draw()
{
	switch (nType)
	{
	case UI_HEART:
		UIHeartDrawControl();
		break;
	case UI_AIM:
		if (!pPlayer) 
			pPlayer = GetPlayer3D();
		else
			if(pPlayer->IsPlayerAiming())
				DrawPolygon(GetDeviceContext());
		break;
	case UI_LOGO:
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / 8.0f, uv.V / 8.0f);
		DrawPolygon(GetDeviceContext());
		break;
	case UI_ATKZOOM:
		if (nFramesActive > 0)
			DrawPolygon(GetDeviceContext());
		break;
	case UI_AURA:
		if (!bIsInUse)
			return;
		DrawPolygon(GetDeviceContext());
		break;
	case UI_GAMEOVER_SCREEN:
		if (!bIsInUse)
			return;
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / 2.0f, uv.V / 10.0f);
		DrawPolygon(GetDeviceContext());
		break;
	case UI_SCORE: case UI_TIMER:
		DrawNumber();
		break;
	case UI_RANK:
		if (GetRank() - 2 < 0)
			break;
		SetPolygonUV(uv.U / 4.0f, uv.V / 10.0f);
		DrawPolygon(GetDeviceContext());
		break;
	case UI_RESULT_SCREEN_NUMBER:
		DrawNumber();
		break;
	default:
		DrawPolygon(GetDeviceContext());
		break;
	}
	
}

//*****************************************************************************
//DrawNumber関数
//数字のレンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::DrawNumber()
{
	XMFLOAT3 x2OriginalPosition;
	int nNumberToPrint;
	int nDividerByTen;
	int nDigits = 0;
	int	nMaxDigits = 7;
	if(nType == UI_TIMER)
		nMaxDigits = 2;
	x2OriginalPosition = Position;
	nNumberToPrint = nNum;
	float fSepatation = 27;
	if (nType == UI_RESULT_SCREEN_NUMBER)
		fSepatation = 105.0f;
	for (nDividerByTen = 1; nNumberToPrint > 0; nNumberToPrint /= 10, nDividerByTen *= 10, nDigits++);
	nDividerByTen /= 10;
	if (nNum < 10)
	{
		for (int i = 0; i < nMaxDigits; i++)
		{
			uv.U = 0;
			SetPolygonFrameSize(x2Frame.x, x2Frame.y);
			SetPolygonUV(uv.U / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
			DrawPolygon(GetDeviceContext());
			Position.x += fSepatation * 0.75f;
		}
		uv.U = nNum;
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
		DrawPolygon(GetDeviceContext());
	}
	else {
		for (int i = 0; i < nMaxDigits - nDigits + 1; i++)
		{
			uv.U = 0;
			SetPolygonFrameSize(x2Frame.x, x2Frame.y);
			SetPolygonUV(uv.U / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
			DrawPolygon(GetDeviceContext());
			Position.x += fSepatation * 0.75f;
		}
		while (nDividerByTen > 0)
		{
			nNumberToPrint = nNum / nDividerByTen % 10;
			SetPolygonFrameSize(x2Frame.x, x2Frame.y);
			SetPolygonUV(nNumberToPrint / (1 / x2Frame.x), uv.V / (1 / x2Frame.y));
			DrawPolygon(GetDeviceContext());
			Position.x += fSepatation * 0.75f;
			nDividerByTen /= 10;
		}
	}
	SetPolygonPos(x2OriginalPosition.x, x2OriginalPosition.y);
}

//*****************************************************************************
//UIHeartDrawControl関数
//HPのレンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::UIHeartDrawControl()
{
	int nHeartsToShow = 1;
	if (nHeartsToShow > nMax_Hearts)
		return;
	XMFLOAT3 ogHeartPos = Position;
	for (int i = 0; i < nMax_Hearts; i++, nHeartsToShow++)
	{
		bHeartActive = nHeartsToShow <= nCurrent_hearts;
		if (bHeartActive)
		{
			SetPolygonSize(HEART_SIZE, HEART_SIZE);
			SetColor(1, 1, 1);
		}
		else
		{
			SetPolygonSize(HEART_SIZE*0.75f, HEART_SIZE*0.75f);
			SetColor(0.25, 0.25, 0.25);
		}
		SetPolygonFrameSize(x2Frame.x, x2Frame.y);
		SetPolygonUV(uv.U / 5.0f, uv.V / 4.0f);
		DrawPolygon(GetDeviceContext());
		if(HEART_TATE)
			Position.y -= HEART_SIZE * 0.75f;
		else
			Position.x -= HEART_SIZE * 0.75f;
	}
	Position = ogHeartPos;
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::End()
{
	//なし
}

//*****************************************************************************
//SetActiveFrames関数
//使っているフレームを設定する
//引数：int
//戻：void
//*****************************************************************************
void UIObject2D::SetActiveFrames(int Frames)
{
	nFramesActive=Frames;
}

//*****************************************************************************
//GetUse関数
//使っている状態を確認する
//引数：void
//戻：bool
//*****************************************************************************
bool UIObject2D::GetUse()
{
	return bIsInUse;
}

//*****************************************************************************
//SetAura関数
//オーラーを設定する
//引数：void
//戻：void
//*****************************************************************************
void UIObject2D::SetAura()
{
	bIsInUse = true;
	Scale = { 0,0,0 };
	fAcceleration = 0;
}

//*****************************************************************************
//SetUse関数
//使うことを設定する
//引数：bool
//戻：void
//*****************************************************************************
void UIObject2D::SetUse(bool use)
{
	bIsInUse = use;
}

//*****************************************************************************
//GetGameOverFrames関数
//ゲームオーバーのフレームを戻す
//引数：void
//戻：int
//*****************************************************************************
int UIObject2D::GetGameOverFrames()
{
	return nGameOverFrames;
}

//*****************************************************************************
//SetSpin関数
//回転を設定する
//引数：bool
//戻：void
//*****************************************************************************
void UIObject2D::SetSpin(bool spin)
{
	bSpin = spin;
}

//*****************************************************************************
//SetNum関数
//数字を設定する
//引数：int
//戻：void
//*****************************************************************************
void UIObject2D::SetNum(int num)
{
	nNum = num;
}

//*****************************************************************************
//SetTutorialMessage関数
//チュートリアルのメッセージを設定する
//引数：int
//戻：void
//*****************************************************************************
void UIObject2D::SetTutorialMessage(int num)
{
	SetTexture(TutorialMessageTexture[num]);
}

////////////////////
 //UIマネージャー
//////////////////

//*****************************************************************************
// コンストラクタ関数
//*****************************************************************************
InGameUI2D::InGameUI2D()
{
	Init();
}

InGameUI2D::~InGameUI2D()
{
	End();
}

//*****************************************************************************
//Init関数
//初期化関数
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::Init()
{
	pPlayerHealth = new UIObject2D(UI_AIM);
	pPlayerAim = new UIObject2D(UI_HEART);
	pAtkEffect = new UIObject2D(UI_ATKZOOM);
	for (int i = 0; i < MAX_AURA; i++)
		pAuraEffects[i]=new UIObject2D(UI_AURA);
	pGameOverScreen = new UIObject2D(UI_GAMEOVER_SCREEN);
	pScore = new UIObject2D(UI_SCORE);
	pTimer = new UIObject2D(UI_TIMER);
	pRankMeter = new UIObject2D(UI_RANK);
	pScoreText = new UIObject2D(UI_SCORETEXT);
	pTutorialMessage = new UIObject2D(UI_TUTORIAL_MESSAGE);
	pPause[0] = new UIObject2D(UI_PAUSE_BLACK);
	pPause[1] = new UIObject2D(UI_PAUSE);
	bUseTutorialMessage = true;
	nCurrentTutorialMessage = 0;
}

//*****************************************************************************
//Update関数
//変更関数
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::Update()
{
	if (pPlayerHealth)
		pPlayerHealth->Update();
	if (pPlayerAim)
		pPlayerAim->Update();
	if (pAtkEffect)
		pAtkEffect->Update();
	for (int i = 0; i < MAX_AURA; i++)
		if(pAuraEffects[i])pAuraEffects[i]->Update();
	if (pGameOverScreen)
		pGameOverScreen->Update();
	if (pScore)
		pScore->Update();	
	if (pRankMeter)
		pRankMeter->Update();	
	if (pTimer)
		pTimer->Update();	
	if (pScoreText)
		pScoreText->Update();	
	if (pTutorialMessage)
		pTutorialMessage->Update();
	for (int i = 0; i < 2; i++)
	{
	if (pPause[i])
		pPause[i]->Update();
	}

}

//*****************************************************************************
//Draw関数
//レンダリング関数
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::Draw()
{
	if (pPlayerHealth)
		pPlayerHealth->Draw();
	if (pPlayerAim)
		pPlayerAim->Draw();
	if (pAtkEffect)
		pAtkEffect->Draw();
	for (int i = 0; i < MAX_AURA; i++)
		if (pAuraEffects[i])pAuraEffects[i]->Draw();
	if (pGameOverScreen)
		pGameOverScreen->Draw();
	if (pScore)
		pScore->Draw();
	if (pRankMeter)
		pRankMeter->Draw();	
	if (pTimer)
		pTimer->Draw();	
	if (pScoreText)
		pScoreText->Draw();	
	if (IsGamePaused()) {
		for (int i = 0; i < 2; i++)
		{
			if (pPause[i])
				pPause[i]->Draw();
		}
	}
	if (pTutorialMessage && bUseTutorialMessage)
		pTutorialMessage->Draw();
}

//*****************************************************************************
//End関数
//終了関数
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::End()
{
	SAFE_DELETE(pPlayerHealth);
	SAFE_DELETE(pPlayerAim);
	for (int i = 0; i < MAX_AURA; i++)
		SAFE_DELETE(pAuraEffects[i]);
	SAFE_DELETE(pAtkEffect);
	SAFE_DELETE(pGameOverScreen);
	SAFE_DELETE(pScore);
	SAFE_DELETE(pTutorialMessage);
}

//*****************************************************************************
//ActivateAtkEffect関数
//攻撃動作のエフェクトを有効する
//引数：int
//戻：void
//*****************************************************************************
void InGameUI2D::ActivateAtkEffect(int Frames)
{
	if (pAtkEffect)
		pAtkEffect->SetActiveFrames(Frames);
}

//*****************************************************************************
//ActivateGameOver関数
//ゲームオーバーの画面を有効する
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::ActivateGameOver()
{

	if (pGameOverScreen) {
		if (pGameOverScreen->GetUse())
			return;
		StopSound();
		PlaySoundGame(SOUND_LABEL_GAMEOVER);
		PlaySoundGame(SOUND_LABEL_SE_MIRRORBREAK);
		pGameOverScreen->SetUse(true);
	}
}

//*****************************************************************************
//SetAura関数
//オーラーを設定する
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::SetAura()
{
	for (int i = 0; i < MAX_AURA; i++)
	{
		if (!pAuraEffects[i])
			continue;
		if (pAuraEffects[i]->GetUse())
			continue;
		pAuraEffects[i]->SetAura();
		pAuraEffects[i]->SetTexture(AuraTexture[NORMAL_AURA_TEXTURE]);
		return;
	}
}

//*****************************************************************************
//SetAura関数
//オーラーを設定する
//引数：int
//戻：void
//*****************************************************************************
void InGameUI2D::SetAura(int Texture)
{
	if (Texture > MAX_AURA_TEXTURE || Texture < 0)
		return;
	for (int i = 0; i < MAX_AURA; i++)
	{
		if (!pAuraEffects[i])
			continue;
		if (pAuraEffects[i]->GetUse())
			continue;
		pAuraEffects[i]->SetAura();
		pAuraEffects[i]->SetTexture(AuraTexture[Texture]);
		if (Texture == HEART_AURA_TEXTURE)
			pAuraEffects[i]->SetSpin(false);
		else
			if (Texture == HEART_AURA_TEXTURE)
				pAuraEffects[i]->SetSpin(true);
		return;
	}
}

//*****************************************************************************
//GetGameOverFrames関数
//ゲームオーバーのフレームを戻す
//引数：void
//戻：int
//*****************************************************************************
int InGameUI2D::GetGameOverFrames()
{
	if (pGameOverScreen)
		return pGameOverScreen->GetGameOverFrames();
	return 0;
}

//*****************************************************************************
//SetNextTutorialMessage関数
//チュートリアルのメッセージを設定する
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::SetNextTutorialMessage()
{
	nCurrentTutorialMessage++;
	if (nCurrentTutorialMessage >= TUTM_MAX)
		nCurrentTutorialMessage = 0;
	if (pTutorialMessage)
		pTutorialMessage->SetTutorialMessage(nCurrentTutorialMessage);
}

//*****************************************************************************
//SetNextTutorialMessage関数
//チュートリアルのメッセージを無効する
//引数：void
//戻：void
//*****************************************************************************
void InGameUI2D::DeactivateTutorial()
{
	bUseTutorialMessage = false;
}
