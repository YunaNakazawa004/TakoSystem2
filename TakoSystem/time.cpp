//=============================================================================
// 
// タイム [time.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "time.h"
#include "player.h"
#include "pause.h"
#include "fade.h"

//#include "game.h"

// マクロ定義
#define	NUM_PLACE	(5)		// タイムの桁数
#define	HEIGHT_TIME_1	(100)		// タイムの桁数
#define	HEIGHT_TIME_2	(88)		// タイムの桁数

#define	TIME_TRIP_60	(60)		// タイムの周期
#define	TIME_TRIP_100	(100)		// タイムの周期

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureTime = NULL;		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTime = NULL;			// 頂点バッファへのポインタ
D3DXVECTOR3 g_posTime;		// タイムの位置

int g_nTime;				// タイムの値
int g_nTimeDelay;			// 時間の遅れ
float g_nTime100Delay;		// 時間の遅れ

// タイムの初期化処理
void InitTime(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	//// ステージ情報の取得
	//int pStage;
	//pStage = GetStage();

	//// リトライ情報の取得
	//bool pRetry;
	//pRetry = GetRetry();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/number000.png",
		&g_pTextureTime);

	//if (pStage == 0 || pRetry == true)
	//{// 最初のステージ又はリトライ時のみの処理
	g_nTimeDelay = 0;
	g_nTime100Delay = 0;
	//}

	g_posTime = D3DXVECTOR3(545.0f, 15.0f, 0.0f);	// 位置を初期化する

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * NUM_PLACE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTime,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTime->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTime = 0; nCntTime < NUM_PLACE; nCntTime++)
	{// タイムの最大数まで繰り返す
		// 頂点座標の設定
		if (nCntTime < 3)
		{
			pVtx[0].pos.x = g_posTime.x + (nCntTime * (HEIGHT_TIME_1 / 2));
			pVtx[0].pos.y = g_posTime.y;
			pVtx[0].pos.z = 0.0f;

			pVtx[1].pos.x = g_posTime.x + (HEIGHT_TIME_1 / 2) + (nCntTime * (HEIGHT_TIME_1 / 2));
			pVtx[1].pos.y = g_posTime.y;
			pVtx[1].pos.z = 0.0f;

			pVtx[2].pos.x = g_posTime.x + (nCntTime * (HEIGHT_TIME_1 / 2));
			pVtx[2].pos.y = g_posTime.y + HEIGHT_TIME_1;
			pVtx[2].pos.z = 0.0f;

			pVtx[3].pos.x = g_posTime.x + (HEIGHT_TIME_1 / 2) + (nCntTime * (HEIGHT_TIME_1 / 2));
			pVtx[3].pos.y = g_posTime.y + HEIGHT_TIME_1;
			pVtx[3].pos.z = 0.0f;
		}
		else
		{
			pVtx[0].pos.x = g_posTime.x + (nCntTime * 20) + (2 * 50);
			pVtx[0].pos.y = g_posTime.y + HEIGHT_TIME_2 - 40;
			pVtx[0].pos.z = 0.0f;

			pVtx[1].pos.x = g_posTime.x + 20 + (nCntTime * 20) + (2 * 50);
			pVtx[1].pos.y = g_posTime.y + HEIGHT_TIME_2 - 40;
			pVtx[1].pos.z = 0.0f;

			pVtx[2].pos.x = g_posTime.x + (nCntTime * 20) + (2 * 50);
			pVtx[2].pos.y = g_posTime.y + HEIGHT_TIME_2;
			pVtx[2].pos.z = 0.0f;

			pVtx[3].pos.x = g_posTime.x + 20 + (nCntTime * 20) + (2 * 50);
			pVtx[3].pos.y = g_posTime.y + HEIGHT_TIME_2;
			pVtx[3].pos.z = 0.0f;
		}
		// rhwの設定
		pVtx[0].rhw = 1.0f;	// 値は1.0fで固定
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = DEFAULT_COLOR;
		pVtx[1].col = DEFAULT_COLOR;
		pVtx[2].col = DEFAULT_COLOR;
		pVtx[3].col = DEFAULT_COLOR;

		// UV座標設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTime->Unlock();
}

// タイムの終了処理
void UninitTime(void)
{
	if (g_pTextureTime != NULL)
	{// テクスチャの破棄
		g_pTextureTime->Release();
		g_pTextureTime = NULL;
	}

	if (g_pVtxBuffTime != NULL)
	{// 頂点バッファの破棄
		g_pVtxBuffTime->Release();
		g_pVtxBuffTime = NULL;
	}
}

