//========================================================================
// 
// 紙吹雪処理[confetti.h]
// Author : Mutsuki Uemura
// 
//========================================================================

#ifndef _CONFETTI_H_
#define _CONFETTI_H_

#include "effect_3d.h"		// 3Dエフェクトヘッダー

// プロトタイプ宣言 ============================================

void InitConfetti(void);		// 紙吹雪の初期化処理
void UninitConfetti(void);	// 紙吹雪の終了処理
void UpdateConfetti(void);	// 紙吹雪の更新処理
void DrawConfetti(void);		// 紙吹雪の描画処理

void SetConfetti(int nLife,				// 寿命
	D3DXVECTOR3 pos,					// 表示位置
	D3DXVECTOR3 rot,					// 角度
	D3DXVECTOR3 vecMove, float fSpeed,	// 移動方向, 移動速度
	float fRadius, float faddRadius,	// 表示サイズ, 加算サイズ
	D3DXCOLOR col);						// 色
                             
void SetPosionConfetti		                 // 紙吹雪の位置設定処理
(int nIdx,                                   // インデックス, 
    const char* pMode, D3DXVECTOR3 pos);     // posの使用形式(put:代入, add:加算), 位置 

#endif