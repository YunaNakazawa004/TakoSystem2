//=============================================================================
// 
// チュートリアル [tutorial.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "tutorial.h"
#include "camera.h"
#include "sound.h"
#include "input.h"
#include "light.h"
#include "fade.h"
#include "model.h"
#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshring.h"
#include "meshorbit.h"
#include "computer.h"
#include "pot.h"
#include "object.h"
#include "stage.h"
#include "esa.h"		// エサ
#include "fishes.h"
#include "watersurf.h"
#include "crosshair.h"	// クロスヘア
#include "ui_gaugeicon.h"
#include "ui_esa.h"
#include "oceancurrents.h"
#include "seadust.h"
#include "map.h"
#include "effect_3d.h"
#include "particle_3d.h"
#include "title.h"
#include "tutorialtxt.h"
#include "readygo.h"
#include "spray.h"
#include "seaweed.h"
#include "bubble.h"
#include "spotlight.h"
#include "screen.h"
#include "ui_tutorial.h"
#include "foodnum.h"

// マクロ定義
#define	MAX_TUTORIAL	(2)	// タイトルで表示するテクスチャの最大数
#define	READY_COUNTER	(60 * 3)	// タイトルで表示するテクスチャの最大数

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTutorial[MAX_TUTORIAL] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTutorial = NULL;	// 頂点バッファへのポインタ
Player_Tutorial g_aPlayerTutorial[MAX_PLAYER];
bool g_bReady = false;
bool g_bOceanCurrent = false;
int g_nReadyCounter = 0;

