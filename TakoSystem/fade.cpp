//===========================================================================
//フェード処理[fade.cpp]
//Author:須藤英翔
//
//===========================================================================
#include "fade.h"
#include "input.h"

//グローバル変数
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFade = NULL;	//頂点バッファへのポインタ
FADE g_fade;									//フェードの状態
MODE g_modeNext;								//次の画面(モード)
D3DXCOLOR g_colorFade;							//ポリゴン(フェード)の色

//===========================================================================
// フェードの初期化処理
//===========================================================================
void InitFade(MODE modenext)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 初期化
	g_fade = FADE_IN;				// フェードイン状態に
	g_modeNext = modenext;			// 次の画面(モード)を設定
	g_colorFade = BLK_VISIBLE_VTX;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffFade,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH, 0.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(0.0f, SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0;
	pVtx[1].rhw = 1.0;
	pVtx[2].rhw = 1.0;
	pVtx[3].rhw = 1.0;

	// 頂点カラーの設定
	pVtx[0].col = g_colorFade;
	pVtx[1].col = g_colorFade;
	pVtx[2].col = g_colorFade;
	pVtx[3].col = g_colorFade;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffFade->Unlock();

	// モードの設定
	SetMode(g_modeNext);
}

//=============================================================================
// フェードの終了処理
//=============================================================================
void UninitFade(void)
{
	// 頂点バッファの破棄
	if (g_pVtxBuffFade != NULL)
	{
		g_pVtxBuffFade->Release();
		g_pVtxBuffFade = NULL;
	}
}

//=============================================================================
// フェードの更新処理
//=============================================================================
void UpdateFade(void)
{
	if (g_fade != FADE_NONE)
	{
		if (g_fade == FADE_IN)
		{// フェードイン状態
			g_colorFade.a -= FADE_SPEED;			// ポリゴンを透明にしていく

			if (g_colorFade.a <= 0.0f)
			{// 透明になった
				g_colorFade.a = 0.0f;
				g_fade = FADE_NONE;			// 何もしていない状態にする
			}
		}

		else if (g_fade == FADE_OUT)
		{// フェードアウト状態
			g_colorFade.a += FADE_SPEED;			// ポリゴンを不透明にしていく

			if (g_colorFade.a >= 1.0f)
			{// 不透明になった
				g_colorFade.a = 1.0f;
				g_fade = FADE_IN;			// フェードイン状態にする

				// モードの設定
				SetMode(g_modeNext);
			}
		}
	}
	else
	{
		g_colorFade.a = 0.0f;
	}

	// ローカル変数宣言
	VERTEX_2D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffFade->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点カラーの設定
	pVtx[0].col = g_colorFade;
	pVtx[1].col = g_colorFade;
	pVtx[2].col = g_colorFade;
	pVtx[3].col = g_colorFade;

	// 頂点バッファをアンロックする
	g_pVtxBuffFade->Unlock();
}

//=============================================================================
// フェードの描画処理
//=============================================================================
void DrawFade(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffFade, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, NULL);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,					// 描画する最初の頂点インデックス
		2);		// 描画するプリミティブ数
}

//=============================================================================
// フェードの設定処理
//=============================================================================
void SetFade(MODE modenext)
{
	if (g_fade == FADE_NONE)
	{
		g_fade = FADE_OUT;					// フェードアウト状態に
		g_modeNext = modenext;				// 次の画面(モード)を設定
		g_colorFade = BLK_INVISIBLE_VTX;
	}
}

//=============================================================================
// フェードの状態設定処理
//=============================================================================
void SetFadeState(FADE fade)
{
	g_fade = fade;
}

//=============================================================================
// フェードの取得
//=============================================================================
FADE GetFade(void)
{
	return g_fade;
}