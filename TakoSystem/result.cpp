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

// マクロ関数 ==================================================

#define	MAX_TEX_RESULT		(32)	// リザルトのテクスチャの最大数
#define	MAX_NUM_RESULT		(32)	// リザルトのポリゴンの最大数
//#define	MAX_SET_RESULT		(64)	// リザルトの最大数

#define MAX_DROWLEVEL		(5)		// 描画順位の最大値

#define SETPOS_PLAYER		(D3DXVECTOR3(-167.0f, 85.0f, 0.0f))
#define SETARIA_PLAYER		(33.0f)

#define WAIT_SETING			(60 * 5)


// 構造体の定義 ================================================

// リザルトポリゴンの情報
typedef struct
{
	int nIdxTexture;			// テクスチャインデックス

	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度

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
	int nIdxTexture;			// テクスチャインデックス
	bool bAlphaBlend;			// aブレンドをするか

	int nDrowLevel;				// 描画順位

	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度

	float fSizeWidth;			// 大きさ(幅)
	float fSizeHeight;			// 大きさ(高さ)

	D3DXVECTOR2 texPos;			// テクスチャ(座標)
	D3DXVECTOR2 texSize;		// テクスチャ(サイズ)

	D3DXCOLOR col;				// 色

}ResultPolygon_info;

typedef struct
{
	
	int nIdxPolygon[5];					// インデックス

}Result_info;

// プロトタイプ宣言 ============================================

void DrawResultPolygon(int nIdx);			// リザルトのポリゴンの描画
//void DrawResultModel(Result* pResult);		// リザルトのモデルの描画

// グローバル変数 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureResult[MAX_TEX_RESULT] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResult = NULL;				// 頂点バッファへのポインタ

ResultPolygon g_aResultPolygon[MAX_NUM_RESULT];					// リザルトのポリゴン情報

// リザルトの状態
RESULTSTATE g_resultState = RESULTSTATE_BEGIN;					// リザルトの状態
int g_nCounterResultState;										// リザルトの状態カウンター
int g_nNowEsaType;

int g_aIdxUiResultGS[MAX_PLAYER + MAX_COMPUTER] = {};			

// ゲームの情報
int g_nMaxPlayer;													// プレイヤーの総数
int g_nIdxSetEsa;													// エサの種類の総数
//WORD g_aEsaScore[MAX_ESATYPE] = {};									// エサのスコア
WORD g_aNumHaveEsa[MAX_PLAYER + MAX_COMPUTER][MAX_ESATYPE] = {};	// 持っているエサの数

// ファイル名
const char* c_apFilenameResult[] =
{
	"data/TEXTURE/In_the_sea.png",		// [0]背景
	"data/TEXTURE/tex_alpha001.jpg",	// [1]スコア背景
	"data/TEXTURE/RESULT.png",			// [2]RESULTテキスト
	"data/TEXTURE/RESULT_LINE.png",		// [3]放射線
	"data/TEXTURE/text_player.png",		// [4]プレイヤーテキスト
	"data/TEXTURE/number000.png",		// [5]数字
};

