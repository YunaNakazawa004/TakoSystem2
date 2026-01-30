//=============================================================================
// 
// マップ [title.cpp]
// Author : Mutsuki Uemura
// 
//=============================================================================
#include "main.h"
#include "fade.h"
#include "camera.h"
#include "game.h"

// マクロ定義
#define	MAX_MAP		(5)			// マップで表示するテクスチャの最大数
#define MAP_SIZE	(100)		// マップサイズ

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureMap[MAX_MAP] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffMap = NULL;	// 頂点バッファへのポインタ

// マップの初期化処理
void InitMap(void)
{
	float posX = 0.0f, posY = 0.0f;
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/maptest000.png",
		&g_pTextureMap[0]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_MAP,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffMap,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffMap->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntMap = 0; nCntMap < MAX_MAP; nCntMap++)
	{
		pVtx[0].pos = D3DXVECTOR3(posX / 2 - MAP_SIZE, posY / 2 - MAP_SIZE, 0.0f);	// 右回りで設定する
		pVtx[1].pos = D3DXVECTOR3(posX / 2 + MAP_SIZE, posY / 2 - MAP_SIZE, 0.0f);	// 2Dの場合Zの値は0にする
		pVtx[2].pos = D3DXVECTOR3(posX / 2 - MAP_SIZE, posY / 2 + MAP_SIZE, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(posX / 2 + MAP_SIZE, posY / 2 + MAP_SIZE, 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;	// 値は1.0fで固定
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffMap->Unlock();
}

// マップの終了処理
void UninitMap(void)
{
	// テクスチャの破棄
	for (int nCntMap = 0; nCntMap < MAX_MAP; nCntMap++)
	{// マップの数だけ確認する
		if (g_pTextureMap[nCntMap] != NULL)
		{// テクスチャの破棄
			g_pTextureMap[nCntMap]->Release();
			g_pTextureMap[nCntMap] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffMap != NULL)
	{
		g_pVtxBuffMap->Release();
		g_pVtxBuffMap = NULL;
	}
}

// マップの更新処理
void UpdateMap(void)
{
}

// マップの描画処理
void DrawMap(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffMap, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntMap = 0; nCntMap < MAX_MAP; nCntMap++)
	{// 敵の最大数まで繰り返す
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureMap[nCntMap]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntMap * 4, 2);
	}
}