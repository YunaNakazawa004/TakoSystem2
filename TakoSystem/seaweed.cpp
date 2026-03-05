//=============================================================================
// 
// 海藻 [seaweed.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "seaweed.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MOVEMENT				(D3DXVECTOR3(0.5f, 0.5f, 0.5f))			// 移動量
#define SEAWEED_HEIGHT			(20.0f)									// 海藻の高さ
#define MAX_SEAWEED				(128)									// 海藻の最大量

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Seaweed g_aSeaweed[MAX_SEAWEED];						// 海藻の情報
Model_Info g_SeaweedModel[SEAWEEDTYPE_MAX];			// 海藻モデルの情報
const char* c_apFilenameSeaweed[SEAWEEDTYPE_MAX] = 
{
	"data\\MODEL\\seaweed\\seaweed000.x",
	"data\\MODEL\\seaweed\\seaweed001.x",
};

//=============================================================================
// 海藻の初期化処理
//=============================================================================
void InitSeaweed(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	Seaweed* pSeaweed = GetSeaweed();

	// 海藻の情報の初期化
	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++, pSeaweed++)
	{
		pSeaweed->pos = FIRST_POS;
		pSeaweed->rot = FIRST_POS;
		pSeaweed->bUse = false;
		pSeaweed->nIdxSafe = -1;
		pSeaweed->nNumModel = 0;
	}

	// モデルの読み込み
	for (int nCntModel = 0; nCntModel < SEAWEEDTYPE_MAX; nCntModel++)
	{// 海藻のパーツの総数分繰り返す
		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(c_apFilenameSeaweed[nCntModel],
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&g_SeaweedModel[nCntModel].pBuffMat,
			NULL,
			&g_SeaweedModel[nCntModel].dwNumMat,
			&g_SeaweedModel[nCntModel].pMesh)))
		{// モデルの読み込みに失敗した場合
			continue;
		}

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)g_SeaweedModel[nCntModel].pBuffMat->GetBufferPointer();

		// マテリアルの読み込み
		for (int nCntMat = 0; nCntMat < (int)g_SeaweedModel[nCntModel].dwNumMat; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_SeaweedModel[nCntModel].apTexture[nCntMat]);
			}
		}
	}

#if 0
	// ランダムな位置に設定
	SetRandomSeaweed(ALL_OCTO - GetNumCamera());
#endif
}

