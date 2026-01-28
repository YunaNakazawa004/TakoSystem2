//=============================================================================
// 
// メッシュリング [meshring.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "meshring.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MESHRING			(128)									// メッシュリングの数
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define FIRST_SIZE				(D3DXVECTOR2(0.0f, 0.0f))				// 初期サイズ
#define MESHRING_TEX			"data\\TEXTURE\\ski000.jpg"				// メッシュリングのテクスチャ

//*****************************************************************************
// メッシュリングの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;		// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;						// メッシュリングの位置情報
	D3DXVECTOR3 rot;						// メッシュリングの向き情報
	D3DXVECTOR2 block;						// 分割数
	D3DXVECTOR2 size;						// サイズ
	D3DXCOLOR col;							// 色
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	bool bUse;
}MeshRing;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureMeshRing = NULL;				// テクスチャへのポインタ
MeshRing g_aMeshRing[MAX_MESHRING];			// メッシュリングの情報

//=============================================================================
// メッシュリングの初期化処理
//=============================================================================
void InitMeshRing(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, MESHRING_TEX, &g_pTextureMeshRing);

	// メッシュリング情報の初期化
	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		g_aMeshRing[nCntMeshRing].pVtxBuff = NULL;
		g_aMeshRing[nCntMeshRing].pIdxBuff = NULL;
		g_aMeshRing[nCntMeshRing].pos = FIRST_POS;
		g_aMeshRing[nCntMeshRing].rot = FIRST_POS;
		g_aMeshRing[nCntMeshRing].block = FIRST_SIZE;
		g_aMeshRing[nCntMeshRing].size = FIRST_SIZE;
		g_aMeshRing[nCntMeshRing].col = WHITE_VTX;
		g_aMeshRing[nCntMeshRing].bUse = false;
	}
}

//=============================================================================
// メッシュリングの終了処理
//=============================================================================
void UninitMeshRing(void)
{
	// テクスチャの破棄
	if (g_pTextureMeshRing != NULL)
	{
		g_pTextureMeshRing->Release();
		g_pTextureMeshRing = NULL;
	}

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		// 頂点バッファの破棄
		if (g_aMeshRing[nCntMeshRing].pVtxBuff != NULL)
		{
			g_aMeshRing[nCntMeshRing].pVtxBuff->Release();
			g_aMeshRing[nCntMeshRing].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_aMeshRing[nCntMeshRing].pIdxBuff != NULL)
		{
			g_aMeshRing[nCntMeshRing].pIdxBuff->Release();
			g_aMeshRing[nCntMeshRing].pIdxBuff = NULL;
		}
	}
}

//=============================================================================
// メッシュリングの更新処理
//=============================================================================
void UpdateMeshRing(void)
{
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse == true)
		{// 使用しているとき
			// 段々輪を大きく、細く
			g_aMeshRing[nCntMeshRing].size.x += 2.0f;
			g_aMeshRing[nCntMeshRing].size.y -= 1.0f;

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshRing[nCntMeshRing].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntMeshRing1 = 0; nCntMeshRing1 < (int)g_aMeshRing[nCntMeshRing].block.y + 1; nCntMeshRing1++)
			{
				for (int nCntMeshRing2 = 0; nCntMeshRing2 < (int)g_aMeshRing[nCntMeshRing].block.x + 1; nCntMeshRing2++)
				{
					float fAngle = ((D3DX_PI * 2.0f) / g_aMeshRing[nCntMeshRing].block.x);
					float fSize = (g_aMeshRing[nCntMeshRing].size.x + (g_aMeshRing[nCntMeshRing].size.y * -((nCntMeshRing2 + nCntMeshRing1) % 2)));

					// 頂点座標の設定
					pVtx[0].pos.x = sinf(((nCntMeshRing2 + (nCntMeshRing1 * ((int)g_aMeshRing[nCntMeshRing].block.x + 1))) / 2)
						* -fAngle) * fSize;

					pVtx[0].pos.y = 0.0f;

					pVtx[0].pos.z = cosf(((nCntMeshRing2 + (nCntMeshRing1 * ((int)g_aMeshRing[nCntMeshRing].block.x + 1))) / 2)
						* fAngle) * fSize;

					// rhwの設定
					pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

					// 頂点カラーの設定
					pVtx[0].col = g_aMeshRing[nCntMeshRing].col;

					// テクスチャ座標の設定
					pVtx[0].tex = D3DXVECTOR2((float)nCntMeshRing2, (float)nCntMeshRing1);

					pVtx++;
				}
			}

			// 頂点バッファをアンロックする
			g_aMeshRing[nCntMeshRing].pVtxBuff->Unlock();

			if (g_aMeshRing[nCntMeshRing].size.y <= 0.0f)
			{// 輪が見えなくなる
				g_aMeshRing[nCntMeshRing].bUse = false;
			}
		}
	}
}

