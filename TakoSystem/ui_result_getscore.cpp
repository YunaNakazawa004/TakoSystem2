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
	"data/TEXTURE/point.png",			// [0]数字
	"data/TEXTURE/2da_symbol_plus.png",	// [1]プラス記号
	"data/TEXTURE/tex_alpha001.jpg",	// [2]スコア背景
};

// リザルトの獲得スコアUIの設定情報(テンプレ)
UiResultGSPolygon_info g_aUiResultGSPolygonInfo[] =
{
	{UI_RESULTGSPOLYGONTYPE_NONE, 2, true, D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 95.0f, 20.0f, 0.0f, 0.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(1.0f, 1.0f)}
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
}

//========================================================================
// リザルトの獲得スコアのUIの終了処理
//========================================================================
void UninUiResultGetScore(void)
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

	float fWidth, fHeight;	// ポリゴンの辺の長さ
	float fAngle, fLength;	// 対角線の値
	// ポリゴン情報の初期化
	for (int nCntUiResultGS = 0; nCntUiResultGS < MAX_PLAYER + MAX_COMPUTER; nCntUiResultGS++)
	{
		if (g_aResultGS[nCntUiResultGS].bUse == true)
		{
			for (int nCntPolygon = 0; nCntPolygon < MAX_GLOUP_UI_RESULTGETSCORE; nCntPolygon)
			{
				// 対角線の値を求める
				fWidth = g_aResultGSPolygon[g_aResultGS[nCntUiResultGS].nIdxPolygon[nCntPolygon]].fSizeWidth * 2.0f;	// 幅の長さを求める
				fHeight = g_aResultGSPolygon[g_aResultGS[nCntUiResultGS].nIdxPolygon[nCntPolygon]].fSizeHeight * 2.0f;	// 高さの長さを求める
				fLength = sqrtf(fWidth * fWidth + fHeight * fHeight) * 0.5f;											// 対角線の長さを求める
				fAngle = atan2f(fWidth, fHeight);																		// 対角線の角度を求める

#if 0
				// 頂点バッファをロックし、頂点情報へのポインタを取得
				g_pVtxBuffUiResultGetScore->Lock(0, 0, (void**)&pVtx, 0);

				pVtx += (g_aResultGS[nCntUiResultGS].nIdxPolygon[nCntPolygon] * 4);	// 頂点バッファをインデックス分進める

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
		if (g_aResultGS[nCntUiResultGS].bUse == true)
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
	}

	return -1;
}