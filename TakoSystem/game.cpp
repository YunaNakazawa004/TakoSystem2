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
#include "seaweed.h"
#include "stage.h"
#include "esa.h"		// エサ
#include "fishes.h"
#include "watersurf.h"
#include "tutorial.h"

#include "crosshair.h"	// クロスヘア
#include "time.h"
#include "ui_gaugeicon.h"
#include "ui_esa.h"
#include "foodnum.h"
#include "oceancurrents.h"
#include "readygo.h"

#include "map.h"

#include "effect_3d.h"
#include "particle_3d.h"
#include "bubble.h"

#include "spotlight.h"
#include "spray.h"
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "screen.h"
#include "title.h"
#include "result.h"

#include "game.h"

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureGame = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffGame = NULL;	// 頂点バッファへのポインタ
bool g_bPause = false;						// ポーズ状態のON/OFF
int g_Stage = 0;		// 現在のステージ

int g_nPointOld[3];	// 前回のポイント

GAMESTATE g_gameState = GAMESTATE_BEGIN;	// ゲームの状態

//===================================================================
// ゲーム画面の初期化処理
//===================================================================
void InitGame(void)
{
	srand((unsigned int)time(NULL));

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.7f, 0.9f, 1.0f, 0.7f));
	SetLightColor(1, D3DXCOLOR(0.4f, 0.5f, 0.7f, 0.6f));
	SetLightColor(2, D3DXCOLOR(0.1f, 0.1f, 0.3f, 0.3f));

	// スポットライトの設定
	InitSpotLight();

	// カメラの初期化処理
	SetNumCamera(GetPlayerSelect());
	//SetCameraPos(0, FIRST_POS, FIRST_POS, D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), CAMERATYPE_PLAYER);
	//SetCameraPos(1, FIRST_POS, FIRST_POS, D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_PLAYER);

	g_gameState = GAMESTATE_BEGIN;	// ゲームの状態を開始状態に設定

	// メッシュオービットの初期化処理
	InitMeshOrbit(); 

	// 泡の初期化
	InitBubble();

	// プレイヤーの初期化処理
	InitPlayer();
	if (GetNumCamera() == 1)
	{// 1人
		SetPlayer(0, GetPlayerTutorial(0).pos, D3DXVECTOR3(0.0f, GetPlayerTutorial(0).rot.y, 0.0f), MOTIONTYPE_NEUTRAL, PLAYERMODE_GAME);
	}
	else
	{// 2人
		SetPlayer(0, GetPlayerTutorial(0).pos, D3DXVECTOR3(0.0f, GetPlayerTutorial(0).rot.y, 0.0f), MOTIONTYPE_NEUTRAL, PLAYERMODE_GAME);
		SetPlayer(1, GetPlayerTutorial(1).pos, D3DXVECTOR3(0.0f, GetPlayerTutorial(1).rot.y, 0.0f), MOTIONTYPE_NEUTRAL, PLAYERMODE_GAME);
	}

	// CPUの初期化処理
	InitComputer(); 
	SetRandomComputer(ALL_OCTO - GetNumCamera());

	// ステージの初期化処理
	//InitStage();

	// メッシュシリンダーの初期化処理
	InitMeshCylinder(); 
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(16.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, true, MESHCYLINDERTYPE_ROCK, MESHCYLINDERSTATE_NONE);
	//SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(OUTCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true, MESHCYLINDERTYPE_SEA);

	// メッシュドームの初期化処理
	InitMeshDome();
	SetMeshDome(FIRST_POS, FIRST_POS, D3DXVECTOR2(16.0f,5.0f), OUTCYLINDER_RADIUS * 2.5f, true, MESHDOMETYPE_SKY);
	SetMeshDome(D3DXVECTOR3(0.0f, CYLINDER_HEIGHT, 0.0f), FIRST_POS,
		D3DXVECTOR2(16.0f, 5.0f), INCYLINDER_RADIUS, false, MESHDOMETYPE_ROCK);

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュリングの初期化処理
	InitMeshRing(); 

	// 塵の初期化処理
	InitSeaDust();

	// 飛沫の初期化処理
	InitSpray();

	// 3Dエフェクトの初期化処理
	InitEffect3D(); 

	// 3Dパーティクルの初期化処理
	InitParticle3D(); 

	// 生き物の初期化処理
	//InitFishes();
	
	// タコつぼの初期化処理
	InitPot();

	// 水面の初期化処理
	InitWaterSurf();
	SetWaterSurf({ 0.0f,CYLINDER_HEIGHT,0.0f }, { 0.0f,0.0f,0.0f }, { 64,64 }, { (8000.0f) / 64, (8000.0f) / 64 }, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f));

	// 配置物の初期化処理
	InitObject("data\\objpos001.txt");

	// エサの初期化処理
	InitEsa(true);

	// 海藻の初期化処理
	InitSeaweed();

	// レディの初期化処理
	InitReady(); 
	SetReadyMove(0, { 500.0f, 360.0f, 0.0f }, { 640.0f, 360.0f, 0.0f }, 18, false);
	SetReady(0, 4);

	// クロスヘアの初期化処理
	InitCrossHair();

	// UIゲージアイコンの初期化処理
	InitUiGaugeIcon();

	// エサUIの初期化処理
	InitUiEsa(); 

	// エサ上限の初期化処理
	InitFoodNum();

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
	g_bPause = false;	// ポーズ解除

	// サウンドの停止
	StopSound();

	// スポットライトの終了処理
	UninitSpotLight();

	// プレイヤーの終了処理
	UninitPlayer();

	// CPUの終了処理
	UninitComputer();

	// 配置物の終了処理
	UninitObject();

	// 海藻の終了処理
	UninitSeaweed();

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

	// 飛沫の終了処理
	UninitSpray();

	// 3Dエフェクトの終了処理
	UninitEffect3D();

	// 3Dパーティクルの終了処理
	UninitParticle3D();

	// 生き物の終了処理
	//UninitFishes();

	// タコつぼの終了処理
	UninitPot();

	// エサの終了処理
	UninitEsa();

	// メッシュオービットの終了処理
	UninitMeshOrbit();

	// 泡の終了
	UninitBubble();

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

	// エサ上限の終了処理
	UninitFoodNum();

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
}

