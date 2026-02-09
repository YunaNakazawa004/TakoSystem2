//=============================================================================
// 
// タコつぼ [pot.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "pot.h"
#include "debugproc.h"
#include "input.h"
#include "esa.h"
#include "ui_esa.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_POTMODEL		(1)										// タコつぼモデルの最大数
#define POT_RADIUS			(50.0f)									// タコつぼ周辺の球判定の半径

//*****************************************************************************
// タコつぼモデルの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	  // テクスチャへのポインタ
	LPD3DXMESH pMesh;							  // マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						  // メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								  // マテリアルの数
	D3DXVECTOR3 VtxMax, VtxMin;					  // 最大値・最小値
}PotModel;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
PotModel g_aPotModel[MAX_POTMODEL];						// タコつぼモデルの情報
Pot g_aPot[MAX_POT];									// タコつぼの情報

//*****************************************************************************
// ファイル名
//*****************************************************************************
const char* c_apFilenamePot[MAX_POTMODEL] =
{
	"data\\MODEL\\Others_Object\\Takotsubo.x",
};

//=============================================================================
// タコつぼの初期化処理
//=============================================================================
void InitPot(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	int nNumVtx;			// 頂点数
	DWORD dwSizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;			// 頂点バッファへのポインタ

	// タコつぼの情報の初期化
	for (int nCntPot = 0; nCntPot < MAX_POT; nCntPot++)
	{
		g_aPot[nCntPot].pos = FIRST_POS;
		g_aPot[nCntPot].rot = FIRST_POS;
		g_aPot[nCntPot].nFood = 0;
		g_aPot[nCntPot].esaQueue.nTail = -1;
		memset(&g_aPot[nCntPot].esaQueue.nData, -1, sizeof(int[MAX_QUEUE]));
		g_aPot[nCntPot].bUse = false;
	}

	// タコつぼモデルの情報の初期化
	for (int nCntPot = 0; nCntPot < MAX_POTMODEL; nCntPot++)
	{
		// 最大値最小値の初期化
		g_aPotModel[nCntPot].VtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
		g_aPotModel[nCntPot].VtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

		// Xファイルの読み込み
		D3DXLoadMeshFromX(c_apFilenamePot[nCntPot],
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&g_aPotModel[nCntPot].pBuffMat,
			NULL,
			&g_aPotModel[nCntPot].dwNumMat,
			&g_aPotModel[nCntPot].pMesh);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)g_aPotModel[nCntPot].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aPotModel[nCntPot].dwNumMat; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aPotModel[nCntPot].apTexture[nCntMat]);
			}
		}

		// 頂点数を取得
		nNumVtx = g_aPotModel[nCntPot].pMesh->GetNumVertices();

		// 頂点フォーマットのサイズを取得
		dwSizeFVF = D3DXGetFVFVertexSize(g_aPotModel[nCntPot].pMesh->GetFVF());

		// 頂点バッファをロック
		g_aPotModel[nCntPot].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;		// 頂点座標の代入

			if (vtx.x > g_aPotModel[nCntPot].VtxMax.x)
			{// Xの最大値
				g_aPotModel[nCntPot].VtxMax.x = vtx.x;
			}

			if (vtx.x < g_aPotModel[nCntPot].VtxMin.x)
			{// Xの最小値
				g_aPotModel[nCntPot].VtxMin.x = vtx.x;
			}

			if (vtx.y > g_aPotModel[nCntPot].VtxMax.y)
			{// Yの最大値
				g_aPotModel[nCntPot].VtxMax.y = vtx.y;
			}

			if (vtx.y < g_aPotModel[nCntPot].VtxMin.y)
			{// Yの最小値
				g_aPotModel[nCntPot].VtxMin.y = vtx.y;
			}

			if (vtx.z > g_aPotModel[nCntPot].VtxMax.z)
			{// Zの最大値
				g_aPotModel[nCntPot].VtxMax.z = vtx.z;
			}

			if (vtx.z < g_aPotModel[nCntPot].VtxMin.z)
			{// Zの最小値
				g_aPotModel[nCntPot].VtxMin.z = vtx.z;
			}

			pVtxBuff += dwSizeFVF;		// 頂点バッファのサイズ分ポインタを進める
		}

		// 頂点バッファをアンロック
		g_aPotModel[nCntPot].pMesh->UnlockVertexBuffer();
	}

	// ランダムな位置に設定
	SetRandomPot(1);
}

