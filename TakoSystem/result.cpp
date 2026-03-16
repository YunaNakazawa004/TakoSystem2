//=============================================================================
// 
// リザルト [result.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "result.h"
#include "ranking.h"
#include "fade.h"

#include "camera.h"
#include "light.h"

#include "esa.h"

#include "file.h"
#include "debugproc.h"
#include "input.h"
#include "sound.h"

#include "ui_result_getscore.h"
#include "title.h"

// マクロ関数 ==================================================

#define FILENAME_RESULT			"data/FILE/result.txt"	// リザルトのファイル情報

#define	MAX_TEX_RESULT			(32)	// リザルトのテクスチャの最大数
#define	MAX_NUM_RESULT			(128)	// リザルトのポリゴンの最大数
#define	MAX_SET_RESULTINFO		(32)	// リザルト設定数の最大数

#define MAX_DROWLEVEL			(5)		// 描画順位の最大値

#define SETPOS_PLAYER			(D3DXVECTOR3(-167.0f, 85.0f, 0.0f))
#define SETARIA_PLAYER			(33.0f)

#define WAIT_SETING				(60 * 4.0f)

#define WAIT_BIGIN				(60 * 1)

// 構造体の定義 ================================================

// リザルトポリゴンの情報
typedef struct
{
	RESULTTYPE type;					 // 種類

	int nIdxTexture;			// テクスチャインデックス

	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度
	D3DXVECTOR3 addRot;			// 加算角度

	float fSizeWidth;			// 大きさ(幅)
	float fSizeHeight;			// 大きさ(高さ)

	D3DXCOLOR col;				// 色

	D3DXVECTOR2 texPos;			// テクスチャ(座標)
	D3DXVECTOR2 texSize;		// テクスチャ(サイズ)

	int nDrowLevel;				// 描画順位

	bool bAlphaBlend;			// aブレンドをするか

	bool bDisp;					// 表示状態
	bool bUse;					// 使用状態

}ResultPolygon;

// リザルトポリゴンの情報
typedef struct
{
	int Laytype;
	int type;					 // 種類

	int nIdxTexture;			// テクスチャインデックス
	bool bAlphaBlend;			// aブレンドをするか

	int nDrowLevel;				// 描画順位

	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度
	D3DXVECTOR3 addRot;			// 加算角度

	float fSizeWidth;			// 大きさ(幅)
	float fSizeHeight;			// 大きさ(高さ)

	D3DXVECTOR2 texPos;			// テクスチャ(座標)
	D3DXVECTOR2 texSize;		// テクスチャ(サイズ)

	D3DXCOLOR col;				// 色

}ResultPolygon_info;

// リザルトのエサの設定情報
typedef struct
{
	int nDrowLevel;						// 描画順位

	D3DXVECTOR3 pos;					// 位置
	float fRotSpeed;					// 回転速度(y)

	D3DXVECTOR3 esaRot[MAX_ESATYPE];	// エサの角度

}ResultEsa_info;

// リザルトのプレイヤー(テキスト)の設定情報
typedef struct
{
	D3DXVECTOR3 pos;		// 位置

	float fSpaceWidth;		// 開ける間隔(幅)
	float fSpaceHeight;		// 開ける間隔(高さ)

}ResultPlayer_info;

// グローバル変数 ==============================================

// テクスチャ
LPDIRECT3DTEXTURE9 g_apTextureResult[MAX_TEX_RESULT] = {};			// テクスチャへのポインタ
int g_aIdxTextureResult[MAX_TEX_RESULT];							// テクスチャのインデックス

// 頂点バッファ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResult = NULL;					// 頂点バッファへのポインタ

// ポリゴン
ResultPolygon g_aResultPolygon[MAX_NUM_RESULT];						// リザルトポリゴン情報
int g_nNumResultPolygon;											// リザルトポリゴンの総数
int g_aIdxResultPolygon[RESULTLAYTYPE_MAX][MAX_SET_RESULTINFO];		// リザルトポリゴンのインデックス

// リザルトの情報
RESULTSTATE g_resultState = RESULTSTATE_BEGIN;						// リザルトの状態
int g_nCounterResultState;											// リザルトの状態カウンター
bool g_bCompleteResultState;										// リザルトの状態で完了しているか
int g_nNowEsaType;													// 今表示しているエサの種類

int g_aIdxUiResultGS[MAX_PLAYER + MAX_COMPUTER] = {};				// リザルトの獲得スコアUIのインデックス(プレイヤー分)
GiveResultPlayer g_aGivePlayerR[MAX_PLAYER + MAX_COMPUTER];			// リザルトに渡す情報

int g_nSetingTime;													// 表示

D3DXVECTOR3 g_aUiResultPlayerPosion[MAX_PLAYER + MAX_COMPUTER];

// リザルトの設定情報
ResultPolygon_info g_aResultPolygonInfo[MAX_NUM_RESULT];			// ポリゴンの読み込んだ情報を格納
ResultEsa_info g_resulEsaInfo;										// エサの読み込んだ情報を格納
ResultPlayer_info g_resultPlayerInfo;								// プレイヤー(テキスト)の読み込んだ情報を格納
int g_aIdxResultInfo[RESULTLAYTYPE_MAX][MAX_SET_RESULTINFO];		// 読み込んだ種類別のポリゴンインデックス

int g_nSelectNum = 0;

// ゲームの情報
int g_nMaxPlayer;													// プレイヤーの総数
int g_nIdxSetEsa;													// エサの種類の総数
WORD g_aNumHaveEsa[MAX_PLAYER + MAX_COMPUTER][MAX_ESATYPE] = {};	// 持っているエサの数

