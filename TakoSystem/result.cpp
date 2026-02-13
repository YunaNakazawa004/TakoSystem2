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



// マクロ関数 ==================================================

#define	MAX_TEX_RESULT		(32)	// リザルトのテクスチャの最大数
#define	MAX_NUM_RESULT		(32)	// リザルトのポリゴンの最大数
#define	MAX_SET_RESULT		(64)	// リザルトの最大数

#define MAX_DROWLEVEL		(5)		// 描画順位の最大値

// 構造体の定義 ================================================

// リザルトポリゴンの情報
typedef struct
{
	int nIdxTexture;			// テクスチャインデックス

	float fSizeWidth;			// 大きさ(幅)
	float fSizeHeight;			// 大きさ(高さ)

	D3DXCOLOR col;				// 色

	D3DXVECTOR2 texPos;			// テクスチャ(座標)
	D3DXVECTOR2 texSize;		// テクスチャ(サイズ)

	bool bAlphaBlend;			// aブレンドをするか

	bool bUse;					// 使用状態

}ResultPolygon;

// リザルトの情報
typedef struct
{
	bool bPolygon;				// ポリゴンかモデルか
	int nIdx;					// インデックス

	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度

	int nDrowLevel;				// 描画順位

	bool bDisp;					// 表示状態
	bool bUse;					// 使用状態

}Result;

// リザルトポリゴンの情報
typedef struct
{
	int nIdxTexture;			// テクスチャインデックス
	bool bAlphaBlend;			// aブレンドをするか

	float fSizeWidth;			// 大きさ(幅)
	float fSizeHeight;			// 大きさ(高さ)

	D3DXVECTOR2 texPos;			// テクスチャ(座標)
	D3DXVECTOR2 texSize;		// テクスチャ(サイズ)

	D3DXCOLOR col;				// 色

}ResultPolygon_info;

typedef struct
{
	bool bPolygon;				// ポリゴンかモデルか
	int nIdx;					// インデックス
	int nDrowLevel;				// 描画順位

	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度


}Result_info;

// プロトタイプ宣言 ============================================

void DrawResultPolygon(int nIdx);			// リザルトのポリゴンの描画
void DrawResultModel(Result* pResult);		// リザルトのモデルの描画

// グローバル変数 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureResult[MAX_TEX_RESULT] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResult = NULL;				// 頂点バッファへのポインタ

ResultPolygon g_aResultPolygon[MAX_NUM_RESULT];					// リザルトのポリゴン情報

Result g_aResult[MAX_SET_RESULT];

// リザルトの状態
RESULTSTATE g_resultState = RESULTSTATE_BEGIN;					// リザルトの状態
int g_nCounterResultState;										// リザルトの状態カウンター

// ゲームの情報
int g_nMaxPlayer;													// プレイヤーの総数
int g_nMaxEsaType;													// エサの種類の総数
WORD g_aEsaScore[MAX_ESATYPE] = {};									// エサのスコア
WORD g_aNumHaveEsa[MAX_PLAYER + MAX_COMPUTER][MAX_ESATYPE] = {};	// 持っているエサの数

// ファイル名
const char* c_apFilenameResult[] =
{
	"data/TEXTURE/In_the_sea.png",		// [0]背景
	"data/TEXTURE/RESULT.png",			// [1]RESULTテキスト
	"data/TEXTURE/RESULT_LINE.png",		// [2]放射線
	"data/TEXTURE/tex_alpha001.jpg",	// [3]スコア背景
};

ResultPolygon_info g_aResultPolygonInfo[] =
{// {テクスチャインデックス, aブレンドをするか, 幅, 高さ, テクスチャ座標, テクスチャサイズ, 色}

	{0, true, SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1.0f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},	// [0]背景情報
	{3, true, 95.0f, 20.0f, D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(1.0f,1.0f), D3DXCOLOR(1.0f,1.0f,1.0f,1.0f)},								// [1]スコア背景情報
};

Result_info g_aResultInfo[] =
{// {ポリゴンか, セットインデックス, 描画順位, 位置, 角度}

	{true, 0, 2, D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f)},
	{true, 1, 2, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,0.0f,0.0f)},
};

int g_nSelectNum = 0;

