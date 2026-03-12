//=============================================================================
// 
// エサ上限ヘッダー [foodnum.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _FOODNUM_H_		// このマクロ定義がされていなかったら
#define _FOODNUM_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// エサ上限の種類
//*****************************************************************************
typedef enum
{
	FOODNUMTYPE_UP = 0,			// 上限増えた
	FOODNUMTYPE_FULL,			// 上限になった
	FOODNUMTYPE_MAX
}FOODNUMTYPE;

//*****************************************************************************
// エサ上限の状態
//*****************************************************************************
typedef enum
{
	FOODNUMSTATE_NONE = 0,		// 表示しない状態
	FOODNUMSTATE_DISP,			// 表示してる状態
	FOODNUMSTATE_MAX
}FOODNUMSTATE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitFoodNum(void);
void UninitFoodNum(void);
void UpdateFoodNum(void);
void DrawFoodNum(void);
int SetFoodNum(FOODNUMTYPE type, FOODNUMSTATE state, D3DXVECTOR3 pos);
void SetFoodNumState(int nIdx, FOODNUMSTATE state);

#endif