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
#define FISHES_MOVEMENT			(D3DXVECTOR3(3.0f, 1.5f, 3.0f))			// 移動量
#define FISHES_ROT				(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define FISHES_INERTIA_MOVE		(0.2f)									// 移動の慣性
#define FISHES_MAX_MOVE			(5.0f)									// 移動の制限
#define FISHES_INERTIA_ANGLE	(0.1f)									// 角度の慣性
#define FISHES_WIDTH			(5.0f)									// 幅
#define FISHES_HEIGHT			(10.0f)									// 高さ
#define FISHES_XMODEL_FILENAME	"data\\motion_octo.txt"					// 生き物のデータファイル
#define FISHES_MAX_NUM			(1024)									// 設置の最大数
#define FISHES_MAX_MODELS		(100)									// 読み込めるモデルの最大数

//*****************************************************************************
// 生き物のモデル情報
//*****************************************************************************
typedef struct
{
	int Model_Idx[FISHES_MAX_MODELS];
	char Model_FileName[FISHES_MAX_MODELS];
}FishesModelInfo;

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
	int NumScanModel = 0;

	// randのランダム化
	srand((unsigned int)time(NULL));

	// 生き物の情報の初期化
	for (int nCntFishes = 0; nCntFishes < FISHES_MAX_NUM; nCntFishes++, pFishes++)
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
		pFishes[nCntFishes].StopTime = 0;
		pFishes[nCntFishes].bMoving = false;
	}
	/*
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
	*/
}