//========================================================================
// リザルトの初期化処理
//========================================================================
void InitResult(void)
{
	// 変数宣言 ===========================================

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	int nCntResult;				// カウンタ

	// ====================================================

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.8f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.5f, 0.6f, 0.8f, 1.0f));
	SetLightColor(2, D3DXCOLOR(0.3f, 0.3f, 0.5f, 1.0f));

	// カメラの数の設定
	SetNumCamera(1);

	// カメラの位置設定
	SetCameraPos(0, D3DXVECTOR3(0.0f, 0.0f, 100.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_STOP);
	
	// エサの初期化
	InitEsa(false);	

	// リザルトの獲得スコアUIの初期化
	InitUiResultGetScore();
	
	// リザルトのポリゴン情報を初期化
	for (nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		g_aResultPolygon[nCntResult].type = RESULTTYPE_NULL;
		g_aResultPolygon[nCntResult].nIdxTexture = -1;								// テクスチャインデックスを初期化
		g_aResultPolygon[nCntResult].nDrowLevel = 0;								// 描画順位を初期化
		g_aResultPolygon[nCntResult].bAlphaBlend = false;							// aブレンドをしない状態に設定
		g_aResultPolygon[nCntResult].bDisp = false;									// 表示しない状態に設定
		g_aResultPolygon[nCntResult].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 位置を初期化
		g_aResultPolygon[nCntResult].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 角度を初期化
		g_aResultPolygon[nCntResult].addRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 加算角度を初期化
		g_aResultPolygon[nCntResult].fSizeWidth = 0.0f;								// 大きさ(幅)を初期化
		g_aResultPolygon[nCntResult].fSizeHeight = 0.0f;							// 大きさ(高さ)を初期化
		g_aResultPolygon[nCntResult].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);		// 色を初期化
		g_aResultPolygon[nCntResult].texPos = D3DXVECTOR2(0.0f, 0.0f);				// テクスチャ座標を初期化
		g_aResultPolygon[nCntResult].texSize = D3DXVECTOR2(0.0f, 0.0f);				// テクスチャサイズを初期化
		g_aResultPolygon[nCntResult].bDisp = false;									// 表示してない状態に設定
		g_aResultPolygon[nCntResult].bUse = false;									// 使用しない状態に設定
	}

	// リザルトのポリゴンの設定情報を初期化
	for (nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		g_aResultPolygonInfo[nCntResult].Laytype = -1;								// レイアウトの種類を初期化
		g_aResultPolygonInfo[nCntResult].type = RESULTLAYTYPE_NULL;					// レイアウトの種類を初期化
		g_aResultPolygonInfo[nCntResult].nIdxTexture = -1;							// テクスチャインデックスを初期化
		g_aResultPolygonInfo[nCntResult].nDrowLevel = 0;							// 描画順位を初期化
		g_aResultPolygonInfo[nCntResult].bAlphaBlend = false;						// aブレンドをしない状態に設定
		g_aResultPolygonInfo[nCntResult].pos = D3DXVECTOR3(0.0f, 0.0, 0.0f);		// 位置を初期化
		g_aResultPolygonInfo[nCntResult].rot = D3DXVECTOR3(0.0f, 0.0, 0.0f);		// 角度を初期化
		g_aResultPolygonInfo[nCntResult].addRot = D3DXVECTOR3(0.0f, 0.0, 0.0f);		// 加算角度を初期化
		g_aResultPolygonInfo[nCntResult].fSizeWidth = 0.0f;							// 大きさ(幅)を初期化
		g_aResultPolygonInfo[nCntResult].fSizeHeight = 0.0f;						// 大きさ(高さ)を初期化
		g_aResultPolygonInfo[nCntResult].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 色を初期化
		g_aResultPolygonInfo[nCntResult].texPos = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャ座標を初期化
		g_aResultPolygonInfo[nCntResult].texSize = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャサイズを初期化
	}
	
	
	// 状態
	g_resultState = RESULTSTATE_BEGIN;	// リザルトの状態を開始状態に設定
	g_nCounterResultState = WAIT_BIGIN;	// リザルトの状態カウンターを初期化
	g_bCompleteResultState = false;		// 完了していない状態に設定
	g_nSetingTime = 0;					// 表示時間を初期化

	// テクスチャインデックス
	memset(&g_aIdxTextureResult[0], -1, sizeof g_aIdxTextureResult);				// リザルトテクスチャのインデックスを初期化

	// レイアウトのポリゴンインデックスを初期化 
	for (int nCntLayType = 0; nCntLayType < RESULTLAYTYPE_MAX; nCntLayType++)
	{
		memset(&g_aIdxResultPolygon[nCntLayType][0], -1, sizeof g_aIdxResultPolygon[nCntLayType]);
	}

	// ポリゴン総数
	g_nNumResultPolygon = 0;			// リザルトポリゴンの総数を初期化
	
	// リザルトポリゴンのインデックスを初期化
	for (int nCntArray = 0; nCntArray < RESULTLAYTYPE_MAX; nCntArray++)
	{
		// 中身を初期化
		memset(&g_aIdxResultInfo[nCntArray][0], -1, sizeof g_aIdxResultInfo[nCntArray]);
	}

	// 表示エサ
	g_nNowEsaType = 0;					// 表示されてるエサの種類を初期化
	g_nIdxSetEsa = -1;					// 表示されてるエサのインデックスを初期化

	// スコアUIインデックス
	memset(&g_aIdxUiResultGS[0], -1, sizeof (int) * (MAX_PLAYER + MAX_COMPUTER));	// リザルトスコアUIのインデックスの初期化

	// プレイヤーUIのPOSを初期化
	for (int nCntArray = 0; nCntArray < (MAX_PLAYER + MAX_COMPUTER); nCntArray++)
	{
		g_aUiResultPlayerPosion[nCntArray] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	// リザルトの設定情報のインデックスを初期化
	for (int nCntArray = 0; nCntArray < RESULTLAYTYPE_MAX; nCntArray++)
	{
		// 中身を初期化
		memset(&g_aIdxResultInfo[nCntArray][0], -1, sizeof g_aIdxResultInfo[nCntArray]);
	}

	// エサの設定情報を初期化
	g_resulEsaInfo.nDrowLevel = 0;										// 描画順位を初期化
	g_resulEsaInfo.pos = D3DXVECTOR3(0.0f, 0.0, 0.0f);					// 位置を初期化
	g_resulEsaInfo.fRotSpeed = 0.0f;									// 回転速度を初期化
	memset(&g_resulEsaInfo.esaRot[0], 0, sizeof g_resulEsaInfo.esaRot);	// エサの角度を初期化

	// プレイヤー(テキスト)の設定情報を初期化
	g_resultPlayerInfo.pos = D3DXVECTOR3(0.0f, 0.0, 0.0f);				// 位置を初期化
	g_resultPlayerInfo.fSpaceWidth = 0.0f;								// 開ける間隔(幅)を初期化
	g_resultPlayerInfo.fSpaceHeight = 0.0f;								// 開ける間隔(高さ)を初期化

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_NUM_RESULT,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffResult,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		
		// 法線ベクトルの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

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
	g_pVtxBuffResult->Unlock();

#ifdef _DEBUG
#if 0	// 渡す値の確認用 *************************************************************

	// 変数宣言 =================================

	// 設定情報
	const int c_nNumPlayer = 4;						// プレイヤーの数
	const int c_nMaxHave = 10;						// エサの所持出来る最大数

	RESULT_TYPEPLAYER aPlayer[c_nNumPlayer] =
	{// プレイヤーの種類

		RESULT_PLAYER_PLAYER,				// [0]番目のプレイヤーは「プレイヤー」。
		RESULT_PLAYER_COMPUTER,				// [1]番目のプレイヤーは「コンピューター」。
		RESULT_PLAYER_COMPUTER,				// [2]番目のプレイヤーは「コンピューター」。
		RESULT_PLAYER_COMPUTER,				// [3]番目のプレイヤーは「コンピューター」。
	};

	int aHaveEsa[c_nNumPlayer][c_nMaxHave] =
	{// エサの種類情報

		{0, 3, 1, 1, 0, 0,-1,-1,-1,-1},	// [0]番目のプレイヤーのエサの所持状況
		{0, 2, 0,-1,-1,-1,-1,-1,-1,-1},	// [1]番目のプレイヤーのエサの所持状況
		{0, 3,-1,-1,-1,-1,-1,-1,-1,-1},	// [2]番目のプレイヤーのエサの所持状況
		{0, 4,-1,-1,-1,-1,-1,-1,-1,-1},	// [3]番目のプレイヤーのエサの所持状況
	};
	
	// 貰う場所
	GiveResultPlayer aResultPlayer[c_nNumPlayer];	// 判定したプレイヤーの情報を貰う場所
	
	// ==========================================

	// リザルトに情報を渡す処理
	ReceiveResult(&aPlayer[0], &aHaveEsa[0][0], c_nNumPlayer, c_nMaxHave);

	// リザルトの情報を渡す処理
	GetRankingForResult(&aResultPlayer[0], c_nNumPlayer);

#endif // *****************************************************************************
#endif

	// リザルトのファイル情報の読み取り
	SetLoadResult(FILENAME_RESULT);

	// リザルトの設定
	SetResult(RESULTLAYTYPE_INIT, -1);			// 初期の設定
	SetResult(RESULTLAYTYPE_PLAYER, -1);		// プレイヤーの設定
	SetResult(RESULTLAYTYPE_STARTTALLTING, -1);	// テキスト(集計開始)の設定
	//SetResult(RESULTLAYTYPE_ESA, 1);			// テキスト(集計開始)の設定

	// サウンドの再生
	PlaySound(SOUND_BGM_RESULT);
}

