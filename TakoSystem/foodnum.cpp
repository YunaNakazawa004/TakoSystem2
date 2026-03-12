//=============================================================================
// 
// エサ上限 [foodnum.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "foodnum.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_FOODNUM				(10)									// 最大
#define MAX_HEIGHT				(620.0f)								// 高さ
#define FADE_MINUS				(0.03f)									// 消え方
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量

//*****************************************************************************
// エサ上限構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXCOLOR col;				// 色
	FOODNUMTYPE type;			// 種類
	FOODNUMSTATE state;			// 状態
	int nCounterState;			// 状態カウンター
	float fWidth;				// 幅
	float fHeight;				// 高さ
	bool bUse;					// 使用しているか
}FoodNum;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureFoodNum[FOODNUMTYPE_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFoodNum = NULL;			// 頂点バッファへのポインタ
FoodNum g_aFoodNum[MAX_FOODNUM];				// エサ上限の情報

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaFoodNum[FOODNUMTYPE_MAX] =
{
	"data\\TEXTURE\\foodNum000.png",
	"data\\TEXTURE\\foodNum001.png",
};

//=============================================================================
// エサ上限の初期化処理
//=============================================================================
void InitFoodNum(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < FOODNUMTYPE_MAX; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaFoodNum[nCntTexture], &g_apTextureFoodNum[nCntTexture]);
	}

	// エサ上限情報の初期化
	for (int nCntNum = 0; nCntNum < MAX_FOODNUM; nCntNum++)
	{
		g_aFoodNum[nCntNum].pos = FIRST_POS;
		g_aFoodNum[nCntNum].col = WHITE_VTX;
		g_aFoodNum[nCntNum].type = FOODNUMTYPE_UP;
		g_aFoodNum[nCntNum].state = FOODNUMSTATE_NONE;
		g_aFoodNum[nCntNum].nCounterState = 0;
		g_aFoodNum[nCntNum].fWidth = 200.0f;
		g_aFoodNum[nCntNum].fHeight = 100.0f;
		g_aFoodNum[nCntNum].bUse = false;
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VERTEX * MAX_FOODNUM,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffFoodNum,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffFoodNum->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntNum = 0; nCntNum < MAX_FOODNUM; nCntNum++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x - 200.0f, g_aFoodNum[nCntNum].pos.y - 100.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x + 200.0f, g_aFoodNum[nCntNum].pos.y - 100.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x - 200.0f, g_aFoodNum[nCntNum].pos.y + 100.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x + 200.0f, g_aFoodNum[nCntNum].pos.y + 100.0f, 0.0f);

		// rhwの設定					 
		pVtx[0].rhw = DEFAULT_RHW;
		pVtx[1].rhw = DEFAULT_RHW;
		pVtx[2].rhw = DEFAULT_RHW;
		pVtx[3].rhw = DEFAULT_RHW;

		// 頂点カラーの設定
		pVtx[0].col = g_aFoodNum[nCntNum].col;
		pVtx[1].col = g_aFoodNum[nCntNum].col;
		pVtx[2].col = g_aFoodNum[nCntNum].col;
		pVtx[3].col = g_aFoodNum[nCntNum].col;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffFoodNum->Unlock();
}

