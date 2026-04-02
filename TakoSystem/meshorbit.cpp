//=============================================================================
// 
// メッシュオービット [meshorbit.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "meshorbit.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MESHORBIT			(128)									// メッシュオービットの数
#define ALPHA_MINUS				(0.03f)								// アルファ値の減衰

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureMeshOrbit[1] = {};				// テクスチャへのポインタ
MeshOrbit g_aMeshOrbit[MAX_MESHORBIT];			// メッシュオービットの情報

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilenameMeshOrbit[1] =
{
	"data\\TEXTURE\\In_the_sea000.jpg",
};

//=============================================================================
// メッシュオービットの初期化処理
//=============================================================================
void InitMeshOrbit(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	// テクスチャの読み込み
	for (int nCntTex = 0; nCntTex < 1; nCntTex++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilenameMeshOrbit[nCntTex], &g_apTextureMeshOrbit[nCntTex]);
	}

	// メッシュオービット情報の初期化
	for (int nCntMeshOrbit = 0; nCntMeshOrbit < MAX_MESHORBIT; nCntMeshOrbit++)
	{
		g_aMeshOrbit[nCntMeshOrbit].pVtxBuff = NULL;
		g_aMeshOrbit[nCntMeshOrbit].pIdxBuff = NULL;
		g_aMeshOrbit[nCntMeshOrbit].aOffset[0] = FIRST_POS;
		g_aMeshOrbit[nCntMeshOrbit].aOffset[1] = FIRST_POS;
		g_aMeshOrbit[nCntMeshOrbit].aCol[0] = WHITE_VTX;
		g_aMeshOrbit[nCntMeshOrbit].aCol[1] = WHITE_VTX;

		for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint += 2)
		{
			g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint] = g_aMeshOrbit[nCntMeshOrbit].aOffset[0];
			g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint + 1] = g_aMeshOrbit[nCntMeshOrbit].aOffset[1];
			g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint] = g_aMeshOrbit[nCntMeshOrbit].aCol[0];
			g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint + 1] = g_aMeshOrbit[nCntMeshOrbit].aCol[1];
		}

		g_aMeshOrbit[nCntMeshOrbit].bUse = false;

		VERTEX_3D* pVtx;					// 頂点情報へのポインタ
		WORD* pIdx;							// インデックス情報へのポインタ

		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_ORBIT_VTX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_aMeshOrbit[nCntMeshOrbit].pVtxBuff,
			NULL);

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点情報の設定
		for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
		{
			// 頂点座標の設定
			pVtx[0].pos = g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint];

			// rhwの設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラーの設定
			pVtx[0].col = WHITE_VTX;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2((float)(nCntPoint / 2), (float)((nCntPoint % 2) ? 1.0f : 0.0f));

			pVtx++;
		}

		// 頂点バッファをアンロックする
		g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Unlock();

		// インデックスバッファの生成
		pDevice->CreateIndexBuffer(sizeof(WORD) * MAX_ORBIT_VTX,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&g_aMeshOrbit[nCntMeshOrbit].pIdxBuff,
			NULL);

		g_aMeshOrbit[nCntMeshOrbit].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
		{
			pIdx[0] = (WORD)nCntPoint;

			pIdx++;
		}

		g_aMeshOrbit[nCntMeshOrbit].pIdxBuff->Unlock();
	}
}

//=============================================================================
// メッシュオービットの終了処理
//=============================================================================
void UninitMeshOrbit(void)
{
	// テクスチャの破棄
	for (int nCntTex = 0; nCntTex < 1; nCntTex++)
	{
		if (g_apTextureMeshOrbit[nCntTex] != NULL)
		{
			g_apTextureMeshOrbit[nCntTex]->Release();
			g_apTextureMeshOrbit[nCntTex] = NULL;
		}
	}

	for (int nCntMeshOrbit = 0; nCntMeshOrbit < MAX_MESHORBIT; nCntMeshOrbit++)
	{
		// 頂点バッファの破棄
		if (g_aMeshOrbit[nCntMeshOrbit].pVtxBuff != NULL)
		{
			g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Release();
			g_aMeshOrbit[nCntMeshOrbit].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_aMeshOrbit[nCntMeshOrbit].pIdxBuff != NULL)
		{
			g_aMeshOrbit[nCntMeshOrbit].pIdxBuff->Release();
			g_aMeshOrbit[nCntMeshOrbit].pIdxBuff = NULL;
		}

		g_aMeshOrbit[nCntMeshOrbit].bUse = false;
	}
}

