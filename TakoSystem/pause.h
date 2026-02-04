//===========================================================================
//ポーズヘッダー[pause.h]
// Author:須藤英翔
// 
//===========================================================================
#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "main.h"

//ポーズメニューの状態
typedef enum
{
	PAUSE_MENU_CONTINUE = 0,	//ゲームに戻る
	PAUSE_MENU_RETRY,			//ゲームをやり直す
	PAUSE_MENU_QUIT,			//タイトルに戻る
	PAUSE_MENU_MAX
}PAUSE_MENU;

extern int g_nSelect;

// プロトタイプ宣言
void InitPause(void);
void UninitPause(void);
void UpdatePause(void);
void DrawPause(void);
void SetPause(PAUSE_MENU pause);
void ResetPause(void);

#endif
