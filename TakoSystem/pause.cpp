//===========================================================================
// 
//ポーズ処理[pasue.cpp]
//Author:須藤英翔
//
//===========================================================================
#include "pause.h"
#include "input.h"
#include "fade.h"
#include "game.h"

// マクロ定義
#define MAX_PAUSE				(9)										// ポーズメニューの最大オブジェクト数
#define MAX_PAUSE_OBJ			(3)										// ポーズメニューの最大数
#define REPEAT_MAX				(40)									// リピートまでの最大フレーム数
#define REPEAT_COUNT			(10)									// リピート間隔
#define GRAY_VTX				(D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f))		// 頂点カラーが灰

// 構造体の定義

//ポーズ情報構造体
typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 size;
	D3DXVECTOR3 sizeBig;
	char FileName[100];
	bool bSelect;
	bool bDisp;

}Pause;

//グローバル変数宣言
LPDIRECT3DTEXTURE9 g_apTexturePause[MAX_PAUSE] = {};
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffPause = NULL;
bool g_bPauseMenu = true;
PAUSE_MENU g_pauseMenu;			//ポーズメニュー
int g_nSelect = 0;

Pause g_aPause[] =
{ // POS,SIZE,ファイル名

	{{ 320.0f, 200.0f, 0.0f },
	{ 400.0f, 200.0f, 0.0f },
	{ 400.0f, 200.0f, 0.0f },
	"data\\TEXTURE\\pause_ink.png",
	false,
	true},

	{ { 1000.0f, 360.0f, 0.0f },
	{ 470.0f, 230.0f, 0.0f },
	{ 470.0f, 230.0f, 0.0f },
	"data\\TEXTURE\\pause_ink2.png",
	false,
	true},

	{ { 370.0f, 560.0f, 0.0f },
	{ 400.0f, 190.0f, 0.0f },
	{ 400.0f, 190.0f, 0.0f },
	"data\\TEXTURE\\pause_ink3.png",
	false,
	true},

	{{ 320.0f, 200.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	"data\\TEXTURE\\Continue_ink000.png",
	false,
	false},

	{{ 1000.0f, 360.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	"data\\TEXTURE\\Retry_ink000.png",
	false,
	false},

	{{ 370.0f, 560.0f, 0.0f},
	{ 345.0f, 184.0f, 0.0f },
	{ 345.0f, 184.0f, 0.0f },
	"data\\TEXTURE\\Quit_ink000.png",
	false,
	false},

	{{ 320.0f, 200.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	"data\\TEXTURE\\Continue001.png",
	false,
	false},

	{{ 1000.0f, 360.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	{ 320.0f, 170.0f, 0.0f },
	"data\\TEXTURE\\Retry001.png",
	false,
	false},

	{{ 370.0f, 560.0f, 0.0f},
	{ 345.0f, 184.0f, 0.0f },
	{ 345.0f, 184.0f, 0.0f },
	"data\\TEXTURE\\Quit001.png",
	false,
	false},
};

//===========================================================================
//ポーズの初期化処理
//===========================================================================
void InitPause(void)
{
	VERTEX_2D* pVtx;					//頂点情報へのポインタ
	LPDIRECT3DDEVICE9 pDevice;			//デバイスへのポインタ
	D3DXVECTOR3 Pos[MAX_PAUSE] = {};

	//デバイスの取得
	pDevice = GetDevice();

	for (int nCntPause = 0; nCntPause < MAX_PAUSE; nCntPause++)
	{
		//テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
			&g_aPause[nCntPause].FileName[0],
			&g_apTexturePause[nCntPause]);

	}

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
		pVtx[0].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x - g_aPause[nCntPause].size.x, g_aPause[nCntPause].pos.y - g_aPause[nCntPause].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x + g_aPause[nCntPause].size.x, g_aPause[nCntPause].pos.y - g_aPause[nCntPause].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x - g_aPause[nCntPause].size.x, g_aPause[nCntPause].pos.y + g_aPause[nCntPause].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x + g_aPause[nCntPause].size.x, g_aPause[nCntPause].pos.y + g_aPause[nCntPause].size.y, 0.0f);

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
		if (g_nSelect < PAUSE_MENU_CONTINUE)
		{
			g_nSelect = PAUSE_MENU_QUIT;
		}
	}
	else if (GetKeyboardTrigger(DIK_S) ||
		GetJoypadTrigger(0, JOYKEY_DOWN) == true)
	{
		g_nSelect++;
		if (g_nSelect >= PAUSE_MENU_MAX)
		{
			g_nSelect = PAUSE_MENU_CONTINUE;
		}

	}
	if (GetKeyboardTrigger(DIK_RETURN) ||
		GetJoypadTrigger(0, JOYKEY_A) == true)
	{
		switch (g_nSelect)
		{
		case PAUSE_MENU_CONTINUE:
			SetEnablePause(false);
			g_pauseMenu = PAUSE_MENU_CONTINUE;
			break;

		case PAUSE_MENU_RETRY:
			g_pauseMenu = PAUSE_MENU_RETRY;
			SetFade(MODE_GAME);
			break;

		case PAUSE_MENU_QUIT:
			g_pauseMenu = PAUSE_MENU_QUIT;
			SetFade(MODE_TITLE);
			break;

		}
	}
	for (int nCntPause = 0; nCntPause < MAX_PAUSE_OBJ; nCntPause++)
	{
		if (g_nSelect == nCntPause)
		{// 選択されている場合

			if (g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].sizeBig == g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].size)
			{ // 文字が大きいところから小さくなる

				if (g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].bSelect == false)
				{
					g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].sizeBig = g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].size * 8;
				}
				else
				{ // サイズが元に戻ったら

					g_aPause[nCntPause + MAX_PAUSE_OBJ].bSelect = true;
					g_aPause[nCntPause + MAX_PAUSE_OBJ].bDisp = true;
				}
			}
			g_aPause[nCntPause].bSelect = true;
			g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].bSelect = true;
			g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].bDisp = true;
		}
		else
		{// 選択されていない場合
			g_aPause[nCntPause].bSelect = false;
			g_aPause[nCntPause + MAX_PAUSE_OBJ].bSelect = false;
			g_aPause[nCntPause + MAX_PAUSE_OBJ].bDisp = false;
			g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].bSelect = false;
			g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].bDisp = false;
		}
	}

	for (int nCntPause = 0; nCntPause < MAX_PAUSE; nCntPause++)
	{
		// sizeBigに合わせたサイズに
		pVtx[0].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x - g_aPause[nCntPause].sizeBig.x, g_aPause[nCntPause].pos.y - g_aPause[nCntPause].sizeBig.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x + g_aPause[nCntPause].sizeBig.x, g_aPause[nCntPause].pos.y - g_aPause[nCntPause].sizeBig.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x - g_aPause[nCntPause].sizeBig.x, g_aPause[nCntPause].pos.y + g_aPause[nCntPause].sizeBig.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aPause[nCntPause].pos.x + g_aPause[nCntPause].sizeBig.x, g_aPause[nCntPause].pos.y + g_aPause[nCntPause].sizeBig.y, 0.0f);

		if (g_aPause[nCntPause].sizeBig.x > g_aPause[nCntPause].size.x)
		{ // 割合で元に戻していく

			g_aPause[nCntPause].sizeBig -= {g_aPause[nCntPause].size.x * 0.37f,
				g_aPause[nCntPause].size.y * 0.37f,
				0.0f};
		}
		else if (g_aPause[nCntPause + MAX_PAUSE_OBJ * 2].bSelect == false)
		{ // sizeBigをsizeと同じ値にする

			g_aPause[nCntPause].sizeBig = g_aPause[nCntPause].size;
		}

		if (g_aPause[nCntPause].bSelect == true)
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

		if (g_aPause[nCntPause].bDisp == true)
		{
			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCntPause * MAX_VERTEX,		// 描画する最初の頂点インデックス
				MAX_POLYGON);				// 描画するプリミティブ数
		}
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
// ポーズメニューの設定
//=============================================================================
void ResetPause(void)
{
	g_nSelect = 0;

	g_aPause[0] =
	{ // POS,SIZE,ファイル名

		{ 320.0f, 200.0f, 0.0f },
		{ 400.0f, 200.0f, 0.0f },
		{ 400.0f, 200.0f, 0.0f },
		"data\\TEXTURE\\pause_ink.png",
		false,
		true
	};
	g_aPause[1] =
	{ // POS,SIZE,ファイル名
		 { 1000.0f, 360.0f, 0.0f },
		{ 470.0f, 230.0f, 0.0f },
		{ 470.0f, 230.0f, 0.0f },
		"data\\TEXTURE\\pause_ink2.png",
		false,
		true
	};
	g_aPause[2] =
	{ // POS,SIZE,ファイル名
		 { 370.0f, 560.0f, 0.0f },
		{ 400.0f, 190.0f, 0.0f },
		{ 400.0f, 190.0f, 0.0f },
		"data\\TEXTURE\\pause_ink3.png",
		false,
		true
	};
	g_aPause[3] =
	{ // POS,SIZE,ファイル名
		{ 320.0f, 200.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		"data\\TEXTURE\\Continue_ink000.png",
		false,
		false
	};
	g_aPause[4] =
	{ // POS,SIZE,ファイル名
		{ 1000.0f, 360.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		"data\\TEXTURE\\Retry_ink000.png",
		false,
		false
	};
	g_aPause[5] =
	{ // POS,SIZE,ファイル名
		{ 370.0f, 560.0f, 0.0f},
		{ 345.0f, 184.0f, 0.0f },
		{ 345.0f, 184.0f, 0.0f },
		"data\\TEXTURE\\Quit_ink000.png",
		false,
		false
	};
	g_aPause[6] =
	{ // POS,SIZE,ファイル名
		{ 320.0f, 200.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		"data\\TEXTURE\\Continue001.png",
		false,
		false
	};
	g_aPause[7] =
	{ // POS,SIZE,ファイル名
		{ 1000.0f, 360.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		{ 320.0f, 170.0f, 0.0f },
		"data\\TEXTURE\\Retry001.png",
		false,
		false
	};
	g_aPause[8] =
	{ // POS,SIZE,ファイル名
		{ 370.0f, 560.0f, 0.0f},
		{ 345.0f, 184.0f, 0.0f },
		{ 345.0f, 184.0f, 0.0f },
		"data\\TEXTURE\\Quit001.png",
		false,
		false
	};
}

//=============================================================================
// ポーズメニューの取得
//=============================================================================
PAUSE_MENU* GetPauseMenu(void)
{
	return &g_pauseMenu;
}