//==================================================================================
// 
// 海流に関する処理[oceancurrents.cpp]
// Author : Soki Inamura 
// 
//==================================================================================

#include "main.h"			// メインヘッダー

#include "oceancurrents.h"	// 海流ヘッダー

#include "meshcylinder.h"

#include "input.h"
#include "debugproc.h"
#include "camera.h"

// マクロ定義 ==================================================

#define MAX_NUM_OCEANCURRENTS		(128)		// ポリゴン数
#define MAX_SET_OCEANCURRENTS		(16)		// 設定出来るポリゴン数
#define MAX_TEX_OCEANCURRENTS		(64)		// 設定出来るテクスチャ数
#define MAX_IDX_OCPOLYGON			(32)		// 設定出来るインデックス数

#define WIRLPOOL_WAITTIME			(9)			// 待機時間の長さ

#define OCEANCURRECT_TIME_NOMAL		(60 * 20)					// 通常状態の継続時間
#define OCEANCURRECT_SPEED_NOMAL	(0.00005f)					// 通常時の海流の速さ	

#define OCEANCURRECT_TIME_WAIT		(60 * WIRLPOOL_WAITTIME)	// 渦潮待機状態の継続時間
#define OCEANCURRECT_SPEED_WAIT		(0.0001f)					// 渦潮待機時の海流の速さ

#define OCEANCURRECT_TIME_WIRLPOOL	(60 * 7)					// 渦潮状態の継続時間
#define OCEANCURRECT_SPEED_WIRLPOOL	(0.001f)					// 渦潮時の海流の速さ

// 構造体の定義 ================================================

// 海流のポリゴン情報
typedef struct
{
	int nIdxTexture;

	D3DXVECTOR3 offPos;	
	D3DXVECTOR3 rot;
	float fSizeWidth;
	float fSizeHeight;

	D3DXCOLOR col;

	D3DXVECTOR2 texPos;
	D3DXVECTOR2 addtexPos;
	D3DXVECTOR2 texSize;

	bool bAlphaBlend;

	bool bDisp;
	bool bUse;

}OceanCurrentsPolygon;

// 海流のUI情報
typedef struct
{
	OCUITYPE type;							// 種類
	int aIdxOCPolygon[MAX_IDX_OCPOLYGON];	// ポリゴンのインデックス
	int nNumIdx;							// インデックスの総数

	D3DXVECTOR3 pos;						// Uiの位置

	float fSizeMag;							// 倍率
	float fColAlpha;						// 全体のaの値
	int nDiaorTime;							// a値の加算方向or時間

	bool bUse;								// 使用状態

}OceanCurrentsUI;

typedef struct
{
	int nNumPlayer;
	OCUITYPE type;
	int nIdxTexture;

	bool bAlphaBlend;

	D3DXVECTOR3 pos;
	D3DXVECTOR3 rot;
	float fSizeWidth;
	float fSizeHeight;

	D3DXVECTOR2 texPos;
	D3DXVECTOR2 addtexPos;
	D3DXVECTOR2 texSize;
	

	D3DXCOLOR col;

}OCUI_info;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureOceanCurrents[MAX_NUM_OCEANCURRENTS] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffOceanCurrents = NULL;						// 頂点バッファ

OceanCurrentsPolygon g_aOCPolygon[MAX_NUM_OCEANCURRENTS];					// 海流のポリゴン情報
OceanCurrentsUI g_aOCUi[MAX_SET_OCEANCURRENTS];								// 海流のui情報

OCEANCURRENTSSTATE g_OceanCurrentsState;	// 海流の状態
int g_nCounterOceanCurrents;				// 海流カウンター
float g_fSpeedOceanCurrent;					// 海流の移動量

int g_nIdxOCWaning;
int g_nIdxOCTimer;

