//=============================================================================
// 
// ゲームヘッダー [game.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _GAME_H_		
#define _GAME_H_		// 2重インクルード防止のマクロを定義

// ゲームの状態
typedef enum
{
	GAMESTATE_BEGIN = 0,	// [0]開始状態
	GAMESTATE_NONE,			// [1]開始状態
	GAMESTATE_TIMEOVER,		// [2]時間切れ状態
	GAMESTATE_END,			// [3]終了状態

	GAMESTATE_MAX,			// 状態の最大

}GAMESTATE;

// プロトタイプ宣言
void InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);
void SetEnablePause(bool bPause);
bool GetPause(void);

#endif