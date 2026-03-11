//=============================================================================
// 
// チュートリアルテキストヘッダー [tutorialtxt.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _BILLBOARD_H_		// このマクロ定義がされていなかったら
#define _BILLBOARD_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// チュートリアルテキストの種類
//*****************************************************************************
typedef enum
{
	TUTTXTTYPE_MOVE = 0,		// 移動方法
	TUTTXTTYPE_CAMERA,			// カメラ
	TUTTXTTYPE_DOWN,			// 下降
	TUTTXTTYPE_UP,				// 上昇
	TUTTXTTYPE_TENT,			// 触手
	TUTTXTTYPE_INK,				// 墨吐き
	TUTTXTTYPE_ESA,				// エサの取り方
	TUTTXTTYPE_POT,				// タコつぼ
	TUTTXTTYPE_POOL,			// 渦潮
	TUTTXTTYPE_RULE,			// ルール
	TUTTXTTYPE_MAX
}TUTTXTTYPE;

//*****************************************************************************
// チュートリアルテキストの状態
//*****************************************************************************
typedef enum
{
	TUTTXTSTATE_NONE = 0,		// 表示しない状態
	TUTTXTSTATE_DISP,			// 表示してる状態
	TUTTXTSTATE_CLEAR,			// 達成した状態
	TUTTXTSTATE_FADE,			// 消えていく状態
	TUTTXTSTATE_MAX
}TUTTXTSTATE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitTutorialTxt(void);
void UninitTutorialTxt(void);
void UpdateTutorialTxt(void);
void DrawTutorialTxt(void);
void SetTutorialTxt(TUTTXTTYPE type, TUTTXTSTATE state, D3DXVECTOR3 pos);
void SetTutorialTxtState(TUTTXTTYPE type, TUTTXTSTATE state);
int GetTutorialTxtFromType(TUTTXTTYPE type);

#endif