// テクスチャファイル情報
const char* c_apFilenameOceanCurrents[] =
{
	"data/TEXTURE/ui_OceanCurrents/whirlpool_warning.png",		// [0]渦潮注意標識
	"data/TEXTURE/ui_OceanCurrents/tex_alpha000.jpg",			// [1]バックテクスチャ
	"data/TEXTURE/ui_OceanCurrents/tex_warning000.jpg",			// [2]トラテープ
	"data/TEXTURE/ui_OceanCurrents/text_warning.png",			// [3]テキスト「warning!」
	"data/TEXTURE/ui_OceanCurrents/text_warning_whirlpool.png",	// [4]テキスト「渦潮注意」
	"data/TEXTURE/ui_OceanCurrents/text_Hide_behind.png",		// [5]テキスト「物陰に隠れろ」
	"data/TEXTURE/number000.png",								// [6]数字
};

OCUI_info g_aOCUiInfo[] =
{// {プレイヤー数(あるだけ), 種類, テクスチャ, aブレンド使用状態, オフセットPOS, 角度, 幅, 高さ, テクスチャ座標, テクスチャ座標加算量, テクスチャサイズ, 色}

	{1, OCUITYPE_TIMER,  6, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 50.0f, 50.0f, D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.1f, 1.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f)},	// 1P 時間：数字
	
	{1, OCUITYPE_WANING, 1, true,  D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 180.0f, 40.0f, D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 0, false, D3DXVECTOR3(-140.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 25.0f, 25.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 0, false, D3DXVECTOR3( 140.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 25.0f, 25.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 2, false, D3DXVECTOR3(0.0f, -35.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 180.0f, 5.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(-0.01f, 0.0f), D3DXVECTOR2(10.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 2, false, D3DXVECTOR3(0.0f, 35.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 180.0f, 5.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(-0.01f, 0.0f), D3DXVECTOR2(10.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 3, false, D3DXVECTOR3(0.0f, -35.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 180.0f, 5.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.01f, 0.0f), D3DXVECTOR2(5.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 3, false, D3DXVECTOR3(0.0f, 35.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 180.0f, 5.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.01f, 0.0f), D3DXVECTOR2(5.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 4, false, D3DXVECTOR3(0.0f, 0.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 100.0f, 25.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
	{1, OCUITYPE_WANING, 5, false, D3DXVECTOR3(0.0f, 60.0f, 0.0f),	 D3DXVECTOR3(0.0f, 0.0f, 0.0f), 65.0f, 20.0f,  D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f)},	// 1P 警告：バックテクスチャ
};

int g_nNumSelect;

//========================================================================
// 海流の初期化処理
//========================================================================
void InitOceanCurrents(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntOC;

	// ====================================================

	// テクスチャの読み込み
	for (nCntOC = 0; nCntOC < sizeof c_apFilenameOceanCurrents / sizeof c_apFilenameOceanCurrents[0]; nCntOC++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,								// Direct3Dデバイスへのポインタ
								  c_apFilenameOceanCurrents[nCntOC],	// 読み込むテクスチャ
								  &g_apTextureOceanCurrents[nCntOC]);	// テクスチャへのポインタ
	}

	// 海流のUI情報を初期化
	for (nCntOC = 0; nCntOC < MAX_SET_OCEANCURRENTS; nCntOC++)
	{
		g_aOCUi[nCntOC].nNumIdx = 0;							// インデックスの総数を初期化
		g_aOCUi[nCntOC].type = OCUITYPE_NULL;					// UIの種類を初期化
		g_aOCUi[nCntOC].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// UIの位置を初期化
		g_aOCUi[nCntOC].fSizeMag = 0.0f;						// 倍率を初期化
		g_aOCUi[nCntOC].fColAlpha = 0.0f;						// 透明度を初期化
		g_aOCUi[nCntOC].nDiaorTime = 0;							// 透明度の加算方向or時間を初期化
		g_aOCUi[nCntOC].bUse = false;							// 使用していない状態に設定

		memset(&g_aOCUi[nCntOC].aIdxOCPolygon[0], -1, sizeof g_aOCUi[nCntOC].aIdxOCPolygon);	// ポリゴンのインデックスを初期化
	}
	
	// 海流のポリゴン情報を初期化
	for (nCntOC = 0; nCntOC < MAX_NUM_OCEANCURRENTS; nCntOC++)
	{
		g_aOCPolygon[nCntOC].nIdxTexture = -1;							// テクスチャのインデックスを設定
		g_aOCPolygon[nCntOC].offPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// オフセットの位置を設定
		g_aOCPolygon[nCntOC].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 角度を設定
		g_aOCPolygon[nCntOC].fSizeWidth = 0.0f;							// 大きさ(幅)を設定
		g_aOCPolygon[nCntOC].fSizeHeight = 0.0f;						// 大きさ高さ)を設定
		g_aOCPolygon[nCntOC].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 色を設定
		g_aOCPolygon[nCntOC].texPos = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャ座標を初期化
		g_aOCPolygon[nCntOC].addtexPos = D3DXVECTOR2(0.0f, 0.0f);		// テクスチャ座標加算量を初期化
		g_aOCPolygon[nCntOC].texSize = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャサイズを初期化
		g_aOCPolygon[nCntOC].bAlphaBlend = false;						// aブレンドをしない状態に設定
		g_aOCPolygon[nCntOC].bDisp = false;								// 表示していない状態に設定
		g_aOCPolygon[nCntOC].bUse = false;								// 使用していない状態に設定
	}

	g_nCounterOceanCurrents = 0;							// カウンタの値を初期化
	g_fSpeedOceanCurrent = OCEANCURRECT_SPEED_NOMAL;		// 海流の速度を通常時の値で設定

	g_nIdxOCWaning = -1;
	g_nIdxOCTimer = -1;

	// D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_NUM_OCEANCURRENTS,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffOceanCurrents,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffOceanCurrents->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntOC = 0; nCntOC < MAX_NUM_OCEANCURRENTS; nCntOC++)
	{
		// 頂点情報の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)			 
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)			 
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)			 
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)		 

		// rwhの設定
		pVtx[0].rhw = 1.0f;									// 1.0fで固定
		pVtx[1].rhw = 1.0f;									// 1.0fで固定
		pVtx[2].rhw = 1.0f;									// 1.0fで固定
		pVtx[3].rhw = 1.0f;									// 1.0fで固定

		// 頂点カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);				// (U, V) 
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);				// (U, V)
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);				// (U, V)
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);				// (U, V)

		pVtx += 4;
	}

	// ▲頂点バッファをアンロックする
	g_pVtxBuffOceanCurrents->Unlock();

	
}

//========================================================================
// 海流の終了処理　
//========================================================================
void UninitOceanCurrents(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_OCEANCURRENTS; nCntTexture++)
	{
		if (g_apTextureOceanCurrents[nCntTexture] != NULL)
		{
			g_apTextureOceanCurrents[nCntTexture]->Release();
			g_apTextureOceanCurrents[nCntTexture] = NULL;		// 中身を空にする
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffOceanCurrents != NULL)
	{
		g_pVtxBuffOceanCurrents->Release();
		g_pVtxBuffOceanCurrents = NULL;		// 中身を空にする
	}
}

//========================================================================
// 海流の更新処理　
//========================================================================
void UpdateOceanCurrents(void)
{
	// 変数宣言 ================================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	D3DXVECTOR3 SetPos;
	int nIdx;
	float fAngle, fLength;

	// =========================================================

#if 0
	if (g_nNumSelect != -1)
	{
		if (GetKeyboardPress(DIK_I)) g_aOCUi[g_nNumSelect].pos.y -= 1.0f;
		if (GetKeyboardPress(DIK_K)) g_aOCUi[g_nNumSelect].pos.y += 1.0f;
		if (GetKeyboardPress(DIK_J)) g_aOCUi[g_nNumSelect].pos.x -= 1.0f;
		if (GetKeyboardPress(DIK_L)) g_aOCUi[g_nNumSelect].pos.x += 1.0f;

		if (GetKeyboardPress(DIK_U)) g_aOCUi[g_nNumSelect].fSizeMag += 0.1f;
		if (GetKeyboardPress(DIK_M)) g_aOCUi[g_nNumSelect].fSizeMag -= 0.1f;

		PrintDebugProc("\n==================================");
		PrintDebugProc("\nOCUI[%d]_POS %f %f %f", g_nNumSelect, g_aOCUi[g_nNumSelect].pos.x, g_aOCUi[g_nNumSelect].pos.y, g_aOCUi[g_nNumSelect].pos.z);
		PrintDebugProc("\nOCUI[%d]_MAG %f", g_nNumSelect, g_aOCUi[g_nNumSelect].fSizeMag);
		PrintDebugProc("\nOCUI[%d]_ALPHA %f", g_nNumSelect, g_aOCUi[g_nNumSelect].fColAlpha);

		PrintDebugProc("\n\nOCPOLYGON[%d]_POS %f %f %f", g_nNumSelect, g_aOCPolygon[g_nNumSelect].offPos.x, g_aOCPolygon[g_nNumSelect].offPos.y, g_aOCPolygon[g_nNumSelect].offPos.z);
		//PrintDebugProc("\n\nOCPOLYGON[%d]_WIDTH %f", g_nNumSelect, g_aOCPolygon[g_nNumSelect].offPos.x, g_aOCPolygon[g_nNumSelect].offPos.y, g_aOCPolygon[g_nNumSelect].offPos.z);
	}
#endif

	// 海流の状態の更新処理
	UpdateOceanCurrentsState();

	// 海流UIの更新
	for (int nCntOC = 0; nCntOC < MAX_SET_OCEANCURRENTS; nCntOC++)
	{
		if (g_aOCUi[nCntOC].bUse == false)
		{// 使用されていない場合

			continue;	// 処理の始めに戻る
		}

		switch (g_aOCUi[nCntOC].type)
		{
		case OCUITYPE_WANING:

			g_aOCUi[nCntOC].fColAlpha += 0.03f * g_aOCUi[nCntOC].nDiaorTime;

			if (g_aOCUi[nCntOC].fColAlpha < 0.0f)
			{// 全体のa値が0.0より低くなった場合

				g_aOCUi[nCntOC].fColAlpha = 0.0f;	// a値を0.0に設定

				g_aOCUi[nCntOC].nDiaorTime = 1;	// 加算方向を+に設定
			}
			else if (g_aOCUi[nCntOC].fColAlpha > 1.0f)
			{// 全体のa値が1.0より高くなった場合

				g_aOCUi[nCntOC].fColAlpha = 1.0f;	// a値を1.0に設定

				g_aOCUi[nCntOC].nDiaorTime = -1;	// 加算方向を-に設定
			}

			break;

		case OCUITYPE_TIMER:

			if (g_aOCUi[nCntOC].nDiaorTime > 0)
			{
				g_aOCUi[nCntOC].nDiaorTime--;	// カウンタを減らす

				g_aOCPolygon[g_aOCUi[nCntOC].aIdxOCPolygon[0]].texPos.x = (g_aOCUi[nCntOC].nDiaorTime / 60) * 0.1f;	// テクスチャ座標を設定
			}

			break;
		}

		// 海流UIのポリゴンの更新
		for (int nCntOCPolygon = 0; nCntOCPolygon < MAX_IDX_OCPOLYGON; nCntOCPolygon++)
		{

			nIdx = g_aOCUi[nCntOC].aIdxOCPolygon[nCntOCPolygon];	// 設定するインデックスを代入

			if (nIdx == -1)
			{// 設定されてない場合

				continue;	// 処理の始めに戻る
			}

			// テクスチャ座標を更新
			g_aOCPolygon[nIdx].texPos += g_aOCPolygon[nIdx].addtexPos;

			// 設定に必要な各値を求める
			SetPos.x = g_aOCUi[nCntOC].pos.x + g_aOCPolygon[nIdx].offPos.x * g_aOCUi[nCntOC].fSizeMag;		// 設定する位置(pos)を求める
			SetPos.y = g_aOCUi[nCntOC].pos.y + g_aOCPolygon[nIdx].offPos.y * g_aOCUi[nCntOC].fSizeMag;		// 設定する位置(pos)を求める
			SetPos.z = g_aOCUi[nCntOC].pos.z + g_aOCPolygon[nIdx].offPos.z * g_aOCUi[nCntOC].fSizeMag;		// 設定する位置(pos)を求める

			fLength = sqrtf((g_aOCPolygon[nIdx].fSizeWidth * 2) * (g_aOCPolygon[nIdx].fSizeWidth * 2)		// 対角線の長さを求める
						  + (g_aOCPolygon[nIdx].fSizeHeight * 2) * (g_aOCPolygon[nIdx].fSizeHeight * 2))
						  * 0.5f;

			fAngle = atan2f(g_aOCPolygon[nIdx].fSizeWidth * 2, g_aOCPolygon[nIdx].fSizeHeight * 2);			// 対角線の角度を求める

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffOceanCurrents->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nIdx * 4);	// 頂点をインデックス分進める

			// 頂点情報の設定
			pVtx[0].pos.x = SetPos.x - sinf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);
			pVtx[1].pos.x = SetPos.x + sinf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);
			pVtx[2].pos.x = SetPos.x - sinf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);
			pVtx[3].pos.x = SetPos.x + sinf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);

			pVtx[0].pos.y = SetPos.y - cosf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);
			pVtx[1].pos.y = SetPos.y - cosf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);
			pVtx[2].pos.y = SetPos.y + cosf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);
			pVtx[3].pos.y = SetPos.y + cosf(fAngle + g_aOCPolygon[nIdx].rot.y) * (fLength * g_aOCUi[nCntOC].fSizeMag);

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// 頂点カラー
			pVtx[0].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, g_aOCUi[nCntOC].fColAlpha);	// (R, G, B, A)
			pVtx[1].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, g_aOCUi[nCntOC].fColAlpha);	// (R, G, B, A)
			pVtx[2].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, g_aOCUi[nCntOC].fColAlpha);	// (R, G, B, A)
			pVtx[3].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, g_aOCUi[nCntOC].fColAlpha);	// (R, G, B, A)

			// テクスチャ座標
			pVtx[0].tex.x = g_aOCPolygon[nIdx].texPos.x;
			pVtx[1].tex.x = g_aOCPolygon[nIdx].texPos.x + g_aOCPolygon[nIdx].texSize.x;
			pVtx[2].tex.x = g_aOCPolygon[nIdx].texPos.x;
			pVtx[3].tex.x = g_aOCPolygon[nIdx].texPos.x + g_aOCPolygon[nIdx].texSize.x;

			pVtx[0].tex.y = g_aOCPolygon[nIdx].texPos.y;
			pVtx[1].tex.y = g_aOCPolygon[nIdx].texPos.y;
			pVtx[2].tex.y = g_aOCPolygon[nIdx].texPos.y + g_aOCPolygon[nIdx].texSize.y;
			pVtx[3].tex.y = g_aOCPolygon[nIdx].texPos.y + g_aOCPolygon[nIdx].texSize.y;

			// ▲頂点バッファをアンロックする
			g_pVtxBuffOceanCurrents->Unlock();
		}

		//PrintDebugProc("\nOC[%d]_POS	  %f %f %f", 0, g_aPosOceanCurrents[0].x, g_aPosOceanCurrents[0].y, g_aPosOceanCurrents[0].z);
		//PrintDebugProc("\nOC[%d]_OFFPOS %f %f %f", nCntOceanCurrents, g_aOceanCurrents[nCntOceanCurrents].offPos.x, g_aOceanCurrents[nCntOceanCurrents].offPos.y, g_aOceanCurrents[nCntOceanCurrents].offPos.z);
		//PrintDebugProc("\nOC[%d]_SIZE	  %f %f", nCntOceanCurrents, g_aOceanCurrents[nCntOceanCurrents].fSizeWidth, g_aOceanCurrents[nCntOceanCurrents].fSizeHeight);
	}

	PrintDebugProc("\n\nOC_CNT %d",g_nCounterOceanCurrents);
	PrintDebugProc("\nOC_STATE %d", g_OceanCurrentsState);
	PrintDebugProc("\nOC_SPEED %f", g_fSpeedOceanCurrent);

}

