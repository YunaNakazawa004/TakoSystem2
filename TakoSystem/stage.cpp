//=============================================================================
// 
// ステージ [stage.cpp]
// Author : Mutsuki Uemura
// 
//=============================================================================
#include "stage.h"
#include "camera.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define XSTAGE_FILE				"data\\MODEL\\StageReplica000.x"	// ステージのファイル名

#define STAGE_OCEANCURRENTS_NOM		(0.001f)	// 通常時の海流の速度
#define STAGE_OCEANCURRENTS_WHIRI	(0.1f)		// 渦潮時の海流の速度

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPD3DXMESH g_pMeshStage = NULL;							// メッシュ(頂点情報)へのポインタ
LPD3DXBUFFER g_pBuffMatStage = NULL;					// マテリアルへのポインタ
LPDIRECT3DTEXTURE9 g_apTextureStage[MAX_TEXTURE] = {};	// テクスチャへのポインタ
DWORD g_dwNumMatStage = 0;								// マテリアルの数
D3DXVECTOR3 g_VtxMaxStage, g_VtxMinStage;				// ステージの最大値・最小値
D3DXVECTOR3 g_posStage;									// ステージの位置情報
D3DXVECTOR3 g_moveStage;								// ステージの移動慣性情報
D3DXVECTOR3 g_rotStage;									// ステージの向き情報
D3DXMATRIX g_mtxWorldStage;								// ワールドマトリックス

//=============================================================================
// ステージの初期化処理
//=============================================================================
void InitStage(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	int nNumVtx;			// 頂点数
	DWORD dwSizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;			// 頂点バッファへのポインタ

	// ステージの情報の初期化
	g_posStage = FIRST_POS;
	g_moveStage = FIRST_POS;
	g_rotStage = FIRST_POS;

	// 最大値最小値の初期化
	g_VtxMaxStage = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
	g_VtxMinStage = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

	// Xファイルの読み込み
	D3DXLoadMeshFromX(XSTAGE_FILE,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&g_pBuffMatStage,
		NULL,
		&g_dwNumMatStage,
		&g_pMeshStage);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)g_pBuffMatStage->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)g_dwNumMatStage; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{// テクスチャファイルが存在する
			D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_apTextureStage[nCntMat]);
		}
	}

	// 頂点数を取得
	nNumVtx = g_pMeshStage->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(g_pMeshStage->GetFVF());

	// 頂点バッファをロック
	g_pMeshStage->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;		// 頂点座標の代入

		if (vtx.x > g_VtxMaxStage.x)
		{// Xの最大値
			g_VtxMaxStage.x = vtx.x;
		}

		if (vtx.x < g_VtxMinStage.x)
		{// Xの最小値
			g_VtxMinStage.x = vtx.x;
		}

		if (vtx.y > g_VtxMaxStage.y)
		{// Yの最大値
			g_VtxMaxStage.y = vtx.y;
		}

		if (vtx.y < g_VtxMinStage.y)
		{// Yの最小値
			g_VtxMinStage.y = vtx.y;
		}

		if (vtx.z > g_VtxMaxStage.z)
		{// Zの最大値
			g_VtxMaxStage.z = vtx.z;
		}

		if (vtx.z < g_VtxMinStage.z)
		{// Zの最小値
			g_VtxMinStage.z = vtx.z;
		}

		pVtxBuff += dwSizeFVF;		// 頂点バッファのサイズ分ポインタを進める
	}

	// 頂点バッファをアンロック
	g_pMeshStage->UnlockVertexBuffer();
}

//=============================================================================
// ステージの終了処理
//=============================================================================
void UninitStage(void)
{
	// メッシュの破棄
	if (g_pMeshStage != NULL)
	{
		g_pMeshStage->Release();
		g_pMeshStage = NULL;
	}

	// マテリアルの破棄
	if (g_pBuffMatStage != NULL)
	{
		g_pBuffMatStage->Release();
		g_pBuffMatStage = NULL;
	}

	// テクスチャの破棄
	for (int nCntStage = 0; nCntStage < (int)g_dwNumMatStage; nCntStage++)
	{
		if (g_apTextureStage[nCntStage] != NULL)
		{
			g_apTextureStage[nCntStage]->Release();
			g_apTextureStage[nCntStage] = NULL;
		}
	}
}

