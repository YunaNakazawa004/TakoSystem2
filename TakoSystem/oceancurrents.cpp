//==================================================================================
// 
// 海流に関する処理[oceancurrents.cpp]
// Author : Soki Inamura 
// 
//==================================================================================

#include "main.h"			// メインヘッダー

#include "game.h"

#include "oceancurrents.h"	// 海流ヘッダー

#include "input.h"
#include "debugproc.h"
#include "camera.h"
#include "sound.h"

#include "ui_result_getscore.h"
#include "player.h"
#include "camera.h"

// マクロ定義 ==================================================

#define MAX_NUM_OCEANCURRENTS		(128)		// ポリゴン数
#define MAX_SET_OCEANCURRENTS		(16)		// 設定出来るポリゴン数
#define MAX_TEX_OCEANCURRENTS		(64)		// 設定出来るテクスチャ数

#define WIRLPOOL_WAITTIME			(15)			// 待機時間の長さ

#define OCEANCURRECT_TIME_NOMAL		(60 * 40)					// 通常状態の継続時間
#define OCEANCURRECT_SPEED_NOMAL	(0.005f)					// 通常時の海流の速さ	

#define OCEANCURRECT_TIME_WAIT		(60 * WIRLPOOL_WAITTIME)	// 渦潮待機状態の継続時間
#define OCEANCURRECT_SPEED_WAIT		(0.006f)					// 渦潮待機時の海流の速さ

#define OCEANCURRECT_TIME_WIRLPOOL	(60 * 7)					// 渦潮状態の継続時間
#define OCEANCURRECT_SPEED_WIRLPOOL	(0.05f)					// 渦潮時の海流の速さ

#define OC_TIMEDIGIT				(2)

// 構造体の定義 ================================================

// 海流のポリゴン情報
typedef struct
{
	OCUITYPE type;				// 種類
	float fValue;				// 量

	int nIdxTexture;			// テクスチャインデックス
	bool bAlphaBlend;			// アルファブレンドするか
								   
	D3DXVECTOR3 offPos;			// オフセット
	float fSizeWidth;			// 幅
	float fSizeHeight;			// 高さ
								   
	D3DXVECTOR2 texPos;			// テクスチャ座標
	D3DXVECTOR2 addTexPos;		// 座標加算量
	D3DXVECTOR2 texSize;		// テクスチャサイズ
								   
	D3DXCOLOR col;				// 色
								   
	bool bUse;					// 使用状態

}OceanCurrentsPolygon;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureOceanCurrents[MAX_TEX_OCEANCURRENTS] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffOceanCurrents = NULL;						// 頂点バッファ

OceanCurrentsPolygon g_aOCPolygon[MAX_NUM_OCEANCURRENTS];					// 海流のポリゴン情報
OceanCurrentsLayout g_aOCLayout[MAX_SET_OCEANCURRENTS];						// 海流のui情報

OCEANCURRENTSSTATE g_OceanCurrentsState;	// 海流の状態
int g_nCounterOceanCurrents;				// 海流カウンター
int g_nMaxTimeOceanCurrents;				// 海流カウンターの元の値
float g_fSpeedOceanCurrent;					// 海流の移動量

int g_nIdxOCWaning;
int g_nIdxOCTimer;

// テクスチャファイル情報
const char* c_apFilenameOceanCurrents[] =
{
	"data/TEXTURE/whirlpool_warning.png",		// [0]渦潮注意標識
	"data/TEXTURE/tex_alpha000.jpg",			// [1]バックテクスチャ
	"data/TEXTURE/tex_warning000.jpg",			// [2]トラテープ
	"data/TEXTURE/text_warning.png",			// [3]テキスト「warning!」
	"data/TEXTURE/text_warning_whirlpool.png",	// [4]テキスト「渦潮注意」
	"data/TEXTURE/text_Hide_behind.png",		// [5]テキスト「物陰に隠れろ」
	"data/TEXTURE/number000.png",				// [6]数字
};

#if 0
OCUI_info g_aOCUiInfo[] =
{// {レイアウトの種類, 種類, テクスチャインデックス, アルファブレンドするか, オフセット, 幅, 高さ, テクスチャ座標, 座標加算量, テクスチャサイズ, 色}
	
	// 上の警告
	//{OCUITYPE_NULL, 3, false, D3DXVECTOR3(0.0f,0.0f,0.0f), 10.0f, 10.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.0f, 0.0f),D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)},

	// 下の警告
	//{1, OCUITYPE_NULL, 3, false, D3DXVECTOR3(0.0f,0.0f,0.0f), 10.0f, 10.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.0f, 0.0f),D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)},
	
	// 右の警告
	//{2, OCUITYPE_NULL, 3, false, D3DXVECTOR3(0.0f,0.0f,0.0f), 10.0f, 10.0f, D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.0f, 0.0f),D3DXVECTOR2(0.0f, 0.0f),D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)},
};
#endif

int g_nMaxLayout = 2;

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

	
	

#if 0
	// テクスチャの読み込み
	for (nCntOC = 0; nCntOC < sizeof c_apFilenameOceanCurrents / sizeof c_apFilenameOceanCurrents[0]; nCntOC++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,								// Direct3Dデバイスへのポインタ
								  c_apFilenameOceanCurrents[nCntOC],	// 読み込むテクスチャ
								  &g_apTextureOceanCurrents[nCntOC]);	// テクスチャへのポインタ
	}
