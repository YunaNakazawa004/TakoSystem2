//=============================================================================
// 
// メッシュフィールド [meshfield.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "meshfield.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MESHFIELD			(128)									// メッシュフィールドの数
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define FIRST_SIZE				(D3DXVECTOR2(0.0f, 0.0f))				// 初期サイズ

//*****************************************************************************
// メッシュフィールドの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;		// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;						// メッシュフィールドの位置情報
	D3DXVECTOR3 rot;						// メッシュフィールドの向き情報
	D3DXVECTOR2 block;						// 分割数
	D3DXVECTOR2 size;						// サイズ
	MESHFIELDTYPE type;						// 種類
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	bool bUse;
}MeshField;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureMeshField[MESHFIELDTYPE_MAX] = {};				// テクスチャへのポインタ
MeshField g_aMeshField[MAX_MESHFIELD];						// メッシュフィールドの情報

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilenameMeshField[MESHFIELDTYPE_MAX] =
{
	"data\\TEXTURE\\In_the_sea.png",
	"data\\TEXTURE\\suna.png",
};

//=============================================================================
// メッシュフィールドの初期化処理
//=============================================================================
void InitMeshField(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < MESHFIELDTYPE_MAX; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilenameMeshField[nCntTexture], &g_apTextureMeshField[nCntTexture]);
	}

	// メッシュフィールド情報の初期化
	for (int nCntMeshField = 0; nCntMeshField < MAX_MESHFIELD; nCntMeshField++)
	{
		g_aMeshField[nCntMeshField].pVtxBuff = NULL;
		g_aMeshField[nCntMeshField].pIdxBuff = NULL;
		g_aMeshField[nCntMeshField].pos = FIRST_POS;
		g_aMeshField[nCntMeshField].rot = FIRST_POS;
		g_aMeshField[nCntMeshField].block = FIRST_SIZE;
		g_aMeshField[nCntMeshField].size = FIRST_SIZE;
		g_aMeshField[nCntMeshField].type = MESHFIELDTYPE_ROAD;
		g_aMeshField[nCntMeshField].bUse = false;
	}
}

//=============================================================================
// メッシュフィールドの終了処理
//=============================================================================
void UninitMeshField(void)
{
	// テクスチャの破棄
	for (int nCntMeshField = 0; nCntMeshField < MESHFIELDTYPE_MAX; nCntMeshField++)
	{
		if (g_apTextureMeshField[nCntMeshField] != NULL)
		{
			g_apTextureMeshField[nCntMeshField]->Release();
			g_apTextureMeshField[nCntMeshField] = NULL;
		}
	}

	for (int nCntMeshField = 0; nCntMeshField < MAX_MESHFIELD; nCntMeshField++)
	{
		// 頂点バッファの破棄
		if (g_aMeshField[nCntMeshField].pVtxBuff != NULL)
		{
			g_aMeshField[nCntMeshField].pVtxBuff->Release();
			g_aMeshField[nCntMeshField].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_aMeshField[nCntMeshField].pIdxBuff != NULL)
		{
			g_aMeshField[nCntMeshField].pIdxBuff->Release();
			g_aMeshField[nCntMeshField].pIdxBuff = NULL;
		}
	}
}

//=============================================================================
// メッシュフィールドの更新処理
//=============================================================================
void UpdateMeshField(void)
{
}

