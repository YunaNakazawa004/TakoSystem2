//=============================================================================
// 
// ゲーム [game.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "camera.h"
#include "crosshair.h"
#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshring.h"
#include "player.h"
#include "object.h"
#include "stage.h"
#include "esa.h"		// エサ
#include "time.h"
#include "fishes.h"
#include "light.h"

#include "game.h"

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureGame = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffGame = NULL;	// 頂点バッファへのポインタ
bool g_bPause = false;						// ポーズ状態のON/OFF
int g_Stage = 0;		// 現在のステージ

int g_nPointOld[3];	// 前回のポイント

// ゲーム画面の初期化処理
void InitGame(void)
{
	srand((unsigned int)time(NULL));

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.7f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.4f, 0.5f, 0.7f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.1f, 0.1f, 0.3f, 0.3f));

	// カメラの初期化処理
	SetNumCamera(MAX_PLAYER);

	// プレイヤーの初期化処理
	InitPlayer();
	SetPlayer(0, D3DXVECTOR3(0.0f, 10000.0f, 15000.0f), FIRST_POS);
	SetPlayer(1, D3DXVECTOR3(0.0f, 15000.0f, -15000.0f), FIRST_POS);

	// ステージの初期化処理
	InitStage();

	// 配置物の初期化処理
	InitObject("objpos.txt");

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(2000.0f, 17500.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), false);
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(18050.0f, 17500.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), true);

	// メッシュドームの初期化処理
	InitMeshDome();

	// メッシュフィールドの初期化処理
	InitMeshField();

	// メッシュリングの初期化処理
	InitMeshRing();

	// 生き物の初期化処理
	InitFishes();

	// エサの初期化処理
	InitEsa();

	// クロスヘアの初期化処理
	InitCrossHair();

	// 時間の初期化処理
	InitTime();

	// 時間の初期設定
	SetTime(DEFAULT_TIME);
}

// ゲーム画面の終了処理
void UninitGame(void)
{
	// プレイヤーの終了処理
	UninitPlayer();

	// ステージの終了処理
	UninitStage();

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

	// 生き物の終了処理
	UninitFishes();

	// エサの終了処理
	UninitEsa();

	// クロスヘアの終了処理
	UninitCrossHair();

	// 時間の終了処理
	UninitTime();

	// サウンドの終了処理
	//StopSound();
	//UninitSound();

	// フェードの終了処理
	//UninitFade();
}

// ゲーム画面の更新処理
void UpdateGame(void)
{
	// プレイヤーの更新処理
	UpdatePlayer();

	// ステージの更新処理
	UpdateStage();

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

	// 生き物の更新処理
	UpdateFishes();

	// エサの更新処理
	UpdateEsa();

	// クロスヘアの更新処理
	UpdateCrossHair();

	// 時間の更新処理
	UpdateTime();
}

// ゲーム画面の描画処理
void DrawGame(void)
{
	// フォグの設定
	Player* pPlayer = GetPlayer();

	for (int nCntCamera = 0; nCntCamera < GetNumCamera(); nCntCamera++)
	{
		SetFog(D3DXCOLOR(0.0f, 0.1f, 0.2f, 1.0f), 10000.0f, pPlayer[nCntCamera].fFog);
	}

	// プレイヤーの描画処理
	DrawPlayer();

	// ステージの描画処理
	DrawStage();

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

	// 生き物の描画処理
	DrawFishes();

	// エサの描画処理
	DrawEsa();

	// クロスヘアの描画処理
	DrawCrossHair();

	// 時間の描画処理
	DrawTime();

	//// ポーズ中の描画処理
	//if (g_bPause == true) DrawPause();
}

//// ポーズの有効無効設定
//void SetEnablePause(bool bPause)
//{
//	g_bPause = bPause;
//}
//
//// ポーズの取得
//bool GetPause(void)
//{
//	return g_bPause;	// ポーズの情報を返す
//}