#endif

	// 海流のUI情報を初期化
	for (nCntOC = 0; nCntOC < MAX_SET_OCEANCURRENTS; nCntOC++)
	{
		g_aOCLayout[nCntOC].type = OCUITYPE_NULL;													// UIの種類を初期化
		memset(&g_aOCLayout[nCntOC].aIdxOCPolygon[0], -1, sizeof g_aOCLayout[nCntOC].aIdxOCPolygon);	// ポリゴンのインデックスを初期化
		g_aOCLayout[nCntOC].nNumIdx = 0;															// インデックスの総数を初期化
		g_aOCLayout[nCntOC].fValue = 0.0f;															// インデックスの総数を初期化
		g_aOCLayout[nCntOC].initPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);									// UIの位置を初期化
		g_aOCLayout[nCntOC].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);									// UIの位置を初期化
		g_aOCLayout[nCntOC].fColA = 1.0f;									// UIの位置を初期化
		g_aOCLayout[nCntOC].bDisp = false;															// 表示していない状態に設定
		g_aOCLayout[nCntOC].bUse = false;															// 使用していない状態に設定
	}
	
	// 海流のポリゴン情報を初期化
	for (nCntOC = 0; nCntOC < MAX_NUM_OCEANCURRENTS; nCntOC++)
	{
		g_aOCPolygon[nCntOC].type = OCUITYPE_NULL;						// 種類
		g_aOCPolygon[nCntOC].fValue = 0.0f;								// 量を初期化
		g_aOCPolygon[nCntOC].nIdxTexture = -1;							// テクスチャのインデックスを設定
		g_aOCPolygon[nCntOC].offPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// オフセットの位置を設定
		g_aOCPolygon[nCntOC].fSizeWidth = 0.0f;							// 大きさ(幅)を設定
		g_aOCPolygon[nCntOC].fSizeHeight = 0.0f;						// 大きさ高さ)を設定
		g_aOCPolygon[nCntOC].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 色を設定
		g_aOCPolygon[nCntOC].texPos = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャ座標を初期化
		g_aOCPolygon[nCntOC].addTexPos = D3DXVECTOR2(0.0f, 0.0f);		// テクスチャ座標加算量を初期化
		g_aOCPolygon[nCntOC].texSize = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャサイズを初期化
		g_aOCPolygon[nCntOC].bAlphaBlend = false;						// aブレンドをしない状態に設定
		g_aOCPolygon[nCntOC].bUse = false;								// 使用していない状態に設定
	}

	if (GetMode() == MODE_TUTORIAL)
	{// ゲームのモードがチュートリアルの場合

		g_nCounterOceanCurrents = OCEANCURRECT_TIME_NOMAL - (60 * 15);		// カウンタの値をのこり15秒に設定
	}
	else
	{
		g_nCounterOceanCurrents = 0;										// カウンタの値を初期化
	}

	g_nMaxTimeOceanCurrents = OCEANCURRECT_TIME_NOMAL;
	g_OceanCurrentsState = OCEANCURRENTSSTATE_NOMAL;		// 海流の状態を通常時に設定
	g_fSpeedOceanCurrent = OCEANCURRECT_SPEED_NOMAL;		// 海流の速度を通常時の値で設定

	g_nIdxOCWaning = -1;
	g_nIdxOCTimer = -1;

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

	SetLoadOceanCurrents("data/FILE/ui_oceancurrents.txt");

#if 0
	for (nCntOC = 0; nCntOC < g_nMaxLayout; nCntOC++)
	{
		// 
		//SetOceanCurrentsLayOut(nCntOC, OCUITYPE_NULL, D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));
	}
#endif
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

	g_nMaxTimeOceanCurrents = OCEANCURRECT_TIME_NOMAL;
	g_OceanCurrentsState = OCEANCURRENTSSTATE_NOMAL;		// 海流の状態を通常時に設定
	g_fSpeedOceanCurrent = OCEANCURRECT_SPEED_NOMAL;		// 海流の速度を通常時の値で設定

	g_nIdxOCWaning = -1;
	g_nIdxOCTimer = -1;
}

