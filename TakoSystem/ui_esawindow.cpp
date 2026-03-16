//==================================================================================
// 
// エサUIのウィンドウに関する処理[ui_esawindow.cpp]
// Author : Soki Inamura
// 
//==================================================================================

#include "main.h"

#include "ui_esawindow.h"
#include "esa.h"
#include "player.h"
#include "title.h"
#include "input.h"
#include "debugproc.h"

// マクロ定義 ==================================================

#define MAX_TEX_UIESAWINDOW		(32)		// 読み込めるテクスチャの総数
#define MAX_NUM_UIESAWINDOW		(256)		// 用意できる総数
#define MAX_SET_UIESAWINDOW		(16)			// 設定できる総数
#define MAX_IDX_UIESAWINDOW		(16)		// 持てるインデックスの総数
#define MAX_GROUP_UIESAWINDOW	(8)			// 持てるのグループの総数

// 構造体の定義 ================================================

// エサウィンドウの情報
typedef struct
{
	//int* pEsaQueue;							// エサの所持状態のポインタ
	//int nSizeEsaQueue;						// エサの所持状態を記録している場所の大きさ

	int nIdxPlayer;

	D3DXVECTOR3 pos;

	bool bDisp;								// 表示状態
	bool bUse;								// 使用状態

}UiEsaWindow;

// 親の情報
typedef struct
{
	//int* pEsaQueue;							// エサの所持状態のポインタ
	//int nSizeEsaQueue;						// エサの所持状態を記録している場所の大きさ

	int nIdxParent;

	int aIdxPolygon[MAX_IDX_UIESAWINDOW];	// ポリゴンのインデックス
	int nNumIdx;							// インデックスの総数

	int nLabel;

	D3DXVECTOR3 pos;						// 位置

	float fSizeScalar;						// 倍率

	bool bDisp;								// 表示状態
	bool bUse;								// 使用状態

}UiEsaWindowGroup;

// ポリゴンの情報
typedef struct
{
	UIESAWINDOWTYPE type;			// 種類
	int nIdxTexture;				// テクスチャインデックス
	bool bAlphaBlend;				// アルファブレンドするか
	// int nDrawLevel;				// 描画順位
	UIESAWINDOWTYPEPOS posType;		// POSの位置

	int nLabel;

	D3DXVECTOR3 pos;				// 位置
	float fSizeWidth;				// 幅の大きさ
	float fSizeHeight;				// 高さの大きさ

	D3DXCOLOR col;					// 色

	D3DXVECTOR2 texPos;				// テクスチャ座標
	D3DXVECTOR2 texSize;			// テクスチャサイズ

	bool bDisp;						// 表示状態
	bool bUse;						// 使用状態

}UiEsaWindowPolygon;

typedef struct
{
	UIESAWINDOWTYPE nLayoutType;	// レイアウトの種類

	UIESAWINDOWTYPE type;			// 種類
	UIESAWINDOWTYPEPOS posType;		// POSの位置
	int nIdxTexture;				// テクスチャインデックス
	bool bAlphaBlend;				// アルファブレンドするか
	
	D3DXVECTOR3 pos;				// 位置
	float fSizeWidth;				// 幅の大きさ
	float fSizeHeight;				// 高さの大きさ

	D3DXCOLOR col;					// 色

	D3DXVECTOR2 texPos;				// テクスチャ座標
	D3DXVECTOR2 texSize;			// テクスチャサイズ

}UiEsaWindowInfo;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureUiEsaWindow[MAX_TEX_UIESAWINDOW] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUiEsaWindow = NULL;					// 頂点バッファ

UiEsaWindow g_aUiEsaWindow[MAX_PLAYER];

UiEsaWindowGroup g_aUiEsaWindowGloup[MAX_SET_UIESAWINDOW];

UiEsaWindowPolygon g_aUiEsaWindowPolygon[MAX_NUM_UIESAWINDOW];			// ポリゴンの情報
int g_nNumUiEsaWindowPolygon = 0;										// ポリゴンの総数

