//=============================================================================
// 
// クロスヘアヘッダー [crosshair.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _CROSSHAIR_H_		// このマクロ定義がされていなかったら
#define _CROSSHAIR_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// クロスヘアの状態
//*****************************************************************************
typedef enum
{
	CROSSHAIRSTATE_NONE = 0,			// 届かない状態	
	CROSSHAIRSTATE_REACH,				// 届く状態
	CROSSHAIRSTATE_MAX
}CROSSHAIRSTATE;

//*****************************************************************************
// クロスヘア構造体を定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	CROSSHAIRSTATE state;			// 状態
	int nCounterState;				// 状態管理カウンター
	bool bDisp;						// 表示するかどうか
}CrossHair;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCrossHair(void);
void UninitCrossHair(void);
void UpdateCrossHair(void);
void DrawCrossHair(void);
void SetCrossHair(int nIdx, CROSSHAIRSTATE state);
CrossHair* GetCrossHair(void);

#endif