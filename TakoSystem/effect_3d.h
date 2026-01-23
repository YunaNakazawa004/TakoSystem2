//========================================================================
// 
// 3Dエフェクトに関する処理[effect_3d.h]
// Author:稲村 蒼稀
// 
//========================================================================

#ifndef _EFFECT_3D_H_	
#define _EFFECT_3D_H_	

// プロトタイプ宣言 ============================================

void InitEffect3D(void);		// 3Dエフェクトの入力処理
void UninitEffect3D(void);		// 3Dエフェクトの終了処理
void UpdateEffect3D(void);		// 3Dエフェクトの更新処理
void DrawEffect3D(void);		// 3Dエフェクトの描画処理

void SetEffect3D				// 3Dエフェクトの設定処理
(int nLife,								// 寿命
 D3DXVECTOR3 pos,						// 表示位置
 D3DXVECTOR3 vecMove, float fSpeed,		// 移動方向, 移動速度
 float fRadius, float faddRadius,		// 表示サイズ, 加算サイズ
 D3DXCOLOR col);						// 色

#endif