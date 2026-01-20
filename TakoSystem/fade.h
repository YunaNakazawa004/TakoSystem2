//===========================================================================
//フェードヘッダー[fade.h]
// Author:須藤英翔
// 
//===========================================================================
#ifndef _FADE_H_
#define _FADE_H_

#include "main.h"
#include <cstdio>

// マクロ定義
#define BLK_VISIBLE_VTX			(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f))		// 黒いポリゴン(不透明)
#define BLK_INVISIBLE_VTX		(D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f))		// 黒いポリゴン(透明)
#define FADE_SPEED				(0.03f)									// フェードの速さ

//フェードの状態
typedef enum
{
	FADE_NONE = 0,
	FADE_IN,
	FADE_OUT,
	FADE_MAX
}FADE;


// プロトタイプ宣言
void InitFade(MODE modeNext);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);
void SetFade(MODE modeNext);
FADE GetFade(void);
#endif