bool g_bScreenTransGame = false;

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
	SetLightColor(1, D3DXCOLOR(0.5f, 0.6f, 0.8f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.3f));

	// カメラの数の設定
	SetNumCamera(1);

	// カメラの位置設定
	SetCameraPos(0, D3DXVECTOR3(0.0f, 0.0f, 100.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), CAMERATYPE_POINT);
	
	// エサの初期化
	InitEsa(false);	

	
#if 0
	// ゲームからの設定がされていない場合の設定
	if (g_bScreenTransGame == false)
	{// エサを初期化してモデル情報を獲得

		g_nMaxEsaType = GetNumEsaType();		// エサの種類の総数を獲得

		EsaData* pEsaData = GetEsaData(0);		// エサの種類の情報を獲得
		const int nMaxHave = 10;

		int aHaveNum[MAX_PLAYER][nMaxHave] = {};

		ReceiveResult(false,&*aHaveNum[0], MAX_PLAYER, nMaxHave, &pEsaData[0], g_nMaxEsaType);

		
	}

	g_bScreenTransGame = false;

	// テクスチャの読み込み
	for (nCntResult = 0; nCntResult < sizeof c_apFilenameResult / sizeof(c_apFilenameResult[0]); nCntResult++)
	{
		D3DXCreateTextureFromFile(pDevice,								// デバイス
								  c_apFilenameResult[nCntResult],		// テクスチャファイル名
								  &g_apTextureResult[nCntResult]);		// テクスチャポインタ
	}

	// リザルトのポリゴン情報を初期化
	for (nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		g_aResultPolygon[nCntResult].nIdxTexture = -1;							// テクスチャインデックスを初期化
		g_aResultPolygon[nCntResult].fSizeWidth = 0.0f;							// 大きさ(幅)を初期化
		g_aResultPolygon[nCntResult].fSizeHeight = 0.0f;						// 大きさ(高さ)を初期化
		g_aResultPolygon[nCntResult].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 色を初期化
		g_aResultPolygon[nCntResult].texPos = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャ座標を初期化
		g_aResultPolygon[nCntResult].texSize = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャサイズを初期化
		g_aResultPolygon[nCntResult].bAlphaBlend = false;						// aブレンドをしない状態に設定
		g_aResultPolygon[nCntResult].bUse = false;								// 使用しない状態に設定
	}

	// リザルトの情報を初期化
	for (nCntResult = 0; nCntResult < MAX_SET_RESULT; nCntResult++)
	{
		g_aResult[nCntResult].bPolygon = true;							// ポリゴンに設定
		g_aResult[nCntResult].nIdx = -1;								// インデックスを初期化
		g_aResult[nCntResult].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		g_aResult[nCntResult].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 角度を初期化
		g_aResult[nCntResult].nDrowLevel = 0;							// 描画順位を初期化
		g_aResult[nCntResult].bDisp = false;							// 表示しない状態に設定
		g_aResult[nCntResult].bUse = false;								// 使用しない状態に設定
	}

	g_resultState = RESULTSTATE_BEGIN;	// リザルトの状態を開始状態に設定
	g_nCounterResultState = 0;			// リザルトの状態カウンターを初期化

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
	g_pVtxBuffResult->Unlock();



	// リザルトの設定
	for (nCntResult = 0; nCntResult < (sizeof g_aResultInfo / sizeof(g_aResultInfo[0])); nCntResult++)
	{
		SetResult(g_aResultInfo[nCntResult].bPolygon, g_aResultInfo[nCntResult].nIdx, g_aResultInfo[nCntResult].nDrowLevel, g_aResultInfo[nCntResult].pos, g_aResultInfo[nCntResult].rot);
	}

#endif

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

	memset(&g_aEsaScore[0], 0, MAX_ESATYPE);					// エサの種類別獲得スコアを初期化

	g_nMaxEsaType = 0;											// エサの種類の総数を初期化
	g_nMaxPlayer = 0;											// プレイヤーの総数を初期化

	
	// エサの終了
	UninitEsa();



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
	// フェード情報の取得
	FADE pFade = GetFade();

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	float fWidth, fHeight;	// ポリゴンの辺の長さ
	float fAngle, fLength;	// 対角線の値
