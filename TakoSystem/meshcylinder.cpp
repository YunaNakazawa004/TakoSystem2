//=============================================================================
// 
// メッシュシリンダー [meshcylinder.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "meshcylinder.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MESHCYLINDER		(128)									// メッシュシリンダーの数
#define HEIGHT_MESHCYLINDER		(80.0f)									// メッシュシリンダーの高さ
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define FIRST_SIZE				(D3DXVECTOR2(0.0f, 0.0f))				// 初期サイズ
#define MESHCYLINDER_TEX		"data\\TEXTURE\\ski000.jpg"				// メッシュシリンダーのテクスチャ

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureMeshCylinder = NULL;				// テクスチャへのポインタ
MeshCylinder g_aMeshCylinder[MAX_MESHCYLINDER];			// メッシュシリンダーの情報

//=============================================================================
// メッシュシリンダーの初期化処理
//=============================================================================
void InitMeshCylinder(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, MESHCYLINDER_TEX, &g_pTextureMeshCylinder);

	// メッシュシリンダー情報の初期化
	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		g_aMeshCylinder[nCntMeshCylinder].pVtxBuff = NULL;
		g_aMeshCylinder[nCntMeshCylinder].pIdxBuff = NULL;
		g_aMeshCylinder[nCntMeshCylinder].pos = FIRST_POS;
		g_aMeshCylinder[nCntMeshCylinder].col = WHITE_VTX;
		g_aMeshCylinder[nCntMeshCylinder].rot = FIRST_POS;
		g_aMeshCylinder[nCntMeshCylinder].block = FIRST_SIZE;
		g_aMeshCylinder[nCntMeshCylinder].size = FIRST_SIZE;
		g_aMeshCylinder[nCntMeshCylinder].bInside = false;
		g_aMeshCylinder[nCntMeshCylinder].bUse = false;
	}
}

//=============================================================================
// メッシュシリンダーの終了処理
//=============================================================================
void UninitMeshCylinder(void)
{
	// テクスチャの破棄
	if (g_pTextureMeshCylinder != NULL)
	{
		g_pTextureMeshCylinder->Release();
		g_pTextureMeshCylinder = NULL;
	}

	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		// 頂点バッファの破棄
		if (g_aMeshCylinder[nCntMeshCylinder].pVtxBuff != NULL)
		{
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff->Release();
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_aMeshCylinder[nCntMeshCylinder].pIdxBuff != NULL)
		{
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuff->Release();
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuff = NULL;
		}
	}
}

//=============================================================================
// メッシュシリンダーの更新処理
//=============================================================================
void UpdateMeshCylinder(void)
{
}

//=============================================================================
// メッシュシリンダーの描画処理
//=============================================================================
void DrawMeshCylinder(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス

	for (int nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		if (g_aMeshCylinder[nCntMeshCylinder].bUse == true)
		{// 使用しているとき
			//// アルファテストを有効にする
			//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

			//// Zテストを無効にする
			//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
			//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

			//// レンダーステートを加算合成にする
			//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshCylinder[nCntMeshCylinder].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshCylinder[nCntMeshCylinder].rot.y, g_aMeshCylinder[nCntMeshCylinder].rot.x, g_aMeshCylinder[nCntMeshCylinder].rot.z);
			D3DXMatrixMultiply(&g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshCylinder[nCntMeshCylinder].pos.x, g_aMeshCylinder[nCntMeshCylinder].pos.y, g_aMeshCylinder[nCntMeshCylinder].pos.z);
			D3DXMatrixMultiply(&g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &g_aMeshCylinder[nCntMeshCylinder].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshCylinder[nCntMeshCylinder].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aMeshCylinder[nCntMeshCylinder].pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_aMeshCylinder[nCntMeshCylinder].pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, NULL);

			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, ((int)g_aMeshCylinder[nCntMeshCylinder].block.x + 1) * ((int)g_aMeshCylinder[nCntMeshCylinder].block.y + 1), 0,
				(((int)g_aMeshCylinder[nCntMeshCylinder].block.x) * ((int)g_aMeshCylinder[nCntMeshCylinder].block.y) * 2) + (((int)g_aMeshCylinder[nCntMeshCylinder].block.y - 1) * 4));

			//// レンダーステートを元に戻す
			//pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			//pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			//pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			//// Zテストを有効にする
			//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

			//// アルファテストを無効にする
			//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
			//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		}
	}
}

