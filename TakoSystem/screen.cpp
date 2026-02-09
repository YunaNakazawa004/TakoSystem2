//=============================================================================
// 
// スクリーン [screen.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "screen.h"
#include "game.h"
#include "time.h"

// グローバル変数
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffScreen = NULL;	// 頂点バッファへのポインタ

D3DXCOLOR g_colorScreen;	// ポリゴン（画面）の色
float g_ScreenStock;	// ピンチの色を保存

bool g_bPinchScreen;

// 画面の初期化処理
void InitScreen(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	g_colorScreen = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 黒いポリゴン（不透明）にしておく
	g_ScreenStock = 0.0f;
	g_bPinchScreen = true;

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffScreen,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffScreen->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
	pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
	pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;	// 値は1.0fで固定
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = g_colorScreen;
	pVtx[1].col = g_colorScreen;
	pVtx[2].col = g_colorScreen;
	pVtx[3].col = g_colorScreen;

	// UV座標設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffScreen->Unlock();
}

// 画面の終了処理
void UninitScreen(void)
{
	// 頂点バッファの破棄
	if (g_pVtxBuffScreen != NULL)
	{
		g_pVtxBuffScreen->Release();
		g_pVtxBuffScreen = NULL;
	}
}

// 画面の更新処理
void UpdateScreen(void)
{
	// ポーズ情報の取得
	bool bPause = GetPause();

	// タイム情報の取得
	int pTime = GetTime();

	if (bPause == true)
	{
		g_colorScreen = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.33f);
	}
	else
	{
		if (pTime <= (PINCH_TIME / 2))
		{// 制限時間がピンチ

			// 赤ランプ調整
			if (g_bPinchScreen == true) g_ScreenStock += 0.0025f;
			else g_ScreenStock -= 0.0025f;

			// 赤ランプ切り替え
			if (g_ScreenStock >= 0.33f) g_bPinchScreen = false;
			else if (g_ScreenStock <= 0.165f) g_bPinchScreen = true;

			g_colorScreen = D3DXCOLOR(1.0f, 0.0f, 0.0f, g_ScreenStock);
		}
		else if (pTime <= PINCH_TIME)
		{// 制限時間がピンチ

			// 赤ランプ調整
			if (g_bPinchScreen == true) g_ScreenStock += 0.0025f;
			else g_ScreenStock -= 0.0025f;

			// 赤ランプ切り替え
			if (g_ScreenStock >= 0.165f) g_bPinchScreen = false;
			else if (g_ScreenStock <= 0.0f) g_bPinchScreen = true;

			g_colorScreen = D3DXCOLOR(1.0f, 0.0f, 0.0f, g_ScreenStock);
		}
		else
		{// 通常の状態
			g_colorScreen = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
		}
	}

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffScreen->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = g_colorScreen;
	pVtx[1].col = g_colorScreen;
	pVtx[2].col = g_colorScreen;
	pVtx[3].col = g_colorScreen;

	// 頂点バッファをアンロックする
	g_pVtxBuffScreen->Unlock();

}

// 画面の描画処理
void DrawScreen(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffScreen, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);	// テクスチャを使用しない時は必ずNULLを指定する

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//// 画面の設定
//void SetScreen(MODE modeNext, float nTime, D3DXCOLOR col)
//{
//	g_fade = FADE_OUT;
//	g_modeNext = modeNext;	// 移行するモード
//	g_fadeTime = nTime;		// 画面にかかる時間
//	g_colorScreen = col;		// 画面の色
//}
//
//// 画面の取得
//FADE GetScreen(void)
//{
//	return g_fade;
//}