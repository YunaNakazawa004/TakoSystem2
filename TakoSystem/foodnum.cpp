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
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量

//*****************************************************************************
// エサ上限構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	FOODNUMTYPE type;			// 種類
	FOODNUMSTATE state;			// 状態
	int nCounterState;			// 状態カウンター
	bool bUse;					// 使用しているか
}FoodNum;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureFoodNum[FOODNUMTYPE_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFoodNum = NULL;			// 頂点バッファへのポインタ
FoodNum g_aFoodNum[FOODNUMTYPE_MAX];				// エサ上限の情報

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaFoodNum[FOODNUMTYPE_MAX] =
{
	"data\\TEXTURE\\FoodNum000.png",
	"data\\TEXTURE\\FoodNum001.png",
};

//=============================================================================
// エサ上限の初期化処理
//=============================================================================
void InitFoodNum(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < FOODNUMTYPE_MAX; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaFoodNum[nCntTexture], &g_apTextureFoodNum[nCntTexture]);
	}

	// エサ上限情報の初期化
	for (int nCntTxt = 0; nCntTxt < FOODNUMTYPE_MAX; nCntTxt++)
	{
		g_aFoodNum[nCntTxt].pos = FIRST_POS;
		g_aFoodNum[nCntTxt].type = FOODNUMTYPE_UP;
		g_aFoodNum[nCntTxt].state = FOODNUMSTATE_NONE;
		g_aFoodNum[nCntTxt].nCounterState = 0;
		g_aFoodNum[nCntTxt].bUse = false;
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * FOODNUMTYPE_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffFoodNum,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffFoodNum->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTxt = 0; nCntTxt < FOODNUMTYPE_MAX; nCntTxt++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-200.0f, 100.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(200.0f, 100.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-200.0f, -100.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(200.0f, -100.0f, 0.0f);

		// rhwの設定					 
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

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
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffFoodNum->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTxt = 0; nCntTxt < FOODNUMTYPE_MAX; nCntTxt++)
	{
		if (g_aFoodNum[nCntTxt].bUse == true)
		{// 使用している
			switch (g_aFoodNum[nCntTxt].state)
			{
			case FOODNUMSTATE_NONE:

				break;

			case FOODNUMSTATE_DISP:

				break;
			}
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
	D3DXMATRIX mtxTrans;				// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス取得用

	for (int nCntTxt = 0; nCntTxt < FOODNUMTYPE_MAX; nCntTxt++)
	{
		if (g_aFoodNum[nCntTxt].bUse == false)
		{// 使用してないなら無視
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aFoodNum[nCntTxt].mtxWorld);

		// ビューマトリックスを取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		// ポリゴンをカメラに対して正面に向ける
		D3DXMatrixInverse(&g_aFoodNum[nCntTxt].mtxWorld, NULL, &mtxView);	// 逆行列を求める
		g_aFoodNum[nCntTxt].mtxWorld._41 = 0.0f;
		g_aFoodNum[nCntTxt].mtxWorld._42 = 0.0f;
		g_aFoodNum[nCntTxt].mtxWorld._43 = 0.0f;

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_aFoodNum[nCntTxt].pos.x, g_aFoodNum[nCntTxt].pos.y, g_aFoodNum[nCntTxt].pos.z);
		D3DXMatrixMultiply(&g_aFoodNum[nCntTxt].mtxWorld, &g_aFoodNum[nCntTxt].mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aFoodNum[nCntTxt].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffFoodNum, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureFoodNum[g_aFoodNum[nCntTxt].type]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCntTxt * MAX_VERTEX,		// 描画する最初の頂点インデックス
			MAX_POLYGON);		// 描画するプリミティブ数
	}
}

//=============================================================================
// エサ上限の設定
//=============================================================================
void SetFoodNum(FOODNUMTYPE type, FOODNUMSTATE state, D3DXVECTOR3 pos)
{
	for (int nCntTxt = 0; nCntTxt < FOODNUMTYPE_MAX; nCntTxt++)
	{
		if (g_aFoodNum[nCntTxt].bUse == false)
		{// 使用してない
			g_aFoodNum[nCntTxt].pos = pos;
			g_aFoodNum[nCntTxt].type = type;
			g_aFoodNum[nCntTxt].state = state;

			g_aFoodNum[nCntTxt].nCounterState = 0;
			g_aFoodNum[nCntTxt].bUse = true;

			break;
		}
	}
}

//=============================================================================
// エサ上限の状態設定
//=============================================================================
void SetFoodNumState(FOODNUMTYPE type, FOODNUMSTATE state)
{
	if (g_aFoodNum[type].state == state)
	{// 同じ状態は入れない
		return;
	}

	g_aFoodNum[type].state = state;
}