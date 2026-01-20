//=============================================================================
// 
// 生き物 [model.cpp]   ! ! このcppは呼び出さないでください ! !
// Author : 中澤優奈
// 
//=============================================================================
#include "model.h"
#include "camera.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define INERTIA_MOVE			(0.4f)									// 移動の慣性
#define XMODEL_FILE				"data\\MODEL\\car000.x"					// 生き物のファイル名

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPD3DXMESH g_pMeshModel = NULL;							// メッシュ(頂点情報)へのポインタ
LPD3DXBUFFER g_pBuffMatModel = NULL;					// マテリアルへのポインタ
LPDIRECT3DTEXTURE9 g_apTextureModel[MAX_TEXTURE] = {};	// テクスチャへのポインタ
DWORD g_dwNumMatModel = 0;								// マテリアルの数
D3DXVECTOR3 g_VtxMaxModel, g_VtxMinModel;				// 生き物の最大値・最小値
D3DXVECTOR3 g_posModel;									// 生き物の位置情報
D3DXVECTOR3 g_moveModel;								// 生き物の移動慣性情報
D3DXVECTOR3 g_rotModel;									// 生き物の向き情報
D3DXMATRIX g_mtxWorldModel;								// ワールドマトリックス

//=============================================================================
// 生き物の初期化処理
//=============================================================================
void InitModel(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	int nNumVtx;			// 頂点数
	DWORD dwSizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;			// 頂点バッファへのポインタ

	// 生き物の情報の初期化
	g_posModel = FIRST_POS;
	g_moveModel = FIRST_POS;
	g_rotModel = FIRST_POS;

	// 最大値最小値の初期化
	g_VtxMaxModel = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
	g_VtxMinModel = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

	// Xファイルの読み込み
	D3DXLoadMeshFromX(XMODEL_FILE,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&g_pBuffMatModel,
		NULL,
		&g_dwNumMatModel,
		&g_pMeshModel);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)g_pBuffMatModel->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)g_dwNumMatModel; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{// テクスチャファイルが存在する
			D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_apTextureModel[nCntMat]);
		}
	}

	// 頂点数を取得
	nNumVtx = g_pMeshModel->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(g_pMeshModel->GetFVF());

	// 頂点バッファをロック
	g_pMeshModel->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;		// 頂点座標の代入

		if (vtx.x > g_VtxMaxModel.x)
		{// Xの最大値
			g_VtxMaxModel.x = vtx.x;
		}

		if (vtx.x < g_VtxMinModel.x)
		{// Xの最小値
			g_VtxMinModel.x = vtx.x;
		}

		if (vtx.y > g_VtxMaxModel.y)
		{// Yの最大値
			g_VtxMaxModel.y = vtx.y;
		}

		if (vtx.y < g_VtxMinModel.y)
		{// Yの最小値
			g_VtxMinModel.y = vtx.y;
		}

		if (vtx.z > g_VtxMaxModel.z)
		{// Zの最大値
			g_VtxMaxModel.z = vtx.z;
		}

		if (vtx.z < g_VtxMinModel.z)
		{// Zの最小値
			g_VtxMinModel.z = vtx.z;
		}

		pVtxBuff += dwSizeFVF;		// 頂点バッファのサイズ分ポインタを進める
	}

	// 頂点バッファをアンロック
	g_pMeshModel->UnlockVertexBuffer();
}

//=============================================================================
// 生き物の終了処理
//=============================================================================
void UninitModel(void)
{
	// メッシュの破棄
	if (g_pMeshModel != NULL)
	{
		g_pMeshModel->Release();
		g_pMeshModel = NULL;
	}

	// マテリアルの破棄
	if (g_pBuffMatModel != NULL)
	{
		g_pBuffMatModel->Release();
		g_pBuffMatModel = NULL;
	}

	// テクスチャの破棄
	for (int nCntModel = 0; nCntModel < (int)g_dwNumMatModel; nCntModel++)
	{
		if (g_apTextureModel[nCntModel] != NULL)
		{
			g_apTextureModel[nCntModel]->Release();
			g_apTextureModel[nCntModel] = NULL;
		}
	}
}