//=============================================================================
// エサ上限の終了処理
//=============================================================================
void UninitFoodNum(void)
{
	// テクスチャの破棄
	for (int nCntFoodNum = 0; nCntFoodNum < FOODNUMTYPE_MAX; nCntFoodNum++)
	{
		if (g_apTextureFoodNum[nCntFoodNum] != NULL)
		{
			g_apTextureFoodNum[nCntFoodNum]->Release();
			g_apTextureFoodNum[nCntFoodNum] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffFoodNum != NULL)
	{
		g_pVtxBuffFoodNum->Release();
		g_pVtxBuffFoodNum = NULL;
	}
}

//=============================================================================
// エサ上限の更新処理
//=============================================================================
void UpdateFoodNum(void)
{
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffFoodNum->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntNum = 0; nCntNum < MAX_FOODNUM; nCntNum++)
	{
		if (g_aFoodNum[nCntNum].bUse == true)
		{// 使用している
			switch (g_aFoodNum[nCntNum].state)
			{
			case FOODNUMSTATE_NONE:
				g_aFoodNum[nCntNum].bUse = false;

				break;

			case FOODNUMSTATE_DISP:
				break;

			case FOODNUMSTATE_MOVE:
				g_aFoodNum[nCntNum].pos.y += -MOVEMENT.y;

				if (g_aFoodNum[nCntNum].pos.y <= MAX_HEIGHT)
				{// 既定の高さ
					g_aFoodNum[nCntNum].pos.y = MAX_HEIGHT;

					g_aFoodNum[nCntNum].nCounterState--;

					if (g_aFoodNum[nCntNum].nCounterState < 0)
					{// カウントがなくなる
						g_aFoodNum[nCntNum].nCounterState = 0;
						g_aFoodNum[nCntNum].state = FOODNUMSTATE_FADE;
					}
				}

				break;

			case FOODNUMSTATE_FADE:
				g_aFoodNum[nCntNum].col.a -= FADE_MINUS;

				if (g_aFoodNum[nCntNum].col.a <= 0.0f)
				{// 透明になった
					g_aFoodNum[nCntNum].col.a = 0.0f;
					g_aFoodNum[nCntNum].state = FOODNUMSTATE_NONE;
				}

				break;
			}

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x - g_aFoodNum[nCntNum].fWidth, g_aFoodNum[nCntNum].pos.y - g_aFoodNum[nCntNum].fHeight, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x + g_aFoodNum[nCntNum].fWidth, g_aFoodNum[nCntNum].pos.y - g_aFoodNum[nCntNum].fHeight, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x - g_aFoodNum[nCntNum].fWidth, g_aFoodNum[nCntNum].pos.y + g_aFoodNum[nCntNum].fHeight, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x + g_aFoodNum[nCntNum].fWidth, g_aFoodNum[nCntNum].pos.y + g_aFoodNum[nCntNum].fHeight, 0.0f);

			// 頂点カラーの設定
			pVtx[0].col = g_aFoodNum[nCntNum].col;
			pVtx[1].col = g_aFoodNum[nCntNum].col;
			pVtx[2].col = g_aFoodNum[nCntNum].col;
			pVtx[3].col = g_aFoodNum[nCntNum].col;
		}

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffFoodNum->Unlock();
}

//=============================================================================
// エサ上限の描画処理
//=============================================================================
void DrawFoodNum(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	for (int nCntNum = 0; nCntNum < MAX_FOODNUM; nCntNum++)
	{
		if (g_aFoodNum[nCntNum].bUse == false)
		{// 使用してないなら無視
			continue;
		}

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffFoodNum, 0, sizeof(VERTEX_2D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_2D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureFoodNum[g_aFoodNum[nCntNum].type]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCntNum * MAX_VERTEX,		// 描画する最初の頂点インデックス
			MAX_POLYGON);		// 描画するプリミティブ数
	}
}

//=============================================================================
// エサ上限の設定
//=============================================================================
int SetFoodNum(FOODNUMTYPE type, FOODNUMSTATE state, D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	int nCntNum = -1;
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffFoodNum->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntNum = 0; nCntNum < MAX_FOODNUM; nCntNum++)
	{
		if (g_aFoodNum[nCntNum].bUse == false)
		{// 使用してない
			g_aFoodNum[nCntNum].pos = pos;
			g_aFoodNum[nCntNum].col = WHITE_VTX;
			g_aFoodNum[nCntNum].type = type;
			g_aFoodNum[nCntNum].state = state;

			g_aFoodNum[nCntNum].nCounterState = (state == FOODNUMSTATE_MOVE) ? 60 : 0;
			g_aFoodNum[nCntNum].fWidth = fWidth;
			g_aFoodNum[nCntNum].fHeight = fHeight;
			g_aFoodNum[nCntNum].bUse = true;

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x - fWidth, g_aFoodNum[nCntNum].pos.y - fHeight, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x + fWidth, g_aFoodNum[nCntNum].pos.y - fHeight, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x - fWidth, g_aFoodNum[nCntNum].pos.y + fHeight, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aFoodNum[nCntNum].pos.x + fWidth, g_aFoodNum[nCntNum].pos.y + fHeight, 0.0f);

			break;
		}

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffFoodNum->Unlock();

	return nCntNum;
}

//=============================================================================
// エサ上限の状態設定
//=============================================================================
void SetFoodNumState(int nIdx, FOODNUMSTATE state)
{
	if (nIdx < 0 || nIdx > MAX_FOODNUM)
	{// 範囲外
		return;
	}

	if (g_aFoodNum[nIdx].state == state)
	{// 同じ状態は入れない
		return;
	}

	g_aFoodNum[nIdx].state = state;
}