//=============================================================================
// メッシュシリンダーの設定処理
//=============================================================================
int SetMeshCylinder(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size, D3DXCOLOR col, bool bInside)
{
	int nCntMeshCylinder = -1;

	for (nCntMeshCylinder = 0; nCntMeshCylinder < MAX_MESHCYLINDER; nCntMeshCylinder++)
	{
		if (g_aMeshCylinder[nCntMeshCylinder].bUse == false)
		{// 使用していない
			g_aMeshCylinder[nCntMeshCylinder].pos = pos;
			g_aMeshCylinder[nCntMeshCylinder].col = col;
			g_aMeshCylinder[nCntMeshCylinder].rot = rot;
			g_aMeshCylinder[nCntMeshCylinder].block = block;
			g_aMeshCylinder[nCntMeshCylinder].size = size;
			g_aMeshCylinder[nCntMeshCylinder].bInside = bInside;
			g_aMeshCylinder[nCntMeshCylinder].bUse = true;

			LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
			VERTEX_3D* pVtx;					// 頂点情報へのポインタ
			WORD* pIdx;							// インデックス情報へのポインタ

			// 頂点バッファの生成
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * ((int)g_aMeshCylinder[nCntMeshCylinder].block.x + 1) * ((int)g_aMeshCylinder[nCntMeshCylinder].block.y + 1),
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshCylinder[nCntMeshCylinder].pVtxBuff,
				NULL);

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntMeshCylinder1 = 0; nCntMeshCylinder1 < (int)g_aMeshCylinder[nCntMeshCylinder].block.y + 1; nCntMeshCylinder1++)
			{
				for (int nCntMeshCylinder2 = 0; nCntMeshCylinder2 < (int)g_aMeshCylinder[nCntMeshCylinder].block.x + 1; nCntMeshCylinder2++)
				{
					// 頂点座標の設定
					if (bInside == true)
					{// 内側
						pVtx[0].pos.x = sinf(nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / g_aMeshCylinder[nCntMeshCylinder].block.x)) * g_aMeshCylinder[nCntMeshCylinder].size.x;
						pVtx[0].pos.y = g_aMeshCylinder[nCntMeshCylinder].size.y - ((g_aMeshCylinder[nCntMeshCylinder].size.y / g_aMeshCylinder[nCntMeshCylinder].block.y) * nCntMeshCylinder1);
						pVtx[0].pos.z = cosf(nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / g_aMeshCylinder[nCntMeshCylinder].block.x)) * g_aMeshCylinder[nCntMeshCylinder].size.x;
					}
					else
					{// 外側
						pVtx[0].pos.x = sinf(-nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / g_aMeshCylinder[nCntMeshCylinder].block.x)) * g_aMeshCylinder[nCntMeshCylinder].size.x;
						pVtx[0].pos.y = g_aMeshCylinder[nCntMeshCylinder].size.y - ((g_aMeshCylinder[nCntMeshCylinder].size.y / g_aMeshCylinder[nCntMeshCylinder].block.y) * nCntMeshCylinder1);
						pVtx[0].pos.z = cosf(-nCntMeshCylinder2 * ((D3DX_PI * 2.0f) / g_aMeshCylinder[nCntMeshCylinder].block.x)) * g_aMeshCylinder[nCntMeshCylinder].size.x;
					}

					// rhwの設定
					pVtx[0].nor = D3DXVECTOR3(pVtx[0].pos.x, 0.0f, pVtx[0].pos.z);
					D3DXVec3Normalize(&pVtx[0].nor, &pVtx[0].nor);

					// 頂点カラーの設定
					pVtx[0].col = col;

					// テクスチャ座標の設定
					pVtx[0].tex = D3DXVECTOR2((float)nCntMeshCylinder2, (float)nCntMeshCylinder1);

					pVtx++;
				}
			}

			// 頂点バッファをアンロックする
			g_aMeshCylinder[nCntMeshCylinder].pVtxBuff->Unlock();

			// インデックスバッファの数
			int nNumIdx = (((int)g_aMeshCylinder[nCntMeshCylinder].block.x) * ((int)g_aMeshCylinder[nCntMeshCylinder].block.y) * 2) + (((int)g_aMeshCylinder[nCntMeshCylinder].block.y - 1) * 4) + 2;

			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * nNumIdx,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshCylinder[nCntMeshCylinder].pIdxBuff,
				NULL);

			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

			int nNum = 0;			// 縮退ポリゴン

			// 頂点番号データの設定
			for (int nCntMeshCylinder1 = 0; nCntMeshCylinder1 < nNumIdx / 2; nCntMeshCylinder1++)
			{
				if (nCntMeshCylinder1 % ((int)g_aMeshCylinder[nCntMeshCylinder].block.x + 2) == ((int)g_aMeshCylinder[nCntMeshCylinder].block.x + 1))
				{// 縮退ポリゴンのところ
					nNum++;

					pIdx[0] = nCntMeshCylinder1 - nNum;
					pIdx[1] = nCntMeshCylinder1 + ((int)g_aMeshCylinder[nCntMeshCylinder].block.x + 1);
				}
				else
				{// 縮退以外のポリゴン
					pIdx[0] = (nCntMeshCylinder1 - nNum) + ((int)g_aMeshCylinder[nCntMeshCylinder].block.x + 1);
					pIdx[1] = (nCntMeshCylinder1 - nNum);
				}

				pIdx += 2;
			}

			// インデックスバッファをアンロックする
			g_aMeshCylinder[nCntMeshCylinder].pIdxBuff->Unlock();

			break;
		}
	}

	return nCntMeshCylinder;
}

