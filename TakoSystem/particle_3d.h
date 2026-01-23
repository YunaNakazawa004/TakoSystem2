//========================================================================
// 
// 3Dパーティクル処理[paticle_3d.h]
// Author:稲村 蒼稀
// 
//========================================================================

#ifndef _PATICLE_3D_H_
#define _PATICLE_3D_H_

// プロトタイプ宣言 ============================================

void InitParticle3D(void);		// 3Dパーティクルの初期化処理
void UninitParticle3D(void);	// 3Dパーティクルの終了処理
void UpdateParticle3D(void);	// 3Dパーティクルの更新処理
void DrawParticle3D(void);		// 3Dパーティクルの描画処理

int SetParticle3D				// 3Dパーティクルの設定処理
(int nValue, int nLifeP,  	            // パーティクル(生成量, 寿命, 色) 
 D3DXVECTOR3 posP, D3DXCOLOR col,	    // パーティクル(位置, 移動方向, 移動速度)
 float fSpeedE, int nLifeE, 			// エフェクト(移動速度, 寿命)
 float fRadiusE, float faddRadiusE); 	// エフェクト(大きさ, 大きさの加算量)
 									 
void SetPosionParticle3D		// 3Dパーティクルの位置設定処理
(int nIdx,                              // インデックス, 
 const char *pMode, D3DXVECTOR3 pos);   // posの使用形式(put:代入, add:加算), 位置 

#endif