//========================================================================
// 海流の描画処理　
//========================================================================
void DrawOceanCurrents(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nIdx;

	// =========================================================

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffOceanCurrents, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntOCUi = 0; nCntOCUi < MAX_SET_OCEANCURRENTS; nCntOCUi++)
	{
		if (g_aOCUi[nCntOCUi].bUse == false)
		{// 使用されていない,場合

			continue;	// 処理の始めに戻る
		}

		// 海流UIに登録されたポリゴンの描画
		for (int nCntOCPolygon = 0; nCntOCPolygon < MAX_IDX_OCPOLYGON; nCntOCPolygon++)
		{

			nIdx = g_aOCUi[nCntOCUi].aIdxOCPolygon[nCntOCPolygon];	// 設定するインデックスを代入

			if (nIdx == -1)
			{// 設定されてない場合

				continue;	// 処理の始めに戻る
			}

			// テクスチャの設定
			if (g_aOCPolygon[nIdx].nIdxTexture != -1)
			{// テクスチャが設定されている場合

				pDevice->SetTexture(0, g_apTextureOceanCurrents[g_aOCPolygon[nIdx].nIdxTexture]);
			}
			else
			{// テクスチャが設定されていない場合

				pDevice->SetTexture(0, NULL);
			}

			if (g_aOCPolygon[nIdx].bAlphaBlend == true)
			{// aブレンドをする状態の場合

				// 減算合成の設定
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3
			}

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
								   nIdx * 4,				// インデックス
								   2);						// プリミティブ（ポリゴン）数

			if (g_aOCPolygon[nIdx].bAlphaBlend == true)
			{// aブレンドをする状態の場合

				// 減算合成の設定を元に戻す 
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
			}
		}
	}
}

