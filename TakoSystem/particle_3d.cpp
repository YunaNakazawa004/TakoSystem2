//========================================================================
// 
// 3Dパーティクル処理[paticle_3d.cpp]
// Author:稲村 蒼稀
// 
//========================================================================
#include "main.h"

#include "effect_3d.h"		// 3Dエフェクトヘッダー
#include "particle_3d.h"	// 3Dパーティクルヘッダー

#include "input.h"

// マクロ定義 ==================================================

#define MAX_SET_PARTCL3D		(128)	// パーティクルの最大数

// 構造体の定義 ================================================

// 3Dパーティクルの構造体
typedef struct
{
	// パーティクルの情報
	D3DXVECTOR3 pos;		// 表示位置
	
	D3DXCOLOR col;			// 色
	
	int nParticleValue;		// パーティクルの生成量
	int nParticleLife;		// パーティクルの寿命
	
	// エフェクトの情報
	float fSpeedEffect;		// エフェクトの移動速度
	
	float fEffectRadius;	// エフェクトの大きさ
	float faddEffectRadius;	// エフェクトの大きさの加算量

	int nEffectLife;		// エフェクトの寿命

	// 状態
	bool bUse;				// 使用状況

}Paticle3D;

// グローバル宣言 ==============================================

Paticle3D g_aPaticle3D[MAX_SET_PARTCL3D];		// 3Dパーティクルの情報

//======================================================================== 
// 3Dパーティクルの初期化処理
//========================================================================
void InitParticle3D(void)
{
	for (int nCntPaticle = 0; nCntPaticle < MAX_SET_PARTCL3D; nCntPaticle++)
	{// 用意した分繰り返す

		// パーティクル
		g_aPaticle3D[nCntPaticle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		
		g_aPaticle3D[nCntPaticle].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色を初期値に設定
		
		g_aPaticle3D[nCntPaticle].nParticleValue = 0;						// パーティクルの生成量を初期化
		g_aPaticle3D[nCntPaticle].nParticleLife = 0;						// パーティクルの寿命を初期化
		
		// エフェクト
		g_aPaticle3D[nCntPaticle].fSpeedEffect = 0.0f;						// エフェクトの移動速度を初期化
		
		g_aPaticle3D[nCntPaticle].fEffectRadius = 0.0f;						// エフェクトの大きさを初期化
		g_aPaticle3D[nCntPaticle].faddEffectRadius = 0.0f;					// エフェクトの大きさの加算量を初期化
		
		g_aPaticle3D[nCntPaticle].nEffectLife = 0;							// エフェクトの寿命を初期化
		
		// 状態
		g_aPaticle3D[nCntPaticle].bUse = false;								// 使用していない状況に設定
	}

}

//======================================================================== 
// 3Dパーティクルの終了処理
//========================================================================
void UninitParticle3D(void)
{
	
}

//======================================================================== 
// 3Dパーティクルの更新処理
//========================================================================
void UpdateParticle3D(void)
{
	// 変数宣言 ===========================================

	float rotXY = 0.0f, rotZ = 0.0f;	// エフェクトの移動角度
		  			
	// ====================================================

	for (int nCntPaticle = 0; nCntPaticle < MAX_SET_PARTCL3D; nCntPaticle++)
	{// 用意した分繰り返す

		if (g_aPaticle3D[nCntPaticle].bUse == true)
		{// 使用している場合
		
			// パーティクルの生成
			for (int nCntAppear = 0; nCntAppear < g_aPaticle3D[nCntPaticle].nParticleValue; nCntAppear++)
			{// 生成するだけ繰り返す

				rotXY = (float)(rand() % 629 - 314) / 100.0f;		// 角度を設定
				rotZ = (float)(rand() % 629 - 314) / 100.0f;		// 角度を設定
				
				// 3Dエフェクトの設定
				SetEffect3D(g_aPaticle3D[nCntPaticle].nEffectLife,				// 寿命
							g_aPaticle3D[nCntPaticle].pos,						// 位置
							D3DXVECTOR3(sinf(rotXY),cosf(rotXY),cosf(rotZ)),	// 移動方向
							g_aPaticle3D[nCntPaticle].fSpeedEffect,				// 移動速度
							g_aPaticle3D[nCntPaticle].fEffectRadius,			// 大きさ
							g_aPaticle3D[nCntPaticle].faddEffectRadius,			// 大きさの加算量
							g_aPaticle3D[nCntPaticle].col);						// エフェクトの色					
			}

			g_aPaticle3D[nCntPaticle].nParticleLife--;	// 寿命を減らす
		}

		if (g_aPaticle3D[nCntPaticle].nParticleLife <= 0)
		{// 寿命が尽きた

			g_aPaticle3D[nCntPaticle].bUse = false;		// 使用していない状態に設定
		}
	}
}

//======================================================================== 
// 3Dパーティクルの描画処理
//========================================================================
void DrawParticle3D(void)
{

}

//======================================================================== 
// 3Dパーティクルの設定処理
//========================================================================
int SetParticle3D
(int nValue, int nLifeP, D3DXVECTOR3 posP, D3DXCOLOR col,   		// パーティクル(位置, 色, 生成量, 寿命) (移動方向, 移動速度)
 float fSpeedE, int nLifeE, float fRadiusE, float faddRadiusE)		// エフェクト(移動速度, 寿命)(大きさ, 大きさの加算量)			     							
{
	for (int nCntParticle = 0; nCntParticle < MAX_SET_PARTCL3D; nCntParticle++)
	{// 用意した分繰り返す

		if (g_aPaticle3D[nCntParticle].bUse == false)
		{// 使用していない場合

			g_aPaticle3D[nCntParticle].nParticleValue = nValue;				// パーティクルの生成量を設定
			g_aPaticle3D[nCntParticle].nParticleLife = nLifeP;				// パーティクルの寿命を設定

			g_aPaticle3D[nCntParticle].pos = posP;							// パーティクルの位置を設定

			g_aPaticle3D[nCntParticle].col = col;							// 色を設定を設定
			
			g_aPaticle3D[nCntParticle].fSpeedEffect = fSpeedE;				// エフェクトの移動速度を設定を設定
			
			g_aPaticle3D[nCntParticle].fEffectRadius = fRadiusE;			// エフェクトの大きさを設定
			g_aPaticle3D[nCntParticle].faddEffectRadius = faddRadiusE;		// エフェクトの大きさの加算量を設定
			
			g_aPaticle3D[nCntParticle].nEffectLife = nLifeE;				// エフェクトの寿命を設定を設定

			g_aPaticle3D[nCntParticle].bUse = true;							// 使用状態をtrueに設定

			return nCntParticle;	// ループを抜ける
		}
	}

	return -1;
}

//======================================================================== 
// 3Dパーティクルの位置設定処理
//======================================================================== 
void SetPosionParticle3D (int nIdx, const char* pMode, D3DXVECTOR3 pos)
{
	if (g_aPaticle3D[nIdx].bUse == true)
	{// 使用している場合

		if (strcmp(&pMode[0], "put") == false)
		{// pModeの値が"put"の場合

			g_aPaticle3D[nIdx].pos = pos;	// posの値を代入
		}
		else if(strcmp(&pMode[0], "add") == false)
		{// pModeの値が"add"の場合

			g_aPaticle3D[nIdx].pos += pos;	// posの値を加算
		}
	}
}