//=============================================================================
// 生き物の終了処理
//=============================================================================
void UninitFishes(void)
{
	// ローカル変数宣言
	Fishes* pFishes = GetFishes();

	for (int nCntModel = 0; nCntModel < FISHES_MAX_NUM; nCntModel++, pFishes++)
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
	// ローカル変数宣言
	Fishes* pFishes = GetFishes();
	FISHESSTATE OldState = FISHESSTATE_STOP;

	for (int nCntFishes = 0; nCntFishes < FISHES_MAX_NUM; nCntFishes++, pFishes++)
	{
		if (pFishes[nCntFishes].bUse == true && pFishes[nCntFishes].bMoving == true)
		{
			// 今の時点のstateを記録
			OldState = pFishes[nCntFishes].state;
			pFishes[nCntFishes].posOld = pFishes[nCntFishes].pos;

			// それぞれのフラグ増加
			if (pFishes[nCntFishes].state == FISHESSTATE_MOVE)
			{
				// 移動
				pFishes[nCntFishes].move.x += sinf(pFishes[nCntFishes].fAngle) * FISHES_MOVEMENT.x;
				pFishes[nCntFishes].move.z += cosf(pFishes[nCntFishes].fAngle) * FISHES_MOVEMENT.z;

				pFishes[nCntFishes].pos += pFishes[nCntFishes].move;

				// 移動慣性
				pFishes[nCntFishes].move.x += (0.0f - pFishes[nCntFishes].move.x) * FISHES_INERTIA_MOVE;
				pFishes[nCntFishes].move.z += (0.0f - pFishes[nCntFishes].move.z) * FISHES_INERTIA_MOVE;


				if (pFishes[nCntFishes].rot.y != pFishes[nCntFishes].fAngle)
				{// 目標地点につくまで慣性で角度を足す
					pFishes[nCntFishes].rot.y += (pFishes[nCntFishes].fAngle - pFishes[nCntFishes].rot.y) * FISHES_INERTIA_ANGLE;

					// 向きを調整
					CorrectAngle(&pFishes[nCntFishes].rot.y, pFishes[nCntFishes].rot.y);
				}

				pFishes[nCntFishes].nCounterState++;
			}
			else if (pFishes[nCntFishes].state == FISHESSTATE_STOP)
			{
				pFishes[nCntFishes].nCounterState++;
			}

			// 生き物の状態遷移
			if (pFishes[nCntFishes].MoveTime < pFishes[nCntFishes].nCounterState)
			{
				pFishes[nCntFishes].state = FISHESSTATE_STOP;
				pFishes[nCntFishes].nCounterState = 0;
			}
			else if (pFishes[nCntFishes].StopTime < pFishes[nCntFishes].nCounterState)
			{
				pFishes[nCntFishes].state = FISHESSTATE_MOVE;
				pFishes[nCntFishes].nCounterState = 0;
			}

			// stopからmoveに移行するとき数値を設定(ランダム)
			if (OldState == FISHESSTATE_STOP && OldState == pFishes[nCntFishes].state)
			{
				pFishes[nCntFishes].MoveTime = rand() % (60 * 8) + 1;				// 移動する時間
				pFishes[nCntFishes].fAngle = ((rand() % 628) - 314) * 100.0f;		// 移動する角度(ｙ軸)
				pFishes[nCntFishes].StopTime = rand() % (60 * 4) + 1;				// 停止している時間
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

	for (int nCntModel = 0; nCntModel < FISHES_MAX_NUM; nCntModel++, pFishes++)
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

			if (pFishes[nCntModel].bUse = true)
			{
				// テクスチャの設定
				pDevice->SetTexture(0, pFishes->aModel[nCntModel].apTexture[nCntMat]);

				// 生き物パーツの描画
				pFishes->aModel[nCntModel].pMesh->DrawSubset(nCntMat);
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
// ファイルスキャン
//=============================================================================
void ScanFile_Fishes(char* FileName)
{/*
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスの取得
	FILE* pFile;
	char afileStr[256];
	char NumStr[256];
	int nWordEnd = 0;
	bool bScanModel = false;
	bool bScript = false;
	
	// ファイル開き
	pFile = fopen(FileName, "r");

	if (pFile != NULL)
	{ // 開いたとき
		while (1)
		{
			// ファイル読み込み
			fgets(&afileStr[0], 100, pFile);

			if (kStrcmp(&afileStr[0], "SCAN_MODEL", &nWordEnd) == true)
			{
				bScanModel = true;

			}
			while (bScanModel)
			{
				//ファイル読み込み
				fgets(&afileStr[0], 100, pFile);

				if (kStrcmp(&afileStr[0], "SCRIPT", &nWordEnd) == true)
				{
					bScript = true;

				}
				while (bScript)
				{
					//ファイル読み込み
					fgets(&afileStr[0], 100, pFile);

					if (kStrcmp(&afileStr[0], "NUM_MODEL", &nWordEnd) == true)
					{
						if (kStrcmp(&afileStr[nWordEnd], " = ", &nWordEnd) == true)
						{
							//文字列代入
							kStrPrint(&afileStr[nWordEnd], &NumStr[0], &nWordEnd);
							//変換
							pPlayer->nNumModel = atoi(NumStr);
							nWordEnd = 0;
							bNumModel = true;
						}
					}
					if (bNumModel == true)
					{
						for (nCntModel = 0; nCntModel < pPlayer->nNumModel;)
						{
							//ファイル読み込み
							fgets(&afileStr[0], 100, pFile);

							if (kStrcmp(&afileStr[0], "MODEL_FILENAME", &nWordEnd) == true)
							{
							}
						}
					}
				}
			}
		}
		// ファイル閉じ
		fclose(pFile);

	}*/
}

//=========================================
// 文字判定処理
//=========================================
bool kStrcmp(const char* aStr, const char* aCmpStr, int* nWE)
{
	for (; *aStr != '\0'; aStr++)
	{
		*nWE += 1;
		if (*aStr == *aCmpStr)
		{ // SCRIPT
			aCmpStr++;
			if (*aCmpStr == '\0')
			{
				return true;
			}
		}
		else if (*aStr == '#')
		{
			*nWE = 0;
			return false;
		}
	}
	*nWE = 0;
	return false;
}

//=========================================
// 文字代入処理
//=========================================
void kStrPrint(const char* aStr, char* aPrintStr, int* nWE)
{
	for (; *aStr != '\0'; aStr++)
	{
		*nWE += 1;
		if (*aStr == '#')
		{
			break;
		}
		else if (*aStr == '\t')
		{
			break;
		}
		else if (*aStr == ' ')
		{
			break;
		}
		else if (*aStr == '\n')
		{
			break;
		}
		else
		{
			sprintf(aPrintStr, "%c", *aStr);
			aPrintStr++;
		}
	}
	aStr++;
}