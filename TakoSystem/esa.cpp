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

#include "meshcylinder.h"
#include "watersurf.h"
#include "player.h"
#include "pot.h"

#include "oceancurrents.h"

// マクロ定義 ==================================================

// 設定値 ===================

// 挙動 : 地面
#define ESA_LANDING_MOVEVALUE	(0.1f)				// 地面にいる時の値の増加量	
#define ESA_LANDING_MOVESPEED	(0.05f)				// 地面にいる時の値の増加量

// 挙動 : 浮遊
#define ESA_BUOYANCY_MOVEVALUE	(0.3f)				// 浮いている時の値の増加量	
#define ESA_BUOYANCY_MOVESPEED	(0.05f)				// 浮いている時の値の増加量	

#define ESA_SWIM_SPEED			(0.001f)			// 浮いている時の移動(回転)速度

#define ESA_HOMING_SPEED		(0.01f)

// 計算用 ===================

#define ESA_CALC_SIZEARRAY(aArray)	(sizeof aArray / sizeof(aArray[0]))	// 配列の大きさを求める

#define ESA_CALC_REVROT(rot)		(((rot) < -D3DX_PI) ? (rot) + D3DX_PI * 2 :  /* rotの値が-PIを超えた場合、超えた-PI分を戻す */ \
									 ((rot) >  D3DX_PI) ? (rot) - D3DX_PI * 2 :  /* rotの値が+PIを超えた場合、超えた+PI分を戻す */ \
									 (rot))										 /* そのまま */

#define ESA_CHANGE_FLOAT(num)		((float)((num) / 100.0f))	// floatの値に変換
#define ESA_CHANGE_INT(num)			((int)  ((num) * 100))		// intの値に変換

// グローバル宣言 ==============================================

EsaModel g_aEsaModel[MAX_MODEL_ESA];	// エサのモデル情報
int g_aIdxEsaModel[MAX_MODEL_ESA];		// モデルのインデックス

Esa g_aEsa[MAX_SET_ESA];				// エサの情報

// モデルファイル情報
EsaModel_info g_aEsaModelInfo[] =
{// {ファイル名, モデルの移動(回転)速度, 当たり判定の大きさ, 獲得スコア}

	{"data/MODEL/esa/shell000.x",		0.0001f,	10.0f,	10},	// [0]車
	{"data/MODEL/esa/kani.x",			0.0003f,	10.0f,	10},	// [1]四角形
	{"data/MODEL/esa/shrimp.x",			0.005f,	10.0f,	10},	// [2]四角形
};

int g_nNumEsatype;						// エサの種類の総数

