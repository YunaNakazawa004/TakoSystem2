//=============================================================================
// 
// タイムヘッダー [time.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _TIME_H_		
#define _TIME_H_		// 2重インクルード防止のマクロを定義

#include "time.h"

// マクロ定義
#define DEFAULT_TIME	(150)	// ゲームの制限時間

#define PINCH_TIME	(20)		// 制限時間がピンチ

// プロトタイプ宣言
void InitTime(void);
void UninitTime(void);
void UpdateTime(void);
void DrawTime(void);
void SetTime(int nTime);
void AddTime(int nValue);

int GetTime(void);

#endif