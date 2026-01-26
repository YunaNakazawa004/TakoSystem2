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
#include <stdio.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FISHES_MOVEMENT			(D3DXVECTOR3(0.4f, 0.4f, 0.4f))			// 移動量
#define FISHES_ROT				(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define FISHES_INERTIA_MOVE		(0.2f)									// 移動の慣性
#define FISHES_MAX_MOVE			(5.0f)									// 移動の制限
#define FISHES_INERTIA_ANGLE	(0.05f)									// 角度の慣性
#define FISHES_WIDTH			(5.0f)									// 幅
#define FISHES_HEIGHT			(10.0f)									// 高さ
#define FISHES_XMODEL_FILENAME	"data\\motion_octo.txt"					// 生き物のデータファイル
#define FISHES_MAX_NUM			(1024)									// 設置の最大数
#define FISHES_MAX_MODELS		(100)									// 読み込めるモデルの最大数
#define FISHES_CALC_SIZEARRAY(aArray)(sizeof aArray / sizeof(aArray[0]))

//*****************************************************************************
// 生き物のモデル情報
//*****************************************************************************
typedef struct
{
	int Model_Idx;
	char Model_FileName[FISHES_MAX_MODELS];
}FishesInfo;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Fishes g_aFishes[FISHES_MAX_NUM];						// 生き物の情報
Fishes_Model g_aFishesModel[FISHES_MAX_MODELS];			// 生き物のモデル情報
char* g_apFilenameFishes[MAX_NUMMODEL] = {};			// モデルファイルへのポインタ

FishesInfo g_aFishInfo[] =
{
	{0,"data\\MODEL\\octo.x"},
	{1,"data\\MODEL\\testmodel\\skitree000.x"},

};

//=============================================================================
// 生き物の初期化処理
//=============================================================================
void InitFishes(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	Fishes* pFishes = GetFishes();
	Fishes_Model* pFishesModel = &g_aFishesModel[0];
	FishesInfo* pFishesInfo = &g_aFishInfo[0];
	int NumScanModel = 0;

	// randのランダム化
	srand((unsigned int)time(NULL));

	// 生き物の情報の初期化
	for (int nCntFishes = 0; nCntFishes < FISHES_MAX_NUM; nCntFishes++, pFishes++)
	{
		pFishes->pos = FIRST_POS;
		pFishes->posOld = FIRST_POS;
		pFishes->move = FIRST_POS;
		pFishes->rot = FIRST_POS;
		pFishes->state = FISHESSTATE_STOP;
		pFishes->nCounterState = 0;
		pFishes->fAngle = 0.0f;
		pFishes->fRadius = FISHES_WIDTH;
		pFishes->fHeight = FISHES_HEIGHT;
		pFishes->bMove = false;
		pFishes->bUse = false;
		pFishes->MoveTime = 0;
		pFishes->StopTime = 0;
		pFishes->bMoving = false;
	}

	for (int nCntModel = 0; nCntModel < FISHES_CALC_SIZEARRAY(g_aFishInfo); nCntModel++, pFishesModel++)
	{
		// Xファイルの読み込み
		D3DXLoadMeshFromX(pFishesInfo[nCntModel].Model_FileName,
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&pFishesModel->pBuffMat,
			NULL,
			&pFishesModel->dwNumMat,
			&pFishesModel->pMesh);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pFishesModel->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pFishesModel->dwNumMat; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &pFishesModel->apTexture[nCntMat]);
			}
		}
	}
	// どのモデルをどれだけ呼び出すか
	SetFishes(0, 3);
	SetFishes(1, 0);

}

//=============================================================================
// 生き物の終了処理
//=============================================================================
void UninitFishes(void)
{
	// ローカル変数宣言
	Fishes* pFishes = GetFishes();
	Fishes_Model* pFishesModel = &g_aFishesModel[0];

	for (int nCntModel = 0; nCntModel < FISHES_MAX_MODELS; nCntModel++, pFishesModel++)
	{
		// メッシュの破棄
		if (pFishesModel->pMesh != NULL)
		{
			pFishesModel->pMesh->Release();
			pFishesModel->pMesh = NULL;
		}

		// マテリアルの破棄
		if (pFishesModel->pBuffMat != NULL)
		{
			pFishesModel->pBuffMat->Release();
			pFishesModel->pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntFishes = 0; nCntFishes < (int)pFishesModel->dwNumMat; nCntFishes++)
		{
			if (pFishesModel->apTexture[nCntFishes] != NULL)
			{
				pFishesModel->apTexture[nCntFishes]->Release();
				pFishesModel->apTexture[nCntFishes] = NULL;
			}
		}
	}
}