// テクスチャファイル名
const char* c_apFilenameUiEsaWindow[] =
{
	"data/TEXTURE/texs_oct_esawindow.png",	// [0]ウィンドウテクスチャ
	"data/TEXTURE/text_have_esa.png",		// [0]ウィンドウテキスト
};

UiEsaWindowInfo g_aUiEsaWindowInfo[] =
{
	// ウィンドウ ===============================

	{UIESAWINDOWTYPE_LAY_WINDOW, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(0.0f,-4.0f,0.0f),34.0f,30.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.0f),D3DXVECTOR2(0.5f,0.5f)},	// 頭
	{UIESAWINDOWTYPE_LAY_WINDOW, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(29.0f,90.0f,0.0f),31.0f,79.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(0.5f,1.0f)},
	{UIESAWINDOWTYPE_LAY_WINDOW, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,1,false,D3DXVECTOR3(90.0f,-8.0f,0.0f),68.0f,15.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	
	// 触手 =====================================

	// タコ足
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(-31.0f,25.0f,0.0f),18.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.75f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(156.0f,32.0f,0.0f),187.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.75f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(348.0f,25.0f,0.0f),25.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.75f,0.75f),D3DXVECTOR2(0.25f,0.25f)},
	
	// 吸盤
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 1.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 2.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 3.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 4.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 5.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 6.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 7.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	{UIESAWINDOWTYPE_LAY_LINE, UIESAWINDOWTYPE_NONE,UIESAWINDOWTYPEPOS_CENTER,0,false,D3DXVECTOR3(42.0f * 8.0f,32.0f,0.0f),16.0f,16.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.5f,0.5f),D3DXVECTOR2(0.25f,0.25f)},
	
	// エサテクスチャ ===========================

	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 1,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 2,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 3,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 4,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 5,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 6,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 7,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
	{UIESAWINDOWTYPE_LAY_ESATEX, UIESAWINDOWTYPE_ESATEX,UIESAWINDOWTYPEPOS_CENTER,-1,false,D3DXVECTOR3(42.0f * 8,36.0f,0.0f),20.0f,20.0f,D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),D3DXVECTOR2(0.0f,0.0f),D3DXVECTOR2(1.0f,1.0f)},
};

