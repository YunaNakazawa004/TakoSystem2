//==================================================================================
// 
// UIゲージアイコンに関する処理[ui_gaugeicon.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _UI_GAUGEICON_H_
#define _UI_GAUGEICON_H_

#include "input.h"

// 列挙型の定義 ================================================

// UIゲージアイコンの状態の種類
typedef enum
{
	UIGAUGEICON_STATE_FULL = 0,		// [0]ゲージ満タンの状態
	UIGAUGEICON_STATE_RECAST,		// [1]リキャスト中の状態
	UIGAUGEICON_STATE_ENDRECAST,	// [2]リキャストが終わった状態

	UIGAUGEICON_STATE_MAX			// 最大値

}UIGAUGEICON_STATE;

// プロトタイプ宣言 ============================================

void InitUiGaugeIcon(void);		// UIゲージアイコンの初期化処理
void UninitUiGaugeIcon(void);	// UIゲージアイコンの終了処理
void UpdateUiGaugeIcon(void);	// UIゲージアイコンの更新処理
void DrawUiGaugeIcon(void);		// UIゲージアイコンの描画処理

void SetUiGaugeIcon				// UIゲージアイコンの設定処理
(int nIdxTexture,									// テクスチャインデックス,
 int nUser, JOYKEY input,							// プレイヤー, 入力キー
 D3DXVECTOR3 pos, D3DXCOLOR col,					// 位置, 色
 float fSizeWidth, float fSizeHeight,				// 大きさ(幅), 大きさ(高さ)
 UIGAUGEICON_STATE startState, int nCounterState);	// 開始状態, 継続時間

void UpdateStateUiGaugeIcon		// UIゲージアイコンの状態更新処理
(int nIdxUiGaugeIcon);

bool GetInputStateUiGaugeIcon	// UIゲージアイコンの入力判定処理
(int nUser,JOYKEY Input);

#endif
