//=============================================================================
// 
// チュートリアル [tutorial.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "tutorial.h"
#include "sound.h"
#include "input.h"
#include "light.h"
#include "fade.h"
#include "model.h"

// マクロ定義
#define	MAX_TUTORIAL	(2)	// タイトルで表示するテクスチャの最大数

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTutorial[MAX_TUTORIAL] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTutorial = NULL;	// 頂点バッファへのポインタ

// リザルトの初期化処理
void InitTutorial(void)
{
	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.8f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.5f, 0.6f, 0.8f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.3f));

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/In_the_sea.png",
		&g_pTextureTutorial[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/TUTORIAL.png",
		&g_pTextureTutorial[1]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TEXTURE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTutorial,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTutorial->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTutorial = 0; nCntTutorial < MAX_TUTORIAL; nCntTutorial++)
	{
		// 頂点座標の設定
		if (nCntTutorial == 0)
		{// 背景
			pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
		else
		{// RESULTロゴ
			pVtx[0].pos = D3DXVECTOR3(460.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(820.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(460.0f, 180.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(820.0f, 180.0f, 0.0f);
		}

		// rhwの設定
		pVtx[0].rhw = DEFAULT_RHW;	// 値は1.0fで固定
		pVtx[1].rhw = DEFAULT_RHW;
		pVtx[2].rhw = DEFAULT_RHW;
		pVtx[3].rhw = DEFAULT_RHW;

		// 頂点カラーの設定
		pVtx[0].col = WHITE_VTX;	// 0~255の値を設定
		pVtx[1].col = WHITE_VTX;
		pVtx[2].col = WHITE_VTX;
		pVtx[3].col = WHITE_VTX;

		// UV座標設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffTutorial->Unlock();

	// サウンドの再生
	PlaySound(SOUND_BGM_TUTORIAL);
}

// リザルトの終了処理
void UninitTutorial(void)
{
	// サウンドの停止
	StopSound();

	// テクスチャの破棄
	for (int nCntTutorial = 0; nCntTutorial < MAX_TUTORIAL; nCntTutorial++)
	{// タイトルの数だけ確認する
		if (g_pTextureTutorial[nCntTutorial] != NULL)
		{// テクスチャの破棄
			g_pTextureTutorial[nCntTutorial]->Release();
			g_pTextureTutorial[nCntTutorial] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTutorial != NULL)
	{
		g_pVtxBuffTutorial->Release();
		g_pVtxBuffTutorial = NULL;
	}
}

// リザルトの更新処理
void UpdateTutorial(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true) && pFade == FADE_NONE)
	{// 決定キー（ENTERキー）が押された
		// モード設定
		SetFade(MODE_GAME);
	}
}

// リザルトの描画処理
void DrawTutorial(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTutorial, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTutorial = 0; nCntTutorial < MAX_TUTORIAL; nCntTutorial++)
	{// 敵の最大数まで繰り返す
		// テクスチャのM
		pDevice->SetTexture(0, g_pTextureTutorial[nCntTutorial]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTutorial * 4, 2);
	}
}