//=============================================================================
// メッシュオービットの更新処理
//=============================================================================
void UpdateMeshOrbit(void)
{
#if 0
	for (int nCntMeshOrbit = 0; nCntMeshOrbit < MAX_MESHORBIT; nCntMeshOrbit++)
	{
		MeshOrbit* pMeshOrbit = &g_aMeshOrbit[nCntMeshOrbit];

		if (g_aMeshOrbit[nCntMeshOrbit].bUse == true)
		{// 使用しているとき
			VERTEX_3D* pVtx;					// 頂点情報へのポインタ

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
			{
				g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint].a -= ALPHA_MINUS;

				if (g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint].a < 0.0f)
				{// 最低値
					g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint].a = 0.0f;
				}

				// 頂点カラーの設定
				pVtx[0].col = g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint];

				pVtx++;
			}

			// 頂点バッファをアンロックする
			g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Unlock();
		}
	}
#endif 
}

//=============================================================================
// メッシュオービットの描画処理
//=============================================================================
void DrawMeshOrbit(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	for (int nCntMeshOrbit = 0; nCntMeshOrbit < MAX_MESHORBIT; nCntMeshOrbit++)
	{
		// アルファテストを有効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

		// Zテストを無効にする
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		// レンダーステートを加算合成にする
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		if (g_aMeshOrbit[nCntMeshOrbit].bUse == true)
		{// 使用しているとき
			// 保存してある頂点座標と頂点カラーをずらす
			for (int nCntPoint = MAX_ORBIT_VTX - 1; nCntPoint >= 3; nCntPoint -= 2)
			{
				g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint] = g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint - 2];
				g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint - 1] = g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint - 3];
				g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint] = g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint - 2];
				g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint - 1] = g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint - 3];
			}

			D3DXMATRIX mtxParent = g_aMeshOrbit[nCntMeshOrbit].pMtxParent;

			// 親のマトリックスにオフセットを掛けて新しい位置を算出
			D3DXVec3TransformCoord(&g_aMeshOrbit[nCntMeshOrbit].aPosPoint[0], &g_aMeshOrbit[nCntMeshOrbit].aOffset[0], &mtxParent);
			D3DXVec3TransformCoord(&g_aMeshOrbit[nCntMeshOrbit].aPosPoint[1], &g_aMeshOrbit[nCntMeshOrbit].aOffset[1], &mtxParent);

			// 色の初期値を設定
			g_aMeshOrbit[nCntMeshOrbit].aColPoint[0] = g_aMeshOrbit[nCntMeshOrbit].aCol[0];
			g_aMeshOrbit[nCntMeshOrbit].aColPoint[1] = g_aMeshOrbit[nCntMeshOrbit].aCol[1];

			VERTEX_3D* pVtx;					// 頂点情報へのポインタ

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
			{
				// 頂点座標の設定
				pVtx[0].pos = g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint];

				g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint].a -= ALPHA_MINUS;

				if (g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint].a < 0.0f)
				{// 最低値
					g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint].a = 0.0f;
				}

				// 頂点カラーの設定
				pVtx[0].col = g_aMeshOrbit[nCntMeshOrbit].aColPoint[nCntPoint];

				pVtx++;
			}

			// 頂点バッファをアンロックする
			g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Unlock();

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshOrbit[nCntMeshOrbit].mtxWorld);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshOrbit[nCntMeshOrbit].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aMeshOrbit[nCntMeshOrbit].pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_aMeshOrbit[nCntMeshOrbit].pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, NULL);

			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, MAX_ORBIT_VTX, 0, MAX_ORBIT_VTX - 2);
		}

		// レンダーステートを元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		// Zテストを有効にする
		pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		// アルファテストを無効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	}
}