//=============================================================================
// タコつぼの終了処理
//=============================================================================
void UninitPot(void)
{
	for (int nCntPotModel = 0; nCntPotModel < MAX_POTMODEL; nCntPotModel++)
	{
		// メッシュの破棄
		if (g_aPotModel[nCntPotModel].pMesh != NULL)
		{
			g_aPotModel[nCntPotModel].pMesh->Release();
			g_aPotModel[nCntPotModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (g_aPotModel[nCntPotModel].pBuffMat != NULL)
		{
			g_aPotModel[nCntPotModel].pBuffMat->Release();
			g_aPotModel[nCntPotModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntPot = 0; nCntPot < (int)g_aPotModel[nCntPotModel].dwNumMat; nCntPot++)
		{
			if (g_aPotModel[nCntPotModel].apTexture[nCntPot] != NULL)
			{
				g_aPotModel[nCntPotModel].apTexture[nCntPot]->Release();
				g_aPotModel[nCntPotModel].apTexture[nCntPot] = NULL;
			}
		}
	}
}

//=============================================================================
// タコつぼの更新処理
//=============================================================================
void UpdatePot(void)
{
	Pot* pPot = GetPot();

	for (int nCntPot = 0; nCntPot < MAX_POT; nCntPot++, pPot++)
	{
		if (pPot->bUse == true)
		{
			//PrintDebugProc("[ %d ]\n入ってるエサの数 %d\n", nCntPot, g_aPot[nCntPot].nFood);
			int nIdx = -1;

			if (CollisionEsa(&nIdx, false, &pPot->pos, 0.0f) == true)
			{// エサと接触した
				Esa* pEsa = GetEsa();

				if (pEsa[nIdx].esaType == ESA_ACTTYPE_GOTO_POT)
				{// タコつぼに入れてる最中
					pEsa[nIdx].bUse = false;
				}
			}
		}
	}
}

//=============================================================================
// タコつぼの描画処理
//=============================================================================
void DrawPot(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ

	for (int nCntPot = 0; nCntPot < MAX_POT; nCntPot++)
	{
		if (g_aPot[nCntPot].bUse == true)
		{// 使用されているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aPot[nCntPot].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aPot[nCntPot].rot.y, g_aPot[nCntPot].rot.x, g_aPot[nCntPot].rot.z);
			D3DXMatrixMultiply(&g_aPot[nCntPot].mtxWorld, &g_aPot[nCntPot].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aPot[nCntPot].pos.x, g_aPot[nCntPot].pos.y, g_aPot[nCntPot].pos.z);
			D3DXMatrixMultiply(&g_aPot[nCntPot].mtxWorld, &g_aPot[nCntPot].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aPot[nCntPot].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_aPotModel[0].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aPotModel[0].dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_aPotModel[0].apTexture[nCntMat]);

				// モデルパーツの描画
				g_aPotModel[0].pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// タコつぼの設定処理
//=============================================================================
void SetPot(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	for (int nCntPot = 0; nCntPot < MAX_POT; nCntPot++)
	{
		if (g_aPot[nCntPot].bUse == false)
		{// タコつぼが使用されていない
			g_aPot[nCntPot].pos = pos;
			g_aPot[nCntPot].rot = rot;
			g_aPot[nCntPot].nFood = 0;
			g_aPot[nCntPot].esaQueue.nTail = -1;
			memset(&g_aPot[nCntPot].esaQueue.nData, -1, sizeof(int[MAX_QUEUE]));
			g_aPot[nCntPot].bUse = true;		// 使用している状態にする

			break;
		}
	}
}

//=============================================================================
// タコつぼのランダム設定処理
//=============================================================================
void SetRandomPot(int nAmount)
{
	for (int nCntPot = 0; nCntPot < nAmount; nCntPot++)
	{
		D3DXVECTOR3 pos;
		float fAngle = (D3DX_PI * 2.0f) * ((float)((nCntPot + 1) * (360.0f / nAmount)) / 360.0f);
		float fsin = sinf(fAngle);

		pos.x = sinf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));
		pos.y = 0.0f;
		pos.z = cosf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));

		SetPot(pos, FIRST_POS);
	}
}

//=============================================================================
// タコつぼの取得
//=============================================================================
Pot* GetPot(void)
{
	return &g_aPot[0];
}

//=============================================================================
// タコつぼの当たり判定
//=============================================================================
bool CollisionPot(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fRadius, float fHeight)
{
	bool bLand = false;		// 着地しているか

	for (int nCntPot = 0; nCntPot < MAX_POT; nCntPot++)
	{
		if (g_aPot[nCntPot].bUse == true)
		{// 使用されているとき
			if ((pPos->x + fRadius > g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMin.x) &&
				(pPos->x - fRadius < g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMax.x) &&
				(g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMin.z - fRadius <= pPos->z) &&
				(g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMax.z + fRadius >= pPos->z) &&
				(g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMin.y - fHeight <= pPos->y) &&
				(g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMax.y >= pPos->y))
			{// 現在の位置がタコつぼの範囲内
				if (((g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMin.x - fRadius == pPosOld->x) &&
					(g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMax.x + fRadius > pPosOld->x)) ||
					((g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMax.x + fRadius == pPosOld->x) &&
						(g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMin.x - fRadius < pPosOld->x)))
				{// 前回の位置がタコつぼの位置なら奥行判定しない

				}
				else if (((g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMax.z + fRadius <= pPosOld->z) &&
					(g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMax.z + fRadius >= pPos->z)))
				{// 奥からの当たり判定
					pPos->z = g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMax.z + fRadius;
					pMove->z = 0.0f;
				}
				else if (((g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMin.z - fRadius >= pPosOld->z) &&
					(g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMin.z - fRadius <= pPos->z)))
				{// 手前からの当たり判定
					pPos->z = g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMin.z - fRadius;
					pMove->z = 0.0f;
				}

				if (((g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMin.z - fRadius == pPosOld->z) &&
					(g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMax.z + fRadius > pPosOld->z)) ||
					((g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMax.z + fRadius == pPosOld->z) &&
						(g_aPot[nCntPot].pos.z + g_aPotModel[0].VtxMin.z - fRadius < pPosOld->z)))
				{// 前回の位置がタコつぼの位置なら左右判定しない

				}
				else if ((g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMin.x - fRadius >= pPosOld->x) &&
					(g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMin.x - fRadius <= pPos->x))
				{// 左からの当たり判定
					pPos->x = g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMin.x - fRadius;
					pMove->x = 0.0f;							// 移動量を0にする
				}
				else if ((g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMax.x + fRadius <= pPosOld->x) &&
					(g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMax.x + fRadius >= pPos->x))
				{// 右からの当たり判定
					pPos->x = g_aPot[nCntPot].pos.x + g_aPotModel[0].VtxMax.x + fRadius;
					pMove->x = 0.0f;							// 移動量を0にする
				}
				else if ((g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMin.y - fHeight >= pPosOld->y) &&
					(g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMin.y - fHeight <= pPos->y))
				{// 下からの当たり判定
					pPos->y = g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMin.y - fHeight;
					pMove->y = -0.5f;							// 移動量を0にする
				}
				else if ((g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMax.y <= pPosOld->y) &&
					(g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMax.y >= pPos->y))
				{// 上からの当たり判定
					pPos->y = g_aPot[nCntPot].pos.y + g_aPotModel[0].VtxMax.y;
					pMove->y = 0.0f;							// 移動量を0にする

					bLand = true;
				}
			}
		}
	}

	return bLand;
}

//=============================================================================
// タコつぼの周辺当たり判定(エサ関係用)
//=============================================================================
bool CollisionPotArea(D3DXVECTOR3 pos, float fRadius, Player* pPlayer, Computer* pComputer, bool bTentacle)
{
	Pot* pPot = GetPot();
	bool bColl = false;

	for (int nCntPot = 0; nCntPot < MAX_POT; nCntPot++, pPot++)
	{
		if (pPot->bUse == false)
		{// 使ってないのは無視
			continue;
		}

		D3DXVECTOR3 dist = pPot->pos - pos;

		if (D3DXVec3Length(&dist) < POT_RADIUS + fRadius)
		{// 距離が当たり判定内
			bColl = true;

			if (pPlayer != NULL)
			{// プレイヤーの判定
				if ((pPot->nFood == 0 && pPlayer->Potstate == POTSTATE_NONE) || pPlayer->Potstate == POTSTATE_HIDE)
				{// 中身が空
					if (bTentacle == false && pPlayer->nFood > 0)
					{// 触手じゃない
						pPlayer->Potstate = POTSTATE_HIDE;

						int nIdx = Dequeue(&pPlayer->esaQueue);
						pPlayer->nFood--;
						pPlayer->nPotIdx = nCntPot;
						SetSubUiEsa(pPlayer->nIdx);
						SetEsa(nIdx, ESA_ACTTYPE_GOTO_POT, 0, pPlayer->pos, FIRST_POS);

						Enqueue(&pPot->esaQueue, nIdx);
						pPot->nFood++;
					}
				}
				else if (pPot->nFood > 0 && pPlayer->Potstate != POTSTATE_HIDE)
				{// 中身がある
					if (bTentacle == true)
					{// 触手
						pPlayer->Potstate = POTSTATE_STEAL;
						pPlayer->nPotIdx = nCntPot;
						int nFood = pPot->nFood;

						for (int nCnt = 0; nCnt < nFood; nCnt++)
						{
							if (pPlayer->nFood < pPlayer->nMaxFood * 8)
							{// 持てる数だけ持つ
								int nIdx = Dequeue(&pPot->esaQueue);
								pPot->nFood--;
								SetAddUiEsa(pPlayer->nIdx, nIdx);
								SetEsa(nIdx, ESA_ACTTYPE_GOTO_PLAYER, 0, pPot->pos, FIRST_POS);

								Enqueue(&pPlayer->esaQueue, nIdx);
								pPlayer->nFood++;
							}
						}
					}
					else if (bTentacle == false)
					{// もし触手を伸ばしてない
						SetVibration(pPlayer->nIdx, 300, 300, 1);
					}
				}
			}
			else if (pComputer != NULL)
			{// CPUの判定
				if ((pPot->nFood == 0 && pComputer->Potstate == POTSTATE_NONE) || pComputer->Potstate == POTSTATE_HIDE)
				{// 中身が空
					if (bTentacle == false && pComputer->nFoodCount > 0)
					{// 触手じゃない
						pComputer->Potstate = POTSTATE_HIDE;

						int nIdx = Dequeue(&pComputer->esaQueue);
						pComputer->nFoodCount--;

						Enqueue(&pPot->esaQueue, nIdx);
						pPot->nFood++;
					}
				}
				else if (pPot->nFood > 0 && pComputer->Potstate != POTSTATE_HIDE)
				{// 中身がある
					if (bTentacle == true)
					{// 触手
						pComputer->Potstate = POTSTATE_STEAL;
						int nFood = pPot->nFood;

						for (int nCnt = 0; nCnt < nFood; nCnt++)
						{
							if (pComputer->nFoodCount < pComputer->nMaxFood * 8)
							{// 持てる数だけ持つ
								int nIdx = Dequeue(&pPot->esaQueue);
								pPot->nFood--;

								Enqueue(&pComputer->esaQueue, nIdx);
								pComputer->nFoodCount++;
							}
						}
					}
				}
			}

			return bColl;
		}
		else
		{// 離れた
			if (pPlayer != NULL && pPlayer->nPotIdx == nCntPot)
			{
				pPlayer->Potstate = POTSTATE_NONE;
				pPlayer->nPotIdx = -1;
			}
			else if (pComputer != NULL && pComputer->nTargetPotIdx == nCntPot)
			{
				pComputer->Potstate = POTSTATE_NONE;
			}

			bColl = false;
		}
	}

	return bColl;
}