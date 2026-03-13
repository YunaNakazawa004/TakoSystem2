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
void SetReady(int nArrayIdx, int Idx,int nDelay);
void ResetReady(void);
void SetReadyMove(int nArrayIdx, D3DXVECTOR3 startPos, D3DXVECTOR3 EndPos, int Speed, bool bOnInertia);
bool MoveReady(int nArrayIdxObj, int nArrayIdxMove, bool bUse);
bool GetGameStart(void);

#endif