#if 0
	if (GetKeyboardTrigger(DIK_L))
	{
		g_aResult[g_nSelectNum].bPolygon = (g_aResult[g_nSelectNum].bPolygon == true) ? false : true;

		if (g_aResult[g_nSelectNum].bPolygon == true)
		{
			g_aResult[g_nSelectNum].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);
		}
		else
		{
			g_aResult[g_nSelectNum].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
	}

	if (GetKeyboardTrigger(DIK_0))
	{
		g_nSelectNum++;
	}
	if (GetKeyboardTrigger(DIK_9))
	{
		g_nSelectNum--;
	}

	if (GetKeyboardTrigger(DIK_8))
	{
		g_aResult[g_nSelectNum].nDrowLevel++;
	}
	if (GetKeyboardTrigger(DIK_7))
	{
		g_aResult[g_nSelectNum].nDrowLevel--;
	}
#endif

#if 0
	FileMovePosion("data/FILE/tmpPosionResult.txt",&g_aResult[g_nSelectNum].pos, 1.0f, DIK_RIGHT, DIK_LEFT, DIK_DOWN, DIK_UP, NULL, NULL);
	
	PrintDebugProc("\nIDXSELECT %d[9,0]", g_nSelectNum);
	PrintDebugProc("\nDRAWLEVEL %d[7,8]", g_aResult[g_nSelectNum].nDrowLevel);
	PrintDebugProc("\nPOS %f %f %f", g_aResult[g_nSelectNum].pos.x, g_aResult[g_nSelectNum].pos.y, g_aResult[g_nSelectNum].pos.z);

	if (g_aResult[g_nSelectNum].bPolygon == true)
	{
		PrintDebugProc("\nTYPE POLYGON[L]");
	}
	else
	{
		PrintDebugProc("\nTYPE MODEL[L]");
	}
	


	// ポリゴンの更新
	for (int nCntResult = 0; nCntResult < MAX_SET_RESULT; nCntResult++)
	{
		if (g_aResult[nCntResult].bUse == true && g_aResult[nCntResult].bPolygon == true)
		{// 使用している場合

			// 対角線の値を求める
			fWidth = g_aResultPolygon[g_aResult[nCntResult].nIdx].fSizeWidth * 2.0f;	// 幅の長さを求める
			fHeight = g_aResultPolygon[g_aResult[nCntResult].nIdx].fSizeHeight * 2.0f;	// 高さの長さを求める
			fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;				// 対角線の長さを求める
			fAngle = atan2f(fWidth, fHeight);											// 対角線の角度を求める

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (g_aResult[nCntResult].nIdx * 4);	// 頂点バッファをインデックス分進める

			// 頂点座標の設定
			pVtx[0].pos.x = g_aResult[nCntResult].pos.x - sinf(fAngle - g_aResult[nCntResult].rot.y) * fLength;
			pVtx[1].pos.x = g_aResult[nCntResult].pos.x + sinf(fAngle + g_aResult[nCntResult].rot.y) * fLength;
			pVtx[2].pos.x = g_aResult[nCntResult].pos.x - sinf(fAngle + g_aResult[nCntResult].rot.y) * fLength;
			pVtx[3].pos.x = g_aResult[nCntResult].pos.x + sinf(fAngle - g_aResult[nCntResult].rot.y) * fLength;

			pVtx[0].pos.y = g_aResult[nCntResult].pos.y - cosf(fAngle - g_aResult[nCntResult].rot.y) * fLength;
			pVtx[1].pos.y = g_aResult[nCntResult].pos.y - cosf(fAngle + g_aResult[nCntResult].rot.y) * fLength;
			pVtx[2].pos.y = g_aResult[nCntResult].pos.y + cosf(fAngle + g_aResult[nCntResult].rot.y) * fLength;
			pVtx[3].pos.y = g_aResult[nCntResult].pos.y + cosf(fAngle - g_aResult[nCntResult].rot.y) * fLength;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// 頂点カラーの設定
			pVtx[0].col = g_aResultPolygon[g_aResult[nCntResult].nIdx].col;	
			pVtx[1].col = g_aResultPolygon[g_aResult[nCntResult].nIdx].col;
			pVtx[2].col = g_aResultPolygon[g_aResult[nCntResult].nIdx].col;
			pVtx[3].col = g_aResultPolygon[g_aResult[nCntResult].nIdx].col;

			// UV座標設定
			pVtx[0].tex.x = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.x;
			pVtx[1].tex.x = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.x + g_aResultPolygon[g_aResult[nCntResult].nIdx].texSize.x;
			pVtx[2].tex.x = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.x;
			pVtx[3].tex.x = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.x + g_aResultPolygon[g_aResult[nCntResult].nIdx].texSize.x;

			pVtx[0].tex.y = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.y;
			pVtx[1].tex.y = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.y;
			pVtx[2].tex.y = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.y + g_aResultPolygon[g_aResult[nCntResult].nIdx].texSize.y;
			pVtx[3].tex.y = g_aResultPolygon[g_aResult[nCntResult].nIdx].texPos.y + g_aResultPolygon[g_aResult[nCntResult].nIdx].texSize.y;

			// 頂点バッファをアンロックする
			g_pVtxBuffResult->Unlock();
		}
	}
