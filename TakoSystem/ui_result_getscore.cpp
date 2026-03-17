//==================================================================================
// 
// リザルトの獲得スコアのUIに関する処理[ui_result_getscore.cpp]
// Author : Soki Inamura 
// 
//==================================================================================
#include "main.h"					// メインヘッダー
#include "ui_result_getscore.h"		// リザルトの獲得スコアのUI
#include "result.h"					// リザルト

#include "debugproc.h"
#include "input.h"
#include "sound.h"

// マクロ定義 ==================================================

#define MAX_TEX_UIRESULTGS	(10)	// 設定出来るテクスチャの総数
#define MAX_NUM_RESULTGS	(256)	// 設定出来るポリゴンの総数
//#define MAX_SET_UI_RESULTGETSCORE	(256)	// 設定出来るポリゴンの総数

#define MAX_GLOUP_RESULTGS	(15)	// 設定出来るグループインデックスの総数

#define NUM_TEXBLOCK				(11)	// 数字テクスチャの分割数

#define MAX_DIGIT					(2)		// 桁数

#define WAIT_UIRESULTSG		((int)(60 * 1))

// 構造体の定義 ================================================

// リザルトの獲得スコアUIのポリゴン情報
typedef struct
{
	int nIdxTexture;				// テクスチャインデックス
	UI_RESULTGSPOLYGONTYPE type;	// ポリゴンの種類
	int nDigit;						// 桁数

	D3DXVECTOR3 pos;				// 位置(オフセット)
	D3DXVECTOR3 rot;				// 角度

	float fSizeWidth;				// 大きさ(幅)
	float fSizeHeight;				// 大きさ(高さ)

	float fSizeWLeft;				// 左頂点の水平方向の大きさ
	float fSizeWRight;				// 右頂点の水平方向の大きさ

	D3DXVECTOR2 texPos;				// テクスチャ(座標)
	D3DXVECTOR2 texSize;			// テクスチャ(サイズ)

	D3DXCOLOR col;

	bool bAlphaBlend;				// aブレンドをするか

	bool bDisp;						// 表示状態
	bool bUse;						// 使用状態

}UiResultGSPolygon;

// リザルトの獲得スコアUIのポリゴンの設定情報
typedef struct
{
	UI_RESULTGSPOLYGONTYPE type;	// ポリゴンの種類
	int nIdxTexture;				// テクスチャインデックス
	bool bAlphaBlend;				// aブレンドをするか

	D3DXVECTOR3 pos;				// 位置(オフセット)
	D3DXVECTOR3 rot;				// 角度

	float fSizeWidth;				// 大きさ(幅)
	float fSizeHeight;				// 大きさ(高さ)

	float fSizeWLeft;				// 左頂点の水平方向の大きさ
	float fSizeWRight;				// 右頂点の水平方向の大きさ

	D3DXVECTOR2 texPos;				// テクスチャ(座標)
	D3DXVECTOR2 texSize;			// テクスチャ(サイズ)

}UiResultGSPolygon_info;

// リザルトの獲得スコアUIの情報
typedef struct
{
	int aIdxPolygon[MAX_GLOUP_RESULTGS];	// ポリゴンインデックス
	int aIdxNumPolygon[MAX_DIGIT];			// 数値のインデックス

	int nMaxHave;							// 総獲得数
	int nNowHave;							// 獲得数

	D3DXVECTOR3 pos;						// 位置
	D3DXCOLOR col;							// 色

	int nUseTimer;							// 使用時間

	bool bUse;								// 使用状態

}UiResultGS;

// グローバル変数 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureUiResultGetScore[MAX_TEX_UIRESULTGS] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUiResultGetScore = NULL;					// 頂点バッファへのポインタ

UiResultGSPolygon g_aResultGSPolygon[MAX_NUM_RESULTGS];						// リザルトの獲得スコアUIポリゴンの情報
UiResultGS g_aResultGS[MAX_PLAYER + MAX_COMPUTER];							// リザルトの獲得スコアUIの情報

int g_nNumResultGS;	// 設定数
int g_nEndResultGS;	// 集計完了数

UIRESULTGS_STATE g_uiResultGSState;
bool g_bSet;

// テクスチャファイル情報
const char* c_apFilenameResultGS[] =
{
	"data/TEXTURE/tex_alpha001.jpg",	// [0]スコア背景
	"data/TEXTURE/point.png",			// [1]数字
	"data/TEXTURE/chara_plus.png",		// [2]プラス記号
	"data/TEXTURE/chara_cross.png",		// [3]クロス記号
};

