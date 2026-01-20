#ifndef _TIME_H_		
#define _TIME_H_		// 2重インクルード防止のマクロを定義

#include "time.h"

// マクロ定義
#define DEFAULT_TIME	(120)	// ゲームの制限時間
#define ADD_TIME		(30)	// ステージ通過の追加時間

#define PINCH_TIME	(15)	// 制限時間がピンチ

// プロトタイプ宣言
void InitTime(void);
void UninitTime(void);
void UpdateTime(void);
void DrawTime(void);
void SetTime(int nTime);
void AddTime(int nValue);

int GetTime(void);

#endif