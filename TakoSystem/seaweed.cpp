//=============================================================================
// 
// 海藻 [seaweed.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "seaweed.h"
#include "debugproc.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MOVEMENT				(D3DXVECTOR3(0.5f, 0.5f, 0.5f))			// 移動量
#define SEAWEED_HEIGHT			(160.0f)								// 海藻の高さ
#define MAX_SEAWEED				(128)									// 海藻の最大量
#define SEAWEED_DIST			(100.0f)								// 海藻が傾く距離

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

	// 海藻の情報の初期化
	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++)
	{
		g_aSeaweed[nCntSeaweed].pos = FIRST_POS;
		g_aSeaweed[nCntSeaweed].rot = FIRST_POS;
		g_aSeaweed[nCntSeaweed].bUse = false;
		g_aSeaweed[nCntSeaweed].nNumModel = 0;
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

	// ランダムな位置に設定
	SetRandomSeaweed(25);
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
	//static int nCounter = 0;

	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++)
	{
		if (g_aSeaweed[nCntSeaweed].bUse == false)
		{// 使用してないとき
			continue;
		}

		for (int nCntModel = 0; nCntModel < g_aSeaweed[nCntSeaweed].nNumModel; nCntModel++)
		{
			if (nCntModel == 0)
			{// 根本
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].fAngle += 0.01f;
			}
			else
			{// それ以外
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].fAngle = 0.01f + g_aSeaweed[nCntSeaweed].aModel[nCntModel - 1].fAngle;
			}

#if 0
			if (nCounter % (ONE_SECOND * 3) == 0)
			{// 定期的に目的の値を設定しなおす
				if (nCntModel == 0)
				{
					D3DXVECTOR3 rotDest;

					rotDest.x = (float)(rand() % 629 - 314) / 100.0f;
					rotDest.y = (float)(rand() % 629 - 314) / 100.0f;
					rotDest.z = (float)(rand() % 629 - 314) / 100.0f;

					g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest = rotDest + g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotOff;
				}
				else
				{
					g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest = -g_aSeaweed[nCntSeaweed].aModel[nCntModel - 1].rotDest;
				}
			}

			g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot +=
				(g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest - g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot) * 0.005f;