//========================================================================
// エサUIのウィンドウの初期化処理
//========================================================================
void InitUiEsaWindow(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	HRESULT hr;					// 実行結果
	char aErrerText[64] = {};	// エラーテキスト

	// ====================================================

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameUiEsaWindow / sizeof(c_apFilenameUiEsaWindow[0]); nCntTexture++)
	{
		// テクスチャを読み込む
		hr = D3DXCreateTextureFromFile(pDevice,									// Direct3Dデバイスへのポインタ
									   c_apFilenameUiEsaWindow[nCntTexture],	// 読み込むテクスチャ
									   &g_apTextureUiEsaWindow[nCntTexture]);	// テクスチャへのポインタ

		if (FAILED(hr))
		{// 読み込みに失敗した場合

			// メッセージを作成
			sprintf(&aErrerText[0], "テクスチャの読み込みに失敗\nby InitUiEsaWindow\nTEXFILE : %s\nHRESULT : 0x%x", c_apFilenameUiEsaWindow[nCntTexture], hr);
			
			// メッセージBOXの作成
			MessageBox(NULL, aErrerText, "ErrerMessage", MB_OK);
		}
	}

	for (int nCntUiEsaWindow = 0; nCntUiEsaWindow < MAX_PLAYER; nCntUiEsaWindow++)
	{
		//g_aUiEsaWindow[nCntUiEsaWindow].pEsaQueue = NULL;	// エサキューのポインタを初期化
		//g_aUiEsaWindow[nCntUiEsaWindow].nSizeEsaQueue = 0;	// エサキューの大きさを初期化
		g_aUiEsaWindow[nCntUiEsaWindow].nIdxPlayer = 0;	
		g_aUiEsaWindow[nCntUiEsaWindow].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置を初期化
		g_aUiEsaWindow[nCntUiEsaWindow].bDisp = false;	// 位置を初期化
		g_aUiEsaWindow[nCntUiEsaWindow].bUse = false;	// 位置を初期化
	}

	// グループの情報を初期化
	for (int nCntUiEsaWindow = 0; nCntUiEsaWindow < MAX_SET_UIESAWINDOW; nCntUiEsaWindow++)
	{
		memset(&g_aUiEsaWindowGloup[nCntUiEsaWindow].aIdxPolygon[0],-1,sizeof(int) * MAX_IDX_UIESAWINDOW);	// ポリゴンのインデックスを初期化
		g_aUiEsaWindowGloup[nCntUiEsaWindow].nNumIdx = 0;												// ポリゴンのインデックスの総数を初期化
		g_aUiEsaWindowGloup[nCntUiEsaWindow].nLabel = -1;												// ポリゴンのインデックスの総数を初期化
		g_aUiEsaWindowGloup[nCntUiEsaWindow].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置を初期化
		g_aUiEsaWindowGloup[nCntUiEsaWindow].fSizeScalar = 0.0f;						// 倍率を初期化
		g_aUiEsaWindowGloup[nCntUiEsaWindow].bDisp = false;							// 表示していない状態に設定
		g_aUiEsaWindowGloup[nCntUiEsaWindow].bUse = false;							// 使用していない状態に設定
	}

	// ポリゴンの初期化
	for (int nCntPolygon = 0; nCntPolygon < MAX_NUM_UIESAWINDOW; nCntPolygon++)
	{
		g_aUiEsaWindowPolygon[nCntPolygon].nIdxTexture = -1;						// テクスチャインデックスを初期化
		g_aUiEsaWindowPolygon[nCntPolygon].bAlphaBlend = false;						// アルファブレンドをしていない状態に設定
		g_aUiEsaWindowPolygon[nCntPolygon].posType = UIESAWINDOWTYPEPOS_CENTER;		// POSの位置に設定
		g_aUiEsaWindowPolygon[nCntPolygon].nLabel = -1;								// 
		g_aUiEsaWindowPolygon[nCntPolygon].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		g_aUiEsaWindowPolygon[nCntPolygon].fSizeWidth = 0.0f;						// 幅の大きさを初期化
		g_aUiEsaWindowPolygon[nCntPolygon].fSizeHeight = 0.0f;						// 高さの大きさを初期化
		g_aUiEsaWindowPolygon[nCntPolygon].col = D3DXCOLOR(1.0f,1.0f,1.0f,1.0f);	// 色を初期化
		g_aUiEsaWindowPolygon[nCntPolygon].texPos = D3DXVECTOR2(0.0f,0.0f);			// テクスチャ座標を初期化
		g_aUiEsaWindowPolygon[nCntPolygon].texSize = D3DXVECTOR2(0.0f,0.0f);		// テクスチャサイズを初期化
		g_aUiEsaWindowPolygon[nCntPolygon].bDisp = false;							// 表示していない状態に設定
		g_aUiEsaWindowPolygon[nCntPolygon].bUse = false;							// 使用していない状態に設定
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_NUM_UIESAWINDOW,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_3D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUiEsaWindow,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffUiEsaWindow->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の初期化
	for (int nCntVertex = 0; nCntVertex < MAX_NUM_UIESAWINDOW; nCntVertex++)
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

		pVtx += 4;	// 頂点バッファのポインタを進める
	}

	// ▲頂点バッファをアンロックする
	g_pVtxBuffUiEsaWindow->Unlock();

	SetUiEsaWindow(0,D3DXVECTOR3(100.0f, 50.0f, 0.0f));

	if (GetPlayerSelect() == 2)
	{
		SetUiEsaWindow(1, D3DXVECTOR3(800.0f, 50.0f, 0.0f));
	}
}

