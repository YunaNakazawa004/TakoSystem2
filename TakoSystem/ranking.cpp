//=============================================================================
// 
// ランキング [ranking.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "camera.h"
#include "light.h"

#include "meshdome.h"
#include "meshring.h"
#include "meshorbit.h"

#include "sound.h"
#include "fishes.h"
#include "watersurf.h"

#include "confetti.h"
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
#define MAX_RANKINGOBJ	(100)							// 取っておく枠
#define RANKTYPE_2D		(0)
#define RANKTYPE_3D		(1)
#define RANK_CALC_SIZEARRAY(aArray)	(sizeof aArray / sizeof(aArray[0]))	// サイズ比較

//=====================================
// 構造体の定義
//=====================================
// 
// テクスチャ情報 ---------------------

typedef struct
{
	char TextureName[100];		// テクスチャ名
	D3DXVECTOR2 Block;			// 分割数

}Ranking_Texture_Info;

//=====================================
// グローバル変数
//=====================================

int g_nObjNum[2];											// オブジェクト数
int nRankNum[MAX_RANK][2] = {};								// 各順位の人数
D3DLIGHT9* pLight = GetLight();
D3DLIGHT9 oldLight[3];
GiveResultPlayer g_aGRP[MAX_RANK];
RankingOBJ g_aRankOBJ[MAX_RANKINGOBJ] = {};					// ランキングオブジェクト
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRank2D = NULL;			// 頂点バッファへのポインタ2D
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRank3D = NULL;			// 頂点バッファへのポインタ3D

Ranking_Info g_aRank_Info[MAX_RANK];						// ランキング情報
Ranking_Texture_Info g_aRank_TexInfo[] =			   
{ // テクスチャパス,分割数

	{"data\\TEXTURE\\ranking_bg.png",{1,1}},
	{"data\\TEXTURE\\ranking_bg_octo.png",{1,1}},
	{"data\\TEXTURE\\numpin000.png",{1,1}},
	{"data\\TEXTURE\\numpin001.png",{1,1}},
	{"data\\TEXTURE\\Cpupin.png",{1,10}},
	{"data\\TEXTURE\\rankscore000.png",{1,1}},
	{"data\\TEXTURE\\rankscoreflame000.png",{1,1}},
	{"data\\TEXTURE\\point.png",{11,1}},
	{"data\\TEXTURE\\rankicon.png",{1,1}},
};
LPDIRECT3DTEXTURE9 g_pTextureRank[RANK_CALC_SIZEARRAY(g_aRank_TexInfo)] = {};	// テクスチャへのポインタ

