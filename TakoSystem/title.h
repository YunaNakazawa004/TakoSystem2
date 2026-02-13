//=============================================================================
// 
// タイトルヘッダー [title.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _TITLE_H_		
#define _TITLE_H_		// 2重インクルード防止のマクロを定義

// タイトルカーソルの種類
typedef enum
{
	TITLECURSOR_PLAYER_SELECT = 0,		// 人数変更
	TITLECURSOR_PLAY_START,				// ゲーム開始
	TITLECURSOR_MAX
}TITLECURSOR;

// プロトタイプ宣言
void InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);

int GetPlayerSelect(void);

#endif