//========================================================================
// エサUIのウィンドウの終了処理
//========================================================================
void UninitUiEsaWindow(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_UIESAWINDOW; nCntTexture++)
	{
		if (g_apTextureUiEsaWindow[nCntTexture] != NULL)
		{// 使用している場合

			g_apTextureUiEsaWindow[nCntTexture]->Release();
			g_apTextureUiEsaWindow[nCntTexture] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffUiEsaWindow != NULL)
	{// 使用している場合

		g_pVtxBuffUiEsaWindow->Release();
		g_pVtxBuffUiEsaWindow = NULL;
	}
}

//========================================================================
// エサUIのウィンドウの更新処理
//========================================================================
void UpdateUiEsaWindow(void)
{
	Player* pPlayer = GetPlayer();

	int nIdxPlayer;
	int nIdxPolygon;
	int nLabelGroup;

#if _DEBUG
#if 0
	static int s_nSelectIdxParent = 0;	// 親のインデックスの値
	static int s_nSelectIdxChild  = 0;	// 子のインデックスの値
	
	if (GetKeyboardPress(DIK_LSHIFT) == false)
	{// LSHIFTを押してない

		// 親のインデックスの切り替え
		if (GetKeyboardTrigger(DIK_2)) s_nSelectIdxParent++;
		if (GetKeyboardTrigger(DIK_1)) s_nSelectIdxParent--;
	}
	else
	{// LSHIFTを押してる

		// 子のインデックスの切り替え
		if (GetKeyboardTrigger(DIK_2)) s_nSelectIdxChild++;
		if (GetKeyboardTrigger(DIK_1)) s_nSelectIdxChild--;
	}

	if (s_nSelectIdxParent != -1 && s_nSelectIdxParent < MAX_SET_UIESAWINDOW)
	{// 親のインデックスが範囲内

		// 全体の大きさを変更
		if (GetKeyboardPress(DIK_4))  g_aUiEsaWindowGloup[s_nSelectIdxParent].fSizeScalar += 0.01f;
		if (GetKeyboardPress(DIK_3))  g_aUiEsaWindowGloup[s_nSelectIdxParent].fSizeScalar -= 0.01f;

		if (GetKeyboardPress(DIK_RSHIFT))
		{// RSHIFTが押された

			// 位置の変更
			if (GetKeyboardPress(DIK_I)) g_aUiEsaWindowGloup[s_nSelectIdxParent].pos.y -= 1.0f;
			if (GetKeyboardPress(DIK_K)) g_aUiEsaWindowGloup[s_nSelectIdxParent].pos.y += 1.0f;
			if (GetKeyboardPress(DIK_J)) g_aUiEsaWindowGloup[s_nSelectIdxParent].pos.x -= 1.0f;
			if (GetKeyboardPress(DIK_L)) g_aUiEsaWindowGloup[s_nSelectIdxParent].pos.x += 1.0f;
		}

		// 変更するポリゴンのインデックスを代入
		int nIdxPolygon = g_aUiEsaWindowGloup[s_nSelectIdxParent].aIdxPolygon[s_nSelectIdxChild];

		if (s_nSelectIdxChild != -1)
		{// 子のインデックスがある

			if (GetKeyboardPress(DIK_LSHIFT))
			{// LSHIFTが押された

				// 大きさの変更
				if (GetKeyboardPress(DIK_I)) g_aUiEsaWindowPolygon[nIdxPolygon].fSizeHeight += 1.0f;
				if (GetKeyboardPress(DIK_K)) g_aUiEsaWindowPolygon[nIdxPolygon].fSizeHeight -= 1.0f;
				if (GetKeyboardPress(DIK_J)) g_aUiEsaWindowPolygon[nIdxPolygon].fSizeWidth  -= 1.0f;
				if (GetKeyboardPress(DIK_L)) g_aUiEsaWindowPolygon[nIdxPolygon].fSizeWidth  += 1.0f;
			}
			else if (GetKeyboardPress(DIK_RSHIFT) == false)
			{
				// 位置の変更
				if (GetKeyboardPress(DIK_I)) g_aUiEsaWindowPolygon[nIdxPolygon].pos.y -= 1.0f;
				if (GetKeyboardPress(DIK_K)) g_aUiEsaWindowPolygon[nIdxPolygon].pos.y += 1.0f;
				if (GetKeyboardPress(DIK_J)) g_aUiEsaWindowPolygon[nIdxPolygon].pos.x -= 1.0f;
				if (GetKeyboardPress(DIK_L)) g_aUiEsaWindowPolygon[nIdxPolygon].pos.x += 1.0f;
			}
		}

		PrintDebugProc("\nUIESAWINDOW_SELECTPARENT[%d]===========", s_nSelectIdxParent);
		PrintDebugProc("\nPOS %f %f %f", g_aUiEsaWindowGloup[s_nSelectIdxParent].pos.x, g_aUiEsaWindowGloup[s_nSelectIdxParent].pos.y, g_aUiEsaWindowGloup[s_nSelectIdxParent].pos.z);
		PrintDebugProc("\nSCALAR %f", g_aUiEsaWindowGloup[s_nSelectIdxParent].fSizeScalar);
		PrintDebugProc("\nUIESAWINDOW_SELECTCHILD [%d]===========", s_nSelectIdxChild);
		PrintDebugProc("\nOFFPOS %f %f %f", g_aUiEsaWindowPolygon[nIdxPolygon].pos.x, g_aUiEsaWindowPolygon[nIdxPolygon].pos.y, g_aUiEsaWindowPolygon[nIdxPolygon].pos.z);
		PrintDebugProc("\nSIZE   %f %f", g_aUiEsaWindowPolygon[nIdxPolygon].fSizeWidth, g_aUiEsaWindowPolygon[nIdxPolygon].fSizeHeight);
	}


#endif
#endif

	// ポリゴンの更新
	for (int nCntUiEsaWindow = 0; nCntUiEsaWindow < MAX_SET_UIESAWINDOW; nCntUiEsaWindow++)
	{
		if (g_aUiEsaWindowGloup[nCntUiEsaWindow].bUse == false)
		{// 使用していない場合

			continue;
		}

		nIdxPlayer = g_aUiEsaWindow[g_aUiEsaWindowGloup[nCntUiEsaWindow].nIdxParent].nIdxPlayer;
		nLabelGroup = g_aUiEsaWindowGloup[nCntUiEsaWindow].nLabel;

		for (int nCntIdx = 0; nCntIdx < MAX_IDX_UIESAWINDOW; nCntIdx++)
		{
			// インデックスを設定
			nIdxPolygon = g_aUiEsaWindowGloup[nCntUiEsaWindow].aIdxPolygon[nCntIdx];

			if (nIdxPolygon == -1)
			{
				continue;
			}

			// 種類別の更新
			switch (g_aUiEsaWindowPolygon[nIdxPolygon].type)
			{
			case UIESAWINDOWTYPE_NONE:

				break;

			case UIESAWINDOWTYPE_ESATEX:

				if (nLabelGroup != -1 && g_aUiEsaWindowPolygon[nIdxPolygon].nLabel != -1)
				{
					g_aUiEsaWindowPolygon[nIdxPolygon].nIdxTexture = pPlayer[nIdxPlayer].esaQueue.nData[g_aUiEsaWindowPolygon[nIdxPolygon].nLabel + (nLabelGroup * 8)];
				}

				break;
			}

			// 頂点情報の設定
			SetVtxUiEsaWindowPolygon(nIdxPolygon, g_aUiEsaWindowGloup[nCntUiEsaWindow].pos, g_aUiEsaWindowGloup[nCntUiEsaWindow].fSizeScalar);
			
		}
	}
}