// リザルトの獲得スコアUIの設定情報(テンプレ)
UiResultGSPolygon_info g_aUiResultGSPolygonInfo[] =
{// type, nIdxTex, bBrendAlpha, pos,  
	{UI_RESULTGSPOLYGONTYPE_BG,			0, true, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),  430.0f, 30.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(1.0f, 1.0f)},				// スコア背景
	{UI_RESULTGSPOLYGONTYPE_NONE,		2, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 45.0f, 30.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(1.0f, 1.0f)},			// プラス記号
	{UI_RESULTGSPOLYGONTYPE_NUM_SCORE,	1, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 45.0f, 30.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.090f, 1.0f)},	// 獲得スコア
	{UI_RESULTGSPOLYGONTYPE_NONE,		1, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 45.0f, 30.0f, 0.0f, 0.0f, D3DXVECTOR2(0.909f, 0.0f),D3DXVECTOR2(0.090f, 1.0f)},		// ポイント
	{UI_RESULTGSPOLYGONTYPE_NONE,		3, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 45.0f, 30.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(1.0f, 1.0f)},			// 掛ける記号
	{UI_RESULTGSPOLYGONTYPE_NUM_GET,	1, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 45.0f, 30.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.090f, 1.0f)},	// 獲得数

};

int g_nNumResultSGPolygon = 0;
int g_nIdxSelectResultSG = 0;

int g_nTimerResultSG = 0;

//========================================================================
// リザルトの獲得スコアのUIの初期化処理
//========================================================================
void InitUiResultGetScore(void)
{
	// 変数宣言 ===========================================

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	int nCntUiResultGS;

	// ====================================================
	
	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameResultGS / sizeof(c_apFilenameResultGS[0]); nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice,										// デバイス
								  c_apFilenameResultGS[nCntTexture],			// テクスチャファイル名
								  &g_apTextureUiResultGetScore[nCntTexture]);	// テクスチャポインタ
	}

	// ポリゴン情報の初期化
	for (nCntUiResultGS = 0; nCntUiResultGS < MAX_NUM_RESULTGS; nCntUiResultGS++)
	{
		g_aResultGSPolygon[nCntUiResultGS].nIdxTexture = -1;							// 
		g_aResultGSPolygon[nCntUiResultGS].nDigit = -1;									// 
		g_aResultGSPolygon[nCntUiResultGS].type = UI_RESULTGSPOLYGONTYPE_NONE;			// 
		g_aResultGSPolygon[nCntUiResultGS].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 
		g_aResultGSPolygon[nCntUiResultGS].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 
		g_aResultGSPolygon[nCntUiResultGS].fSizeWidth = 0.0f;							// 
		g_aResultGSPolygon[nCntUiResultGS].fSizeHeight = 0.0f;							// 
		g_aResultGSPolygon[nCntUiResultGS].fSizeWLeft = 0.0f;							// 
		g_aResultGSPolygon[nCntUiResultGS].fSizeWRight = 0.0f;							// 
		g_aResultGSPolygon[nCntUiResultGS].texPos = D3DXVECTOR2(0.0f,0.0f);				// 
		g_aResultGSPolygon[nCntUiResultGS].texSize = D3DXVECTOR2(0.0f,0.0f);			// 
		g_aResultGSPolygon[nCntUiResultGS].col = D3DXCOLOR(0.0f,0.0f,0.0f,0.0);			// 
		g_aResultGSPolygon[nCntUiResultGS].bAlphaBlend = false;							// 
		g_aResultGSPolygon[nCntUiResultGS].bDisp = false;								// 
		g_aResultGSPolygon[nCntUiResultGS].bUse = false;								// 
	}

	// 獲得スコアのUI情報の初期化
	for (nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		memset(&g_aResultGS[nCntUiResultGS].aIdxPolygon[0], -1, sizeof g_aResultGS[nCntUiResultGS].aIdxPolygon);		// 
		memset(&g_aResultGS[nCntUiResultGS].aIdxNumPolygon[0], -1, sizeof g_aResultGS[nCntUiResultGS].aIdxNumPolygon);	// 
		g_aResultGS[nCntUiResultGS].nMaxHave = 0;																		// 
		g_aResultGS[nCntUiResultGS].nNowHave = 0;																		// 
		g_aResultGS[nCntUiResultGS].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);												// 
		g_aResultGS[nCntUiResultGS].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);											// 
		g_aResultGS[nCntUiResultGS].nUseTimer = 0;																		// 
		g_aResultGS[nCntUiResultGS].bUse = false;																		// 
	}

	g_nNumResultGS = 0;		// 
	g_nEndResultGS = 0;		// 

	g_nTimerResultSG = 0;

	g_uiResultGSState = UIRESULTGS_STATE_BIGIN;
	g_bSet = false;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_NUM_RESULTGS,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUiResultGetScore,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// ▼頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffUiResultGetScore->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntUiResultGS = 0; nCntUiResultGS < MAX_NUM_RESULTGS; nCntUiResultGS++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;	// 値は1.0fで固定
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

	// ▲頂点バッファをアンロックする
	g_pVtxBuffUiResultGetScore->Unlock();

	//SetUiResultGetScore(D3DXVECTOR3(0.0f,  0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 10, 9);
}

