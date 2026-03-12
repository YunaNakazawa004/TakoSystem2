//=============================================================================
// 
// タイトル [title.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "title.h"
#include "input.h"
#include "sound.h"
#include "light.h"
#include "fade.h"
#include "camera.h"

#include "object.h"
#include "meshcylinder.h"
#include "meshfield.h"
#include "meshorbit.h"	// 消えない
#include "waterSurf.h"
#include "computer.h"
#include "spray.h"

// マクロ定義
#define	MAX_TITLE	(8)	// タイトルで表示するテクスチャの最大数
#define	RANKING_DELEY	(1500)	// ランキング移行に掛かる時間（25秒）
#define	CLEAR_DELEY	(60)	// 登場→消滅にかかる時間
#define	CURSOR_DELEY	(20)	// CURSOR移動にかかる時間
#define	ENTRY_DELEY	(90)	// 登場→消滅→再登場までにかかる時間
#define	TITLE_DELEY_MAX	(500.0f)	// タイトルの最大数

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTitle[MAX_TITLE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle = NULL;	// 頂点バッファへのポインタ

float g_TitleDeley;		// タイトル移動表示時間
int g_PressEnterDeley;	// PRESSENTER表示時間
int g_PlayerSelect = 1;	// プレイヤーの人数

//TITLECURSOR g_Cursor;
int g_CursorPos;	// カーソルの位置情報
int g_CursorDeley;	// カーソルの表示時間

bool g_bTestTitle = true;
bool g_CursorSwitch;	// カーソルの切り替え処理

const char* c_apFilenameTitle[] =
{
	"data/TEXTURE/TITLE.png",
	"data/TEXTURE/PLAYER_SELECT001.png",
	"data/TEXTURE/START004.png",
	"data/TEXTURE/number000.png",
	"data/TEXTURE/WPO.png",
	"data/TEXTURE/CURSOR.png",
	"data/TEXTURE/SELECT_CURSOR.png",
	"data/TEXTURE/SELECT_CURSOR.png",
};

//===================================================================
// タイトルの初期化処理
//===================================================================
void InitTitle(void)
{

	// デバイスへのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
		
	g_TitleDeley = 0.0f;		// ディレイの値を初期化	g_PressEnterDeley = 0;

	g_CursorPos = 0;			// カーソルの位置を初期化
	g_CursorDeley = 0;
	g_CursorSwitch = false;

	int nCamera = rand() % 6;	// カメラの位置設定
	int nVecR = rand() % 5;		// カメラの角度設定

	g_PressEnterDeley = 0;


	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameTitle / sizeof(c_apFilenameTitle[0]); nCntTexture++)
	{
		if (FAILED(D3DXCreateTextureFromFile(pDevice,
											 c_apFilenameTitle[nCntTexture],
											 &g_pTextureTitle[nCntTexture])))
		{// 読み込みに失敗

			continue;
		}
	}

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
			pVtx[0].pos = D3DXVECTOR3(300.0f, 460.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(600.0f, 460.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(300.0f, 640.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(600.0f, 640.0f, 0.0f);
		}
		else if (nCntTitle == 2)
		{// タイトル：START
			pVtx[0].pos = D3DXVECTOR3(320.0f, 540.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(960.0f, 540.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(320.0f, 720.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(960.0f, 720.0f, 0.0f);
		}
		else if (nCntTitle == 3)
		{// タイトル：プレイ数値
			pVtx[0].pos = D3DXVECTOR3(730.0f, 480.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(800.0f, 480.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(730.0f, 620.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(800.0f, 620.0f, 0.0f);
		}
		else if (nCntTitle == 4)
		{// タイトル：(C)WPO
			pVtx[0].pos = D3DXVECTOR3(0.0f, 690.0f, 0.0f);		// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(310.0f, 690.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(310.0f, 720.0f, 0.0f);
		}
		else if (nCntTitle == 5)
		{// カーソル
			pVtx[0].pos = D3DXVECTOR3(180.0f, 500.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(300.0f, 500.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(180.0f, 580.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(300.0f, 580.0f, 0.0f);
		}
		else if (nCntTitle == 6)
		{// 左人数カーソル
			pVtx[0].pos = D3DXVECTOR3(640.0f, 530.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(700.0f, 530.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(640.0f, 590.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(700.0f, 590.0f, 0.0f);
		}
		else
		{// 右人数カーソル
			pVtx[0].pos = D3DXVECTOR3(830.0f, 530.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(890.0f, 530.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(830.0f, 590.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(890.0f, 590.0f, 0.0f);
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
		else if (nCntTitle == 6)
		{// 左人数カーソル
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.5f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.5f, 1.0f);
		}
		else if (nCntTitle == 7)
		{// 右人数カーソル
			pVtx[0].tex = D3DXVECTOR2(0.5f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.5f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
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

	DebugADD();

	// カメラの数の設定
	SetNumCamera(1);

	// カメラの位置設定
	SetCameraPos(0,
				 D3DXVECTOR3(0.0f, ((float)nCamera * 100.0f) + 600.0f, 0.0f),
				 D3DXVECTOR3(0.0f, (((float)nCamera * 100.0f) + 600.0f) + (((float)nVecR * 50.0f) - 100.0f), 0.0f),
				 D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				 CAMERATYPE_POINT);

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.8f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.5f, 0.6f, 0.8f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.3f));

	// CPUの初期化処理
	InitComputer();

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();	DebugADD();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, true, MESHCYLINDERTYPE_ROCK, MESHCYLINDERSTATE_NONE);
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(OUTCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true, true, MESHCYLINDERTYPE_SEA, MESHCYLINDERSTATE_NONE);

	// メッシュフィールドの初期化処理
	InitMeshField();

	// 飛沫の初期化処理
	InitSpray();

	// メッシュオービットの初期化処理
	InitMeshOrbit();

	// 水面の初期化処理
	InitWaterSurf();
	SetWaterSurf({ 0.0f,CYLINDER_HEIGHT,0.0f }, { 0.0f,0.0f,0.0f }, { 64,64 }, { (8000.0f) / 64, (8000.0f) / 64 }, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f));

	// 配置物の初期化処理
	InitObject("data\\objpos001.txt");	// 必ず最後(メッシュ後)に初期化する

	// サウンドの再生
	PlaySound(SOUND_BGM_TITLE);	
}

//===================================================================
// タイトルの終了処理
//===================================================================
void UninitTitle(void)
{
	// サウンドの停止
	StopSound();

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

	// CPUの終了処理
	UninitComputer();

	// メッシュシリンダーの終了処理
	UninitMeshCylinder();

	// メッシュフィールドの終了処理
	UninitMeshField();

	// 飛沫の終了処理
	UninitSpray();

	// メッシュオービットの終了処理
	UninitMeshOrbit();

	// 配置物の終了処理
	UninitObject();

	// 水面の終了処理
	UninitWaterSurf();
}

//===================================================================
// タイトルの更新処理
//===================================================================
void UpdateTitle(void)
{
#ifdef _DEBUG
#if 0 // タイトルの遷移(F11で解除)

	if (GetKeyboardTrigger(DIK_F11)) g_bTestTitle = (g_bTestTitle == true) ? false : true;

	if (g_bTestTitle == true)
	{
		SetFade(MODE_LOGO);
	}
#endif 
#endif

	if (((GetJoypadStick(0, JOYKEY_LEFTSTICK_UP, NULL, NULL) == true) || 
		(GetJoypadStick(0, JOYKEY_LEFTSTICK_DOWN, NULL, NULL) == true) || 
		(GetJoypadStick(0, JOYKEY_LEFTSTICK_LEFT, NULL, NULL) == true) || 
		(GetJoypadStick(0, JOYKEY_LEFTSTICK_RIGHT, NULL, NULL) == true)) && g_CursorSwitch == false)
	{
		g_CursorDeley = CURSOR_DELEY;
		g_CursorSwitch = true;
	}
	else
	{
		g_CursorDeley++;
		if ((GetJoypadStick(0, JOYKEY_LEFTSTICK_UP, NULL, NULL) == false) &&
			(GetJoypadStick(0, JOYKEY_LEFTSTICK_DOWN, NULL, NULL) == false) &&
			(GetJoypadStick(0, JOYKEY_LEFTSTICK_LEFT, NULL, NULL) == false) &&
			(GetJoypadStick(0, JOYKEY_LEFTSTICK_RIGHT, NULL, NULL) == false))
		{
			g_CursorSwitch = false;
		}

	}

	// CPUの更新処理
	UpdateComputer();

	// メッシュシリンダーの更新処理
	UpdateMeshCylinder();

	// メッシュフィールドの更新処理
	UpdateMeshField();

	// 飛沫の更新処理
	UpdateSpray();

	// メッシュオービットの更新処理
	UpdateMeshOrbit();

	// 配置物の更新処理
	UpdateObject();

	// 水面の更新処理
	UpdateWaterSurf();

	if (g_TitleDeley < TITLE_DELEY_MAX)
	{// 特定の位置まで繰り返す
		g_TitleDeley += 2.5f;
	}
	g_PressEnterDeley++;

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
		else if (nCntTitle == 1)
		{// タイトル：PRESS ENTER
			if ((g_PressEnterDeley % ENTRY_DELEY) > CLEAR_DELEY && g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// 一定間隔で消滅
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			}
			else if (g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// カーソルが合った時
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{// それ以外
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			}
		}
		else if (nCntTitle == 2)
		{// タイトル：PRESS ENTER
			if (GetFade() == FADE_OUT && g_PressEnterDeley <= RANKING_DELEY)
			{// PRESSENTERをクリック
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
			}
			else if ((g_PressEnterDeley % ENTRY_DELEY) > CLEAR_DELEY && g_CursorPos == TITLECURSOR_PLAY_START)
			{// 一定間隔で消滅
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			}
			else if (g_CursorPos == TITLECURSOR_PLAY_START)
			{// カーソルが合った時
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{// それ以外
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			}
		}
		else if (nCntTitle == 3)
		{// プレイ人数
			pVtx[0].tex = D3DXVECTOR2((g_PlayerSelect * 0.1f), 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.1f + (g_PlayerSelect * 0.1f), 0.0f);
			pVtx[2].tex = D3DXVECTOR2((g_PlayerSelect * 0.1f), 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.1f + (g_PlayerSelect * 0.1f), 1.0f);

			if ((g_PressEnterDeley % ENTRY_DELEY) > CLEAR_DELEY && g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// 一定間隔で消滅
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			}
			else if (g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// カーソルが合った時
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{// それ以外
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			}
		}
		else if (nCntTitle == 5)
		{// カーソル
			if (g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// 人数設定
				pVtx[0].pos = D3DXVECTOR3(180.0f, 500.0f, 0.0f);	// 右回りで設定する
				pVtx[1].pos = D3DXVECTOR3(300.0f, 500.0f, 0.0f);	// 2Dの場合Zの値は0にする
				pVtx[2].pos = D3DXVECTOR3(180.0f, 580.0f, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(300.0f, 580.0f, 0.0f);
			}
			else if (g_CursorPos == TITLECURSOR_PLAY_START)
			{// ゲームスタート
				pVtx[0].pos = D3DXVECTOR3(180.0f, 580.0f, 0.0f);	// 右回りで設定する
				pVtx[1].pos = D3DXVECTOR3(300.0f, 580.0f, 0.0f);	// 2Dの場合Zの値は0にする
				pVtx[2].pos = D3DXVECTOR3(180.0f, 660.0f, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(300.0f, 660.0f, 0.0f);
			}
		}
		else if (nCntTitle == 6)
		{// 左カーソル
			if ((g_PressEnterDeley % ENTRY_DELEY) > CLEAR_DELEY && g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// 一定間隔で消滅
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			}
			else if (g_PlayerSelect == MAX_PLAYER && g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// プレイ人数1人
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{// それ以外
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			}
		}
		else if (nCntTitle == 7)
		{// 左カーソル
			if ((g_PressEnterDeley % ENTRY_DELEY) > CLEAR_DELEY && g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// 一定間隔で消滅
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			}
			else if (g_PlayerSelect == 1 && g_CursorPos == TITLECURSOR_PLAYER_SELECT)
			{// プレイ人数2人
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
			else
			{// それ以外
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
			}
		}

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();

	if ((GetKeyboardRepeat(DIK_W) || GetJoypadRepeat(0, JOYKEY_UP) ||
		(GetJoypadStick(0, JOYKEY_LEFTSTICK_UP, NULL, NULL) == true && 
			(GetFade() == FADE_NONE && g_CursorDeley % CURSOR_DELEY == 0))))
	{// カーソル下移動

		g_CursorPos--;

		if (g_CursorPos < 0) g_CursorPos = TITLECURSOR_PLAY_START;
		PlaySound(SOUND_SE_CURSORMOVE);	// 選択音
		if (GetFade() != FADE_OUT) g_PressEnterDeley = 0;	// ディレイリセット
	}
	else if ((GetKeyboardRepeat(DIK_S) || GetJoypadRepeat(0, JOYKEY_DOWN) ||
		(GetJoypadStick(0, JOYKEY_LEFTSTICK_DOWN, NULL, NULL) == true && 
		(GetFade() == FADE_NONE && g_CursorDeley % CURSOR_DELEY == 0))))
	{// カーソル上移動

		g_CursorPos++;

		if (g_CursorPos >= TITLECURSOR_MAX) g_CursorPos = TITLECURSOR_PLAYER_SELECT;
		PlaySound(SOUND_SE_CURSORMOVE);	// 選択音
		if (GetFade() != FADE_OUT) g_PressEnterDeley = 0;	// ディレイリセット
	}

	if (g_CursorPos == TITLECURSOR_PLAYER_SELECT)
	{
		if ((GetKeyboardPress(DIK_A) || GetJoypadPress(0, JOYKEY_LEFT) ||
			(GetJoypadStick(0, JOYKEY_LEFTSTICK_LEFT, NULL, NULL) == true) && 
			(g_CursorDeley % CURSOR_DELEY == 0)) && g_PlayerSelect > 1)
		{

			g_PlayerSelect--;

			PlaySound(SOUND_SE_CURSORMOVE);	// 選択音
			if (GetFade() != FADE_OUT) g_PressEnterDeley = 0;	// ディレイリセット
		}
		else if ((GetKeyboardPress(DIK_D) || GetJoypadPress(0, JOYKEY_RIGHT) ||
			(GetJoypadStick(0, JOYKEY_LEFTSTICK_RIGHT, NULL, NULL) == true) && 
			(g_CursorDeley % CURSOR_DELEY == 0)) && g_PlayerSelect < MAX_PLAYER)
		{

			g_PlayerSelect++;

			PlaySound(SOUND_SE_CURSORMOVE);	// 選択音
			if (GetFade() != FADE_OUT) g_PressEnterDeley = 0;	// ディレイリセット
		}
	}
	else if (g_CursorPos == TITLECURSOR_PLAY_START)
	{
		if ((GetKeyboardTrigger(DIK_RETURN) == true ||
			GetJoypadTrigger(0, JOYKEY_START) == true ||
			GetJoypadTrigger(0, JOYKEY_A) == true) &&
			GetFade() == FADE_NONE && g_TitleDeley == TITLE_DELEY_MAX)
		{// 決定キー（ENTERキー）が押された
			// モード設定
			PlaySound(SOUND_SE_DECISION);
			SetFade(MODE_TUTORIAL);
		}

	}

	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true))
	{// 特定のキーを押すと即座に到着
		g_TitleDeley = TITLE_DELEY_MAX;
	}

	if (GetFade() == FADE_NONE && g_PressEnterDeley > RANKING_DELEY)
	{// 時間経過でランキングへ移行

		SetFade(MODE_LOGO);
	}
}

//===================================================================
// タイトルの描画処理
//===================================================================
void DrawTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	for (int nCntCamera = 0; nCntCamera < GetNumCamera(); nCntCamera++)
	{
		SetFog(D3DXCOLOR(0.0f, 0.1f, 0.2f, 1.0f), 1000.0f, 0.0f, false);
	}

#if 1
	// 配置物の描画処理
	DrawObject();

	// CPUの描画処理
	DrawComputer();

	// メッシュシリンダーの描画処理
	DrawMeshCylinder();
#endif

	// メッシュフィールドの描画処理
	DrawMeshField();

	// 飛沫の描画処理
	DrawSpray();

	// メッシュオービットの描画処理
	DrawMeshOrbit();

	// 水面の描画処理
	DrawWaterSurf();


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

//===================================================================
// プレイヤーの人数を取得
//===================================================================
int GetPlayerSelect(void)
{
	return g_PlayerSelect;
}