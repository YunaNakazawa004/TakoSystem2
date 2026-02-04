//=============================================================================
// 
// ゲーム [game.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "camera.h"
#include "light.h"

#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshring.h"

#include "player.h"
#include "sound.h"
#include "computer.h"
#include "pot.h"
#include "object.h"
#include "stage.h"
#include "esa.h"		// エサ
#include "fishes.h"
#include "watersurf.h"

#include "crosshair.h"	// クロスヘア
#include "time.h"
#include "ui_gaugeicon.h"
#include "ui_esa.h"

#include "map.h"

#include "effect_3d.h"
#include "particle_3d.h"
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "title.h"

#include "game.h"

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureGame = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffGame = NULL;	// 頂点バッファへのポインタ
bool g_bPause = false;						// ポーズ状態のON/OFF
int g_Stage = 0;		// 現在のステージ

int g_nPointOld[3];	// 前回のポイント

//===================================================================
// ゲーム画面の初期化処理
//===================================================================
void InitGame(void)
{
	srand((unsigned int)time(NULL));

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.7f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.4f, 0.5f, 0.7f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.1f, 0.1f, 0.3f, 0.3f));

	// カメラの初期化処理
	SetNumCamera(GetPlayerSelect());

	// プレイヤーの初期化処理
	InitPlayer();
	SetPlayer(0, D3DXVECTOR3(0.0f, 1500.0f, 500.0f), FIRST_POS);
	SetPlayer(1, D3DXVECTOR3(0.0f, 1500.0f, -500.0f), FIRST_POS);

	// CPUの初期化処理
	InitComputer();

	// ステージの初期化処理
	//InitStage();

	// 配置物の初期化処理
	InitObject("objpos.txt");

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, MESHCYLINDERTYPE_ROCK);
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(OUTCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true, MESHCYLINDERTYPE_SEA);

	// メッシュドームの初期化処理
	InitMeshDome();

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュリングの初期化処理
	InitMeshRing();
		
	// 3Dエフェクトの初期化処理
	InitEffect3D();

	// 3Dパーティクルの初期化処理
	InitParticle3D();

	// 生き物の初期化処理
	InitFishes();
	
	// タコつぼの初期化処理
	InitPot();

	// エサの初期化処理
	InitEsa();

	// 水面の初期化処理
	InitWaterSurf();

	// クロスヘアの初期化処理
	InitCrossHair();

	// UIゲージアイコンの初期化処理
	InitUiGaugeIcon();

	// エサUIの初期化処理
	InitUiEsa();

	// 時間の初期化処理
	InitTime();

	// 時間の初期設定
	SetTime(DEFAULT_TIME);

	// マップの初期化処理
	InitMap();

	// ポーズの初期化処理
	InitPause();

	// サウンドの再生
	PlaySound(SOUND_BGM_GAME);

	g_bPause = false;	// ポーズ解除
}

//===================================================================
// ゲーム画面の終了処理
//===================================================================
void UninitGame(void)
{
	// サウンドの停止
	StopSound();

	// プレイヤーの終了処理
	UninitPlayer();

	// CPUの終了処理
	UninitComputer();

	// ステージの終了処理
	//UninitStage();

	// 配置物の終了処理
	UninitObject();

	// メッシュシリンダーの終了処理
	UninitMeshCylinder();

	// メッシュドームの終了処理
	UninitMeshDome();

	// メッシュフィールドの終了処理
	UninitMeshField();

	// メッシュリングの終了処理
	UninitMeshRing();

	// 3Dエフェクトの終了処理
	UninitEffect3D();

	// 3Dパーティクルの終了処理
	UninitParticle3D();

	// 生き物の終了処理
	UninitFishes();

	// タコつぼの終了処理
	UninitPot();

	// エサの終了処理
	UninitEsa();

	// 水面の終了処理
	UninitWaterSurf();

	// クロスヘアの終了処理
	UninitCrossHair();

	// UIゲージアイコンの終了処理
	UninitUiGaugeIcon();
	
	// エサUIの終了処理
	UninitUiEsa();

	// 時間の終了処理
	UninitTime();

	// マップの終了処理
	UninitMap();

	// ポーズ終了処理
	UninitPause();

	// サウンドの終了処理
	//StopSound();
	//UninitSound();

	// フェードの終了処理
	//UninitFade();


}

