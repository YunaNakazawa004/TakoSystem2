//==================================================================================
// 
// リザルトの獲得スコアのUIに関する処理[ui_result_getscore.cpp]
// Author : Soki Inamura 
// 
//==================================================================================
#include "main.h"					// メインヘッダー
#include "ui_result_getscore.h"		// リザルトの獲得スコアのUI

// マクロ定義 ==================================================

#define MAX_TEX_UI_RESULTGETSCORE	(10)	// 設定出来るテクスチャの総数
#define MAX_NUM_UI_RESULTGETSCORE	(256)	// 設定出来るポリゴンの総数
//#define MAX_SET_UI_RESULTGETSCORE	(256)	// 設定出来るポリゴンの総数

#define MAX_GLOUP_UI_RESULTGETSCORE	(15)	// 設定出来るグループインデックスの総数

#define NUM_TEXBLOCK				(11)	// 数字テクスチャの分割数

// 構造体の定義 ================================================

// リザルトの獲得スコアUIのポリゴン情報
typedef struct
{
	int nIdxTexture;				// テクスチャインデックス
	UI_RESULTGSPOLYGONTYPE type;	// ポリゴンの種類

	D3DXVECTOR3 pos;				// 位置(オフセット)
	D3DXVECTOR3 rot;				// 角度

	float fSizeWidth;				// 大きさ(幅)
	float fSizeHeight;				// 大きさ(高さ)

	float fSizeWLeft;				// 左頂点の水平方向の大きさ
	float fSizeWRight;				// 右頂点の水平方向の大きさ

	D3DXVECTOR2 texPos;				// テクスチャ(座標)
	D3DXVECTOR2 texSize;			// テクスチャ(サイズ)

	bool bAlphaBlend;				// aブレンドをするか

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
	int nIdxPolygon[MAX_GLOUP_UI_RESULTGETSCORE];	// ポリゴンインデックス
	
	D3DXVECTOR3 pos;								// 位置
	D3DXCOLOR col;									// 色

	bool bUse;										// 使用状態

}UiResultGS;

// グローバル変数 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureUiResultGetScore[MAX_TEX_UI_RESULTGETSCORE] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUiResultGetScore = NULL;							// 頂点バッファへのポインタ

UiResultGSPolygon g_aResultGSPolygon[MAX_NUM_UI_RESULTGETSCORE];					// リザルトの獲得スコアUIポリゴンの情報
UiResultGS g_aResultGS[MAX_PLAYER + MAX_COMPUTER];									// リザルトの獲得スコアUIの情報

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
{
	{UI_RESULTGSPOLYGONTYPE_BG, 0, true, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 110.0f, 20.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(1.0f, 1.0f)},				// スコア背景
	{UI_RESULTGSPOLYGONTYPE_NONE, 2, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 20.0f, 20.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(1.0f, 1.0f)},			// プラス記号
	{UI_RESULTGSPOLYGONTYPE_NUM_SCORE, 1, false, D3DXVECTOR3(-10.0f, 2.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 17.0f, 18.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.090f, 1.0f)},	// 獲得スコア
	{UI_RESULTGSPOLYGONTYPE_NONE, 1, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 17.0f, 18.0f, 0.0f, 0.0f, D3DXVECTOR2(0.909f, 0.0f),D3DXVECTOR2(0.090f, 1.0f)},		// ポイント
	{UI_RESULTGSPOLYGONTYPE_NONE, 3, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 18.0f, 20.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(1.0f, 1.0f)},			// 掛ける記号
	{UI_RESULTGSPOLYGONTYPE_NUM_GET, 1, false, D3DXVECTOR3(0.0f, 2.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 17.0f, 18.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.090f, 1.0f)},	// 獲得数

};

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
	for (nCntUiResultGS = 0; nCntUiResultGS < MAX_NUM_UI_RESULTGETSCORE; nCntUiResultGS++)
	{
		g_aResultGSPolygon[nCntUiResultGS].nIdxTexture = -1;
		g_aResultGSPolygon[nCntUiResultGS].type = UI_RESULTGSPOLYGONTYPE_NONE;
		g_aResultGSPolygon[nCntUiResultGS].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aResultGSPolygon[nCntUiResultGS].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aResultGSPolygon[nCntUiResultGS].fSizeWidth = 0.0f;
		g_aResultGSPolygon[nCntUiResultGS].fSizeHeight = 0.0f;
		g_aResultGSPolygon[nCntUiResultGS].fSizeWLeft;
		g_aResultGSPolygon[nCntUiResultGS].fSizeWRight;
		g_aResultGSPolygon[nCntUiResultGS].texPos = D3DXVECTOR2(0.0f,0.0f);
		g_aResultGSPolygon[nCntUiResultGS].texSize = D3DXVECTOR2(0.0f,0.0f);
		g_aResultGSPolygon[nCntUiResultGS].bAlphaBlend = false;
		g_aResultGSPolygon[nCntUiResultGS].bUse = false;
	}

	// 獲得スコアのUI情報の初期化
	for (nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		memset(&g_aResultGS[nCntUiResultGS].nIdxPolygon[0], -1, sizeof g_aResultGS[nCntUiResultGS].nIdxPolygon);
		
		g_aResultGS[nCntUiResultGS].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aResultGS[nCntUiResultGS].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		
		g_aResultGS[nCntUiResultGS].bUse = false;
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_NUM_UI_RESULTGETSCORE,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUiResultGetScore,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// ▼頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffUiResultGetScore->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntUiResultGS = 0; nCntUiResultGS < MAX_NUM_UI_RESULTGETSCORE; nCntUiResultGS++)
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

	// ▲頂点バッファをアンロックする
	g_pVtxBuffUiResultGetScore->Unlock();

	//SetUiResultGetScore(D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), 10, 9);
}