//========================================================================
// 海流の更新処理　
//========================================================================
void UpdateOceanCurrents(void)
{
	// 変数宣言 ================================================

	//D3DXVECTOR3 SetPos;
	int nIdx;

	int aTexU[OC_TIMEDIGIT] = {};
	int nNum;

	float fGoal = 0.0f;
	float fLength = 0.0f;
	float* pSetPos = NULL;


	float fNum = 0.0f;
	float fPos = 0.0f;

	D3DXVECTOR2 mulTexPos;

	// =========================================================

#ifdef _DEBUG
#if 0
	static int s_nSelectIdxLayType = 0;
	static int s_nSelectIdxPolygon = 0;

	if (GetKeyboardTrigger(DIK_2)) s_nSelectIdxLayType++;
	if (GetKeyboardTrigger(DIK_1)) s_nSelectIdxLayType--;

	if (GetKeyboardTrigger(DIK_4)) s_nSelectIdxPolygon++;
	if (GetKeyboardTrigger(DIK_3)) s_nSelectIdxPolygon--;

	if (s_nSelectIdxLayType != -1 && s_nSelectIdxPolygon != -1 
	 && g_aOCLayout[s_nSelectIdxLayType].bUse == true)
	{
		if (GetKeyboardPress(DIK_B)) g_aOCLayout[s_nSelectIdxLayType].pos.x += 1.0f;
		if (GetKeyboardPress(DIK_V)) g_aOCLayout[s_nSelectIdxLayType].pos.x -= 1.0f;
		if (GetKeyboardPress(DIK_M)) g_aOCLayout[s_nSelectIdxLayType].pos.y += 1.0f;
		if (GetKeyboardPress(DIK_N)) g_aOCLayout[s_nSelectIdxLayType].pos.y -= 1.0f;

		// レイアウトの設定
		if (GetKeyboardPress(DIK_RSHIFT) == false)
		{
			// 移動
			if (GetKeyboardPress(DIK_T)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].offPos.y -= 1.0f;
			if (GetKeyboardPress(DIK_G)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].offPos.y += 1.0f;
			if (GetKeyboardPress(DIK_F)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].offPos.x -= 1.0f;
			if (GetKeyboardPress(DIK_H)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].offPos.x += 1.0f;
		}
		else
		{
			// 大きさ
			if (GetKeyboardPress(DIK_T)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].fSizeHeight += 1.0f;
			if (GetKeyboardPress(DIK_G)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].fSizeHeight -= 1.0f;
			if (GetKeyboardPress(DIK_H)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].fSizeWidth += 1.0f;
			if (GetKeyboardPress(DIK_F)) g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].fSizeWidth -= 1.0f;
		}

		if (GetKeyboardPress(DIK_X))g_aOCLayout[s_nSelectIdxLayType].fValue += 0.01f;
		if (GetKeyboardPress(DIK_Z))g_aOCLayout[s_nSelectIdxLayType].fValue -= 0.01f;
		
		PrintDebugProc("\nOCUI_SELECTLAY[%d]===================================", s_nSelectIdxLayType);
		PrintDebugProc("\nNUM_IDX %d", g_aOCLayout[s_nSelectIdxLayType].nNumIdx);
		PrintDebugProc("\nDISP %s", (g_aOCLayout[s_nSelectIdxLayType].bDisp == true) ? "TRUE":"FALSE");
		PrintDebugProc("\nPOS %f %f %f", g_aOCLayout[s_nSelectIdxLayType].pos.x, g_aOCLayout[s_nSelectIdxLayType].pos.y, g_aOCLayout[s_nSelectIdxLayType].pos.z);
		PrintDebugProc("\nVALUE %f", g_aOCLayout[s_nSelectIdxLayType].fValue);
		PrintDebugProc("\n OCUI_SELECTPOLYGON[%d]------------------------------", s_nSelectIdxPolygon);
		PrintDebugProc("\n OFFPOS  %f %f %f", g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].offPos.x, 
											  g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].offPos.y,
											  g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].offPos.z);
		PrintDebugProc("\n SIZE    %f %f", g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].fSizeWidth,
										   g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].fSizeHeight);
		PrintDebugProc("\n TEXPOS  %f %f", g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].texPos.x, 
										   g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].texPos.y);
		PrintDebugProc("\n TEXSIZE %f %f", g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].texSize.x,
										   g_aOCPolygon[g_aOCLayout[s_nSelectIdxLayType].aIdxOCPolygon[s_nSelectIdxPolygon]].texSize.y);
		//PrintDebugProc("\n\nOCPOLYGON[%d]_WIDTH %f", g_nNumSelect, g_aOCPolygon[g_nNumSelect].offPos.x, g_aOCPolygon[g_nNumSelect].offPos.y, g_aOCPolygon[g_nNumSelect].offPos.z);
	}
