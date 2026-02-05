//==================================================================================
// 
// エサに関する処理[esa.cpp]
// Author : Soki Inamura 
// 
//==================================================================================
#include "main.h"

#include "esa.h"

#include "input.h"
#include "debugproc.h"

#include "effect_3d.h"

// マクロ定義 ==================================================

// 設定値 ===================

// 挙動 : 地面
#define ESA_LANDING_MOVEVALUE	(0.1f)				// 地面にいる時の値の増加量	
#define ESA_LANDING_MOVESPEED	(0.05f)				// 地面にいる時の値の増加量

// 挙動 : 浮遊
#define ESA_BUOYANCY_MOVEVALUE	(0.3f)				// 浮いている時の値の増加量	
#define ESA_BUOYANCY_MOVESPEED	(0.05f)				// 浮いている時の値の増加量	

#define ESA_SWIM_SPEED			(0.001f)			// 浮いている時の移動(回転)速度

// 計算用 ===================

#define ESA_CALC_SIZEARRAY(aArray)	(sizeof aArray / sizeof(aArray[0]))	// 配列の大きさを求める

#define ESA_CALC_REVROT(rot)		(((rot) < -D3DX_PI) ? (rot) + D3DX_PI * 2 :  /* rotの値が-PIを超えた場合、超えた-PI分を戻す */ \
									 ((rot) >  D3DX_PI) ? (rot) - D3DX_PI * 2 :  /* rotの値が+PIを超えた場合、超えた+PI分を戻す */ \
									 (rot))										 /* そのまま */

// グローバル宣言 ==============================================

EsaModel g_aEsaModel[MAX_MODEL_ESA];	// エサのモデル情報
int g_aIdxEsaModel[MAX_MODEL_ESA];		// モデルのインデックス

Esa g_aEsa[MAX_SET_ESA];				// エサの情報

// モデルファイル情報
EsaModel_info g_aEsaModelInfo[] =
{// {ファイル名, モデルの移動(回転)速度, 当たり判定の大きさ, 獲得スコア}

	{"data/MODEL/testmodel/car000.x",		0.001f,	10.0f,	10},	// [0]車
	{"data/MODEL/esa/kani.x",				0.001f,	10.0f,	10},	// [1]四角形
	{"data/MODEL/testmodel/skitree000.x",	0.001f,	10.0f,	10},	// [2]四角形
};

int g_nNumEsatype;						// エサの種類の総数

