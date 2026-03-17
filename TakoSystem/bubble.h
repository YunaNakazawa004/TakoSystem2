//==================================================================================
// 
// 泡に関する処理[bubble.h]
// Author : Soki Inamura
// 
//==================================================================================

#ifndef _BUBBLE_H_	
#define _BUBBLE_H_	

// プロトタイプ宣言 ============================================

void InitBubble(void);		// 泡の入力処理
void UninitBubble(void);	// 泡の終了処理
void UpdateBubble(void);	// 泡の更新処理
void DrawBubble(void);		// 泡の描画処理

int SetBubbleParticle		// 泡パーティクルの設定処理
(D3DXVECTOR3* pPos, bool bRefPointer, int nLife, int nWait, int nValue, int nSetLife, float fSetSizeMax, float fSetSizeMin);

void SetBubblePolygon		// 泡ポリゴンの設定処理
(D3DXVECTOR3 pos, float fSizeRadius, int nLife);

void DelBubbleParticle		// 泡パーティクルの削除処理
(int nIdxParticle);

#endif