#endif
#endif

	// 海流の状態の更新処理
	UpdateOceanCurrentsState();

	// 海流UIの更新
	for (int nCntOC = 0; nCntOC < MAX_SET_OCEANCURRENTS; nCntOC++)
	{
		if (g_aOCLayout[nCntOC].bUse == false)
		{// 使用されていない場合

			continue;	// 処理の始めに戻る
		}

		// レイアウトの種類別の更新
		switch (g_aOCLayout[nCntOC].type)
		{
		case OCUITYPE_LAY_TOPWANING:
		case OCUITYPE_LAY_BOTWANING:
			
			if (g_aOCLayout[nCntOC].type == OCUITYPE_LAY_TOPWANING)
			{
				fNum = 1.0f;			// 値
				fPos = 0.0f;			// 位置(回転軸)
				fLength = 10.0f;		// 距離
			}
			else
			{
				fNum = -1.0f;			// 値
				fPos = SCREEN_HEIGHT;	// 位置(回転軸)
				fLength = 10.0f;		// 距離
			}
			

			if (g_OceanCurrentsState == OCEANCURRENTSSTATE_WAIT 
			 || g_OceanCurrentsState == OCEANCURRENTSSTATE_WIRLPOOL)
			{// 表示する状態の時
				
				fGoal = 1.57f * fNum;
			}
			else
			{
				fGoal = 1.57f * (fNum * -1);
			}

			// 変更する座標を設定
			pSetPos = &g_aOCLayout[nCntOC].pos.y;

			break;

		
		case OCUITYPE_LAY_LEFTWANING:
		case OCUITYPE_LAY_RIGHTWANING:
	
			if (g_aOCLayout[nCntOC].type == OCUITYPE_LAY_LEFTWANING)
			{
				fNum = 1.0f;			// 値
				fPos = 0.0f;			// 位置(回転軸)
				fLength = 35.0f * 2;	// 距離
			}
			else
			{
				fNum = -1.0f;			// 値
				fPos = SCREEN_WIDTH;	// 位置(回転軸)
				fLength = 35.0f * 2;	// 距離
			}

			if (g_OceanCurrentsState == OCEANCURRENTSSTATE_WAIT || g_OceanCurrentsState == OCEANCURRENTSSTATE_WIRLPOOL)
			{// 表示する状態の時

				fGoal = 1.57f * fNum;
			}
			else
			{
				fGoal = 1.57f * (fNum * -1);
			}

			// 変更する座標を設定
			pSetPos = &g_aOCLayout[nCntOC].pos.x;

			break;
		}

		// 目標の値に近づける
		g_aOCLayout[nCntOC].fValue += (fGoal - g_aOCLayout[nCntOC].fValue) * 0.1f;

		// 位置の設定
		if(pSetPos)*pSetPos = fPos + sinf(g_aOCLayout[nCntOC].fValue) * fLength;

		// 裏にいる時は描画しない
		if (g_OceanCurrentsState != OCEANCURRENTSSTATE_WAIT && g_OceanCurrentsState != OCEANCURRENTSSTATE_WIRLPOOL
		 && g_aOCLayout[nCntOC].fValue <= fGoal + 0.1f && g_aOCLayout[nCntOC].fValue >= fGoal - 0.1f)
		{
			g_aOCLayout[nCntOC].bDisp = false;
		}
		else
		{
			g_aOCLayout[nCntOC].bDisp = true;
		}

		// 海流UIのポリゴンの更新
		for (int nCntOCPolygon = 0; nCntOCPolygon < MAX_IDX_OCPOLYGON; nCntOCPolygon++)
		{
			nIdx = g_aOCLayout[nCntOC].aIdxOCPolygon[nCntOCPolygon];	// 設定するインデックスを代入

			mulTexPos = D3DXVECTOR2(1.0f, 1.0f);

			if (nIdx == -1)
			{// 設定されてない場合

				continue;	// 処理の始めに戻る
			}

			switch (g_aOCPolygon[nIdx].type)
			{
			case OCUITYPE_TIMER:
				
				if (g_OceanCurrentsState == OCEANCURRENTSSTATE_WAIT 
				 || g_OceanCurrentsState == OCEANCURRENTSSTATE_WIRLPOOL)
				{
					nNum = g_nMaxTimeOceanCurrents - g_nCounterOceanCurrents;	// 残りの時間を求める

					// 各桁の値を求める
					CalcDigit((nNum <= 0) ? 0 : nNum / 60, 2, &aTexU[0], OC_TIMEDIGIT);

					g_aOCPolygon[nIdx].texPos.x = 0.1f * aTexU[(int)g_aOCPolygon[nIdx].fValue];
				}
				else
				{
					g_aOCPolygon[nIdx].texPos.x = 0.0f;
				}

				break;

			case OCUITYPE_WANING:

				if (g_OceanCurrentsState == OCEANCURRENTSSTATE_WIRLPOOL)
				{
					mulTexPos.x = 6.0f;	// 移動速度を1.5倍
				}

				break;
			}

			// テクスチャ座標を更新
			g_aOCPolygon[nIdx].texPos += D3DXVECTOR2(g_aOCPolygon[nIdx].addTexPos.x * mulTexPos.x, 
													 g_aOCPolygon[nIdx].addTexPos.y * mulTexPos.y);

			// テクスチャ座標の補正
			if (g_aOCPolygon[nIdx].texPos.x >  1.0f) g_aOCPolygon[nIdx].texPos.x -= 1.0f;
			if (g_aOCPolygon[nIdx].texPos.x < -1.0f) g_aOCPolygon[nIdx].texPos.x += 1.0f;
			if (g_aOCPolygon[nIdx].texPos.y >  1.0f) g_aOCPolygon[nIdx].texPos.y -= 1.0f;
			if (g_aOCPolygon[nIdx].texPos.y < -1.0f) g_aOCPolygon[nIdx].texPos.y += 1.0f;
		}

		// レイアウト内の頂点情報を更新
		SetVtxOceanCurrents(&g_aOCLayout[nCntOC]);
	}

#if 1
	PrintDebugProc("\n\nOC_CNT %d",g_nCounterOceanCurrents);
	PrintDebugProc("\nOC_STATE %d", g_OceanCurrentsState);
	PrintDebugProc("\nOC_SPEED %f", g_fSpeedOceanCurrent);
