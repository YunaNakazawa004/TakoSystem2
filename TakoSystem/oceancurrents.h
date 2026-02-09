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

// 海流UIの種類
typedef enum
{
	OCUITYPE_NULL = -1,		// [-1]無し
	OCUITYPE_WANING = 0,	// [0]警告
	OCUITYPE_TIMER,			// [1]タイマー
	
	OCUITYPE_MAX			// 最大値

}OCUITYPE;

// プロトタイプ宣言 ============================================

void InitOceanCurrents(void);				// 海流の初期化処理
void UninitOceanCurrents(void);				// 海流の終了処理
void UpdateOceanCurrents(void);				// 海流の更新処理
void DrawOceanCurrents(void);				// 海流の描画処理

int SetOceanCurrentsUi						// 海流のUI設定処理
(OCUITYPE type,									// 種類
 D3DXVECTOR3 pos, float fSizeMag);				// 位置, 倍率

int SetOceanCurrentsPolygon					// 海流のポリゴン設定処理		
(int nIdxTex, bool bAlphaBlend,										// 種類, aブレンドをするか
 D3DXVECTOR3 pos, D3DXVECTOR3 rot,									// 位置, 角度
 float fWidth, float fHeight,										// 大きさ(幅, 高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 addtexPos, D3DXVECTOR2 texSize,	// テクスチャ(座標, 座標加算量, サイズ)
 D3DXCOLOR col);													// 色					

void DelOceanCurrentsUi						// 海流UIの削除処理
(int nIdxOCUi);									// インテックス

void UpdateOceanCurrentsState(void);					// 海流の状態更新処理

void MoveOceanCurrents						// 中心を軸にした海流の移動処理
(D3DXVECTOR3* pPos);

OCEANCURRENTSSTATE GetOceanCurrents(void);	// 現在の海流の状態を返す

#endif
