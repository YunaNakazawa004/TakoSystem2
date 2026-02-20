//=============================================================================
// 
// メッシュドーム [meshdome.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "meshdome.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MESHDOME			(128)									// メッシュドームの数
#define RADIUS_MESHDOME			(40.0f)									// メッシュドームの初期半径
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define FIRST_SIZE				(D3DXVECTOR2(0.0f, 0.0f))				// 初期サイズ
#define MESHDOME_TEX			"data\\TEXTURE\\Sky_Texture.jpg"		// メッシュドームのテクスチャ

//*****************************************************************************
// メッシュドームの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;		// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;						// メッシュドームの位置情報
	D3DXVECTOR3 rot;						// メッシュドームの向き情報
	D3DXVECTOR2 block;						// 分割数
	float fRadius;							// 半径
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	bool bUse;
}MeshDome;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureMeshDome = NULL;				// テクスチャへのポインタ
MeshDome g_aMeshDome[MAX_MESHDOME];			// メッシュドームの情報

//=============================================================================
// メッシュドームの初期化処理
//=============================================================================
void InitMeshDome(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, MESHDOME_TEX, &g_pTextureMeshDome);

	// メッシュドーム情報の初期化
	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		g_aMeshDome[nCntMeshDome].pVtxBuff = NULL;
		g_aMeshDome[nCntMeshDome].pIdxBuff = NULL;
		g_aMeshDome[nCntMeshDome].pos = FIRST_POS;
		g_aMeshDome[nCntMeshDome].rot = FIRST_POS;
		g_aMeshDome[nCntMeshDome].block = FIRST_SIZE;
		g_aMeshDome[nCntMeshDome].fRadius = RADIUS_MESHDOME;
		g_aMeshDome[nCntMeshDome].bUse = false;
	}
}

//=============================================================================
// メッシュドームの終了処理
//=============================================================================
void UninitMeshDome(void)
{
	// テクスチャの破棄
	if (g_pTextureMeshDome != NULL)
	{
		g_pTextureMeshDome->Release();
		g_pTextureMeshDome = NULL;
	}

	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		// 頂点バッファの破棄
		if (g_aMeshDome[nCntMeshDome].pVtxBuff != NULL)
		{
			g_aMeshDome[nCntMeshDome].pVtxBuff->Release();
			g_aMeshDome[nCntMeshDome].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_aMeshDome[nCntMeshDome].pIdxBuff != NULL)
		{
			g_aMeshDome[nCntMeshDome].pIdxBuff->Release();
			g_aMeshDome[nCntMeshDome].pIdxBuff = NULL;
		}
	}
}

//=============================================================================
// メッシュドームの更新処理
//=============================================================================
void UpdateMeshDome(void)
{
}

//=============================================================================
// メッシュドームの描画処理
//=============================================================================
void DrawMeshDome(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;		// 計算用マトリックス

	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		if (g_aMeshDome[nCntMeshDome].bUse == true)
		{// 使用しているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMeshDome[nCntMeshDome].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aMeshDome[nCntMeshDome].rot.y, g_aMeshDome[nCntMeshDome].rot.x, g_aMeshDome[nCntMeshDome].rot.z);
			D3DXMatrixMultiply(&g_aMeshDome[nCntMeshDome].mtxWorld, &g_aMeshDome[nCntMeshDome].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aMeshDome[nCntMeshDome].pos.x, g_aMeshDome[nCntMeshDome].pos.y, g_aMeshDome[nCntMeshDome].pos.z);
			D3DXMatrixMultiply(&g_aMeshDome[nCntMeshDome].mtxWorld, &g_aMeshDome[nCntMeshDome].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMeshDome[nCntMeshDome].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aMeshDome[nCntMeshDome].pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_aMeshDome[nCntMeshDome].pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureMeshDome);

			// ポリゴンの描画(FAN)
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, 0, 0, 
				((int)g_aMeshDome[nCntMeshDome].block.x + 1), 0,
				((int)g_aMeshDome[nCntMeshDome].block.x));

			// ポリゴンの描画(STRIP)
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, 
				((int)g_aMeshDome[nCntMeshDome].block.x + 1) * ((int)g_aMeshDome[nCntMeshDome].block.y + 1),
				((int)g_aMeshDome[nCntMeshDome].block.x + 2),
				(((int)g_aMeshDome[nCntMeshDome].block.x) * ((int)g_aMeshDome[nCntMeshDome].block.y) * 2) + (((int)g_aMeshDome[nCntMeshDome].block.y - 1) * 4));
		}
	}
}