#endif
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
		if (g_aOCLayout[nCntOCUi].bUse == false || g_aOCLayout[nCntOCUi].bDisp == false)
		{// 使用されていない,場合

			continue;	// 処理の始めに戻る
		}

		// 海流UIに登録されたポリゴンの描画
		for (int nCntOCPolygon = 0; nCntOCPolygon < MAX_IDX_OCPOLYGON; nCntOCPolygon++)
		{
			nIdx = g_aOCLayout[nCntOCUi].aIdxOCPolygon[nCntOCPolygon];	// 設定するインデックスを代入

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
// 海流の頂点設定処理
//========================================================================
void SetVtxOceanCurrents(OceanCurrentsLayout *pOCLayout)
{
	// 変数宣言 ================================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	D3DXVECTOR3 SetPos;	//

	// =========================================================

	for (int nCntLayOut = 0; nCntLayOut < pOCLayout->nNumIdx; nCntLayOut++)
	{
		// インデックスを設定
		int nIdx = pOCLayout->aIdxOCPolygon[nCntLayOut];

		if (nIdx == -1)
		{
			continue;
		}

		// 位置を設定
		SetPos.x = pOCLayout->pos.x + g_aOCPolygon[nIdx].offPos.x;
		SetPos.y = pOCLayout->pos.y + g_aOCPolygon[nIdx].offPos.y;
		SetPos.z = 0.0f;

		// ▼頂点バッファをロックして頂点情報へのポインタを所得
		g_pVtxBuffOceanCurrents->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (pOCLayout->aIdxOCPolygon[nCntLayOut] * 4);	// 頂点をインデックス分進める

		// 頂点情報の設定
		pVtx[0].pos.x = SetPos.x - g_aOCPolygon[nIdx].fSizeWidth;
		pVtx[1].pos.x = SetPos.x + g_aOCPolygon[nIdx].fSizeWidth;
		pVtx[2].pos.x = SetPos.x - g_aOCPolygon[nIdx].fSizeWidth;
		pVtx[3].pos.x = SetPos.x + g_aOCPolygon[nIdx].fSizeWidth;

		pVtx[0].pos.y = SetPos.y - g_aOCPolygon[nIdx].fSizeHeight;
		pVtx[1].pos.y = SetPos.y - g_aOCPolygon[nIdx].fSizeHeight;
		pVtx[2].pos.y = SetPos.y + g_aOCPolygon[nIdx].fSizeHeight;
		pVtx[3].pos.y = SetPos.y + g_aOCPolygon[nIdx].fSizeHeight;

		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.z = 0.0f;

		// 頂点カラー
		pVtx[0].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, pOCLayout->fColA);
		pVtx[1].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, pOCLayout->fColA);
		pVtx[2].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, pOCLayout->fColA);
		pVtx[3].col = D3DXCOLOR(g_aOCPolygon[nIdx].col.r, g_aOCPolygon[nIdx].col.g, g_aOCPolygon[nIdx].col.b, pOCLayout->fColA);

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
}

//========================================================================
// 海流のUI設定処理
//========================================================================
int SetOceanCurrentsLayOut(OCUITYPE type, D3DXVECTOR3 pos, OCUI_info* pOCUiInfo, int nSetNum)
{
#if 1
	int aSetIdx[MAX_IDX_OCPOLYGON];	// 同じレイアウトのインデックス
	//int nNumSetIdx = 0;				// インデックスの総数

	memset(&aSetIdx[0], -1, sizeof(int) * MAX_IDX_OCPOLYGON);

	// レイアウトの設定
	for (int nCntOCUi = 0; nCntOCUi < MAX_SET_OCEANCURRENTS; nCntOCUi++)
	{
		if (g_aOCLayout[nCntOCUi].bUse == true)
		{// 使用されている場合

			continue;	// 処理の始めに戻る
		}

		// 海流UIの各情報を設定
		memset(&g_aOCLayout[nCntOCUi].aIdxOCPolygon[0], -1, sizeof g_aOCLayout[nCntOCUi].aIdxOCPolygon);
		g_aOCLayout[nCntOCUi].nNumIdx = 0;						// インデックスの総数を初期化
		g_aOCLayout[nCntOCUi].type = type;						// 種類を設定
		g_aOCLayout[nCntOCUi].fValue = 0.0f;						// 種類を設定
		g_aOCLayout[nCntOCUi].pos = pos;						// 位置を設定
		g_aOCLayout[nCntOCUi].initPos = pos;						// 位置を設定
		g_aOCLayout[nCntOCUi].fColA = 1.0f;						// 透明度を設定
		g_aOCLayout[nCntOCUi].bDisp = true;						// 使用している状態に設定
		g_aOCLayout[nCntOCUi].bUse = true;						// 使用している状態に設定

		int nCntSetIdx = 0;// 設定する箇所

		for(int nCntIdx = 0; nCntIdx < nSetNum; nCntIdx++)
		{
			int nNumSetPolygon = 1;	// ポリゴンの設定数(デフォルト：1)	

			if (pOCUiInfo[nCntIdx].type == OCUITYPE_TIMER)
			{
				nNumSetPolygon = OC_TIMEDIGIT;	// 桁分の設定数を設定
			}

			do
			{// 設定する数だけ繰り返し設定

				// ポリゴンを設定してそのインデックスを獲得
				g_aOCLayout[nCntOCUi].aIdxOCPolygon[nCntSetIdx] = SetOceanCurrentsPolygon((OCUITYPE)pOCUiInfo[nCntIdx].type,
																					   pOCUiInfo[nCntIdx].nIdxTexture,
																					   pOCUiInfo[nCntIdx].bAlphaBlend,
																					   pOCUiInfo[nCntIdx].offPos,
																					   pOCUiInfo[nCntIdx].fSizeWidth,
																					   pOCUiInfo[nCntIdx].fSizeHeight,
																					   pOCUiInfo[nCntIdx].texPos,
																					   pOCUiInfo[nCntIdx].addTexPos,
																					   pOCUiInfo[nCntIdx].texSize,
																					   pOCUiInfo[nCntIdx].col);

				// 総数を増やす
				g_aOCLayout[nCntOCUi].nNumIdx++;

				nCntSetIdx++;		// 設定する箇所をずらす

				nNumSetPolygon--;	// 設定数を減らす

			} while (nNumSetPolygon > 0);
		}
		
		return nCntOCUi;	// 設定した場所のインデックスを返す
	}

#endif
	return -1;				// 設定していない値を返す
}

