//=============================================================================
// 
// 配置物 [object.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "object.h"
#include "pot.h"
#include "player.h"
#include "meshfield.h"
#include "meshcylinder.h"
#include "debugproc.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FIRST_SCALE				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 初期拡大率

//*****************************************************************************
// グローバル変数
//*****************************************************************************
ObjectModel g_aObjectModel[MAX_OBJECTMODEL];					// 配置物モデルの情報
Object g_aObject[MAX_OBJECT];									// 配置物の情報
char* g_apFilenameObject[MAX_OBJECTMODEL] = {};					// モデルファイルへのポインタ
int g_nNumObject = 0;											// オブジェクトの総数

//=============================================================================
// 配置物の初期化処理
//=============================================================================
void InitObject(const char* pStr)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	int nNumVtx;			// 頂点数
	DWORD dwSizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;			// 頂点バッファへのポインタ

	// 配置物の情報の初期化
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		g_aObject[nCntObject].pos = FIRST_POS;
		g_aObject[nCntObject].posOff = FIRST_POS;
		g_aObject[nCntObject].move = FIRST_POS;
		g_aObject[nCntObject].rot = FIRST_POS;
		g_aObject[nCntObject].scale = FIRST_SCALE;
		g_aObject[nCntObject].state = OBJECTSTATE_NONE;
		g_aObject[nCntObject].nCounterState = 0;
		g_aObject[nCntObject].nType = 0;
		g_aObject[nCntObject].nIdxShadow = -1;
		g_aObject[nCntObject].bUse = false;
		g_aObject[nCntObject].bCollision = true;
	}

	g_nNumObject = 0;

	// モデルの読み込み
	LoadStage(pStr);

	// 配置物読み込み
	LoadObject(pStr);

	// 配置物モデルの情報の初期化
	for (int nCntObject = 0; nCntObject < MAX_OBJECTMODEL; nCntObject++)
	{
		// 最大値最小値の初期化
		g_aObjectModel[nCntObject].VtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
		g_aObjectModel[nCntObject].VtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(g_apFilenameObject[nCntObject],
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&g_aObjectModel[nCntObject].pBuffMat,
			NULL,
			&g_aObjectModel[nCntObject].dwNumMat,
			&g_aObjectModel[nCntObject].pMesh)))
		{
			continue;
		}

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)g_aObjectModel[nCntObject].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aObjectModel[nCntObject].dwNumMat; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aObjectModel[nCntObject].apTexture[nCntMat]);
			}
		}

		// 頂点数を取得
		nNumVtx = g_aObjectModel[nCntObject].pMesh->GetNumVertices();

		// 頂点フォーマットのサイズを取得
		dwSizeFVF = D3DXGetFVFVertexSize(g_aObjectModel[nCntObject].pMesh->GetFVF());

		// 頂点バッファをロック
		g_aObjectModel[nCntObject].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;		// 頂点座標の代入

			if (vtx.x > g_aObjectModel[nCntObject].VtxMax.x)
			{// Xの最大値
				g_aObjectModel[nCntObject].VtxMax.x = vtx.x;
			}

			if (vtx.x < g_aObjectModel[nCntObject].VtxMin.x)
			{// Xの最小値
				g_aObjectModel[nCntObject].VtxMin.x = vtx.x;
			}

			if (vtx.y > g_aObjectModel[nCntObject].VtxMax.y)
			{// Yの最大値
				g_aObjectModel[nCntObject].VtxMax.y = vtx.y;
			}

			if (vtx.y < g_aObjectModel[nCntObject].VtxMin.y)
			{// Yの最小値
				g_aObjectModel[nCntObject].VtxMin.y = vtx.y;
			}

			if (vtx.z > g_aObjectModel[nCntObject].VtxMax.z)
			{// Zの最大値
				g_aObjectModel[nCntObject].VtxMax.z = vtx.z;
			}

			if (vtx.z < g_aObjectModel[nCntObject].VtxMin.z)
			{// Zの最小値
				g_aObjectModel[nCntObject].VtxMin.z = vtx.z;
			}

			pVtxBuff += dwSizeFVF;		// 頂点バッファのサイズ分ポインタを進める
		}

		// 頂点バッファをアンロック
		g_aObjectModel[nCntObject].pMesh->UnlockVertexBuffer();
	}
}

