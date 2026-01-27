//=============================================================================
// 
// タイトル [title.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "title.h"
#include "input.h"
#include "sound.h"
#include "light.h"
#include "fade.h"
#include "game.h"

// マクロ定義
#define	MAX_TITLE	(2)	// タイトルの最大数
#define	RANKING_DELEY	(720)	// ランキング移行に掛かる時間
#define	CLEAR_DELEY	(60)	// 消滅にかかる時間
#define	TITLE_DELEY_MAX	(500.0f)	// タイトルの最大数

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTitle[MAX_TITLE] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle = NULL;	// 頂点バッファへのポインタ

float g_TitleDeley;	// タイトル移動表示時間
int g_PressEnterDeley;	// PRESSENTER表示時間

// タイトルの初期化処理
void InitTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	g_TitleDeley = 0.0f;	// ディレイの値を初期化
	g_PressEnterDeley = 0;

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.8f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.5f, 0.6f, 0.8f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.3f));

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	
	//D3DXCreateTextureFromFile(pDevice,
	//	"data/TEXTURE/TITLE_BG.png",
	//	&g_pTextureTitle[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/TITLE.png",
		&g_pTextureTitle[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/PRESS.png",
		&g_pTextureTitle[1]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TITLE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{
		// 頂点座標の設定
		if (nCntTitle == 0)
		{// タイトル：ロゴ
			pVtx[0].pos = D3DXVECTOR3(160.0f, -540, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1120.0f, -540, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(160.0f, 0.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1120.0f, 0.0f, 0.0f);
		}
		else
		{// タイトル：PRESS ENTER
			pVtx[0].pos = D3DXVECTOR3(480.0f, 450.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(800.0f, 450.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(480.0f, 630.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(800.0f, 630.0f, 0.0f);
		}

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

		// UV座標設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();

	//// サウンドの再生
	//PlaySound(SOUND_LABEL_TITLE);		// 再生したいサウンドを指定
}

// タイトルの終了処理
void UninitTitle(void)
{
	// サウンドの停止
	StopSound();

	// テクスチャの破棄
	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{// タイトルの数だけ確認する
		if (g_pTextureTitle[nCntTitle] != NULL)
		{// テクスチャの破棄
			g_pTextureTitle[nCntTitle]->Release();
			g_pTextureTitle[nCntTitle] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTitle != NULL)
	{
		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}
}

// タイトルの更新処理
void UpdateTitle(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	if (g_TitleDeley < TITLE_DELEY_MAX)
	{// 特定の位置まで繰り返す
		g_TitleDeley += 2.5f;
	}
	g_PressEnterDeley++;

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{
		// 頂点座標の設定
		if (nCntTitle == 0)
		{// タイトルのpVtx
			pVtx[0].pos = D3DXVECTOR3(160.0f, -540 + g_TitleDeley, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1120.0f, -540 + g_TitleDeley, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(160.0f, 0.0f + g_TitleDeley, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1120.0f, 0.0f + g_TitleDeley, 0.0f);
		}
		else
		{// タイトル：PRESS ENTER
			if (pFade == FADE_OUT && g_PressEnterDeley <= RANKING_DELEY)
			{// PRESSENTERをクリック
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f * (g_PressEnterDeley % 3));
			}
			else if ((g_PressEnterDeley % 100) > CLEAR_DELEY)
			{// 一定間隔で消滅
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			}
			else
			{// それ以外
				pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
				pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}

	if ((GetKeyboardTrigger(DIK_RETURN) == true || 
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true) &&
		pFade == FADE_NONE && g_TitleDeley == TITLE_DELEY_MAX)
	{// 決定キー（ENTERキー）が押された
		// モード設定
		//PlaySound(SOUND_LABEL_SE_SCORE);	// 再生したいサウンドを指定
		SetFade(MODE_TUTORIAL);
	}
	else if (pFade == FADE_NONE && g_PressEnterDeley > RANKING_DELEY)
	{
		SetFade(MODE_RANKING);
	}

	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true))
	{// 特定のキーを押すと即座に到着
		g_TitleDeley = TITLE_DELEY_MAX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();

}

// タイトルの描画処理
void DrawTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTitle, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTitle = 0; nCntTitle < MAX_TITLE; nCntTitle++)
	{// 敵の最大数まで繰り返す
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureTitle[nCntTitle]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTitle * 4, 2);
	}
}