//========================================================================
// リザルトの獲得スコアのUIの終了処理
//========================================================================
void UninitUiResultGetScore(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_UIRESULTGS; nCntTexture++)
	{// タイトルの数だけ確認する

		// テクスチャの破棄
		if (g_apTextureUiResultGetScore[nCntTexture] != NULL)
		{
			g_apTextureUiResultGetScore[nCntTexture]->Release();
			g_apTextureUiResultGetScore[nCntTexture] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffUiResultGetScore != NULL)
	{
		g_pVtxBuffUiResultGetScore->Release();
		g_pVtxBuffUiResultGetScore = NULL;
	}
}

//========================================================================
// リザルトの獲得スコアのUIの更新処理
//========================================================================
void UpdateUiResultGetScore(void)
{
	// 変数宣言 ===========================================
	
	int nIdx, nDigit;
	int aTexU[MAX_DIGIT];

	// ====================================================

#if _DEBUG
#if 0
	// 選択する対象の設定
	if (GetKeyboardTrigger(DIK_1))
	{
		g_nIdxSelectResultSG--;
	}
	if (GetKeyboardTrigger(DIK_2))
	{
		if (g_nNumResultSGPolygon == 0)
		{
			g_nIdxSelectResultSG = -1;
		}
		else
		{
			g_nIdxSelectResultSG++;

			g_nIdxSelectResultSG = g_nIdxSelectResultSG % g_nNumResultSGPolygon;
		}
	}

	if (g_nIdxSelectResultSG != -1)
	{
		if (GetKeyboardPress(DIK_LSHIFT))
		{
			if (GetKeyboardPress(DIK_UP))    g_aResultGSPolygon[g_nIdxSelectResultSG].fSizeHeight += 1.0f;
			if (GetKeyboardPress(DIK_DOWN))  g_aResultGSPolygon[g_nIdxSelectResultSG].fSizeHeight -= 1.0f;
			if (GetKeyboardPress(DIK_LEFT))  g_aResultGSPolygon[g_nIdxSelectResultSG].fSizeWidth  -= 1.0f;
			if (GetKeyboardPress(DIK_RIGHT)) g_aResultGSPolygon[g_nIdxSelectResultSG].fSizeWidth  += 1.0f;
		}
		else
		{
			if (GetKeyboardPress(DIK_UP))    g_aResultGS[g_nIdxSelectResultSG].pos.y -= 1.0f;
			if (GetKeyboardPress(DIK_DOWN))  g_aResultGS[g_nIdxSelectResultSG].pos.y += 1.0f;
			if (GetKeyboardPress(DIK_LEFT))  g_aResultGS[g_nIdxSelectResultSG].pos.x -= 1.0f;
			if (GetKeyboardPress(DIK_RIGHT)) g_aResultGS[g_nIdxSelectResultSG].pos.x += 1.0f;
		}
	}

	// デバッグテキスト
	PrintDebugProc("\nUI_RESULTSG_POLYGONNUM %d", g_nNumResultSGPolygon);
	PrintDebugProc("\nUI_RESULTSG_SELECT     %d", g_nIdxSelectResultSG);

	for (int nCntTest = 0; nCntTest < MAX_NUM_RESULTGS; nCntTest++)
	{
		if (g_aResultGSPolygon[nCntTest].bUse == true)
		{
			PrintDebugProc("\nUI_RESULTSG:POLYGON[%d]=====", nCntTest);
			
			if (nCntTest == g_nIdxSelectResultSG)
			{
				PrintDebugProc("\nPOS  %f %f %f", g_aResultGSPolygon[nCntTest].pos.x, g_aResultGSPolygon[nCntTest].pos.y, g_aResultGSPolygon[nCntTest].pos.z);
				PrintDebugProc("\nSIZE %f %f", g_aResultGSPolygon[nCntTest].fSizeWidth, g_aResultGSPolygon[nCntTest].fSizeHeight);
				
			}
		}
	}

	PrintDebugProc("\nUI_RESULT_NUM %d", g_nNumResultGS);
	PrintDebugProc("\nUI_RESULT_END %d", g_nEndResultGS);

#endif
#endif

	// ポリゴン情報の更新
	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{

		if (g_aResultGS[nCntUiResultGS].bUse == false)
		{// 使用してない

			continue;
		}

		if (g_uiResultGSState == UIRESULTGS_STATE_TALLY)
		{// 獲得スコアの状態が終了数集計の状態の場合

			// スコアの値の更新
			if (g_aResultGS[nCntUiResultGS].nUseTimer > 0)
			{
				g_aResultGS[nCntUiResultGS].nUseTimer--;

				if (g_aResultGS[nCntUiResultGS].nUseTimer == 0)
				{
					if (g_aResultGS[nCntUiResultGS].nNowHave < g_aResultGS[nCntUiResultGS].nMaxHave)
					{// 今のスコアが低い場合

						g_aResultGS[nCntUiResultGS].nNowHave++;		// 今のスコアをインクリメント

						// エサの種類別のSEを鳴らす
						EsaPlaySE(GetNowEsaTypeResult());


						if (g_aResultGS[nCntUiResultGS].nNowHave == g_aResultGS[nCntUiResultGS].nMaxHave)
						{// 目標のスコアと同じになった

							g_nEndResultGS++;	// 集計完了数をインクリメント
						}
					}

					g_aResultGS[nCntUiResultGS].nUseTimer = WAIT_UIRESULTSG;
				}
			}
		}
			
		// 桁数を求める
		nDigit = CalcNumDigit(g_aResultGS[nCntUiResultGS].nNowHave);

		// 桁の値を求める
		CalcDigit(g_aResultGS[nCntUiResultGS].nNowHave, nDigit, &aTexU[0], MAX_DIGIT);

		// グループのポリゴンを更新
		for (int nCntPolygon = 0; nCntPolygon < MAX_GLOUP_RESULTGS; nCntPolygon++)
		{
			// インデックスを設定
			nIdx = g_aResultGS[nCntUiResultGS].aIdxPolygon[nCntPolygon];
			
			if (nIdx == -1)
			{// インデックスがない場合

				continue;	// 処理の始めに戻る
			}

			switch (g_aResultGSPolygon[nIdx].type)
			{
			case UI_RESULTGSPOLYGONTYPE_BG:

				if (g_uiResultGSState == UIRESULTGS_STATE_BIGIN)
				{
					if (g_aResultGSPolygon[nIdx].fSizeWLeft <= g_aResultGSPolygon[nIdx].fSizeWidth)
					{
						g_aResultGSPolygon[nIdx].fSizeWLeft += 20.0f;

						if (g_aResultGSPolygon[nIdx].fSizeWLeft >= g_aResultGSPolygon[nIdx].fSizeWidth)
						{// 超えた場合

							g_nEndResultGS++;
						}
					}
				}
				else if (g_uiResultGSState == UIRESULTGS_STATE_END)
				{
					if (g_aResultGSPolygon[nIdx].fSizeWRight <= g_aResultGSPolygon[nIdx].fSizeWidth)
					{
						g_aResultGSPolygon[nIdx].fSizeWRight += 20.0f;

						if (g_aResultGSPolygon[nIdx].fSizeWRight >= g_aResultGSPolygon[nIdx].fSizeWidth)
						{// 超えた場合

							g_nEndResultGS++;
						}
					}
				}

				break;
			}

			// 頂点の更新
			UpdateVtxUiResultGetScore(nCntUiResultGS, nIdx);
		}

		// グループの数値ポリゴンの更新
		for (int nCntPolygon = 0; nCntPolygon < MAX_DIGIT; nCntPolygon++)
		{
			// インデックスを設定
			nIdx = g_aResultGS[nCntUiResultGS].aIdxNumPolygon[nCntPolygon];

			if (nIdx == -1)
			{// インデックスがない場合

				continue;	// 処理の始めに戻る
			}

			// 一桁は一桁だけ表示されるように設定
			if		(nCntPolygon <  nDigit/*&& g_aResultGSPolygon[nIdx].bDisp == false*/)
			{// 桁数よりも小さい場合

				g_aResultGSPolygon[nIdx].bDisp = true;
			}
			else if (nCntPolygon >= nDigit)
			{// 桁数よりも大きい場合

				g_aResultGSPolygon[nIdx].bDisp = false;
			}

			// テクスチャ座標を数値の場所に設定
			g_aResultGSPolygon[nIdx].texPos.x = (1.0f / 11) * aTexU[nCntPolygon];
			g_aResultGSPolygon[nIdx].texPos.y = 0.0f;

			// 頂点の更新
			UpdateVtxUiResultGetScore(nCntUiResultGS, nIdx);
		}
	}

	if		(GetResultState() == RESULTSTATE_WAIT && GetCompletGetScore(UIRESULTGS_STATE_BIGIN))
	{// リザルトの状態が集計中でスコアUIの開始状態が全て完了している場合

		g_uiResultGSState = UIRESULTGS_STATE_TALLY;
		g_nEndResultGS = 0;
	}
	else if (GetResultState() == RESULTSTATE_WAIT && GetCompletGetScore(UIRESULTGS_STATE_TALLY))
	{// 全ての集計が終わった

		g_uiResultGSState = UIRESULTGS_STATE_TALLY_END;
	}

	// 集計終了字のカウンタ
	if (GetCompletGetScore(UIRESULTGS_STATE_TALLY_END))
	{
		g_nTimerResultSG++;

		if (g_nTimerResultSG > 60 * 1)
		{
			g_uiResultGSState = UIRESULTGS_STATE_END;

			g_nTimerResultSG = 0;

			g_nEndResultGS = 0;
		}
	}

}

