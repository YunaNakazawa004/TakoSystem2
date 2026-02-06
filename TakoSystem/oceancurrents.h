//==================================================================================
// 
// 海流に関する処理[oceancurrents.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _OCEANCURRENTS_H_
#define _OCEANCURRENTS_H_

// 列挙型の定義 ================================================

// 海流の状態
typedef enum
{
	OCEANCURRENTSSTATE_NOMAL = 0,	// [0]通常状態
	OCEANCURRENTSSTATE_WAIT,		// [1]渦潮待機状態
	OCEANCURRENTSSTATE_WIRLPOOL,	// [2]渦潮状態
	
	OCEANCURRENTSSTATE_MAX			// 最大値

}OCEANCURRENTSSTATE;

// プロトタイプ宣言 ============================================

void InitOceanCurrents(void);				// 海流の初期化処理
void UninitOceanCurrents(void);				// 海流の終了処理
void UpdateOceanCurrents(void);				// 海流の更新処理
void DrawOceanCurrents(void);				// 海流の描画処理

int SetOceanCurrents						// 海流の設定処理		
(int nIdxTex,									// 種類
 D3DXVECTOR3 pos, D3DXVECTOR3 rot,				// 位置, 角度
 float fWidth, float fHeight);					// 大きさ(幅, 高さ)

void MoveOceanCurrents						// 中心を軸にした海流の移動処理
(D3DXVECTOR3* pPos);

void SetWarningOceanCurrents				// 海流の警告処理
(D3DXVECTOR3 pos, int nWaitTime);

OCEANCURRENTSSTATE GetOceanCurrents(void);	// 現在の海流の状態を返す

#endif
