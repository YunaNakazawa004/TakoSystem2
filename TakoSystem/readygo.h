//===========================================================================
// 
// レディゴーヘッダー[readygo.h]
// Author:Mitsuki Uemura
// 
//===========================================================================
#ifndef _READY_GO_H_
#define _READY_GO_H_

#include "main.h"

// プロトタイプ宣言
void InitReady(void);
void UninitReady(void);
void UpdateReady(void);
void DrawReady(void);
void SetReady(int aIdx, int Idx);
void ResetReady(void);
void SetReadyMove(int aIdx, D3DXVECTOR3 startPos, D3DXVECTOR3 EndPos, int Speed, bool bOnInertia);
bool MoveReady(int aIdxObj, int aIdxMove, bool bUse);
bool GetGameStart(void);

#endif