//=============================================================================
// 海藻の終了処理
//=============================================================================
void UninitSeaweed(void)
{
	for (int nCntModel = 0; nCntModel < SEAWEEDTYPE_MAX; nCntModel++)
	{
		// メッシュの破棄
		if (g_SeaweedModel[nCntModel].pMesh != NULL)
		{
			g_SeaweedModel[nCntModel].pMesh->Release();
			g_SeaweedModel[nCntModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (g_SeaweedModel[nCntModel].pBuffMat != NULL)
		{
			g_SeaweedModel[nCntModel].pBuffMat->Release();
			g_SeaweedModel[nCntModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntTex = 0; nCntTex < MAX_TEXTURE; nCntTex++)
		{

			if (g_SeaweedModel[nCntModel].apTexture[nCntTex] != NULL)
			{
				g_SeaweedModel[nCntModel].apTexture[nCntTex]->Release();
				g_SeaweedModel[nCntModel].apTexture[nCntTex] = NULL;
			}
		}
	}
}

//=============================================================================
// 海藻の更新処理
//=============================================================================
void UpdateSeaweed(void)
{
}

//=============================================================================
// 海藻の描画処理
//=============================================================================
void DrawSeaweed(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ
	Seaweed* pSeaweed = GetSeaweed();

	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++, pSeaweed++)
	{
		if (pSeaweed->bUse == true)
		{// 使用しているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pSeaweed->mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pSeaweed->rot.y, pSeaweed->rot.x, pSeaweed->rot.z);
			D3DXMatrixMultiply(&pSeaweed->mtxWorld, &pSeaweed->mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pSeaweed->pos.x, pSeaweed->pos.y, pSeaweed->pos.z);
			D3DXMatrixMultiply(&pSeaweed->mtxWorld, &pSeaweed->mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pSeaweed->mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// 全モデル(パーツ)の描画
			for (int nCntModel = 0; nCntModel < pSeaweed->nNumModel; nCntModel++)
			{
				D3DXMATRIX mtxRotModel, mtxTransModel, mtxScaleModel;		// 計算用マトリックス
				D3DXMATRIX mtxParent;						// 親のマトリックス

				// パーツのワールドマトリックスの初期化
				D3DXMatrixIdentity(&pSeaweed->aModel[nCntModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotModel, pSeaweed->aModel[nCntModel].rot.y, pSeaweed->aModel[nCntModel].rot.x, pSeaweed->aModel[nCntModel].rot.z);
				D3DXMatrixMultiply(&pSeaweed->aModel[nCntModel].mtxWorld, &pSeaweed->aModel[nCntModel].mtxWorld, &mtxRotModel);

				// パーツの位置を反映
				D3DXMatrixTranslation(&mtxTransModel, pSeaweed->aModel[nCntModel].pos.x, pSeaweed->aModel[nCntModel].pos.y, pSeaweed->aModel[nCntModel].pos.z);
				D3DXMatrixMultiply(&pSeaweed->aModel[nCntModel].mtxWorld, &pSeaweed->aModel[nCntModel].mtxWorld, &mtxTransModel);

				// パーツの「親のマトリックス」を設定
				if (pSeaweed->aModel[nCntModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = pSeaweed->aModel[pSeaweed->aModel[nCntModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = pSeaweed->mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
				D3DXMatrixMultiply(&pSeaweed->aModel[nCntModel].mtxWorld,
					&pSeaweed->aModel[nCntModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &pSeaweed->aModel[nCntModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)g_SeaweedModel[pSeaweed->aModel[nCntModel].type].pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)g_SeaweedModel[pSeaweed->aModel[nCntModel].type].dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, g_SeaweedModel[pSeaweed->aModel[nCntModel].type].apTexture[nCntMat]);

					// モデルパーツの描画
					g_SeaweedModel[pSeaweed->aModel[nCntModel].type].pMesh->DrawSubset(nCntMat);
				}
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// 海藻の設定処理
//=============================================================================
void SetSeaweed(D3DXVECTOR3 pos, int nLength)
{
	Seaweed* pSeaweed = GetSeaweed();

	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++, pSeaweed++)
	{
		if (pSeaweed->bUse == false)
		{// 使用していない
			pSeaweed->pos = pos;
			pSeaweed->rot = FIRST_POS;
			pSeaweed->nIdxSafe = -1;
			pSeaweed->bUse = true;
			pSeaweed->nNumModel = nLength;

			for (int nCntModel = 0; nCntModel < nLength; nCntModel++)
			{
				pSeaweed->aModel[nCntModel].nIdx = nCntModel;
				pSeaweed->aModel[nCntModel].nIdxModelParent = nCntModel - 1;

				if (nCntModel == 0)
				{// 親
					pSeaweed->aModel[nCntModel].pos = FIRST_POS;
					pSeaweed->aModel[nCntModel].posOff = FIRST_POS;
				}
				else
				{// 親以外
					pSeaweed->aModel[nCntModel].pos = D3DXVECTOR3(0.0f, SEAWEED_HEIGHT, 0.0f);
					pSeaweed->aModel[nCntModel].posOff = D3DXVECTOR3(0.0f, SEAWEED_HEIGHT, 0.0f);
				}

				pSeaweed->aModel[nCntModel].rot = FIRST_POS;
				pSeaweed->aModel[nCntModel].rotOff = FIRST_POS;

				if (nCntModel != nLength - 1)
				{// 最後以外
					pSeaweed->aModel[nCntModel].type = SEAWEEDTYPE_BOTTOM;
				}
				else
				{// 最後
					pSeaweed->aModel[nCntModel].type = SEAWEEDTYPE_TOP;
				}
			}

			break;
		}
	}
}

//=============================================================================
// 海藻の取得
//=============================================================================
Seaweed* GetSeaweed(void)
{
	return &g_aSeaweed[0];
}

//=============================================================================
// 海藻のランダム設定処理
//=============================================================================
void SetRandomSeaweed(int nAmount)
{
	for (int nCntCPU = 0; nCntCPU < nAmount; nCntCPU++)
	{
		D3DXVECTOR3 pos;
		float fAngle = (D3DX_PI * 2.0f) * ((float)((nCntCPU + 1) * (360.0f / nAmount)) / 360.0f);
		//float fsin = sinf(fAngle);

		pos.x = sinf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));
		pos.y = 0.0f;
		pos.z = cosf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));

		int nLength = rand() % 70 + 30;

		SetSeaweed(pos, nLength);
	}
}