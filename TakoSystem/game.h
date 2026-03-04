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
	GAMESTATE_LITTLETIME,	// [2]残り時間が少ない状態
	GAMESTATE_TIMEOVER,		// [3]時間切れ状態
	GAMESTATE_END,			// [4]終了状態

	GAMESTATE_MAX,			// 状態の最大

}GAMESTATE;

// プロトタイプ宣言
void InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);
void SetEnablePause(bool bPause);
bool GetPause(void);

void SetGameState(GAMESTATE setState);	// ゲームの状態を設定する処理
GAMESTATE GetGameState(void);			// ゲームの状態を返す処理

void GiveResultForGame(void);	// リザルトに情報を渡す処理

#endif