//========================================================================
// リザルトの終了処理
//========================================================================
void UninitResult(void)
{
	// ゲームの各情報を初期化
	for (int nCntResult = 0; nCntResult < MAX_PLAYER + MAX_COMPUTER; nCntResult++)
	{// プレイヤーの最大数分繰り返す

		memset(&g_aNumHaveEsa[nCntResult][0], 0, sizeof (WORD) * MAX_ESATYPE);	// エサの種類別所持数を初期化
	}

	g_nMaxPlayer = 0;											// プレイヤーの総数を初期化

	// エサの終了
	UninitEsa();

	// リザルトの獲得スコアUIの終了
	UninitUiResultGetScore();

	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_RESULT; nCntTexture++)
	{// タイトルの数だけ確認する

		// テクスチャの破棄
		if (g_apTextureResult[nCntTexture] != NULL)
		{
			g_apTextureResult[nCntTexture]->Release();
			g_apTextureResult[nCntTexture] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffResult != NULL)
	{
		g_pVtxBuffResult->Release();
		g_pVtxBuffResult = NULL;
	}

	// サウンドの停止
	StopSound();
}

//========================================================================
// リザルトの更新処理
//========================================================================
void UpdateResult(void)
{
	// 変数宣言 ======================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ
	
	// フェード情報の取得
	FADE pFade = GetFade();

	EsaData* pEsaData = GetEsaData(0);
	Esa* pEsa = GetEsa();

	float fWidth, fHeight;
	float fLength, fAngle;

	int nTmpIdx = 0;

	// ===============================================
#if 0
	SetFade(MODE_RANKING);
	return;
#endif
	

#if _DEBUG	// デバッグ


	// 演出スキップ
	PrintDebugProc("\n●SKIP[Oキー]-------\n");

	if (GetKeyboardTrigger(DIK_O))
	{// oキーが押された

		// モード設定
		SetFade(MODE_RANKING);
	}

#if 0
	// 選択する対象の設定
	if (GetKeyboardTrigger(DIK_1))
	{
		g_nSelectNum--;
	}
	if (GetKeyboardTrigger(DIK_2))
	{
		g_nSelectNum++;

		if (g_nSelectNum >= g_nNumResultPolygon)
		{
			g_nSelectNum = g_nNumResultPolygon - 1;
		}
	}

	// デバッグテキスト
	PrintDebugProc("\nNUM_POLYGON %d", g_nNumResultPolygon);
	PrintDebugProc("\nSELECT_POLYGON %d[1,2]", g_nSelectNum);

	if (g_nSelectNum != -1)
	{// インデックスがある場合

		// 位置の更新
		if (GetKeyboardPress(DIK_LSHIFT) == false)
		{// キーが押されていない場合(位置)

			FileMovePosion("data/FILE/tmpPosionResult.txt", &g_aResultPolygon[g_nSelectNum].pos, 1.0f, DIK_RIGHT, DIK_LEFT, DIK_DOWN, DIK_UP, NULL, NULL);
		}
		else
		{// キーが押されている場合(大きさ)

			if (GetKeyboardPress(DIK_UP))	 g_aResultPolygon[g_nSelectNum].fSizeHeight += 1.0f;
			if (GetKeyboardPress(DIK_DOWN))  g_aResultPolygon[g_nSelectNum].fSizeHeight -= 1.0f;
			if (GetKeyboardPress(DIK_LEFT))  g_aResultPolygon[g_nSelectNum].fSizeWidth  -= 1.0f;
			if (GetKeyboardPress(DIK_RIGHT)) g_aResultPolygon[g_nSelectNum].fSizeWidth  += 1.0f;
		}
	}
#endif
#endif

#if 1

	 //g_nCounterResultState--;	// リザルトの状態カウンタをデクリメント

	// リザルトの状態別処理
	switch(g_resultState)
	{
	case RESULTSTATE_BEGIN:

		if (g_bCompleteResultState == true)
		{// 完了した状態の場合

			g_nCounterResultState--;	// リザルトの状態カウンタをデクリメント
		}

		if (g_nCounterResultState <= 0)
		{// 状態カウンタが0になった

			// レイアウトの削除
			DelResult(RESULTLAYTYPE_STARTTALLTING);	// 集計開始レイアウト
			
			g_resultState = RESULTSTATE_SETING;		// 設定状態に設定

			g_bCompleteResultState = false;		// 完了していない状態に設定
		}
	
		break;

	case RESULTSTATE_WAIT:
	
		if (GetCompletGetScore(UIRESULTGS_STATE_END) == true)
		{// UIの設定が完了した

			g_nCounterResultState--;	// リザルトの状態カウンタをデクリメント
		}

		if (g_nCounterResultState <= 0)
		{// カウンタが0になった

			g_nNowEsaType++;							// 現在のエサの種類を増やす

			if (g_nNowEsaType < GetNumEsaType())
			{// 現在のエサの種類が総数より低い場合

				// エサの削除
				if (g_nIdxSetEsa != -1)
				{// エサのインデックスがある

					// レイアウトの削除
					DelResult(RESULTLAYTYPE_ESA);
				}

				// リザルトスコアUIの削除
				for (int nCntPlayer = 0; nCntPlayer < g_nMaxPlayer; nCntPlayer++)
				{
					// // リザルトスコアUIの削除
					DelUiResultGetScore(g_aIdxUiResultGS[nCntPlayer]);
				}

				g_resultState = RESULTSTATE_SETING;		// 状態を設定状態に設定
			}
			else
			{// 全て集計した

				g_resultState = RESULTSTATE_COMPLETE;	// 状態を完了した状態に設定
			}

			g_bCompleteResultState = false;		// 完了していない状態に設定
		}
		

		break;
	
	case RESULTSTATE_SETING:
	
		// プレイヤーの総数分設定
		for (int nCntPlayer = 0; nCntPlayer < g_nMaxPlayer; nCntPlayer++)
		{
			if (g_aNumHaveEsa[nCntPlayer][g_nNowEsaType] > 0)
			{// 1個でも獲得していた場合

				// リザルトスコアUIの設定
				g_aIdxUiResultGS[nCntPlayer] = SetUiResultGetScore(g_aUiResultPlayerPosion[nCntPlayer], D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
																   pEsaData[g_nNowEsaType].nScore, g_aNumHaveEsa[nCntPlayer][g_nNowEsaType]);
			}
		}

		// レイアウトの設定
		SetResult(RESULTLAYTYPE_ESA, g_nNowEsaType);
		
		g_resultState = RESULTSTATE_WAIT;		// 状態を待機状態に設定
		g_nCounterResultState = g_nSetingTime;	// 待機時間を設定

		g_bCompleteResultState = false;		// 完了していない状態に設定
		
		break;
	}
#endif

	// エサの角度を更新
	if (g_nIdxSetEsa != -1)
	{
		// 表示されているエサの角度の更新
		pEsa[g_nIdxSetEsa].rot.y += g_resulEsaInfo.fRotSpeed;

		// 角度の修正
		if (pEsa[g_nIdxSetEsa].rot.y < -D3DX_PI)
		{// -3.14を超えた場合

			pEsa[g_nIdxSetEsa].rot.y += D3DX_PI * 2.0f;
		}
		else if (pEsa[g_nIdxSetEsa].rot.y > D3DX_PI)
		{// 3.14を超えた場合

			pEsa[g_nIdxSetEsa].rot.y -= D3DX_PI * 2.0f;
		}
	}

	PrintDebugProc("\nESA IDX  %d", g_nIdxSetEsa);
	PrintDebugProc("\nESA NUW  %d", g_nNowEsaType);

	for (int nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		if (g_aResultPolygon[nCntResult].bUse == true)
		{// 使用している場合

			switch (g_aResultPolygon[nCntResult].type)
			{
			case RESULTTYPE_ST_BACK:

				g_aResultPolygon[nCntResult].fSizeWidth += 10.0f;	// 幅を加算

				if (g_aResultPolygon[nCntResult].fSizeWidth <= SCREEN_WIDTH * 0.5f)
				{// 幅が画面の幅より小さい場合

					
				}
				else
				{// 幅が画面の幅より大きい場合

					g_aResultPolygon[nCntResult].col.a -= 0.03f;	// 透明度を減らす

					if (g_aResultPolygon[nCntResult].col.a <= 0.0f)
					{// 透明になった

						g_aResultPolygon[nCntResult].col.a = 0.0f;	// 透明度を修正

						g_bCompleteResultState = true;				// 完了した状態に設定
					}					
				}

				nTmpIdx = nCntResult;

				break;

			case RESULTTYPE_ST_TEXT:

				if (nTmpIdx != -1)
				{// インデックスがある場合

					if (g_aResultPolygon[nTmpIdx].fSizeWidth > SCREEN_WIDTH * 0.5f)
					{// インデックスの幅が画面の幅より大きい場合

						g_aResultPolygon[nCntResult].col.a -= 0.02f;	// 透明度を減らす

						if (g_aResultPolygon[nCntResult].col.a <= 0.0f)
						{// 透明になった

							g_aResultPolygon[nCntResult].col.a = 0.0f;	// 透明度を修正
						}
					}
				}

				break;
			}

			// 加算角度を加算
			g_aResultPolygon[nCntResult].rot += g_aResultPolygon[nCntResult].addRot;

			// 角度の修正
			if		(g_aResultPolygon[nCntResult].rot.x < -D3DX_PI) { g_aResultPolygon[nCntResult].rot.x += D3DX_PI * 2.0f; }
			else if (g_aResultPolygon[nCntResult].rot.x >  D3DX_PI) { g_aResultPolygon[nCntResult].rot.x -= D3DX_PI * 2.0f; }

			if		(g_aResultPolygon[nCntResult].rot.y < -D3DX_PI) { g_aResultPolygon[nCntResult].rot.y += D3DX_PI * 2.0f; }
			else if (g_aResultPolygon[nCntResult].rot.y >  D3DX_PI) { g_aResultPolygon[nCntResult].rot.y -= D3DX_PI * 2.0f; }

			if		(g_aResultPolygon[nCntResult].rot.z < -D3DX_PI) { g_aResultPolygon[nCntResult].rot.z += D3DX_PI * 2.0f; }
			else if (g_aResultPolygon[nCntResult].rot.z >  D3DX_PI) { g_aResultPolygon[nCntResult].rot.z -= D3DX_PI * 2.0f; }

			fWidth = g_aResultPolygon[nCntResult].fSizeWidth;
			fHeight = g_aResultPolygon[nCntResult].fSizeHeight;

			fLength = sqrtf((fWidth * 2) * (fWidth * 2) + (fHeight * 2) * (fHeight * 2)) * 0.5f;
			fAngle = atan2f((fWidth * 2), (fHeight * 2));

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntResult * 4);		// 頂点データのポインタをインデックス分進める
			
			// 頂点座標の設定
			pVtx[0].pos.x = g_aResultPolygon[nCntResult].pos.x - sinf(fAngle - g_aResultPolygon[nCntResult].rot.y) * fLength;
			pVtx[1].pos.x = g_aResultPolygon[nCntResult].pos.x + sinf(fAngle + g_aResultPolygon[nCntResult].rot.y) * fLength;
			pVtx[2].pos.x = g_aResultPolygon[nCntResult].pos.x - sinf(fAngle + g_aResultPolygon[nCntResult].rot.y) * fLength;
			pVtx[3].pos.x = g_aResultPolygon[nCntResult].pos.x + sinf(fAngle - g_aResultPolygon[nCntResult].rot.y) * fLength;

			pVtx[0].pos.y = g_aResultPolygon[nCntResult].pos.y - cosf(fAngle - g_aResultPolygon[nCntResult].rot.y) * fLength;
			pVtx[1].pos.y = g_aResultPolygon[nCntResult].pos.y - cosf(fAngle + g_aResultPolygon[nCntResult].rot.y) * fLength;
			pVtx[2].pos.y = g_aResultPolygon[nCntResult].pos.y + cosf(fAngle + g_aResultPolygon[nCntResult].rot.y) * fLength;
			pVtx[3].pos.y = g_aResultPolygon[nCntResult].pos.y + cosf(fAngle - g_aResultPolygon[nCntResult].rot.y) * fLength;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// 頂点カラーの設定
			pVtx[0].col = g_aResultPolygon[nCntResult].col;
			pVtx[1].col = g_aResultPolygon[nCntResult].col;
			pVtx[2].col = g_aResultPolygon[nCntResult].col;
			pVtx[3].col = g_aResultPolygon[nCntResult].col;

			// UV座標設定
			pVtx[0].tex.x = g_aResultPolygon[nCntResult].texPos.x;
			pVtx[1].tex.x = g_aResultPolygon[nCntResult].texPos.x + g_aResultPolygon[nCntResult].texSize.x;
			pVtx[2].tex.x = g_aResultPolygon[nCntResult].texPos.x;
			pVtx[3].tex.x = g_aResultPolygon[nCntResult].texPos.x + g_aResultPolygon[nCntResult].texSize.x;

			pVtx[0].tex.y = g_aResultPolygon[nCntResult].texPos.y;
			pVtx[1].tex.y = g_aResultPolygon[nCntResult].texPos.y;
			pVtx[2].tex.y = g_aResultPolygon[nCntResult].texPos.y + g_aResultPolygon[nCntResult].texSize.y;
			pVtx[3].tex.y = g_aResultPolygon[nCntResult].texPos.y + g_aResultPolygon[nCntResult].texSize.y;

			// 頂点バッファをアンロックする
			g_pVtxBuffResult->Unlock();

#if 0
			PrintDebugProc("\nPOLYGON[%d]===", nCntResult);

			if (g_nSelectNum == nCntResult)
			{
				PrintDebugProc("\n  TEXIDX  %d", g_aResultPolygon[nCntResult].nIdxTexture);
				PrintDebugProc("\n  DRAWLV  %d", g_aResultPolygon[nCntResult].nDrowLevel);
				PrintDebugProc("\n  A_BREND [%s]", ((g_aResultPolygon[nCntResult].bAlphaBlend == true) ? "TRUE" : "FALSE"));
				PrintDebugProc("\n  POS     %f %f %f", g_aResultPolygon[nCntResult].pos.x, g_aResultPolygon[nCntResult].pos.y, g_aResultPolygon[nCntResult].pos.z);
				PrintDebugProc("\n  ROT     %f %f %f", g_aResultPolygon[nCntResult].rot.x, g_aResultPolygon[nCntResult].rot.y, g_aResultPolygon[nCntResult].rot.z);
				PrintDebugProc("\n  SIZE    %f %f ", g_aResultPolygon[nCntResult].fSizeWidth, g_aResultPolygon[nCntResult].fSizeHeight);
				PrintDebugProc("\n  COL     %f %f %f %f", g_aResultPolygon[nCntResult].col.r, g_aResultPolygon[nCntResult].col.g, g_aResultPolygon[nCntResult].col.b, g_aResultPolygon[nCntResult].col.a);
				PrintDebugProc("\n  TEXPOS  %f %f", g_aResultPolygon[nCntResult].texPos.x, g_aResultPolygon[nCntResult].texPos.y);
				PrintDebugProc("\n  TEXSIZE %f %f", g_aResultPolygon[nCntResult].texSize.x, g_aResultPolygon[nCntResult].texSize.y);
			}
#endif
		}
	}

	// リザルトの獲得スコアUIの更新
	UpdateUiResultGetScore();

	// 次のモードへの移動処理
