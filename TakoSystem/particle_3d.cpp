//========================================================================
// 
// 3Dパーティクル処理[paticle_3d.cpp]
// Author:稲村 蒼稀
// 
//========================================================================
#include "main.h"

#include "particle_3d.h"	// 3Dパーティクルヘッダー

#include "input.h"
#include "player.h"

// マクロ定義 ==================================================

#define MAX_SET_PARTCL3D		(5096)	// パーティクルの最大数

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

		g_aPaticle3D[nCntPaticle].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 移動量を初期化

		g_aPaticle3D[nCntPaticle].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色を初期値に設定

		g_aPaticle3D[nCntPaticle].fSpeedPaticle = 6.0f;						// パーティクルの移動量を初期化
		g_aPaticle3D[nCntPaticle].fVecMoveParticle = { .0f,0.0f,0.0f };		// パーティクルの移動方向を初期化

		g_aPaticle3D[nCntPaticle].nParticleValue = 0;						// パーティクルの生成量を初期化
		g_aPaticle3D[nCntPaticle].nParticleLifeO = 0;						// パーティクルの寿命を初期化
		g_aPaticle3D[nCntPaticle].nParticleLifeV = 0;						// パーティクルの寿命の最大値の初期化	

		// エフェクト
		g_aPaticle3D[nCntPaticle].fSpeedEffect = 0.0f;						// エフェクトの移動速度を初期化

		g_aPaticle3D[nCntPaticle].fEffectRadius = 0.0f;						// エフェクトの大きさを初期化
		g_aPaticle3D[nCntPaticle].faddEffectRadius = 0.0f;					// エフェクトの大きさの加算量を初期化

		g_aPaticle3D[nCntPaticle].nEffectLife = 0;							// エフェクトの寿命を初期化

		// 状態
		g_aPaticle3D[nCntPaticle].bUse = false;								// 使用していない状況に設定

		g_aPaticle3D[nCntPaticle].effecttype = EFFECTTYPE_NORMAL;			// エフェクトタイプを初期化
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

	D3DXVECTOR3 rot = {};	// エフェクトの移動角度

	// ====================================================


	for (int nCntPaticle = 0; nCntPaticle < MAX_SET_PARTCL3D; nCntPaticle++)
	{// 用意した分繰り返す

		if (g_aPaticle3D[nCntPaticle].bUse == true)
		{// 使用している場合

			if (g_aPaticle3D[nCntPaticle].effecttype = EFFECTTYPE_OCTOINK)
			{ // 用途がタコ墨の場合

				// パーティクル本体の位置を移動
				g_aPaticle3D[nCntPaticle].pos += (g_aPaticle3D[nCntPaticle].fVecMoveParticle * g_aPaticle3D[nCntPaticle].fSpeedPaticle);

				// パーティクルの生成
				if (g_aPaticle3D[nCntPaticle].nParticleLifeV < g_aPaticle3D[nCntPaticle].nParticleLifeO / 6)
				{ //発射から一定時間経過

					for (int nCntAppear = 0; nCntAppear < g_aPaticle3D[nCntPaticle].nParticleValue; nCntAppear++)
					{// 生成するだけ繰り返す

						rot.x = (float)(rand() % 629 - 314) / 100.0f;		// 角度を設定
						rot.y = (float)(rand() % 629 - 314) / 100.0f;		// 角度を設定
						rot.z = (float)(rand() % 629 - 314) / 100.0f;		// 角度を設定

						D3DXVec3Normalize(&rot, &rot);						// 正規化

						// 3Dエフェクトの設定
						SetEffect3D(g_aPaticle3D[nCntPaticle].nEffectLife,	// 寿命
							g_aPaticle3D[nCntPaticle].pos,					// 位置
							D3DXVECTOR3(rot.x, rot.y, rot.z),				// 移動方向
							g_aPaticle3D[nCntPaticle].fSpeedEffect,			// 移動速度
							g_aPaticle3D[nCntPaticle].fEffectRadius,		// 大きさ
							g_aPaticle3D[nCntPaticle].faddEffectRadius,		// 大きさの加算量
							g_aPaticle3D[nCntPaticle].col,					// エフェクトの色	
							g_aPaticle3D[nCntPaticle].effecttype);
					}
				}
				else
				{
					rot.x = (float)(rand() % 629 - 314) / 1000.0f;		// 角度を設定
					rot.y = (float)(rand() % 629 - 314) / 1000.0f;		// 角度を設定
					rot.z = (float)(rand() % 629 - 314) / 1000.0f;		// 角度を設定

					for (int nCntAppear = 0; nCntAppear < 1; nCntAppear++)
					{// 生成するだけ繰り返す

						// 3Dエフェクトの設定
						SetEffect3D(100,										// 寿命
							g_aPaticle3D[nCntPaticle].pos,						// 位置
							D3DXVECTOR3(rot.x, rot.y, rot.z),					// 移動方向
							g_aPaticle3D[nCntPaticle].fSpeedEffect * ((float)g_aPaticle3D[nCntPaticle].nParticleLifeV / g_aPaticle3D[nCntPaticle].nParticleLifeO),			// 移動速度
							g_aPaticle3D[nCntPaticle].fEffectRadius / 2,		// 大きさ
							g_aPaticle3D[nCntPaticle].faddEffectRadius / 2,		// 大きさの加算量
							g_aPaticle3D[nCntPaticle].col,						// エフェクトの色	
							g_aPaticle3D[nCntPaticle].effecttype);
					}
				}
			}

			g_aPaticle3D[nCntPaticle].nParticleLifeV--;	// 寿命を減らす
		}

		if (g_aPaticle3D[nCntPaticle].nParticleLifeV <= 0)
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
(int nValue, int nLifeP, D3DXVECTOR3 posP, D3DXCOLOR col, D3DXVECTOR3 vec,   				// パーティクル(位置, 色, 生成量, 寿命) (移動方向, 移動速度)
	float fSpeedE, int nLifeE, float fRadiusE, float faddRadiusE, EFFECTTYPE effecttype)	// エフェクト(移動速度, 寿命)(大きさ, 大きさの加算量, 用途)			     							
{
	int Radian = 400;		//範囲

	for (int nCntParticle = 0; nCntParticle < MAX_SET_PARTCL3D; nCntParticle++)
	{// 用意した分繰り返す

		if (g_aPaticle3D[nCntParticle].bUse == false)
		{// 使用していない場合

			g_aPaticle3D[nCntParticle].nParticleValue = nValue;				// パーティクルの生成量を設定
			g_aPaticle3D[nCntParticle].nParticleLifeO = nLifeP;				// パーティクルの寿命を設定
			g_aPaticle3D[nCntParticle].nParticleLifeV = nLifeP;

			g_aPaticle3D[nCntParticle].pos = posP;							// パーティクルの位置を設定
			g_aPaticle3D[nCntParticle].fVecMoveParticle =					// パーティクルの移動方向を設定
			{
			sinf(vec.y) + ((rand() % Radian - (Radian / 2)) * 3.14f / 1000),
			(float)(rand() % 157 - 79) / 100.0f,
			cosf(vec.y) + ((rand() % Radian - (Radian / 2)) * 3.14f / 1000)
			};

			g_aPaticle3D[nCntParticle].move = {};							// 移動量を初期化

			g_aPaticle3D[nCntParticle].col = col;							// 色を設定を設定

			g_aPaticle3D[nCntParticle].fSpeedEffect = fSpeedE;				// エフェクトの移動速度を設定を設定
			g_aPaticle3D[nCntParticle].fEffectRadius = fRadiusE;			// エフェクトの大きさを設定
			g_aPaticle3D[nCntParticle].faddEffectRadius = faddRadiusE;		// エフェクトの大きさの加算量を設定

			g_aPaticle3D[nCntParticle].nEffectLife = nLifeE;				// エフェクトの寿命を設定を設定

			g_aPaticle3D[nCntParticle].bUse = true;							// 使用状態をtrueに設定

			g_aPaticle3D[nCntParticle].effecttype = effecttype;				// 用途を設定

			return nCntParticle;	// ループを抜ける
		}
	}

	return -1;
}

//======================================================================== 
// 3Dパーティクルの位置設定処理
//======================================================================== 
void SetPosionParticle3D(int nIdx, const char* pMode, D3DXVECTOR3 pos)
{
	if (g_aPaticle3D[nIdx].bUse == true)
	{// 使用している場合

		if (strcmp(&pMode[0], "put") == false)
		{// pModeの値が"put"の場合

			g_aPaticle3D[nIdx].pos = pos;	// posの値を代入
		}
		else if (strcmp(&pMode[0], "add") == false)
		{// pModeの値が"add"の場合

			g_aPaticle3D[nIdx].pos += pos;	// posの値を加算
		}
	}
}