#endif

	// 次のモードへの移動処理
	if (pFade == FADE_NONE && g_resultState == RESULTSTAE_WAIT 
	    || (GetKeyboardTrigger(DIK_RETURN) == true || GetJoypadTrigger(0, JOYKEY_START) == true || GetJoypadTrigger(0, JOYKEY_A) == true))
	{// 決定キー（ENTERキー）が押された
		
		// モード設定
		SetFade(MODE_RANKING);
	}
}

//========================================================================
// リザルトの描画処理
//========================================================================
void DrawResult(void)
{
	//DrawBG();

	for (int nCntLevel = 0; nCntLevel < MAX_DROWLEVEL; nCntLevel++)
	{// 描画レベル分繰り返す

		for (int nCntResult = 0; nCntResult < MAX_SET_RESULT; nCntResult++)
		{// リザルトの最大数だけ繰り返す

			if (g_aResult[nCntResult].bUse == true && g_aResult[nCntResult].bDisp == true)
			{// 使用されている && 表示されている

				if (g_aResult[nCntResult].nDrowLevel != nCntLevel)
				{// 描画レベルが同じでない場合

					continue;	// 処理の始めに戻る
				}

				if (g_aResult[nCntResult].bPolygon == true)
				{// 種類がポリゴンの場合

					// ポリゴンの描画処理
					DrawResultPolygon(g_aResult[nCntResult].nIdx);
				}
				else
				{// 種類がモデルの場合

					// モデルの描画処理
					DrawResultModel(&g_aResult[nCntResult]);
				}
				
			}
		}
	}


}

//========================================================================
// リザルトのポリゴンの描画処理
//========================================================================
void DrawResultPolygon(int nIdx)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffResult, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

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
// リザルトのモデルの描画処理
//========================================================================
void DrawResultModel(Result* pResult)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	EsaData *pEsaData = GetEsaData(0);

	D3DXMATRIX mtxWorld;
	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;				// マテリアルのポインタ

	// ====================================================

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);	// ワールドマトリックスの初期値を設定

	// 向きの反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot,
								   pResult->rot.y, pResult->rot.x, pResult->rot.z);

	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 位置を設定
	D3DXMatrixTranslation(&mtxTrans,
						  pResult->pos.x, pResult->pos.y, pResult->pos.z);

	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	// 現在のマテリアルを所得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを所得
	pMat = (D3DXMATERIAL*)pEsaData[pResult->nIdx].model.pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pEsaData[pResult->nIdx].model.dwNumMat; nCntMat++)
	{// マテリアルの数分繰り返す

		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, pEsaData[pResult->nIdx].model.apTexture[nCntMat]);

		// モデルパーツの描画
		pEsaData[pResult->nIdx].model.pMesh->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//========================================================================