#if 1
	if (pFade == FADE_NONE && g_resultState == RESULTSTATE_COMPLETE)
	{
		
		// モード設定
		SetFade(MODE_RANKING);
	}
#endif
}

//========================================================================
// リザルトの描画処理
//========================================================================
void DrawResult(void)
{
	for (int nCntLevel = 0; nCntLevel < MAX_DROWLEVEL; nCntLevel++)
	{// 描画レベル分繰り返す

		for (int nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
		{// リザルトの最大数だけ繰り返す

			if (g_aResultPolygon[nCntResult].bUse == true && g_aResultPolygon[nCntResult].bDisp == true)
			{// 使用されている && 表示されている

				if (g_aResultPolygon[nCntResult].nDrowLevel != nCntLevel)
				{// 描画レベルが同じでない場合

					continue;	// 処理の始めに戻る
				}

				// リザルトポリゴンの描画
				DrawResultPolygon(nCntResult);
			}
		}

		if (nCntLevel == g_resulEsaInfo.nDrowLevel)
		{
			// エサの描画
			DrawEsa();
		}

		if (nCntLevel == 3)
		{
			// リザルトの獲得スコアUIの描画
			DrawUiResultGetScore();
		}
	}

	
}

//========================================================================
// リザルトのポリゴン描画処理
//========================================================================
void DrawResultPolygon(int nIdxPolygon)
{
	// 変数宣言 ===========================================

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	D3DXMATRIX mtxWorld;						// 
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	// ====================================================

	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffResult, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	if (g_aResultPolygon[nIdxPolygon].nIdxTexture != -1)
	{// テクスチャがある場合

		pDevice->SetTexture(0, g_apTextureResult[g_aResultPolygon[nIdxPolygon].nIdxTexture]);
	}
	else
	{// テクスチャがない場合

		pDevice->SetTexture(0, NULL);
	}

	if (g_aResultPolygon[nIdxPolygon].bAlphaBlend == true)
	{// aブレンドをする場合

		// 減算合成の設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3
	}

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	// 描画形式
						   nIdxPolygon * 4,		// インデックス
						   2);					// ポリゴン数

	if (g_aResultPolygon[nIdxPolygon].bAlphaBlend == true)
	{// aブレンドをする場合

		// 減算合成の設定を元に戻す 
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
	}

	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}