// エサの配置情報
Esa_info g_aEsaInfo[] =
{// {モデル種類, エサの挙動, 挙動の値, 位置, 角度}

	{0, ESA_ACTTYPE_SWIM, 0, D3DXVECTOR3(0.0f, 1200.0f, 800.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
	{1, ESA_ACTTYPE_GOTO_PLAYER, 0, D3DXVECTOR3(0.0f, 1200.0f, 1000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
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
		g_aEsa[nCntEsa].esaType = ESA_ACTTYPE_LAND;				// エサの挙動をLANDに設定
		g_aEsa[nCntEsa].nNumBehavior = 0;						// 挙動の値を初期化
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
#if 0	// Infoの設定

	for (int nCntEsa = 0; nCntEsa < ESA_CALC_SIZEARRAY(g_aEsaInfo); nCntEsa++)
	{// 配置する数だけ繰り返す

		// エサの設定処理
		SetEsa(g_aEsaInfo[nCntEsa].nidxType,
			   g_aEsaInfo[nCntEsa].esaType, g_aEsaInfo[nCntEsa].nBehavior,
			   g_aEsaInfo[nCntEsa].pos, g_aEsaInfo[nCntEsa].rot);
	}

#else	// ランダム設定

	for (int nCntEsa = 0; nCntEsa < 20; nCntEsa++)
	{// 配置する数だけ繰り返す

		int nSetType = rand() % ESATYPE_MAX;											// ランダムで種類を設定
		float fRandRadius = rand() % (int)OUTCYLINDER_RADIUS + (int)INCYLINDER_RADIUS;	// 中心からの距離を設定
		float fRandAngle = (float)(rand() % 629 - 314) / 1000.0f;						// 角度を設定
		float fRandHeight = rand() % (int)CYLINDER_HEIGHT;								// 高さを設定

		// 位置を設定
		D3DXVECTOR3 setPos = D3DXVECTOR3(sinf(fRandAngle) * fRandRadius,
										 fRandHeight,
										 cosf(fRandAngle) * fRandRadius);

		// エサの設定処理
		SetEsa(nSetType, ESA_ACTTYPE_SWIM, 0, setPos, D3DXVECTOR3(0.0f,0.0f,0.0f));
	}

#endif
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
	// 変数宣言 ===========================================
	
	float* pWaterSurfHeight = GetWaterSurf_Height();	// 現在の海面の高さを獲得

	// ====================================================
	
	for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	{
		if (g_aEsa[nCntEsa].bUse == true)
		{// 使用している場合

			// エサの挙動処理
			BehaviorEsa(&g_aEsa[nCntEsa]);


			// エサの移動処理
			MoveEsa(&g_aEsa[nCntEsa]);

			MoveOceanCurrents(&g_aEsa[nCntEsa].pos);

			// 海面を超えないよう修正
			if (g_aEsa[nCntEsa].pos.y > *pWaterSurfHeight - g_aEsaModel[g_aEsa[nCntEsa].nIdxModel].fHitRadius)
			{// 

				g_aEsa[nCntEsa].pos.y = *pWaterSurfHeight - g_aEsaModel[g_aEsa[nCntEsa].nIdxModel].fHitRadius;
			}

			SetEffect3D(70, g_aEsa[nCntEsa].pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0.0f, 30.0f, -0.1f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),EFFECTTYPE_NORMAL);

#if 0
			PrintDebugProc("\nESA[%d]_POS %s", nCntEsa, (g_aEsa[nCntEsa].bUse == true ? "true":"false"));
			PrintDebugProc("\nESA[%d]_POS %f %f %f", nCntEsa, g_aEsa[nCntEsa].pos.x, g_aEsa[nCntEsa].pos.y, g_aEsa[nCntEsa].pos.z);
#endif
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
int SetEsa(int nEsaType, ESA_ACTTYPE esaType, int nBehavior, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// 設定したいモデルがない場合
	if (g_aIdxEsaModel[nEsaType] == -1) return -1;	// 処理を抜ける

	for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	{
		if (g_aEsa[nCntEsa].bUse == false)
		{// 使用していない場合

			g_aEsa[nCntEsa].nIdxModel = g_aIdxEsaModel[nEsaType];	// 種類を設定
			g_aEsa[nCntEsa].pos = pos;								// 位置を設定
			g_aEsa[nCntEsa].rot = rot;								// 角度を設定
			g_aEsa[nCntEsa].fMoveAngle = 0.0f;						// 移動角度を初期化
			g_aEsa[nCntEsa].esaType = esaType;						// エサの挙動を設定
			g_aEsa[nCntEsa].nNumBehavior = 0;						// 挙動の値を初期化
			g_aEsa[nCntEsa].bDisp = true;							// 表示している状態に設定
			g_aEsa[nCntEsa].bUse = true;							// 使用している状態に設定

			return nCntEsa;	// 設定した場所を返す
		}
	}

	return -1;				// 設定してない事を返す
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

			pEsaModel->fSpeed = infoEsaModel.fSpeed;			// 移動速度を設定

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
// エサの状態切り替え処理
//========================================================================
void ChangeEsaState(int nIdxEsa, ESA_ACTTYPE changeState, int nValue)
{
	if (g_aEsa[nIdxEsa].bUse == true)
	{// 使用されている場合

		g_aEsa[nIdxEsa].esaType = changeState;	// 挙動の状態を設定
		g_aEsa[nIdxEsa].nNumBehavior = nValue;	// 挙動の値を設定
	}
}

//========================================================================
// エサの挙動の処理
//========================================================================
void BehaviorEsa(Esa* pEsa)
{
	// 変数宣言 ===========================================

	float fChangeBehavior = ESA_CHANGE_FLOAT(pEsa->nNumBehavior);	// 挙動の値(float)

	// ====================================================

	switch (pEsa->esaType)
	{
	case ESA_ACTTYPE_NONE:			// 何もしない


		break;

	case ESA_ACTTYPE_LAND:			// 着地状態

		fChangeBehavior = ESA_CALC_REVROT(fChangeBehavior + ESA_LANDING_MOVESPEED);		// 挙動の値(回転角度)を加算

		// エサの角度を更新
		pEsa->rot.z = sinf(fChangeBehavior) * ESA_LANDING_MOVEVALUE;

		// 挙動の値をIntの値に変換
		pEsa->nNumBehavior = ESA_CHANGE_INT(fChangeBehavior);

		break;

	case ESA_ACTTYPE_SWIM:			// 浮遊状態

		fChangeBehavior = ESA_CALC_REVROT(fChangeBehavior + ESA_BUOYANCY_MOVESPEED);	// 挙動の値(移動角度)を加算

		// エサの位置を更新
		pEsa->pos.y += sinf(fChangeBehavior) * ESA_BUOYANCY_MOVEVALUE;

		// 挙動の値をIntの値に変換
		pEsa->nNumBehavior = ESA_CHANGE_INT(fChangeBehavior);

		break;

	case ESA_ACTTYPE_GOTO_PLAYER:	// プレイヤーに向かう

		// 当たり判定より大きい判定でプレイヤーがその中に入ったら

		break;

	case ESA_ACTTYPE_GOTO_POT:		// ポットに向かう



		break;
	}
}

//========================================================================
// エサの移動処理
//========================================================================
void MoveEsa(Esa* pEsa)
{
	// 変数宣言 ===========================================

	float fWidth,fHeight, fDipth;	// 幅, 高さ, 奥行きがどれぐらいの幅なのか
	float fDistRadius;				// 中心からの距離(半径)
	float fNomRadius;				// 正規化した距離(半径)
	float fToTagetAngle = 0.0f;		// 対象との角度

	float *pWaterSurf = GetWaterSurf_Height();	// 海面の高さの情報

	Player* pPlayer = GetPlayer();				// プレイヤーの情報

	D3DXVECTOR3 tmpPos = pEsa->pos;				// エサの更新前の位置

	// ====================================================

	switch (pEsa->esaType)
	{
	case ESA_ACTTYPE_SWIM:			// 浮いている場合
#if 1
		fDistRadius = sqrtf(pEsa->pos.x * pEsa->pos.x + pEsa->pos.z * pEsa->pos.z);	// 中心からの距離を求める
		fNomRadius = fDistRadius / OUTCYLINDER_RADIUS;								// MAXとの正規化した値を求める
		fToTagetAngle = (float)atan2(pEsa->pos.x, pEsa->pos.z);						// 中心からの角度を求める

		// 角度を更新
		if (pEsa->nIdxModel == -1)
		{
			fToTagetAngle += ESA_SWIM_SPEED / fNomRadius;									// 移動量(角度)を正規化した距離の長さにする
		}
		else
		{
			fToTagetAngle += g_aEsaModel[pEsa->nIdxModel].fSpeed / fNomRadius;				// 移動量(角度)を正規化した距離の長さにする
		}
		
		// 位置を更新
		pEsa->pos.x = sinf(ESA_CALC_REVROT(fToTagetAngle)) * fDistRadius;
		pEsa->pos.z = cosf(ESA_CALC_REVROT(fToTagetAngle)) * fDistRadius;
#endif
		break;

	case ESA_ACTTYPE_GOTO_PLAYER:	// プレイヤーに向かう

		// 距離を求める
		fWidth  = pPlayer[pEsa->nNumBehavior].pos.x - pEsa->pos.x;	// 幅の値を求める
		fHeight = pPlayer[pEsa->nNumBehavior].pos.y - pEsa->pos.y;	// 高さの値を求める
		fDipth  = pPlayer[pEsa->nNumBehavior].pos.z - pEsa->pos.z;	// 奥行きの値を求める
		//fDistRadius = sqrtf(fWidth * fWidth + fDipth * fDipth);		// プレイヤーとの離れ具合を求める

		// 角度を求める
		fToTagetAngle = atan2f(fWidth * fWidth, fDipth * fDipth);	// プレイヤーとの角度を求める

		// 位置を更新
		//pEsa->pos.x += sinf(ESA_CALC_REVROT(fToTagetAngle)) * 0.9f;
		//pEsa->pos.z += cosf(ESA_CALC_REVROT(fToTagetAngle)) * 0.9f;
		pEsa->pos.x += fWidth * ESA_HOMING_SPEED;
		pEsa->pos.y += fHeight * ESA_HOMING_SPEED;
		pEsa->pos.z += fDipth * ESA_HOMING_SPEED;

		PrintDebugProc("\nESA_ROT %f", fToTagetAngle);

		break;

	case ESA_ACTTYPE_GOTO_POT:		// ポットに向かう

		// ポットの位置にホーミング
		pEsa->pos.x += pPlayer[pEsa->nNumBehavior].pos.x - pEsa->pos.x * ESA_HOMING_SPEED;
		pEsa->pos.y += pPlayer[pEsa->nNumBehavior].pos.y - pEsa->pos.y * ESA_HOMING_SPEED;
		pEsa->pos.z += pPlayer[pEsa->nNumBehavior].pos.z - pEsa->pos.z * ESA_HOMING_SPEED;

		break;
	}

	fWidth = tmpPos.x - pEsa->pos.x;
	fDipth = tmpPos.z - pEsa->pos.z;

	fToTagetAngle = atan2f(fWidth, fDipth);

	pEsa->rot.y = fToTagetAngle;
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

			//PrintDebugProc("ESA_COLLISION_DISTX %f\n", fDistX);
			//PrintDebugProc("ESA_COLLISION_DISTZ %f\n", fDistZ);
			//PrintDebugProc("ESA_COLLISION_DIST  %f\n", fDistLength);
			//PrintDebugProc("ESA_COLLISION_ROT  %f\n", fRot);

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