//=======================================
// ランキングの初期化処理
//=======================================
void InitRanking(void)
{
	// ローカル変数宣言 -----------------

	int nCntObj2D = 0, nCntObj3D = 0;
	VERTEX_2D* pVtx2D;							// 頂点情報へのポインタ
	VERTEX_3D* pVtx3D;							// 頂点情報へのポインタ
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXVECTOR3 vecDir;		// ライトの方向ベクトル
	Fishes* pFishes = GetFishes();
	int nCharNum = GetPlayerSelect();
	g_nObjNum[0] = 0;
	g_nObjNum[1] = 0;
	GetRankingForResult(&g_aGRP[0], MAX_RANK);

	srand((unsigned int)time(NULL));

	oldLight[0] = pLight[0];
	oldLight[1] = pLight[1];
	oldLight[2] = pLight[2];

	// ライトの設定
	SetLightColor(0, D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f));
	SetLightColor(1, D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f));
	SetLightColor(2, D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f));

	// ライトの方向を設定
	vecDir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	pLight[0].Direction = vecDir;

	vecDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	pLight[1].Direction = vecDir;

	vecDir = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	pLight[2].Direction = vecDir;

	// ライトを設定する
	pDevice->SetLight(0, &pLight[0]);
	pDevice->SetLight(1, &pLight[1]);
	pDevice->SetLight(2, &pLight[2]);

	// カメラの初期化処理
	SetNumCamera(1);
	SetCameraPos(0, { 0.0f,1000.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);

	// モデルの初期化処理
	InitFishes();


	// 紙吹雪の初期化処理
	InitConfetti();

	for (int nCntRank = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{ // 初期化
		g_aRankOBJ[nCntRank].bDisp = false;
		g_aRankOBJ[nCntRank].bUse = false;
		g_aRankOBJ[nCntRank].col = { 0.0f,0.0f,0.0f,0.0f };
		g_aRankOBJ[nCntRank].nObjType = -1;
		g_aRankOBJ[nCntRank].nTexType = -1;
		g_aRankOBJ[nCntRank].NumIdx = { -1,-1 };
		g_aRankOBJ[nCntRank].pos = { 0.0f,0.0f,0.0f };
		g_aRankOBJ[nCntRank].size = { 0.0f,0.0f };
	}
	for (int nCntRank = 0; nCntRank < MAX_RANK; nCntRank++)
	{ // プレイヤー情報の初期化と代入

		g_aRank_Info[nCntRank].nCharaIdx = g_aGRP[nCntRank].typePlayer;
		g_aRank_Info[nCntRank].nScore = g_aGRP[nCntRank].nTotalScore;
		g_aRank_Info[nCntRank].nRank = 0;
		nRankNum[nCntRank][0] = 0;
		nRankNum[nCntRank][1] = 0;
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
		if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_PLAYER)
		{
			SetFishes(1, 1, false, { (-50.0f * (MAX_RANK / 2)) + nCntRank * 50.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, FISHESTYPE_PLAYER);
		}
		else if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
		{
			SetFishes(1, 1, false, { (-50.0f * (MAX_RANK / 2)) + nCntRank * 50.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, FISHESTYPE_COMPUTER);
		}
		SetMotionFishes(nCntRank, MOTIONTYPE_NEUTRAL, pFishes[nCntRank].bBlendMotion, pFishes[nCntRank].nFrameBlend);
	}
	for (int nCntRank = 0; nCntRank < MAX_RANK; nCntRank++)
	{ // 各順位の人数

		for (int nCntRank2 = 0; nCntRank2 < MAX_RANK; nCntRank2++)
		{
			if (g_aRank_Info[nCntRank2].nRank == nCntRank)
			{
				nRankNum[nCntRank][0]++;
			}
		}
	}

	SetRankingObj({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f,1.0f }, { 15.0f,15.0f }, RANKTYPE_3D, TEXTURE_PIN_P1, { -1 ,-1 }, true);

	if (nCharNum == 2)
	{
		SetRankingObj({ 0.0f,0.0f,0.0f }, { 0.2f,1.0f,0.2f,1.0f }, { 15.0f,15.0f }, RANKTYPE_3D, TEXTURE_PIN_P2, { -1 ,-1 }, true);
	}
	for (int nCntRank = 0; nCntRank < MAX_RANK - nCharNum; nCntRank++)
	{
		SetRankingObj({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 15.0f,15.0f }, RANKTYPE_3D, TEXTURE_PIN_CPU, { 0 ,0 }, true);
		SetRankingObj({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 15.0f,15.0f }, RANKTYPE_3D, TEXTURE_PIN_CPU, { 0 ,(float)nCntRank + 1 }, true);
	}

	SetRankingObj({ SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 1280.0f,720.0f }, RANKTYPE_2D, TEXTURE_BG, { -1 ,-1 }, true);
	SetRankingObj({ SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2,1.0f }, { 1.0f,1.0f,1.0f,0.8f }, { 1280.0f,720.0f }, RANKTYPE_2D, TEXTURE_BG_OCTO, { -1 ,-1 }, true);

	for (int nCntRank = 0; nCntRank < MAX_RANK; nCntRank++)
	{
		SetRankingObj(/*pos*/{ 1300.0f,70.0f + nCntRank * 90.0f,0.0f }, /*color*/{ 1.0f,1.0f,1.0f,1.0f },/*size*/{ 900.0f,90.0f }, RANKTYPE_2D, TEXTURE_RANKSCORE, { -1,-1 }, false);
		SetRankingObj(/*pos*/{ 1300.0f,70.0f + nCntRank * 90.0f,0.0f }, /*color*/{ 1.0f,1.0f,1.0f,1.0f },/*size*/{ 900.0f,90.0f }, RANKTYPE_2D, TEXTURE_RANKSCOREFLAME, { -1,-1 }, false);
	}

	for (int nCntRank = 0, nCntRank2 = 0, nPlayerCnt = 0, nCpuCnt = 0; nCntRank < MAX_RANK; nCntRank++)
	{ // 順位のOBJ設置

		if (g_aRank_Info[nCntRank].nRank == 0 && nRankNum[g_aRank_Info[nCntRank].nRank][1] == 0)
		{

			if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_PLAYER)
			{
				if (nPlayerCnt == 0)
				{
					SetRankingObj(/*pos*/{ 930.0f,70.0f  ,0.0f }, /*color*/{ 1.0f,1.0f,0.0f,1.0f },/*size*/{ 65.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_P1, { -1,-1 }, false);
				}
				else
				{
					SetRankingObj(/*pos*/{ 930.0f,70.0f  ,0.0f }, /*color*/{ 1.0f,1.0f,0.0f,1.0f },/*size*/{ 65.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_P2, { -1,-1 }, false);
				}
			}
			else if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
			{
				SetRankingObj(/*pos*/{ 930.0f - 19,70.0f   ,0.0f }, /*color*/{ 1.0f,1.0f,0.0f,1.0f },/*size*/{ 50.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_CPU, { 0,0 }, false);
				SetRankingObj(/*pos*/{ 930.0f + 24,70.0f   ,0.0f }, /*color*/{ 1.0f,1.0f,0.0f,1.0f },/*size*/{ 50.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_CPU, { 0,nCpuCnt + 1.0f }, false);
			}
			if (nRankNum[g_aRank_Info[nCntRank].nRank][0] != nRankNum[g_aRank_Info[nCntRank].nRank][1] + 1)
			{
				nRankNum[g_aRank_Info[nCntRank].nRank][1]++;
			}

			Score_Print({ 1175.0f ,70.0f ,0.0f }, RANKTYPE_2D, nCntRank, { 1.0f,1.0f,0.0f,1.0f });

			SetRankingObj(/*pos*/{ 840.0f - 19,70.0f  ,0.0f }, /*color*/{ 1.0f,1.0f,1.0f,1.0f },/*size*/{ 90.0f,90.0f }, RANKTYPE_2D, TEXTURE_RANKICON, { 0,0 }, false);
		}
		else
		{
			nCntRank2 = g_aRank_Info[nCntRank].nRank + nRankNum[g_aRank_Info[nCntRank].nRank][1];

			if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_PLAYER)
			{
				if (nPlayerCnt == 0)
				{
					SetRankingObj(/*pos*/{ 930.0f,70.0f + nCntRank2 * 90.0f ,0.0f }, /*color*/{ 1.0f,0.0f,0.0f,1.0f },/*size*/{ 65.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_P1, { -1,-1 }, false);
				}
				else
				{
					SetRankingObj(/*pos*/{ 930.0f,70.0f + nCntRank2 * 90.0f ,0.0f }, /*color*/{ 0.2f,1.0f,0.2f,1.0f },/*size*/{ 65.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_P2, { -1,-1 }, false);
				}
			}
			else if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
			{
				SetRankingObj(/*pos*/{ 930.0f - 19,70.0f + nCntRank2 * 90.0f  ,0.0f }, /*color*/{ 1.0f,1.0f,1.0f,1.0f },/*size*/{ 50.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_CPU, { 0,0 }, false);
				SetRankingObj(/*pos*/{ 930.0f + 24,70.0f + nCntRank2 * 90.0f  ,0.0f }, /*color*/{ 1.0f,1.0f,1.0f,1.0f },/*size*/{ 50.0f,65.0f }, RANKTYPE_2D, TEXTURE_PIN_CPU, { 0,nCpuCnt + 1.0f }, false);
			}
			if (nRankNum[g_aRank_Info[nCntRank].nRank][0] != nRankNum[g_aRank_Info[nCntRank].nRank][1] + 1)
			{
				nRankNum[g_aRank_Info[nCntRank].nRank][1]++;
			}

			Score_Print({ 1175.0f ,70.0f + nCntRank2 * 90.0f ,0.0f }, RANKTYPE_2D, nCntRank, { 1.0f,1.0f,1.0f,1.0f });
		}
		if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_PLAYER)
		{
			nPlayerCnt++;
		}
		else if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
		{
			nCpuCnt++;
		}
	}

	for (int nCntRank = 0; nCntRank < RANK_CALC_SIZEARRAY(g_aRank_TexInfo); nCntRank++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
			g_aRank_TexInfo[nCntRank].TextureName,
			&g_pTextureRank[nCntRank]);
	}

	// 2DOBJ頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VERTEX * g_nObjNum[RANKTYPE_2D],
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffRank2D,
		NULL);

	// 3DOBJ頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * g_nObjNum[RANKTYPE_3D],
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffRank3D,
		NULL);

	SetCameraPos(0, { 0.0f ,65.0f ,0.0f }, { 0.0f,35.0f,60.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffRank2D->Lock(0, 0, (void**)&pVtx2D, 0);

	VERTEX_2D* pCur2D = pVtx2D;					// 頂点情報へのポインタ

	for (int nCntRank = 0,nWrite = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{
		if (!g_aRankOBJ[nCntRank].bUse)
		{ // 使用されていない
			continue;
		}

		if (g_aRankOBJ[nCntRank].nObjType != RANKTYPE_2D)
		{ // not2Dオブジェクト
			continue;
		}

		pCur2D[0].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x - (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y - (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);	// 右回りで設定する
		pCur2D[1].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x + (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y - (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);	// 2Dの場合Zの値は0にする
		pCur2D[2].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x - (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y + (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);
		pCur2D[3].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x + (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y + (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);

		// rhwの設定
		pCur2D[0].rhw = 1.0f;
		pCur2D[1].rhw = 1.0f;
		pCur2D[2].rhw = 1.0f;
		pCur2D[3].rhw = 1.0f;

		// 頂点カラーの設定
		pCur2D[0].col = g_aRankOBJ[nCntRank].col;	// 0~255の値を設定
		pCur2D[1].col = g_aRankOBJ[nCntRank].col;
		pCur2D[2].col = g_aRankOBJ[nCntRank].col;
		pCur2D[3].col = g_aRankOBJ[nCntRank].col;

		if (g_aRankOBJ[nCntRank].NumIdx.x != -1 || g_aRankOBJ[nCntRank].NumIdx.y != -1)
		{
			pCur2D[0].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y));
			pCur2D[1].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x)), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y));
			pCur2D[2].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y)));
			pCur2D[3].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x)), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y)));
		}
		else
		{
			pCur2D[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pCur2D[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pCur2D[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pCur2D[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		pCur2D += 4;
		nWrite++;

		if (nWrite >= g_nObjNum[RANKTYPE_2D])
		{ //予定数書いたら終了
			break;
		}
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffRank2D->Unlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffRank3D->Lock(0, 0, (void**)&pVtx3D, 0);

	VERTEX_3D* pCur3D = pVtx3D;					// 頂点情報へのポインタ

	for (int nCntRank = 0, nWrite = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{
		if (!g_aRankOBJ[nCntRank].bUse)
		{ // 使用されてない
			continue;
		}

		if (g_aRankOBJ[nCntRank].nObjType != RANKTYPE_3D)
		{ // not2Dオブジェクト
			continue;
		}

		pCur3D[0].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x - (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y - (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);	// 右回りで設定する
		pCur3D[1].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x + (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y - (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);	// 2Dの場合Zの値は0にする
		pCur3D[2].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x - (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y + (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);
		pCur3D[3].pos = D3DXVECTOR3(g_aRankOBJ[nCntRank].pos.x + (g_aRankOBJ[nCntRank].size.x / 2), g_aRankOBJ[nCntRank].pos.y + (g_aRankOBJ[nCntRank].size.y / 2), g_aRankOBJ[nCntRank].pos.z);

		// rhwの設定
		pCur3D[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pCur3D[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pCur3D[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pCur3D[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 頂点カラーの設定
		pCur3D[0].col = g_aRankOBJ[nCntRank].col;	// 0~255の値を設定
		pCur3D[1].col = g_aRankOBJ[nCntRank].col;
		pCur3D[2].col = g_aRankOBJ[nCntRank].col;
		pCur3D[3].col = g_aRankOBJ[nCntRank].col;

		if (g_aRankOBJ[nCntRank].NumIdx.x != -1 || g_aRankOBJ[nCntRank].NumIdx.y != -1)
		{
			pCur3D[0].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y)));
			pCur3D[1].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x)), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y)));
			pCur3D[2].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y));
			pCur3D[3].tex = D3DXVECTOR2(0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.x) * g_aRankOBJ[nCntRank].NumIdx.x)), 0.0f + ((1.0f / g_aRank_TexInfo[g_aRankOBJ[nCntRank].nTexType].Block.y) * g_aRankOBJ[nCntRank].NumIdx.y));
		}
		else
		{
			pCur3D[0].tex = D3DXVECTOR2(0.0f, 1.0f);
			pCur3D[1].tex = D3DXVECTOR2(1.0f, 1.0f);
			pCur3D[2].tex = D3DXVECTOR2(0.0f, 0.0f);
			pCur3D[3].tex = D3DXVECTOR2(1.0f, 0.0f);
		}
		pCur3D += 4;
		nWrite++;

		if (nWrite >= g_nObjNum[RANKTYPE_3D])
		{ //予定数書いたら終了
			break;
		}
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffRank3D->Unlock();
}