//=============================================================================
// 配置物の終了処理
//=============================================================================
void UninitObject(void)
{
	for (int nCntObjectModel = 0; nCntObjectModel < MAX_OBJECTMODEL; nCntObjectModel++)
	{
		// メッシュの破棄
		if (g_aObjectModel[nCntObjectModel].pMesh != NULL)
		{
			g_aObjectModel[nCntObjectModel].pMesh->Release();
			g_aObjectModel[nCntObjectModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (g_aObjectModel[nCntObjectModel].pBuffMat != NULL)
		{
			g_aObjectModel[nCntObjectModel].pBuffMat->Release();
			g_aObjectModel[nCntObjectModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntObject = 0; nCntObject < MAX_TEXTURE; nCntObject++)
		{
			if (g_aObjectModel[nCntObjectModel].apTexture[nCntObject] != NULL)
			{
				g_aObjectModel[nCntObjectModel].apTexture[nCntObject]->Release();
				g_aObjectModel[nCntObjectModel].apTexture[nCntObject] = NULL;
			}
		}
	}
}

//=============================================================================
// 配置物の更新処理
//=============================================================================
void UpdateObject(void)
{
	//PrintDebugProc("配置したオブジェクト数 : %d\n", g_nNumObject);
	static bool nFrag = false;

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == true)
		{
			if (strcmp(g_aObject[nCntObject].sFileName, "data/MODEL/Others_Object/wakame000.x") == 0)
			{  // ワカメならユラユラする

				if (g_aObject[nCntObject].rot.x > 0.1f)
				{
					nFrag = false;
				}
				if (g_aObject[nCntObject].rot.x < -0.1f)
				{
					nFrag = true;
				}
				if (nFrag == true)
				{
					g_aObject[nCntObject].rot.x += 0.002f;
				}
				else
				{
					g_aObject[nCntObject].rot.x -= 0.002f;
				}
			}
		}
	}
}

//=============================================================================
// 配置物の描画処理
//=============================================================================
void DrawObject(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == true)
		{// 使用されているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aObject[nCntObject].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aObject[nCntObject].rot.y, g_aObject[nCntObject].rot.x, g_aObject[nCntObject].rot.z);
			D3DXMatrixMultiply(&g_aObject[nCntObject].mtxWorld, &g_aObject[nCntObject].mtxWorld, &mtxRot);

			// 拡大率を反映
			D3DXMatrixScaling(&mtxScale, g_aObject[nCntObject].scale.x, g_aObject[nCntObject].scale.y, g_aObject[nCntObject].scale.z);
			D3DXMatrixMultiply(&g_aObject[nCntObject].mtxWorld, &g_aObject[nCntObject].mtxWorld, &mtxScale);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aObject[nCntObject].pos.x, g_aObject[nCntObject].pos.y, g_aObject[nCntObject].pos.z);
			D3DXMatrixMultiply(&g_aObject[nCntObject].mtxWorld, &g_aObject[nCntObject].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aObject[nCntObject].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_aObjectModel[g_aObject[nCntObject].nType].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aObjectModel[g_aObject[nCntObject].nType].dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_aObjectModel[g_aObject[nCntObject].nType].apTexture[nCntMat]);

				// モデルパーツの描画
				g_aObjectModel[g_aObject[nCntObject].nType].pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// 配置物の設定処理
//=============================================================================
void SetObject(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdx, bool bCollision)
{
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == false)
		{// 配置物が使用されていない
			g_aObject[nCntObject].pos = pos;
			g_aObject[nCntObject].posOff = pos;
			g_aObject[nCntObject].move = FIRST_POS;
			g_aObject[nCntObject].rot = rot;
			g_aObject[nCntObject].scale = FIRST_SCALE;
			g_aObject[nCntObject].state = OBJECTSTATE_NONE;
			g_aObject[nCntObject].nCounterState = 0;
			g_aObject[nCntObject].nType = nIdx;
			g_aObject[nCntObject].nIdxShadow = -1;
			g_aObject[nCntObject].bCollision = bCollision;
			g_aObject[nCntObject].bUse = true;		// 使用している状態にする
			strcpy(g_aObject[nCntObject].sFileName, g_apFilenameObject[nIdx]);

			g_nNumObject++;

			break;
		}
	}
}

