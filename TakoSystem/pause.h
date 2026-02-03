//===========================================================================
//ポーズヘッダー[pause.h]
// Author:須藤英翔
// 
//===========================================================================
#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "main.h"

#define MAX_PAUSE				(3)										// ポーズメニューの最大数
#define REPEAT_MAX				(40)									// リピートまでの最大フレーム数
#define REPEAT_COUNT			(10)									// リピート間隔
#define GRAY_VTX				(D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f))		// 頂点カラーが灰

//ポーズメニューの状態
typedef enum
{
	PAUSE_MENU_CONTINUE = 0,		//ゲームに戻る
	PAUSE_MENU_RETRY,				//ゲームをやり直す
	PAUSE_MENU_QUIT,				//タイトルに戻る
	PAUSE_MENU_MAX
}PAUSE_MENU;

extern int g_nSelect;

// プロトタイプ宣言
void InitPause(void);
void UninitPause(void);
void UpdatePause(void);
void DrawPause(void);

#endif