//=============================================================================
// メッシュシリンダーの当たり判定
//=============================================================================
void CollisionMeshCylinder(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fRadius, float fHeight, bool binsec)
{
	MeshCylinder* pMeshC = GetMeshCylinder();

	for (int nCntMeshC = 0; nCntMeshC < MAX_MESHCYLINDER; nCntMeshC++, pMeshC++)
	{
		if (pMeshC->bUse == false)
		{
			continue;
		}

		for (int nCnt = 0; nCnt < pMeshC->block.x; nCnt++)
		{
			PrintDebugProc("[ %d ]\n", nCnt);

			D3DXVECTOR3 start, end;
			float fXS, fZS, fXE, fZE;
			D3DXVECTOR3 vecLine, vecMove, vecToPos, vecToPosOld, vecNor, vecMoveRef, vecMoveDest;		// 各ベクトル
			static D3DXVECTOR3 insec = FIRST_POS;		// 交点
			D3DXVECTOR3 vecLineW, posDest;
			float fRate, fMoveRate, fDot;

			if (pMeshC->bInside == true)
			{// 内側
				fXS = sinf(nCnt * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x - fRadius);
				fZS = cosf(nCnt * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x - fRadius);

				fXE = sinf((nCnt + 1) * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x - fRadius);
				fZE = cosf((nCnt + 1) * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x - fRadius);
			}
			else
			{// 外側
				fXS = sinf(nCnt * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x + fRadius);
				fZS = cosf(nCnt * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x + fRadius);

				fXE = sinf((nCnt + 1) * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x + fRadius);
				fZE = cosf((nCnt + 1) * ((D3DX_PI * 2.0f) / pMeshC->block.x)) * (pMeshC->size.x + fRadius);
			}

			// 始点
			start.x = pMeshC->pos.x + fXS;
			start.y = 0.0f;
			start.z = pMeshC->pos.z + fZS;

			PrintDebugProc("始点( %f %f %f )\n", start.x, start.y, start.z);

			// 終点
			end.x = pMeshC->pos.x + fXE;
			end.y = 0.0f;
			end.z = pMeshC->pos.z + fZE;

			PrintDebugProc("終点( %f %f %f )\n", end.x, end.y, end.z);

			// 境界線ベクトル
			vecLine.x = (end.x) - (start.x);
			vecLine.y = 0.0f;
			vecLine.z = (end.z) - (start.z);

			PrintDebugProc("境界線ベクトル( %f %f %f )\n", vecLine.x, vecLine.y, vecLine.z);

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
			if (pMeshC->bInside == true)
			{// 内側
				vecNor.x = (vecLine.x * cosf(D3DX_PI * 0.5f)) + (vecLine.z * sinf(D3DX_PI * 0.5f));
				vecNor.y = 0.0f;
				vecNor.z = (vecLine.x * sinf(-D3DX_PI * 0.5f)) - (vecLine.z * cosf(-D3DX_PI * 0.5f));
				D3DXVec3Normalize(&vecNor, &vecNor);		// ベクトルを正規化する
			}
			else
			{// 外側
				vecNor.x = (vecLine.x * cosf(-D3DX_PI * 0.5f)) + (vecLine.z * sinf(-D3DX_PI * 0.5f));
				vecNor.y = 0.0f;
				vecNor.z = (vecLine.x * sinf(D3DX_PI * 0.5f)) - (vecLine.z * cosf(D3DX_PI * 0.5f));
				D3DXVec3Normalize(&vecNor, &vecNor);		// ベクトルを正規化する
			}

			//PrintDebugProc("法線ベクトル( %f %f %f )\n", vecNor.x, vecNor.y, vecNor.z);

			// 内積
			fDot = (-vecMove.x * vecNor.x) + (-vecMove.z * vecNor.z);

			// 外積
			float f = ((vecMove.z * vecNor.x) - (vecMove.x * vecNor.z));

			// 交点の割合
			fRate = ((vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z)) /
				((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

			//PrintDebugProc("fRate : %f\n", fRate);

			// vecMoveの交点の割合
			//fMoveRate = ((vecMove.z * vecToPosOld.x) - (vecMove.x * vecToPosOld.z)) /
			//	(((fRate * vecLine.z) * vecToPosOld.x) - ((fRate * vecLine.x) * vecToPosOld.z));

			//PrintDebugProc("fMoveRate : %f\n", fMoveRate);

			float fAngle;

			// 角度
			if (pMeshC->bInside == true)
			{// 内側
				fAngle = D3DX_PI - atan2f(vecMove.x, vecMove.z);
				float fMeshAngle = atan2f(vecLine.x, vecLine.z);
				fAngle += fMeshAngle;
				fAngle = D3DX_PI - fAngle;
				fAngle += D3DX_PI * 0.5f;
				CorrectAngle(&fAngle, fAngle);
			}
			else
			{// 外側
				fAngle = D3DX_PI - atan2f(vecMove.x, vecMove.z);
				float fMeshAngle = atan2f(vecLine.x, vecLine.z);
				fAngle += fMeshAngle;
				fAngle = D3DX_PI - fAngle;
				fAngle -= D3DX_PI * 0.5f;
				CorrectAngle(&fAngle, fAngle);
			}

			PrintDebugProc("入射角 : %f\n", fAngle);

			// 反射後の移動ベクトル
			vecMoveRef.x = vecMove.x + ((vecNor.x * fDot) * 2);
			vecMoveRef.y = 0.0f;
			vecMoveRef.z = vecMove.z + ((vecNor.z * fDot) * 2);

			// 壁刷りベクトル
			//vecMoveDest.x = (insec.x - pPos->x) + (vecNor.x * fDot * (1.0f - fRate));
			//vecMoveDest.y = 0.0f;
			//vecMoveDest.z = (insec.z - pPos->z) + (vecNor.z * fDot * (1.0f - fRate));
			
			//vecMoveDest.x = -(insec.x - pPos->x);
			//vecMoveDest.y = 0.0f;
			//vecMoveDest.z = -(insec.z - pPos->z);

			if (fRate >= 0.0f && fRate <= 1.0f)
			{// 交点の割合が範囲内
				float fPosLine = (float)((int)(((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z)) * 1000.0f) / (int)1);
				float fPosOldLine = (float)((int)(((vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z)) * 1000.0f) / (int)1);

				if ((pMeshC->bInside == true && fPosLine < 0.0f && fPosOldLine >= 0.0f) ||
					(pMeshC->bInside == false && fPosLine > 0.0f && fPosOldLine <= 0.0f))
				{// 交差した
					if (binsec == true)
					{// 交点出す用
						insec.x = start.x + (vecLine.x * (fRate));
						insec.y = pPos->y;
						insec.z = start.z + (vecLine.z * (fRate));

						//PrintDebugProc("交点( %f %f %f )\n", insec.x, insec.y, insec.z);
					}
					else
					{
						D3DXVECTOR3 vecPosDiff;
						vecPosDiff.x = -(insec.x - pPos->x);
						vecPosDiff.y = 0.0f;
						vecPosDiff.z = -(insec.z - pPos->z);
						D3DXVec3Normalize(&vecPosDiff, &vecPosDiff);		// ベクトルを正規化する

						PrintDebugProc("めり込みベクトル( %f %f %f )\n", vecPosDiff.x, vecPosDiff.y, vecPosDiff.z);

						if (fAngle < 0)
						{// 壁に対して右側から
							vecMoveDest.x = (vecPosDiff.x * cosf(-(D3DX_PI * 0.5f) - fAngle)) + (vecPosDiff.z * sinf(-(D3DX_PI * 0.5f) - fAngle));
							vecMoveDest.y = 0.0f;
							vecMoveDest.z = (vecPosDiff.x * sinf((D3DX_PI * 0.5f) - fAngle)) - (vecPosDiff.z * cosf((D3DX_PI * 0.5f) - fAngle));
						}
						else
						{// 左側から
							vecMoveDest.x = (vecPosDiff.x * cosf((D3DX_PI * 0.5f) - fAngle)) + (vecPosDiff.z * sinf((D3DX_PI * 0.5f) - fAngle));
							vecMoveDest.y = 0.0f;
							vecMoveDest.z = (vecPosDiff.x * sinf(-(D3DX_PI * 0.5f) - fAngle)) - (vecPosDiff.z * cosf(-(D3DX_PI * 0.5f) - fAngle));
						}

						PrintDebugProc("壁刷りベクトル( %f %f %f )\n", vecMoveDest.x, vecMoveDest.y, vecMoveDest.z);

						pPos->x = start.x + (vecLine.x * fRate);
						pPos->z = start.z + (vecLine.z * fRate);

						pMove->x = vecMoveDest.x;
						pMove->z = vecMoveDest.z;
					}
				}
			}
		}
	}
}

//=============================================================================
// メッシュシリンダーの取得
//=============================================================================
MeshCylinder* GetMeshCylinder(void)
{
	return &g_aMeshCylinder[0];
}