//========================================================================
// リザルトの獲得スコアのUIの終了処理
//========================================================================
void UninitUiResultGetScore(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_UI_RESULTGETSCORE; nCntTexture++)
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
	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	D3DXVECTOR3 setPos, setSize;
	float fWidth, fHeight;	// ポリゴンの辺の長さ
	

	int nIdx;

	// ポリゴン情報の初期化
	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		if (g_aResultGS[nCntUiResultGS].bUse == true)
		{
			for (int nCntPolygon = 0; nCntPolygon < MAX_GLOUP_UI_RESULTGETSCORE; nCntPolygon++)
			{
				nIdx = g_aResultGS[nCntUiResultGS].nIdxPolygon[nCntPolygon];
				
				if (nIdx == -1)
				{
					continue;	// 処理の始めに戻る
				}

				// 対角線の値を求める
				fWidth = g_aResultGSPolygon[nIdx].fSizeWidth * 2.0f;	// 幅の長さを求める
				fHeight = g_aResultGSPolygon[nIdx].fSizeHeight * 2.0f;	// 高さの長さを求める

				setPos.x = g_aResultGS[nCntUiResultGS].pos.x + g_aResultGSPolygon[nIdx].pos.x;
				setPos.y = g_aResultGS[nCntUiResultGS].pos.y + g_aResultGSPolygon[nIdx].pos.y;
				setPos.z = 0.0f;
#if 1

				// 頂点バッファをロックし、頂点情報へのポインタを取得
				g_pVtxBuffUiResultGetScore->Lock(0, 0, (void**)&pVtx, 0);

				pVtx += (nIdx * 4);	// 頂点バッファをインデックス分進める

				// 頂点座標の設定
				pVtx[0].pos.x = setPos.x;
				pVtx[1].pos.x = setPos.x + fWidth;
				pVtx[2].pos.x = setPos.x;
				pVtx[3].pos.x = setPos.x + fWidth;

				pVtx[0].pos.y = setPos.y;
				pVtx[1].pos.y = setPos.y;
				pVtx[2].pos.y = setPos.y + fHeight;
				pVtx[3].pos.y = setPos.y + fHeight;

				pVtx[0].pos.z = setPos.z;
				pVtx[1].pos.z = setPos.z;
				pVtx[2].pos.z = setPos.z;
				pVtx[3].pos.z = setPos.z;

				// 頂点カラーの設定
				pVtx[0].col = g_aResultGS[nCntUiResultGS].col;
				pVtx[1].col = g_aResultGS[nCntUiResultGS].col;
				pVtx[2].col = g_aResultGS[nCntUiResultGS].col;
				pVtx[3].col = g_aResultGS[nCntUiResultGS].col;

				// UV座標設定
				pVtx[0].tex.x = g_aResultGSPolygon[nIdx].texPos.x;
				pVtx[1].tex.x = g_aResultGSPolygon[nIdx].texPos.x + g_aResultGSPolygon[nIdx].texSize.x;
				pVtx[2].tex.x = g_aResultGSPolygon[nIdx].texPos.x;
				pVtx[3].tex.x = g_aResultGSPolygon[nIdx].texPos.x + g_aResultGSPolygon[nIdx].texSize.x;

				pVtx[0].tex.y = g_aResultGSPolygon[nIdx].texPos.y;
				pVtx[1].tex.y = g_aResultGSPolygon[nIdx].texPos.y;
				pVtx[2].tex.y = g_aResultGSPolygon[nIdx].texPos.y + g_aResultGSPolygon[nIdx].texSize.y;
				pVtx[3].tex.y = g_aResultGSPolygon[nIdx].texPos.y + g_aResultGSPolygon[nIdx].texSize.y;

				// 頂点バッファをアンロックする
				g_pVtxBuffUiResultGetScore->Unlock();
#endif
			}
		
		}
	}
}