//=============================================================================
// メッシュリングの描画処理
//=============================================================================
void DrawMeshRing(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse == true)
		{// 使用しているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshRing[nCntMeshRing].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshRing[nCntMeshRing].rot.y, g_aMeshRing[nCntMeshRing].rot.x, g_aMeshRing[nCntMeshRing].rot.z);
			D3DXMatrixMultiply(&g_aMeshRing[nCntMeshRing].mtxWorld, &g_aMeshRing[nCntMeshRing].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshRing[nCntMeshRing].pos.x, g_aMeshRing[nCntMeshRing].pos.y, g_aMeshRing[nCntMeshRing].pos.z);
			D3DXMatrixMultiply(&g_aMeshRing[nCntMeshRing].mtxWorld, &g_aMeshRing[nCntMeshRing].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshRing[nCntMeshRing].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aMeshRing[nCntMeshRing].pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_aMeshRing[nCntMeshRing].pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, NULL);

			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, ((int)g_aMeshRing[nCntMeshRing].block.x + 1) * ((int)g_aMeshRing[nCntMeshRing].block.y + 1), 0,
				(((int)g_aMeshRing[nCntMeshRing].block.x) * ((int)g_aMeshRing[nCntMeshRing].block.y) * 2) + (((int)g_aMeshRing[nCntMeshRing].block.y - 1) * 4));
		}
	}
}

//=============================================================================
// メッシュリングの設定処理
//=============================================================================
void SetMeshRing(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size, D3DXCOLOR col)
{
	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse == false)
		{// 使用していない
			g_aMeshRing[nCntMeshRing].pos.x = pos.x;
			g_aMeshRing[nCntMeshRing].pos.y = 0.0f;
			g_aMeshRing[nCntMeshRing].pos.z = pos.z;
			g_aMeshRing[nCntMeshRing].rot = rot;
			g_aMeshRing[nCntMeshRing].block = block;
			g_aMeshRing[nCntMeshRing].size = size;
			g_aMeshRing[nCntMeshRing].col = col;
			g_aMeshRing[nCntMeshRing].bUse = true;

			LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
			VERTEX_3D* pVtx;					// 頂点情報へのポインタ
			WORD* pIdx;							// インデックス情報へのポインタ

				// 頂点バッファの生成
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * (((int)g_aMeshRing[nCntMeshRing].block.x + 1) * ((int)g_aMeshRing[nCntMeshRing].block.y + 1)),
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshRing[nCntMeshRing].pVtxBuff,
				NULL);

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshRing[nCntMeshRing].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntMeshRing1 = 0; nCntMeshRing1 < (int)g_aMeshRing[nCntMeshRing].block.y + 1; nCntMeshRing1++)
			{
				for (int nCntMeshRing2 = 0; nCntMeshRing2 < (int)g_aMeshRing[nCntMeshRing].block.x + 1; nCntMeshRing2++)
				{
					float fAngle = ((D3DX_PI * 2.0f) / g_aMeshRing[nCntMeshRing].block.x);
					float fSize = (g_aMeshRing[nCntMeshRing].size.x + (g_aMeshRing[nCntMeshRing].size.y * -((nCntMeshRing2 + nCntMeshRing1) % 2)));

					// 頂点座標の設定
					pVtx[0].pos.x = sinf(((nCntMeshRing2 + (nCntMeshRing1 * ((int)g_aMeshRing[nCntMeshRing].block.x + 1))) / 2)
						* -fAngle) * fSize;

					pVtx[0].pos.y = 0.0f;

					pVtx[0].pos.z = cosf(((nCntMeshRing2 + (nCntMeshRing1 * ((int)g_aMeshRing[nCntMeshRing].block.x + 1))) / 2)
						* fAngle) * fSize;

					// rhwの設定
					pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

					// 頂点カラーの設定
					pVtx[0].col = g_aMeshRing[nCntMeshRing].col;

					// テクスチャ座標の設定
					pVtx[0].tex = D3DXVECTOR2((float)nCntMeshRing2, (float)nCntMeshRing1);

					pVtx++;
				}
			}

			// 頂点バッファをアンロックする
			g_aMeshRing[nCntMeshRing].pVtxBuff->Unlock();

			// インデックスバッファの数
			int nNumIdx = (((int)g_aMeshRing[nCntMeshRing].block.x) * ((int)g_aMeshRing[nCntMeshRing].block.y) * 2) + (((int)g_aMeshRing[nCntMeshRing].block.y - 1) * 4) + 2;

			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * nNumIdx,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshRing[nCntMeshRing].pIdxBuff,
				NULL);

			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshRing[nCntMeshRing].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

			int nNum = 0;			// 縮退ポリゴン

			// 頂点番号データの設定
			for (int nCntMeshRing1 = 0; nCntMeshRing1 < nNumIdx; nCntMeshRing1++)
			{
				pIdx[0] = nCntMeshRing1;
				pIdx++;
			}

			// インデックスバッファをアンロックする
			g_aMeshRing[nCntMeshRing].pIdxBuff->Unlock();

			break;
		}
	}
}