//========================================================================
// 海流のUI設定処理
//========================================================================
int SetOceanCurrentsUi(OCUITYPE type, D3DXVECTOR3 pos, float fSizeMag)
{
	int nSetDiaorTime;	// a値の加算方向or時間の設定値
	float fSetColAlpha;

	for (int nCntOCUi = 0; nCntOCUi < MAX_SET_OCEANCURRENTS; nCntOCUi++)
	{
		if (g_aOCUi[nCntOCUi].bUse == true)
		{// 使用されている場合

			continue;	// 処理の始めに戻る
		}

		// 種類別の設定値を設定
		switch (type)
		{
		case OCUITYPE_WANING:	// 警告
			
			nSetDiaorTime = 1;		// 加算方向を+方向に設定
			
			fSetColAlpha = 0.0f;	// a値の値を0.0fに設定
			
			break;

		case OCUITYPE_TIMER:	// タイマー

			nSetDiaorTime = OCEANCURRECT_TIME_WAIT;	// 待機時間を設定

			fSetColAlpha = 1.0f;					// a値の値を1.0fに設定

			break;

		default:				// それ以外
			
			nSetDiaorTime = 0;		// 設定しない

			fSetColAlpha = 1.0f;	// a値の値を1.0fに設定

			break;
		}

		// 海流UIの各情報を設定
		g_aOCUi[nCntOCUi].nNumIdx = 0;						// インデックスの総数を初期化
		g_aOCUi[nCntOCUi].type = type;						// 種類を設定
		g_aOCUi[nCntOCUi].pos = pos;						// 位置を設定
		g_aOCUi[nCntOCUi].fSizeMag = fSizeMag;				// 倍率を設定
		g_aOCUi[nCntOCUi].fColAlpha = fSetColAlpha;			// 全体のa値を設定
		g_aOCUi[nCntOCUi].nDiaorTime = nSetDiaorTime;		// a値の加算方向を設定
		g_aOCUi[nCntOCUi].bUse = true;						// 使用している状態に設定

		for (int nCntOCSet = 0; nCntOCSet < sizeof g_aOCUiInfo / sizeof(g_aOCUiInfo[0]); nCntOCSet++)
		{
			//if (GetNumCamera() != g_aOCUiInfo[nCntOCSet].nNumPlayer)
			//{// 設定人数が違う場合
			//
			//	continue;	// 処理の始めに戻る
			//}

			if (g_aOCUi[nCntOCUi].type != g_aOCUiInfo[nCntOCSet].type)
			{// 種類が違う場合

				continue;	// 処理の始めに戻る
			}

			// ポリゴンの設定処理
			g_aOCUi[nCntOCUi].aIdxOCPolygon[g_aOCUi[nCntOCUi].nNumIdx] = SetOceanCurrentsPolygon(g_aOCUiInfo[nCntOCSet].nIdxTexture, g_aOCUiInfo[nCntOCSet].bAlphaBlend,
																								 g_aOCUiInfo[nCntOCSet].pos, g_aOCUiInfo[nCntOCSet].rot, 
																								 g_aOCUiInfo[nCntOCSet].fSizeWidth, g_aOCUiInfo[nCntOCSet].fSizeHeight,
																								 g_aOCUiInfo[nCntOCSet].texPos, g_aOCUiInfo[nCntOCSet].addtexPos, g_aOCUiInfo[nCntOCSet].texSize, 
																								 g_aOCUiInfo[nCntOCSet].col);
			
			g_aOCUi[nCntOCUi].nNumIdx++;	// インデックスの総数を増やす
		}
		
		return nCntOCUi;	// 設定した場所のインデックスを返す
	}

	return -1;				// 設定していない値を返す
}

