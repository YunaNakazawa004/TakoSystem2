//=============================================================================
// 
// 生き物 [fishes.cpp]
// Author : Mutsuki Uemura
// 
//=============================================================================
#include "fishes.h"

#include "camera.h"
#include "input.h"
#include <time.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FISHES_MOVEMENT			(D3DXVECTOR3(3.0f, 1.5f, 3.0f))			// 移動量
#define FISHES_ROT				(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define FISHES_INERTIA_MOVE		(0.2f)									// 移動の慣性
#define FISHES_MAX_MOVE			(5.0f)									// 移動の制限
#define FISHES_INERTIA_ANGLE	(0.1f)									// 角度の慣性
#define FISHES_WIDTH			(5.0f)									// 幅
#define FISHES_HEIGHT			(10.0f)									// 高さ
#define FISHES_XMODEL_FILENAME	"data\\motion_octo.txt"					// 生き物のデータファイル
#define MAX_FISHES				(1)										// 置ける最大数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Fishes g_aFishes[256];									// 生き物の情報
char* g_apFilenameFishes[MAX_NUMMODEL] = {};			// モデルファイルへのポインタ

//=============================================================================
// 生き物の初期化処理
//=============================================================================
void InitFishes(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	Fishes* pFishes = GetFishes();

	srand((unsigned int)time(NULL));

	// 生き物の情報の初期化
	for (int nCntFishes = 0; nCntFishes < MAX_FISHES; nCntFishes++, pFishes++)
	{
		pFishes[nCntFishes].pos = FIRST_POS;
		pFishes[nCntFishes].posOld = FIRST_POS;
		pFishes[nCntFishes].move = FIRST_POS;
		pFishes[nCntFishes].rot = FIRST_POS;
		pFishes[nCntFishes].state = FISHESSTATE_STOP;
		pFishes[nCntFishes].nCounterState = 0;
		pFishes[nCntFishes].fAngle = 0.0f;
		pFishes[nCntFishes].fRadius = FISHES_WIDTH;
		pFishes[nCntFishes].fHeight = FISHES_HEIGHT;
		pFishes[nCntFishes].bMove = false;
		pFishes[nCntFishes].bUse = false;
		pFishes[nCntFishes].MoveTime = 0;
		pFishes[nCntFishes].MoveRot = 0.0f;
		pFishes[nCntFishes].StopTime = 0;
		pFishes[nCntFishes].bMoving = false;

		// Xファイルの読み込み
		D3DXLoadMeshFromX(FISHES_XMODEL_FILENAME,
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&pFishes->aModel[nCntFishes].pBuffMat,
			NULL,
			&pFishes->aModel[nCntFishes].dwNumMat,
			&pFishes->aModel[nCntFishes].pMesh);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pFishes->aModel[nCntFishes].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pFishes->aModel[nCntFishes].dwNumMat; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &pFishes->aModel[nCntFishes].apTexture[nCntMat]);
			}
		}
	}
}

