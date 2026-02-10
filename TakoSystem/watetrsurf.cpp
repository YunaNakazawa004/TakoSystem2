//=============================================================================
// 
// 水面のcpp
// Author : Mutsuki Uemura
// 
//=============================================================================
#include "main.h"
#include "watersurf.h"
#include "input.h"
#include "meshcylinder.h"

//=======================================
// 構造体の定義
//=======================================

// 水面の構造体 -------------------------

typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;		// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;						// 位置
	D3DXVECTOR3 rot;						// 角度
	D3DXCOLOR col;							// 色
	D3DXVECTOR2 block;						// 分割数
	D3DXMATRIX mtxWorld;
	D3DXVECTOR2 size;						// サイズ
	bool bUse;								// 使用しているか
	bool bUp;								// 上昇か
}WaterSurf;

//=======================================
// グローバル宣言
//=======================================

LPDIRECT3DTEXTURE9 g_pTextureWaterSurf = NULL;			// テクスチャへのポインタ
WaterSurf g_aWatersurf[MAX_WATERSURF];

//=============================================================================
// 水面の初期化処理
//=============================================================================
void InitWaterSurf(void)
{
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ
	float pRadius = 1000.0f;

	// デバイスの取得
	pDevice = GetDevice();

	// randのランダム化
	srand((unsigned int)time(NULL));

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(
		pDevice,
		"data/TEXTURE/sea3.jpg",
		&g_pTextureWaterSurf);


	SetWaterSurf({ 0.0f,CYLINDER_HEIGHT,0.0f }, { 0.0f,0.0f,0.0f }, { 4,4 }, { pRadius * 2, pRadius * 2 }, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.4f));
}

//=========================================
// 水面の終了処理
//=========================================
void UninitWaterSurf(void)
{
	// テクスチャの破棄
	if (g_pTextureWaterSurf != NULL)
	{
		g_pTextureWaterSurf->Release();
		g_pTextureWaterSurf = NULL;
	}
	// 頂点バッファの破棄
	for (int nCntSurf = 0; nCntSurf < MAX_WATERSURF; nCntSurf++)
	{
		if (g_aWatersurf[nCntSurf].pVtxBuff != NULL)
		{
			g_aWatersurf[nCntSurf].pVtxBuff->Release();
			g_aWatersurf[nCntSurf].pVtxBuff = NULL;
		}
		// インデックスバッファの破棄
		if (g_aWatersurf[nCntSurf].pIdxBuff != NULL)
		{
			g_aWatersurf[nCntSurf].pIdxBuff->Release();
			g_aWatersurf[nCntSurf].pIdxBuff = NULL;
		}
	}
}

//=========================================
// 水面の更新処理
//=========================================
void UpdateWaterSurf(void)
{
	VERTEX_3D* pVtx;							// 頂点情報へのポインタ
	static float speedX = 0.0002f;			// 速度
	static float speedY = 0.0002f;			// 速度
	static D3DXVECTOR2 move = { 0.0f,0.0f };	// 移動量


	move = {speedX, speedY};
	
	for (int nCntWaterSurf = 0; nCntWaterSurf < MAX_WATERSURF; nCntWaterSurf++)
	{
		if (g_aWatersurf[nCntWaterSurf].bUse == true)
		{
			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aWatersurf[nCntWaterSurf].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			if (g_aWatersurf[nCntWaterSurf].pos.y < CYLINDER_HEIGHT / 2)
			{
				g_aWatersurf[nCntWaterSurf].bUp = true;
			}
			else if (g_aWatersurf[nCntWaterSurf].pos.y > CYLINDER_HEIGHT)
			{
				g_aWatersurf[nCntWaterSurf].bUp = false;
			}

			if (g_aWatersurf[nCntWaterSurf].bUp == true)
			{ // 水面上昇
				g_aWatersurf[nCntWaterSurf].pos.y += CYLINDER_HEIGHT / 10000;
			}
			else
			{ // 水面下降
				g_aWatersurf[nCntWaterSurf].pos.y -= CYLINDER_HEIGHT / 10000;
			}

			for (int nCntWaterSurf1 = 0; nCntWaterSurf1 <((int)g_aWatersurf[nCntWaterSurf].block.x + 1) * ((int)g_aWatersurf[nCntWaterSurf].block.y + 1); nCntWaterSurf1++)
			{
				pVtx[0].tex.x += move.x;
				pVtx[0].tex.y += move.y;

				pVtx++;
			}

			// 頂点バッファをアンロックする
			g_aWatersurf[nCntWaterSurf].pVtxBuff->Unlock();
		}

	}
}

//=========================================
// 水面の描画処理
//=========================================
void DrawWaterSurf(void)
{
	int nCntWaterSurf = 0;
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスの取得

	D3DXMATRIX mtxRot, mtxTrans;					// 計算用マトリックス

	for (nCntWaterSurf = 0; nCntWaterSurf < MAX_WATERSURF; nCntWaterSurf++)
	{
		if (g_aWatersurf[nCntWaterSurf].bUse == true)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aWatersurf[nCntWaterSurf].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aWatersurf[nCntWaterSurf].rot.y, g_aWatersurf[nCntWaterSurf].rot.x, g_aWatersurf[nCntWaterSurf].rot.z);
			D3DXMatrixMultiply(&g_aWatersurf[nCntWaterSurf].mtxWorld, &g_aWatersurf[nCntWaterSurf].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aWatersurf[nCntWaterSurf].pos.x, g_aWatersurf[nCntWaterSurf].pos.y, g_aWatersurf[nCntWaterSurf].pos.z);
			D3DXMatrixMultiply(&g_aWatersurf[nCntWaterSurf].mtxWorld, &g_aWatersurf[nCntWaterSurf].mtxWorld, &mtxTrans);

			// 裏面も描画したいポリゴン
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aWatersurf[nCntWaterSurf].mtxWorld);

			// 選択バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aWatersurf[nCntWaterSurf].pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_aWatersurf[nCntWaterSurf].pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureWaterSurf);
			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, ((int)g_aWatersurf[nCntWaterSurf].block.x + 1) * ((int)g_aWatersurf[nCntWaterSurf].block.y + 1), 0,
				(((int)g_aWatersurf[nCntWaterSurf].block.x) * ((int)g_aWatersurf[nCntWaterSurf].block.y) * 2) + (((int)g_aWatersurf[nCntWaterSurf].block.y - 1) * 4));
		}
		// 元に戻す（通常は裏面カリング）
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

