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
#include "debugproc.h"

//=======================================
// マクロ定義 
//=======================================

#define FISHES_MOVEMENT					(D3DXVECTOR3(0.4f, 0.4f, 0.4f))		// 移動量
#define FISHES_ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))	// 向き移動量
#define FISHES_INERTIA_MOVE				(0.2f)								// 移動の慣性
#define FISHES_MAX_MOVE					(5.0f)								// 移動の制限
#define FISHES_INERTIA_ANGLE			(0.05f)								// 角度の慣性

//=======================================
// グローバル宣言
//=======================================

Fishes g_aFishes[FISHES_MAX_NUM];						// 生き物の情報
Fishes_Model g_aFishesModel[FISHES_MAX_MODELS];			// 生き物のモデル情報

// 生き物の状態 -------------------------

FishesInfo g_aFishInfo[] =
{ // [ モデルのインデックス,モデルのファイル名 ]

	{0,"data\\motion_octo_0.txt"},
	{1,"data\\motion_octo_1.txt"},

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
	Fishes_Model* pFishesModel = GetFishesModel();
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

		pFishes->motionType = MOTIONTYPE_NEUTRAL;
		pFishes->bLoopMotion = pFishes->aMotionInfo[0].bLoop;
		pFishes->nNumKey = pFishes->aMotionInfo[0].nNumKey;
		pFishes->nKey = 0;
		pFishes->nCounterMotion = 0;
		pFishes->bFinishMotion = false;
		pFishes->bBlendMotion = false;
		pFishes->motionTypeBlend = MOTIONTYPE_NEUTRAL;
		pFishes->bLoopMotionBlend = pFishes->aMotionInfo[0].bLoop;
		pFishes->nNumKeyBlend = pFishes->aMotionInfo[0].nNumKey;
		pFishes->nKeyBlend = 0;
		pFishes->nCounterMotionBlend = 0;
		pFishes->nFrameBlend = 0;
		pFishes->nCounterBlend = 0;
	}

	for (int nCntFishes = 0; nCntFishes < FISHES_CALC_SIZEARRAY(g_aFishInfo); nCntFishes++, pFishesModel++)
	{
		LoadFishes(nCntFishes);

		for (int nCntModel = 0; nCntModel < pFishesModel->nNumModel; nCntModel++)
		{
			// Xファイルの読み込み
			D3DXLoadMeshFromX(&pFishesModel->sModelFileName[nCntModel][0],
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				&pFishesModel->aModel[nCntModel].pBuffMat,
				NULL,
				&pFishesModel->aModel[nCntModel].dwNumMat,
				&pFishesModel->aModel[nCntModel].pMesh);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)pFishesModel->aModel[nCntModel].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)pFishesModel->aModel[nCntModel].dwNumMat; nCntMat++)
			{
				if (pMat[nCntMat].pTextureFilename != NULL)
				{// テクスチャファイルが存在する
					D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &pFishesModel->aModel[nCntModel].apTexture[nCntMat]);
				}
			}
		}
	}

	// どのモデルをどれだけ呼び出すか
	SetFishes(0, 0);
	SetFishes(1, 3);

	for (int nCntFishes = 0; nCntFishes < g_aFishes[0].nUseNum; nCntFishes++)
	{
		SetMotionFishes(nCntFishes, MOTIONTYPE_NEUTRAL, g_aFishes[nCntFishes].bBlendMotion, g_aFishes[nCntFishes].nFrameBlend);
	}

}

