//=============================================================================
// 
// ロゴ [logo.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "title.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "camera.h"

// マクロ定義
#define	MAX_LOGO	(2)	// ロゴで表示するテクスチャの最大数
#define	LOGO_MOVE	(1.0f)	// ロゴが大きくなるサイズ
#define	LOGO_DELEY_MAX	(150.0f)	// ロゴの最大サイズ
#define	TITLE_TRANS	(LOGO_DELEY_MAX + 50.0f)	// タイトルまでの時間

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureLogo[MAX_LOGO] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffLogo = NULL;	// 頂点バッファへのポインタ

float g_LogoDeley;	// ロゴの表示時間
float g_LogoSizeMove;	// ロゴのサイズ変化
float g_LogoOut;	// ロゴの透明化

// ロゴの初期化処理
void InitLogo(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	g_LogoDeley = 0.0f;	// ロゴの値を初期化
	g_LogoSizeMove = 0.0f;
	g_LogoOut = 0.0f;

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/In_the_sea.png",
		&g_pTextureLogo[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/LOGO.png",
		&g_pTextureLogo[1]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_LOGO,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffLogo,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffLogo->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{
		// 頂点座標の設定
		if (nCntLogo == 0)
		{// 背景
			pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);

			pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);	// 0~255の値を設定
			pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		}
		else if (nCntLogo == 1)
		{// ロゴ
			pVtx[0].pos = D3DXVECTOR3(590.0f, 310.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(690.0f, 310.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(590.0f, 410.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(690.0f, 410.0f, 0.0f);

			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// rhwの設定
		pVtx[0].rhw = 1.0f;	// 値は1.0fで固定
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// テクスチャ
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffLogo->Unlock();

	PlaySound(SOUND_SE_BOOT);	// 起動音
}

// ロゴの終了処理
void UninitLogo(void)
{
	// テクスチャの破棄
	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{// ロゴの数だけ確認する
		if (g_pTextureLogo[nCntLogo] != NULL)
		{// テクスチャの破棄
			g_pTextureLogo[nCntLogo]->Release();
			g_pTextureLogo[nCntLogo] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffLogo != NULL)
	{
		g_pVtxBuffLogo->Release();
		g_pVtxBuffLogo = NULL;
	}
}

// ロゴの更新処理
void UpdateLogo(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	// 特定の位置まで繰り返す
	if (g_LogoDeley < LOGO_DELEY_MAX) g_LogoSizeMove += LOGO_MOVE;
	else
	{// ロゴをフェードアウト
		g_LogoOut += LOGO_MOVE * 0.02f;
		g_LogoSizeMove += LOGO_MOVE;
	}
	g_LogoDeley += LOGO_MOVE;

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffLogo->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{
		// 頂点座標の設定
		if (nCntLogo == 0)
		{// 背景
			pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);
		}
		else
		{// ロゴ
			pVtx[0].pos = D3DXVECTOR3(590.0f - g_LogoSizeMove, 310.0f - g_LogoSizeMove, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(690.0f + g_LogoSizeMove, 310.0f - g_LogoSizeMove, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(590.0f - g_LogoSizeMove, 410.0f + g_LogoSizeMove, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(690.0f + g_LogoSizeMove, 410.0f + g_LogoSizeMove, 0.0f);

			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - g_LogoOut);	// 0~255の値を設定
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - g_LogoOut);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - g_LogoOut);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f - g_LogoOut);
		}

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}	
	
	// 頂点バッファをアンロックする
	g_pVtxBuffLogo->Unlock();

	if (pFade == FADE_NONE && g_LogoDeley > TITLE_TRANS)
	{
		SetFade(MODE_TITLE);
	}

	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true) &&
		g_LogoDeley < LOGO_DELEY_MAX)
	{// 特定のキーを押すと即座にタイトル
		SetFade(MODE_TITLE);
	}
}

// ロゴの描画処理
void DrawLogo(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffLogo, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{// 敵の最大数まで繰り返す
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureLogo[nCntLogo]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntLogo * 4, 2);
	}
}