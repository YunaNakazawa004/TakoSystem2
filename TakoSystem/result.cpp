//=============================================================================
// 
// リザルト [result.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "ranking.h"
#include "result.h"
#include "sound.h"
#include "input.h"
#include "light.h"
#include "fade.h"
#include "camera.h"

// マクロ関数
#define	MAX_RESULT	(3)	// タイトルで表示するテクスチャの最大数

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureResult[MAX_RESULT] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResult = NULL;	// 頂点バッファへのポインタ
Result g_aResult;

int g_ResultDeley;	// リザルトのスコア移動時間

// リザルトの初期化処理
void InitResult(void)
{
	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.8f, 0.9f, 1.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.5f, 0.6f, 0.8f, 0.7f));
	SetLightColor(2, D3DXCOLOR(0.3f, 0.3f, 0.5f, 0.3f));

	// カメラの数の設定
	SetNumCamera(1);

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	g_ResultDeley = 0;	// スコア移動時間を初期化

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/In_the_sea.png",
		&g_pTextureResult[0]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/RESULT.png",
		&g_pTextureResult[1]);

	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/RESULT_LINE.png",
		&g_pTextureResult[2]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_RESULT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffResult,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffResult->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntResult = 0; nCntResult < MAX_RESULT; nCntResult++)
	{
		// 頂点座標の設定
		if (nCntResult == 0)
		{// 背景
			pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);
		}
		else if (nCntResult == 1)
		{// RESULTロゴ
			pVtx[0].pos = D3DXVECTOR3(460.0f, 0.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(820.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(460.0f, 180.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(820.0f, 180.0f, 0.0f);
		}
		else
		{// 集中線
			pVtx[0].pos = D3DXVECTOR3(800.0f, 100.0f, 0.0f);	// 右回りで設定する
			pVtx[1].pos = D3DXVECTOR3(1200.0f, 100.0f, 0.0f);	// 2Dの場合Zの値は0にする
			pVtx[2].pos = D3DXVECTOR3(800.0f, 500.0f, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(1200.0f, 500.0f, 0.0f);
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
	g_pVtxBuffResult->Unlock();

	//// スコアのセット処理
	//SetScore(g_aResult.nAddScore, SCORE_RESULT);

	// サウンドの再生
	PlaySound(SOUND_BGM_RESULT);
}

// リザルトの終了処理
void UninitResult(void)
{
	// サウンドの停止
	StopSound();

	// テクスチャの破棄
	for (int nCntResult = 0; nCntResult < MAX_RESULT; nCntResult++)
	{// タイトルの数だけ確認する
		if (g_pTextureResult[nCntResult] != NULL)
		{// テクスチャの破棄
			g_pTextureResult[nCntResult]->Release();
			g_pTextureResult[nCntResult] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffResult != NULL)
	{
		g_pVtxBuffResult->Release();
		g_pVtxBuffResult = NULL;
	}
}

// リザルトの更新処理
void UpdateResult(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	g_ResultDeley++;

	//if ((g_ResultDeley % 4) == 0 && g_ResultDeley > 90)
	//{// 特定の時間で発動
	//	if (g_aResult.nAddLife > 0)
	//	{// ライフの数だけスコア増加
	//		g_aResult.nAddLife--;
	//		AddScore(500, SCORE_RESULT);
	//		PlaySound(SOUND_LABEL_SE_SCORE);	// 再生したいサウンドを指定
	//	}
	//	else if (g_aResult.nAddTime > 0)
	//	{// タイムの数だけスコア増加
	//		g_aResult.nAddTime--;
	//		AddScore(50, SCORE_RESULT);
	//		PlaySound(SOUND_LABEL_SE_SCORE);	// 再生したいサウンドを指定
	//	}
	//}

	if ((GetKeyboardTrigger(DIK_RETURN) == true || 
		GetJoypadTrigger(0, JOYKEY_START) == true || 
		GetJoypadTrigger(0, JOYKEY_A) == true) &&
		pFade == FADE_NONE && 
		g_aResult.nAddLife == 0 && g_aResult.nAddTime == 0)
	{// 決定キー（ENTERキー）が押された
		
		//// ランキングにスコアを入れる
		//SetRanking(GetScore());

		// モード設定
		SetFade(MODE_RANKING);
	}

	//if ((GetKeyboardTrigger(DIK_RETURN) == true ||
	//	GetJoypadTrigger(JOYKEY_START) == true ||
	//	GetJoypadTrigger(JOYKEY_A_BUTTON) == true))
	//{// 特定のキーを押すと即座に計算
	//	do
	//	{// ライフとタイムが無くなるまで計算
	//		if (g_aResult.nAddLife > 0)
	//		{// ライフの数だけスコア増加
	//			g_aResult.nAddLife--;
	//			AddScore(500, SCORE_RESULT);
	//		}
	//		else if (g_aResult.nAddTime > 0)
	//		{// タイムの数だけスコア増加
	//			g_aResult.nAddTime--;
	//			AddScore(50, SCORE_RESULT);
	//		}
	//	} while(g_aResult.nAddLife != 0 || g_aResult.nAddTime != 0);
	//	PlaySound(SOUND_LABEL_SE_SCORE);	// 再生したいサウンドを指定
	//}
}

// リザルトの描画処理
void DrawResult(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffResult, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntResult = 0; nCntResult < MAX_RESULT; nCntResult++)
	{// 敵の最大数まで繰り返す
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureResult[nCntResult]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntResult * 4, 2);
	}
}

// スコアをリザルトへ代入
void SetResult(int nScore, int nLife, int nTime)
{
	g_aResult.nAddScore = nScore;
	g_aResult.nAddLife = nLife;
	g_aResult.nAddTime = nTime;
}

// ボスの取得
Result* GetResult(void)
{
	return &g_aResult;	// リザルト情報の先頭アドレスを返す
}