ResultPolygon_info g_aResultPolygonInfo[] =
{// {テクスチャインデックス, aブレンドをするか,描画順位, オフセット, 角度,  幅, 高さ, テクスチャ座標, テクスチャサイズ, 色}

	{0, false, 0, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1.0f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},	// [0]背景情報
	{1, true, 0, D3DXVECTOR3(0.0f, 112.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 250.0f, 13.0f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1.0f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},							// [1]リザルトテキスト背景
	{2, false, 0, D3DXVECTOR3(-176.0f, 112.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 50.0f, 33.0f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1.0f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},							// [2]リザルトテキスト
	{3, false, 0, D3DXVECTOR3(134.0f,11.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 70.0f, 70.0f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1.0f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},								// [3]放射線
	{4, false, 0, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 45.0f, 10.0f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1.0f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},								// [4]プレイヤーテキスト
	{5, false, 0, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 10.0f, 10.0f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(0.1f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},								// [5]プレイヤーナンバー
};


int g_nSelectNum = 0;

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
	SetCameraPos(0, D3DXVECTOR3(0.0f, 0.0f, 100.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_STOP);
	
	// エサの初期化
	InitEsa(false);	

	// リザルトの獲得スコアUIの初期化
	InitUiResultGetScore();
	
	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameResult / sizeof(c_apFilenameResult[0]); nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice,								// デバイス
								  c_apFilenameResult[nCntTexture],		// テクスチャファイル名
								  &g_apTextureResult[nCntTexture]);		// テクスチャポインタ
	}

	// リザルトのポリゴン情報を初期化
	for (nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		g_aResultPolygon[nCntResult].nIdxTexture = -1;							// テクスチャインデックスを初期化
		g_aResultPolygon[nCntResult].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		g_aResultPolygon[nCntResult].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 角度を初期化
		g_aResultPolygon[nCntResult].fSizeWidth = 0.0f;							// 大きさ(幅)を初期化
		g_aResultPolygon[nCntResult].fSizeHeight = 0.0f;						// 大きさ(高さ)を初期化
		g_aResultPolygon[nCntResult].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 色を初期化
		g_aResultPolygon[nCntResult].texPos = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャ座標を初期化
		g_aResultPolygon[nCntResult].texSize = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャサイズを初期化
		g_aResultPolygon[nCntResult].nDrowLevel = 0;							// 描画順位を初期化
		g_aResultPolygon[nCntResult].bAlphaBlend = false;						// aブレンドをしない状態に設定
		g_aResultPolygon[nCntResult].bDisp = false;								// 表示しない状態に設定
		g_aResultPolygon[nCntResult].bUse = false;								// 使用しない状態に設定
	}

	g_resultState = RESULTSTATE_BEGIN;	// リザルトの状態を開始状態に設定
	g_nCounterResultState = 0;			// リザルトの状態カウンターを初期化

	g_nNowEsaType = -1;

	g_nIdxSetEsa = -1;

	memset(&g_aIdxUiResultGS[0], -1, MAX_PLAYER + MAX_COMPUTER);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_NUM_RESULT,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_3D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffResult,
								NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタ

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
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

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

	// リザルトポリゴンの設定(背景)
	SetResultPolygon(g_aResultPolygonInfo[0].nIdxTexture, g_aResultPolygonInfo[0].bAlphaBlend, g_aResultPolygonInfo[0].nDrowLevel,							// 
					 g_aResultPolygonInfo[0].pos, g_aResultPolygonInfo[0].rot, g_aResultPolygonInfo[0].fSizeWidth, g_aResultPolygonInfo[0].fSizeHeight,		// 
					 g_aResultPolygonInfo[0].texPos, g_aResultPolygonInfo[0].texSize, g_aResultPolygonInfo[0].col);											// 

	// リザルトポリゴンの設定(リザルトテキスト背景)
	SetResultPolygon(g_aResultPolygonInfo[1].nIdxTexture, g_aResultPolygonInfo[1].bAlphaBlend, g_aResultPolygonInfo[1].nDrowLevel,							// 
					 g_aResultPolygonInfo[1].pos, g_aResultPolygonInfo[1].rot, g_aResultPolygonInfo[1].fSizeWidth, g_aResultPolygonInfo[1].fSizeHeight,		// 
					 g_aResultPolygonInfo[1].texPos, g_aResultPolygonInfo[1].texSize, g_aResultPolygonInfo[1].col);											// 

	// リザルトポリゴンの設定(リザルトテキスト)
	SetResultPolygon(g_aResultPolygonInfo[2].nIdxTexture, g_aResultPolygonInfo[2].bAlphaBlend, g_aResultPolygonInfo[2].nDrowLevel,							// 
					 g_aResultPolygonInfo[2].pos, g_aResultPolygonInfo[2].rot, g_aResultPolygonInfo[2].fSizeWidth, g_aResultPolygonInfo[2].fSizeHeight,		// 
					 g_aResultPolygonInfo[2].texPos, g_aResultPolygonInfo[2].texSize, g_aResultPolygonInfo[2].col);											// 

	//  リザルトポリゴンの設定(放射線)
	SetResultPolygon(g_aResultPolygonInfo[3].nIdxTexture, g_aResultPolygonInfo[3].bAlphaBlend, g_aResultPolygonInfo[3].nDrowLevel,							// 
					 g_aResultPolygonInfo[3].pos, g_aResultPolygonInfo[3].rot, g_aResultPolygonInfo[3].fSizeWidth, g_aResultPolygonInfo[3].fSizeHeight,		// 
					 g_aResultPolygonInfo[3].texPos, g_aResultPolygonInfo[3].texSize, g_aResultPolygonInfo[3].col);											// 

	int aTmpHaveEsa[3][5] =
	{
		{0, 1, 2, 3, 4},
		{0, 0, 0, 0, 0},
		{1, 0, 0, 0, -1}
	};

	//ReceiveResult(&aTmpHaveEsa[0][0], 3, 5);

	// リザルトの設定
	SetResult(RESULTTYPE_PLAYER, SETPOS_PLAYER);


	// サウンドの再生
	PlaySound(SOUND_BGM_RESULT);
}

//========================================================================
// リザルトの終了処理
//========================================================================
void UninitResult(void)
{
	int nCntResult;	// カウンター

	// ゲームの各情報を初期化
	for (nCntResult = 0; nCntResult < MAX_PLAYER + MAX_COMPUTER; nCntResult++)
	{// プレイヤーの最大数分繰り返す

		memset(&g_aNumHaveEsa[nCntResult][0], 0, MAX_ESATYPE);	// エサの種類別所持数を初期化
	}

	//memset(&g_aEsaScore[0], 0, MAX_ESATYPE);					// エサの種類別獲得スコアを初期化

	g_nMaxPlayer = 0;											// プレイヤーの総数を初期化

	
	// エサの終了
	UninitEsa();

	// リザルトの獲得スコアUIの終了
	UninitUiResultGetScore();

	// テクスチャの破棄
	for (nCntResult = 0; nCntResult < MAX_TEX_RESULT; nCntResult++)
	{// タイトルの数だけ確認する

		// テクスチャの破棄
		if (g_apTextureResult[nCntResult] != NULL)
		{
			g_apTextureResult[nCntResult]->Release();
			g_apTextureResult[nCntResult] = NULL;
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
	VERTEX_3D* pVtx;	// 頂点情報へのポインタ
	
	// フェード情報の取得
	FADE pFade = GetFade();

	EsaData* pEsaData = GetEsaData(0);
	Esa* pEsa = GetEsa();


	// リザルトの獲得スコアUIの更新
	UpdateUiResultGetScore();

#if 1	// デバッグ
	
	// 選択する対象の設定
	if (GetKeyboardTrigger(DIK_1)) g_nSelectNum--;
	if (GetKeyboardTrigger(DIK_2)) g_nSelectNum++;
	
	if (g_nSelectNum != -1)
	{// インデックスがある場合

		// 位置の更新
		FileMovePosion("data/FILE/tmpPosionResult.txt", &g_aResultPolygon[g_nSelectNum].pos, 1.0f, DIK_RIGHT, DIK_LEFT, DIK_DOWN, DIK_UP, NULL, NULL);

		// デバッグテキスト
		PrintDebugProc("\nSELECT_POLYGON %d[1,2]", g_nSelectNum);
		PrintDebugProc("\nDRAWLEVEL %d", g_aResultPolygon[g_nSelectNum].nDrowLevel);
		PrintDebugProc("\nPOS %f %f %f", g_aResultPolygon[g_nSelectNum].pos.x, g_aResultPolygon[g_nSelectNum].pos.y, g_aResultPolygon[g_nSelectNum].pos.z);
		PrintDebugProc("\nCOUNTER %d", g_nCounterResultState);
	}

#endif

	switch(g_resultState)
	{
	case RESULTSTATE_BEGIN:

		if (GetKeyboardTrigger(DIK_RETURN))
		{
			g_resultState = RESULTSTATE_SETING;
		}
	
		break;

	case RESULTSTATE_WAIT:
	
		g_nCounterResultState--;

		if (g_nCounterResultState <= 0)
		{// カウンタが0になった

			g_resultState = RESULTSTATE_SETING;
			
			if (g_nIdxSetEsa != -1)
			{// エサのインデックスがある

				pEsa[g_nIdxSetEsa].bUse = false;	// インデックスのエサを使用していない状態に設定
			
				g_nIdxSetEsa = -1;					// インデックスがない状態に設定
			}

			for (int nCntPlayer = 0; nCntPlayer < g_nMaxPlayer; nCntPlayer++)
			{
				// // リザルトスコアUIの削除
				DelUiResultGetScore(g_aIdxUiResultGS[nCntPlayer]);
			}
		}
		else
		{// カウントが0になっていない

			// 表示されているエサの角度の更新
			pEsa[g_nIdxSetEsa].rot.y += 0.01f;

			if		(pEsa[g_nIdxSetEsa].rot.y < -D3DX_PI)
			{// -3.14を超えた場合

				pEsa[g_nIdxSetEsa].rot.y + D3DX_PI * 2.0f;
			}
			else if (pEsa[g_nIdxSetEsa].rot.y < -D3DX_PI)
			{// 3.14を超えた場合

				pEsa[g_nIdxSetEsa].rot.y - D3DX_PI * 2.0f;
			}
		}

		break;
	
	case RESULTSTATE_SETING:
	
		g_nCounterResultState = WAIT_SETING;

		g_resultState = RESULTSTATE_WAIT;

		g_nNowEsaType++;

		if (g_nNowEsaType < GetNumEsaType())
		{
			for (int nCntPlayer = 0; nCntPlayer < g_nMaxPlayer; nCntPlayer++)
			{
				if (g_aNumHaveEsa[nCntPlayer][g_nNowEsaType] > 0)
				{// 1個でも獲得していた場合

					// リザルトスコアUIの設定
					g_aIdxUiResultGS[nCntPlayer] = SetUiResultGetScore(SETPOS_PLAYER - D3DXVECTOR3(-70.0f, SETARIA_PLAYER * nCntPlayer, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
																	   pEsaData[g_nNowEsaType].nScore, g_aNumHaveEsa[nCntPlayer][g_nNowEsaType]);
				}
			}

			// エサの設定
			g_nIdxSetEsa = SetEsa(g_nNowEsaType, ESA_ACTTYPE_NONE, 0, D3DXVECTOR3(134.0f, 11.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
		}
		else
		{
			g_resultState = RESULTSTATE_COMPLETE;
		}

		break;
	}

	for (int nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		if (g_aResultPolygon[nCntResult].bUse == true)
		{// 使用している場合

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntResult * 4);		// 頂点データのポインタをインデックス分進める
			
			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-g_aResultPolygon[nCntResult].fSizeWidth,  g_aResultPolygon[nCntResult].fSizeHeight, 0.0f);
			pVtx[1].pos = D3DXVECTOR3( g_aResultPolygon[nCntResult].fSizeWidth,  g_aResultPolygon[nCntResult].fSizeHeight, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aResultPolygon[nCntResult].fSizeWidth, -g_aResultPolygon[nCntResult].fSizeHeight, 0.0f);
			pVtx[3].pos = D3DXVECTOR3( g_aResultPolygon[nCntResult].fSizeWidth, -g_aResultPolygon[nCntResult].fSizeHeight, 0.0f);

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

		}
	}

	// 次のモードへの移動処理
#if 1
	if (pFade == FADE_NONE && g_resultState == RESULTSTATE_COMPLETE 
	    || (/*GetKeyboardTrigger(DIK_RETURN) == true ||*/ GetJoypadTrigger(0, JOYKEY_START) == true || GetJoypadTrigger(0, JOYKEY_A) == true))
	{// 決定キー（ENTERキー）が押された
		
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

				// ポリゴンの描画処理
				DrawResultPolygon(nCntResult);				
			}
		}
	}

	// エサの描画
	DrawEsa();

	// リザルトの獲得スコアUIの描画
	DrawUiResultGetScore();
}

//========================================================================
// リザルトのポリゴンの描画処理
//========================================================================
void DrawResultPolygon(int nIdx)
{
	// 変数宣言 ===========================================

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	D3DXMATRIX mtxWorld;
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルのポインタ

	// ====================================================

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);	// ワールドマトリックスの初期値を設定

	// 向きの反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
								   g_aResultPolygon[nIdx].rot.y, g_aResultPolygon[nIdx].rot.x, g_aResultPolygon[nIdx].rot.z);

	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を設定
	D3DXMatrixTranslation(&mtxTrans,
						  g_aResultPolygon[nIdx].pos.x, g_aResultPolygon[nIdx].pos.y, g_aResultPolygon[nIdx].pos.z);

	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffResult, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	if (nIdx != -1)
	{// テクスチャがある場合

		pDevice->SetTexture(0, g_apTextureResult[g_aResultPolygon[nIdx].nIdxTexture]);
	}
	else
	{// テクスチャがない場合

		pDevice->SetTexture(0, NULL);
	}

	if (g_aResultPolygon[nIdx].bAlphaBlend == true)
	{// aブレンドをする場合

		// 減算合成の設定
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3
	}

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	// 描画形式
						   nIdx * 4,			// インデックス
						   2);					// ポリゴン数

	if (g_aResultPolygon[nIdx].bAlphaBlend == true)
	{// aブレンドをする場合

		// 減算合成の設定を元に戻す 
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
	}
}

