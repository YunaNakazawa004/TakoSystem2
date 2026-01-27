//=============================================================================
// 
// ランキングヘッダー [ranking.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _RANKING_H_		
#define _RANKING_H_		// 2重インクルード防止のマクロを定義

#include "main.h"

// マクロ定義
#define MAX_RANK	(5)	// ランク数

// ランキング構造体
typedef struct
{
	D3DXVECTOR3 pos;	// 位置
	int nScore;			// スコア
} Ranking;

// プロトタイプ宣言
void InitRanking(void);
void UninitRanking(void);
void UpdateRanking(void);
void DrawRanking(void);
//void ResetRanking(void);
//void SetRanking(int nScore);

#endif