//========================================================================
// リザルトの獲得スコアのUIの頂点更新処理
//========================================================================
void UpdateVtxUiResultGetScore(int nIdxP,int nIdxC)
{
	// 変数宣言 ===========================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	D3DXVECTOR3 setPos;		// 設定位置
	float fWidth, fHeight;	// ポリゴンの辺の長さ

	// ====================================================

	// 対角線の値を求める
	fWidth = g_aResultGSPolygon[nIdxC].fSizeWidth;		// 幅の長さを求める
	fHeight = g_aResultGSPolygon[nIdxC].fSizeHeight;	// 高さの長さを求める

	// 設定位置を求める
	setPos.x = g_aResultGS[nIdxP].pos.x + g_aResultGSPolygon[nIdxC].pos.x;
	setPos.y = g_aResultGS[nIdxP].pos.y + g_aResultGSPolygon[nIdxC].pos.y;
	setPos.z = 0.0f;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffUiResultGetScore->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += (nIdxC * 4);	// 頂点バッファをインデックス分進める

	// 頂点座標の設定
	if (g_aResultGSPolygon[nIdxC].type == UI_RESULTGSPOLYGONTYPE_BG)
	{
		pVtx[0].pos = D3DXVECTOR3(setPos.x + fWidth - g_aResultGSPolygon[nIdxC].fSizeWLeft, setPos.y - fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(setPos.x + fWidth - g_aResultGSPolygon[nIdxC].fSizeWRight, setPos.y - fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(setPos.x + fWidth - g_aResultGSPolygon[nIdxC].fSizeWLeft, setPos.y + fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(setPos.x + fWidth - g_aResultGSPolygon[nIdxC].fSizeWRight, setPos.y + fHeight, 0.0f);
	}
	else
	{ 
		pVtx[0].pos = D3DXVECTOR3(setPos.x		   , setPos.y - fHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(setPos.x + fWidth, setPos.y - fHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(setPos.x		   , setPos.y + fHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(setPos.x + fWidth, setPos.y + fHeight, 0.0f);
	}
	// 頂点カラーの設定
	pVtx[0].col = g_aResultGS[nIdxP].col;
	pVtx[1].col = g_aResultGS[nIdxP].col;
	pVtx[2].col = g_aResultGS[nIdxP].col;
	pVtx[3].col = g_aResultGS[nIdxP].col;

	// UV座標設定
	pVtx[0].tex.x = g_aResultGSPolygon[nIdxC].texPos.x;
	pVtx[1].tex.x = g_aResultGSPolygon[nIdxC].texPos.x + g_aResultGSPolygon[nIdxC].texSize.x;
	pVtx[2].tex.x = g_aResultGSPolygon[nIdxC].texPos.x;
	pVtx[3].tex.x = g_aResultGSPolygon[nIdxC].texPos.x + g_aResultGSPolygon[nIdxC].texSize.x;

	pVtx[0].tex.y = g_aResultGSPolygon[nIdxC].texPos.y;
	pVtx[1].tex.y = g_aResultGSPolygon[nIdxC].texPos.y;
	pVtx[2].tex.y = g_aResultGSPolygon[nIdxC].texPos.y + g_aResultGSPolygon[nIdxC].texSize.y;
	pVtx[3].tex.y = g_aResultGSPolygon[nIdxC].texPos.y + g_aResultGSPolygon[nIdxC].texSize.y;

	// 頂点バッファをアンロックする
	g_pVtxBuffUiResultGetScore->Unlock();
}

//========================================================================
// リザルトの獲得スコアのUIの描画処理
//========================================================================
void DrawUiResultGetScore(void)
{
	
	// ポリゴンの描画
	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		if (g_aResultGS[nCntUiResultGS].bUse == false)
		{// 使用していない

			continue;	// 処理の始めに戻る
		}

		// インデックスのポリゴンを描画
		for (int nCntPolygon = 0; nCntPolygon < MAX_GLOUP_RESULTGS; nCntPolygon++)
		{// インテックスの数分繰り返す
			
			int nIdx = g_aResultGS[nCntUiResultGS].aIdxPolygon[nCntPolygon];	// インデックスの値を格納

			if (nIdx == -1)
			{
				continue;	// 処理の始めに戻る
			}

			// スコアのUIの描画設定
			SetDrawUiResultGetScore(nIdx);
		}

		// インデックスの数字のポリゴンを描画
		for (int nCntNumber = 0; nCntNumber < MAX_DIGIT; nCntNumber++)
		{
			int nIdx = g_aResultGS[nCntUiResultGS].aIdxNumPolygon[nCntNumber];	// インデックスの値を格納

			if (nIdx == -1)
			{
				continue;	// 処理の始めに戻る
			}

			// スコアのUIの描画設定
			SetDrawUiResultGetScore(nIdx);
		}
	}
}

//========================================================================
// リザルトの獲得スコアのUIの描画設定処理
//========================================================================
void SetDrawUiResultGetScore(int nIdx)
{
	// 変数宣言 ===========================================

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// ====================================================

	if (g_aResultGSPolygon[nIdx].bDisp == false)
	{
		return;
	}

	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffUiResultGetScore, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	if (g_aResultGSPolygon[nIdx].nIdxTexture != -1)
	{// テクスチャがある場合

		pDevice->SetTexture(0, g_apTextureUiResultGetScore[g_aResultGSPolygon[nIdx].nIdxTexture]);
	}
	else
	{// テクスチャがない場合

		pDevice->SetTexture(0, NULL);
	}

	if (g_aResultGSPolygon[nIdx].bAlphaBlend == true)
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

	if (g_aResultGSPolygon[nIdx].bAlphaBlend == true)
	{// aブレンドをする場合

		// 減算合成の設定を元に戻す 
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
	}

	pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

//========================================================================
// リザルトの獲得スコアのUIのポリゴンの設定処理
//========================================================================
int SetUiResultGetScorePolygon(int nIdxTexture, bool bAlphaBlend, UI_RESULTGSPOLYGONTYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot,
							   float fSizeWidth, float fSizeHeight, D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_NUM_RESULTGS; nCntUiResultGS++)
	{
		if (g_aResultGSPolygon[nCntUiResultGS].bUse == true)
		{// 使用している

			continue;	// 処理の始めに戻る
		}

		g_aResultGSPolygon[nCntUiResultGS].nIdxTexture = nIdxTexture;
		g_aResultGSPolygon[nCntUiResultGS].type = type;
		g_aResultGSPolygon[nCntUiResultGS].pos = pos;
		g_aResultGSPolygon[nCntUiResultGS].rot = rot;
		g_aResultGSPolygon[nCntUiResultGS].fSizeWidth = fSizeWidth;
		g_aResultGSPolygon[nCntUiResultGS].fSizeHeight = fSizeHeight;
		g_aResultGSPolygon[nCntUiResultGS].fSizeWLeft = 0.0f;
		g_aResultGSPolygon[nCntUiResultGS].fSizeWRight = 0.0f;
		g_aResultGSPolygon[nCntUiResultGS].texPos = texPos;
		g_aResultGSPolygon[nCntUiResultGS].texSize = texSize;
		g_aResultGSPolygon[nCntUiResultGS].bAlphaBlend = bAlphaBlend;
		g_aResultGSPolygon[nCntUiResultGS].col = col;
		g_aResultGSPolygon[nCntUiResultGS].bDisp = true;
		g_aResultGSPolygon[nCntUiResultGS].bUse = true;

		g_nNumResultSGPolygon++;

		return nCntUiResultGS;
	}

	return -1;
}

//========================================================================
// リザルトの獲得スコアのUIの設定処理
//========================================================================
int SetUiResultGetScore(D3DXVECTOR3 pos, D3DXCOLOR col, int nScore, int nHaveNum)
{
	D3DXVECTOR3 setPos = D3DXVECTOR3(0.0f,0.0f,0.0f);	// ずらす量(前のテクスチャ分)
	D3DXVECTOR2 setTexPos = D3DXVECTOR2(0.0f,0.0f);

	int nIdx;
	int aTexU[MAX_DIGIT] = {};	// テクスチャ座標
	int nNum = 0;				// 計算用数値
	int nDigit;					// 桁数

	//int nCntNumber;
	int nAddSet = 0;			// 追加で設定した数

	int nSetLoop = 0;

	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		if (g_aResultGS[nCntUiResultGS].bUse == true)
		{// 使用している

			continue;	// 処理の始めに戻る
		}

		// リザルトのの獲得スコアの情報を初期化
		g_aResultGS[nCntUiResultGS].pos = pos;																		// 位置
		g_aResultGS[nCntUiResultGS].col = col;																		// 色
		g_aResultGS[nCntUiResultGS].nMaxHave = nHaveNum;															// 総獲得数
		g_aResultGS[nCntUiResultGS].nUseTimer = WAIT_UIRESULTSG;													// カウントの待機時間
		g_aResultGS[nCntUiResultGS].bUse = true;																	// 使用している状態に設定

		memset(&g_aResultGS[nCntUiResultGS].aIdxPolygon[0], -1, sizeof g_aResultGS[nCntUiResultGS].aIdxPolygon);	// ポリゴンインデックスを初期化

		// テンプレのポリゴンを設定
		for (int nCntInfo = 0; nCntInfo < sizeof g_aUiResultGSPolygonInfo / sizeof(g_aUiResultGSPolygonInfo[0]); nCntInfo++)
		{// 設定する数だけ繰り返す

			switch (g_aUiResultGSPolygonInfo[nCntInfo].type)
			{
			case UI_RESULTGSPOLYGONTYPE_NUM_SCORE:
			case UI_RESULTGSPOLYGONTYPE_NUM_GET:

				// 繰り返し設定する回数を設定
				nSetLoop = MAX_DIGIT;	// 桁数分繰り返す
				
				// 設定する値を設定
				if (g_aUiResultGSPolygonInfo[nCntInfo].type == UI_RESULTGSPOLYGONTYPE_NUM_SCORE)
				{// スコアの場合

					nNum = nScore;
				}
				else if (g_aUiResultGSPolygonInfo[nCntInfo].type == UI_RESULTGSPOLYGONTYPE_NUM_GET)
				{// 獲得数の場合

					nNum = nHaveNum;
				}

				// 桁数を求める
				nDigit = CalcNumDigit(nNum);

				// 桁の値を求める
				CalcDigit(nNum, nDigit, &aTexU[0], sizeof aTexU);

				break;

			default:

				// 繰り返し設定する回数を設定
				nSetLoop = 1;			// 1回だけ

				break;
			}

			// ポリゴンの設定
			for (int nCntLoop = 0; nCntLoop < nSetLoop; nCntLoop++)
			{// 桁数分繰り返す

				if (g_aUiResultGSPolygonInfo[nCntInfo].type == UI_RESULTGSPOLYGONTYPE_NUM_SCORE
				 || g_aUiResultGSPolygonInfo[nCntInfo].type == UI_RESULTGSPOLYGONTYPE_NUM_GET)
				{
					// テクスチャの位置を設定
					setTexPos.x = g_aUiResultGSPolygonInfo[nCntInfo].texPos.x + (1.0f / 11) * aTexU[nCntLoop];
					setTexPos.y = g_aUiResultGSPolygonInfo[nCntInfo].texPos.y;
				}
				else
				{
					// テクスチャの位置を設定
					setTexPos.x = g_aUiResultGSPolygonInfo[nCntInfo].texPos.x;
					setTexPos.y = g_aUiResultGSPolygonInfo[nCntInfo].texPos.y;
				}

				// ポリゴンの設定
				nIdx = SetUiResultGetScorePolygon(g_aUiResultGSPolygonInfo[nCntInfo].nIdxTexture,	// テクスチャインデックス
												  g_aUiResultGSPolygonInfo[nCntInfo].bAlphaBlend,	// アルファブレンドするか
												  g_aUiResultGSPolygonInfo[nCntInfo].type, 			// 種類
												  setPos + g_aUiResultGSPolygonInfo[nCntInfo].pos,	// 位置
												  g_aUiResultGSPolygonInfo[nCntInfo].rot, 			// 角度
												  g_aUiResultGSPolygonInfo[nCntInfo].fSizeWidth,	// 幅
												  g_aUiResultGSPolygonInfo[nCntInfo].fSizeHeight,	// 高さ
												  setTexPos,										// テクスチャ座標
												  g_aUiResultGSPolygonInfo[nCntInfo].texSize,		// テクスチャサイズ
												  col);												// 色
				
				// 追加設定時に同じ場所に設定するのの防止
				if (nCntLoop != 0)
				{// 最初じゃない場合

					nAddSet++;	// 追加で設定した分を加算
				}

				// 設定したポリゴンのインデックスを保存
				if(g_aUiResultGSPolygonInfo[nCntInfo].type == UI_RESULTGSPOLYGONTYPE_NUM_GET)
				{// 

					// 数字ポリゴンインデックスに設定したインデックスを代入
					g_aResultGS[nCntUiResultGS].aIdxNumPolygon[nCntLoop] = nIdx;
				}
				else
				{// 数字でない場合

					// ポリゴンインデックスに設定したインデックスを代入
					g_aResultGS[nCntUiResultGS].aIdxPolygon[nCntInfo + nAddSet] = nIdx;
				}

				// 位置をずらす
				if (g_aUiResultGSPolygonInfo[nCntInfo].type != UI_RESULTGSPOLYGONTYPE_BG)
				{// 種類が背景でない場合

					setPos.x += g_aUiResultGSPolygonInfo[nCntInfo].pos.x + g_aUiResultGSPolygonInfo[nCntInfo].fSizeWidth;
				}
				

			}
		}	
		
		g_nNumResultGS++;		// 総数を増やす

		return nCntUiResultGS;	// 設定した場所のインデックスを返す
		
	}

	return -1;						// 場所のないインデックスを返す
}

//========================================================================
// リザルトの獲得スコアのUIの終了処理
//========================================================================
void DelUiResultGetScore(int nIdx)
{
	if (g_aResultGS[nIdx].bUse == true)
	{// 使用している場合

		for (int nCntPolygon = 0; nCntPolygon < MAX_GLOUP_RESULTGS; nCntPolygon++)
		{
			if (g_aResultGS[nIdx].aIdxPolygon[nCntPolygon] != -1)
			{// インデックスがある場合

				g_aResultGSPolygon[g_aResultGS[nIdx].aIdxPolygon[nCntPolygon]].bUse = false;	// インデックスのポリゴンを使用していない状態に設定

				g_nNumResultSGPolygon--;
			}
		}
		g_aResultGS[nIdx].nNowHave = 0;
		g_aResultGS[nIdx].nMaxHave = 0;
		g_aResultGS[nIdx].bUse = false;	// 使用していない状態に設定
	}

	g_uiResultGSState = UIRESULTGS_STATE_BIGIN;

	g_nNumResultGS = 0;
	g_nEndResultGS = 0;
}

//========================================================================
// 桁数を求める処理
//========================================================================
int CalcNumDigit(int nNum)
{
	int nDigit = 0;

	if (nNum == 0)
	{// 値が0の場合

		nDigit = 1;
	}
	else
	{// 値が0でない場合

		for (nDigit = 0; nNum > 0; nDigit++)
		{// 値が0になるまで桁を減らしていく

			nNum /= 10;
		}
	}

	return nDigit;
}

//========================================================================
// 桁の値を求める処理
//========================================================================
void CalcDigit(int nNum, int nDigit, int *pTexU, int nSizeTexU)
{
	int nPow;			// 累乗値

	for (int nCntNumber = 0; nCntNumber < nDigit; nCntNumber++)
	{// 桁数分繰り返す

		if (nCntNumber >= nSizeTexU)
		{// カウンタが配列を超えた場合

			return;
		}

		// 累乗値を求める
		nPow = (int)(pow(10, nDigit - (nCntNumber + 1)));

		// nCntNumber目の桁の値を取り出す
		if (nCntNumber == 0)
		{// 最初の場合

			pTexU[nCntNumber] = nNum			   / nPow;
		}
		else
		{// 最初じゃない場合

			pTexU[nCntNumber] = nNum % (nPow * 10) / nPow;
		}
	}
}

//========================================================================
// 完了したかを返す処理
//========================================================================
bool GetCompletGetScore(UIRESULTGS_STATE state)
{
	if (g_uiResultGSState == state && g_nNumResultGS == g_nEndResultGS)
	{// 全てが目標の値に到達した場合

		return true;
	}
	else
	{
		return false;
	}
}