//========================================================================
// 海流のポリゴン設定処理
//========================================================================
int SetOceanCurrentsPolygon(OCUITYPE type, int nIdxTex, bool bAlphaBlend,
					 D3DXVECTOR3 pos, float fWidth, float fHeight,					
					 D3DXVECTOR2 texPos, D3DXVECTOR2 addTexPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	static int nCntDigit = 0;
	
	D3DXVECTOR3 shiftPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	for (int nCntOceanCurrents = 0; nCntOceanCurrents < MAX_NUM_OCEANCURRENTS; nCntOceanCurrents++)
	{
		if (g_aOCPolygon[nCntOceanCurrents].bUse == true)
		{// 使用されている場合

			continue;	// 処理の始めに戻る
		}

		// 海流のポリゴン情報を設定
		if (type == OCUITYPE_TIMER)
		{
			g_aOCPolygon[nCntOceanCurrents].fValue = (float)nCntDigit;
			
			shiftPos = (type == OCUITYPE_TIMER) ? D3DXVECTOR3((fWidth * 2) * nCntDigit, 0.0f, 0.0f) : D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			
			nCntDigit = (nCntDigit + 1) % OC_TIMEDIGIT;
		}
		else
		{
			g_aOCPolygon[nCntOceanCurrents].fValue = 0.0f;
		}

		g_aOCPolygon[nCntOceanCurrents].type = type;
		g_aOCPolygon[nCntOceanCurrents].nIdxTexture = nIdxTex;			// テクスチャのインデックスを設定
		g_aOCPolygon[nCntOceanCurrents].bAlphaBlend = bAlphaBlend;		// aブレンドの設定状態を設定
		g_aOCPolygon[nCntOceanCurrents].offPos = pos + shiftPos;					// オフセットの位置を設定
		g_aOCPolygon[nCntOceanCurrents].fSizeWidth = fWidth;			// 大きさ(幅)を設定
		g_aOCPolygon[nCntOceanCurrents].fSizeHeight = fHeight;			// 大きさ(高さ)を設定
		g_aOCPolygon[nCntOceanCurrents].col = col;						// 色を設定
		g_aOCPolygon[nCntOceanCurrents].texPos = texPos;				// テクスチャ座標を設定
		g_aOCPolygon[nCntOceanCurrents].addTexPos = addTexPos;			// テクスチャ座標加算量を設定
		g_aOCPolygon[nCntOceanCurrents].texSize = texSize;				// テクスチャサイズを設定
		
			
		g_aOCPolygon[nCntOceanCurrents].bUse = true;					// 使用している状態に設定
		
		return nCntOceanCurrents;	// 設定した場所のインデックスを返す
	}
	 
	return -1;						// 設定していない値を返す
}