//=============================================================================
// メッシュフィールドの描画処理
//=============================================================================
void DrawMeshField(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	for (int nCntMeshField = 0; nCntMeshField < MAX_MESHFIELD; nCntMeshField++)
	{
		if (g_aMeshField[nCntMeshField].bUse == true)	
		{// 使用しているとき

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshField[nCntMeshField].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshField[nCntMeshField].rot.y, g_aMeshField[nCntMeshField].rot.x, g_aMeshField[nCntMeshField].rot.z);
			D3DXMatrixMultiply(&g_aMeshField[nCntMeshField].mtxWorld, &g_aMeshField[nCntMeshField].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshField[nCntMeshField].pos.x, g_aMeshField[nCntMeshField].pos.y, g_aMeshField[nCntMeshField].pos.z);
			D3DXMatrixMultiply(&g_aMeshField[nCntMeshField].mtxWorld, &g_aMeshField[nCntMeshField].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshField[nCntMeshField].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aMeshField[nCntMeshField].pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_aMeshField[nCntMeshField].pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			if (g_aMeshField[nCntMeshField].type != -1 && g_aMeshField[nCntMeshField].type < MESHFIELDTYPE_MAX)
			{// テクスチャがある場合

				pDevice->SetTexture(0, g_apTextureMeshField[g_aMeshField[nCntMeshField].type]);
			}
			else
			{

				pDevice->SetTexture(0, NULL);
			}

			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
										  0,
										  0,
										  ((int)g_aMeshField[nCntMeshField].block.x + 1) * ((int)g_aMeshField[nCntMeshField].block.y + 1),
										  0,
										  (((int)g_aMeshField[nCntMeshField].block.x) * ((int)g_aMeshField[nCntMeshField].block.y) * 2) + (((int)g_aMeshField[nCntMeshField].block.y - 1) * 4));

			////OutputDebugStringA("Before DrawMeshField\n");
			//HRESULT hr = pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, ((int)g_aMeshField[nCntMeshField].block.x + 1) * ((int)g_aMeshField[nCntMeshField].block.y + 1), 0,
			//	(((int)g_aMeshField[nCntMeshField].block.x) * ((int)g_aMeshField[nCntMeshField].block.y) * 2));
			////OutputDebugStringA("After DrawMeshField\n");

			//if (FAILED(hr))
			//{
			//	char buf[128];
			//	sprintf_s(buf, "DrawMeshField FAILED hr=0x%08X\n", hr);
			//	OutputDebugStringA(buf);
			//}
		}
	}
}

//=============================================================================
// メッシュフィールドの設定処理
//=============================================================================
void SetMeshField(MESHFIELDTYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size)
{
	for (int nCntMeshField = 0; nCntMeshField < MAX_MESHFIELD; nCntMeshField++)
	{
		if (g_aMeshField[nCntMeshField].bUse == false)
		{// 使用していない
			g_aMeshField[nCntMeshField].pos = pos;
			g_aMeshField[nCntMeshField].rot = rot;
			g_aMeshField[nCntMeshField].block = block;
			g_aMeshField[nCntMeshField].size = size;
			g_aMeshField[nCntMeshField].type = type;
			g_aMeshField[nCntMeshField].bUse = true;

			LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
			VERTEX_3D* pVtx;					// 頂点情報へのポインタ
			WORD* pIdx;							// インデックス情報へのポインタ

			// 頂点バッファの生成
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * ((int)g_aMeshField[nCntMeshField].block.x + 1) * ((int)g_aMeshField[nCntMeshField].block.y + 1),
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshField[nCntMeshField].pVtxBuff,
				NULL);

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshField[nCntMeshField].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntMeshField1 = 0; nCntMeshField1 < (int)g_aMeshField[nCntMeshField].block.y + 1; nCntMeshField1++)
			{
				for (int nCntMeshField2 = 0; nCntMeshField2 < (int)g_aMeshField[nCntMeshField].block.x + 1; nCntMeshField2++)
				{
					// 頂点座標の設定
					pVtx[0].pos.x = -((g_aMeshField[nCntMeshField].block.x * g_aMeshField[nCntMeshField].size.x) * 0.5f) + (nCntMeshField2 * g_aMeshField[nCntMeshField].size.x);
					pVtx[0].pos.y = 0.0f;
					pVtx[0].pos.z = ((g_aMeshField[nCntMeshField].block.y * g_aMeshField[nCntMeshField].size.y) * 0.5f) - (nCntMeshField1 * g_aMeshField[nCntMeshField].size.y);

					// rhwの設定
					pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

					// 頂点カラーの設定
					pVtx[0].col = WHITE_VTX;

					// テクスチャ座標の設定
					pVtx[0].tex = D3DXVECTOR2((float)nCntMeshField2, (float)nCntMeshField1);

					pVtx++;
				}
			}

			// 頂点バッファをアンロックする
			g_aMeshField[nCntMeshField].pVtxBuff->Unlock();

			// インデックスバッファの数
			int nNumIdx = (((int)g_aMeshField[nCntMeshField].block.x) * ((int)g_aMeshField[nCntMeshField].block.y) * 2) + (((int)g_aMeshField[nCntMeshField].block.y - 1) * 4) + 2;

			//int bx = (int)g_aMeshField[nCntMeshField].block.x;
			//int by = (int)g_aMeshField[nCntMeshField].block.y;
			//int rowWidth = bx + 1;
			//int quadCount = bx * by;
			//int nNumIdx = quadCount * 6;

			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * nNumIdx,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshField[nCntMeshField].pIdxBuff,
				NULL);

			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshField[nCntMeshField].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