//=============================================================================
// メッシュドームの設定処理
//=============================================================================
void SetMeshDome(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, float fRadius)
{
	for (int nCntMeshDome = 0; nCntMeshDome < MAX_MESHDOME; nCntMeshDome++)
	{
		if (g_aMeshDome[nCntMeshDome].bUse == false)
		{// 使用していない
			g_aMeshDome[nCntMeshDome].pos = pos;
			g_aMeshDome[nCntMeshDome].rot = rot;
			g_aMeshDome[nCntMeshDome].block = block;
			g_aMeshDome[nCntMeshDome].fRadius = fRadius;
			g_aMeshDome[nCntMeshDome].bUse = true;

			LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
			VERTEX_3D* pVtx;					// 頂点情報へのポインタ
			WORD* pIdx;							// インデックス情報へのポインタ
			int nNumVtxCylinder = ((int)g_aMeshDome[nCntMeshDome].block.x + 1) * ((int)g_aMeshDome[nCntMeshDome].block.y + 1);
			int nNumVtxFan = (int)g_aMeshDome[nCntMeshDome].block.x + 1;

			// 頂点バッファの生成
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * (nNumVtxCylinder + nNumVtxFan),
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aMeshDome[nCntMeshDome].pVtxBuff,
				NULL);

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aMeshDome[nCntMeshDome].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			float fAngleX = ((D3DX_PI * 2.0f) / g_aMeshDome[nCntMeshDome].block.x);
			float fAngleY = (D3DX_PI / 2.0f) / (g_aMeshDome[nCntMeshDome].block.y + 1);

			for (int nCntMeshDome1 = 0; nCntMeshDome1 < nNumVtxFan; nCntMeshDome1++)
			{
				if (nCntMeshDome1 == 0)
				{// 真ん中
					pVtx[0].pos.x = 0.0f;
					pVtx[0].pos.y = g_aMeshDome[nCntMeshDome].fRadius;
					pVtx[0].pos.z = 0.0f;
				}
				else
				{// 真ん中以外
					// 頂点座標の設定
					pVtx[0].pos.x = sinf(-nCntMeshDome1 * fAngleX) * g_aMeshDome[nCntMeshDome].fRadius * sinf(fAngleY);
					pVtx[0].pos.y = cosf(-fAngleY) * g_aMeshDome[nCntMeshDome].fRadius;
					pVtx[0].pos.z = cosf(-nCntMeshDome1 * fAngleX) * g_aMeshDome[nCntMeshDome].fRadius * sinf(fAngleY);
				}

				// rhwの設定
				pVtx[0].nor = D3DXVECTOR3(-pVtx[0].pos.x, -pVtx[0].pos.y, -pVtx[0].pos.z);
				D3DXVec3Normalize(&pVtx[0].nor, &pVtx[0].nor);

				// 頂点カラーの設定
				pVtx[0].col = WHITE_VTX;

				// テクスチャ座標の設定
				pVtx[0].tex = D3DXVECTOR2(0.0f, (float)nCntMeshDome1);

				pVtx++;
			}

			// 頂点情報の設定
			for (int nCntMeshDome1 = 0; nCntMeshDome1 < (int)g_aMeshDome[nCntMeshDome].block.y + 1; nCntMeshDome1++)
			{
				for (int nCntMeshDome2 = 0; nCntMeshDome2 < (int)g_aMeshDome[nCntMeshDome].block.x + 1; nCntMeshDome2++)
				{
					// 頂点座標の設定
					pVtx[0].pos.x = sinf(nCntMeshDome2 * fAngleX) * g_aMeshDome[nCntMeshDome].fRadius * sinf(fAngleY * (nCntMeshDome1 + 1));
					pVtx[0].pos.y = cosf(fAngleY * (nCntMeshDome1 + 1)) * g_aMeshDome[nCntMeshDome].fRadius;
					pVtx[0].pos.z = cosf(nCntMeshDome2 * fAngleX) * g_aMeshDome[nCntMeshDome].fRadius * sinf(fAngleY * (nCntMeshDome1 + 1));

					// rhwの設定
					pVtx[0].nor = D3DXVECTOR3(-pVtx[0].pos.x, -pVtx[0].pos.y, -pVtx[0].pos.z);
					D3DXVec3Normalize(&pVtx[0].nor, &pVtx[0].nor);

					// 頂点カラーの設定
					pVtx[0].col = WHITE_VTX;

					// テクスチャ座標の設定
					pVtx[0].tex.x = (float)(1.0f / g_aMeshDome[nCntMeshDome].block.x * nCntMeshDome2);
					pVtx[0].tex.y = (float)(1.0f / g_aMeshDome[nCntMeshDome].block.y * nCntMeshDome1);

					pVtx++;
				}
			}

			// 頂点バッファをアンロックする
			g_aMeshDome[nCntMeshDome].pVtxBuff->Unlock();

			// インデックスバッファの数
			int nNumIdxCylinder = (((int)g_aMeshDome[nCntMeshDome].block.x) * ((int)g_aMeshDome[nCntMeshDome].block.y) * 2) + (((int)g_aMeshDome[nCntMeshDome].block.y - 1) * 4) + 2;
			int nNumIdxFan = (int)g_aMeshDome[nCntMeshDome].block.x + 2;

			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * (nNumIdxCylinder + nNumIdxFan),
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aMeshDome[nCntMeshDome].pIdxBuff,
				NULL);

			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshDome[nCntMeshDome].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

			// 頂点番号データの設定
			for (int nCntMeshDome1 = 0; nCntMeshDome1 < nNumIdxFan; nCntMeshDome1++)
			{
				pIdx[0] = nCntMeshDome1;

				if (nCntMeshDome1 == nNumIdxFan - 1)
				{// 最後は戻ってくる
					pIdx[0] = 1;
				}

				pIdx++;
			}

			int nNum = 0;			// 縮退ポリゴン

			// 頂点番号データの設定
			for (int nCntMeshDome1 = 0; nCntMeshDome1 < nNumIdxCylinder / 2; nCntMeshDome1++)
			{
				if (nCntMeshDome1 % ((int)g_aMeshDome[nCntMeshDome].block.x + 2) == ((int)g_aMeshDome[nCntMeshDome].block.x + 1))
				{// 縮退ポリゴンのところ
					nNum++;

					pIdx[0] = ((int)g_aMeshDome[nCntMeshDome].block.x + 1) + nCntMeshDome1 - nNum;
					pIdx[1] = ((int)g_aMeshDome[nCntMeshDome].block.x + 1) + nCntMeshDome1 - nNum + ((int)g_aMeshDome[nCntMeshDome].block.x + 2);
				}
				else
				{// 縮退以外のポリゴン
					pIdx[0] = ((int)g_aMeshDome[nCntMeshDome].block.x + 1) + (nCntMeshDome1 - nNum) + ((int)g_aMeshDome[nCntMeshDome].block.x + 1);
					pIdx[1] = ((int)g_aMeshDome[nCntMeshDome].block.x + 1) + (nCntMeshDome1 - nNum);
				}

				pIdx += 2;
			}

			// インデックスバッファをアンロックする
			g_aMeshDome[nCntMeshDome].pIdxBuff->Unlock();

			break;
		}
	}
}