//=============================================================================
// 生き物の終了処理
//=============================================================================
void UninitFishes(void)
{
	Fishes* pFishes = GetFishes();

	for (int nCntModel = 0; nCntModel < MAX_FISHES; nCntModel++, pFishes++)
	{
		// メッシュの破棄
		if (pFishes->aModel[nCntModel].pMesh != NULL)
		{
			pFishes->aModel[nCntModel].pMesh->Release();
			pFishes->aModel[nCntModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (pFishes->aModel[nCntModel].pBuffMat != NULL)
		{
			pFishes->aModel[nCntModel].pBuffMat->Release();
			pFishes->aModel[nCntModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntFishes = 0; nCntFishes < (int)pFishes->aModel[0].dwNumMat; nCntFishes++)
		{
			if (pFishes->aModel[nCntModel].apTexture[nCntFishes] != NULL)
			{
				pFishes->aModel[nCntModel].apTexture[nCntFishes]->Release();
				pFishes->aModel[nCntModel].apTexture[nCntFishes] = NULL;
			}
		}
	}
}

//=============================================================================
// 生き物の更新処理
//=============================================================================
void UpdateFishes(void)
{
	Fishes* pFishes = GetFishes();
	FISHESSTATE OldState = FISHESSTATE_STOP;
	static int nCntStop = 0, nCntMove = 0;

	// 追加予定
	// 1 最初に進む時間 角度 止まる時間を設定する
	for (int nCntFishes = 0; nCntFishes < MAX_FISHES; nCntFishes++, pFishes++)
	{
		if (pFishes[nCntFishes].bUse == true && pFishes[nCntFishes].bMoving == true)
		{
			//今の時点のstateを記録
			OldState = pFishes[nCntFishes].state;

			//それぞれのフラグ増加
			if (pFishes[nCntFishes].state == FISHESSTATE_STOP)
			{
				nCntStop++;
			}
			else if (pFishes[nCntFishes].state == FISHESSTATE_MOVE)
			{
				nCntMove++;
			}

			//生き物の状態遷移
			if (pFishes[nCntFishes].MoveTime < nCntMove)
			{
				pFishes[nCntFishes].state = FISHESSTATE_STOP;
				nCntMove = 0;
			}
			else if (pFishes[nCntFishes].StopTime < nCntStop)
			{
				pFishes[nCntFishes].state = FISHESSTATE_MOVE;
				nCntStop = 0;
			}

			//stopからmoveに移行するとき数値を設定(ランダム)
			if (OldState == FISHESSTATE_STOP && OldState == pFishes[nCntFishes].state)
			{
				pFishes[nCntFishes].MoveTime = rand() % (60 * 8) + 1;			//移動する時間
				pFishes[nCntFishes].MoveRot = ((rand() % 628) - 314) * 100.0f;	//移動する角度(ｙ軸)
				pFishes[nCntFishes].StopTime = rand() % (60 * 4) + 1;			//停止している時間
			}
		}
	}
}

//=============================================================================
// 生き物の描画処理
//=============================================================================
void DrawFishes(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;					// 計算用マトリックス
	D3DMATERIAL9 matDef;							// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;								// マテリアルデータへのポインタ
	Fishes* pFishes = GetFishes();

	for (int nCntModel = 0; nCntModel < MAX_FISHES; nCntModel++, pFishes++)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pFishes->mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pFishes->rot.y, pFishes->rot.x, pFishes->rot.z);
		D3DXMatrixMultiply(&pFishes->mtxWorld, &pFishes->mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pFishes->pos.x, pFishes->pos.y, pFishes->pos.z);
		D3DXMatrixMultiply(&pFishes->mtxWorld, &pFishes->mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pFishes->mtxWorld);

		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pFishes->aModel[nCntModel].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pFishes->aModel[nCntModel].dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pFishes->aModel[nCntModel].apTexture[nCntMat]);

			// 生き物パーツの描画
			pFishes->aModel[nCntModel].pMesh->DrawSubset(nCntMat);
		}

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//=============================================================================
// 生き物の当たり判定
//=============================================================================
void CollisionFishes(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fWidth, float fDepth)
{
	Fishes* pFishes = GetFishes();

	if ((pPos->x + fWidth > pFishes[0].pos.x - FISHES_WIDTH) &&
		(pPos->x - fWidth < pFishes[0].pos.x + FISHES_WIDTH) &&
		(pFishes[0].pos.z - FISHES_HEIGHT <= pPos->z) &&
		(pFishes[0].pos.z + FISHES_HEIGHT + fDepth >= pPos->z))
	{// 現在の位置がブロックの範囲内
		if (((pFishes[0].pos.z + FISHES_HEIGHT <= pPosOld->z) &&
			(pFishes[0].pos.z + FISHES_HEIGHT >= pPos->z)))
		{// 奥からの当たり判定
			pPos->z = pFishes[0].pos.z + FISHES_HEIGHT;
		}
		else if (((pFishes[0].pos.z - FISHES_HEIGHT + fDepth >= pPosOld->z) &&
			(pFishes[0].pos.z - FISHES_HEIGHT + fDepth <= pPos->z)))
		{// 手前からの当たり判定
			pPos->z = pFishes[0].pos.z - FISHES_HEIGHT + fDepth;
		}
		else if ((pFishes[0].pos.x - FISHES_WIDTH - fWidth >= pPosOld->x) &&
			(pFishes[0].pos.x - FISHES_WIDTH - fWidth <= pPos->x))
		{// 左からの当たり判定
			pPos->x = pFishes[0].pos.x - FISHES_WIDTH - fWidth;
			pMove->x = 0.0f;							// 移動量を0にする
		}
		else if ((pFishes[0].pos.x + FISHES_WIDTH + fWidth <= pPosOld->x) &&
			(pFishes[0].pos.x + FISHES_WIDTH + fWidth >= pPos->x))
		{// 右からの当たり判定
			pPos->x = pFishes[0].pos.x + FISHES_WIDTH + fWidth;
			pMove->x = 0.0f;							// 移動量を0にする
		}
	}
}

//=============================================================================
// 生き物の取得処理
//=============================================================================
Fishes* GetFishes(void)
{
	return &g_aFishes[0];
}