//=============================================================================
// ステージの更新処理
//=============================================================================
void UpdateStage(void)
{
}
//=============================================================================
// ステージの描画処理
//=============================================================================
void DrawStage(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldStage);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotStage.y, g_rotStage.x, g_rotStage.z);
	D3DXMatrixMultiply(&g_mtxWorldStage, &g_mtxWorldStage, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_posStage.x, g_posStage.y, g_posStage.z);
	D3DXMatrixMultiply(&g_mtxWorldStage, &g_mtxWorldStage, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldStage);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)g_pBuffMatStage->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)g_dwNumMatStage; nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureStage[nCntMat]);

		// ステージパーツの描画
		g_pMeshStage->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//=============================================================================
// ステージの当たり判定
//=============================================================================
void CollisionStage(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fWidth, float fDepth)
{
	if ((pPos->x + fWidth > g_posStage.x + g_VtxMinStage.x) &&
		(pPos->x - fWidth < g_posStage.x + g_VtxMaxStage.x) &&
		(g_posStage.z + g_VtxMinStage.z <= pPos->z) &&
		(g_posStage.z + g_VtxMaxStage.z + fDepth >= pPos->z))
	{// 現在の位置がブロックの範囲内
		if (((g_posStage.z + g_VtxMaxStage.z <= pPosOld->z) &&
			(g_posStage.z + g_VtxMaxStage.z >= pPos->z)))
		{// 奥からの当たり判定
			pPos->z = g_posStage.z + g_VtxMaxStage.z;
		}
		else if (((g_posStage.z + g_VtxMinStage.z + fDepth >= pPosOld->z) &&
			(g_posStage.z + g_VtxMinStage.z + fDepth <= pPos->z)))
		{// 手前からの当たり判定
			pPos->z = g_posStage.z + g_VtxMinStage.z + fDepth;
		}
		else if ((g_posStage.x + g_VtxMinStage.x - fWidth >= pPosOld->x) &&
			(g_posStage.x + g_VtxMinStage.x - fWidth <= pPos->x))
		{// 左からの当たり判定
			pPos->x = g_posStage.x + g_VtxMinStage.x - fWidth;
			pMove->x = 0.0f;							// 移動量を0にする
		}
		else if ((g_posStage.x + g_VtxMaxStage.x + fWidth <= pPosOld->x) &&
			(g_posStage.x + g_VtxMaxStage.x + fWidth >= pPos->x))
		{// 右からの当たり判定
			pPos->x = g_posStage.x + g_VtxMaxStage.x + fWidth;
			pMove->x = 0.0f;							// 移動量を0にする
		}
	}
}

//=============================================================================
// 中心を軸にした海流の移動処理
//=============================================================================
void MoveOceanCurrents(D3DXVECTOR3* pPos)
{
	// 変数宣言 ===========================================

	float fDistRadius;	// 中心からの距離(半径)
	float fNomRadius;	// 正規化した距離(半径)
	float fNowAngle;	// 現在の角度

	// ====================================================

	fDistRadius = sqrtf(pPos->x * pPos->x + pPos->z * pPos->z);		// 中心からの距離を求める
	fNomRadius = fDistRadius / MAX_ARIA_STAGE;						// posが中心からどれだけ離れているかを求める
	fNowAngle = atan2(pPos->x, pPos->z);							// 中心からの角度を求める

	// 角度を更新
	fNowAngle += STAGE_OCEANCURRENTS_NOM / fNomRadius;				// 移動量(角度)を正規化した距離の長さにする

	// 角度が超えた場合、角度を範囲内に修正
	if		(fNowAngle < -D3DX_PI) fNowAngle + D3DX_PI * 2;
	else if (fNowAngle >  D3DX_PI) fNowAngle - D3DX_PI * 2;
	
	// 位置を設定
	pPos->x = sinf(fNowAngle) * fDistRadius;
	pPos->z = cosf(fNowAngle) * fDistRadius;
}