//========================================================================
// リザルトの獲得スコアのUIの描画処理
//========================================================================
void DrawUiResultGetScore(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffUiResultGetScore, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);
	
	// ポリゴンの描画
	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		if (g_aResultGS[nCntUiResultGS].bUse == false)
		{// 使用していない

			continue;	// 処理の始めに戻る
		}

		// インデックスのポリゴンを描画
		for (int nCntPolygon = 0; nCntPolygon < MAX_GLOUP_UI_RESULTGETSCORE; nCntPolygon++)
		{// インテックスの数分繰り返す
			
			int nIdx = g_aResultGS[nCntUiResultGS].nIdxPolygon[nCntPolygon];	// インデックスの値を格納

			if (nIdx == -1)
			{
				continue;	// 処理の始めに戻る
			}

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
		}
	}
}

//========================================================================
// リザルトの獲得スコアのUIのポリゴンの設定処理
//========================================================================
int SetUiResultGetScorePolygon(int nIdxTexture, bool bAlphaBlend, UI_RESULTGSPOLYGONTYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot,
							   float fSizeWidth, float fSizeHeight, D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_NUM_UI_RESULTGETSCORE; nCntUiResultGS++)
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
		g_aResultGSPolygon[nCntUiResultGS].bUse = true;

		return nCntUiResultGS;
	}

	return -1;
}