#endif

			CorrectAngle(&g_aSeaweed[nCntSeaweed].aModel[nCntModel].fAngle, g_aSeaweed[nCntSeaweed].aModel[nCntModel].fAngle);

			g_aSeaweed[nCntSeaweed].aModel[nCntModel].pos.x = g_aSeaweed[nCntSeaweed].aModel[nCntModel].posOff.x +
				(sinf(g_aSeaweed[nCntSeaweed].aModel[nCntModel].fAngle) * 3.0f);
			g_aSeaweed[nCntSeaweed].aModel[nCntModel].pos.z = g_aSeaweed[nCntSeaweed].aModel[nCntModel].posOff.z +
				(cosf(g_aSeaweed[nCntSeaweed].aModel[nCntModel].fAngle) * 3.0f);

			g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.x +=
				(g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.x - g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.x) * 0.01f;
			g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.x +=
				(g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.z - g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.z) * 0.01f;

			g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.x +=
				(g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotOff.x - g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.x) * 0.01f;
			g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.z +=
				(g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotOff.z - g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.z) * 0.01f;

			g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.x +=
				(0.0f - g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.x) * 0.01f;
			g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.z +=
				(0.0f - g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.z) * 0.01f;
		}
	}

	//nCounter++;
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

	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++)
	{
		if (g_aSeaweed[nCntSeaweed].bUse == true)
		{// 使用しているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aSeaweed[nCntSeaweed].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aSeaweed[nCntSeaweed].rot.y, g_aSeaweed[nCntSeaweed].rot.x, g_aSeaweed[nCntSeaweed].rot.z);
			D3DXMatrixMultiply(&g_aSeaweed[nCntSeaweed].mtxWorld, &g_aSeaweed[nCntSeaweed].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aSeaweed[nCntSeaweed].pos.x, g_aSeaweed[nCntSeaweed].pos.y, g_aSeaweed[nCntSeaweed].pos.z);
			D3DXMatrixMultiply(&g_aSeaweed[nCntSeaweed].mtxWorld, &g_aSeaweed[nCntSeaweed].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aSeaweed[nCntSeaweed].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// 全モデル(パーツ)の描画
			for (int nCntModel = 0; nCntModel < g_aSeaweed[nCntSeaweed].nNumModel; nCntModel++)
			{
				D3DXMATRIX mtxRotModel, mtxTransModel, mtxScaleModel;		// 計算用マトリックス
				D3DXMATRIX mtxParent;						// 親のマトリックス

				// パーツのワールドマトリックスの初期化
				D3DXMatrixIdentity(&g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotModel, g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.y, g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.x, g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot.z);
				D3DXMatrixMultiply(&g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld, &g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld, &mtxRotModel);

				// パーツの位置を反映
				D3DXMatrixTranslation(&mtxTransModel, g_aSeaweed[nCntSeaweed].aModel[nCntModel].pos.x, g_aSeaweed[nCntSeaweed].aModel[nCntModel].pos.y, g_aSeaweed[nCntSeaweed].aModel[nCntModel].pos.z);
				D3DXMatrixMultiply(&g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld, &g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld, &mtxTransModel);

				// パーツの「親のマトリックス」を設定
				if (g_aSeaweed[nCntSeaweed].aModel[nCntModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = g_aSeaweed[nCntSeaweed].aModel[g_aSeaweed[nCntSeaweed].aModel[nCntModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = g_aSeaweed[nCntSeaweed].mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
				D3DXMatrixMultiply(&g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld,
					&g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)g_SeaweedModel[g_aSeaweed[nCntSeaweed].aModel[nCntModel].type].pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)g_SeaweedModel[g_aSeaweed[nCntSeaweed].aModel[nCntModel].type].dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, g_SeaweedModel[g_aSeaweed[nCntSeaweed].aModel[nCntModel].type].apTexture[nCntMat]);

					// モデルパーツの描画
					g_SeaweedModel[g_aSeaweed[nCntSeaweed].aModel[nCntModel].type].pMesh->DrawSubset(nCntMat);
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
	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++)
	{
		if (g_aSeaweed[nCntSeaweed].bUse == false)
		{// 使用していない
			g_aSeaweed[nCntSeaweed].pos = pos;
			g_aSeaweed[nCntSeaweed].rot = D3DXVECTOR3(0.0f, (float)(rand() % 629 - 314) / 1000.0f, 0.0f);
			g_aSeaweed[nCntSeaweed].bUse = true;
			g_aSeaweed[nCntSeaweed].nNumModel = nLength;

			for (int nCntModel = 0; nCntModel < nLength; nCntModel++)
			{
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].nIdx = nCntModel;
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].nIdxModelParent = nCntModel - 1;

				if (nCntModel == 0)
				{// 親
					g_aSeaweed[nCntSeaweed].aModel[nCntModel].pos = FIRST_POS;
					g_aSeaweed[nCntSeaweed].aModel[nCntModel].posOff = FIRST_POS;
				}
				else
				{// 親以外
					g_aSeaweed[nCntSeaweed].aModel[nCntModel].pos = D3DXVECTOR3(0.0f, SEAWEED_HEIGHT, 0.0f);
					g_aSeaweed[nCntSeaweed].aModel[nCntModel].posOff = D3DXVECTOR3(0.0f, SEAWEED_HEIGHT, 0.0f);
				}

				D3DXVECTOR3 rot = FIRST_POS;
				rot.y = (float)(rand() % 629 - 314) / 1000.0f;

				g_aSeaweed[nCntSeaweed].aModel[nCntModel].rot = rot;
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotOff = rot;
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest = FIRST_POS;

				if (nCntModel != nLength - 1)
				{// 最後以外
					g_aSeaweed[nCntSeaweed].aModel[nCntModel].type = SEAWEEDTYPE_BOTTOM;
				}
				else
				{// 最後
					g_aSeaweed[nCntSeaweed].aModel[nCntModel].type = SEAWEEDTYPE_TOP;
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

		pos.x = sinf(fAngle) * (OUTCYLINDER_RADIUS + (((float)(rand() % (int)((OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) / 2) + 1))));
		pos.y = -20.0f;
		pos.z = cosf(fAngle) * (OUTCYLINDER_RADIUS + (((float)(rand() % (int)((OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) / 2) + 1))));

		int nLength = rand() % 3 + 2;

		SetSeaweed(pos, nLength);
	}
}

//=============================================================================
// 海藻の当たり判定
//=============================================================================
void CollisionSeaweed(D3DXVECTOR3 pos)
{
	for (int nCntSeaweed = 0; nCntSeaweed < MAX_SEAWEED; nCntSeaweed++)
	{
		if (g_aSeaweed[nCntSeaweed].bUse == false)
		{// 使用してないとき
			continue;
		}

		for (int nCntModel = 0; nCntModel < g_aSeaweed[nCntSeaweed].nNumModel; nCntModel++)
		{
			D3DXVECTOR3 SeaweedPos = D3DXVECTOR3(g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld._41,
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld._42, g_aSeaweed[nCntSeaweed].aModel[nCntModel].mtxWorld._43);
			D3DXVECTOR3 dist = SeaweedPos - pos;

			if (D3DXVec3Length(&dist) < SEAWEED_DIST)
			{// 近い
				D3DXVec3Normalize(&dist, &dist);

				g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.x = sinf(dist.z);
				g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.z = cosf(dist.x);

				if (nCntModel != g_aSeaweed[nCntSeaweed].nNumModel - 1)
				{// 最後じゃないとき
					g_aSeaweed[nCntSeaweed].aModel[nCntModel + 1].rotDest.x = -g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.x;
					g_aSeaweed[nCntSeaweed].aModel[nCntModel + 1].rotDest.z = -g_aSeaweed[nCntSeaweed].aModel[nCntModel].rotDest.z;
				}

				break;
			}
		}
	}
}