//========================================================================
// 海流の状態更新処理
//========================================================================
void UpdateOceanCurrentsState(void)
{
	float fSpeedOceanCurrect = 0.0f;	// 海流の速度

	bool bGamePlayer = true;	// 全てのプレイヤーがゲームモードに入ったか
	
	if (GetMode() == MODE_TUTORIAL)
	{// 今のモードがチュートリアルの場合

		Player* pPlayer = GetPlayer();	// プレイヤーの情報
		int nNumGamePlayer = 0;			// ゲームモードに入ったプレイヤーの数

		for (int nCntPlayer = 0; nCntPlayer < GetNumCamera(); nCntPlayer++)
		{// プレイヤーの数だけ繰り返す

			if (pPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
			{// チュートリアルモードの場合
				continue;
			}

			nNumGamePlayer++;	// ゲームモードに入ったプレイヤー数をインクリメント
		}

		if (GetNumCamera() > nNumGamePlayer)
		{// カメラの数より少ない

			bGamePlayer = false;	// まだゲームモードに入ってないプレイヤーがいる状態に設定
		}
	}

	//g_nCounterOceanCurrents++;	// カウンタを加算

	// 海流の回転速度の処理
	switch (g_OceanCurrentsState)
	{
	case OCEANCURRENTSSTATE_NOMAL:		// 通常時

		// 回転量を通常時の移動量に修正
		g_fSpeedOceanCurrent += (OCEANCURRECT_SPEED_NOMAL - g_fSpeedOceanCurrent) * 0.03f;	// 慣性

		if (GetGameState() != GAMESTATE_LITTLETIME && bGamePlayer == true)
		{// 残り時間が少なくない場合
			
			if (g_fSpeedOceanCurrent <= OCEANCURRECT_SPEED_NOMAL + 0.1f
			 && g_fSpeedOceanCurrent >= OCEANCURRECT_SPEED_NOMAL - 0.1f)
			{// 速さが海流の状態の速さになった

				g_nCounterOceanCurrents++;
			}

			if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_NOMAL)
			{// 通常時の継続時間を過ぎた

				g_OceanCurrentsState = OCEANCURRENTSSTATE_WAIT;			// 渦潮待機状態に設定

				g_nMaxTimeOceanCurrents = OCEANCURRECT_TIME_WAIT;
				g_nCounterOceanCurrents = 0;							// カウンタを初期化
			}
		}

		break;

	case OCEANCURRENTSSTATE_WAIT:		// 渦潮待機時

		// 回転量を渦潮待機時の移動量に修正
		g_fSpeedOceanCurrent += (OCEANCURRECT_SPEED_WAIT - g_fSpeedOceanCurrent) * 0.05f;	// 慣性

		if (g_fSpeedOceanCurrent <= OCEANCURRECT_SPEED_WAIT + 0.1f
		 && g_fSpeedOceanCurrent >= OCEANCURRECT_SPEED_WAIT - 0.1f)
		{// 速さが海流の状態の速さになった

			g_nCounterOceanCurrents++;
		}

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_WAIT)
		{// 渦潮待機時の継続時間を過ぎた
			PlaySound(SOUND_SE_POOL);

			g_OceanCurrentsState = OCEANCURRENTSSTATE_WIRLPOOL;		// 渦潮状態に設定

			g_nMaxTimeOceanCurrents = OCEANCURRECT_TIME_WIRLPOOL;
			g_nCounterOceanCurrents = 0;							// カウンタを初期化

		}

		break;

	case OCEANCURRENTSSTATE_WIRLPOOL:	// 渦潮時

		// 回転量を渦潮時の移動量に修正
		g_fSpeedOceanCurrent += (OCEANCURRECT_SPEED_WIRLPOOL - g_fSpeedOceanCurrent) * 0.05f;	// 慣性

		if (g_fSpeedOceanCurrent <= OCEANCURRECT_SPEED_WIRLPOOL + 0.1f
		 && g_fSpeedOceanCurrent >= OCEANCURRECT_SPEED_WIRLPOOL - 0.1f)
		{// 速さが海流の状態の速さになった

			g_nCounterOceanCurrents++;
		}

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_WIRLPOOL)
		{// 渦潮時の継続時間を過ぎた

			g_OceanCurrentsState = OCEANCURRENTSSTATE_NOMAL;		// 通常状態に設定

			g_nMaxTimeOceanCurrents = OCEANCURRECT_TIME_NOMAL;
			g_nCounterOceanCurrents = 0;							// カウンタを初期化
		}

		if (g_fSpeedOceanCurrent <= OCEANCURRECT_SPEED_WIRLPOOL + 0.01f && g_fSpeedOceanCurrent >= OCEANCURRECT_SPEED_WIRLPOOL - 0.01f)
		{
			PrintDebugProc("OCEAN_SPEED_MAX_WILPOOL");
		}
		break;
	}

	// 回転量を通常時の移動量に修正
	g_fSpeedOceanCurrent += (fSpeedOceanCurrect - g_fSpeedOceanCurrent) * 0.03f;	// 慣性
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

	// ====================================================

	fDistRadius = sqrtf(pPos->x * pPos->x + pPos->z * pPos->z);		// 中心からの距離を求める
	fNomRadius = fDistRadius / OUTCYLINDER_RADIUS;					// posが中心からどれだけ離れているかを求める
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