//=============================================================================
// 生き物の終了処理
//=============================================================================
void UninitFishes(void)
{
	// ローカル変数宣言
	Fishes* pFishes = GetFishes();
	Fishes_Model* pFishesModel = GetFishesModel();

	for (int nCntModel = 0; nCntModel < FISHES_CALC_SIZEARRAY(g_aFishInfo); nCntModel++, pFishesModel++)
	{
		// メッシュの破棄
		if (pFishesModel->aModel[nCntModel].pMesh != NULL)
		{
			pFishesModel->aModel[nCntModel].pMesh->Release();
			pFishesModel->aModel[nCntModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (pFishesModel->aModel[nCntModel].pBuffMat != NULL)
		{
			pFishesModel->aModel[nCntModel].pBuffMat->Release();
			pFishesModel->aModel[nCntModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntFishes = 0; nCntFishes < (int)pFishesModel->aModel[nCntModel].dwNumMat; nCntFishes++)
		{
			if (pFishesModel->aModel[nCntModel].apTexture[nCntFishes] != NULL)
			{
				pFishesModel->aModel[nCntModel].apTexture[nCntFishes]->Release();
				pFishesModel->aModel[nCntModel].apTexture[nCntFishes] = NULL;
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
	// モーションの更新処理
	UpdateMotionFishes();
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

	for (int nCntFishes = 0; nCntFishes < g_aFishes[0].nUseNum; nCntFishes++, pFishes++)
	{
		if (pFishes->bUse == true)
		{// 使用しているとき
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


			// 全モデル(パーツ)の描画
			for (int nCntModel = 0; nCntModel < pFishes->nNumModel; nCntModel++)
			{
				D3DXMATRIX mtxRotModel, mtxTransModel;		// 計算用マトリックス
				D3DXMATRIX mtxParent;						// 親のマトリックス

				// パーツのワールドマトリックスの初期化
				D3DXMatrixIdentity(&pFishes->aModel[nCntModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotModel, pFishes->aModel[nCntModel].rot.y, pFishes->aModel[nCntModel].rot.x, pFishes->aModel[nCntModel].rot.z);
				D3DXMatrixMultiply(&pFishes->aModel[nCntModel].mtxWorld, &pFishes->aModel[nCntModel].mtxWorld, &mtxRotModel);

				// パーツの位置を反映
				D3DXMatrixTranslation(&mtxTransModel, pFishes->aModel[nCntModel].pos.x, pFishes->aModel[nCntModel].pos.y, pFishes->aModel[nCntModel].pos.z);
				D3DXMatrixMultiply(&pFishes->aModel[nCntModel].mtxWorld, &pFishes->aModel[nCntModel].mtxWorld, &mtxTransModel);

				// パーツの「親のマトリックス」を設定
				if (pFishes->aModel[nCntModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = pFishes->aModel[pFishes->aModel[nCntModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = pFishes->mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
				D3DXMatrixMultiply(&pFishes->aModel[nCntModel].mtxWorld,
					&pFishes->aModel[nCntModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &pFishes->aModel[nCntModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pFishes->aModel[nCntModel].pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pFishes->aModel[nCntModel].dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pFishes->aModel[nCntModel].apTexture[nCntMat]);

					// モデルパーツの描画
					pFishes->aModel[nCntModel].pMesh->DrawSubset(nCntMat);
				}
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
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
// 生き物の取得処理
//=============================================================================
Fishes_Model* GetFishesModel(void)
{
	return &g_aFishesModel[0];
}

//=============================================================================
// 生き物の設定処理
//=============================================================================
void SetFishes(int ModelIdx, int nNumSet)
{
	// ローカル変数宣言
	Fishes* pFishes = GetFishes();
	Fishes_Model* pFishesModel = GetFishesModel();
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

			pFishes->bLoopMotion = pFishesModel[ModelIdx].aMotionInfo->bLoop;		// ループするかどうか
			pFishes->fRadius = pFishesModel[ModelIdx].fRadius;						// 半径
			pFishes->fHeight = pFishesModel[ModelIdx].fHeight;						// 高さ
			pFishes->nNumModel = pFishesModel[ModelIdx].nNumModel;					// モデル(パーツ)の総数
			pFishes->nNumMotion = pFishesModel[ModelIdx].nNumMotion;				// モーションの総数
			pFishes->bBlendMotion = pFishesModel[ModelIdx].bBlendMotion;			// ブレンドモーションがあるかどうか
			pFishes->motionTypeBlend = pFishesModel[ModelIdx].motionTypeBlend;		// ブレンドモーションの種類
			pFishes->bLoopMotionBlend = pFishesModel[ModelIdx].bLoopMotionBlend;	// ブレンドモーションがループするかどうか
			pFishes->nNumKeyBlend = pFishesModel[ModelIdx].nNumKeyBlend;			// ブレンドモーションのキーの総数
			pFishes->nFrameBlend = pFishesModel[ModelIdx].nFrameBlend;				// ブレンドフレーム数

			// 全モデル(パーツ)の描画
			for (int nCntModel = 0; nCntModel < pFishes->nNumModel; nCntModel++)
			{
				pFishes->aModel[nCntModel] = pFishesModel[ModelIdx].aModel[nCntModel];		// モデル(パーツ)
			}
			for (int nCntMotion = 0; nCntMotion < pFishes->nNumMotion; nCntMotion++)
			{
				pFishes->aMotionInfo[nCntMotion] = pFishesModel[ModelIdx].aMotionInfo[nCntMotion];		// モーション情報
			}

			g_aFishes[0].nUseNum++;
			nModelSet++;
		}
	}
}
//=============================================================================
// 生き物のロード処理
//=============================================================================
void LoadFishes(int Idx)
{
	// ローカル変数宣言
	FILE* pFile;
	Fishes_Model* pFishesModel = GetFishesModel();
	FishesInfo* pFishesInfo = &g_aFishInfo[0];
	char aString[512] = {};				// ファイルのテキスト読み込み
	char aTrash[512] = {};				// ごみ箱
	char aModelName[128][512] = {};		// モデルの名前

	// テクスチャ読み込み用の変数
	int nNumTexture = 0;
	char aTextureName[64][256] = {};		// テクスチャの名前

	// キャラクターセット用の変数
	int nNumParts = 0;		// 読み込むパーツ数
	float fRadius = 0.0f;	// キャラクターの半径
	float fHeight = 0.0f;	// キャラクターの高さ
	float fMove = 0.0f;		// キャラクターの移動量
	float fJump = 0.0f;		// キャラクターのジャンプ量
	D3DXVECTOR2 Blowoff = D3DXVECTOR2(0.0f, 0.0f);		// 吹っ飛び量
	D3DXVECTOR2 Somersault = D3DXVECTOR2(0.0f, 0.0f);	// バク宙量

	// パーツセット用の変数
	int nIdx = -1;			// モデル番号
	int nIdxParent = -1;	// 親のモデル番号
	D3DXVECTOR3 pos = FIRST_POS;		// 位置
	D3DXVECTOR3 rot = FIRST_POS;		// 向き

	// モーションセット用の変数
	int nLoop = 0;			// ループするかどうか
	int nNumKey = 0;		// キーの総数
	int nFrame = 0;			// 再生フレーム数
	int nCntMotion = 0;		// モーション番号
	KEY key = {};			// キー要素

	pFile = fopen(pFishesInfo[Idx].Model_FileName, "r");

	if (pFile != NULL)
	{// ファイルが開けた場合
		fscanf(pFile, "%[^SCRIPT]", &aTrash[0]);

		while (1)
		{
			fscanf(pFile, "%s", &aString[0]);

			if (aString[0] == '#')
			{// コメントは無視
				fgets(&aTrash[0], ONE_LINE, pFile);

				continue;
			}

			if (strcmp(&aString[0], "NUM_MODEL") == 0)
			{// モデル数の読み込み
				fscanf(pFile, " = %d", &pFishesModel[Idx].nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < pFishesModel[Idx].nNumModel; nCntModel++)
				{
					if (nCntModel > 0)
					{// 2回目以降のMODEL_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "MODEL_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aModelName[nCntModel][0]);		// モデルのパス
					strcpy(pFishesModel[Idx].sModelFileName[nCntModel], aModelName[nCntModel]);


					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				continue;
			}

			if (strcmp(&aString[0], "NUM_TEXTURE") == 0)
			{// テクスチャ数の読み込み
				fscanf(pFile, " = %d", &nNumTexture);

				continue;
			}

			if (strcmp(&aString[0], "TEXTURE_FILENAME") == 0)
			{// テクスチャの名前読み込み
				for (int nCntTexture = 0; nCntTexture < nNumTexture; nCntTexture++)
				{
					if (nCntTexture > 0)
					{// 2回目以降のTEXTURE_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "TEXTURE_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aTextureName[nCntTexture][0]);		// テクスチャのパス

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				continue;
			}

			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{// キャラクターの設定
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "NUM_PARTS") == 0)
				{// 読み込むパーツ数
					fscanf(pFile, " = %d", &nNumParts);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "RADIUS") == 0)
				{// キャラクターの半径
					fscanf(pFile, " = %f", &pFishesModel[Idx].fRadius);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "HEIGHT") == 0)
				{// キャラクターの高さ
					fscanf(pFile, " = %f", &pFishesModel[Idx].fHeight);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "MOVE") == 0)
				{// キャラクターの移動量
					fscanf(pFile, " = %f", &fMove);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "JUMP") == 0)
				{// キャラクターのジャンプ量
					fscanf(pFile, " = %f", &fJump);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOWOFF") == 0)
				{// キャラクターの吹っ飛び量
					fscanf(pFile, " = %f %f", &Blowoff.x, &Blowoff.y);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SOMERSAULT") == 0)
				{// キャラクターのバク宙量
					fscanf(pFile, " = %f %f", &Somersault.x, &Somersault.y);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				// パーツ読み込み
				for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
				{
					if (strcmp(&aString[0], "PARTSSET") == 0)
					{// PARTSSET
						fscanf(pFile, "%s", &aString[0]);
					}

					if (strcmp(&aString[0], "INDEX") == 0)
					{// モデル番号
						fscanf(pFile, " = %d", &nIdx);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "PARENT") == 0)
					{// 親のモデル番号
						fscanf(pFile, " = %d", &nIdxParent);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "POS") == 0)
					{// パーツの位置(オフセット)
						fscanf(pFile, " = %f %f %f", &pos.x, &pos.y, &pos.z);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "ROT") == 0)
					{// パーツの向き
						fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "END_PARTSSET") == 0)
					{// END_PARTSSET
						fscanf(pFile, "%s", &aString[0]);
					}

					// プレイヤーのモデル情報に設定
					pFishesModel[Idx].aModel[nCntParts].nIdx = nIdx;
					pFishesModel[Idx].aModel[nCntParts].nIdxModelParent = nIdxParent;
					pFishesModel[Idx].aModel[nCntParts].pos = pos;
					pFishesModel[Idx].aModel[nCntParts].rot = rot;
					pFishesModel[Idx].aModel[nCntParts].posOff = pos;
					pFishesModel[Idx].aModel[nCntParts].rotOff = rot;
				}

				continue;
			}

			if (strcmp(&aString[0], "MOTIONSET") == 0)
			{// モーションの設定
				fscanf(pFile, "%s", &aString[0]);

				if (aString[0] == '#')
				{// コメント無視
					fgets(&aTrash[0], ONE_LINE, pFile);

					// 次の文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);
				}

				if (strcmp(&aString[0], "LOOP") == 0)
				{// ループするかどうか
					fscanf(pFile, " = %d", &nLoop);

					// ループの真偽を代入
					if (nLoop == 0)
					{// false
						pFishesModel[Idx].aMotionInfo[nCntMotion].bLoop = false;
					}
					else if (nLoop == 1)
					{// true
						pFishesModel[Idx].aMotionInfo[nCntMotion].bLoop = true;
					}

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "NUM_KEY") == 0)
				{// キーの総数
					fscanf(pFile, " = %d", &nNumKey);

					// キーの総数を代入
					pFishesModel[Idx].aMotionInfo[nCntMotion].nNumKey = nNumKey;

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				// キー読み込み
				for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
				{
					if (strcmp(&aString[0], "KEYSET") == 0)
					{// KEYSET
						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "FRAME") == 0)
					{// 再生フレーム数
						fscanf(pFile, " = %d", &nFrame);

						pFishesModel[Idx].aMotionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = nFrame;

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					// パーツごとに読み込み
					for (int nCntPartsKey = 0; nCntPartsKey < nNumParts; nCntPartsKey++)
					{
						if (strcmp(&aString[0], "KEY") == 0)
						{// KEY
							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "POS") == 0)
						{// パーツの位置(オフセット)
							fscanf(pFile, " = %f %f %f", &key.fPosX, &key.fPosY, &key.fPosZ);

							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "ROT") == 0)
						{// パーツの向き
							fscanf(pFile, " = %f %f %f", &key.fRotX, &key.fRotY, &key.fRotZ);

							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "END_KEY") == 0)
						{// END_KEY
							fscanf(pFile, "%s", &aString[0]);
						}

						// モーション情報に代入
						pFishesModel[Idx].aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPartsKey] = key;
					}

					if (strcmp(&aString[0], "END_KEYSET") == 0)
					{// END_KEYSET
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				nCntMotion++;

				continue;
			}

			if (strcmp(&aString[0], "END_SCRIPT") == 0)
			{// ファイルの読み込みが完了
				break;
			}
		}

		// モーションの総数を代入
		pFishesModel[Idx].nNumMotion = nCntMotion + 1;

		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合
		printf("ファイルを開けませんでした");
	}
}

//=============================================================================
// 生き物のモーションの更新処理
//=============================================================================
void UpdateMotionFishes(void)
{
	int nNextKey;
	Fishes* pFishes = GetFishes();

	for (int nCntFishes = 0; nCntFishes < FISHES_MAX_NUM; nCntFishes++, pFishes++)
	{
		// 全モデル(パーツ)の更新
		for (int nCntModel = 0; nCntModel < pFishes->nNumModel; nCntModel++)
		{
			// ローカル変数宣言
			KEY* pKey;
			KEY* pKeyNext;
			Model* pModel;
			D3DXVECTOR3 posCurrent, rotCurrent;
			D3DXVECTOR3 posDest, rotDest;
			float fDiffKey, fRateKey;

			// 1フレームあたりに動く割合
			fRateKey = ((float)pFishes->nCounterMotion / (float)pFishes->aMotionInfo[pFishes->motionType].aKeyInfo[pFishes->nKey].nFrame);

			// 次のキー
			if (pFishes->bLoopMotion == true)
			{// ループモーション
				nNextKey = (pFishes->nKey + 1) % (pFishes->aMotionInfo[pFishes->motionType].nNumKey);
			}
			else
			{// ループしないモーション
				nNextKey = pFishes->nKey + 1;

				if (nNextKey > pFishes->aMotionInfo[pFishes->motionType].nNumKey - 1)
				{// キーの総数を超えた
					nNextKey = pFishes->aMotionInfo[pFishes->motionType].nNumKey - 1;
				}
			}

			// 現在のキーのポインタ
			pKey = &pFishes->aMotionInfo[pFishes->motionType].aKeyInfo[pFishes->nKey].aKey[nCntModel];

			// 次のキーのポインタ
			pKeyNext = &pFishes->aMotionInfo[pFishes->motionType].aKeyInfo[nNextKey].aKey[nCntModel];

			// モデルのポインタ
			pModel = &pFishes->aModel[nCntModel];

			if (pFishes->bBlendMotion == true)
			{// ブレンドあり
				// ローカル変数宣言
				KEY* pKeyBlend;
				KEY* pKeyNextBlend;
				D3DXVECTOR3 posBlend, rotBlend;
				float fDiffKeyBlend, fDiffBlend, fRateKeyBlend, fRateBlend;
				int nNextKeyBlend;

				// 1フレームあたりに動く割合
				fRateKeyBlend = ((float)pFishes->nCounterMotionBlend / (float)pFishes->aMotionInfo[pFishes->motionTypeBlend].aKeyInfo[pFishes->nKeyBlend].nFrame);

				// ブレンドの相対値
				fRateBlend = (float)pFishes->nCounterBlend / (float)pFishes->nFrameBlend;

				// 次のキー
				if (pFishes->bLoopMotionBlend == true)
				{// ループモーション
					nNextKeyBlend = (pFishes->nKeyBlend + 1) % (pFishes->aMotionInfo[pFishes->motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					nNextKeyBlend = pFishes->nKeyBlend + 1;

					if (nNextKeyBlend > pFishes->aMotionInfo[pFishes->motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						nNextKeyBlend = pFishes->aMotionInfo[pFishes->motionTypeBlend].nNumKey - 1;
					}
				}

				// 現在のキーのポインタ
				pKeyBlend = &pFishes->aMotionInfo[pFishes->motionTypeBlend].aKeyInfo[pFishes->nKeyBlend].aKey[nCntModel];

				// 次のキーのポインタ
				pKeyNextBlend = &pFishes->aMotionInfo[pFishes->motionTypeBlend].aKeyInfo[nNextKeyBlend].aKey[nCntModel];

				// キー情報から位置・向きを算出
				// 位置X
				fDiffKey = pKeyNext->fPosX - pKey->fPosX;					// 現在のモーション
				posCurrent.x = pKey->fPosX + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosX - pKeyBlend->fPosX;	// ブレンドモーション
				posBlend.x = pKeyBlend->fPosX + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.x - posCurrent.x;						// 差分
				posDest.x = posCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

				// 位置Y
				fDiffKey = pKeyNext->fPosY - pKey->fPosY;					// 現在のモーション
				posCurrent.y = pKey->fPosY + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosY - pKeyBlend->fPosY;	// ブレンドモーション
				posBlend.y = pKeyBlend->fPosY + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.y - posCurrent.y;						// 差分
				posDest.y = posCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

				// 位置Z
				fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;					// 現在のモーション
				posCurrent.z = pKey->fPosZ + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosZ - pKeyBlend->fPosZ;	// ブレンドモーション
				posBlend.z = pKeyBlend->fPosZ + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.z - posCurrent.z;						// 差分
				posDest.z = posCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

				// 向きX
				fDiffKey = pKeyNext->fRotX - pKey->fRotX;					// 現在のモーション
				rotCurrent.x = pKey->fRotX + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotX - pKeyBlend->fRotX;	// ブレンドモーション
				rotBlend.x = pKeyBlend->fRotX + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.x - rotCurrent.x;						// 差分
				rotDest.x = rotCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.x > D3DX_PI)
				{
					rotDest.x -= D3DX_PI * 2;
				}
				else if (rotDest.x < -D3DX_PI)
				{
					rotDest.x += D3DX_PI * 2;
				}

				// 向きY
				fDiffKey = pKeyNext->fRotY - pKey->fRotY;					// 現在のモーション
				rotCurrent.y = pKey->fRotY + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotY - pKeyBlend->fRotY;	// ブレンドモーション
				rotBlend.y = pKeyBlend->fRotY + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.y - rotCurrent.y;						// 差分
				rotDest.y = rotCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.y > D3DX_PI)
				{
					rotDest.y -= D3DX_PI * 2;
				}
				else if (rotDest.y < -D3DX_PI)
				{
					rotDest.y += D3DX_PI * 2;
				}

				// 向きZ
				fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;					// 現在のモーション
				rotCurrent.z = pKey->fRotZ + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotZ - pKeyBlend->fRotZ;	// ブレンドモーション
				rotBlend.z = pKeyBlend->fRotZ + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.z - rotCurrent.z;						// 差分
				rotDest.z = rotCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.z > D3DX_PI)
				{
					rotDest.z -= D3DX_PI * 2;
				}
				else if (rotDest.z < -D3DX_PI)
				{
					rotDest.z += D3DX_PI * 2;
				}
			}
			else
			{// ブレンドなし
				// キー情報から位置・向きを算出
				// 位置X
				fDiffKey = pKeyNext->fPosX - pKey->fPosX;
				posDest.x = pKey->fPosX + (fDiffKey * fRateKey);

				// 位置Y
				fDiffKey = pKeyNext->fPosY - pKey->fPosY;
				posDest.y = pKey->fPosY + (fDiffKey * fRateKey);

				// 位置Z
				fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
				posDest.z = pKey->fPosZ + (fDiffKey * fRateKey);

				// 向きX
				fDiffKey = pKeyNext->fRotX - pKey->fRotX;
				rotDest.x = pKey->fRotX + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.x > D3DX_PI)
				{
					rotDest.x -= D3DX_PI * 2;
				}
				else if (rotDest.x < -D3DX_PI)
				{
					rotDest.x += D3DX_PI * 2;
				}

				// 向きY
				fDiffKey = pKeyNext->fRotY - pKey->fRotY;
				rotDest.y = pKey->fRotY + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.y > D3DX_PI)
				{
					rotDest.y -= D3DX_PI * 2;
				}
				else if (rotDest.y < -D3DX_PI)
				{
					rotDest.y += D3DX_PI * 2;
				}

				// 向きZ
				fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;
				rotDest.z = pKey->fRotZ + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.z > D3DX_PI)
				{
					rotDest.z -= D3DX_PI * 2;
				}
				else if (rotDest.z < -D3DX_PI)
				{
					rotDest.z += D3DX_PI * 2;
				}
			}

			// パーツの位置・向きを設定
			pModel->pos = posDest + pFishes->aModel[nCntModel].posOff;
			pModel->rot = rotDest + pFishes->aModel[nCntModel].rotOff;
		}

		if (pFishes->bBlendMotion == true)
		{// ブレンドあり
			pFishes->nCounterMotion++;

			if (pFishes->nCounterMotion >= pFishes->aMotionInfo[pFishes->motionType].aKeyInfo[pFishes->nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pFishes->nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (pFishes->bLoopMotion == true)
				{// ループモーション
					pFishes->nKey = (pFishes->nKey + 1) % (pFishes->aMotionInfo[pFishes->motionType].nNumKey);
				}
				else
				{// ループしないモーション
					pFishes->nKey++;

					if (pFishes->nKey >= pFishes->aMotionInfo[pFishes->motionType].nNumKey - 1)
					{// キーの総数を超えた
						pFishes->nKey = pFishes->aMotionInfo[pFishes->motionType].nNumKey - 1;

						pFishes->bFinishMotion = true;
					}
				}
			}

			pFishes->nCounterMotionBlend++;

			if (pFishes->nCounterMotionBlend >= pFishes->aMotionInfo[pFishes->motionTypeBlend].aKeyInfo[pFishes->nKeyBlend].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pFishes->nCounterMotionBlend = 0;	// カウンターを戻す

				// ループかどうか
				if (pFishes->bLoopMotionBlend == true)
				{// ループモーション
					pFishes->nKeyBlend = (pFishes->nKeyBlend + 1) % (pFishes->aMotionInfo[pFishes->motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					pFishes->nKeyBlend++;

					if (pFishes->nKeyBlend >= pFishes->aMotionInfo[pFishes->motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						pFishes->nKeyBlend = pFishes->aMotionInfo[pFishes->motionTypeBlend].nNumKey - 1;

						pFishes->bFinishMotion = true;
					}
				}
			}

			pFishes->nCounterBlend++;

			if (pFishes->nCounterBlend >= pFishes->nFrameBlend)
			{// ブレンドフレームに到達
				// ブレンドモーションを現在のモーションに設定
				pFishes->motionType = pFishes->motionTypeBlend;
				pFishes->bLoopMotion = pFishes->bLoopMotionBlend;
				pFishes->nNumKey = pFishes->nNumKeyBlend;
				pFishes->nKey = pFishes->nKeyBlend;
				pFishes->nCounterMotion = pFishes->nCounterMotionBlend;
				//pFishes->bFinishMotion = true;

				pFishes->nKeyBlend = 0;
				pFishes->nCounterMotionBlend = 0;
				pFishes->bBlendMotion = false;
				pFishes->nCounterBlend = 0;
				pFishes->nFrameBlend = 0;
			}
		}
		else
		{// ブレンドなし
			pFishes->nCounterMotion++;

			if (pFishes->nCounterMotion >= pFishes->aMotionInfo[pFishes->motionType].aKeyInfo[pFishes->nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pFishes->nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (pFishes->bLoopMotion == true)
				{// ループモーション
					pFishes->nKey = (pFishes->nKey + 1) % (pFishes->aMotionInfo[pFishes->motionType].nNumKey);
				}
				else
				{// ループしないモーション
					pFishes->nKey++;

					if (pFishes->nKey >= pFishes->aMotionInfo[pFishes->motionType].nNumKey - 1)
					{// キーの総数を超えた
						pFishes->nKey = pFishes->aMotionInfo[pFishes->motionType].nNumKey - 1;

						pFishes->bFinishMotion = true;
					}
				}
			}
		}
	}
}

//=============================================================================
// 生き物のモーションの設定処理
//=============================================================================
void SetMotionFishes(int nIdx, MOTIONTYPE motionType, bool bBlendMotion, int nFrameBlend)
{
	Fishes* pFishes = &g_aFishes[nIdx];

	if (pFishes->motionTypeBlend != motionType)
	{// 違うモーションが設定されたときだけ
		if (bBlendMotion == true)
		{// ブレンドあり
			pFishes->motionTypeBlend = motionType;
			pFishes->bLoopMotionBlend = pFishes->aMotionInfo[motionType].bLoop;
			pFishes->nNumKeyBlend = pFishes->aMotionInfo[motionType].nNumKey;
			pFishes->nKeyBlend = 0;
			pFishes->nCounterMotionBlend = 0;
			pFishes->bFinishMotion = false;

			pFishes->bBlendMotion = bBlendMotion;
			pFishes->nFrameBlend = nFrameBlend;
			pFishes->nCounterBlend = 0;
		}
		else
		{// ブレンドなし
			pFishes->motionType = motionType;
			pFishes->bLoopMotion = pFishes->aMotionInfo[motionType].bLoop;
			pFishes->nNumKey = pFishes->aMotionInfo[motionType].nNumKey;
			pFishes->nKey = 0;
			pFishes->nCounterMotion = 0;
			pFishes->bFinishMotion = false;

			pFishes->bBlendMotion = bBlendMotion;

			// 全モデル(パーツ)の初期設定
			for (int nCntModel = 0; nCntModel < pFishes->nNumModel; nCntModel++)
			{
				pFishes->aModel[nCntModel].pos.x = pFishes->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosX;
				pFishes->aModel[nCntModel].pos.y = pFishes->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosY;
				pFishes->aModel[nCntModel].pos.z = pFishes->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosZ;

				pFishes->aModel[nCntModel].rot.x = pFishes->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotX;
				pFishes->aModel[nCntModel].rot.y = pFishes->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotY;
				pFishes->aModel[nCntModel].rot.z = pFishes->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotZ;
			}
		}
	}
}
