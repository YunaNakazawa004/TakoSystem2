//=============================================================================
// 
// タイトル [title.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "title.h"
#include "input.h"
#include "sound.h"
#include "light.h"
#include "fade.h"
#include "camera.h"
#include "game.h"
#include "object.h"
#include "meshcylinder.h"
#include "meshfield.h"
#include "waterSurf.h"
#include "computer.h"

// マクロ定義
#define	MAX_TITLE	(5)	// タイトルで表示するテクスチャの最大数
#define	RANKING_DELEY	(1500)	// ランキング移行に掛かる時間（25秒）
#define	CLEAR_DELEY	(60)	// 消滅にかかる時間
#define	TITLE_DELEY_MAX	(500.0f)	// タイトルの最大数

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTitle[MAX_TITLE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle = NULL;	// 頂点バッファへのポインタ

float g_TitleDeley;	// タイトル移動表示時間
int g_PressEnterDeley;	// PRESSENTER表示時間
int g_PlayerSelect = 1;	// プレイヤーの人数

// タイトルの初期化処理
void InitTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	g_TitleDeley = 0.0f;	// ディレイの値を初期化
	g_PressEnterDeley = 0;

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, MESHCYLINDERTYPE_ROCK);
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(OUTCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true, MESHCYLINDERTYPE_SEA);

	// メッシュフィールドの初期化処理
	InitMeshField();

	// 水面の初期化処理
	InitWaterSurf();

	// CPUの初期化処理
	InitComputer();

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.8f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.5f, 0.6f, 0.8f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.3f));

	// カメラの数の設定
	SetNumCamera(1);

	// 乱数の種を設定
	srand((unsigned int)time(0));

	int nCamera = rand() % 6;	// カメラの位置設定
	int nVecR = rand() % 5;		// カメラの角度設定

	// カメラの位置設定
	SetCameraPos(0, D3DXVECTOR3(0.0f, ((float)nCamera * 100.0f) + 600.0f, 0.0f), 
		D3DXVECTOR3(0.0f, (((float)nCamera * 100.0f) + 600.0f) + (((float)nVecR * 50.0f) - 100.0f), 0.0f),
		CAMERATYPE_POINT);

	// 配置物の初期化処理
	InitObject("data\\objpos001.txt");

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/TITLE.png",
		&g_pTextureTitle[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/PLAYER_SELECT000.png",
		&g_pTextureTitle[1]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/START002.png",
		&g_pTextureTitle[2]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/number000.png",
		&g_pTextureTitle[3]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/WPO.png",
		&g_pTextureTitle[4]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TITLE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{
		// 頂点座標の設定
		if (nCntTitle == 0)
		{// タイトル：ロゴ
			pVtx[0].pos = D3DXVECTOR3(160.0f, -540, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1120.0f, -540, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(160.0f, 0.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1120.0f, 0.0f, 0.0f);
		}
		else if (nCntTitle == 1)
		{// タイトル：プレイ人数
			pVtx[0].pos = D3DXVECTOR3(640.0f, 460.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1000.0f, 460.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(640.0f, 640.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1000.0f, 640.0f, 0.0f);
		}
		else if (nCntTitle == 2)
		{// タイトル：START
			pVtx[0].pos = D3DXVECTOR3(640.0f, 540.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1280.0f, 540.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(640.0f, 720.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);
		}
		else if (nCntTitle == 3)
		{// タイトル：プレイ数値
			pVtx[0].pos = D3DXVECTOR3(1080.0f, 480.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1150.0f, 480.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(1080.0f, 620.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1150.0f, 620.0f, 0.0f);
		}
		else
		{// タイトル：(C)WPO
			pVtx[0].pos = D3DXVECTOR3(0.0f, 690.0f, 0.0f);		// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(310.0f, 690.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(310.0f, 720.0f, 0.0f);
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
		if (nCntTitle == 3)
		{// タイトル：プレイ人数
			pVtx[0].tex = D3DXVECTOR2((g_PlayerSelect * 0.1f), 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.1f + (g_PlayerSelect * 0.1f), 0.0f);
			pVtx[2].tex = D3DXVECTOR2((g_PlayerSelect * 0.1f), 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.1f + (g_PlayerSelect * 0.1f), 1.0f);
		}
		else
		{
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();

	// サウンドの再生
	PlaySound(SOUND_BGM_TITLE);
}

// タイトルの終了処理
void UninitTitle(void)
{
	// サウンドの停止
	StopSound();

	// 配置物の終了処理
	UninitObject();

	// メッシュシリンダーの終了処理
	UninitMeshCylinder();

	// メッシュフィールドの終了処理
	UninitMeshField();

	// 水面の終了処理
	UninitWaterSurf();

	// CPUの終了処理
	UninitComputer();

	// テクスチャの破棄
	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{// タイトルの数だけ確認する
		if (g_pTextureTitle[nCntTitle] != NULL)
		{// テクスチャの破棄
			g_pTextureTitle[nCntTitle]->Release();
			g_pTextureTitle[nCntTitle] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTitle != NULL)
	{
		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}
}

// タイトルの更新処理
void UpdateTitle(void)
{
	// 配置物の更新処理
	UpdateObject();

	// メッシュシリンダーの更新処理
	UpdateMeshCylinder();

	// メッシュフィールドの更新処理
	UpdateMeshField();

	// 水面の更新処理
	UpdateWaterSurf();

	// CPUの更新処理
	UpdateComputer();

	// フェード情報の取得
	FADE pFade = GetFade();

	if (g_TitleDeley < TITLE_DELEY_MAX)
	{// 特定の位置まで繰り返す
		g_TitleDeley += 2.5f;
	}
	g_PressEnterDeley++;

	if ((GetKeyboardTrigger(DIK_A) || GetJoypadTrigger(0, JOYKEY_LEFT) ||
		GetJoypadStick(0, JOYKEY_LEFTSTICK_LEFT, NULL, NULL) == true)
		&& g_PlayerSelect > 1)
	{
		g_PlayerSelect--;
		g_PressEnterDeley %= 100;	// ランキング移行までの時間を短縮
		PlaySound(SOUND_SE_CURSORMOVE);
	}
	else if ((GetKeyboardTrigger(DIK_D) || GetJoypadTrigger(0, JOYKEY_RIGHT) ||
		GetJoypadStick(0, JOYKEY_LEFTSTICK_RIGHT, NULL, NULL) == true)
		&& g_PlayerSelect < MAX_PLAYER)
	{
		g_PlayerSelect++;
		g_PressEnterDeley %= 100;	// ランキング移行までの時間を短縮
		PlaySound(SOUND_SE_CURSORMOVE);
	}

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{
		// 頂点座標の設定
		if (nCntTitle == 0)
		{// タイトルのpVtx
			pVtx[0].pos = D3DXVECTOR3(160.0f, -540 + g_TitleDeley, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1120.0f, -540 + g_TitleDeley, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(160.0f, 0.0f + g_TitleDeley, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1120.0f, 0.0f + g_TitleDeley, 0.0f);
		}
		else if (nCntTitle == 2)
		{// タイトル：PRESS ENTER
			if (pFade == FADE_OUT && g_PressEnterDeley <= RANKING_DELEY)
			{// PRESSENTERをクリック
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
			}
			else if ((g_PressEnterDeley % 100) > CLEAR_DELEY)
			{// 一定間隔で消滅
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			}
			else
			{// それ以外
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
		else if (nCntTitle == 3)
		{
			pVtx[0].tex = D3DXVECTOR2((g_PlayerSelect * 0.1f), 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.1f + (g_PlayerSelect * 0.1f), 0.0f);
			pVtx[2].tex = D3DXVECTOR2((g_PlayerSelect * 0.1f), 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.1f + (g_PlayerSelect * 0.1f), 1.0f);
		}

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}

	if ((GetKeyboardTrigger(DIK_RETURN) == true || 
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true) &&
		pFade == FADE_NONE && g_TitleDeley == TITLE_DELEY_MAX)
	{// 決定キー（ENTERキー）が押された
		// モード設定
		PlaySound(SOUND_SE_DECISION);
		SetFade(MODE_TUTORIAL);
	}
	else if (pFade == FADE_NONE && g_PressEnterDeley > RANKING_DELEY)
	{// 時間経過でランキングへ移行
		SetFade(MODE_LOGO);
	}

	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true))
	{// 特定のキーを押すと即座に到着
		g_TitleDeley = TITLE_DELEY_MAX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();

}

// タイトルの描画処理
void DrawTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	for (int nCntCamera = 0; nCntCamera < GetNumCamera(); nCntCamera++)
	{
		SetFog(D3DXCOLOR(0.0f, 0.1f, 0.2f, 1.0f), 1000.0f, 0.0f, false);
	}

	//SetCamera(0);

	// 配置物の描画処理
	DrawObject();

	// メッシュシリンダーの描画処理
	DrawMeshCylinder();

	// メッシュフィールドの描画処理
	DrawMeshField();

	// 水面の描画処理
	DrawWaterSurf();

	// CPUの描画処理
	DrawComputer();

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTitle, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{// 敵の最大数まで繰り返す
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureTitle[nCntTitle]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTitle * 4, 2);
	}
}

// プレイヤーの人数を取得
int GetPlayerSelect(void)
{
	return g_PlayerSelect;
}