#if 1
			int nNum = 0;			// 縮退ポリゴン

			// 頂点番号データの設定
			for (int nCntMeshField1 = 0; nCntMeshField1 < nNumIdx / 2; nCntMeshField1++)
			{
				if (nCntMeshField1 % ((int)g_aMeshField[nCntMeshField].block.x + 2) == ((int)g_aMeshField[nCntMeshField].block.x + 1))
				{// 縮退ポリゴンのところ
					nNum++;

					pIdx[0] = nCntMeshField1 - nNum;
					pIdx[1] = nCntMeshField1 - nNum + ((int)g_aMeshField[nCntMeshField].block.x + 2);
				}
				else
				{// 縮退以外のポリゴン
					pIdx[0] = (nCntMeshField1 - nNum) + ((int)g_aMeshField[nCntMeshField].block.x + 1);
					pIdx[1] = (nCntMeshField1 - nNum);
				}

				pIdx += 2;
			}
#endif
			//int idx = 0;
			//for (int y = 0; y < by; ++y)
			//{
			//	for (int x = 0; x < bx; ++x)
			//	{
			//		int v0 = y * rowWidth + x;
			//		int v1 = y * rowWidth + (x + 1);
			//		int v2 = (y + 1) * rowWidth + x;
			//		int v3 = (y + 1) * rowWidth + (x + 1);

			//		// 三角形1（CCW）
			//		pIdx[idx++] = v0;
			//		pIdx[idx++] = v1;
			//		pIdx[idx++] = v2;

			//		// 三角形2（CCW）
			//		pIdx[idx++] = v1;
			//		pIdx[idx++] = v3;
			//		pIdx[idx++] = v2;
			//	}
			//}

			// インデックスバッファをアンロックする
			g_aMeshField[nCntMeshField].pIdxBuff->Unlock();

			break;
		}
	}
}

//=============================================================================
// メッシュフィールドのタイプ取得
//=============================================================================
bool GetMeshFieldType(MESHFIELDTYPE type, D3DXVECTOR3 pos)
{
	for (int nCntMeshField = 0; nCntMeshField < MAX_MESHFIELD; nCntMeshField++)
	{
		if (g_aMeshField[nCntMeshField].bUse == true && g_aMeshField[nCntMeshField].type == type)
		{// 使用しているとき
			float fDistanceX = (g_aMeshField[nCntMeshField].size.x * g_aMeshField[nCntMeshField].block.x * 0.5f);
			float fDistanceZ = (g_aMeshField[nCntMeshField].size.y * g_aMeshField[nCntMeshField].block.y * 0.5f);

			if (pos.x >= g_aMeshField[nCntMeshField].pos.x - fDistanceX &&
				pos.x <= g_aMeshField[nCntMeshField].pos.x + fDistanceX &&
				pos.z >= g_aMeshField[nCntMeshField].pos.z - fDistanceZ &&
				pos.z <= g_aMeshField[nCntMeshField].pos.z + fDistanceZ)
			{// 範囲内にいる
				return true;
			}
		}
	}

	return false;
}