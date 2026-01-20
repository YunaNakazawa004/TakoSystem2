//=============================================================================
// 
// デバッグ表示ヘッダー [debugproc.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _DEBUGPROC_H_		// このマクロ定義がされていなかったら
#define _DEBUGPROC_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitDebugProc(void);
void UninitDebugProc(void);
void UpdateDebugProc(void);
void DrawDebugProc(void);

void PrintDebugProc(const char* fmt, ...);

#endif