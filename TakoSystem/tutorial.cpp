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
#include "player.h"
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
#include "map.h"
#include "effect_3d.h"
#include "particle_3d.h"
#include "title.h"

// マクロ定義
#define	MAX_TUTORIAL	(2)	// タイトルで表示するテクスチャの最大数

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTutorial[MAX_TUTORIAL] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTutorial = NULL;	// 頂点バッファへのポインタ

// リザルトの初期化処理
void InitTutorial(void)
{
	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.7f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.4f, 0.5f, 0.7f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.1f, 0.1f, 0.3f, 0.3f));

	// カメラの初期化処理
	SetNumCamera(GetPlayerSelect());
	SetCameraPos(0, FIRST_POS, FIRST_POS, CAMERATYPE_PLAYER);
	SetCameraPos(1, FIRST_POS, FIRST_POS, CAMERATYPE_PLAYER);

	// プレイヤーの初期化処理
	InitPlayer();

	// CPUの初期化処理
	InitComputer();

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, MESHCYLINDERTYPE_ROCK);

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

	// メッシュオービットの初期化処理
	InitMeshOrbit();

	// 水面の初期化処理
	InitWaterSurf();

	// 配置物の初期化処理
	InitObject("data\\objpos001.txt");

	// クロスヘアの初期化処理
	InitCrossHair();

	// UIゲージアイコンの初期化処理
	InitUiGaugeIcon();

	// エサUIの初期化処理
	InitUiEsa();

	// マップの初期化処理
	InitMap();

	// 海流の初期化処理
	InitOceanCurrents();

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/In_the_sea.png",
		&g_pTextureTutorial[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/TUTORIAL.png",
		&g_pTextureTutorial[1]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TEXTURE,
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
		else
		{// RESULTロゴ
			pVtx[0].pos = D3DXVECTOR3(460.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(820.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(460.0f, 180.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(820.0f, 180.0f, 0.0f);
		}

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

// リザルトの終了処理
void UninitTutorial(void)
{
	// サウンドの停止
	StopSound();

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

	// クロスヘアの終了処理
	UninitCrossHair();

	// UIゲージアイコンの終了処理
	UninitUiGaugeIcon();

	// エサUIの終了処理
	UninitUiEsa();

	// マップの終了処理
	UninitMap();

	// 海流の終了処理
	UninitOceanCurrents();

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

// リザルトの更新処理
void UpdateTutorial(void)
{
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

	// クロスヘアの更新処理
	UpdateCrossHair();

	// UIゲージアイコンの更新処理
	UpdateUiGaugeIcon();

	// エサUIの更新処理
	UpdateUiEsa();

	// マップの更新処理
	UpdateMap();

	// 海流の更新処理
	UpdateOceanCurrents();

	// フェード情報の取得
	FADE pFade = GetFade();

	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true) && pFade == FADE_NONE)
	{// 決定キー（ENTERキー）が押された
		// モード設定
		SetFade(MODE_GAME);
	}
}

// リザルトの描画処理
void DrawTutorial(void)
{
	// プレイヤーの描画処理
	DrawPlayer();

	// CPUの描画処理
	DrawComputer();

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

	// メッシュオービットの描画処理
	DrawMeshOrbit();

	// 水面の描画処理
	DrawWaterSurf();

	// 配置物の描画処理
	DrawObject();

	// クロスヘアの描画処理
	DrawCrossHair();

	// UIゲージアイコンの描画処理
	DrawUiGaugeIcon();

	// エサUIの描画処理
	DrawUiEsa();

	// マップの描画処理
	DrawMap();

	// 海流の描画処理
	DrawOceanCurrents();

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