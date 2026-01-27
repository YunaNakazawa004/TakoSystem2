//=============================================================================
// 
// ランキング [ranking.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "ranking.h"
#include "sound.h"
#include "input.h"
#include "fade.h"

// マクロ定義
#define TEXT_NAME	"data/RANKING/RankingData.bin"	// 相対パスのテキスト名
#define	TITLE_DELEY	(480)	// タイトル移行に掛かる時間

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureRanking = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRanking = NULL;	// 頂点バッファへのポインタ

Ranking g_aRanking[MAX_RANK];	// ランキング情報
int g_nRankingUpdate = -1;		// 更新ランクNo.
int g_nRankingDeley;			// ランキング表示時間

// ランキングの初期化処理
void InitRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	g_nRankingDeley = 0;	// 表示時間の初期化

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data/TEXTURE/RANKING.png",
		&g_pTextureRanking);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffRanking,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffRanking->Lock(0, 0, (void**)&pVtx, 0);

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
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// UV座標設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffRanking->Unlock();
	
	//// スコアのセット処理
	//SetScoreRanking(g_aRanking[0].nScore, g_aRanking[1].nScore, g_aRanking[2].nScore, g_aRanking[3].nScore, g_aRanking[4].nScore);
	
	//// サウンドの再生
	//PlaySound(SOUND_LABEL_RANKING);		// 再生したいサウンドを指定
}

// ランキングの終了処理
void UninitRanking(void)
{
	g_nRankingUpdate = -1;
	// サウンドの停止
	StopSound();

	// テクスチャの破棄
	if (g_pTextureRanking != NULL)
	{
		g_pTextureRanking->Release();
		g_pTextureRanking = NULL;
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffRanking != NULL)
	{
		g_pVtxBuffRanking->Release();
		g_pVtxBuffRanking = NULL;
	}
}

// ランキングの更新処理
void UpdateRanking(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	g_nRankingDeley++;

	//// ランキング点滅
	//if (g_nRankingUpdate != -1)
	//{// ランキングが更新された
	//	UpdateScore(g_nRankingUpdate);
	//}

	// 一定時間経過ORキー入力
	if ((GetKeyboardTrigger(DIK_RETURN) == true || 
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true)
		&& pFade == FADE_NONE || 
		g_nRankingDeley > TITLE_DELEY && pFade == FADE_NONE)
	{// 決定キー（ENTERキー）が押された
		//if (g_nRankingDeley < TITLE_DELEY)
		//{// クリックで反応
		//	PlaySound(SOUND_LABEL_SE_SCORE);	// 再生したいサウンドを指定
		//}
		SetFade(MODE_TITLE);
	}
}

// ランキングの描画処理
void DrawRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffRanking, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureRanking);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//// ランキングのリセット
//void ResetRanking(void)
//{
//	FILE* pFile;	// ファイルポインタを宣言
//
//	// 問題を外部ファイルから読み込む
//	pFile = fopen(TEXT_NAME, "rb");
//
//	if (pFile != NULL)
//	{// ファイルを開けた場合
//		for (int nRanking = 0; nRanking < MAX_RANK; nRanking++)
//		{// 順位の数だけ繰り返す
//			fscanf(pFile, "%d", &g_aRanking[nRanking].nScore);
//		}
//		// ファイルを閉じる
//		fclose(pFile);
//	}
//	else
//	{// ファイルが開けなかった場合
//		g_aRanking[0].nScore = 20000;
//		g_aRanking[1].nScore = 18000;
//		g_aRanking[2].nScore = 16000;
//		g_aRanking[3].nScore = 14000;
//		g_aRanking[4].nScore = 12000;
//	}
//}
//
//// ランキングの設定
//void SetRanking(int nScore)
//{
//	for (int nCntRank = 0; nCntRank < MAX_RANK; nCntRank++)
//	{
//		// ランキング更新
//		if (g_aRanking[nCntRank].nScore < nScore)
//		{// 指定のスコアより上の場合
//			if (g_nRankingUpdate == -1)
//			{// ランキングが更新された
//				g_nRankingUpdate = nCntRank;
//			}
//		}
//	}
//
//	// ランキングの更新を確認
//	if (g_nRankingUpdate != -1)
//	{
//		// ランクを全て入れ替えるまで繰り返す
//		for (int nSetRank = 0; nSetRank < MAX_RANK - g_nRankingUpdate; nSetRank++)
//		{
//			if ((4 - g_nRankingUpdate) == nSetRank)
//			{// 指定のランキングにスコアを入れる
//				g_aRanking[4 - nSetRank].nScore = nScore;
//			}
//			else
//			{// ランキングを更新する
//				g_aRanking[4 - nSetRank].nScore = g_aRanking[3 - nSetRank].nScore;
//			}
//		}
//	}
//
//	FILE* pFile;	// ファイルポインタを宣言
//
//	pFile = fopen(TEXT_NAME, "wb");
//
//	if (pFile != NULL)
//	{// ファイルを開けた場合
//
//		for (int nRanking = 0; nRanking < MAX_RANK; nRanking++)
//		{// ランキングの数だけ繰り返す
//			fprintf(pFile, "%d\n", g_aRanking[nRanking].nScore);
//		}
//
//		// ファイルを閉じる
//		fclose(pFile);
//	}
//	else printf("<!> ファイルが開けませんでした");	// ファイルが開けなかった場合
//
//	//// 順位が高いほど振動時間が上昇
//	//if (g_nRankingUpdate != -1) SetJoypadVibration(10000, 10000, 20 * (MAX_RANK - g_nRankingUpdate));
//}