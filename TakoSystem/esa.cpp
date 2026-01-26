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

// マクロ定義 ==================================================

#define MAX_MODEL_ESA	(64)	// 用意出来るモデルの最大値
#define MAX_SET_ESA		(128)	// 設定出来るエサの総数

#define ESA_CALC_SIZEARRAY(aArray)	(sizeof aArray / sizeof(aArray[0]))

// 構造体の定義 ================================================

// エサの設定情報
typedef struct
{
	int nidxType;		// エサの種類

	ESATYPE esaType;	// エサの挙動

	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 角度

}Esa_info;

// グローバル宣言 ==============================================

EsaModel g_aEsaModel[MAX_MODEL_ESA];	// エサのモデル情報
int g_aIdxEsaModel[MAX_MODEL_ESA];		// モデルのインデックス

Esa g_aEsa[MAX_SET_ESA];				// エサの情報

// モデルファイル情報
EsaModel_info g_aEsaModelInfo[] =
{// {ファイル名, 当たり判定の大きさ}

	{"data/MODEL/testmodel/car000.x",		10.0f},	// [0]車
	{"data/MODEL/testmodel/skitree000.x",	10.0f},	// [1]四角形
};

// エサの配置情報
Esa_info g_aEsaInfo[] =
{// {モデル種類, 位置, 角度}

	{0, ESATYPE_LAND, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
	{1, ESATYPE_LAND, D3DXVECTOR3(50.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
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
		g_aEsa[nCntEsa].bDisp = false;							// 表示していない状態に設定
		g_aEsa[nCntEsa].bUse = false;							// 使用していない状態に設定
	}

	memset(g_aIdxEsaModel,-1,sizeof g_aIdxEsaModel);	// モデルのインデックスを初期化

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
	bool bD;
#if 0
	if (GetKeyboardPress(DIK_NUMPAD5)) g_aEsa[0].pos.z += 1.0f;
	if (GetKeyboardPress(DIK_NUMPAD2)) g_aEsa[0].pos.z -= 1.0f;
	if (GetKeyboardPress(DIK_NUMPAD1)) g_aEsa[0].pos.x -= 1.0f;
	if (GetKeyboardPress(DIK_NUMPAD3)) g_aEsa[0].pos.x += 1.0f;
#endif
	//bD = CollisionEsa(NULL, g_aEsa[0].pos, g_aEsaModel[g_aIdxEsaModel[g_aEsa[0].nIdxModel]].fHitRadius);

	//PrintDebugProc("ESA_COLLISION %s", (bD == true) ? "TRUE" : "FALSE");
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
		{
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
void SetEsa(int nEsaType, ESATYPE esaType, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
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

			return nCntModel;									// 設定した場所を返す
			
		}
	}

	// 設定できる場所が残っていなかった場合
	return -1;		// 設定した場所がない事を返す
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
			fDistLength = sqrtf((fDistX * fDistX + fDistZ * fDistZ) * 0.5f);

			// 角度を求める
			fRot = atan2f(fDistX * fDistX, fDistZ * fDistX);

			PrintDebugProc("left", "ESA_COLLISION_DISTX %f\n", fDistX);
			PrintDebugProc("left", "ESA_COLLISION_DISTZ %f\n", fDistZ);
			PrintDebugProc("left", "ESA_COLLISION_DIST  %f\n", fDistLength);
			PrintDebugProc("left", "ESA_COLLISION_ROT  %f\n", fRot);

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