//========================================================================
// エサUIのウィンドウの描画処理
//========================================================================
void DrawUiEsaWindow(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	Player* pPlayer = GetPlayer();
	EsaData* pEsaData = GetEsaData(0);

	// =========================================================

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffUiEsaWindow, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntUiEsaWindow = 0; nCntUiEsaWindow < MAX_SET_UIESAWINDOW; nCntUiEsaWindow++)
	{// 用意した分繰り返す

		if (g_aUiEsaWindowGloup[nCntUiEsaWindow].bUse == false || g_aUiEsaWindowGloup[nCntUiEsaWindow].bDisp == false)
		{// 使用していない、表示していない状態の場合

			continue;
		}

		if (g_aUiEsaWindowGloup[nCntUiEsaWindow].nLabel >= pPlayer->nMaxFood)
		{
			continue;
		}

		for (int nCntIdx = 0; nCntIdx < MAX_IDX_UIESAWINDOW; nCntIdx++)
		{
			int nIdx = g_aUiEsaWindowGloup[nCntUiEsaWindow].aIdxPolygon[nCntIdx];

			if (nIdx == -1)
			{
				continue;
			}

			// テクスチャの設定
			if (g_aUiEsaWindowPolygon[nIdx].type == UIESAWINDOWTYPE_ESATEX)
			{
				if (g_aUiEsaWindowPolygon[nIdx].nIdxTexture != -1)
				{// テクスチャが設定されている場合

					pDevice->SetTexture(0, pEsaData[g_aUiEsaWindowPolygon[nIdx].nIdxTexture].pTexture);// エサのテクスチャを設定
				}
				else
				{// テクスチャが設定されていない場合

					continue;	// 描画しない
				}
			}
			else
			{
				if (g_aUiEsaWindowPolygon[nIdx].nIdxTexture != -1)
				{// テクスチャが設定されている場合

					pDevice->SetTexture(0, g_apTextureUiEsaWindow[g_aUiEsaWindowPolygon[nIdx].nIdxTexture]);
				}
				else
				{// テクスチャが設定されていない場合

					pDevice->SetTexture(0, NULL);
				}
			}

			if (g_aUiEsaWindowPolygon[nIdx].bAlphaBlend == true)
			{// アルファブレンドする場合

				// 減算合成の設定
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3
			}

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
								   nIdx * 4,				// インデックス
								   2);						// プリミティブ（ポリゴン）数

			if (g_aUiEsaWindowPolygon[nIdx].bAlphaBlend == true)
			{// アルファブレンドする場合

				// 減算合成を元に戻す 
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
			}
		}
	}
}

