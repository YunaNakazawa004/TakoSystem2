//========================================================================
// 
// 3Dパーティクル処理[paticle_3d.h]
// Author:稲村 蒼稀
// 
//========================================================================

#ifndef _PATICLE_3D_H_
#define _PATICLE_3D_H_

#include "effect_3d.h"		// 3Dエフェクトヘッダー

// 構造体の定義 ================================================

// 3Dパーティクルの構造体
typedef struct
{
	// パーティクルの情報
	D3DXVECTOR3 pos;				// 表示位置

	D3DXVECTOR3 move;				// 移動量の蓄積

	D3DXCOLOR col;					// 色

	float fSpeedPaticle;			// パーティクルの移動速度
	D3DXVECTOR3 fVecMoveParticle;	// パーティクルの移動方向

	int nParticleValue;				// パーティクルの生成量
	int nParticleLifeO;				// パーティクルの寿命(判定などに使う全体値)
	int nParticleLifeV;				// パーティクルの寿命(寿命として減少させる値)

	// エフェクトの情報
	float fSpeedEffect;				// エフェクトの移動速度

	float fEffectRadius;			// エフェクトの大きさ
	float faddEffectRadius;			// エフェクトの大きさの加算量

	int nEffectLife;				// エフェクトの寿命

	EFFECTTYPE effecttype;			// エフェクトの用途

	// 状態
	bool bUse;						// 使用状況

}Paticle3D;

// プロトタイプ宣言 ============================================

void InitParticle3D(void);		// 3Dパーティクルの初期化処理
void UninitParticle3D(void);	// 3Dパーティクルの終了処理
void UpdateParticle3D(void);	// 3Dパーティクルの更新処理
void DrawParticle3D(void);		// 3Dパーティクルの描画処理

int SetParticle3D				// 3Dパーティクルの設定処理
(int nValue, int nLifeP,                                // パーティクル(生成量, 寿命, 色) 
    D3DXVECTOR3 posP, D3DXCOLOR col, D3DXVECTOR3 vec,   // パーティクル(位置, 移動方向, 移動速度)
    float fSpeedE, int nLifeE,                          // エフェクト(移動速度, 寿命)
    float fRadiusE, float faddRadiusE,                  // エフェクト(大きさ, 大きさの加算量)
    EFFECTTYPE effecttype);                             // エフェクト(用途)

void SetPosionParticle3D		             // 3Dパーティクルの位置設定処理
(int nIdx,                                   // インデックス, 
    const char* pMode, D3DXVECTOR3 pos);     // posの使用形式(put:代入, add:加算), 位置 

Paticle3D *GetParticlePos(void);
#endif