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
#define ALPHA_MINUS				(0.03f)									// アルファ値の減衰

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
	"data\\TEXTURE\\In_the_sea.png",
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
		MeshOrbit* pMeshOrbit = &g_aMeshOrbit[nCntMeshOrbit];

		pMeshOrbit->pVtxBuff = NULL;
		pMeshOrbit->pIdxBuff = NULL;
		pMeshOrbit->aOffset[0] = FIRST_POS;
		pMeshOrbit->aOffset[1] = FIRST_POS;
		pMeshOrbit->aCol[0] = WHITE_VTX;
		pMeshOrbit->aCol[1] = WHITE_VTX;

		for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint += 2)
		{
			pMeshOrbit->aPosPoint[nCntPoint] = pMeshOrbit->aOffset[0];
			pMeshOrbit->aPosPoint[nCntPoint + 1] = pMeshOrbit->aOffset[1];
			pMeshOrbit->aColPoint[nCntPoint] = pMeshOrbit->aCol[0];
			pMeshOrbit->aColPoint[nCntPoint + 1] = pMeshOrbit->aCol[1];
		}

		pMeshOrbit->bUse = false;

		VERTEX_3D* pVtx;					// 頂点情報へのポインタ
		WORD* pIdx;							// インデックス情報へのポインタ

		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_ORBIT_VTX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&pMeshOrbit->pVtxBuff,
			NULL);

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		pMeshOrbit->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点情報の設定
		for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
		{
			// 頂点座標の設定
			pVtx[0].pos = pMeshOrbit->aPosPoint[nCntPoint];

			// rhwの設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラーの設定
			pVtx[0].col = WHITE_VTX;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2((float)(nCntPoint / 2), (float)((nCntPoint % 2) ? 1.0f : 0.0f));

			pVtx++;
		}

		// 頂点バッファをアンロックする
		pMeshOrbit->pVtxBuff->Unlock();

		// インデックスバッファの生成
		pDevice->CreateIndexBuffer(sizeof(WORD) * MAX_ORBIT_VTX,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&pMeshOrbit->pIdxBuff,
			NULL);

		pMeshOrbit->pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
		{
			pIdx[0] = nCntPoint;

			pIdx++;
		}

		pMeshOrbit->pIdxBuff->Unlock();
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

		if (pMeshOrbit->bUse == true)
		{// 使用しているとき
			VERTEX_3D* pVtx;					// 頂点情報へのポインタ

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			pMeshOrbit->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
			{
				pMeshOrbit->aColPoint[nCntPoint].a -= ALPHA_MINUS;

				if (pMeshOrbit->aColPoint[nCntPoint].a < 0.0f)
				{// 最低値
					pMeshOrbit->aColPoint[nCntPoint].a = 0.0f;
				}

				// 頂点カラーの設定
				pVtx[0].col = pMeshOrbit->aColPoint[nCntPoint];

				pVtx++;
			}

			// 頂点バッファをアンロックする
			pMeshOrbit->pVtxBuff->Unlock();
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
	MeshOrbit* pMeshOrbit = GetMeshOrbit();

	for (int nCntMeshOrbit = 0; nCntMeshOrbit < MAX_MESHORBIT; nCntMeshOrbit++, pMeshOrbit++)
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

		if (pMeshOrbit->bUse == true)
		{// 使用しているとき
			// 保存してある頂点座標と頂点カラーをずらす
			for (int nCntPoint = MAX_ORBIT_VTX - 1; nCntPoint >= 3; nCntPoint -= 2)
			{
				pMeshOrbit->aPosPoint[nCntPoint] = pMeshOrbit->aPosPoint[nCntPoint - 2];
				pMeshOrbit->aPosPoint[nCntPoint - 1] = pMeshOrbit->aPosPoint[nCntPoint - 3];
				pMeshOrbit->aColPoint[nCntPoint] = pMeshOrbit->aColPoint[nCntPoint - 2];
				pMeshOrbit->aColPoint[nCntPoint - 1] = pMeshOrbit->aColPoint[nCntPoint - 3];
			}

			D3DXMATRIX mtxParent = pMeshOrbit->pMtxParent;

			// 親のマトリックスにオフセットを掛けて新しい位置を算出
			D3DXVec3TransformCoord(&pMeshOrbit->aPosPoint[0], &pMeshOrbit->aOffset[0], &mtxParent);
			D3DXVec3TransformCoord(&pMeshOrbit->aPosPoint[1], &pMeshOrbit->aOffset[1], &mtxParent);

			// 色の初期値を設定
			pMeshOrbit->aColPoint[0] = pMeshOrbit->aCol[0];
			pMeshOrbit->aColPoint[1] = pMeshOrbit->aCol[1];

			VERTEX_3D* pVtx;					// 頂点情報へのポインタ

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			pMeshOrbit->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
			{
				// 頂点座標の設定
				pVtx[0].pos = pMeshOrbit->aPosPoint[nCntPoint];

				pMeshOrbit->aColPoint[nCntPoint].a -= ALPHA_MINUS;

				if (pMeshOrbit->aColPoint[nCntPoint].a < 0.0f)
				{// 最低値
					pMeshOrbit->aColPoint[nCntPoint].a = 0.0f;
				}

				// 頂点カラーの設定
				pVtx[0].col = pMeshOrbit->aColPoint[nCntPoint];

				pVtx++;
			}

			// 頂点バッファをアンロックする
			pMeshOrbit->pVtxBuff->Unlock();

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pMeshOrbit->mtxWorld);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pMeshOrbit->mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, pMeshOrbit->pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(pMeshOrbit->pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureMeshOrbit[0]);

			// ポリゴンの描画
			int primitiveCount = (MAX_ORBIT_VTX / 2 - 1) * 2;
			HRESULT hr = pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, MAX_ORBIT_VTX, 0, MAX_ORBIT_VTX - 2);
			//pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, MAX_ORBIT_VTX - 2);
			//OutputDebugStringA("Before DrawMeshOrbit\n");
			//HRESULT hr = pDevice->DrawPrimitive(D3DPT_LINELIST, 0, MAX_ORBIT_VTX / 2);
			//OutputDebugStringA("After DrawMeshOrbit\n");

			//if (FAILED(hr))
			//{
			//	char buf[128];
			//	sprintf_s(buf, "DrawMeshOrbit FAILED hr=0x%08X\n", hr);y
			//	OutputDebugStringA(buf);
			//}
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

	MeshOrbit* pMeshOrbit = &g_aMeshOrbit[nIdx];

	if (pMeshOrbit->bUse == false)
	{// 使ってなかったら無視
		return;
	}

	pMeshOrbit->aOffset[0] = Offset0;
	pMeshOrbit->aOffset[1] = Offset1;
	pMeshOrbit->aCol[0] = col0;
	pMeshOrbit->aCol[1] = col1;
	pMeshOrbit->pMtxParent = *pMtxParent;
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

		if (pMeshOrbit->bUse == false)
		{// 使用していない
			pMeshOrbit->bUse = true;
			pMeshOrbit->aOffset[0] = Offset0;
			pMeshOrbit->aOffset[1] = Offset1;
			pMeshOrbit->aCol[0] = col0;
			pMeshOrbit->aCol[1] = col1;
			pMeshOrbit->pMtxParent = *pMtxParent;

			D3DXMATRIX mtxParent = pMeshOrbit->pMtxParent;

			for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint += 2)
			{
				D3DXVec3TransformCoord(&pMeshOrbit->aPosPoint[nCntPoint], &pMeshOrbit->aOffset[0], &mtxParent);
				D3DXVec3TransformCoord(&pMeshOrbit->aPosPoint[nCntPoint + 1], &pMeshOrbit->aOffset[1], &mtxParent);
			}

			//VERTEX_3D* pVtx;					// 頂点情報へのポインタ

			//// 頂点バッファをロックし、頂点情報へのポインタを取得
			//pMeshOrbit->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			//// 頂点情報の設定
			//for (int nCntPoint = 0; nCntPoint < MAX_ORBIT_VTX; nCntPoint++)
			//{
			//	// 頂点座標の設定
			//	pVtx[0].pos = pMeshOrbit->aPosPoint[nCntPoint];

			//	pVtx++;
			//}

			//// 頂点バッファをアンロックする
			//pMeshOrbit->pVtxBuff->Unlock();

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