// タイムの更新処理
void UpdateTime(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	if (pFade == FADE_NONE)
	{
		g_nTimeDelay++;
		g_nTime100Delay += 1.6666667f;

		if ((g_nTimeDelay % TIME_TRIP_60) == 0)
		{// 時間を減らす
			g_nTimeDelay = 0;
			g_nTime100Delay = 0;
			AddTime(-1);
		}
		else
		{// 下の桁を減らす
			AddTime(0);
		}

		// 時間切れ
		if (g_nTime < 0) SetFade(MODE_RESULT);
	}
}

// タイムの描画処理
void DrawTime(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTime, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntTime = 0; nCntTime < NUM_PLACE; nCntTime++)
	{// タイムの最大数まで繰り返す
		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureTime);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTime * 4, 2);
	}
}

// タイムの設定処理
void SetTime(int nTime)
{
	int aTexU[NUM_PLACE];
	g_nTime = nTime;
	float fData = (g_nTimeDelay - g_nTime100Delay);

	//// ステージ情報の取得
	//int pStage;
	//pStage = GetStage();

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTime->Lock(0, 0, (void**)&pVtx, 0);

	if (g_nTime < 0)
	{// 制限時間0秒未満
		aTexU[0] = 0;
		aTexU[1] = 0;
		aTexU[2] = 0;
		aTexU[3] = 0;
		aTexU[4] = 0;
	}
	else
	{
		aTexU[0] = (g_nTime % 1000) / 100;
		aTexU[1] = (g_nTime % 100) / 10;
		aTexU[2] = (g_nTime % 10) / 1;
		aTexU[3] = ((TIME_TRIP_100 - (g_nTimeDelay - (int)fData)) % 100) / 10;
		aTexU[4] = ((TIME_TRIP_100 - (g_nTimeDelay - (int)fData)) % 10) / 1;
	}

	for (int nCntTime = 0; nCntTime < NUM_PLACE; nCntTime++)
	{
		// UV座標設定
		pVtx[0].tex = D3DXVECTOR2((aTexU[nCntTime] * 0.1f), 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f + (aTexU[nCntTime] * 0.1f), 0.0f);
		pVtx[2].tex = D3DXVECTOR2((aTexU[nCntTime] * 0.1f), 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f + (aTexU[nCntTime] * 0.1f), 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}

	//if (pStage == 0)
	//{// 最初のステージのみの処理
	g_nTimeDelay = TIME_TRIP_60 - 1;
	//}

	// 頂点バッファをアンロックする
	g_pVtxBuffTime->Unlock();
}

// タイムの加算処理
void AddTime(int nValue)
{
	int aTexU[NUM_PLACE];
	g_nTime += nValue;

	float fData = (g_nTimeDelay - g_nTime100Delay);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTime->Lock(0, 0, (void**)&pVtx, 0);

	if (g_nTime < 0)
	{// 制限時間0秒未満
		aTexU[0] = 0;
		aTexU[1] = 0;
		aTexU[2] = 0;
		aTexU[3] = 0;
		aTexU[4] = 0;
	}
	else
	{
		aTexU[0] = (g_nTime % 1000) / 100;
		aTexU[1] = (g_nTime % 100) / 10;
		aTexU[2] = (g_nTime % 10) / 1;
		aTexU[3] = ((TIME_TRIP_100 - (g_nTimeDelay - (int)fData)) % 100) / 10;
		aTexU[4] = ((TIME_TRIP_100 - (g_nTimeDelay - (int)fData)) % 10) / 1;
	}

	for (int nCntTime = 0; nCntTime < NUM_PLACE; nCntTime++)
	{
		// UV座標設定
		pVtx[0].tex = D3DXVECTOR2((aTexU[nCntTime] * 0.1f), 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f + (aTexU[nCntTime] * 0.1f), 0.0f);
		pVtx[2].tex = D3DXVECTOR2((aTexU[nCntTime] * 0.1f), 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f + (aTexU[nCntTime] * 0.1f), 1.0f);

		if (g_nTime > PINCH_TIME)
		{// 制限時間15秒以上
			pVtx[0].col = DEFAULT_COLOR;
			pVtx[1].col = DEFAULT_COLOR;
			pVtx[2].col = DEFAULT_COLOR;
			pVtx[3].col = DEFAULT_COLOR;
		}
		else
		{// 制限時間15秒未満
			pVtx[0].col = PINCH_COLOR;
			pVtx[1].col = PINCH_COLOR;
			pVtx[2].col = PINCH_COLOR;
			pVtx[3].col = PINCH_COLOR;
		}
		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTime->Unlock();
}

// タイムの取得
int GetTime(void)
{
	return g_nTime;
}