//===================================================================
// ゲーム画面の更新処理
//===================================================================
void UpdateGame(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	if (pFade == FADE_NONE)
	{// フェードが何もしていない状態のみ発動
		if (GetKeyboardTrigger(DIK_P) || GetJoypadTrigger(0, JOYKEY_START) == true)
		{// ポーズの確認
			ResetPause();
			g_bPause = g_bPause ? false : true;
		}
	}

	if (g_bPause == true)
	{
		// ポーズの更新処理
		UpdatePause();
	}
	else
	{

		// プレイヤーの更新処理
		UpdatePlayer();

		// CPUの更新処理
		UpdateComputer();

		// ステージの更新処理
		//UpdateStage();

		// 配置物の更新処理
		UpdateObject();

		// メッシュシリンダーの更新処理
		UpdateMeshCylinder();

		// メッシュドームの更新処理
		UpdateMeshRing();

		// メッシュフィールドの更新処理
		UpdateMeshField();

		// メッシュリングの更新処理
		UpdateMeshRing();

		// 3Dエフェクトの更新処理
		UpdateEffect3D();

		// 3Dパーティクルの更新処理
		UpdateParticle3D();

		// 生き物の更新処理
		UpdateFishes();

		// タコつぼの更新処理
		UpdatePot();

		// エサの更新処理
		UpdateEsa();

		// 水面の更新処理
		UpdateWaterSurf();

		// クロスヘアの更新処理
		UpdateCrossHair();

		// UIゲージアイコンの更新処理
		UpdateUiGaugeIcon();

		// エサUIの更新処理
		UpdateUiEsa();

		// 時間の更新処理
		UpdateTime();

		// マップの更新処理
		UpdateMap();

	}

}

//===================================================================
// ゲーム画面の描画処理
//===================================================================
void DrawGame(void)
{
	// フォグの設定
	Player* pPlayer = GetPlayer();

	for (int nCntCamera = 0; nCntCamera < GetNumCamera(); nCntCamera++)
	{
		SetFog(D3DXCOLOR(0.0f, 0.1f, 0.2f, 1.0f), 1000.0f, pPlayer[nCntCamera].fFog);
	}

	// プレイヤーの描画処理
	DrawPlayer();

	// CPUの描画処理
	DrawComputer();

	// ステージの描画処理
	//DrawStage();

	// 配置物の描画処理
	DrawObject();

	// メッシュシリンダーの描画処理
	DrawMeshCylinder();

	// メッシュドームの描画処理
	DrawMeshDome();

	// メッシュフィールドの描画処理
	DrawMeshField();

	// メッシュリングの描画処理
	DrawMeshRing();

	// 3Dエフェクトの描画処理
	DrawEffect3D();

	// 3Dパーティクルの描画処理
	DrawParticle3D();

	// 生き物の描画処理
	DrawFishes();

	// タコつぼの描画処理
	DrawPot();

	// エサの描画処理
	DrawEsa();

	// 水面の描画処理
	DrawWaterSurf();

	// クロスヘアの描画処理
	DrawCrossHair();

	// UIゲージアイコンの描画処理
	DrawUiGaugeIcon();

	// エサUIの描画処理
	DrawUiEsa();

	// 時間の描画処理
	DrawTime();

	// マップの描画処理
	DrawMap();

	// ポーズ中の描画処理
	if (g_bPause == true) DrawPause();
}

//===================================================================
// ポーズの有効無効設定
//===================================================================
void SetEnablePause(bool bPause)
{
	g_bPause = bPause;
}

//===================================================================
// ポーズの取得
//===================================================================
bool GetPause(void)
{
	return g_bPause;	// ポーズの情報を返す
}