//========================================================================
// リザルトの設定処理
//========================================================================
void SetResult(RESULTTYPE type, D3DXVECTOR3 pos)
{
	D3DXVECTOR3 setPos, setShiftPos;
	float fSetWidth, fSetHeight;
	D3DXVECTOR2 setTexPos, setTexSize;
	D3DXCOLOR setCol;

	

	switch (type)
	{
	case RESULTTYPE_PLAYER:

		int aIdxInfo[] = { 4, 5 };

		for (int nCntPlayer = 0; nCntPlayer < g_nMaxPlayer; nCntPlayer++)
		{
			setShiftPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			for (int nCntNum = 0; nCntNum < sizeof aIdxInfo / sizeof(aIdxInfo[0]); nCntNum++)
			{// プレイヤーと数字の設定

				setPos.x = pos.x + g_aResultPolygonInfo[aIdxInfo[nCntNum]].pos.x + setShiftPos.x + ((nCntNum > 0) ? g_aResultPolygonInfo[aIdxInfo[nCntNum]].fSizeWidth : 0);
				setPos.y = pos.y + g_aResultPolygonInfo[aIdxInfo[nCntNum]].pos.y - SETARIA_PLAYER * nCntPlayer;
				setPos.z = pos.z + g_aResultPolygonInfo[aIdxInfo[nCntNum]].pos.z;

				if (nCntNum > 0)
				{
					setTexPos.x = (nCntPlayer + 1) * 0.1f;
					setTexPos.y = 0.0f;
				}
				else
				{
					setTexPos = g_aResultPolygonInfo[aIdxInfo[nCntNum]].texPos;
				}


				// リザルトポリゴンの設定
				SetResultPolygon(g_aResultPolygonInfo[aIdxInfo[nCntNum]].nIdxTexture,
								 g_aResultPolygonInfo[aIdxInfo[nCntNum]].bAlphaBlend,
								 g_aResultPolygonInfo[aIdxInfo[nCntNum]].nDrowLevel,
								 setPos,
								 g_aResultPolygonInfo[aIdxInfo[nCntNum]].rot,
								 g_aResultPolygonInfo[aIdxInfo[nCntNum]].fSizeWidth,
								 g_aResultPolygonInfo[aIdxInfo[nCntNum]].fSizeHeight, 
								 setTexPos,
								 g_aResultPolygonInfo[aIdxInfo[nCntNum]].texSize,
								 g_aResultPolygonInfo[aIdxInfo[nCntNum]].col);

				setShiftPos.x += g_aResultPolygonInfo[aIdxInfo[nCntNum]].fSizeWidth;
			}
		}

		break;
	}
}

