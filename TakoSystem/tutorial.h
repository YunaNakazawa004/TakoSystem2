//=============================================================================
// 
// チュートリアルヘッダー [tutorial.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _TUTORIAL_H_		
#define _TUTORIAL_H_		// 2重インクルード防止のマクロを定義

#include "player.h"

// プロトタイプ宣言
void InitTutorial(void);
void UninitTutorial(void);
void UpdateTutorial(void);
void DrawTutorial(void);

Player_Tutorial GetPlayerTutorial(int nIdx);

#endif