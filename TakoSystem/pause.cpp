//===========================================================================
//ポーズ処理[pasue.cpp]
//Author:須藤英翔
//
//===========================================================================
#include "pause.h"
#include "input.h"
#include "fade.h"


//グローバル変数宣言
LPDIRECT3DTEXTURE9 g_apTexturePause[3] = {};
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPause = NULL;
PAUSE_MENU g_pauseMenu;			//ポーズメニュー
int g_nSelect = 0;
//===========================================================================
//ポーズの初期化処理
//===========================================================================
void InitPause(void)
{

	LPDIRECT3DDEVICE9 pDevice;				//デバイスへのポインタ

	//デバイスの取得
	pDevice = GetDevice();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\CONTINUE.png",
		&g_apTexturePause[0]);


	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\RETRY.png",
		&g_apTexturePause[1]);

	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\QUIT.png",
		&g_apTexturePause[2]);

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * PAUSE_MENU_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffPause,
		NULL);



	VERTEX_2D* pVtx;			//頂点情報へのポインタ

	//頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntPause = 0; nCntPause < PAUSE_MENU_MAX; nCntPause++)
	{

		pVtx[0].pos = D3DXVECTOR3(SCREEN_WIDTH / 2.0f - 125.0f, 200.0f + (nCntPause * 120.0f), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(SCREEN_WIDTH / 2.0f + 125.0f, 200.0f + (nCntPause * 120.0f), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(SCREEN_WIDTH / 2.0f - 125.0f, 200.0f + (nCntPause * 120.0f) + 100.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(SCREEN_WIDTH / 2.0f + 125.0f, 200.0f + (nCntPause * 120.0f) + 100.0f, 0.0f);

		//rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		//頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}
	//頂点バッファをアンロックする
	g_pVtxBuffPause->Unlock();
	g_nSelect = 0;
}

//===========================================================================
//ポーズの終了処理
//===========================================================================
void UninitPause(void)
{

	for (int nCntTex = 0; nCntTex < PAUSE_MENU_MAX; nCntTex++)
	{
		if (g_apTexturePause[nCntTex] != NULL)
		{
			// テクスチャの破棄
			g_apTexturePause[nCntTex]->Release();
			g_apTexturePause[nCntTex] = NULL;
		}
	}

	//頂点バッファの破棄
	if (g_pVtxBuffPause != NULL)
	{
		g_pVtxBuffPause->Release();
		g_pVtxBuffPause = NULL;
	}
}
//===========================================================================
//ポーズの更新処理
//===========================================================================
void UpdatePause(void)
{
	if (GetFade() != FADE_NONE)
	{
		return;
	}
	//頂点座標の更新
	VERTEX_2D* pVtx;			//頂点情報へのポインタ

	//頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	if (GetKeyboardTrigger(DIK_W))
	{
		g_nSelect--;
		if (g_nSelect < PAUSE_MENU_CONTINUE)
		{
			g_nSelect = PAUSE_MENU_QUIT;
		}
	}
	else if (GetKeyboardTrigger(DIK_S))
	{
		g_nSelect++;
		if (g_nSelect >= PAUSE_MENU_MAX)
		{
			g_nSelect = PAUSE_MENU_CONTINUE;
		}

	}if (GetKeyboardTrigger(DIK_RETURN))
	{
		switch (g_nSelect)
		{
		case PAUSE_MENU_CONTINUE:

			break;

		case PAUSE_MENU_RETRY:

			SetFade(MODE_GAME);
			break;

		case PAUSE_MENU_QUIT:

			SetFade(MODE_TITLE);
			break;

		}
	}


	for (int nCount = 0; nCount < PAUSE_MENU_MAX; nCount++)
	{
		if (nCount == g_nSelect)
		{
			pVtx[0].col = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f);

		}
		else
		{

			pVtx[0].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
			pVtx[1].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
			pVtx[2].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
			pVtx[3].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		}
		pVtx += 4;
	}

	//頂点バッファをアンロックする
	g_pVtxBuffPause->Unlock();
}

//===========================================================================
//ポーズの描画処理
//===========================================================================
void DrawPause(void)
{
	if (GetFade() != FADE_NONE)
	{
		return;
	}

	LPDIRECT3DDEVICE9 pDevice;				//デバイスへのポインタ

	//デバイスの取得
	pDevice = GetDevice();

	//頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffPause, 0, sizeof(VERTEX_2D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);
	for (int nCntPause = 0; nCntPause < PAUSE_MENU_MAX; nCntPause++)
	{
		//テクスチャの設定
		pDevice->SetTexture(0, g_apTexturePause[nCntPause]);

		//ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntPause * 4, 2);
	}

}