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
// プロトタイプ宣言
//*****************************************************************************
void InitTutorialTxt(void);
void UninitTutorialTxt(void);
void UpdateTutorialTxt(void);
void DrawTutorialTxt(void);
void SetTutorialTxt(D3DXVECTOR3 pos);

#endif