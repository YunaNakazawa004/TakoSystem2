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
#include "meshorbit.h"

#include "seadust.h"
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
#include "oceancurrents.h"
#include "readygo.h"

#include "map.h"

#include "effect_3d.h"
#include "particle_3d.h"
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "screen.h"
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
	SetCameraPos(0, FIRST_POS, FIRST_POS, D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_PLAYER);
	SetCameraPos(1, FIRST_POS, FIRST_POS, D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_PLAYER);

	// メッシュオービットの初期化処理
	InitMeshOrbit();

	// プレイヤーの初期化処理
	InitPlayer();

	// CPUの初期化処理
	InitComputer();

	// ステージの初期化処理
	//InitStage();

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, MESHCYLINDERTYPE_ROCK);
	//SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(OUTCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true, MESHCYLINDERTYPE_SEA);

	// メッシュドームの初期化処理
	InitMeshDome();
	SetMeshDome(FIRST_POS, FIRST_POS, D3DXVECTOR2(16.0f,5.0f), OUTCYLINDER_RADIUS * 2.5f);

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュリングの初期化処理
	InitMeshRing();

	// 塵の初期化処理
	InitSeaDust();

	// 3Dエフェクトの初期化処理
	InitEffect3D();

	// 3Dパーティクルの初期化処理
	InitParticle3D();

	// 生き物の初期化処理
	InitFishes();
	
	// タコつぼの初期化処理
	InitPot();

	// エサの初期化処理
	InitEsa(true);

	// 水面の初期化処理
	InitWaterSurf();

	// 配置物の初期化処理
	InitObject("data\\objpos001.txt");

	// レディの初期化処理
	InitReady();

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

	// 海流の初期化処理
	InitOceanCurrents();

	// 画面の初期化処理
	InitScreen();

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
	StopSound();

	// メッシュオービットの終了処理
	UninitMeshOrbit();

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

	// 塵の終了処理
	UninitSeaDust();

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

	// レディの終了処理
	UninitReady();

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

	// 海流の終了処理
	UninitOceanCurrents();

	// 画面の終了処理
	UninitScreen();

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
	//FADE pFade = GetFade();
	bool bGameStart = GetGameStart();

	// ゲームの状態による処理
	switch (g_gameState)
	{
	case GAMESTATE_BEGIN:		// 開始状態

		if (FADE_NONE == GetFade())
		{// フェードの状態が何もない状態の場合

			g_gameState = GAMESTATE_NONE;
		}

		break;

	case GAMESTATE_NONE:		// 通常状態

		if (GetTime() < PINCH_TIME)
		{// 残り時間がピンチ時間を下回った

			g_gameState = GAMESTATE_LITTLETIME;	// 残り時間が少ない状態に設定
		}

		break;

	case GAMESTATE_LITTLETIME:	// 残り時間が少ない状態

		break;

	case GAMESTATE_TIMEOVER:	// 時間切れ状態

		// リザルトに情報を渡す
		GiveResultForGame();

		// フェードの設定
		SetFade(MODE_RESULT);

		g_gameState = GAMESTATE_END;	// 終了状態に設定

		break;

	case GAMESTATE_END:			// 終了状態



		break;
	}
	if (g_gameState == GAMESTATE_BEGIN)
	{// ゲームの状態が開始状態の場合

		return;	// 処理しない
	}

			}	
	// レディの更新処理
	UpdateReady(); FileLogPass("ready");

	if (bGameStart == true)
	{
		// ポーズ状態の切り替え
		if (GetKeyboardTrigger(DIK_P) || GetJoypadTrigger(0, JOYKEY_START) == true)
		{// ポーズの確認
			
			if (g_bPause == true)
			{// 現在のポーズの状態がポーズしている状態の場合
				
				PlaySound(SOUND_SE_DECISION);
			}
			else
			{// 現在のポーズの状態がポーズしてない状態の場合
				
				PlaySound(SOUND_SE_MUD);
			}

			StopSound(SOUND_BGM_GAME);
			
			ResetPause();
			
			g_bPause = g_bPause ? false : true;
		}

		if (g_bPause == true)
		{

			// ポーズの更新処理
			UpdatePause(); FileLogPass("pause");
		}
		else
		{
			// CPUの更新処理
			UpdateComputer(); FileLogPass("computer");

			// 水面の更新処理
			UpdateWaterSurf(); FileLogPass("waterserf");

			// クロスヘアの更新処理
			UpdateCrossHair(); FileLogPass("crosshair");

			// UIゲージアイコンの更新処理
			UpdateUiGaugeIcon(); FileLogPass("gaugeicon");

			// エサUIの更新処理
			UpdateUiEsa(); FileLogPass("uiesa");

			// 時間の更新処理
			UpdateTime(); FileLogPass("time");

			// 海流の更新処理
			UpdateOceanCurrents(); FileLogPass("ocean_c");
		}

	}


	if (g_bPause == false)
	{// ポーズしてない場合
	
		// プレイヤーの更新処理
		UpdatePlayer(); FileLogPass("player");

		// ステージの更新処理
		//UpdateStage();

		// 配置物の更新処理
		UpdateObject(); FileLogPass("object");

		// メッシュシリンダーの更新処理
		UpdateMeshCylinder(); FileLogPass("mesh_cy");

		// メッシュドームの更新処理
		UpdateMeshDome();
		UpdateMeshDome(); FileLogPass("mesh_do");

		// メッシュフィールドの更新処理

		UpdateMeshField(); FileLogPass("mesh_fi");

		// メッシュリングの更新処理
		UpdateMeshRing(); FileLogPass("mesh_ri");

		// 塵の更新処理
		UpdateSeaDust(); FileLogPass("setdest");

		// 3Dエフェクトの更新処理
		UpdateEffect3D(); FileLogPass("effect");

		// 3Dパーティクルの更新処理
		UpdateParticle3D(); FileLogPass("particle");

		// 生き物の更新処理
		UpdateFishes(); FileLogPass("fishee");

		// タコつぼの更新処理
		UpdatePot(); FileLogPass("pot");

		// エサの更新処理
		UpdateEsa(); FileLogPass("esa");

		// マップの更新処理
		UpdateMap(); FileLogPass("map");

		// メッシュオービットの更新処理
		UpdateMeshOrbit(); FileLogPass("obit");
	}

	// 画面の更新処理
	UpdateScreen(); FileLogPass("screen");
	

	FileLogPass("game=====");
}

