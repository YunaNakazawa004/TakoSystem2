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
// 列挙型の定義
//=====================================

// テクスチャ一覧 ---------------------

typedef enum
{
	TEXTURE_BG = 0,				// 背景
	TEXTURE_BG_OCTO,			// 背景タコ
	TEXTURE_PIN_P1,				// player1pin
	TEXTURE_PIN_P2,				// player2pin
	TEXTURE_PIN_CPU,			// computerpin
	TEXTURE_RANKSCORE,			// ランキング表示用
	TEXTURE_RANKSCOREFLAME,		// ランキング表示用
	TEXTURE_SCORENUM,			// スコア表示用
	TEXTURE_RANKICON,			// 金の王冠
	TEXTURE_MAX,

}RANKING_TEXTURE_LABEL;

//=====================================
// 構造体の定義
//=====================================

// ランキングOBJ ----------------------

typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXCOLOR col;				// 色
	D3DXVECTOR2 size;			// サイズ
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	bool bUse;					// 使用しているか
	bool bDisp;					// 表示するか
	int nObjType;				// Objタイプ
	int nTexType;				// Texタイプ
	D3DXVECTOR2 NumIdx;			// 数字系テクスチャのインデックス
} RankingOBJ;

// ランキング情報 ---------------------

typedef struct
{
	int nCharaIdx;				// P1,P2かCPUか
	int nScore;					// スコア
	int nRank;					// 順位

}Ranking_Info;

// プロトタイプ宣言
void InitRanking(void);
void UninitRanking(void);
void UpdateRanking(void);
void DrawRanking(void);
void SetRankingObj(D3DXVECTOR3 pos,		// 位置
	D3DXCOLOR col,						// 色
	D3DXVECTOR2 size,					// サイズ
	int nObjType,						// 2Dor3D
	int nTexType,						// テクスチャタイプ
	D3DXVECTOR2 NumIdx,					// 数字系テクスチャのインデックス[数字じゃないなら-1]
	bool bDisp);						// 最初から表示するか
void Score_Print(D3DXVECTOR3 pos, int nType, int nCnt, D3DXCOLOR col);

#endif