// リザルトの設定処理
//========================================================================
int SetResult(bool bPolygon, int nIdx, int nDrowLevel, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	EsaData* pEsaData = GetEsaData(0);

	for (int nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		if (g_aResultPolygon[nCntResult].bUse == false)
		{// 使用していない場合

			g_aResult[nCntResult].bPolygon = bPolygon;
			g_aResult[nCntResult].nDrowLevel = nDrowLevel;
			g_aResult[nCntResult].pos = pos;
			g_aResult[nCntResult].rot = rot;
			g_aResult[nCntResult].bDisp = true;
			g_aResult[nCntResult].bUse = true;
			
			if (g_aResult[nCntResult].bPolygon == true)
			{// ポリゴンの場合

				// ポリゴンの設定
				g_aResult[nCntResult].nIdx = SetResultPolygon(g_aResultPolygonInfo[nIdx].nIdxTexture, g_aResultPolygonInfo[nIdx].bAlphaBlend,	// インデックステクスチャ, aブレンドをするか
															  g_aResultPolygonInfo[nIdx].fSizeWidth, g_aResultPolygonInfo[nIdx].fSizeHeight,	// 幅, 高さ
															  g_aResultPolygonInfo[nIdx].texPos, g_aResultPolygonInfo[nIdx].texSize, 			// テクスチャ座標, テクスチャサイズ
															  g_aResultPolygonInfo[nIdx].col);													// 色
			}
			else
			{// モデルの場合

				if (pEsaData[nIdx].bUse == true)
				{
					g_aResult[nCntResult].nIdx = nIdx;
				}
			}

			g_nSelectNum = nCntResult;

			return nCntResult;
		}
	}

	return -1;
}

//========================================================================
// リザルトのポリゴンの設定処理
//========================================================================
int SetResultPolygon(int nIdxTexture, bool bAlphaBlend, float fSizeWidth, float fSizeHeight,
					  D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	for (int nCntResult = 0; nCntResult < MAX_NUM_RESULT; nCntResult++)
	{
		if (g_aResultPolygon[nCntResult].bUse == false)
		{// 使用していない場合

			// ポリゴンの情報を設定
			g_aResultPolygon[nCntResult].nIdxTexture = nIdxTexture;		// テクスチャインデックスを設定
			g_aResultPolygon[nCntResult].fSizeWidth = fSizeWidth;		// 幅の大きさを設定
			g_aResultPolygon[nCntResult].fSizeHeight = fSizeHeight;		// 高さの大きさを設定
			g_aResultPolygon[nCntResult].col = col;						// 色を設定
			g_aResultPolygon[nCntResult].texPos = texPos;				// 位置を設定
			g_aResultPolygon[nCntResult].texSize = texSize;				// テクスチャ座標を設定
			g_aResultPolygon[nCntResult].bAlphaBlend = bAlphaBlend;		// aブレンドの状態を設定
			g_aResultPolygon[nCntResult].bUse = true;					// 使用している状態に設定

			return nCntResult;	// 設定した場所のインデックスを返す
		}
	}

	return -1;					// 設定していない値を返す
}

//========================================================================
// リザルトの情報を貰う処理
//========================================================================
void ReceiveResult(bool bGame,
				   int* pHaveEsa, int nMaxPlayer, int nMaxHave,
				   EsaData* pEsaData, int nMaxEsaType)
{
	// 変数宣言 ===========================================

	int nCntResult;

	// ====================================================

	g_bScreenTransGame = bGame;		// ゲームから情報を貰ったかを設定

	g_nMaxPlayer = nMaxPlayer;		// プレイヤーの総数を設定
	g_nMaxEsaType = nMaxEsaType;	// エサの種類の総数を設定

	// エサの種類別所持数を初期化
	for (nCntResult = 0; nCntResult < g_nMaxPlayer; nCntResult++)
	{
		
		memset(&g_aNumHaveEsa[nCntResult][0], 0, MAX_ESATYPE);
	}

	// 持っているエサの数の集計
	for (nCntResult = 0; nCntResult < g_nMaxPlayer; nCntResult++)
	{// プレイヤーの総数分繰り返す

		int *pHave = &pHaveEsa[nCntResult];	// nCnt目のプレイヤーの所持情報を設定

		// 持っているエサの所持数を増やす
		for (int nCntHave = 0; nCntHave < nMaxHave; nCntHave++)
		{// 所持数の総数分繰り返す

			if (*pHave == -1)
			{// もっていない場合

				continue;	// 処理の始めに戻る
			}

			g_aNumHaveEsa[nCntResult][*pHave]++;	// 所持しているプレイヤーのエサの種類の場所の値を増やす
		}
	}

	// リザルト用のエサモデルの設定
	for (nCntResult = 0; nCntResult < g_nMaxEsaType; nCntResult++)
	{// エサの種類の数だけ繰り返す

		//g_aEsaModelResult[nCntResult] = pEsaData->model;	// モデルの情報をリザルト用のモデル情報に代入

		g_aEsaScore[nCntResult] = pEsaData->nScore;			// スコアを設定

		pEsaData++;	// モデルのポインタを進める
	}
}