//===================================================================
// ゲーム画面の更新処理
//===================================================================
void UpdateGame(void)
{
	// スポットライトの更新処理
	UpdateSpotLight();

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

	// レディの更新処理
	UpdateReady();

#ifdef _DEBUG
#if 0
	if (bGameStart == true)
	{// ゲーム開始状態

		SetFade(MODE_TUTORIAL);	// ゲームにフェード
	}
#endif
#endif

	if (bGameStart == true)
	{// ゲーム開始状態の場合

		// ポーズ状態の切り替え
		if (GetKeyboardTrigger(DIK_P) || GetJoypadTrigger(0, JOYKEY_START) == true)
		{// ポーズの確認
			
			if (g_bPause == true)
			{// 現在のポーズの状態がポーズしている状態の場合
				
				PlaySound(SOUND_SE_DECISION);	// 決定音を鳴らす

				PlaySound(SOUND_BGM_GAME);		// ゲームのBGMを鳴らす
			}
			else
			{// 現在のポーズの状態がポーズしてない状態の場合
				
				PlaySound(SOUND_SE_MUD);		// 水の音を鳴らす

				StopSound(SOUND_BGM_GAME);		// ゲームのBGMを止める
			}

			// 現在のポーズの状態を切り替える
			g_bPause = g_bPause ? false : true;
			
			// ポーズのリセット
			ResetPause();
		}
	}

#if 1 
	if (g_bPause == true)
	{// ポーズしている場合

		// ポーズの更新処理
		UpdatePause();
	}
	else
	{// ポーズしていない場合

#if 1
		if (bGameStart == true)
		{
			// CPUの更新処理
			UpdateComputer(); 

			// 水面の更新処理
			UpdateWaterSurf(); 

			// クロスヘアの更新処理
			UpdateCrossHair();

			// UIゲージアイコンの更新処理
			UpdateUiGaugeIcon();

			// エサUIの更新処理
			UpdateUiEsa(); 

			// エサ上限の更新処理
			UpdateFoodNum();

			// 時間の更新処理
			UpdateTime(); 

			// 海流の更新処理
			UpdateOceanCurrents();
		}

		// プレイヤーの更新処理
		UpdatePlayer(); 

		// ステージの更新処理
		//UpdateStage();

		// 配置物の更新処理
		UpdateObject();

		// 海藻の更新処理
		UpdateSeaweed();

		// メッシュシリンダーの更新処理
		UpdateMeshCylinder();

		// メッシュドームの更新処理
		UpdateMeshDome(); 

		// メッシュフィールドの更新処理
		UpdateMeshField();

		// メッシュリングの更新処理
		UpdateMeshRing(); 

		// 塵の更新処理
		UpdateSeaDust();

		// 飛沫の更新処理
		UpdateSpray();

		// 3Dエフェクトの更新処理
		UpdateEffect3D(); 

		// 3Dパーティクルの更新処理
		UpdateParticle3D();

		// タコつぼの更新処理
		UpdatePot();

		// エサの更新処理
		UpdateEsa();

		// マップの更新処理
		UpdateMap(); 

		// メッシュオービットの更新処理
		UpdateMeshOrbit();

		// 泡の更新
		UpdateBubble();
#endif
	}

	// 画面の更新処理
	UpdateScreen(); FileLogPass("screen");
	
	FileLogPass("e_game====");
#endif
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

	// 配置物の描画処理
	DrawObject();

	// 海藻の描画処理
	DrawSeaweed();

	// メッシュドームの描画処理
	DrawMeshDome();

	// メッシュフィールドの描画処理
	DrawMeshField();

	

	// メッシュシリンダーの描画処理
	DrawMeshCylinder();

	// 塵の描画処理
	DrawSeaDust();

	// 飛沫の描画処理
	DrawSpray();

	// 3Dエフェクトの描画処理
	DrawEffect3D();

	// 3Dパーティクルの描画処理
	DrawParticle3D();

	// 泡の描画
	DrawBubble();

	// タコつぼの描画処理
	DrawPot();

	// エサの描画処理
	DrawEsa();

	// メッシュオービットの描画処理
	DrawMeshOrbit();

	// メッシュリングの描画処理
	DrawMeshRing();

	// 水面の描画処理
	DrawWaterSurf();

	SetFog(WHITE_VTX, 0.0f, 0.0f, false);

	// レディの描画処理
	DrawReady();

	// 海流の描画処理
	DrawOceanCurrents();

	// UIゲージアイコンの描画処理
	DrawUiGaugeIcon();

	// エサUIの描画処理
	DrawUiEsa();

	// エサ上限の描画処理
	DrawFoodNum();

	// 時間の描画処理
	DrawTime();

	if (bGameStart == true)
	{
		// クロスヘアの描画処理
		DrawCrossHair();

		// マップの描画処理
		DrawMap();
	}

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