//========================================================================
// リザルトのポリゴンの設定処理
//========================================================================
int SetResultPolygon(int nIdxTexture, bool bAlphaBlend, int nDrowLevel, 
					 D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fSizeWidth, float fSizeHeight,
					 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	for (int nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		if (g_aResultPolygon[nCntResult].bUse == false)
		{// 使用していない場合

			// ポリゴンの情報を設定
			g_aResultPolygon[nCntResult].nIdxTexture = nIdxTexture;		// テクスチャインデックスを設定
			g_aResultPolygon[nCntResult].pos = pos;						// 位置を設定
			g_aResultPolygon[nCntResult].rot = rot;						// 角度を設定
			g_aResultPolygon[nCntResult].fSizeWidth = fSizeWidth;		// 幅の大きさを設定
			g_aResultPolygon[nCntResult].fSizeHeight = fSizeHeight;		// 高さの大きさを設定
			g_aResultPolygon[nCntResult].col = col;						// 色を設定
			g_aResultPolygon[nCntResult].texPos = texPos;				// 位置を設定
			g_aResultPolygon[nCntResult].texSize = texSize;				// テクスチャ座標を設定
			g_aResultPolygon[nCntResult].nDrowLevel = nDrowLevel;		// 描画順位を設定
			g_aResultPolygon[nCntResult].bAlphaBlend = bAlphaBlend;		// aブレンドの状態を設定
			g_aResultPolygon[nCntResult].bDisp = true;					// 表示している状態に設定
			g_aResultPolygon[nCntResult].bUse = true;					// 使用している状態に設定

			return nCntResult;	// 設定した場所のインデックスを返す
		}
	}

	return -1;					// 設定していない値を返す
}