//========================================================================
// 海流のポリゴン設定処理
//========================================================================
int SetOceanCurrentsPolygon(int nIdxTex, bool bAlphaBlend,	
					 D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight,					
					 D3DXVECTOR2 texPos, D3DXVECTOR2 addtexPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{

	for (int nCntOceanCurrents = 0; nCntOceanCurrents < MAX_NUM_OCEANCURRENTS; nCntOceanCurrents++)
	{
		if (g_aOCPolygon[nCntOceanCurrents].bUse == true)
		{// 使用されている場合

			continue;	// 処理の始めに戻る
		}

		// 海流のポリゴン情報を設定
		g_aOCPolygon[nCntOceanCurrents].nIdxTexture = nIdxTex;			// テクスチャのインデックスを設定
		g_aOCPolygon[nCntOceanCurrents].offPos = pos;					// オフセットの位置を設定
		g_aOCPolygon[nCntOceanCurrents].rot = rot;						// 角度を設定
		g_aOCPolygon[nCntOceanCurrents].fSizeWidth = fWidth;			// 大きさ(幅)を設定
		g_aOCPolygon[nCntOceanCurrents].fSizeHeight = fHeight;			// 大きさ(高さ)を設定
		g_aOCPolygon[nCntOceanCurrents].col = col;						// 色を設定
		g_aOCPolygon[nCntOceanCurrents].texPos = texPos;				// テクスチャ座標を設定
		g_aOCPolygon[nCntOceanCurrents].addtexPos = addtexPos;			// テクスチャ座標加算量を設定
		g_aOCPolygon[nCntOceanCurrents].texSize = texSize;				// テクスチャサイズを設定
		g_aOCPolygon[nCntOceanCurrents].bAlphaBlend = bAlphaBlend;		// aブレンドの設定状態を設定
		g_aOCPolygon[nCntOceanCurrents].bDisp = true;					// 表示している状態に設定
		g_aOCPolygon[nCntOceanCurrents].bUse = true;					// 使用している状態に設定

		return nCntOceanCurrents;	// 設定した場所のインデックスを返す
	}
	 
	return -1;						// 設定していない値を返す
}

//========================================================================
// 海流UIの削除処理
//========================================================================
void DelOceanCurrentsUi(int nIdxOCUi)
{
	if (g_aOCUi[nIdxOCUi].bUse == false)
	{// 使用していない場合

		return;		// 処理を抜ける
	}

	g_aOCUi[nIdxOCUi].bUse = false;	// 使用していない状態に設定
	
	// インデックスを初期化
	for (int nCntOC = 0; nCntOC < MAX_IDX_OCPOLYGON; nCntOC++)
	{
		if (g_aOCUi[nIdxOCUi].aIdxOCPolygon[nCntOC] != -1)
		{// 設定されている場合

			g_aOCPolygon[g_aOCUi[nIdxOCUi].aIdxOCPolygon[nCntOC]].bUse = false;	// インデックスのポリゴンを使用していない状態に設定

			g_aOCUi[nIdxOCUi].aIdxOCPolygon[nCntOC] = -1;						// インデックスの値を初期化
		}
	}
	
}

//========================================================================
// 海流の状態更新処理
//========================================================================
void UpdateOceanCurrentsState(void)
{
	g_nCounterOceanCurrents++;	// カウンタを加算

	// 海流の回転速度の処理
	switch (g_OceanCurrentsState)
	{
	case OCEANCURRENTSSTATE_NOMAL:		// 通常時

		// 回転量を通常時の移動量に修正
		g_fSpeedOceanCurrent += OCEANCURRECT_SPEED_NOMAL - g_fSpeedOceanCurrent * 0.03f;	// 慣性

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_NOMAL)
		{// 通常時の継続時間を過ぎた

			g_OceanCurrentsState = OCEANCURRENTSSTATE_WAIT;			// 渦潮待機状態に設定

			g_nCounterOceanCurrents = 0;							// カウンタを初期化

			// 海流UIの設定
			g_nIdxOCWaning = SetOceanCurrentsUi(OCUITYPE_WANING, D3DXVECTOR3(SCREEN_WIDTH * 0.5f, 635.0f, 0.0f), 1.0f);	// 警告
			g_nIdxOCTimer = SetOceanCurrentsUi(OCUITYPE_TIMER, D3DXVECTOR3(SCREEN_WIDTH * 0.5f, 160.0f, 0.0f), 1.0f);	// 時間
		}

		break;

	case OCEANCURRENTSSTATE_WAIT:		// 渦潮待機時

		// 回転量を渦潮待機時の移動量に修正
		g_fSpeedOceanCurrent += OCEANCURRECT_SPEED_WAIT - g_fSpeedOceanCurrent * 0.05f;	// 慣性

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_WAIT)
		{// 渦潮待機時の継続時間を過ぎた

			g_OceanCurrentsState = OCEANCURRENTSSTATE_WIRLPOOL;		// 渦潮状態に設定

			g_nCounterOceanCurrents = 0;							// カウンタを初期化

			if (g_nIdxOCWaning != -1)
			{// インデックスが設定されている場合

				// 海流UIの削除
				DelOceanCurrentsUi(g_nIdxOCWaning);
				DelOceanCurrentsUi(g_nIdxOCTimer);
			}
		}

		break;

	case OCEANCURRENTSSTATE_WIRLPOOL:	// 渦潮時

		// 回転量を渦潮時の移動量に修正
		g_fSpeedOceanCurrent += OCEANCURRECT_SPEED_WIRLPOOL - g_fSpeedOceanCurrent * 0.05f;	// 慣性

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_WIRLPOOL)
		{// 渦潮時の継続時間を過ぎた

			g_OceanCurrentsState = OCEANCURRENTSSTATE_NOMAL;		// 通常状態に設定

			g_nCounterOceanCurrents = 0;							// カウンタを初期化
		}

		break;
	}
}

