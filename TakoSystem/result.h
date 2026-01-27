//=============================================================================
// 
// リザルトヘッダー [result.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _RESULT_H_		
#define _RESULT_H_		// 2重インクルード防止のマクロを定義

// リザルト構造体の定義
typedef struct
{
	int nAddScore;	// ゲームで得たスコア
	int nAddLife;	// ライフをスコアに追加
	int nAddTime;	// タイムをスコアに追加
}Result;

// プロトタイプ宣言
void InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);
void SetResult(int nScore, int nLife, int nTime);

Result* GetResult(void);

#endif