//========================================================================
// エサUIのウィンドウポリゴンの頂点設定処理
//========================================================================
void SetVtxUiEsaWindowPolygon(int nIdx,D3DXVECTOR3 pos, float fSizeScalar)
{
	// 変数宣言 ======================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言
	
	D3DXVECTOR3 offPos  = g_aUiEsaWindowPolygon[nIdx].pos;
	float fSizeWidth    = g_aUiEsaWindowPolygon[nIdx].fSizeWidth;
	float fSizeHeight   = g_aUiEsaWindowPolygon[nIdx].fSizeHeight;
	D3DXCOLOR col	    = g_aUiEsaWindowPolygon[nIdx].col;
	D3DXVECTOR2 texPos  = g_aUiEsaWindowPolygon[nIdx].texPos;
	D3DXVECTOR2 texSize = g_aUiEsaWindowPolygon[nIdx].texSize;

	// ===============================================

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffUiEsaWindow->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += (nIdx * 4);	// 開始位置まで進める

	// 位置の設定
	switch (g_aUiEsaWindowPolygon[nIdx].posType)
	{
	case UIESAWINDOWTYPEPOS_CENTER:		// 中心

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);

		break;

	case UIESAWINDOWTYPEPOS_TOP:		// 上
		
		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y			    ) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y			    ) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);

		break;

	case UIESAWINDOWTYPEPOS_BOT:		// 下

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);

		break;

	case UIESAWINDOWTYPEPOS_LEFT:		// 左

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);

		break;

	case UIESAWINDOWTYPEPOS_RIGHT:		// 右

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);

		break;
		
	case UIESAWINDOWTYPEPOS_TOPLEFT:	// 左上

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);

		break;

	case UIESAWINDOWTYPEPOS_TOPRIGHT:	// 右上

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y + fSizeHeight) * fSizeScalar, 0.0f);

		break;

	case UIESAWINDOWTYPEPOS_BOTLEFT:	// 左下

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x + fSizeWidth) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);

		break;

	case UIESAWINDOWTYPEPOS_BOTRIGHT:	// 左右

		pVtx[0].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y - fSizeHeight) * fSizeScalar, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x + (offPos.x - fSizeWidth) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + (offPos.x				) * fSizeScalar, pos.y + (offPos.y				) * fSizeScalar, 0.0f);

		break;
	}

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(texPos.x			  ,	texPos.y			);
	pVtx[1].tex = D3DXVECTOR2(texPos.x + texSize.x, texPos.y			);
	pVtx[2].tex = D3DXVECTOR2(texPos.x			  , texPos.y + texSize.y);
	pVtx[3].tex = D3DXVECTOR2(texPos.x + texSize.x, texPos.y + texSize.y);

	// ▲頂点バッファをアンロックする
	g_pVtxBuffUiEsaWindow->Unlock();
}