//========================================================================
// リザルトの獲得スコアのUIの設定処理
//========================================================================
int SetUiResultGetScore(D3DXVECTOR3 pos, D3DXCOLOR col, int nScore, int nHaveNum)
{
	int nAddSet;										// 追加で設定した数
	D3DXVECTOR3 setPos = D3DXVECTOR3(0.0f,0.0f,0.0f);	// ずらす量(前のテクスチャ分)
	D3DXVECTOR2 setTexPos = D3DXVECTOR2(0.0f,0.0f);

	int nTexU[2] = {};	// テクスチャ座標
	int nNum;			// 計算用数値
	int nDigit;			// 桁数
	int nPow;			// 累乗値

	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		if (g_aResultGS[nCntUiResultGS].bUse == true)
		{// 使用している

			continue;	// 処理の始めに戻る
		}

		// リザルトのの獲得スコアの情報を初期化
		g_aResultGS[nCntUiResultGS].pos = pos;																		// 位置
		g_aResultGS[nCntUiResultGS].col = col;																		// 色
		g_aResultGS[nCntUiResultGS].bUse = true;																	// 使用している状態に設定

		memset(&g_aResultGS[nCntUiResultGS].nIdxPolygon[0], -1, sizeof g_aResultGS[nCntUiResultGS].nIdxPolygon);	// ポリゴンインデックスを初期化

		nAddSet = 0;	// 追加で設定した数

		// テンプレのポリゴンを設定
		for (int nCntPolygon = 0; nCntPolygon < sizeof g_aUiResultGSPolygonInfo / sizeof(g_aUiResultGSPolygonInfo[0]); nCntPolygon++)
		{// 設定する数だけ繰り返す

			if (g_aUiResultGSPolygonInfo[nCntPolygon].type == UI_RESULTGSPOLYGONTYPE_NUM_SCORE 
			 || g_aUiResultGSPolygonInfo[nCntPolygon].type == UI_RESULTGSPOLYGONTYPE_NUM_GET)
			{// 数字系の設定の場合
				
				// 設定する値を設定
				if (g_aUiResultGSPolygonInfo[nCntPolygon].type == UI_RESULTGSPOLYGONTYPE_NUM_SCORE)
				{// スコアの場合

					nNum = nScore;
				}
				else if (g_aUiResultGSPolygonInfo[nCntPolygon].type == UI_RESULTGSPOLYGONTYPE_NUM_GET)
				{// 獲得数の場合

					nNum = nHaveNum;
				}

				// 桁数を求める
				nDigit = nCalcNumDigit(nNum);

				for (int nCntNumber = 0; nCntNumber < nDigit; nCntNumber++)
				{// 桁数分繰り返す

					// 累乗値を求める
					nPow = (int)(pow(10, nDigit - (nCntNumber + 1)));

					// nCntNumber目の桁の値を取り出す
					if (nCntNumber == 0)
					{
						nTexU[nCntNumber] = nNum			   / nPow;
					}
					else
					{
						nTexU[nCntNumber] = nNum % (nPow * 10) / nPow;
					}
					

					// テクスチャの位置を設定
					setTexPos.x = g_aUiResultGSPolygonInfo[nCntPolygon].texPos.x + (1.0f / 11) * nTexU[nCntNumber];	// テクスチャの位置から桁の分ずらす
					setTexPos.y = g_aUiResultGSPolygonInfo[nCntPolygon].texPos.y;

					// ポリゴンの設定(設定したインデックスを代入)
					g_aResultGS[nCntUiResultGS].nIdxPolygon[nCntPolygon + nAddSet] = SetUiResultGetScorePolygon(g_aUiResultGSPolygonInfo[nCntPolygon].nIdxTexture,
																												g_aUiResultGSPolygonInfo[nCntPolygon].bAlphaBlend,
																												g_aUiResultGSPolygonInfo[nCntPolygon].type, 
																												setPos + g_aUiResultGSPolygonInfo[nCntPolygon].pos,
																												g_aUiResultGSPolygonInfo[nCntPolygon].rot, 
																												g_aUiResultGSPolygonInfo[nCntPolygon].fSizeWidth,
																												g_aUiResultGSPolygonInfo[nCntPolygon].fSizeHeight,
																												setTexPos,
																												g_aUiResultGSPolygonInfo[nCntPolygon].texSize,
																												col);

					setPos.x += g_aUiResultGSPolygonInfo[nCntPolygon].pos.x + g_aUiResultGSPolygonInfo[nCntPolygon].fSizeWidth * 2.0f;
					

					nAddSet++;	// 追加で設定した分を加算
				}
			}
			else
			{// 数字でない場合

				// ポリゴンの設定(設定したインデックスを代入)
				g_aResultGS[nCntUiResultGS].nIdxPolygon[nCntPolygon + nAddSet] = SetUiResultGetScorePolygon(g_aUiResultGSPolygonInfo[nCntPolygon].nIdxTexture,
																											g_aUiResultGSPolygonInfo[nCntPolygon].bAlphaBlend,
																											g_aUiResultGSPolygonInfo[nCntPolygon].type, 
																											setPos + g_aUiResultGSPolygonInfo[nCntPolygon].pos,
																											g_aUiResultGSPolygonInfo[nCntPolygon].rot, 
																											g_aUiResultGSPolygonInfo[nCntPolygon].fSizeWidth,
																											g_aUiResultGSPolygonInfo[nCntPolygon].fSizeHeight,
																											g_aUiResultGSPolygonInfo[nCntPolygon].texPos,
																											g_aUiResultGSPolygonInfo[nCntPolygon].texSize,
																											col);

				if (g_aUiResultGSPolygonInfo[nCntPolygon].type != UI_RESULTGSPOLYGONTYPE_BG)
				{// 種類が背景でない場合

					setPos.x += g_aUiResultGSPolygonInfo[nCntPolygon].pos.x + g_aUiResultGSPolygonInfo[nCntPolygon].fSizeWidth * 2.0f;
				}
			}

			
		}

		return nCntUiResultGS;
	}

	return -1;
}

//========================================================================
// リザルトの獲得スコアのUIの終了処理
//========================================================================
void DelUiResultGetScore(int nIdx)
{
	if (g_aResultGS[nIdx].bUse == true)
	{// 使用している場合

		for (int nCntPolygon = 0; nCntPolygon < MAX_GLOUP_UI_RESULTGETSCORE; nCntPolygon++)
		{
			if (g_aResultGS[nIdx].nIdxPolygon[nCntPolygon] != -1)
			{// インデックスがある場合

				g_aResultGSPolygon[g_aResultGS[nIdx].nIdxPolygon[nCntPolygon]].bUse = false;	// インデックスのポリゴンを使用していない状態に設定
			}
		}

		g_aResultGS[nIdx].bUse = false;	// 使用していない状態に設定
	}
}

//========================================================================
// 桁数を求める処理
//========================================================================
int nCalcNumDigit(int nNum)
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

			nNum *= 0.1f;
		}
	}

	return nDigit;
}