//========================================================================
// リザルトの設定処理
//========================================================================
void SetResult(RESULTLAYTYPE Laytype, int nNum)
{
	// 変数宣言 ===========================================

	int nIdx = 0;
	
	D3DXVECTOR3 setPos      = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
				setShiftPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	D3DXVECTOR2 setTexPos  = D3DXVECTOR2(0.0f, 0.0f),
				setTexSize = D3DXVECTOR2(0.0f, 0.0f);

	D3DXCOLOR setCol = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

	float fShiftHeight = 0.0f;

	// ====================================================

	switch (Laytype)
	{
	case RESULTLAYTYPE_INIT:
	case RESULTLAYTYPE_STARTTALLTING:

		for (int nCntInfo = 0; nCntInfo < MAX_SET_RESULTINFO; nCntInfo++)
		{
			if (g_aIdxResultInfo[Laytype][nCntInfo] == -1)
			{// インデックスがない場合
				
				continue;
			}

			// インデックスを設定
			nIdx = g_aIdxResultInfo[Laytype][nCntInfo];

			// リザルトポリゴンの設定
			g_aIdxResultPolygon[Laytype][nCntInfo] = SetResultPolygon((RESULTTYPE)g_aResultPolygonInfo[nIdx].type,
																   g_aResultPolygonInfo[nIdx].nIdxTexture,
																   g_aResultPolygonInfo[nIdx].bAlphaBlend,
																   g_aResultPolygonInfo[nIdx].nDrowLevel,
																   g_aResultPolygonInfo[nIdx].pos,
																   g_aResultPolygonInfo[nIdx].rot,
																   g_aResultPolygonInfo[nIdx].addRot,
																   g_aResultPolygonInfo[nIdx].fSizeWidth,
																   g_aResultPolygonInfo[nIdx].fSizeHeight,
																   g_aResultPolygonInfo[nIdx].texPos,
																   g_aResultPolygonInfo[nIdx].texSize,
																   g_aResultPolygonInfo[nIdx].col);
		}

		break;

	case RESULTLAYTYPE_PLAYER:
	
		setShiftPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		
		for (int nCntPlayer = 0; nCntPlayer < g_nMaxPlayer; nCntPlayer++)
		{// プレイヤーの総数分繰り返す

			setShiftPos.x = 0.0f;				// ずらす幅の位置を初期化
			setShiftPos.y += fShiftHeight;		// ずらす高さを加算

			if (GetPlayerSelect() == 2 && nCntPlayer == 1)
			{
				setCol = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
			}
			else if (nCntPlayer == 0)
			{
				setCol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			}
			else
			{
				setCol = g_aResultPolygonInfo[nIdx].col;
			}

			for (int nCntInfo = 0; nCntInfo < MAX_SET_RESULTINFO; nCntInfo++)
			{// プレイヤーと数字の設定

				if (g_aIdxResultInfo[Laytype][nCntInfo] == -1)
				{// インデックスがない場合

					continue;
				}

				// インデックスを設定
				nIdx = g_aIdxResultInfo[Laytype][nCntInfo];

				// 位置を設定
				setPos.x = g_resultPlayerInfo.pos.x + setShiftPos.x + g_aResultPolygonInfo[nIdx].pos.x + g_aResultPolygonInfo[nIdx].fSizeWidth;
				setPos.y = g_resultPlayerInfo.pos.y + setShiftPos.y + g_aResultPolygonInfo[nIdx].pos.y + g_aResultPolygonInfo[nIdx].fSizeHeight;
				setPos.z = 0.0f;

				// テクスチャの座標を設定
				if (g_aResultPolygonInfo[nIdx].texSize.x != 1.0f)
				{// テクスチャサイズの幅が1.0でない場合

					setTexPos.x = (nCntPlayer + 1) * 0.1f;
					setTexPos.y = 0.0f;
				}
				else
				{// テクスチャサイズの幅が1.0の場合

					setTexPos = g_aResultPolygonInfo[nIdx].texPos;
				}

				// リザルトポリゴンの設定
				g_aIdxResultPolygon[Laytype][nCntInfo] = SetResultPolygon((RESULTTYPE)g_aResultPolygonInfo[nIdx].type,
																	   g_aResultPolygonInfo[nIdx].nIdxTexture,
																	   g_aResultPolygonInfo[nIdx].bAlphaBlend,
																	   g_aResultPolygonInfo[nIdx].nDrowLevel,
																	   setPos,
																	   g_aResultPolygonInfo[nIdx].rot,
																	   g_aResultPolygonInfo[nIdx].addRot,
																	   g_aResultPolygonInfo[nIdx].fSizeWidth,
																	   g_aResultPolygonInfo[nIdx].fSizeHeight, 
																	   setTexPos,
																	   g_aResultPolygonInfo[nIdx].texSize,
																	   setCol);

				// ずらす量を設定
				setShiftPos.x += g_aResultPolygonInfo[nIdx].fSizeWidth * 2.0f + g_resultPlayerInfo.fSpaceWidth;

				fShiftHeight = g_aResultPolygonInfo[nIdx].fSizeHeight * 2.0f + g_resultPlayerInfo.fSpaceHeight;
			}

			g_aUiResultPlayerPosion[nCntPlayer].x = g_resultPlayerInfo.pos.x + setShiftPos.x;
			g_aUiResultPlayerPosion[nCntPlayer].y = g_resultPlayerInfo.pos.y + setShiftPos.y + g_aResultPolygonInfo[nIdx].fSizeHeight;
			g_aUiResultPlayerPosion[nCntPlayer].z = 0.0f;
		}

		break;

	case RESULTLAYTYPE_ESA:

		if (nNum == -1)
		{// 値がない

			break;	// switchの処理をココで抜ける
		}

		// インデックスのInfo分繰り返す
		for (int nCntInfo = 0; nCntInfo < MAX_SET_RESULTINFO; nCntInfo++)
		{
			if (g_aIdxResultInfo[Laytype][nCntInfo] == -1)
			{// インデックスがない場合

				continue;
			}

			// インデックスを設定
			nIdx = g_aIdxResultInfo[Laytype][nCntInfo];

			// テクスチャの座標を設定
			if (g_aResultPolygonInfo[nIdx].texSize.y != 1.0f)
			{// テクスチャサイズの幅が1.0でない場合

				setTexPos.x = 0.0f;
				setTexPos.y = nNum * 0.143f;
			}
			else
			{// テクスチャサイズの幅が1.0の場合

				setTexPos = g_aResultPolygonInfo[nIdx].texPos;
			}

			// リザルトポリゴンの設定
			g_aIdxResultPolygon[Laytype][nCntInfo] = SetResultPolygon((RESULTTYPE)g_aResultPolygonInfo[nIdx].type,
																   g_aResultPolygonInfo[nIdx].nIdxTexture,
																   g_aResultPolygonInfo[nIdx].bAlphaBlend,
																   g_aResultPolygonInfo[nIdx].nDrowLevel,
																   g_aResultPolygonInfo[nIdx].pos,
																   g_aResultPolygonInfo[nIdx].rot,
																   g_aResultPolygonInfo[nIdx].addRot,
																   g_aResultPolygonInfo[nIdx].fSizeWidth,
																   g_aResultPolygonInfo[nIdx].fSizeHeight,
																   setTexPos,
																   g_aResultPolygonInfo[nIdx].texSize,
																   g_aResultPolygonInfo[nIdx].col);
		}

		// エサの設定
		g_nIdxSetEsa = SetEsa(nNum,								// 設定種類
							  false,							// オービット設定
							  ESA_ACTTYPE_NONE,					// 行動タイプ
							  0,								// 値
							  g_resulEsaInfo.pos,				// 位置
							  g_resulEsaInfo.esaRot[nNum]);		// 角度

		break;
	}
}

