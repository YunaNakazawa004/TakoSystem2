//=============================================================================
// 
// ランキング [ranking.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "camera.h"
#include "light.h"

#include "meshcylinder.h"
#include "meshdome.h"
#include "meshfield.h"
#include "meshring.h"
#include "meshorbit.h"

#include "sound.h"
#include "fishes.h"
#include "watersurf.h"

#include "effect_3d.h"
#include "particle_3d.h"
#include "input.h"
#include "fade.h"
#include "screen.h"
#include "title.h"
#include "result.h"

#include "game.h"
#include "ranking.h"

int g_rStage = 0;		// 現在のステージ

GAMESTATE g_gamerState = GAMESTATE_BEGIN;	// ゲームの状態

// マクロ定義
#define TEXT_NAME	"data/RANKING/RankingData.bin"	// 相対パスのテキスト名
#define	MAX_RANKING2D	(1)	// ランキングの最大数
#define	MAX_RANKING3D	(1)	// ランキングの最大数
#define MAX_RANK_TEX	(2)
#define RANKTYPE_2D		(0)
#define RANKTYPE_3D		(1)

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureRanking[MAX_RANK_TEX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRanking2D = NULL;	// 頂点バッファへのポインタ2D
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRanking3D = NULL;	// 頂点バッファへのポインタ3D
Ranking g_aRanking[MAX_RANK];	// ランキング情報

int g_nRankingUpdate = -1;		// 更新ランクNo.

//=======================================
// ランキングの初期化処理
//=======================================
void InitRanking(void)
{
	srand((unsigned int)time(NULL));

	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D* pVtx2D;	// 頂点情報へのポインタ
	VERTEX_3D* pVtx3D;	// 頂点情報へのポインタ
	int nCntRanking = 0;

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(1.7f, 1.9f, 2.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(1.4f, 1.5f, 1.7f, 0.7f));
	SetLightColor(2, D3DXCOLOR(1.1f, 1.1f, 1.3f, 0.3f));

	// カメラの初期化処理
	SetNumCamera(GetPlayerSelect());
	SetCameraPos(0, { 0.0f,1000.0f,0.0f }, { 0.0f,0.0f,0.0f }, {0.0f,0.0f,0.0f}, CAMERATYPE_STOP);

	g_gamerState = GAMESTATE_BEGIN;	// ゲームの状態を開始状態に設定


	// ステージの初期化処理
	//InitStage();

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, MESHCYLINDERTYPE_ROCK);

	InitFishes();

	InitMeshField();
	SetMeshField(MESHFIELDTYPE_SNOW, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1,1 }, { 600,600 });

	// 3Dエフェクトの初期化処理
	InitEffect3D();

	// 3Dパーティクルの初期化処理
	InitParticle3D();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\test1.png",
		&g_pTextureRanking[0]);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\numpin000.png",
		&g_pTextureRanking[1]);

	if (MAX_RANKING3D > 0)
	{
		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * MAX_RANKING3D,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffRanking3D,
			NULL);

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_pVtxBuffRanking3D->Lock(0, 0, (void**)&pVtx3D, 0);

		for (; nCntRanking < MAX_RANKING3D; nCntRanking++)
		{
			g_aRanking[nCntRanking].bUse = true;
			g_aRanking[nCntRanking].pos = { 0.0f,0.0f,0.0f };
			g_aRanking[nCntRanking].nType = RANKTYPE_3D;
			g_aRanking[nCntRanking].TexIdx = 1;

			pVtx3D[0].pos = D3DXVECTOR3(-10.0f, 10.0f, 0.0f);		// 右回りで設定する
			pVtx3D[1].pos = D3DXVECTOR3(10.0f, 10.0f, 0.0f);		// 2Dの場合Zの値は0にする
			pVtx3D[2].pos = D3DXVECTOR3(-10.0f, -10.0f, 0.0f);
			pVtx3D[3].pos = D3DXVECTOR3(10.0f, -10.0f, 0.0f);

			// norの設定					 
			pVtx3D[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx3D[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx3D[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx3D[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			// 頂点カラーの設定
			pVtx3D[0].col = WHITE_VTX;	// 0~255の値を設定
			pVtx3D[1].col = WHITE_VTX;
			pVtx3D[2].col = WHITE_VTX;
			pVtx3D[3].col = WHITE_VTX;

			pVtx3D[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx3D[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx3D[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx3D[3].tex = D3DXVECTOR2(1.0f, 1.0f);

			pVtx3D += 4;		// 頂点データのポインタを4つ分進める
		}
		// 頂点バッファをアンロックする
		g_pVtxBuffRanking3D->Unlock();

	}
	if (MAX_RANKING2D > 0)
	{
		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VERTEX * MAX_RANKING2D,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_2D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffRanking2D,
			NULL);

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_pVtxBuffRanking2D->Lock(0, 0, (void**)&pVtx2D, 0);

		for (; nCntRanking < MAX_RANKING2D + MAX_RANKING3D; nCntRanking++)
		{
			g_aRanking[nCntRanking].bUse = true;
			g_aRanking[nCntRanking].pos = { 0.0f,0.0f,0.0f };
			g_aRanking[nCntRanking].nType = RANKTYPE_2D;
			g_aRanking[nCntRanking].TexIdx = 0;

			pVtx2D[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);			// 右回りで設定する
			pVtx2D[1].pos = D3DXVECTOR3(1280.0f, 0.0f, 0.0f);		// 2Dの場合Zの値は0にする
			pVtx2D[2].pos = D3DXVECTOR3(0.0f, 720.0f, 0.0f);
			pVtx2D[3].pos = D3DXVECTOR3(1280.0f, 720.0f, 0.0f);

			// rhwの設定
			pVtx2D[0].rhw = 1.0f;
			pVtx2D[1].rhw = 1.0f;
			pVtx2D[2].rhw = 1.0f;
			pVtx2D[3].rhw = 1.0f;

			// 頂点カラーの設定
			pVtx2D[0].col = WHITE_VTX;	// 0~255の値を設定
			pVtx2D[1].col = WHITE_VTX;
			pVtx2D[2].col = WHITE_VTX;
			pVtx2D[3].col = WHITE_VTX;

			pVtx2D[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx2D[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx2D[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx2D[3].tex = D3DXVECTOR2(1.0f, 1.0f);

			pVtx2D += 4;		// 頂点データのポインタを4つ分進める
		}
		// 頂点バッファをアンロックする
		g_pVtxBuffRanking2D->Unlock();
	}

	//// スコアのセット処理
	//SetScoreRanking(g_aRanking[0].nScore, g_aRanking[1].nScore, g_aRanking[2].nScore, g_aRanking[3].nScore, g_aRanking[4].nScore);

	SetFishes(1, 1, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	SetFishes(1, 1, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	SetFishes(1, 1, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	SetFishes(1, 1, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	SetFishes(1, 1, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	SetFishes(1, 1, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
	SetFishes(1, 1, false, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

	// サウンドの再生
	PlaySound(SOUND_BGM_RANKING);
}

//=======================================
// ランキングの終了処理
//=======================================
void UninitRanking(void)
{
	// サウンドの停止
	StopSound();

	// ステージの終了処理
	//UninitStage();

	UninitFishes();

	UninitMeshField();

	// 3Dエフェクトの終了処理
	UninitEffect3D();

	// 3Dパーティクルの終了処理
	UninitParticle3D();


	UninitCamera();

	// サウンドの終了処理
	//StopSound();
	//UninitSound();

	// フェードの終了処理
	//UninitFade();

	// テクスチャの破棄
	for (int nCntRanking = 0; nCntRanking < MAX_RANK_TEX; nCntRanking++)
	{// タイトルの数だけ確認する
		if (g_pTextureRanking[nCntRanking] != NULL)
		{// テクスチャの破棄
			g_pTextureRanking[nCntRanking]->Release();
			g_pTextureRanking[nCntRanking] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffRanking2D != NULL)
	{
		g_pVtxBuffRanking2D->Release();
		g_pVtxBuffRanking2D = NULL;
	}
	// 頂点バッファの破棄
	if (g_pVtxBuffRanking3D != NULL)
	{
		g_pVtxBuffRanking3D->Release();
		g_pVtxBuffRanking3D = NULL;
	}
}

//=======================================
// ランキングの更新処理
//=======================================
void UpdateRanking(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();
	Fishes* pFishes = GetFishes();
	Camera* pCamera = GetCamera();
	static int N = 1;
	static int V = 1;
	//int aHaveNum[MAX_PLAYER + MAX_COMPUTER][] = {};

	if (pFade == FADE_NONE)
	{// フェードが何もしていない状態のみ発動

	}
	if (GetKeyboardPress(DIK_A) == true)
	{
		N -= 1;
	}
	SetCameraPos(0, { 0.0f+N ,40.0f,-300.0f }, { 0.0f,60.0f,0.0f }, {0.0f,0.0f,0.0f}, CAMERATYPE_STOP);

	// ステージの更新処理
	//UpdateStage();

	UpdateFishes();

	UpdateMeshField();

	// 3Dエフェクトの更新処理
	UpdateEffect3D();

	// 3Dパーティクルの更新処理
	UpdateParticle3D();

	//// ランキング点滅
	//if (g_nRankingUpdate != -1)
	//{// ランキングが更新された
	//	UpdateScore(g_nRankingUpdate);
	//}

	for (int nCntRanking = 0; nCntRanking < MAX_RANKING2D + MAX_RANKING3D; nCntRanking++)
	{
		if (g_aRanking[nCntRanking].nType == RANKTYPE_3D)
		{
			g_aRanking[nCntRanking].pos = { pFishes[0].pos.x ,pFishes[0].pos.y + 60 ,pFishes[0].pos.z };
		}

	}

	// 一定時間経過ORキー入力
	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true)
		&& pFade == FADE_NONE)
	{// 決定キー（ENTERキー）が押された
		//if (g_nRankingDeley < TITLE_DELEY)
		//{// クリックで反応
		//	PlaySound(SOUND_LABEL_SE_SCORE);	// 再生したいサウンドを指定
		//}
		SetFade(MODE_TITLE);
	}
}

//=======================================
// ランキングの描画処理
//=======================================
void DrawRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ
	D3DXMATRIX mtxTrans;		// 計算用マトリックス
	D3DXMATRIX mtxView;			// ビューマトリックス取得用

	// デバイスの取得
	pDevice = GetDevice();

	// ステージの描画処理
	//DrawStage();

		// ライティングを無効化（影の影響を受けない）
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (int nCntRanking = 0, n = 0; nCntRanking < MAX_RANKING2D + MAX_RANKING3D; nCntRanking++)
	{
		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aRanking[nCntRanking].mtxWorld);

		// ビューマトリックスを取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		if (g_aRanking[nCntRanking].bUse == true)
		{
			if (g_aRanking[nCntRanking].nType == RANKTYPE_2D)
			{
				// ワールドマトリックスの設定
				pDevice->SetTransform(D3DTS_WORLD, &g_aRanking[nCntRanking].mtxWorld);

				// 頂点バッファをデータストリームに設定
				pDevice->SetStreamSource(0, g_pVtxBuffRanking2D, 0, sizeof(VERTEX_2D));

				// 頂点フォーマットの設定
				pDevice->SetFVF(FVF_VERTEX_2D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_pTextureRanking[g_aRanking[nCntRanking].TexIdx]);

				// ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
					n * MAX_VERTEX,	// 描画する最初の頂点インデックス
					MAX_POLYGON);	// 描画するプリミティブ数

				n++;
			}
		}
		// 2D描画
		pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
	}
	for (int nCntRanking = 0, n = 0; nCntRanking < MAX_RANKING2D + MAX_RANKING3D; nCntRanking++)
	{
		if (g_aRanking[nCntRanking].bUse == true)
		{
			if (g_aRanking[nCntRanking].nType == RANKTYPE_3D)
			{
				// Zテストを無効にする
				pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
				pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Zバッファ更新の無効の設定

				//// ポリゴンをカメラに対して正面に向ける
				//D3DXMatrixInverse(&g_aRanking[nCntRanking].mtxWorld, NULL, &mtxView);	// 逆行列を求める
				//g_aRanking[nCntRanking].mtxWorld._41 = 0.0f;
				//g_aRanking[nCntRanking].mtxWorld._42 = 0.0f;
				//g_aRanking[nCntRanking].mtxWorld._43 = 0.0f;

				// 位置を反映
				D3DXMatrixTranslation(&mtxTrans, g_aRanking[nCntRanking].pos.x, g_aRanking[nCntRanking].pos.y, g_aRanking[nCntRanking].pos.z);
				D3DXMatrixMultiply(&g_aRanking[nCntRanking].mtxWorld, &g_aRanking[nCntRanking].mtxWorld, &mtxTrans);

				// ワールドマトリックスの設定
				pDevice->SetTransform(D3DTS_WORLD, &g_aRanking[nCntRanking].mtxWorld);

				// 頂点バッファをデータストリームに設定
				pDevice->SetStreamSource(0, g_pVtxBuffRanking3D, 0, sizeof(VERTEX_3D));

				// 頂点フォーマットの設定
				pDevice->SetFVF(FVF_VERTEX_3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_pTextureRanking[g_aRanking[nCntRanking].TexIdx]);

				// ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
					n * MAX_VERTEX,		// 描画する最初の頂点インデックス
					MAX_POLYGON);		// 描画するプリミティブ数

				n++;

				// Zテストを有効にする
				pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
				pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ更新の有効の設定

			}
		}
	}

	// 元に戻す
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	DrawFishes();

	DrawMeshField();

	// 3Dエフェクトの描画処理
	DrawEffect3D();

	// 3Dパーティクルの描画処理
	DrawParticle3D();

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