//=============================================================================
// 中心を軸にした海流の移動処理
//=============================================================================
void MoveOceanCurrents(D3DXVECTOR3* pPos)
{
	// 変数宣言 ===========================================

	float fDistRadius;	// 中心からの距離(半径)
	float fNomRadius;	// 正規化した距離(半径)
	float fNowAngle;	// 現在の角度

	MeshCylinder *pMeshCylinder = GetMeshCylinder();

	// ====================================================

	fDistRadius = sqrtf(pPos->x * pPos->x + pPos->z * pPos->z);		// 中心からの距離を求める
	fNomRadius = fDistRadius / pMeshCylinder[1].size.x;				// posが中心からどれだけ離れているかを求める
	fNowAngle = (float)atan2(pPos->x, pPos->z);						// 中心からの角度を求める

	// 角度を更新
	fNowAngle += g_fSpeedOceanCurrent / fNomRadius;		// 移動量(角度)を正規化した距離の長さにする

	// 角度が超えた場合、角度を範囲内に修正
	if		(fNowAngle < -D3DX_PI) fNowAngle = fNowAngle + D3DX_PI * 2;
	else if (fNowAngle >  D3DX_PI) fNowAngle = fNowAngle - D3DX_PI * 2;

	// 位置を設定
	pPos->x = sinf(fNowAngle) * fDistRadius;
	pPos->z = cosf(fNowAngle) * fDistRadius;
}

//========================================================================
// 現在の海流の状態を返す
//========================================================================
OCEANCURRENTSSTATE GetOceanCurrents(void)
{
	return g_OceanCurrentsState;
}