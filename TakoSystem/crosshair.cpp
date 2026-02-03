//=============================================================================
// 
// クロスヘア [crosshair.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "crosshair.h"
#include "camera.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CROSSHAIR_WIDTH		(30)									// 幅の半分
#define CROSSHAIR_HEIGHT	(30)									// 高さの半分
#define LEFT_SENTER			(D3DXVECTOR3(320.0f, 360.0f, 0.0f))		// 左半分の中心
#define RIGHT_SENTER		(D3DXVECTOR3(960.0f, 360.0f, 0.0f))		// 右半分の中心

//*****************************************************************************
// クロスヘア構造体を定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	CROSSHAIRSTATE state;			// 状態
	int nCounterState;				// 状態管理カウンター
	bool bDisp;						// 表示するかどうか
}CrossHair;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureCrossHair[CROSSHAIRSTATE_MAX] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffCrossHair = NULL;						// 頂点バッファへのポインタ
CrossHair g_aCrossHair[MAX_PLAYER];										// クロスヘアの情報

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaCrossHair[CROSSHAIRSTATE_MAX] =
{
	"data\\TEXTURE\\crosshair.png",
	"data\\TEXTURE\\crosshairhit.png",
};

//=============================================================================
// クロスヘアの初期化処理
//=============================================================================
void InitCrossHair(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	for (int nCntCrossHair = 0; nCntCrossHair < CROSSHAIRSTATE_MAX; nCntCrossHair++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaCrossHair[nCntCrossHair], &g_apTextureCrossHair[nCntCrossHair]);
	}

	// クロスヘア情報の初期化
	for (int nCntCrossHair = 0; nCntCrossHair < MAX_PLAYER; nCntCrossHair++)
	{
		g_aCrossHair[nCntCrossHair].state = CROSSHAIRSTATE_NONE;
		g_aCrossHair[nCntCrossHair].nCounterState = 0;
		g_aCrossHair[nCntCrossHair].bDisp = true;
	}

	if (GetNumCamera() == 1)
	{// 1人プレイ
		g_aCrossHair[0].pos = CENTER;
		g_aCrossHair[1].bDisp = false;
	}
	else if (GetNumCamera() == 2)
	{// 2人プレイ
		g_aCrossHair[0].pos = LEFT_SENTER;
		g_aCrossHair[1].pos = RIGHT_SENTER;
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VERTEX * MAX_PLAYER,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffCrossHair,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffCrossHair->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntCrossHair = 0; nCntCrossHair < MAX_PLAYER; nCntCrossHair++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(g_aCrossHair[nCntCrossHair].pos.x - CROSSHAIR_WIDTH, g_aCrossHair[nCntCrossHair].pos.y - CROSSHAIR_HEIGHT, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aCrossHair[nCntCrossHair].pos.x + CROSSHAIR_WIDTH, g_aCrossHair[nCntCrossHair].pos.y - CROSSHAIR_HEIGHT, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_aCrossHair[nCntCrossHair].pos.x - CROSSHAIR_WIDTH, g_aCrossHair[nCntCrossHair].pos.y + CROSSHAIR_HEIGHT, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aCrossHair[nCntCrossHair].pos.x + CROSSHAIR_WIDTH, g_aCrossHair[nCntCrossHair].pos.y + CROSSHAIR_HEIGHT, 0.0f);

		// rhwの設定
		pVtx[0].rhw = DEFAULT_RHW;
		pVtx[1].rhw = DEFAULT_RHW;
		pVtx[2].rhw = DEFAULT_RHW;
		pVtx[3].rhw = DEFAULT_RHW;

		// 頂点カラーの設定
		pVtx[0].col = WHITE_VTX;
		pVtx[1].col = WHITE_VTX;
		pVtx[2].col = WHITE_VTX;
		pVtx[3].col = WHITE_VTX;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffCrossHair->Unlock();
}

//=============================================================================
// クロスヘアの終了処理
//=============================================================================
void UninitCrossHair(void)
{
	// テクスチャの破棄
	for (int nCntCrossHair = 0; nCntCrossHair < CROSSHAIRSTATE_MAX; nCntCrossHair++)
	{
		if (g_apTextureCrossHair[nCntCrossHair] != NULL)
		{
			g_apTextureCrossHair[nCntCrossHair]->Release();
			g_apTextureCrossHair[nCntCrossHair] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffCrossHair != NULL)
	{
		g_pVtxBuffCrossHair->Release();
		g_pVtxBuffCrossHair = NULL;
	}
}

//=============================================================================
// クロスヘアの更新処理
//=============================================================================
void UpdateCrossHair(void)
{
}

//=============================================================================
// クロスヘアの描画処理
//=============================================================================
void DrawCrossHair(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	for (int nCntCrossHair = 0; nCntCrossHair < GetNumCamera(); nCntCrossHair++)
	{
		if (g_aCrossHair[nCntCrossHair].bDisp == true)
		{
			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffCrossHair, 0, sizeof(VERTEX_2D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_2D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureCrossHair[g_aCrossHair[nCntCrossHair].state]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCntCrossHair * MAX_VERTEX,		// 描画する最初の頂点インデックス
				MAX_POLYGON);		// 描画するプリミティブ数
		}
	}
}

//=============================================================================
// クロスヘアの設定処理
//=============================================================================
void SetCrossHair(int nIdx, CROSSHAIRSTATE state)
{
	if (g_aCrossHair[nIdx].state != state)
	{
		g_aCrossHair[nIdx].state = state;
		g_aCrossHair[nIdx].nCounterState = 0;
		g_aCrossHair[nIdx].bDisp = true;
	}
}