//=========================================
// 水面の設定処理
//=========================================
void SetWaterSurf(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size,D3DXCOLOR col)
{
	for (int nCntWaterSurf = 0; nCntWaterSurf < MAX_WATERSURF; nCntWaterSurf++)
	{
		if (g_aWatersurf[nCntWaterSurf].bUse == false)
		{// 使用していない
			g_aWatersurf[nCntWaterSurf].pos = pos;
			g_aWatersurf[nCntWaterSurf].rot = rot;
			g_aWatersurf[nCntWaterSurf].block = block;
			g_aWatersurf[nCntWaterSurf].size = size;
			g_aWatersurf[nCntWaterSurf].bUse = true;

			LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
			VERTEX_3D* pVtx;					// 頂点情報へのポインタ
			WORD* pIdx;							// インデックス情報へのポインタ

			// 頂点バッファの生成
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * ((int)g_aWatersurf[nCntWaterSurf].block.x + 1) * ((int)g_aWatersurf[nCntWaterSurf].block.y + 1),
				D3DUSAGE_WRITEONLY,
				FVF_VERTEX_3D,
				D3DPOOL_MANAGED,
				&g_aWatersurf[nCntWaterSurf].pVtxBuff,
				NULL);

			// 頂点バッファをロックし、頂点情報へのポインタを取得
			g_aWatersurf[nCntWaterSurf].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			// 頂点情報の設定
			for (int nCntWaterSurf1 = 0; nCntWaterSurf1 < (int)g_aWatersurf[nCntWaterSurf].block.y + 1; nCntWaterSurf1++)
			{
				for (int nCntWaterSurf2 = 0; nCntWaterSurf2 < (int)g_aWatersurf[nCntWaterSurf].block.x + 1; nCntWaterSurf2++)
				{
					// 頂点座標の設定
					pVtx[0].pos.x = -((g_aWatersurf[nCntWaterSurf].block.x * g_aWatersurf[nCntWaterSurf].size.x) * 0.5f) + (nCntWaterSurf2 * g_aWatersurf[nCntWaterSurf].size.x);
					pVtx[0].pos.y = 0.0f;
					pVtx[0].pos.z = ((g_aWatersurf[nCntWaterSurf].block.y * g_aWatersurf[nCntWaterSurf].size.y) * 0.5f) - (nCntWaterSurf1 * g_aWatersurf[nCntWaterSurf].size.y);

					// rhwの設定
					pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

					// 頂点カラーの設定
					pVtx[0].col = col;

					// テクスチャ座標の設定
					pVtx[0].tex = D3DXVECTOR2((float)nCntWaterSurf2, (float)nCntWaterSurf1);

					pVtx++;
				}
			}

			// 頂点バッファをアンロックする
			g_aWatersurf[nCntWaterSurf].pVtxBuff->Unlock();

			// インデックスバッファの数
			int nNumIdx = (((int)g_aWatersurf[nCntWaterSurf].block.x) * ((int)g_aWatersurf[nCntWaterSurf].block.y) * 2) + (((int)g_aWatersurf[nCntWaterSurf].block.y - 1) * 4) + 2;

			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * nNumIdx,
				D3DUSAGE_WRITEONLY,
				D3DFMT_INDEX16,
				D3DPOOL_MANAGED,
				&g_aWatersurf[nCntWaterSurf].pIdxBuff,
				NULL);

			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aWatersurf[nCntWaterSurf].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

			int nNum = 0;			// 縮退ポリゴン

			// 頂点番号データの設定
			for (int nCntWaterSurf1 = 0; nCntWaterSurf1 < nNumIdx / 2; nCntWaterSurf1++)
			{
				if (nCntWaterSurf1 % ((int)g_aWatersurf[nCntWaterSurf].block.x + 2) == ((int)g_aWatersurf[nCntWaterSurf].block.x + 1))
				{// 縮退ポリゴンのところ
					nNum++;

					pIdx[0] = nCntWaterSurf1 - nNum;
					pIdx[1] = nCntWaterSurf1 - nNum + ((int)g_aWatersurf[nCntWaterSurf].block.x + 2);
				}
				else
				{// 縮退以外のポリゴン
					pIdx[0] = (nCntWaterSurf1 - nNum) + ((int)g_aWatersurf[nCntWaterSurf].block.x + 1);
					pIdx[1] = (nCntWaterSurf1 - nNum);
				}

				pIdx += 2;
			}

			// インデックスバッファをアンロックする
			g_aWatersurf[nCntWaterSurf].pIdxBuff->Unlock();

			break;
		}
	}
}
//=========================================
// 水面の取得
//=========================================
float* GetWaterSurf_Height(void)
{
	return &g_aWatersurf->pos.y;
}
