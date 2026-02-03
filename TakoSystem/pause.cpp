//===========================================================================
//ポーズ処理[pasue.cpp]
//Author:須藤英翔
//
//===========================================================================
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "game.h"
#include "sound.h"

//グローバル変数宣言
LPDIRECT3DTEXTURE9 g_apTexturePause[MAX_PAUSE] = {};
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPause = NULL;
bool g_bPauseMenu = true;
PAUSE_MENU g_pauseMenu;			//ポーズメニュー
int g_nSelect = 0;
//===========================================================================
//ポーズの初期化処理
//===========================================================================
void InitPause(void)
{
	VERTEX_2D* pVtx;					//頂点情報へのポインタ
	LPDIRECT3DDEVICE9 pDevice;			//デバイスへのポインタ

	//デバイスの取得
	pDevice = GetDevice();

	//テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\CONTINUE000.png",
		&g_apTexturePause[0]);


	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\RETRY000.png",
		&g_apTexturePause[1]);

	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\QUIT000.png",
		&g_apTexturePause[2]);

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VERTEX * MAX_PAUSE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffPause,
		NULL);

	// ポーズメニューの初期化
	g_pauseMenu = PAUSE_MENU_CONTINUE;
	g_nSelect = 0;

	// フェードの連打阻止の初期化
	g_bPauseMenu = true;

	//頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntPause = 0; nCntPause < MAX_PAUSE; nCntPause++)
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

		pVtx += MAX_VERTEX;
	}
	//頂点バッファをアンロックする
	g_pVtxBuffPause->Unlock();
}

//===========================================================================
//ポーズの終了処理
//===========================================================================
void UninitPause(void)
{

	for (int nCntTex = 0; nCntTex < MAX_PAUSE; nCntTex++)
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

	//頂点座標の更新
	VERTEX_2D* pVtx;			//頂点情報へのポインタ

	//頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffPause->Lock(0, 0, (void**)&pVtx, 0);

	if (GetKeyboardTrigger(DIK_W) ||
		GetJoypadTrigger(0, JOYKEY_UP) == true)
	{
		g_nSelect--;
		PlaySound(SOUND_SE_CURSORMOVE);
		if (g_nSelect < PAUSE_MENU_CONTINUE)
		{
			g_nSelect = PAUSE_MENU_QUIT;

		}
	}
	else if (GetKeyboardTrigger(DIK_S) || 
		GetJoypadTrigger(0, JOYKEY_DOWN) == true)
	{
		g_nSelect++;
		PlaySound(SOUND_SE_CURSORMOVE);
		if (g_nSelect >= PAUSE_MENU_MAX)
		{
			g_nSelect = PAUSE_MENU_CONTINUE;
		}

	}
	if (GetKeyboardTrigger(DIK_RETURN)||
		GetJoypadTrigger(0, JOYKEY_A) == true)
	{
		switch (g_nSelect)
		{
		case PAUSE_MENU_CONTINUE:
			SetEnablePause(false);
			g_pauseMenu = PAUSE_MENU_CONTINUE;
			PlaySound(SOUND_SE_DECISION);
			break;

		case PAUSE_MENU_RETRY:
			g_pauseMenu = PAUSE_MENU_RETRY;
			SetFade(MODE_GAME);
			PlaySound(SOUND_SE_DECISION);
			break;

		case PAUSE_MENU_QUIT:
			g_pauseMenu = PAUSE_MENU_QUIT;
			SetFade(MODE_TITLE);
			PlaySound(SOUND_SE_DECISION);
			break;

		}
	}


	for (int nCntPause = 0; nCntPause < MAX_PAUSE; nCntPause++)
	{
		if (g_nSelect == nCntPause)
		{// 選択されている場合
			// 頂点カラーの設定
			pVtx[0].col = WHITE_VTX;
			pVtx[1].col = WHITE_VTX;
			pVtx[2].col = WHITE_VTX;
			pVtx[3].col = WHITE_VTX;
		}
		else
		{// 選択されていない場合
			// 頂点カラーの設定
			pVtx[0].col = GRAY_VTX;
			pVtx[1].col = GRAY_VTX;
			pVtx[2].col = GRAY_VTX;
			pVtx[3].col = GRAY_VTX;
		}

		pVtx += MAX_VERTEX;
	}

	//頂点バッファをアンロックする
	g_pVtxBuffPause->Unlock();
}

//===========================================================================
//ポーズの描画処理
//===========================================================================
void DrawPause(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffPause, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntPause = 0; nCntPause < MAX_PAUSE; nCntPause++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_apTexturePause[nCntPause]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCntPause * MAX_VERTEX,		// 描画する最初の頂点インデックス
			MAX_POLYGON);				// 描画するプリミティブ数
	}
}
//=============================================================================
// ポーズメニューの設定
//=============================================================================
void SetPause(PAUSE_MENU pause)
{
	g_nSelect = pause;
}

//=============================================================================
// ポーズメニューの取得
//=============================================================================
PAUSE_MENU* GetPauseMenu(void)
{
	return &g_pauseMenu;
}