//=============================================================================
// 生き物の更新処理
//=============================================================================
void UpdateModel(void)
{
	Camera* pCamera = GetCamera();

	// 移動
	if (GetKeyboardPress(DIK_UP) == true)
	{// 奥に移動
		g_moveModel.x += sinf(D3DX_PI * 0.0f + pCamera->rot.y) * MOVEMENT.x;
		g_moveModel.z += cosf(D3DX_PI * 0.0f + pCamera->rot.y) * MOVEMENT.z;

		g_rotModel.y = pCamera->rot.y - D3DX_PI;
	}
	else if (GetKeyboardPress(DIK_DOWN) == true)
	{// 手前に移動
		g_moveModel.x += sinf(D3DX_PI * 1.0f + pCamera->rot.y) * MOVEMENT.x;
		g_moveModel.z += cosf(D3DX_PI * 1.0f + pCamera->rot.y) * MOVEMENT.z;

		g_rotModel.y = pCamera->rot.y;
	}

	if (GetKeyboardPress(DIK_LEFT) == true)
	{// 左に移動
		g_moveModel.x += sinf(-D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.x;
		g_moveModel.z += cosf(-D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.z;

		g_rotModel.y = pCamera->rot.y + (D3DX_PI * 0.5f);
	}
	else if (GetKeyboardPress(DIK_RIGHT) == true)
	{// 右に移動
		g_moveModel.x += sinf(D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.x;
		g_moveModel.z += cosf(D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.z;

		g_rotModel.y = pCamera->rot.y - (D3DX_PI * 0.5f);
	}

	// 慣性
	g_posModel += g_moveModel;
	g_moveModel.x += (0.0f - g_moveModel.x) * INERTIA_MOVE;
	g_moveModel.z += (0.0f - g_moveModel.z) * INERTIA_MOVE;

	if (GetKeyboardPress(DIK_LSHIFT) == true)
	{// Y軸回転
		g_rotModel.y += -ROT.y;
	}
	else if (GetKeyboardPress(DIK_RSHIFT) == true)
	{// Y軸回転
		g_rotModel.y += ROT.y;
	}

	// 位置回転リセット
	if (GetKeyboardPress(DIK_RETURN) == true)
	{// Y軸回転
		g_posModel = FIRST_POS;
		g_rotModel = FIRST_POS;
	}
#if 0
	if (GetKeyboardPress(DIK_I) == true)
	{// 上に移動
		g_moveModel.y += MOVEMENT.y;
	}
	else if (GetKeyboardPress(DIK_K) == true)
	{// 下に移動
		g_moveModel.y += -MOVEMENT.y;
	}

	// 回転
	if (GetKeyboardPress(DIK_UP) == true)
	{// X軸回転
		g_rotModel.x += ROT.x;
	}
	else if (GetKeyboardPress(DIK_DOWN) == true)
	{// X軸回転
		g_rotModel.x += -ROT.x;
	}

	if (GetKeyboardPress(DIK_LEFT) == true)
	{// Z軸回転
		g_rotModel.z += ROT.z;
	}
	else if (GetKeyboardPress(DIK_RIGHT) == true)
	{// Z軸回転
		g_rotModel.z += -ROT.z;
	}

#endif
}

//=============================================================================
// 生き物の描画処理
//=============================================================================
void DrawModel(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldModel);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotModel.y, g_rotModel.x, g_rotModel.z);
	D3DXMatrixMultiply(&g_mtxWorldModel, &g_mtxWorldModel, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_posModel.x, g_posModel.y, g_posModel.z);
	D3DXMatrixMultiply(&g_mtxWorldModel, &g_mtxWorldModel, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldModel);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)g_pBuffMatModel->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)g_dwNumMatModel; nCntMat++)
	{
		// マテリアルの設定
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureModel[nCntMat]);

		// 生き物パーツの描画
		g_pMeshModel->DrawSubset(nCntMat);
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}

//=============================================================================
// 生き物の当たり判定
//=============================================================================
void CollisionModel(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fWidth, float fDepth)
{
	if ((pPos->x + fWidth > g_posModel.x + g_VtxMinModel.x) &&
		(pPos->x - fWidth < g_posModel.x + g_VtxMaxModel.x) &&
		(g_posModel.z + g_VtxMinModel.z <= pPos->z) &&
		(g_posModel.z + g_VtxMaxModel.z + fDepth >= pPos->z))
	{// 現在の位置がブロックの範囲内
		if (((g_posModel.z + g_VtxMaxModel.z <= pPosOld->z) &&
			(g_posModel.z + g_VtxMaxModel.z >= pPos->z)))
		{// 奥からの当たり判定
			pPos->z = g_posModel.z + g_VtxMaxModel.z;
		}
		else if (((g_posModel.z + g_VtxMinModel.z + fDepth >= pPosOld->z) &&
			(g_posModel.z + g_VtxMinModel.z + fDepth <= pPos->z)))
		{// 手前からの当たり判定
			pPos->z = g_posModel.z + g_VtxMinModel.z + fDepth;
		}
		else if ((g_posModel.x + g_VtxMinModel.x - fWidth >= pPosOld->x) &&
			(g_posModel.x + g_VtxMinModel.x - fWidth <= pPos->x))
		{// 左からの当たり判定
			pPos->x = g_posModel.x + g_VtxMinModel.x - fWidth;
			pMove->x = 0.0f;							// 移動量を0にする
		}
		else if ((g_posModel.x + g_VtxMaxModel.x + fWidth <= pPosOld->x) &&
			(g_posModel.x + g_VtxMaxModel.x + fWidth >= pPos->x))
		{// 右からの当たり判定
			pPos->x = g_posModel.x + g_VtxMaxModel.x + fWidth;
			pMove->x = 0.0f;							// 移動量を0にする
		}
	}
}