//=============================================================================
// 配置物のランダム設定処理
//=============================================================================
void SetObjectRandom(int nType, D3DXVECTOR3 posMin, D3DXVECTOR3 posMax, int nAmount)
{
	D3DXVECTOR3 pos, rot;

	for (int nCntObject = 0; nCntObject < nAmount; nCntObject++)
	{
		pos.x = rand() % (int)(posMax.x - posMin.x) + posMin.x;
		pos.y = 0.0f;
		pos.z = rand() % (int)(posMax.z - posMin.z) + posMin.z;

		rot.x = 0.0f;
		rot.y = (float)(rand() % 360) / 360.0f;
		rot.z = 0.0f;

		SetObject(pos, rot, nType, false);
	}

}

//=============================================================================
// 配置物の当たり判定
//=============================================================================
bool CollisionObject(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fRadius, float fHeight, bool bInsec)
{
	bool bLand = false;		// 着地しているか
	Object* pObject = GetObjectAll();

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{
		if (pObject->bUse == false || pObject->bCollision == false)
		{// 使用されてない
			continue;
		}

#if 0
		float fLengthX = 0.0f;
		float fLengthZ = 0.0f;

		if (pObject->rot.y == 0.0f || pObject->rot.y == D3DX_PI)
		{// そのままの向き
			fLengthX = (g_aObjectModel[pObject->nType].VtxMax.x - g_aObjectModel[pObject->nType].VtxMin.x) / 2.0f;
			fLengthZ = (g_aObjectModel[pObject->nType].VtxMax.z - g_aObjectModel[pObject->nType].VtxMin.z) / 2.0f;
		}
		else
		{// 角度が変わっている
			fLengthX = (g_aObjectModel[pObject->nType].VtxMax.z - g_aObjectModel[pObject->nType].VtxMin.z) / 2.0f;
			fLengthZ = (g_aObjectModel[pObject->nType].VtxMax.x - g_aObjectModel[pObject->nType].VtxMin.x) / 2.0f;
		}

		if ((pPos->x + fRadius > pObject->posOff.x - fLengthX) &&
			(pPos->x - fRadius < pObject->posOff.x + fLengthX) &&
			(pObject->posOff.z - fLengthZ - fRadius <= pPos->z) &&
			(pObject->posOff.z + fLengthZ + fRadius >= pPos->z) &&
			(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight <= pPos->y) &&
			(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y >= pPos->y))
		{// 現在の位置が配置物の範囲内
			if (((pObject->posOff.x - fLengthX - fRadius == pPosOld->x) &&
				(pObject->posOff.x + fLengthX + fRadius > pPosOld->x)) ||
				((pObject->posOff.x + fLengthX + fRadius == pPosOld->x) &&
					(pObject->posOff.x - fLengthX - fRadius < pPosOld->x)))
			{// 前回の位置が配置物の位置なら奥行判定しない

			}
			else if (((pObject->posOff.z + fLengthZ + fRadius <= pPosOld->z) &&
				(pObject->posOff.z + fLengthZ + fRadius >= pPos->z)))
			{// 奥からの当たり判定
				pPos->z = pObject->posOff.z + fLengthZ + fRadius;
				pMove->z = 0.0f;
			}
			else if (((pObject->posOff.z - fLengthZ - fRadius >= pPosOld->z) &&
				(pObject->posOff.z - fLengthZ - fRadius <= pPos->z)))
			{// 手前からの当たり判定
				pPos->z = pObject->posOff.z - fLengthZ - fRadius;
				pMove->z = 0.0f;
			}

			if (((pObject->posOff.z - fLengthZ - fRadius == pPosOld->z) &&
				(pObject->posOff.z + fLengthZ + fRadius > pPosOld->z)) ||
				((pObject->posOff.z + fLengthZ + fRadius == pPosOld->z) &&
					(pObject->posOff.z - fLengthZ - fRadius < pPosOld->z)))
			{// 前回の位置が配置物の位置なら左右判定しない

			}
			else if ((pObject->posOff.x - fLengthX - fRadius >= pPosOld->x) &&
				(pObject->posOff.x - fLengthX - fRadius <= pPos->x))
			{// 左からの当たり判定
				pPos->x = pObject->posOff.x - fLengthX - fRadius;
				pMove->x = 0.0f;
			}
			else if ((pObject->posOff.x + fLengthX + fRadius <= pPosOld->x) &&
				(pObject->posOff.x + fLengthX + fRadius >= pPos->x))
			{// 右からの当たり判定
				pPos->x = pObject->posOff.x + fLengthX + fRadius;
				pMove->x = 0.0f;
			}
			else if ((pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight >= pPosOld->y) &&
				(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight <= pPos->y))
			{// 下からの当たり判定
				pPos->y = pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight;
				pMove->y = -0.5f;							// 移動量を0にする
			}
			else if ((pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y <= pPosOld->y) &&
				(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y >= pPos->y))
			{// 上からの当たり判定
				pPos->y = pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y;
				pMove->y = 0.0f;							// 移動量を0にする

				bLand = true;
			}
		}
#else
		for (int nCnt = 0; nCnt < 4; nCnt++)
		{
			ObjectModel* pObjectModel = &g_aObjectModel[pObject->nType];

			D3DXVECTOR3 start, end;
			float fXS, fZS, fXE, fZE;
			D3DXVECTOR3 vecLine, vecMove, vecToPos, vecToPosOld, vecNor, vecMoveRef, vecMoveDest;		// 各ベクトル
			static D3DXVECTOR3 insec = FIRST_POS;		// 交点
			D3DXVECTOR3 vecLineW, posDest;
			float fRate, fDot;
			float fXLength = pObjectModel->VtxMax.x - pObjectModel->VtxMin.x;
			float fZLength = pObjectModel->VtxMax.z - pObjectModel->VtxMin.z;
			float fLength = sqrtf((fXLength * fXLength) + (fZLength * fZLength)) * 0.5f;
			float fRotS = (nCnt * D3DX_PI * 0.5f) - (D3DX_PI * 0.25f) + pObject->rot.y;
			float fRotE = ((nCnt + 1) * D3DX_PI * 0.5f) - (D3DX_PI * 0.25f) + pObject->rot.y;

			CorrectAngle(&fRotS, fRotS);
			CorrectAngle(&fRotE, fRotE);

			fXS = sinf(fRotS) * (fLength + fRadius);
			fZS = cosf(fRotS) * (fLength + fRadius);

			fXE = sinf(fRotE) * (fLength + fRadius);
			fZE = cosf(fRotE) * (fLength + fRadius);

			// 始点
			start.x = pObject->pos.x + fXS;
			start.y = 0.0f;
			start.z = pObject->pos.z + fZS;

			// 終点
			end.x = pObject->pos.x + fXE;
			end.y = 0.0f;
			end.z = pObject->pos.z + fZE;

			// 境界線ベクトル
			vecLine.x = (end.x) - (start.x);
			vecLine.y = 0.0f;
			vecLine.z = (end.z) - (start.z);

			// 移動ベクトル
			vecMove.x = pPos->x - pPosOld->x;
			vecMove.y = 0.0f;
			vecMove.z = pPos->z - pPosOld->z;

			vecToPos.x = pPos->x - (start.x);
			vecToPos.y = 0.0f;
			vecToPos.z = pPos->z - (start.z);

			vecToPosOld.x = pPosOld->x - (start.x);
			vecToPosOld.y = 0.0f;
			vecToPosOld.z = pPosOld->z - (start.z);

			// 法線ベクトル
			vecNor.x = (vecLine.x * cosf(-D3DX_PI * 0.5f)) + (vecLine.z * sinf(-D3DX_PI * 0.5f));
			vecNor.y = 0.0f;
			vecNor.z = (vecLine.x * sinf(D3DX_PI * 0.5f)) - (vecLine.z * cosf(D3DX_PI * 0.5f));
			D3DXVec3Normalize(&vecNor, &vecNor);		// ベクトルを正規化する

			// 内積
			fDot = (-vecMove.x * vecNor.x) + (-vecMove.z * vecNor.z);

			// 外積
			float f = ((vecMove.z * vecNor.x) - (vecMove.x * vecNor.z));

			// 交点の割合
			fRate = ((vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z)) /
				((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

			float fAngle;

			// 角度
			fAngle = D3DX_PI - atan2f(vecMove.x, vecMove.z);
			float fMeshAngle = atan2f(vecLine.x, vecLine.z);
			fAngle += fMeshAngle;
			fAngle = D3DX_PI - fAngle;
			fAngle -= D3DX_PI * 0.5f;
			CorrectAngle(&fAngle, fAngle);

			// 反射後の移動ベクトル
			vecMoveRef.x = vecMove.x + ((vecNor.x * fDot) * 2);
			vecMoveRef.y = 0.0f;
			vecMoveRef.z = vecMove.z + ((vecNor.z * fDot) * 2);

			if (fRate >= 0.0f && fRate <= 1.0f)
			{// 交点の割合が範囲内
				float fPosLine = (float)((int)(((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z)) * 1.0f) / (int)1);
				float fPosOldLine = (float)((int)(((vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z)) * 1.0f) / (int)1);

				if (fPosLine > 0.0f && (fPosOldLine <= 0.0f))
				{// 交差した
					if (bInsec == true)
					{// 交点出す用
						insec.x = start.x + (vecLine.x * (fRate));
						insec.y = pPos->y;
						insec.z = start.z + (vecLine.z * (fRate));
					}
					else if ((pObject->posOff.y + pObjectModel->VtxMin.y - fHeight <= pPos->y) &&
						(pObject->posOff.y + pObjectModel->VtxMax.y >= pPos->y))
					{
						D3DXVECTOR3 move = vecMove;
						move.y = 0.0f;
						D3DXVec3Normalize(&move, &move);

						float fDotN = D3DXVec3Dot(&move, &vecNor);

						if (fDotN < 0.0f)
						{// 壁に向かっているときだけ法線成分を消す
							vecMoveDest = move - (vecNor * fDotN);
						}

						pPos->x = start.x + (vecLine.x * fRate) + vecMoveDest.x;
						pPos->z = start.z + (vecLine.z * fRate) + vecMoveDest.z;

						pMove->x = vecMoveDest.x;
						pMove->z = vecMoveDest.z;
					}
				}

				if (fPosLine > 0.0f)
				{// 今の位置が内側にいる
					if (bInsec == false)
					{// 交点じゃない
						if ((pObject->posOff.y + pObjectModel->VtxMin.y - fHeight >= pPosOld->y) &&
							(pObject->posOff.y + pObjectModel->VtxMin.y - fHeight <= pPos->y))
						{// 下からの当たり判定
							pPos->y = pObject->posOff.y + pObjectModel->VtxMin.y - fHeight;
							pMove->y = -0.5f;							// 移動量を0にする
						}
						else if ((pObject->posOff.y + pObjectModel->VtxMax.y <= pPosOld->y) &&
							(pObject->posOff.y + pObjectModel->VtxMax.y >= pPos->y))
						{// 上からの当たり判定
							pPos->y = pObject->posOff.y + pObjectModel->VtxMax.y;
							pMove->y = 0.0f;							// 移動量を0にする

							bLand = true;
						}
					}
				}
			}
		}
#endif
	}

	return bLand;
}

//=============================================================================
// 配置物の渦潮安地判定
//=============================================================================
bool CollisionObjectArea(D3DXVECTOR3 pos)
{
	Object* pObject = GetObjectAll();

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{
		if (pObject->bUse == false)
		{// 使用されてない
			continue;
		}

		ObjectModel* pObjectModel = &g_aObjectModel[pObject->nType];

		float fXLength = pObjectModel->VtxMax.x - pObjectModel->VtxMin.x;
		float fZLength = pObjectModel->VtxMax.z - pObjectModel->VtxMin.z;
		float fLength = sqrtf((fXLength * fXLength) + (fZLength * fZLength)) * 0.5f;	// 対角線の長さ = 半径

		float fDistRadius = sqrtf(pObject->pos.x * pObject->pos.x + pObject->pos.z * pObject->pos.z);	// 中心からの距離

		float fVerDist = sqrtf((fDistRadius * fDistRadius) - ((fLength / 2.0f) * (fLength / 2.0f)));
		float fNowAngle = atan2f(pObject->pos.x, pObject->pos.z);
		float fAngle = cosf(fVerDist / fDistRadius) * 0.2f;
		fAngle += fNowAngle;
		CorrectAngle(&fAngle, fAngle);

		D3DXVECTOR3 SafePos;
		SafePos.x = sinf(fAngle) * fDistRadius;
		SafePos.y = pObject->pos.y + pObjectModel->VtxMin.y;
		SafePos.z = cosf(fAngle) * fDistRadius;

		D3DXVECTOR2 dist = D3DXVECTOR2(SafePos.x - pos.x, SafePos.z - pos.z);

		if (D3DXVec2Length(&dist) <= fLength && pos.y >= SafePos.y && pos.y <= (pObject->pos.y + pObjectModel->VtxMax.y))
		{// 安地の範囲内
			return true;
		}
	}

	return false;
}

//=============================================================================
// 配置物モデル読み込み
//=============================================================================
void LoadObject(const char* pStr)
{
	// ローカル変数宣言
	FILE* pFile;
	char aString[256] = {};				// ファイルのテキスト読み込み
	char aTrash[256] = {};				// ゴミ箱
	int nNumModel = 0;					// モデルの読み込み数
	char aModelName[64][256] = {};		// モデルの名前

	// セット用の変数
	int nIdx = 0;			// テクスチャやモデルの種類
	int nCollision = 0;		// 当たり判定するかどうか
	int nShadow = 0;		// 影をつけるかどうか
	bool bCollision = true;	// 当たり判定するかどうか
	bool bShadow = false;	// 影をつけるかどうか
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き

	pFile = fopen(pStr, "r");

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
				fscanf(pFile, " = %d", &nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
				{
					if (nCntModel > 0)
					{// 2回目以降のMODEL_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "MODEL_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aModelName[nCntModel][0]);		// モデルのパス
					g_apFilenameObject[nCntModel] = &aModelName[nCntModel][0];

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				fscanf(pFile, "%[^MODELSET]", &aTrash[0]);

				continue;
			}

			if (strcmp(&aString[0], "MODELSET") == 0)
			{// 配置物の設置
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "TYPE") == 0)
				{// テクスチャやモデルの種類
					fscanf(pFile, " = %d", &nIdx);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "POS") == 0)
				{// 位置
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
				{// 向き
					fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SHADOW") == 0)
				{// 影
					fscanf(pFile, " = %d", &nShadow);

					// 影の真偽を代入
					if (nShadow == 0)
					{// false
						bShadow = false;
					}
					else if (nShadow == 1)
					{// true
						bShadow = true;
					}

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "COLLISION") == 0)
				{// 当たり判定
					fscanf(pFile, " = %d", &nCollision);

					// 当たり判定の真偽を代入
					if (nCollision == 0)
					{// false
						bCollision = false;
					}
					else if (nCollision == 1)
					{// true
						bCollision = true;
					}

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (nIdx == 8)
				{// タコつぼ
					SetPot(pos, D3DXVECTOR3(D3DX_PI * rot.x / 180.0f, D3DX_PI * rot.y / 180.0f, D3DX_PI * rot.z / 180.0f), POTTYPE_NORMAL);
				}
				//else if (nIdx == 9)
				//{// タコつぼ
				//	SetPot(pos, D3DXVECTOR3(D3DX_PI * rot.x / 180.0f, D3DX_PI * rot.y / 180.0f, D3DX_PI * rot.z / 180.0f), POTTYPE_LIE);
				//}
				else
				{// タコつぼ以外
					SetObject(pos, D3DXVECTOR3(D3DX_PI * rot.x / 180.0f, D3DX_PI * rot.y / 180.0f, D3DX_PI * rot.z / 180.0f), nIdx, bCollision);
					bCollision = true;
				}
			}

			if (strcmp(&aString[0], "END_SCRIPT") == 0)
			{// ファイルの読み込みが完了
				break;
			}
		}

		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合
		printf("ファイルを開けませんでした");
	}
}

//=============================================================================
// ステージ読み込み
//=============================================================================
void LoadStage(const char* pStr)
{
	// ローカル変数宣言
	FILE* pFile;
	char aString[256] = {};				// ファイルのテキスト読み込み
	char aTrash[256] = {};				// ゴミ箱
	int nNumTexture = 0;				// テクスチャの読み込み数
	char aTextureName[64][256] = {};	// テクスチャの名前
	int nNumModel = 0;					// モデルの読み込み数
	char aModelName[64][256] = {};		// モデルの名前

	// セット用の変数
	int nIdx = 0;			// テクスチャやモデルの種類
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	int nBlockX = 0;		// 分割数(X)
	int nBlockZ = 0;		// 分割数(Z)
	float fWidth = 0.0f;	// 大きさ(X)(幅)
	float fDepth = 0.0f;	// 大きさ(Z)(奥行)

	pFile = fopen(pStr, "r");

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

			if (strcmp(&aString[0], "NUM_MODEL") == 0)
			{// モデル数の読み込み
				fscanf(pFile, " = %d", &nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
				{
					if (nCntModel > 0)
					{// 2回目以降のMODEL_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "MODEL_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aModelName[nCntModel][0]);		// モデルのパス

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				continue;
			}

			if (strcmp(&aString[0], "FIELDSET") == 0)
			{// 地面の設置
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "TEXTYPE") == 0)
				{// テクスチャやモデルの種類
					fscanf(pFile, " = %d", &nIdx);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "POS") == 0)
				{// 位置
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
				{// 向き
					fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOCK") == 0)
				{// 分割数
					fscanf(pFile, " = %d %d", &nBlockX, &nBlockZ);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SIZE") == 0)
				{// 大きさ
					fscanf(pFile, " = %f %f", &fWidth, &fDepth);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				SetMeshField((MESHFIELDTYPE)nIdx, pos, D3DXVECTOR3(0.0f, D3DX_PI * rot.y / 180.0f, 0.0f), D3DXVECTOR2((float)nBlockX, (float)nBlockZ), D3DXVECTOR2(fWidth / (float)nBlockX, fDepth / (float)nBlockZ));

				continue;
			}

			if (strcmp(&aString[0], "WALLSET") == 0)
			{// 壁の設置
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "TEXTYPE") == 0)
				{// テクスチャやモデルの種類
					fscanf(pFile, " = %d", &nIdx);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "POS") == 0)
				{// 位置
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
				{// 向き
					fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOCK") == 0)
				{// 分割数
					fscanf(pFile, " = %d %d", &nBlockX, &nBlockZ);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SIZE") == 0)
				{// 大きさ
					fscanf(pFile, " = %f %f", &fWidth, &fDepth);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				//SetMeshWall(pos, D3DXVECTOR3(0.0f, D3DX_PI * rot.y / 180.0f, 0.0f), D3DXVECTOR2((float)nBlockX, (float)nBlockZ), D3DXVECTOR2(fWidth / (float)nBlockX, fDepth / (float)nBlockZ));

				continue;
			}

			if (strcmp(&aString[0], "END_SCRIPT") == 0)
			{// ファイルの読み込みが完了
				break;
			}
		}

		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合
		printf("ファイルを開けませんでした");
	}
}

//=============================================================================
// 配置物の取得
//=============================================================================
Object* GetObjectAll(void)
{
	return &g_aObject[0];
}

//=============================================================================
// 配置物モデルの取得
//=============================================================================
ObjectModel* GetObjectModel(void)
{
	return &g_aObjectModel[0];
}