//===================================================================
// ゲーム画面の描画処理
//===================================================================
void DrawGame(void)
{
	bool bGameStart = GetGameStart();

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

	// 塵の描画処理
	DrawSeaDust();

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

	// メッシュオービットの描画処理
	DrawMeshOrbit();

	// 水面の描画処理
	DrawWaterSurf();

	// レディの描画処理
	DrawReady();

	if (bGameStart == true)
	{
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
	}

	// 海流の描画処理
	DrawOceanCurrents();

	// 画面の描画処理
	DrawScreen();

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


//===================================================================
// ゲームの状態の設定
//===================================================================
void SetGameState(GAMESTATE setState)
{
	// ゲームの状態を設定
	g_gameState = setState;
}

//===================================================================
// ゲームの状態の所得
//===================================================================
GAMESTATE GetGameState(void)
{
	return g_gameState;	// 現在のゲームの状態を返す
}

//===================================================================
// リザルトに情報を渡す処理
//===================================================================
void GiveResultForGame(void)
{
	// 変数宣言 ===========================================

	// リザルトで渡す時の情報
	Player* pPlayer = GetPlayer();								// プレイヤーの情報
	Computer* pComputer = GetComputer();						// コンピューターの情報
	int aHaveQueue[MAX_PLAYER + MAX_COMPUTER][MAX_QUEUE] = {};	// 全員のキュー情報
	int nCntPlayer, nCntComputer;								// カウンター(人数)
	
	RESULT_TYPEPLAYER aTypePlayer[MAX_PLAYER + MAX_COMPUTER];	// プレイヤーの種類
	
	// ====================================================

	memset(&aTypePlayer[0], RESULT_PLAYER_NONE, sizeof aTypePlayer);	// プレイヤーの種類を初期化

	// プレイヤーのエサの獲得状況を獲得
	for (nCntPlayer = 0; nCntPlayer < GetNumCamera(); nCntPlayer++)
	{
		// エサキューの中身をコピー
		memcpy(&aHaveQueue[nCntPlayer][0], &pPlayer[nCntPlayer].esaQueue.nData[0], sizeof(int) * MAX_QUEUE);

		aTypePlayer[nCntPlayer] = RESULT_PLAYER_PLAYER;		// 種類をプレイヤーに設定
	}

	// コンピューターのエサの獲得状況を獲得
	for (nCntComputer = 0; nCntComputer < ALL_OCTO - GetNumCamera(); nCntComputer++)
	{
		// エサキューの中身をコピー
		memcpy(&aHaveQueue[nCntPlayer + nCntComputer][0], &pComputer[nCntComputer].esaQueue.nData[0], sizeof(int) * MAX_QUEUE);

		aTypePlayer[nCntPlayer + nCntComputer] = RESULT_PLAYER_COMPUTER;	// 種類をコンピューターに設定
	}

	// リザルトに値を渡す
	ReceiveResult(&aTypePlayer[0], &aHaveQueue[0][0], nCntPlayer + nCntComputer, MAX_QUEUE);
}