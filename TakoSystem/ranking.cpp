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

int g_rStage = 0;								// 現在のステージ

GAMESTATE g_gamerState = GAMESTATE_BEGIN;		// ゲームの状態

//=====================================
// マクロ定義
//=====================================

#define MAX_RANK		(7)								// ランク数
#define TEXT_NAME		"data/RANKING/RankData.bin"		// 相対パスのテキスト名
#define	MAX_RANKING2D	(20)							// 2Dオブジェクトの数
#define	MAX_RANKING3D	(20)							// 3Dオブジェクトの数
#define MAX_RANKINGOBJ	(100)							// 取っておく枠
#define MAX_RANK_TEX	(5)
#define RANKTYPE_2D		(1)
#define RANKTYPE_3D		(2)

//=====================================
// グローバル変数
//=====================================

LPDIRECT3DTEXTURE9 g_pTextureRank[MAX_RANK_TEX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRank2D = NULL;		// 頂点バッファへのポインタ2D
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRank3D = NULL;		// 頂点バッファへのポインタ3D
RankingOBJ g_aRank[MAX_RANKINGOBJ] =					// ランキングオブジェクト
{// Pos,Col,Size,ワールドマトリックス,bUse,nType,TexIdx,NumIdx

	{{0.0f,0.0f,0.0f},{1.0f,0.0f,0.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{2} },
	{{0.0f,0.0f,0.0f},{0.2f,0.2f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{3} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{1} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{2} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{3} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{4} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{5} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{6} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{7} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{8} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{0} },
	{{0.0f,0.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{15.0f,15.0f,0.0f},{},{false},{RANKTYPE_3D},{4},{9} },
	{{640.0f,360.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1280.0f,720.0f,0.0f},{},{true},{RANKTYPE_2D},{0}},
	{{640.0f,360.0f,0.0f},{1.0f,1.0f,1.0f,1.0f},{1280.0f,720.0f,0.0f},{},{true},{RANKTYPE_2D},{1},{1}},
};

Ranking_Info g_aRank_Info[MAX_RANK];					// ランキング情報
GiveResultPlayer g_aGRP[MAX_RANK];

//=======================================
// ランキングの初期化処理
//=======================================
void InitRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	Fishes* pFishes = GetFishes();
	VERTEX_2D* pVtx2D;							// 頂点情報へのポインタ
	VERTEX_3D* pVtx3D;							// 頂点情報へのポインタ

	srand((unsigned int)time(NULL));

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(1.7f, 1.9f, 2.0f, 1.0f));
	SetLightColor(1, D3DXCOLOR(1.4f, 1.5f, 1.7f, 0.7f));
	SetLightColor(2, D3DXCOLOR(1.1f, 1.1f, 1.3f, 0.3f));

	// カメラの初期化処理
	SetNumCamera(1);
	SetCameraPos(0, { 0.0f,1000.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);

	// メッシュシリンダーの初期化処理
	InitMeshCylinder();
	SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 2.0f), D3DXVECTOR2(INCYLINDER_RADIUS, CYLINDER_HEIGHT), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), false, MESHCYLINDERTYPE_ROCK);

	// モデルの初期化処理
	InitFishes();

	// メッシュフィールドの初期化処理
	InitMeshField();
	SetMeshField(MESHFIELDTYPE_SNOW, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1,1 }, { 600,600 });

	// 3Dエフェクトの初期化処理
	InitEffect3D();

	// 3Dパーティクルの初期化処理
	InitParticle3D();

	GetRankingForResult(&g_aGRP[0], MAX_RANK);

	for (int nCntRank = 0; nCntRank < MAX_RANK; nCntRank++)
	{ // プレイヤー情報の初期化と代入

		g_aRank_Info[nCntRank].nCharaIdx = g_aGRP[nCntRank].typePlayer;
		g_aRank_Info[nCntRank].nScore = g_aGRP[nCntRank].nTotalScore;
		g_aRank_Info[nCntRank].nRank = 0;
		g_aRank_Info[0].nNumFirstRank = 0;
	}

	for (int nCntRank = 0, nCntRank2 = 0; nCntRank < MAX_RANK; nCntRank++, nCntRank2 = 0)
	{ // ランキング情報の代入とモデルの設置

		for (; nCntRank2 < MAX_RANK; nCntRank2++)
		{
			if (g_aRank_Info[nCntRank].nScore > g_aRank_Info[nCntRank2].nScore || g_aRank_Info[nCntRank].nScore == g_aRank_Info[nCntRank2].nScore)
			{
				g_aRank_Info[nCntRank].nRank--;
			}

			g_aRank_Info[nCntRank2].nRank++;
		}
		SetFishes(1, 1, false, { (-50.0f * (MAX_RANK / 2)) + nCntRank * 50.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
		SetMotionFishes(nCntRank, MOTIONTYPE_NEUTRAL, pFishes[nCntRank].bBlendMotion, pFishes[nCntRank].nFrameBlend);
	}

	for (int nCntRank = 0; nCntRank < MAX_RANK; nCntRank++)
	{ // １位の人数

		if (g_aRank_Info[nCntRank].nRank == 0)
		{
			g_aRank_Info[0].nNumFirstRank++;
		}
	}

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\test1.png",
		&g_pTextureRank[0]);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\test2.png",
		&g_pTextureRank[1]);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\numpin000.png",
		&g_pTextureRank[2]);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\numpin001.png",
		&g_pTextureRank[3]);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\Cpupin.png",
		&g_pTextureRank[4]);



	if (MAX_RANKING3D > 0)
	{
		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * MAX_RANKING3D,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffRank3D,
			NULL);

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_pVtxBuffRank3D->Lock(0, 0, (void**)&pVtx3D, 0);

		for (int nCntRank = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
		{
			if (g_aRank[nCntRank].nType == RANKTYPE_3D)
			{
				pVtx3D[0].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x - (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y + (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);	// 右回りで設定する
				pVtx3D[1].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x + (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y + (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);	// 2Dの場合Zの値は0にする
				pVtx3D[2].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x - (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y - (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);
				pVtx3D[3].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x + (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y - (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);

				// norの設定					 
				pVtx3D[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
				pVtx3D[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
				pVtx3D[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
				pVtx3D[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

				// 頂点カラーの設定
				pVtx3D[0].col = g_aRank[nCntRank].col;	// 0~255の値を設定
				pVtx3D[1].col = g_aRank[nCntRank].col;
				pVtx3D[2].col = g_aRank[nCntRank].col;
				pVtx3D[3].col = g_aRank[nCntRank].col;

				if (g_aRank[nCntRank].TexIdx == 4)
				{
					pVtx3D[0].tex = D3DXVECTOR2(0.0f, 0.1f * g_aRank[nCntRank].NumIdx);
					pVtx3D[1].tex = D3DXVECTOR2(1.0f, 0.1f * g_aRank[nCntRank].NumIdx);
					pVtx3D[2].tex = D3DXVECTOR2(0.0f, 0.1f * g_aRank[nCntRank].NumIdx + 0.1f);
					pVtx3D[3].tex = D3DXVECTOR2(1.0f, 0.1f * g_aRank[nCntRank].NumIdx + 0.1f);
				}
				else
				{
					pVtx3D[0].tex = D3DXVECTOR2(0.0f, 0.0f);
					pVtx3D[1].tex = D3DXVECTOR2(1.0f, 0.0f);
					pVtx3D[2].tex = D3DXVECTOR2(0.0f, 1.0f);
					pVtx3D[3].tex = D3DXVECTOR2(1.0f, 1.0f);
				}

				pVtx3D += 4;		// 頂点データのポインタを4つ分進める
			}
		}
		// 頂点バッファをアンロックする
		g_pVtxBuffRank3D->Unlock();

	}
	if (MAX_RANKING2D > 0)
	{
		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VERTEX * MAX_RANKING2D,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_2D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffRank2D,
			NULL);

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_pVtxBuffRank2D->Lock(0, 0, (void**)&pVtx2D, 0);

		for (int nCntRank = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
		{
			if (g_aRank[nCntRank].nType == RANKTYPE_2D)
			{
				pVtx2D[0].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x - (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y - (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);	// 右回りで設定する
				pVtx2D[1].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x + (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y - (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);	// 2Dの場合Zの値は0にする
				pVtx2D[2].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x - (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y + (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);
				pVtx2D[3].pos = D3DXVECTOR3(g_aRank[nCntRank].pos.x + (g_aRank[nCntRank].size.x / 2), g_aRank[nCntRank].pos.y + (g_aRank[nCntRank].size.y / 2), g_aRank[nCntRank].pos.z);

				// rhwの設定
				pVtx2D[0].rhw = 1.0f;
				pVtx2D[1].rhw = 1.0f;
				pVtx2D[2].rhw = 1.0f;
				pVtx2D[3].rhw = 1.0f;

				// 頂点カラーの設定
				pVtx2D[0].col = g_aRank[nCntRank].col;	// 0~255の値を設定
				pVtx2D[1].col = g_aRank[nCntRank].col;
				pVtx2D[2].col = g_aRank[nCntRank].col;
				pVtx2D[3].col = g_aRank[nCntRank].col;

				if (g_aRank[nCntRank].TexIdx == 4)
				{
					pVtx2D[0].tex = D3DXVECTOR2(0.0f, 0.1f * g_aRank[nCntRank].NumIdx);
					pVtx2D[1].tex = D3DXVECTOR2(1.0f, 0.1f * g_aRank[nCntRank].NumIdx);
					pVtx2D[2].tex = D3DXVECTOR2(0.0f, 0.1f * g_aRank[nCntRank].NumIdx + 0.1f);
					pVtx2D[3].tex = D3DXVECTOR2(1.0f, 0.1f * g_aRank[nCntRank].NumIdx + 0.1f);
				}
				else
				{
					pVtx2D[0].tex = D3DXVECTOR2(0.0f, 0.0f);
					pVtx2D[1].tex = D3DXVECTOR2(1.0f, 0.0f);
					pVtx2D[2].tex = D3DXVECTOR2(0.0f, 1.0f);
					pVtx2D[3].tex = D3DXVECTOR2(1.0f, 1.0f);
				}

				pVtx2D += 4;		// 頂点データのポインタを4つ分進める
			}
		}
		// 頂点バッファをアンロックする
		g_pVtxBuffRank2D->Unlock();
	}
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

	// モデルの終了処理
	UninitFishes();

	// メッシュフィールドの終了処理
	UninitMeshField();

	// 3Dエフェクトの終了処理
	UninitEffect3D();

	// 3Dパーティクルの終了処理
	UninitParticle3D();

	// カメラの終了処理
	UninitCamera();

	// サウンドの終了処理
	StopSound();

	// フェードの終了処理
	//UninitFade();

	// テクスチャの破棄
	for (int nCntRank = 0; nCntRank < MAX_RANK_TEX; nCntRank++)
	{ // タイトルの数だけ確認する

		if (g_pTextureRank[nCntRank] != NULL)
		{ // テクスチャの破棄

			g_pTextureRank[nCntRank]->Release();
			g_pTextureRank[nCntRank] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffRank2D != NULL)
	{
		g_pVtxBuffRank2D->Release();
		g_pVtxBuffRank2D = NULL;
	}
	// 頂点バッファの破棄
	if (g_pVtxBuffRank3D != NULL)
	{
		g_pVtxBuffRank3D->Release();
		g_pVtxBuffRank3D = NULL;
	}
}

//=======================================
// ランキングの更新処理
//=======================================
void UpdateRanking(void)
{
	// フェード情報の取得
	VERTEX_2D* pVtx2D;							// 頂点情報へのポインタ
	VERTEX_3D* pVtx3D;							// 頂点情報へのポインタ
	FADE pFade = GetFade();
	Fishes* pFishes = GetFishes();
	Camera* pCamera = GetCamera();
	int nPlayerNum = GetPlayerSelect();
	static float N = 1;
	int nPlayerCnt = 0;
	int nCpuCnt = 0;
	static int TimeCnt = 0;						// 経過時間のカウント

	if (pFade == FADE_NONE)
	{// フェードが何もしていない状態のみ発動

	}
#if 0
	if (GetKeyboardPress(DIK_A) == true)
	{
		N -= 1;
	}
#endif

	// ステージの更新処理
	//UpdateStage();

	// モデルの更新処理
	UpdateFishes();

	// メッシュフィールドの更新処理
	UpdateMeshField();

	// 3Dエフェクトの更新処理
	UpdateEffect3D();

	// 3Dパーティクルの更新処理
	UpdateParticle3D();

	for (int nCntRank = 0, n3D = 0, n2D = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{ // テクスチャ位置を動かす

		if (g_aRank[nCntRank].TexIdx == 1 && g_aRank[nCntRank].NumIdx == 1)
		{
			if (g_aRank[nCntRank].nType == RANKTYPE_3D)
			{
				// 頂点バッファをロックし、頂点情報へのポインタを取得
				g_pVtxBuffRank3D->Lock(0, 0, (void**)&pVtx3D, 0);

				pVtx3D += n3D * 4;		// 頂点データのポインタを4つ分進める

				pVtx3D[0].tex += D3DXVECTOR2(0.0005f, 0.0005f);
				pVtx3D[1].tex += D3DXVECTOR2(0.0005f, 0.0005f);
				pVtx3D[2].tex += D3DXVECTOR2(0.0005f, 0.0005f);
				pVtx3D[3].tex += D3DXVECTOR2(0.0005f, 0.0005f);

				// 頂点バッファをアンロックする
				g_pVtxBuffRank3D->Unlock();
			}
			else if (g_aRank[nCntRank].nType == RANKTYPE_2D)
			{
				// 頂点バッファをロックし、頂点情報へのポインタを取得
				g_pVtxBuffRank2D->Lock(0, 0, (void**)&pVtx2D, 0);

				pVtx2D += n2D * 4;		// 頂点データのポインタを4つ分進める

				pVtx2D[0].tex -= D3DXVECTOR2(0.0005f, 0.0005f);
				pVtx2D[1].tex -= D3DXVECTOR2(0.0005f, 0.0005f);
				pVtx2D[2].tex -= D3DXVECTOR2(0.0005f, 0.0005f);
				pVtx2D[3].tex -= D3DXVECTOR2(0.0005f, 0.0005f);

				// 頂点バッファをアンロックする
				g_pVtxBuffRank2D->Unlock();
			}
		}
		else
		{
			if (g_aRank[nCntRank].nType == RANKTYPE_3D)
			{
				n3D++;
			}
			else if (g_aRank[nCntRank].nType == RANKTYPE_2D)
			{
				n2D++;
			}

		}
	}

	for (int nCntRank = 0, nCntRank2 = 0, nPlayerCnt = 0, nCpuCnt = 0; nCntRank < MAX_RANK; nCntRank++)
	{ // 番号をプレイヤーの上に移動させる

		if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_PLAYER)
		{ // プレイヤーなら

			g_aRank[nPlayerCnt].pos = { pFishes[nCntRank].pos.x ,pFishes[nCntRank].pos.y + 60 ,pFishes[nCntRank].pos.z };
			g_aRank[nPlayerCnt].bUse = true;
			nPlayerCnt++;
		}
		else if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
		{ // コンピューターなら

			g_aRank[2 + nCpuCnt].pos = { pFishes[nCntRank].pos.x - g_aRank[2 + nCpuCnt].size.x / 2 ,pFishes[nCntRank].pos.y + 60 ,pFishes[nCntRank].pos.z };
			g_aRank[2 + nCpuCnt + 1].pos = { pFishes[nCntRank].pos.x + g_aRank[2 + nCpuCnt + 1].size.x / 2 ,pFishes[nCntRank].pos.y + 60 ,pFishes[nCntRank].pos.z };

			if (TimeCnt == 0)
			{
				g_aRank[2 + nCpuCnt].bUse = true;
				g_aRank[2 + nCpuCnt + 1].bUse = true;
			}

			nCpuCnt += 2;
		}
	}
	if (TimeCnt == 0)
	{
		N = 0;
		SetCameraPos(0, { 0.0f ,70.0f ,-220.0f }, { 0.0f,55.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
	}

	TimeCnt++;

	if (TimeCnt == 120)
	{
		// サウンドの再生
		PlaySound(SOUND_SE_DRUMROLL);
	}
	if (TimeCnt > 120 && TimeCnt < 300)
	{
		// カメラを横に流していく
		N += 300 / 180.0f;
		SetCameraPos(0, { (-50.0f * (MAX_RANK / 2)) + N,45.0f ,-130.0f }, { (-50.0f * (MAX_RANK / 2)) + N,45.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
	}
	if (TimeCnt > 300)
	{
		SetCameraPos(0, { 0.0f ,80.0f ,-235.0f }, { 0.0f,55.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
	}
	if (TimeCnt == 420)
	{
		// サウンドの再生
		PlaySound(SOUND_BGM_RANKING);

		// １位を中心に移動、その他を周りに倒す
		float y = 0, rot = 0;

		for (int nCntRank = 0, nRank = 0, nCpuCnt = 0; nCntRank < MAX_RANK; nCntRank++)
		{
			if (g_aRank_Info[nCntRank].nRank == 0)
			{
				pFishes[nCntRank].pos = { (-20.0f * (g_aRank_Info[0].nNumFirstRank - 1)) + (nRank * 40.0f),30.0f,0.0f };
				SetMotionFishes(nCntRank, MOTIONTYPE_WIN1, pFishes[nCntRank].bBlendMotion, pFishes[nCntRank].nFrameBlend);

				/*	if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
					{
						g_aRank[2 + nCpuCnt].bUse = true;
						g_aRank[2 + nCpuCnt + 1].bUse = true;
					}*/
				nRank++;
			}
			else
			{
				y += 6.28f / (MAX_RANK - g_aRank_Info[0].nNumFirstRank);
				rot = ((rand() % 618) - 314) / 100.0f;

				pFishes[nCntRank].move.x = 0.0f + sinf(y - D3DX_PI) * (70.0f * 1);
				pFishes[nCntRank].move.z = -20.0f + cosf(y - D3DX_PI) * (30.0f * 1);

				pFishes[nCntRank].pos = { pFishes[nCntRank].move.x, -15.0f,pFishes[nCntRank].move.z };
				pFishes[nCntRank].rot.y = rot;

				SetMotionFishes(nCntRank, MOTIONTYPE_DEATH, pFishes[nCntRank].bBlendMotion, pFishes[nCntRank].nFrameBlend);

				/*if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
				{
					g_aRank[2 + nCpuCnt].bUse = false;
					g_aRank[2 + nCpuCnt + 1].bUse = false;
				}*/
			}
			if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
			{
				nCpuCnt += 2;
			}
		}
	}

	// 一定時間経過ORキー入力
	if ((GetKeyboardTrigger(DIK_RETURN) == true ||
		GetJoypadTrigger(0, JOYKEY_START) == true ||
		GetJoypadTrigger(0, JOYKEY_A) == true)
		&& pFade == FADE_NONE)
	{// 決定キー（ENTERキー）が押された
		//if (g_nRankDeley < TITLE_DELEY)
		//{// クリックで反応
		//	PlaySound(SOUND_LABEL_SE_SCORE);	// 再生したいサウンドを指定
		//}
		TimeCnt = 0;
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

	for (int nCntRank = 0, VertexNum = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{  // 2Dオブジェクトの処理

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aRank[nCntRank].mtxWorld);

		// ビューマトリックスを取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		if (g_aRank[nCntRank].nType == RANKTYPE_2D)
		{
			pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aRank[nCntRank].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffRank2D, 0, sizeof(VERTEX_2D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_2D);

			if (g_aRank[nCntRank].bUse == true)
			{
				// テクスチャの設定
				pDevice->SetTexture(0, g_pTextureRank[g_aRank[nCntRank].TexIdx]);

				// ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
					VertexNum * MAX_VERTEX,	// 描画する最初の頂点インデックス
					MAX_POLYGON);	// 描画するプリミティブ数
			}

			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			VertexNum++;
		}

	}

	DrawFishes();

	//DrawMeshField();

	// ライティングを無効化（影の影響を受けない）
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (int nCntRank = 0, VertexNum = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{ // 3Dオブジェクトの処理

		if (g_aRank[nCntRank].nType == RANKTYPE_3D)
		{
			// ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aRank[nCntRank].mtxWorld, NULL, &mtxView);	// 逆行列を求める
			g_aRank[nCntRank].mtxWorld._41 = 0.0f;
			g_aRank[nCntRank].mtxWorld._42 = 0.0f;
			g_aRank[nCntRank].mtxWorld._43 = 0.0f;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aRank[nCntRank].pos.x, g_aRank[nCntRank].pos.y, g_aRank[nCntRank].pos.z);
			D3DXMatrixMultiply(&g_aRank[nCntRank].mtxWorld, &g_aRank[nCntRank].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aRank[nCntRank].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffRank3D, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			if (g_aRank[nCntRank].bUse == true)
			{
				// Zテストを無効にする
				//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
				//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Zバッファ更新の無効の設定

				//アルファテストを有効にする
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファブレンドを有効に設定
				pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ( , 比較方法(より大きい))
				pDevice->SetRenderState(D3DRS_ALPHAREF, 100);				// ( , 基準値)

				// テクスチャの設定
				pDevice->SetTexture(0, g_pTextureRank[g_aRank[nCntRank].TexIdx]);

				// ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
					VertexNum * MAX_VERTEX,		// 描画する最初の頂点インデックス
					MAX_POLYGON);				// 描画するプリミティブ数

				// Zテストを有効にする
				//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
				//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ更新の有効の設定

				//// アルファテストを無効にする
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);			// アルファブレンドを無効に設定
				pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);		// ( , 比較方法(すべて))
				pDevice->SetRenderState(D3DRS_ALPHAREF, 0);						// ( , 基準値)
			}
			VertexNum++;
		}
	}

	// 元に戻す
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// 3Dエフェクトの描画処理
	DrawEffect3D();

	// 3Dパーティクルの描画処理
	DrawParticle3D();

}