//=============================================================================
// 生き物の更新処理
//=============================================================================
void UpdateFishes(void)
{
	// ローカル変数宣言
	Fishes* pFishes = GetFishes();
	FISHESSTATE OldState = FISHESSTATE_STOP;
	float fmoveAngle = 0.0f;
	int Radian = 400;

	for (int nCntFishes = 0; nCntFishes < g_aFishes[0].nUseNum; nCntFishes++, pFishes++)
	{
		if (pFishes->bUse == true && pFishes->bMoving == true)
		{
			// 今の時点のstateを記録
			OldState = pFishes->state;
			pFishes->posOld = pFishes->pos;

			// それぞれのフラグ増加
			if (pFishes->state == FISHESSTATE_MOVE)
			{
				// 移動
				pFishes->move.x += sinf(pFishes->rot.y - D3DX_PI) * FISHES_MOVEMENT.x;
				pFishes->move.z += cosf(pFishes->rot.y - D3DX_PI) * FISHES_MOVEMENT.z;

				pFishes->pos += pFishes->move;

				// 移動慣性
				pFishes->move.x += (0.0f - pFishes->move.x) * FISHES_INERTIA_MOVE;
				pFishes->move.z += (0.0f - pFishes->move.z) * FISHES_INERTIA_MOVE;

				pFishes->nMoveCnt++;
			}
			else if (pFishes->state == FISHESSTATE_STOP)
			{
				pFishes->nStopCnt++;
			}

			fmoveAngle = pFishes->fAngle - pFishes->rot.y;

			// 向きを調整
			CorrectAngle(&pFishes->fAngle, fmoveAngle);

			if (pFishes->rot.y != pFishes->fAngle)

			{// 目標地点につくまで慣性で角度を足す
				pFishes->rot.y += (pFishes->fAngle - pFishes->rot.y) * FISHES_INERTIA_ANGLE;

				// 向きを調整
				CorrectAngle(&pFishes->rot.y, pFishes->rot.y);
			}

			// 生き物の状態遷移
			if (pFishes->MoveTime < pFishes->nMoveCnt)
			{
				pFishes->state = FISHESSTATE_STOP;
				pFishes->nMoveCnt = 0;
			}
			else if (pFishes->StopTime < pFishes->nStopCnt)
			{
				pFishes->state = FISHESSTATE_MOVE;
				pFishes->nStopCnt = 0;
			}

			// stopからmoveに移行するとき数値を設定(ランダム)
			if (OldState == FISHESSTATE_STOP && OldState != pFishes->state)
			{
				pFishes->MoveTime = 10;																		// 移動する時間
				pFishes->fAngle = pFishes->fAngle + ((rand() % Radian - (Radian / 2)) / 10 * 3.14f / 100);	// 移動する角度(ｙ軸)
				pFishes->StopTime = 0;																		// 停止している時間
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
	Fishes_Model* pFishesModel = &g_aFishesModel[0];

	for (int nCntFishes = 0; nCntFishes < g_aFishes[0].nUseNum; nCntFishes++, pFishes++)
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
		pMat = (D3DXMATERIAL*)pFishesModel[pFishes->nModelIdx].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pFishesModel[pFishes->nModelIdx].dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			if (pFishes->bUse == true)
			{
				// テクスチャの設定
				pDevice->SetTexture(0, pFishesModel[pFishes->nModelIdx].apTexture[nCntMat]);

				// 生き物パーツの描画
				pFishesModel[pFishes->nModelIdx].pMesh->DrawSubset(nCntMat);
			}
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
	// ローカル変数宣言
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

//=============================================================================
// 生き物の設定処理
//=============================================================================
void SetFishes(int ModelIdx, int nNumSet)
{
	// ローカル変数宣言
	Fishes* pFishes = GetFishes();
	int nModelSet = 0;

	for (int nCntFishes = 0; nCntFishes < FISHES_MAX_NUM; nCntFishes++, pFishes++)
	{
		// nNumSet分設定したら
		if (nModelSet == nNumSet)
		{
			nModelSet = 0;
			break;
		}
		if (pFishes->bUse == false)
		{
			pFishes->bUse = true;
			pFishes->nModelIdx = ModelIdx; 
			pFishes->bMoving = true;
			g_aFishes[0].nUseNum++;
			nModelSet++;
		}
	}
}