// エサの配置情報
Esa_info g_aEsaInfo[] =
{// {モデル種類, エサの挙動, 位置, 角度}

	{1, ESA_ACTTYPE_LAND, D3DXVECTOR3(0.0f, 1200.0f, 800.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
	{1, ESA_ACTTYPE_LAND, D3DXVECTOR3(0.0f, 1200.0f, 1000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
};

//========================================================================
// エサの初期化処理
//========================================================================
void InitEsa(void)
{
	// エサの情報を初期化
	for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	{
		g_aEsa[nCntEsa].nIdxModel = -1;							// モデルのインデックスを初期化
		g_aEsa[nCntEsa].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置を初期化
		g_aEsa[nCntEsa].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 角度を初期化
		g_aEsa[nCntEsa].fMoveAngle = 0.0f;						// 移動角度を初期化
		g_aEsa[nCntEsa].esaType = ESA_ACTTYPE_LAND;					// エサの挙動をLANDに設定
		g_aEsa[nCntEsa].fNumBehavior = 0.0f;					// 挙動の値を初期化
		g_aEsa[nCntEsa].bHave = false;							// 所持されてない状態に設定
		g_aEsa[nCntEsa].bDisp = false;							// 表示していない状態に設定
		g_aEsa[nCntEsa].bUse = false;							// 使用していない状態に設定
	}

	memset(g_aIdxEsaModel,-1,sizeof g_aIdxEsaModel);	// モデルのインデックスを初期化

	g_nNumEsatype = 0;									// エサの種類の総数を初期化

	// モデル読み込み
	for (int nCntEsaModel = 0; nCntEsaModel < ESA_CALC_SIZEARRAY(g_aEsaModelInfo); nCntEsaModel++)
	{// 用意したファイルの数だけ繰り返す

		// エサのモデル読み込み処理
		g_aIdxEsaModel[nCntEsaModel] = SetModelEsa(g_aEsaModelInfo[nCntEsaModel], &g_aEsaModel[0], ESA_CALC_SIZEARRAY(g_aEsaModel));
	}

	// エサの配置
	for (int nCntEsa = 0; nCntEsa < ESA_CALC_SIZEARRAY(g_aEsaInfo); nCntEsa++)
	{// 配置する数だけ繰り返す

		// エサの設定処理
		SetEsa(g_aEsaInfo[nCntEsa].nidxType, g_aEsaInfo[nCntEsa].esaType, g_aEsaInfo[nCntEsa].pos, g_aEsaInfo[nCntEsa].rot);
	}
}

//========================================================================
// エサの終了処理
//========================================================================
void UninitEsa(void)
{
	for (int nCntEsaModel = 0; nCntEsaModel < MAX_MODEL_ESA; nCntEsaModel++)
	{
		// テクスチャの破棄
		for (int nCntTexture = 0; nCntTexture < MAX_TEXTURE; nCntTexture++)
		{
			if (g_aEsaModel[nCntEsaModel].apTexture[nCntTexture] != NULL)
			{// 情報がある場合

				g_aEsaModel[nCntEsaModel].apTexture[nCntTexture]->Release();
				g_aEsaModel[nCntEsaModel].apTexture[nCntTexture] = NULL;
			}
		}

		// メッシュの破棄
		if (g_aEsaModel[nCntEsaModel].pMesh != NULL)
		{// 情報がある場合

			g_aEsaModel[nCntEsaModel].pMesh->Release();
			g_aEsaModel[nCntEsaModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (g_aEsaModel[nCntEsaModel].pBuffMat != NULL)
		{// 情報がある場合

			g_aEsaModel[nCntEsaModel].pBuffMat->Release();
			g_aEsaModel[nCntEsaModel].pBuffMat = NULL;
		}
	}
}

//========================================================================
// エサの更新処理
//========================================================================
void UpdateEsa(void)
{
	for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	{
		if (g_aEsa[nCntEsa].bUse == true)
		{// 使用している場合

			// エサの挙動処理
			BehaviorEsa(&g_aEsa[nCntEsa]);

			// エサの移動処理
			MoveEsa(&g_aEsa[nCntEsa]);


			SetEffect3D(70, g_aEsa[nCntEsa].pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0.0f, 30.0f, -0.1f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),EFFECTTYPE_NORMAL);
		}
	}


}

//========================================================================
// エサの描画処理
//========================================================================
void DrawEsa(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DMATERIAL9 matDef;			// 現在のマテリアル保存用
	D3DXMATERIAL *pMat;				// マテリアルのポインタ

	// ====================================================

	for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	{
		if (g_aEsa[nCntEsa].bUse == true && g_aEsa[nCntEsa].bDisp == true)
		{// 使用している場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aEsa[nCntEsa].mtxWorld);	// ワールドマトリックスの初期値を設定

			// 向きの反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
										   g_aEsa[nCntEsa].rot.y, g_aEsa[nCntEsa].rot.x, g_aEsa[nCntEsa].rot.z);

			D3DXMatrixMultiply(&g_aEsa[nCntEsa].mtxWorld, &g_aEsa[nCntEsa].mtxWorld, &mtxRot);

			// 位置を設定
			D3DXMatrixTranslation(&mtxTrans,
								  g_aEsa[nCntEsa].pos.x, g_aEsa[nCntEsa].pos.y, g_aEsa[nCntEsa].pos.z);

			D3DXMatrixMultiply(&g_aEsa[nCntEsa].mtxWorld, &g_aEsa[nCntEsa].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aEsa[nCntEsa].mtxWorld);

			// 現在のマテリアルを所得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを所得
			pMat = (D3DXMATERIAL*)g_aEsaModel[g_aEsa[nCntEsa].nIdxModel].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aEsaModel[g_aEsa[nCntEsa].nIdxModel].dwNumMat; nCntMat++)
			{// マテリアルの数分繰り返す

				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_aEsaModel[g_aEsa[nCntEsa].nIdxModel].apTexture[nCntMat]);

				// モデルパーツの描画
				g_aEsaModel[g_aEsa[nCntEsa].nIdxModel].pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//========================================================================
// エサの設定処理
//========================================================================
void SetEsa(int nEsaType, ESA_ACTTYPE esaType, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 設定したいモデルがない場合
	if (g_aIdxEsaModel[nEsaType] == -1) return;	// 処理を抜ける

	for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	{
		if (g_aEsa[nCntEsa].bUse == false)
		{// 使用していない場合

			g_aEsa[nCntEsa].nIdxModel = g_aIdxEsaModel[nEsaType];	// 種類を設定
			g_aEsa[nCntEsa].pos = pos;								// 位置を設定
			g_aEsa[nCntEsa].rot = rot;								// 角度を設定
			g_aEsa[nCntEsa].fMoveAngle = 0.0f;						// 移動角度を初期化
			g_aEsa[nCntEsa].esaType = esaType;						// エサの挙動を設定
			g_aEsa[nCntEsa].fNumBehavior = 0.0f;					// 挙動の値を初期化
			g_aEsa[nCntEsa].bDisp = true;							// 表示している状態に設定
			g_aEsa[nCntEsa].bUse = true;							// 使用している状態に設定

			break;	// for文を抜ける
		}
	}
}

//========================================================================
// エサのモデル設定処理
//========================================================================
int SetModelEsa(EsaModel_info infoEsaModel, EsaModel* pEsaModel, int nMaxSizeNum)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ====================================================

	for (int nCntModel = 0; nCntModel < nMaxSizeNum; nCntModel++, pEsaModel++)
	{
		if (pEsaModel->bUse == false)
		{// 使用していない場合

			// Xファイルの読み込み
			if(FAILED(D3DXLoadMeshFromX(&infoEsaModel.aFilename[0],
									    D3DXMESH_SYSTEMMEM,
									    pDevice,
									    NULL,
									    &pEsaModel->pBuffMat,
									    NULL,
									    &pEsaModel->dwNumMat,
									    &pEsaModel->pMesh)))
			{// Xファイルの読み込みに失敗した場合

				return -1;	// 設定した場所がない事を返す
			}

			D3DXMATERIAL* pMat;	// マテリアルのポインタを宣言

			// マテリアルデータへのポインタを所得
			pMat = (D3DXMATERIAL*)pEsaModel->pBuffMat->GetBufferPointer();

			// マテリアルの設定
			for (int nCntMat = 0; nCntMat < (int)pEsaModel->dwNumMat; nCntMat++)
			{// マテリアルの数分繰り返す

				if (pMat[nCntMat].pTextureFilename != NULL)
				{// テクスチャファイルがある

					// テクスチャの読み込み
					D3DXCreateTextureFromFile(pDevice,							// Direct3Dデバイスへのポインタ
											  pMat[nCntMat].pTextureFilename,	// 読み込むテクスチャ
											  &pEsaModel->apTexture[nCntMat]);	// テクスチャへのポインタ
				}
			}

			pEsaModel->fHitRadius = infoEsaModel.fHitRadius;	// 当たり判定の大きさを設定

			pEsaModel->bUse = true;								// 使用している状態に設定

			g_nNumEsatype++;									// エサの種類の総数を増やす

			return nCntModel;									// 設定した場所を返す
		}
	}

	// 設定できる場所が残っていなかった場合
	return -1;		// 設定した場所がない事を返す
}

//========================================================================
// エサの挙動の処理
//========================================================================
void BehaviorEsa(Esa* pEsa)
{
	switch (pEsa->esaType)
	{
	case ESA_ACTTYPE_LAND:	// 着地状態

		pEsa->fNumBehavior = ESA_CALC_REVROT(pEsa->fNumBehavior + ESA_LANDING_MOVESPEED);	// 挙動の値(回転角度)を加算

		// エサの角度を更新
		pEsa->rot.z = sinf(pEsa->fNumBehavior) * ESA_LANDING_MOVEVALUE;

		break;

	case ESA_ACTTYPE_SWIM:	// 浮遊状態

		pEsa->fNumBehavior = ESA_CALC_REVROT(pEsa->fNumBehavior + ESA_BUOYANCY_MOVESPEED);	// 挙動の値(移動角度)を加算

		// エサの位置を更新
		pEsa->pos.y += sinf(pEsa->fNumBehavior) * ESA_BUOYANCY_MOVEVALUE;

		break;
	}
}

//========================================================================
// エサの移動処理
//========================================================================
void MoveEsa(Esa* pEsa)
{
	// 変数宣言 ===========================================

	float fDistRadius;	// 中心からの距離(半径)
	float fNomRadius;	// 正規化した距離(半径)
	float fNowAngle;	// 現在の角度

	// ====================================================

	if (pEsa->esaType == ESA_ACTTYPE_SWIM)
	{// 浮いている場合

		fDistRadius = sqrtf(pEsa->pos.x * pEsa->pos.x + pEsa->pos.z * pEsa->pos.z);	// 中心からの距離を求める
		fNomRadius = fDistRadius / 18050.0f;										// MAXとの正規化した値を求める
		fNowAngle = (float)atan2(pEsa->pos.x, pEsa->pos.z);							// 中心からの角度を求める

		// 角度を更新
		fNowAngle += ESA_SWIM_SPEED / fNomRadius;									// 移動量(角度)を正規化した距離の長さにする
		
		// 位置を設定
		pEsa->pos.x = sinf(ESA_CALC_REVROT(fNowAngle)) * fDistRadius;
		pEsa->pos.z = cosf(ESA_CALC_REVROT(fNowAngle)) * fDistRadius;
	}
}

//========================================================================
// エサの当たり判定処理
//========================================================================
bool CollisionEsa(int* pIdx, bool bCollision, D3DXVECTOR3 *pos, float fHitRadius)
{
	// 変数宣言 ===========================================

	int nCntEsa = 1;

	float fDistX, fDistZ;
	float fDistLength;

	float fRot;

	// ====================================================

	//for (nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	//{}
		if (g_aEsa[nCntEsa].bUse == true)
		{// 使用している場合

			fDistX = g_aEsa[nCntEsa].pos.x - pos->x;
			fDistZ = g_aEsa[nCntEsa].pos.z - pos->z;

			// 離れている距離を求める
			fDistLength = sqrtf(fDistX * fDistX + fDistZ * fDistZ) * 0.5f;

			// 角度を求める
			fRot = atan2f(fDistX * fDistX, fDistZ * fDistX);

			PrintDebugProc("ESA_COLLISION_DISTX %f\n", fDistX);
			PrintDebugProc("ESA_COLLISION_DISTZ %f\n", fDistZ);
			PrintDebugProc("ESA_COLLISION_DIST  %f\n", fDistLength);
			PrintDebugProc("ESA_COLLISION_ROT  %f\n", fRot);

			// 判定
			if (fDistLength < g_aEsaModel[g_aIdxEsaModel[g_aEsa[nCntEsa].nIdxModel]].fHitRadius + fHitRadius)
			{// 離れている距離が当たり判定より小さい場合

				if (pIdx)*pIdx = nCntEsa;	// 接触したエサのインデックスを設定

				if (bCollision == true)
				{// 当たった時の処理をおこなう場合

					pos->x = g_aEsa[nCntEsa].pos.x + sinf(fRot + D3DX_PI) * g_aEsaModel[g_aIdxEsaModel[g_aEsa[nCntEsa].nIdxModel]].fHitRadius + fHitRadius;
					pos->z = g_aEsa[nCntEsa].pos.z + cosf(fRot + D3DX_PI) * g_aEsaModel[g_aIdxEsaModel[g_aEsa[nCntEsa].nIdxModel]].fHitRadius + fHitRadius;
				}

				return true;
			}
		}

	return false;
}

//========================================================================
// エサの情報を返す処理
//========================================================================
Esa* GetEsa(void)
{
	return &g_aEsa[0];
}

//========================================================================
// エサの種類数を返す処理
//========================================================================
int GetNumEsaType(void)
{
	return g_nNumEsatype;
}