//===================================================================
// チュートリアルの初期化処理
//===================================================================
void InitTutorial(void)
{
	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.7f, 0.9f, 1.0f, 0.7f));
	SetLightColor(1, D3DXCOLOR(0.4f, 0.5f, 0.7f, 0.6f));
	SetLightColor(2, D3DXCOLOR(0.1f, 0.1f, 0.3f, 0.3f));

	// スポットライトの設定
	InitSpotLight();

	// カメラの初期化処理
	SetNumCamera(GetPlayerSelect());
	SetCameraPos(0, FIRST_POS, FIRST_POS, D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_PLAYER);
	SetCameraPos(1, FIRST_POS, FIRST_POS, D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_PLAYER);

	// メッシュオービットの初期化処理
	InitMeshOrbit();

	// 泡の初期化
	InitBubble();

	// プレイヤーの初期化処理
	InitPlayer();
	if (GetPlayerSelect() == 1)
	{// 1人
		SetPlayer(0, D3DXVECTOR3(0.0f, 200.0f, -7500.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), MOTIONTYPE_NEUTRAL, PLAYERMODE_TUTORIAL, PLAYERSTATE_NORMAL);
	}
	else
	{// 2人
		SetPlayer(0, D3DXVECTOR3(200.0f, 200.0f, -7500.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), MOTIONTYPE_NEUTRAL, PLAYERMODE_TUTORIAL, PLAYERSTATE_NORMAL);
		SetPlayer(1, D3DXVECTOR3(-200.0f, 200.0f, -7500.0f), D3DXVECTOR3(0.0f, D3DX_PI, 0.0f), MOTIONTYPE_NEUTRAL, PLAYERMODE_TUTORIAL, PLAYERSTATE_NORMAL);
	}

	g_aPlayerTutorial[0].pos = FIRST_POS;
	g_aPlayerTutorial[1].pos = FIRST_POS;
	g_bReady = false;
	g_bOceanCurrent = false;
	g_nReadyCounter = 0;

	// CPUの初期化処理
	InitComputer();

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(16.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, true, MESHCYLINDERTYPE_ROCK, MESHCYLINDERSTATE_NONE);

	// メッシュドームの初期化処理
	InitMeshDome();
	SetMeshDome(FIRST_POS, FIRST_POS, D3DXVECTOR2(16.0f, 5.0f), OUTCYLINDER_RADIUS * 6.0f, true, MESHDOMETYPE_SKY);
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
	InitFishes();

	// タコつぼの初期化処理
	InitPot();


	// 水面の初期化処理
	InitWaterSurf();
	SetWaterSurf({ 0.0f,CYLINDER_HEIGHT,0.0f }, { 0.0f,0.0f,0.0f }, { 64,64 },
		{ (OUTCYLINDER_RADIUS * 6.0f * 2.0f) / 48, (OUTCYLINDER_RADIUS * 6.0f * 2.0f) / 48 },
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f));

	// 配置物の初期化処理
	InitObject("data\\objpos002.txt");

	// エサの初期化処理
	InitEsa(true);
	SetEsa(0, false, ESA_ACTTYPE_LAND, 0, D3DXVECTOR3(-100.0f, 0.0f, -4000.0f), FIRST_POS);
	SetEsa(1, false, ESA_ACTTYPE_LAND, 0, D3DXVECTOR3(150.0f, 200.0f, -3800.0f), FIRST_POS);
	SetEsa(2, false, ESA_ACTTYPE_LAND, 0, D3DXVECTOR3(-150.0f, 250.0f, -3600.0f), FIRST_POS);
	SetEsa(1, false, ESA_ACTTYPE_LAND, 0, D3DXVECTOR3(100.0f, 150.0f, -3400.0f), FIRST_POS);

	// 海藻の初期化処理
	InitSeaweed();



	// チュートリアルテキストの初期化処理
	InitTutorialTxt();

	// クロスヘアの初期化処理
	InitCrossHair();

	// UIゲージアイコンの初期化処理
	InitUiGaugeIcon();

	// エサUIの初期化処理
	InitUiEsa();

	// エサ上限の初期化処理
	InitFoodNum();

	// マップの初期化処理
	InitMap();

	// レディの初期化処理
	InitReady();

	// 海流の初期化処理
	InitOceanCurrents();

	InitUiTutorial();

	InitScreen();

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/In_the_sea000.jpg",
		&g_pTextureTutorial[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/TUTORIAL002.png",
		&g_pTextureTutorial[1]);

	//D3DXCreateTextureFromFile(pDevice,
	//	"data/TEXTURE/text_tmp_rule_explanation000.png",
	//	&g_pTextureTutorial[2]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TUTORIAL,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTutorial,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTutorial->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTutorial = 0; nCntTutorial < MAX_TUTORIAL; nCntTutorial++)
	{
		// 頂点座標の設定
		if (nCntTutorial == 0)
		{// 背景
			pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
		else if (nCntTutorial == 1)
		{// RESULTロゴ
			pVtx[0].pos = D3DXVECTOR3(460.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(820.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(460.0f, 180.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(820.0f, 180.0f, 0.0f);
		}
		//else
		//{// RESULTロゴ

		//	if (GetPlayerSelect() == 1)
		//	{// 1人プレイ時

		//		pVtx[0].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 470.0f) - 140.0f, ((SCREEN_HEIGHT / 2) - 150.0f) - 180.0f, 0.0f);	// 右回りで設定する
		//		pVtx[1].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 470.0f) + 140.0f, ((SCREEN_HEIGHT / 2) - 150.0f) - 180.0f, 0.0f);	// 2Dの場合Zの値は0にする
		//		pVtx[2].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 470.0f) - 140.0f, ((SCREEN_HEIGHT / 2) - 150.0f) + 180.0f, 0.0f);	// (位置(中央 +- 中央からずらす量)) +- (大きさ)
		//		pVtx[3].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 470.0f) + 140.0f, ((SCREEN_HEIGHT / 2) - 150.0f) + 180.0f, 0.0f);
		//	}
		//	else
		//	{
		//		pVtx[0].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 0.0f) - 110.0f, ((SCREEN_HEIGHT / 2) + 160.0f) - 145.0f, 0.0f);	// 右回りで設定する
		//		pVtx[1].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 0.0f) + 110.0f, ((SCREEN_HEIGHT / 2) + 160.0f) - 145.0f, 0.0f);	// 2Dの場合Zの値は0にする
		//		pVtx[2].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 0.0f) - 110.0f, ((SCREEN_HEIGHT / 2) + 160.0f) + 145.0f, 0.0f);
		//		pVtx[3].pos = D3DXVECTOR3(((SCREEN_WIDTH / 2) - 0.0f) + 110.0f, ((SCREEN_HEIGHT / 2) + 160.0f) + 145.0f, 0.0f);
		//	}
		//}

		// rhwの設定
		pVtx[0].rhw = DEFAULT_RHW;	// 値は1.0fで固定
		pVtx[1].rhw = DEFAULT_RHW;
		pVtx[2].rhw = DEFAULT_RHW;
		pVtx[3].rhw = DEFAULT_RHW;

		// 頂点カラーの設定
		pVtx[0].col = WHITE_VTX;	// 0~255の値を設定
		pVtx[1].col = WHITE_VTX;
		pVtx[2].col = WHITE_VTX;
		pVtx[3].col = WHITE_VTX;

		// UV座標設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffTutorial->Unlock();

	// サウンドの再生
	PlaySound(SOUND_BGM_TUTORIAL);
}

//===================================================================
// リザルトの終了処理
//===================================================================
void UninitTutorial(void)
{
	// サウンドの停止
	StopSound();

	// スポットライトの終了処理
	UninitSpotLight();

	// プレイヤーの終了処理
	UninitPlayer();

	// CPUの終了処理
	UninitComputer();

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
	UninitFishes();

	// タコつぼの終了処理
	UninitPot();

	// エサの終了処理
	UninitEsa();

	// メッシュオービットの終了処理
	UninitMeshOrbit();

	// 水面の終了処理
	UninitWaterSurf();

	// 配置物の終了処理
	UninitObject();

	// 海藻の終了処理
	UninitSeaweed();

	// 泡の終了
	UninitBubble();

	// チュートリアルテキストの終了処理
	UninitTutorialTxt();

	// クロスヘアの終了処理
	UninitCrossHair();

	// UIゲージアイコンの終了処理
	UninitUiGaugeIcon();

	// エサUIの終了処理
	UninitUiEsa();

	// エサ上限の終了処理
	UninitFoodNum();

	// マップの終了処理
	UninitMap();

	// レディの終了処理
	UninitReady();

	// 海流の終了処理
	UninitOceanCurrents();

	UninitUiTutorial();

	UninitScreen();

	// テクスチャの破棄
	for (int nCntTutorial = 0; nCntTutorial < MAX_TUTORIAL; nCntTutorial++)
	{// タイトルの数だけ確認する
		if (g_pTextureTutorial[nCntTutorial] != NULL)
		{// テクスチャの破棄
			g_pTextureTutorial[nCntTutorial]->Release();
			g_pTextureTutorial[nCntTutorial] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTutorial != NULL)
	{
		g_pVtxBuffTutorial->Release();
		g_pVtxBuffTutorial = NULL;
	}

}

//===================================================================
// リザルトの更新処理
//===================================================================
void UpdateTutorial(void)
{
	// スポットライトの更新処理
	UpdateSpotLight();

	// プレイヤーの更新処理
	UpdatePlayer();

	// CPUの更新処理
	UpdateComputer();

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

	// 生き物の更新処理
	UpdateFishes();

	// タコつぼの更新処理
	UpdatePot();

	// エサの更新処理
	UpdateEsa();

	// メッシュオービットの更新処理
	UpdateMeshOrbit();

	// 水面の更新処理
	UpdateWaterSurf();

	// 配置物の更新処理
	UpdateObject();

	// 海藻の更新処理
	UpdateSeaweed();

	// 泡の更新
	UpdateBubble();

	// チュートリアルテキストの更新処理
	UpdateTutorialTxt();

	// クロスヘアの更新処理
	UpdateCrossHair();

	// UIゲージアイコンの更新処理
	UpdateUiGaugeIcon();

	// エサUIの更新処理
	UpdateUiEsa();

	// エサ上限の更新処理
	UpdateFoodNum();

	// マップの更新処理
	UpdateMap();

	// レディの更新処理
	UpdateReady();

	// 海流の更新処理
	UpdateOceanCurrents();
	if (GetOceanCurrents() == OCEANCURRENTSSTATE_WIRLPOOL && g_bOceanCurrent == false)
	{// 渦潮がきた
		g_bOceanCurrent = true;
	}

	UpdateUiTutorial();

	UpdateScreen();

	if (g_bOceanCurrent == true && GetOceanCurrents() == OCEANCURRENTSSTATE_NOMAL)
	{// カウンター加算
		g_nReadyCounter++;
	}

	// フェード情報の取得
	FADE pFade = GetFade();

	if ((GetSkipTutorial() == true && pFade == FADE_NONE) ||
		g_nReadyCounter > READY_COUNTER)
	{// 次の画面に転移する条件(SKIP長押し)を満たした

		Player* pPlayer = GetPlayer();
		for (int nCntPlayer = 0; nCntPlayer < GetNumCamera(); nCntPlayer++, pPlayer++)
		{
			if (pPlayer->mode == PLAYERMODE_GAME)
			{// マップ内に入っていたら
				g_aPlayerTutorial[nCntPlayer].pos = pPlayer->pos;
				g_aPlayerTutorial[nCntPlayer].rot = pPlayer->rot;
			}
		}

		if (g_bReady == false)
		{// 一回だけ
			SetReadyMove(0, { 500.0f, 360.0f, 0.0f }, { 640.0f, 360.0f, 0.0f }, 18, false);
			SetReady(0, 2, 0);

			g_bReady = true;
		}

		g_nReadyCounter = 0;
		g_bOceanCurrent = false;

		// モード設定
		//SetFade(MODE_GAME);
	}
}

//===================================================================
// リザルトの描画処理
//===================================================================
void DrawTutorial(void)
{
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

	// 泡の描画
	DrawBubble();

	// メッシュリングの描画処理
	DrawMeshRing();

	SetFog(WHITE_VTX, 0.0f, 0.0f, false);

	// チュートリアルテキストの描画処理
	DrawTutorialTxt();

	// 画面の描画
	DrawScreen();

	// クロスヘアの描画処理
	DrawCrossHair();

	// 海流の描画処理
	DrawOceanCurrents();

	// UIゲージアイコンの描画処理
	DrawUiGaugeIcon();

	// エサUIの描画処理
	DrawUiEsa();

	// エサ上限の描画処理
	DrawFoodNum();

	// マップの描画処理
	DrawMap();



	DrawUiTutorial();

	// レディの描画処理
	DrawReady();

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTutorial, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTutorial = 0; nCntTutorial < MAX_TUTORIAL; nCntTutorial++)
	{// 敵の最大数まで繰り返す
		// テクスチャのM
		pDevice->SetTexture(0, g_pTextureTutorial[nCntTutorial]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTutorial * 4, 2);
	}
}

//===================================================================
// チュートリアル終了時のプレイヤーの位置を取得
//===================================================================
Player_Tutorial GetPlayerTutorial(int nIdx)
{
	return g_aPlayerTutorial[nIdx];
}