//========================================================================
// エサUIのウィンドウの設定処理
//========================================================================
void SetUiEsaWindow(int nIdx, D3DXVECTOR3 pos)
{
	for (int nCntUiEsaWindow = 0; nCntUiEsaWindow < MAX_PLAYER; nCntUiEsaWindow++)
	{
		if (g_aUiEsaWindow[nCntUiEsaWindow].bUse == true)
		{
			continue;
		}


		//g_aUiEsaWindow[nCntUiEsaWindow].pEsaQueue = (pEsaQueue != NULL) ? &pEsaQueue[0] : NULL;
		//g_aUiEsaWindow[nCntUiEsaWindow].nSizeEsaQueue = nSizeEsaQueue;
		g_aUiEsaWindow[nCntUiEsaWindow].nIdxPlayer = nIdx;
		g_aUiEsaWindow[nCntUiEsaWindow].pos = pos;
		g_aUiEsaWindow[nCntUiEsaWindow].bDisp = true;
		g_aUiEsaWindow[nCntUiEsaWindow].bUse = true;

		int nTmpIdx1 = -1;
		int nTmpIdx2 = -1;

		SetUiEsaWindowGroup(nCntUiEsaWindow,UIESAWINDOWTYPE_LAY_WINDOW, D3DXVECTOR3(pos.x, pos.y, 0.0f), 1.0f);

		for (int nCnt = 0; nCnt < 3; nCnt++)
		{
			nTmpIdx1 = SetUiEsaWindowGroup(nCntUiEsaWindow, UIESAWINDOWTYPE_LAY_LINE  , D3DXVECTOR3(pos.x, pos.y + 40 * nCnt, 0.0f), 1.0f);
			nTmpIdx2 = SetUiEsaWindowGroup(nCntUiEsaWindow, UIESAWINDOWTYPE_LAY_ESATEX, D3DXVECTOR3(pos.x, pos.y + 40 * nCnt, 0.0f), 1.0f);
			
			if (nTmpIdx1 != -1)
			{
				g_aUiEsaWindowGloup[nTmpIdx1].nLabel = nCnt;
			}
			
			if (nTmpIdx2 != -1)
			{
				g_aUiEsaWindowGloup[nTmpIdx2].nLabel = nCnt;
			}
		}

		break;
	}
}