//=======================================
// ランキングの終了処理
//=======================================
void UninitRanking(void)
{

	// モデルの終了処理
	UninitFishes();


	// 紙吹雪の終了処理
	UninitConfetti();


	// サウンドの停止
	StopSound();

	// テクスチャの破棄
	for (int nCntRank = 0; nCntRank < RANK_CALC_SIZEARRAY(g_aRank_TexInfo); nCntRank++)
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
	// ライトの方向を戻す
	pLight[0] = oldLight[0];
	pLight[1] = oldLight[1];
	pLight[2] = oldLight[2];
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
	static float N = 1;
	static int TimeCnt = 0;						// 経過時間のカウント
	float r = 0.0f, g = 0.0f, b = 0.0f;

	if (pFade == FADE_IN)
	{// フェードが何もしていない状態のみ発動
		TimeCnt = 0;
		N = 0;
	}
	else
	{
		TimeCnt++;
	}
#if 0
	if (GetKeyboardPress(DIK_A) == true)
	{
		N -= 1;
		SetCameraPos(0, { 0.0f + N ,80.0f ,-235.0f - N }, { 0.0f,55.0f,15.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
	}
	else
	{
		N = 0;
	}
#endif

	// モデルの更新処理
	UpdateFishes();


	// 紙吹雪の更新処理
	UpdateConfetti();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffRank2D->Lock(0, 0, (void**)&pVtx2D, 0);

	VERTEX_2D* pCur2D = pVtx2D;					// 頂点情報へのポインタ

	for (int nCntRank = 0, nWrite = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{ // テクスチャ位置を動かす

		if (!g_aRankOBJ[nCntRank].bUse ||g_aRankOBJ[nCntRank].nObjType != RANKTYPE_2D)
		{ // 使用されてない // 2Dでない
			continue;
		}
		if (g_aRankOBJ[nCntRank].nTexType != TEXTURE_BG_OCTO)
		{ // テクスチャタイプが違ったら

			nWrite++;
			pCur2D += 4;
			continue;
		}

		pCur2D[0].tex += D3DXVECTOR2(0.0005f, 0.0005f);
		pCur2D[1].tex += D3DXVECTOR2(0.0005f, 0.0005f);
		pCur2D[2].tex += D3DXVECTOR2(0.0005f, 0.0005f);
		pCur2D[3].tex += D3DXVECTOR2(0.0005f, 0.0005f);

		pCur2D += 4;		// 頂点データのポインタを4つ分進める
		nWrite++;

		if (nWrite >= g_nObjNum[RANKTYPE_2D])
		{ //予定数書いたら終了
			break;
		}
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffRank2D->Unlock();

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffRank3D->Lock(0, 0, (void**)&pVtx3D, 0);

	VERTEX_3D* pCur3D = pVtx3D;					// 頂点情報へのポインタ

	for (int nCntRank = 0, nWrite = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{ // テクスチャ位置を動かす

		if (!g_aRankOBJ[nCntRank].bUse || g_aRankOBJ[nCntRank].nObjType != RANKTYPE_3D)
		{ // 使用されてない// 3Dでない
			continue;
		}
		if (g_aRankOBJ[nCntRank].nTexType != TEXTURE_BG_OCTO)
		{ // テクスチャタイプが違ったら

			nWrite++;
			pCur3D += 4;
			continue;
		}

		pCur3D[0].tex += D3DXVECTOR2(0.0005f, 0.0005f);
		pCur3D[1].tex += D3DXVECTOR2(0.0005f, 0.0005f);
		pCur3D[2].tex += D3DXVECTOR2(0.0005f, 0.0005f);
		pCur3D[3].tex += D3DXVECTOR2(0.0005f, 0.0005f);

		pCur3D += 4;		// 頂点データのポインタを4つ分進める
		nWrite++;

		if (nWrite >= g_nObjNum[RANKTYPE_3D])
		{ //予定数書いたら終了
			break;
		}
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffRank3D->Unlock();

	for (int nCntRank = 0, nCntRank2 = 0, nCntRank3 = 0, nPlayerCnt = 0, nCpuCnt = 0; nCntRank < MAX_RANK; nCntRank++)
	{ // 番号をプレイヤーの上に移動させる

		if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_PLAYER)
		{ // プレイヤーなら

			for (nCntRank2 = 0; nCntRank2 < MAX_RANKINGOBJ; nCntRank2++)
			{
				if (g_aRankOBJ[nCntRank2].nTexType == TEXTURE_PIN_P1 && g_aRankOBJ[nCntRank2].nObjType == RANKTYPE_3D && nPlayerCnt == 0)
				{
					break;
				}
				else if (g_aRankOBJ[nCntRank2].nTexType == TEXTURE_PIN_P2 && g_aRankOBJ[nCntRank2].nObjType == RANKTYPE_3D && nPlayerCnt == 1)
				{
					break;
				}
			}
			g_aRankOBJ[nCntRank2].pos = { pFishes[nCntRank].pos.x ,pFishes[nCntRank].pos.y + 60 ,pFishes[nCntRank].pos.z };
			g_aRankOBJ[nCntRank2].bDisp = true;
			nPlayerCnt++;
		}
		else if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
		{ // コンピューターなら

			for (nCntRank2 = 0, nCntRank3 = 0; nCntRank2 < MAX_RANKINGOBJ; nCntRank2++)
			{
				if (g_aRankOBJ[nCntRank2].nTexType == TEXTURE_PIN_CPU && g_aRankOBJ[nCntRank2].nObjType == RANKTYPE_3D && g_aRankOBJ[nCntRank2].NumIdx.y == 0)
				{
					if (nCntRank3 >= nCpuCnt)
					{
						break;
					}
					nCntRank3++;
				}
			}
			g_aRankOBJ[nCntRank2].pos = { pFishes[nCntRank].pos.x - g_aRankOBJ[nCntRank2].size.x * 0.45f ,pFishes[nCntRank].pos.y + 60 ,pFishes[nCntRank].pos.z };
			g_aRankOBJ[nCntRank2].bDisp = true;

			for (nCntRank2 = 0, nCntRank3 = 0; nCntRank2 < MAX_RANKINGOBJ; nCntRank2++)
			{
				if (g_aRankOBJ[nCntRank2].nTexType == TEXTURE_PIN_CPU && g_aRankOBJ[nCntRank2].NumIdx.y == nCpuCnt + 1)
				{
					break;
				}
			}
			g_aRankOBJ[nCntRank2].pos = { pFishes[nCntRank].pos.x + g_aRankOBJ[nCntRank2].size.x * 0.45f ,pFishes[nCntRank].pos.y + 60 ,pFishes[nCntRank].pos.z };
			g_aRankOBJ[nCntRank2].bDisp = true;

			nCpuCnt += 1;
		}
	}

	if (TimeCnt == 120)
	{
		// サウンドの再生
		PlaySound(SOUND_SE_DRUMROLL);
	}
	if (TimeCnt > 120 && TimeCnt < 300)
	{
		// カメラを横に流していく
		N += 300 / 180.0f;
		SetCameraPos(0, { (-50.0f * (MAX_RANK / 2)) + N,40.0f ,-130.0f }, { (-50.0f * (MAX_RANK / 2)) + N,40.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
	}
	if (TimeCnt == 300)
	{
		SetCameraPos(0, { 0.0f ,65.0f ,-0.0f }, { 0.0f,35.0f,60.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);

	}
	if (TimeCnt == 420)
	{
		// サウンドの再生
		PlaySound(SOUND_SE_JAN);

		// １位を中心に移動、その他を周りに倒す
		float y = 0, rot = 0;

		for (int nCntRank = 0, nRank = 0, nCpuCnt = 0; nCntRank < MAX_RANK; nCntRank++)
		{
			if (g_aRank_Info[nCntRank].nRank == 0)
			{
				pFishes[nCntRank].pos = { (-20.0f * (nRankNum[0][0] - 1)) + (nRank * 40.0f),30.0f,0.0f };

				nRank++;
			}
			else
			{
				y += 6.28f / (MAX_RANK - nRankNum[0][0]);
				rot = ((rand() % 618) - 314) / 100.0f;

				pFishes[nCntRank].move.x = 0.0f + sinf(y - D3DX_PI) * (70.0f * 1);
				pFishes[nCntRank].move.z = -20.0f + cosf(y - D3DX_PI) * (30.0f * 1);

				pFishes[nCntRank].pos = { pFishes[nCntRank].move.x, -15.0f,pFishes[nCntRank].move.z };
				pFishes[nCntRank].rot.y = rot;

				SetMotionFishes(nCntRank, MOTIONTYPE_DEATH, pFishes[nCntRank].bBlendMotion, pFishes[nCntRank].nFrameBlend);

			}
			if (g_aRank_Info[nCntRank].nCharaIdx == RESULT_PLAYER_COMPUTER)
			{
				nCpuCnt += 2;
			}
		}
		SetCameraPos(0, { 0.0f ,74.0f ,0.0f }, { 0.0f,45.0f,100.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
	}
	if (TimeCnt == 510)
	{
		// サウンドの再生
		PlaySound(SOUND_BGM_RANKING);

		for (int nCntRank = 0; nCntRank < MAX_RANK; nCntRank++)
		{
			if (g_aRank_Info[nCntRank].nRank == 0)
			{
				SetMotionFishes(nCntRank, MOTIONTYPE_WIN1, pFishes[nCntRank].bBlendMotion, pFishes[nCntRank].nFrameBlend);
			}
		}
		N = 0;
	}
	if (TimeCnt >= 510)
	{
		r = ((rand() % 2) * 1.0f), g = ((rand() % 2) * 1.0f), b = ((rand() % 2) * 1.0f);

		if (r == 0 && g == 0 && b == 0)
		{
			r = 1.0f;
			g = 1.0f;
			b = 1.0f;
		}

		SetConfetti(300,/*pos*/{ float(rand() % 600 - 300),200.0f,float(rand() % 300 - 150) },/*rot*/{ (rand() % 628 - 314) / 100.0f, (rand() % 628 - 314) / 100.0f, (rand() % 628 - 314) / 100.0f }, D3DXVECTOR3(0.0f, -1.0f, 0.0f), (rand() % 150 + 50) / 100.0f, (rand() % 300 + 150) / 100.0f, 0.00f, D3DXCOLOR(r, g, b, 1.0f));
	}
	if (TimeCnt >= 600)
	{
		if (N < 40)
		{
			N ++;
			SetCameraPos(0, { 0.0f + (N) ,85.0f ,-200.0f }, { 0.0f + (N),55.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
		}
	}
	if (TimeCnt >= 640)
	{
		SetCameraPos(0, { 0.0f + (N) ,85.0f ,-200.0f }, { 0.0f + (N),55.0f,0.0f }, { 0.0f,0.0f,0.0f }, CAMERATYPE_STOP);
	}
	if (TimeCnt == 700)
	{
		for (int nCntRank = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
		{ // テクスチャ位置を動かす

			if (!g_aRankOBJ[nCntRank].bUse || g_aRankOBJ[nCntRank].nObjType != RANKTYPE_2D)
			{ // 使用されてない // 2Dでない
				continue;
			}
			if (g_aRankOBJ[nCntRank].nTexType != TEXTURE_PIN_CPU &&
				g_aRankOBJ[nCntRank].nTexType != TEXTURE_PIN_P1 &&
				g_aRankOBJ[nCntRank].nTexType != TEXTURE_PIN_P2 &&
				g_aRankOBJ[nCntRank].nTexType != TEXTURE_RANKICON &&
				g_aRankOBJ[nCntRank].nTexType != TEXTURE_RANKSCORE &&
				g_aRankOBJ[nCntRank].nTexType != TEXTURE_RANKSCOREFLAME &&
				g_aRankOBJ[nCntRank].nTexType != TEXTURE_SCORENUM)
			{ // テクスチャタイプが違ったら

				continue;
			}

			g_aRankOBJ[nCntRank].bDisp = true;
		}
	}
	if (TimeCnt >= 700)
	{
		// 一定時間経過ORキー入力
		if ((GetKeyboardTrigger(DIK_RETURN) == true ||
			GetJoypadTrigger(0, JOYKEY_START) == true ||
			GetJoypadTrigger(0, JOYKEY_A) == true)
			&& pFade == FADE_NONE)
		{// 決定キー（ENTERキー）が押された

			SetFade(MODE_LOGO);
			return;
		}

#ifdef ENABLE_ONELAP
		if (GetFade() == FADE_NONE)
		{// フェード終了

			SetFade(MODE_LOGO);
			return;
		}
#endif
	}
}

//=======================================
// ランキングの描画処理
//=======================================
void DrawRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ
	D3DXMATRIX mtxWorld;		// 計算用マトリックス
	D3DXMATRIX mtxTrans;		// 計算用マトリックス
	D3DXMATRIX mtxView;			// ビューマトリックス取得用

	// デバイスの取得
	pDevice = GetDevice();

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffRank2D, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntRank = 0, nWrite = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{  // 2Dオブジェクトの処理

		if (!g_aRankOBJ[nCntRank].bUse || g_aRankOBJ[nCntRank].nObjType != RANKTYPE_2D)
		{
			continue;
		}
		if (g_aRankOBJ[nCntRank].nTexType == TEXTURE_RANKSCORE || g_aRankOBJ[nCntRank].nTexType == TEXTURE_RANKSCOREFLAME)
		{
			// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

			if (g_aRankOBJ[nCntRank].nTexType == TEXTURE_RANKSCORE)
			{
				// 加算合成
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
			}
		}

		if (g_aRankOBJ[nCntRank].bDisp == true)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureRank[g_aRankOBJ[nCntRank].nTexType]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nWrite * MAX_VERTEX,	// 描画する最初の頂点インデックス
				MAX_POLYGON);	// 描画するプリミティブ数
		}
		nWrite++;

		if (g_aRankOBJ[nCntRank].nTexType == TEXTURE_RANKSCORE || g_aRankOBJ[nCntRank].nTexType == TEXTURE_RANKSCOREFLAME)
		{
			// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

			// 元に戻す
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}

		if (nWrite >= g_nObjNum[RANKTYPE_2D])
		{
			break;
		}
	}

	DrawFishes();

	// ライティングを無効化（影の影響を受けない）
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファブレンドを有効に設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ( , 比較方法(より大きい))
	pDevice->SetRenderState(D3DRS_ALPHAREF, 100);				// ( , 基準値)

		// ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&mtxWorld, NULL, &mtxView);	// 逆行列を求める
	mtxWorld._41 = 0.0f;
	mtxWorld._42 = 0.0f;
	mtxWorld._43 = 0.0f;

		// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffRank3D, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	for (int nCntRank = 0, nWrite = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{ // 3Dオブジェクトの処理

		if (!g_aRankOBJ[nCntRank].bUse || g_aRankOBJ[nCntRank].nObjType != RANKTYPE_3D)
		{
			continue;
		}

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_aRankOBJ[nCntRank].pos.x, g_aRankOBJ[nCntRank].pos.y, g_aRankOBJ[nCntRank].pos.z);
		D3DXMatrixMultiply(&g_aRankOBJ[nCntRank].mtxWorld, &mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aRankOBJ[nCntRank].mtxWorld);


		if (g_aRankOBJ[nCntRank].bDisp == true)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureRank[g_aRankOBJ[nCntRank].nTexType]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nWrite * MAX_VERTEX,		// 描画する最初の頂点インデックス
				MAX_POLYGON);				// 描画するプリミティブ数

		}
		nWrite++;

		if (nWrite >= g_nObjNum[RANKTYPE_3D])
		{
			break;
		}
	}
	//// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);			// アルファブレンドを無効に設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);		// ( , 比較方法(すべて))
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);						// ( , 基準値)

	// 紙吹雪の描画処理
	DrawConfetti();

	// 元に戻す
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//=======================================
// ランキングOBjの設定
//=======================================
bool SetRankingObj(D3DXVECTOR3 pos,		// 位置
	D3DXCOLOR col,						// 色
	D3DXVECTOR2 size,					// サイズ
	int nObjType,						// 2Dor3D
	int nTexType,						// テクスチャタイプ
	D3DXVECTOR2 NumIdx,					// 数字系テクスチャのインデックス[数字じゃないなら-1]
	bool bDisp)							// 最初から表示するか
{
	// ローカル変数宣言 -----------------

	//VERTEX_2D* pVtx2D;							// 頂点情報へのポインタ
	//VERTEX_3D* pVtx3D;							// 頂点情報へのポインタ
	//LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	//Fishes* pFishes = GetFishes();	for (int nCntRank = 0; nCntRank < MAX_RANKINGOBJ; nCntRank++)
	{
		if (!g_aRankOBJ[nCntRank].bUse)
		{ // 使用されていない

			if (nObjType == RANKTYPE_2D)
			{ // 2Dオブジェクトにする

				g_aRankOBJ[nCntRank].pos = pos;
				g_aRankOBJ[nCntRank].col = col;
				g_aRankOBJ[nCntRank].size = size;
				g_aRankOBJ[nCntRank].nObjType = nObjType;
				g_aRankOBJ[nCntRank].nTexType = nTexType;
				g_aRankOBJ[nCntRank].NumIdx = NumIdx;
				g_aRankOBJ[nCntRank].bDisp = bDisp;
				g_aRankOBJ[nCntRank].bUse = true;
				g_aRankOBJ[nCntRank].VtxIdx = g_nObjNum[RANKTYPE_2D];

				g_nObjNum[RANKTYPE_2D]++;
				return true;
			}
			else if (nObjType == RANKTYPE_3D)
			{ // 3Dオブジェクトにする

				g_aRankOBJ[nCntRank].pos = pos;
				g_aRankOBJ[nCntRank].col = col;
				g_aRankOBJ[nCntRank].size = size;
				g_aRankOBJ[nCntRank].nObjType = nObjType;
				g_aRankOBJ[nCntRank].nTexType = nTexType;
				g_aRankOBJ[nCntRank].NumIdx = NumIdx;
				g_aRankOBJ[nCntRank].bDisp = bDisp;
				g_aRankOBJ[nCntRank].bUse = true;
				g_aRankOBJ[nCntRank].VtxIdx = g_nObjNum[RANKTYPE_3D];

				g_nObjNum[RANKTYPE_3D]++;
				return true;
			}
		}
	}
	return false;
}

//=======================================
// スコア表示
//=======================================
void Score_Print(D3DXVECTOR3 pos,int nType, int nCnt,D3DXCOLOR col)
{
	// ローカル変数宣言 -----------------

	int nDig = 4;
	int nDiv = 1000;
	int nCntScore = 0;

	for (; nDiv > 1; nDiv /= 10, nDig--)
	{
		if (g_aRank_Info[nCnt].nScore / nDiv != 0)
		{
			break;
		}
	}

	for (nCntScore = 0, nDiv = 1; nCntScore < nDig; nCntScore++)
	{
		SetRankingObj(/*pos*/{ pos.x - nCntScore * 40.0f,pos.y  ,0.0f }, col,/*size*/{ 50.0f,65.0f }, nType, TEXTURE_SCORENUM, { float((g_aRank_Info[nCnt].nScore % (nDiv * 10)) / nDiv),0 }, false);

		nDiv *= 10;
	}

	SetRankingObj(/*pos*/{ pos.x + 40.0f,pos.y +15.0f ,0.0f }, col,/*size*/{ 50.0f,65.0f }, nType, TEXTURE_SCORENUM, { 10,0 }, false);
}