//========================================================================
// リザルトの消去処理
//========================================================================
void DelResult(RESULTLAYTYPE layType)
{
	// 変数宣言 ===========================================

	Esa* pEsa = GetEsa();	//　エサの情報

	int nIdx;				// インデックス

	// ====================================================

	for (int nCntInfo = 0; nCntInfo < MAX_SET_RESULTINFO; nCntInfo++)
	{
		if (g_aIdxResultPolygon[layType][nCntInfo] != -1)
		{// インデックスがある場合

			// インデックスを設定
			nIdx = g_aIdxResultPolygon[layType][nCntInfo];

			// インデックスのポリゴンを削除
			if (g_aResultPolygon[nIdx].bUse == true)
			{// 使用している

				g_aResultPolygon[nIdx].bUse = false;	// 使用していない状態に設定

				g_nNumResultPolygon--;					// ポリゴンの総数をデクリメント
			}
			else
			{// 使用していない

				continue;
			}
		}
	}

	switch (layType)
	{
	case RESULTLAYTYPE_INIT:
	case RESULTLAYTYPE_STARTTALLTING:

		break;

	case RESULTLAYTYPE_ESA:

		pEsa[g_nIdxSetEsa].bUse = false;	// インデックスのエサを使用していない状態に設定

		g_nIdxSetEsa = -1;					// インデックスがない状態に設定

		break;

	case RESULTLAYTYPE_PLAYER:



		break;
	}
}

//========================================================================
// リザルトのポリゴンの設定処理
//========================================================================
int SetResultPolygon(RESULTTYPE type, int nIdxTexture, bool bAlphaBlend, int nDrowLevel,
					 D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 addRot, float fSizeWidth, float fSizeHeight,
					 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	for (int nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		if (g_aResultPolygon[nCntResult].bUse == false)
		{// 使用していない場合

			// ポリゴンの情報を設定
			g_aResultPolygon[nCntResult].type = type;					// テクスチャインデックスを設定
			g_aResultPolygon[nCntResult].nIdxTexture = nIdxTexture;		// テクスチャインデックスを設定
			g_aResultPolygon[nCntResult].pos = pos;						// 位置を設定
			g_aResultPolygon[nCntResult].rot = rot;						// 角度を設定
			g_aResultPolygon[nCntResult].addRot = addRot;				// 加算角度を設定
			g_aResultPolygon[nCntResult].fSizeWidth = fSizeWidth;		// 幅の大きさを設定
			g_aResultPolygon[nCntResult].fSizeHeight = fSizeHeight;		// 高さの大きさを設定
			g_aResultPolygon[nCntResult].col = col;						// 色を設定
			g_aResultPolygon[nCntResult].texPos = texPos;				// 位置を設定
			g_aResultPolygon[nCntResult].texSize = texSize;				// テクスチャ座標を設定
			g_aResultPolygon[nCntResult].nDrowLevel = nDrowLevel;		// 描画順位を設定
			g_aResultPolygon[nCntResult].bAlphaBlend = bAlphaBlend;		// aブレンドの状態を設定
			g_aResultPolygon[nCntResult].bDisp = true;					// 表示している状態に設定
			g_aResultPolygon[nCntResult].bUse = true;					// 使用している状態に設定

			g_nNumResultPolygon++;

			return nCntResult;	// 設定した場所のインデックスを返す
		}
	}

	return -1;					// 設定していない値を返す
}

//========================================================================
// リザルトの情報を貰う処理
//========================================================================
void ReceiveResult(RESULT_TYPEPLAYER* pTypePlayer, int pHaveEsa[], int nMaxPlayer, int nMaxHave)
{
	// 変数宣言 ===========================================

	int nCntResult;

	EsaData* pEsaData = GetEsaData(0);

	// ====================================================

	// ランキングに渡す情報を初期化
	for (nCntResult = 0; nCntResult < MAX_PLAYER + MAX_COMPUTER; nCntResult++)
	{
		g_aGivePlayerR[nCntResult].typePlayer = RESULT_PLAYER_NONE;	// プレイヤーの種類を初期化
		g_aGivePlayerR[nCntResult].nTotalScore = 0;					// 合計スコアを初期化
	}

	g_nMaxPlayer = nMaxPlayer;		// プレイヤーの総数を設定

	// エサの種類別所持数を初期化
	for (nCntResult = 0; nCntResult < g_nMaxPlayer; nCntResult++)
	{		
		memset(&g_aNumHaveEsa[nCntResult][0], 0, sizeof (WORD) * MAX_ESATYPE);
	}

	// 持っているエサの数の集計
	for (nCntResult = 0; nCntResult < g_nMaxPlayer; nCntResult++)
	{// プレイヤーの総数分繰り返す

		g_aGivePlayerR[nCntResult].typePlayer = pTypePlayer[nCntResult];	// プレイヤーの種類を設定	

		int *pHave = &pHaveEsa[nMaxHave * nCntResult];	// nCntResult目の先頭の所持情報を設定

		// 持っているエサの所持数を増やす
		for (int nCntHave = 0; nCntHave < nMaxHave; nCntHave++, pHave++)
		{// 所持数の総数分繰り返す

			if (*pHave == -1)
			{// もっていない場合

				continue;	// 処理の始めに戻る
			}

			g_aGivePlayerR[nCntResult].nTotalScore += pEsaData[*pHave].nScore;	// プレイヤースコアを加算
			
			g_aNumHaveEsa[nCntResult][*pHave]++;	// 所持しているプレイヤーのエサの種類の場所の値を増やす
		}
	}
}