//=============================================================================
// メッシュオービットの場所設定処理
//=============================================================================
void SetMeshOrbitPos(int nIdx, D3DXVECTOR3 Offset0, D3DXVECTOR3 Offset1, D3DXCOLOR col0, D3DXCOLOR col1, D3DMATRIX* pMtxParent)
{
	if (nIdx < 0 || nIdx >= MAX_MESHORBIT)
	{// インデックスがない場合は無視
		OutputDebugStringA("SetMeshOrbitPos: invalid idx\n");
		return;
	}

	if (g_aMeshOrbit[nIdx].bUse == false)
	{// 使ってなかったら無視
		return;
	}

	g_aMeshOrbit[nIdx].aOffset[0] = Offset0;
	g_aMeshOrbit[nIdx].aOffset[1] = Offset1;
	g_aMeshOrbit[nIdx].aCol[0] = col0;
	g_aMeshOrbit[nIdx].aCol[1] = col1;
	g_aMeshOrbit[nIdx].pMtxParent = *pMtxParent;
}

//=============================================================================
// メッシュオービットの設定処理
//=============================================================================
int SetMeshOrbit(D3DXVECTOR3 Offset0, D3DXVECTOR3 Offset1, D3DXCOLOR col0, D3DXCOLOR col1, D3DMATRIX* pMtxParent)
{
	int nCntMeshOrbit = -1;

	for (nCntMeshOrbit = 0; nCntMeshOrbit < MAX_MESHORBIT; nCntMeshOrbit++)
	{
		MeshOrbit* pMeshOrbit = &g_aMeshOrbit[nCntMeshOrbit];

		if (g_aMeshOrbit[nCntMeshOrbit].bUse == false)
		{// 使用していない
			g_aMeshOrbit[nCntMeshOrbit].bUse = true;
			g_aMeshOrbit[nCntMeshOrbit].aOffset[0] = Offset0;
			g_aMeshOrbit[nCntMeshOrbit].aOffset[1] = Offset1;
			g_aMeshOrbit[nCntMeshOrbit].aCol[0] = col0;
			g_aMeshOrbit[nCntMeshOrbit].aCol[1] = col1;
			g_aMeshOrbit[nCntMeshOrbit].pMtxParent = *pMtxParent;

			D3DXMATRIX mtxParent = g_aMeshOrbit[nCntMeshOrbit].pMtxParent;

			for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint += 2)
			{
				D3DXVec3TransformCoord(&g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint], &g_aMeshOrbit[nCntMeshOrbit].aOffset[0], &mtxParent);
				D3DXVec3TransformCoord(&g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint + 1], &g_aMeshOrbit[nCntMeshOrbit].aOffset[1], &mtxParent);
			}

			//VERTEX_3D* pVtx;					// 頂点情報へのポインタ

			//// 頂点バッファをロックし、頂点情報へのポインタを取得
			//g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			//// 頂点情報の設定
			//for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
			//{
			//	// 頂点座標の設定
			//	pVtx[0].pos = g_aMeshOrbit[nCntMeshOrbit].aPosPoint[nCntPoint];

			//	pVtx++;
			//}

			//// 頂点バッファをアンロックする
			//g_aMeshOrbit[nCntMeshOrbit].pVtxBuff->Unlock();

			return nCntMeshOrbit;
		}
	}

	return -1;
}

//=============================================================================
// メッシュオービットの取得
//=============================================================================
MeshOrbit* GetMeshOrbit(void)
{
	return &g_aMeshOrbit[0];
}

//=============================================================================
// メッシュオービットの破棄
//=============================================================================
void DeleteMeshOrbit(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_MESHORBIT)
	{// インデックスがない場合は無視
		OutputDebugStringA("DeleteMeshOrbitPos: invalid idx\n");
		return;
	}

	g_aMeshOrbit[nIdx].bUse = false;
}