//========================================================================
// エサUIのウィンドウのグループの設定処理
//========================================================================
int SetUiEsaWindowGroup(int nIdxParent, UIESAWINDOWTYPE layType, D3DXVECTOR3 pos, float fSizeScalar)
{

	int nCntEsa = 0;

	for (int nCntUiEsaWindow = 0; nCntUiEsaWindow < MAX_SET_UIESAWINDOW; nCntUiEsaWindow++)
	{
		if (g_aUiEsaWindowGloup[nCntUiEsaWindow].bUse == true)
		{// 使用している場合

			continue;	// 処理の始めに戻る
		}

		g_aUiEsaWindowGloup[nCntUiEsaWindow].nIdxParent = nIdxParent;
		g_aUiEsaWindowGloup[nCntUiEsaWindow].pos = pos;
		g_aUiEsaWindowGloup[nCntUiEsaWindow].fSizeScalar = fSizeScalar;
		g_aUiEsaWindowGloup[nCntUiEsaWindow].bDisp = true;
		g_aUiEsaWindowGloup[nCntUiEsaWindow].bUse = true;

		int nCntSet = 0;	// 設定数

		for (int nCntInfo = 0; nCntInfo < sizeof g_aUiEsaWindowInfo / sizeof(g_aUiEsaWindowInfo[0]); nCntInfo++)
		{
			if (g_aUiEsaWindowInfo[nCntInfo].nLayoutType == layType)
			{// 設定するレイアウトの種類と同じ

				// ポリゴンの設定
				g_aUiEsaWindowGloup[nCntUiEsaWindow].aIdxPolygon[nCntSet] = SetUiEsaWindowPolygon(g_aUiEsaWindowInfo[nCntInfo].posType,g_aUiEsaWindowInfo[nCntInfo].type,
																							 g_aUiEsaWindowInfo[nCntInfo].nIdxTexture, g_aUiEsaWindowInfo[nCntInfo].bAlphaBlend,
																							 g_aUiEsaWindowInfo[nCntInfo].pos,
																							 g_aUiEsaWindowInfo[nCntInfo].fSizeWidth, g_aUiEsaWindowInfo[nCntInfo].fSizeHeight,
																							 g_aUiEsaWindowInfo[nCntInfo].texPos, g_aUiEsaWindowInfo[nCntInfo].texSize, 
																							 g_aUiEsaWindowInfo[nCntInfo].col);

				switch (g_aUiEsaWindowInfo[nCntInfo].nLayoutType)
				{
				case UIESAWINDOWTYPE_LAY_ESATEX:

					g_aUiEsaWindowPolygon[g_aUiEsaWindowGloup[nCntUiEsaWindow].aIdxPolygon[nCntSet]].nLabel = nCntEsa;

					nCntEsa++;

					break;
				}

				nCntSet++;
			}
		}

		return nCntUiEsaWindow;
	}

	return -1;
}

//========================================================================
// エサUIのウィンドウポリゴンの設定処理
//========================================================================
int SetUiEsaWindowPolygon(UIESAWINDOWTYPEPOS posType, UIESAWINDOWTYPE type, int nIdxTexture, bool bAlphaBlend,
	D3DXVECTOR3 pos, float fSizeWidth, float fSizeHeight,
	D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	for (int nCntPolygon = 0; nCntPolygon < MAX_NUM_UIESAWINDOW; nCntPolygon++)
	{
		if (g_aUiEsaWindowPolygon[nCntPolygon].bUse == true)
		{// 使用している場合

			continue;	// 処理の始めに戻る
		}

		g_aUiEsaWindowPolygon[nCntPolygon].posType = posType;
		g_aUiEsaWindowPolygon[nCntPolygon].type = type;
		g_aUiEsaWindowPolygon[nCntPolygon].nIdxTexture = nIdxTexture;
		g_aUiEsaWindowPolygon[nCntPolygon].bAlphaBlend = bAlphaBlend;
		g_aUiEsaWindowPolygon[nCntPolygon].pos = pos;
		g_aUiEsaWindowPolygon[nCntPolygon].fSizeWidth = fSizeWidth;
		g_aUiEsaWindowPolygon[nCntPolygon].fSizeHeight = fSizeHeight;
		g_aUiEsaWindowPolygon[nCntPolygon].col = col;
		g_aUiEsaWindowPolygon[nCntPolygon].texPos = texPos;
		g_aUiEsaWindowPolygon[nCntPolygon].texSize = texSize;
		g_aUiEsaWindowPolygon[nCntPolygon].bDisp = true;
		g_aUiEsaWindowPolygon[nCntPolygon].bUse = true;

		return nCntPolygon;
	}

	return -1;
}