//========================================================================
// リザルトの情報を貰う処理
//========================================================================
void ReceiveResult(int pHaveEsa[], int nMaxPlayer, int nMaxHave)
{
	// 変数宣言 ===========================================

	int nCntResult;

	// ====================================================

	g_nMaxPlayer = nMaxPlayer;		// プレイヤーの総数を設定

	// エサの種類別所持数を初期化
	for (nCntResult = 0; nCntResult < g_nMaxPlayer; nCntResult++)
	{		
		memset(&g_aNumHaveEsa[nCntResult][0], 0, MAX_ESATYPE);
	}

	// 持っているエサの数の集計
	for (nCntResult = 0; nCntResult < g_nMaxPlayer; nCntResult++)
	{// プレイヤーの総数分繰り返す

		int *pHave = &pHaveEsa[nMaxHave * nCntResult];	// nCntResult目の先頭の所持情報を設定

		// 持っているエサの所持数を増やす
		for (int nCntHave = 0; nCntHave < nMaxHave; nCntHave++, pHave++)
		{// 所持数の総数分繰り返す

			if (*pHave == -1)
			{// もっていない場合

				continue;	// 処理の始めに戻る
			}

			g_aNumHaveEsa[nCntResult][*pHave]++;	// 所持しているプレイヤーのエサの種類の場所の値を増やす
		}
	}
}