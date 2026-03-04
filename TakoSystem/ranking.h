//=============================================================================
// 
// ランキングヘッダー [ranking.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _RANKING_H_		
#define _RANKING_H_		// 2重インクルード防止のマクロを定義

#include "main.h"

//=====================================
// 構造体の定義
//=====================================

// ランキングOBJ ----------------------

typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXCOLOR col;				// 色
	D3DXVECTOR3 size;			// サイズ
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	bool bUse;					// 使用しているか
	int nType;					// タイプ
	int TexIdx;					// 使用する画像のインデックス
	int NumIdx;					// 数字系テクスチャのインデックス
} RankingOBJ;

// ランキング情報 ---------------------

typedef struct
{
	int nCharaIdx;				// P1,P2かCPUか
	int nScore;					// スコア
	int nRank;					// 順位
	int nNumFirstRank;			// 一位の人数

}Ranking_Info;

// プロトタイプ宣言
void InitRanking(void);
void UninitRanking(void);
void UpdateRanking(void);
void DrawRanking(void);
//void ResetRanking(void);
//void SetRanking(int nScore);

#endif