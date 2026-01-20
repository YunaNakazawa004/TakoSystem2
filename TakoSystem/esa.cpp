//==================================================================================
// 
// エサに関する処理[esa.cpp]
// Author : Soki Inamura 
// 
//==================================================================================
#include "main.h"

#include "model.h"
#include "esa.h"

// マクロ定義 ==================================================

#define MAX_MODEL_ESA	(64)	// 用意出来るモデルの最大値
#define MAX_SET_ESA		(128)	// 設定出来るエサの総数

#define ESA_CALC_SIZEARRAY(aArray)	(sizeof aArray / sizeof(aArray[0]))

// 構造体の定義 ================================================

// エサの情報
typedef struct
{
	int nIdxModel;			// モデルのインデックス

	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 角度

	D3DXMATRIX mtxWorld;	// ワールドマトリックス

	bool bDisp;				// 表示状態
	bool bUse;				// 使用状態

}Esa;

// エサの設定情報
typedef struct
{
	int nidxType;		// エサの種類

	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 角度

}Esa_info;

// グローバル宣言 ==============================================

EsaModel g_aEsaModel[MAX_MODEL_ESA];	// エサのモデル情報
int g_aIdxEsaModel[MAX_MODEL_ESA];		// モデルのインデックス

Esa g_aEsa[MAX_SET_ESA];				// エサの情報

// モデルファイル名
const char* c_apFilenameEsa[] =
{
	"data/MODEL/testmodel/car000.x",	// [0]車
	"data/MODEL/testmodel/skitree000.x",	// [1]四角形
};

// エサの配置情報
Esa_info g_aEsaInfo[] =
{
	{0, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
	{0, D3DXVECTOR3(50.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f)},
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

	memset(g_aIdxEsaModel,-1,sizeof g_aIdxEsaModel);	// モデルのラベルを初期化

	// モデル読み込み
	for (int nCntEsaModel = 0; nCntEsaModel < ESA_CALC_SIZEARRAY(c_apFilenameEsa); nCntEsaModel++)
	{// 用意したファイルの数だけ繰り返す

		// エサのモデル読み込み処理
		g_aIdxEsaModel[nCntEsaModel] = SetModelEsa(c_apFilenameEsa[nCntEsaModel], &g_aEsaModel[0], ESA_CALC_SIZEARRAY(g_aEsaModel));
	}

	// エサの配置
	for (int nCntEsa = 0; nCntEsa < ESA_CALC_SIZEARRAY(g_aEsaInfo); nCntEsa++)
	{// 配置する数だけ繰り返す

		// エサの設定処理
		SetEsa(g_aEsaInfo[nCntEsa].nidxType, g_aEsaInfo[nCntEsa].pos, g_aEsaInfo[nCntEsa].rot);
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
			pMat = (D3DXMATERIAL*)g_aEsaModel[g_aIdxEsaModel[g_aEsa[nCntEsa].nIdxModel]].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aEsaModel[g_aIdxEsaModel[g_aEsa[nCntEsa].nIdxModel]].dwNumMat; nCntMat++)
			{// マテリアルの数分繰り返す

				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_aEsaModel[g_aIdxEsaModel[g_aEsa[nCntEsa].nIdxModel]].apTexture[nCntMat]);

				// モデルパーツの描画
				g_aEsaModel[g_aIdxEsaModel[g_aEsa[nCntEsa].nIdxModel]].pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//========================================================================
// エサの設定処理
//========================================================================
void SetEsa(int nType, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	// モデルが設定されていない場合
	if (g_aIdxEsaModel[nType] == -1) return;	// 処理を抜ける

	for (int nCntEsa = 0; nCntEsa < MAX_SET_ESA; nCntEsa++)
	{
		if (g_aEsa[nCntEsa].bUse == false)
		{// 使用していない場合

			g_aEsa[nCntEsa].nIdxModel = nType;	// 種類を設定
			g_aEsa[nCntEsa].pos = pos;			// 位置を設定
			g_aEsa[nCntEsa].rot = rot;			// 角度を設定
			g_aEsa[nCntEsa].bDisp = true;		// 表示している状態に設定
			g_aEsa[nCntEsa].bUse = true;		// 使用している状態に設定

			break;	// for文を抜ける
		}
	}
}

//========================================================================
// エサのモデル設定処理
//========================================================================
int SetModelEsa(const char* pFilenameModel, EsaModel* pEsaModel, int nMaxSizeNum)
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
			D3DXLoadMeshFromX(pFilenameModel,
							  D3DXMESH_SYSTEMMEM,
							  pDevice,
							  NULL,
							  &pEsaModel->pBuffMat,
							  NULL,
							  &pEsaModel->dwNumMat,
							  &pEsaModel->pMesh);

			D3DXMATERIAL* pMat;	// マテリアルのポインタを宣言

			// マテリアルデータへのポインタを所得
			pMat = (D3DXMATERIAL*)pEsaModel->pBuffMat->GetBufferPointer();

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

			pEsaModel->bUse = true;	// 使用している状態に設定

			return nCntModel;	// 設定した位置を返す
		}
	}

	return -1;
}