//========================================================================
// 現在の海流の状態を返す
//========================================================================
void SetLoadOceanCurrents(const char* pFilename)
{
	// 変数宣言 ===========================================

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ

	FILE* pFile;									// ファイルポインタ

	char aReadText[256] = {};						// 読み取ったテキスト
	char aBlank[256] = {};							// 空の読み取り用

	int nNumTexture = 0;							// 読み込むテクスチャ数
	int nCntTexture = 0;							// 読み取ったテクスチャ数

	char aErrorText[256] = {};						// エラー文章

	int nSetLay = -1;
	D3DXVECTOR3 setLayPos;

	OCUI_info aOCPolygonInfo[MAX_IDX_OCPOLYGON];
	int nSetPolygonNum = 0;

	int nTmpNum;

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

		if (strcmp(&aReadText[0], "END_SCRIPT") == 0)
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
				if (FAILED(D3DXCreateTextureFromFile(pDevice,									// デバイス
													 &aReadText[0],								// テクスチャファイル名
													 &g_apTextureOceanCurrents[nCntTexture])))	// テクスチャポインタ
				{// テクスチャの読み込みに失敗

					// エラーメッセージテキストの作成
					sprintf(&aErrorText[0], "テクスチャの読み込みに失敗\n場所 : result\nTEX_FILENAME : %s", &aReadText[0]);

					// ウィンドウの作成
					MessageBox(NULL, &aErrorText[0], "ErrorMessage", MB_OK);
				}
			}

			nCntTexture++;	// テクスチャカウンタをインクリメント

		}
		if (strcmp(&aReadText[0], "SET_LAYOUT") == 0)
		{// SET_LAYOUT

			while (FileExtractText(pFile, &aReadText[0]))
			{// 文字の読み取り

				if (strcmp(&aReadText[0], "END_LAYOUT") == 0)
				{// END_LAYOUT

					break;	// while文を抜ける
				}
				else if (strcmp(&aReadText[0], "SET_LAY[") == 0)
				{
					fscanf(pFile, "%d", &nSetLay);

					nSetPolygonNum = 0;	// 読み込んだ数を初期化

					while (FileExtractText(pFile, &aReadText[0]))
					{// 文字の読み取り

						if (strcmp(&aReadText[0], "END_LAY") == 0)
						{// END_LAY

							if (nSetLay != -1)
							{
								// 海流UIの設定処理
								SetOceanCurrentsLayOut((OCUITYPE)nSetLay, setLayPos, &aOCPolygonInfo[0], nSetPolygonNum);
							}

							nSetLay = -1;	// 

							break;	// while文を抜ける
						}
						else if (strcmp(&aReadText[0], "POS") == 0)
						{// POS

							fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
							fscanf(pFile, "%f %f %f", &setLayPos.x, &setLayPos.y,&setLayPos.z);
						}
						else if (strcmp(&aReadText[0], "SET_POLYGON2D") == 0)
						{// SET_POLYGON2D

							while (FileExtractText(pFile, &aReadText[0]))
							{// 文字の読み取り

								if (strcmp(&aReadText[0], "END_POLYGON2D") == 0)
								{// END_POLYGON2D

									nSetPolygonNum++;

									break;	// while文を抜ける
								}
								else if (strcmp(&aReadText[0], "TYPE") == 0)
								{// TEXIDX

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%d", &aOCPolygonInfo[nSetPolygonNum].type);
								}
								else if (strcmp(&aReadText[0], "TEXIDX") == 0)
								{// TEXIDX

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%d", &aOCPolygonInfo[nSetPolygonNum].nIdxTexture);
								}
								else if (strcmp(&aReadText[0], "BLEND_A") == 0)
								{// BLEND_A

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%d",  &nTmpNum);

									// アルファブレンドをするかを設定
									aOCPolygonInfo[nSetPolygonNum].bAlphaBlend = (nTmpNum == 0) ? false : true;
								}
								else if (strcmp(&aReadText[0], "OFFPOS") == 0)
								{// OFFPOS

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f %f", &aOCPolygonInfo[nSetPolygonNum].offPos.x, &aOCPolygonInfo[nSetPolygonNum].offPos.y, &aOCPolygonInfo[nSetPolygonNum].offPos.z);
								}
								else if (strcmp(&aReadText[0], "SIZE") == 0)
								{// SIZE 

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f", &aOCPolygonInfo[nSetPolygonNum].fSizeWidth, &aOCPolygonInfo[nSetPolygonNum].fSizeHeight);
								}
								else if (strcmp(&aReadText[0], "COL") == 0)
								{// COL

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f %f %f", &aOCPolygonInfo[nSetPolygonNum].col.r, &aOCPolygonInfo[nSetPolygonNum].col.g, &aOCPolygonInfo[nSetPolygonNum].col.b, &aOCPolygonInfo[nSetPolygonNum].col.a);
								}
								else if (strcmp(&aReadText[0], "TEXPOS") == 0)
								{// TEXPOS

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f", &aOCPolygonInfo[nSetPolygonNum].texPos.x, &aOCPolygonInfo[nSetPolygonNum].texPos.y);
								}
								else if (strcmp(&aReadText[0], "TEXADDPOS") == 0)
								{// TEXADDPOS

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f", &aOCPolygonInfo[nSetPolygonNum].addTexPos.x, &aOCPolygonInfo[nSetPolygonNum].addTexPos.y);
								}
								else if (strcmp(&aReadText[0], "TEXSIZE") == 0)
								{// TEXSIZE

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f", &aOCPolygonInfo[nSetPolygonNum].texSize.x, &aOCPolygonInfo[nSetPolygonNum].texSize.y);
								}
							}
						}
						else if (strcmp(&aReadText[0], "SET_TIMELIMIT") == 0)
						{// SET_TIMELIMIT

							while (FileExtractText(pFile, &aReadText[0]))
							{// 文字の読み取り

								if (strcmp(&aReadText[0], "END_TIMELIMIT") == 0)
								{// END_TIMELIMIT

									nSetPolygonNum++;

									break;	// while文を抜ける
								}
								else if (strcmp(&aReadText[0], "TYPE") == 0)
								{// TYPE

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%d", &aOCPolygonInfo[nSetPolygonNum].type);
								}
								else if (strcmp(&aReadText[0], "TEXIDX") == 0)
								{// TEXIDX

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%d", &aOCPolygonInfo[nSetPolygonNum].nIdxTexture);
								}
								else if (strcmp(&aReadText[0], "DIGIT") == 0)
								{// DIGIT

									//fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									//fscanf(pFile, "%d", &aOCPolygonInfo[nSetPolygonNum].);
								}
								else if (strcmp(&aReadText[0], "STARTPOS") == 0)
								{// STARTPOS

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f %f", &aOCPolygonInfo[nSetPolygonNum].offPos.x, &aOCPolygonInfo[nSetPolygonNum].offPos.y, &aOCPolygonInfo[nSetPolygonNum].offPos.z);

								}
								else if (strcmp(&aReadText[0], "SIZE") == 0)
								{// SIZE

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f", &aOCPolygonInfo[nSetPolygonNum].fSizeWidth, &aOCPolygonInfo[nSetPolygonNum].fSizeHeight);
								}
								else if (strcmp(&aReadText[0], "COL") == 0)
								{// COL

									fscanf(pFile, "%[ =\t\n]", &aBlank[0]);						// 余分な情報を読み取る
									fscanf(pFile, "%f %f %f %f", &aOCPolygonInfo[nSetPolygonNum].col.r, &aOCPolygonInfo[nSetPolygonNum].col.g, &aOCPolygonInfo[nSetPolygonNum].col.b, &aOCPolygonInfo[nSetPolygonNum].col.a);

								}

							}
						}
					}
				}
			}
		}
	}
}