//========================================================================
// リザルトからランキングに情報を渡す処理
//========================================================================
int GetRankingForResult(GiveResultPlayer* pResultPlayer, int nNumPlayer)
{
	if (pResultPlayer)
	{// プレイヤーのタイプ情報を入れる場所がある場合

		for (int nCntPlayer = 0; nCntPlayer < nNumPlayer; nCntPlayer++, pResultPlayer++)
		{// 

			pResultPlayer->typePlayer = g_aGivePlayerR[nCntPlayer].typePlayer;		// プレイヤーのタイプ情報を設定
			pResultPlayer->nTotalScore = g_aGivePlayerR[nCntPlayer].nTotalScore;		// プレイヤートータルスコア情報を設定
		}
	}

	return g_nMaxPlayer;	// プレイヤーの総数を返す
}

//========================================================================
// リザルトの読み取り処理
//========================================================================
void SetLoadResult(const char* pFilename)
{
	// 変数宣言 ===========================================

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ

	FILE* pFile;									// ファイルポインタ

	char aReadText[256] = {};						// 読み取ったテキスト
	char aBlank[256] = {};							// 空の読み取り用

	int nNumTexture = 0;							// 読み込むテクスチャ数
	int nCntTexture = 0;							// 読み取ったテクスチャ数

	RESULTLAYTYPE setLayType = RESULTLAYTYPE_NULL;	//　設定するレイアウトの種類

	int nTmpNum;									// 仮の値を入れる
	int nCntIdxInfo = 0;							// インデクスのカウンタ
	int nCntPolygonInfo = 0;						// ポリゴンInfoのインデクスのカウンタ

	char aErrorText[64] = {};						// エラー文章

	// ====================================================

	// ▼ファイルを開く
	pFile = fopen(pFilename, "r");

	if (pFile == NULL)
	{// ファイルが開けなかった場合

		// メッセージウィンドウの作成
		MessageBox(NULL, "読み取りエラー\nby result.cpp", "error", MB_OK);

		return;	// 処理を抜ける
	}

	// ファイルの先頭を読み込む
	if (FileReadTop(pFile) == false)
	{// SCRIPTまで

		// メッセージウィンドウの作成
		MessageBox(NULL, "読み取りエラー\nby result.cpp", "error", MB_OK);

		return;	// 処理を抜ける
	}

	while (FileExtractText(pFile, &aReadText[0]))
	{// 文字の読み取り

		if		(strcmp(&aReadText[0], "END_SCRIPT") == 0)
		{// END_SCRIPT

			break;	// while文を抜ける
		}
		else if (strcmp(&aReadText[0], "NUM_TEXTURE") == 0)
		{// NUM_TEXTURE

			fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
			fscanf(pFile, "%d", &nNumTexture);							// 読み込むテクスチャ数を読み取る
		}
		else if (strcmp(&aReadText[0], "TEXTURE_FILENAME") == 0)
		{// TEXTURE_FILENAME

			if (nCntTexture < nNumTexture)
			{// 今のテクスチャ数より読み取る数が多い場合

				fscanf(pFile, "%[ =\t\n]", &aBlank[0]);					// 余分な情報を読み取る
				fscanf(pFile, "%s", &aReadText[0]);						// 読み込むテクスチャ数を読み取る

				// テクスチャの読み込み
				if (FAILED(D3DXCreateTextureFromFile(pDevice,								// デバイス
													 &aReadText[0],							// テクスチャファイル名
													 &g_apTextureResult[nCntTexture])))		// テクスチャポインタ
				{// テクスチャの読み込みに失敗

					// エラーメッセージテキストの作成
					sprintf(&aErrorText[0], "テクスチャの読み込みに失敗\n場所 : result\nTEX_FILENAME : %s", &aReadText[0]);

					// ウィンドウの作成
					MessageBox(NULL, &aErrorText[0], "ErrorMessage", MB_OK);
				}
			}
			
			nCntTexture++;	// テクスチャカウンタをインクリメント

		}
		else if (strcmp(&aReadText[0], "SET_LAYOUT_RESULT") == 0)
		{// SET_LAYOUT_RESULT

			// --------------------------------------------
			// 配置情報の読み取り
			// --------------------------------------------

			while (FileExtractText(pFile, &aReadText[0]))
			{// 文字の読み取り

				if		(strcmp(&aReadText[0], "END_LAYOUT_RESULT") == 0)
				{// END_LAYOUT_RESULT

					break;	// while文を抜ける
				}
				else if (strcmp(&aReadText[0], "DISPTIME") == 0)
				{// DISPTIME

					fscanf(pFile, "%[ =\t\n]", &aBlank[0]);			// 余分な情報を読み取る
					fscanf(pFile, "%d", &g_nSetingTime);			// 表示時間を読み取る
				}
				else if (strcmp(&aReadText[0], "SET_LAY_INIT") == 0)
				{// SET_LAY_INIT

					setLayType = RESULTLAYTYPE_INIT;				// 設定する場所を設定
				}
				else if (strcmp(&aReadText[0], "SET_LAY_STARTTALLTING") == 0)
				{// SET_LAY_STARTTALLTING

					setLayType = RESULTLAYTYPE_STARTTALLTING;		// 設定する場所を設定
				}
				else if (strcmp(&aReadText[0], "SET_LAY_ESA") == 0)
				{// SET_LAY_ESA

					setLayType = RESULTLAYTYPE_ESA;					// 設定する場所を設定
				}
				else if (strcmp(&aReadText[0], "SET_LAY_PLAYER") == 0)
				{// SET_LAY_PLAYER

					setLayType = RESULTLAYTYPE_PLAYER;				// 設定する場所を設定
				}
				else if (strcmp(&aReadText[0], "END_LAY_INIT") == 0				// END_LAY_INIT
					  || strcmp(&aReadText[0], "END_LAY_STARTTALLTING") == 0	// END_LAY_STARTTALLTING
					  || strcmp(&aReadText[0], "END_LAY_ESA") == 0				// END_LAY_ESA
					  || strcmp(&aReadText[0], "END_LAY_PLAYER") == 0)			// END_LAY_PLAYER
				{// END_LAY

					setLayType = RESULTLAYTYPE_NULL;				// 設定する場所を戻す
					nCntIdxInfo = 0;								// カウンタを初期化
				}

				// ----------------------------------------
				// 設定情報の設定
				// ----------------------------------------
				else if(setLayType != RESULTLAYTYPE_NULL)
				{// 設定する場所がある場合

					if (strcmp(&aReadText[0], "SET_POLYGON2D") == 0)
					{// SET_POLYGON2D

						while (FileExtractText(pFile, &aReadText[0]))
						{// 文字の読み取り

							if		(strcmp(&aReadText[0], "END_POLYGON2D") == 0)
							{// END_POLYGON2D

								g_aResultPolygonInfo[nCntPolygonInfo].Laytype = setLayType;

								g_aIdxResultInfo[setLayType][nCntIdxInfo] = nCntPolygonInfo;

								nCntIdxInfo++;

								nCntPolygonInfo++;		// リザルトポリゴンの総数をインクリメント

								break;	// while文を抜ける
							}
							else if (strcmp(&aReadText[0], "TYPE") == 0)
							{// TEXIDX

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%d", &g_aResultPolygonInfo[nCntPolygonInfo].type);		// テクスチャのインデックスを読み取る
							}
							else if (strcmp(&aReadText[0], "TEXIDX") == 0)
							{// TEXIDX

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%d", &g_aResultPolygonInfo[nCntPolygonInfo].nIdxTexture);		// テクスチャのインデックスを読み取る
							}
							else if (strcmp(&aReadText[0], "DRAWLEVEL") == 0)
							{// DRAWLEVEL

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%d", &g_aResultPolygonInfo[nCntPolygonInfo].nDrowLevel);			// 描画順位を読み取る
							}
							else if (strcmp(&aReadText[0], "BLEND_A") == 0)
							{// BLEND_A

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%d", &nTmpNum);														// アルファブレンドをするかを読み取る

								g_aResultPolygonInfo[nCntPolygonInfo].bAlphaBlend = (nTmpNum == 0) ? false : true;	// アルファブレンドをするかを設定							
							}
							else if (strcmp(&aReadText[0], "POS") == 0)
							{// POS

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f %f", &g_aResultPolygonInfo[nCntPolygonInfo].pos.x,				// 位置を読み取る
														  &g_aResultPolygonInfo[nCntPolygonInfo].pos.y,
														  &g_aResultPolygonInfo[nCntPolygonInfo].pos.z);							
							}
							else if (strcmp(&aReadText[0], "ROT") == 0)
							{// ROT

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f %f", &g_aResultPolygonInfo[nCntPolygonInfo].rot.x,				// 角度を読み取る
														  &g_aResultPolygonInfo[nCntPolygonInfo].rot.y,
														  &g_aResultPolygonInfo[nCntPolygonInfo].rot.z);
							}
							else if (strcmp(&aReadText[0], "ADDROT") == 0)
							{// ADDROT

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f %f", &g_aResultPolygonInfo[nCntPolygonInfo].addRot.x,			// 加算角度を読み取る
														  &g_aResultPolygonInfo[nCntPolygonInfo].addRot.y,
														  &g_aResultPolygonInfo[nCntPolygonInfo].addRot.z);
							}
							else if (strcmp(&aReadText[0], "SIZE") == 0)
							{// SIZE

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f", &g_aResultPolygonInfo[nCntPolygonInfo].fSizeWidth,			// 大きさを読み取る
													   &g_aResultPolygonInfo[nCntPolygonInfo].fSizeHeight);							
							}
							else if (strcmp(&aReadText[0], "COL") == 0)
							{// COL

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f %f %f", &g_aResultPolygonInfo[nCntPolygonInfo].col.r,			// 色を読み取る
															 &g_aResultPolygonInfo[nCntPolygonInfo].col.g,
															 &g_aResultPolygonInfo[nCntPolygonInfo].col.b,
															 &g_aResultPolygonInfo[nCntPolygonInfo].col.a);		
							}
							else if (strcmp(&aReadText[0], "TEXPOS") == 0)
							{// TEXPOS

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f", &g_aResultPolygonInfo[nCntPolygonInfo].texPos.x,				// テクスチャ座標を読み取る
													   &g_aResultPolygonInfo[nCntPolygonInfo].texPos.y);												
							}
							else if (strcmp(&aReadText[0], "TEXSIZE") == 0)
							{// TEXPOS

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f", &g_aResultPolygonInfo[nCntPolygonInfo].texSize.x,			// テクスチャサイズを読み取る
													   &g_aResultPolygonInfo[nCntPolygonInfo].texSize.y);
							}
						}
					}
					else if (strcmp(&aReadText[0], "SET_ESASET") == 0)
					{// SET_ESASET

						while (FileExtractText(pFile, &aReadText[0]))
						{// 文字の読み取り

							if		(strcmp(&aReadText[0], "END_ESASET") == 0)
							{// END_ESASET

								break;	// while文を抜ける
							}
							else if (strcmp(&aReadText[0], "DRAWLEVEL") == 0)
							{// DRAWLEVEL

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%d", &g_resulEsaInfo.nDrowLevel);									// 描画順位を読み取る
							}
							else if (strcmp(&aReadText[0], "POS") == 0)
							{// POS

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f %f %f", &g_resulEsaInfo.pos.x,									// 位置を読み取る
														  &g_resulEsaInfo.pos.y,
														  &g_resulEsaInfo.pos.z);
							}
							else if (strcmp(&aReadText[0], "ROTSPEED") == 0)
							{// ROTSPEED

								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);												// 余分な情報を読み取る
								fscanf(pFile, "%f", &g_resulEsaInfo.fRotSpeed);										// 回転速度を読み取る
							}
						}
					}
					else if (strcmp(&aReadText[0], "SET_ESAANGLE") == 0)
					{// SET_ESAANGLE

						while (FileExtractText(pFile, &aReadText[0]))
						{// 文字の読み取り

							if		(strcmp(&aReadText[0], "END_ESAANGLE") == 0)
							{// END_ESAANGLE

								break;
							}
							else if (strcmp(&aReadText[0], "ESA[") == 0)
							{// ESA[ %d ]

								fscanf(pFile, " %d ]", &nTmpNum);								// 余分な情報を読み取る
								fscanf(pFile, "%[ =\t\n]", &aBlank[0]);							// 余分な情報を読み取る
								fscanf(pFile, "%f %f %f", &g_resulEsaInfo.esaRot[nTmpNum].x,	// 初期角度を読み取る
														  &g_resulEsaInfo.esaRot[nTmpNum].y,
														  &g_resulEsaInfo.esaRot[nTmpNum].z);		
							}
						}
					}
					else if (setLayType == RESULTLAYTYPE_PLAYER)
					{
						if (strcmp(&aReadText[0], "INITPOS") == 0)
						{// INITPOS

							fscanf(pFile, "%[ =\t\n]", &aBlank[0]);							// 余分な情報を読み取る
							fscanf(pFile, "%f %f %f", &g_resultPlayerInfo.pos.x,			// 初期位置を読み取る
													  &g_resultPlayerInfo.pos.y,
													  &g_resultPlayerInfo.pos.z);
						}
						else if (strcmp(&aReadText[0], "SPACE_WIDTH") == 0)
						{// SPACE_WIDTH

							fscanf(pFile, "%[ =\t\n]", &aBlank[0]);							// 余分な情報を読み取る
							fscanf(pFile, "%f", &g_resultPlayerInfo.fSpaceWidth);			// 開ける間隔(幅)を読み取る
						}
						else if (strcmp(&aReadText[0], "SPACE_HEIGHT") == 0)
						{// SPACE_HEIGHT

							fscanf(pFile, "%[ =\t\n]", &aBlank[0]);							// 余分な情報を読み取る
							fscanf(pFile, "%f", &g_resultPlayerInfo.fSpaceHeight);			// 開ける間隔(高さ)を読み取る
						}
					}
				
				}
			}
		}

	}

	// ▲ファイルを閉じる
	fclose(pFile);
}

//========================================================================
// リザルトの集計中のエサの種類を返す処理
//========================================================================
int GetNowEsaTypeResult(void)
{
	return g_nNowEsaType;
}

RESULTSTATE GetResultState(void)
{
	return g_resultState;
}