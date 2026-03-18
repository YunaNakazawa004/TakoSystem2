//=============================================================================
// 
// メッシュリング [meshring.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "meshring.h"
#include "input.h"
#include "debugproc.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TEX_MESHRING		(16)									// メッシュリングのテクスチャ数
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

	MESHRINGTYPE type;						// 種類

	D3DXVECTOR3 pos;						// メッシュリングの位置情報
	D3DXVECTOR3 rot;						// メッシュリングの向き情報
	D3DXVECTOR2 block;						// 分割数
	D3DXVECTOR2 size;						// サイズ
	D3DXCOLOR col;							// 色
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	bool bUse;
}MeshRing;

// メッシュリングの種類別の情報の構造体
typedef struct
{
	int nIdxTexture;		// テクスチャインデックス
	bool bAlphaBrend;		// アルファブレンドをするか
	bool bCulling;			// 両面カリングをするか

}MeshRingTypeInfo;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureMeshRing[MAX_TEX_MESHRING] = {};				// テクスチャへのポインタ
MeshRing g_aMeshRing[MAX_MESHRING];			// メッシュリングの情報

// テクスチャファイル名
const char* c_apFilenameMeshRing[] =
{
	"data\\TEXTURE\\tex_alpha_hit001.jpg",		// [0]衝撃波
};

// 種類別の情報
MeshRingTypeInfo g_aMeshRingTypeInfo[MESHRINGTYPE_MAX] =
{// テクスチャ, アルファブレンドをするか, 両面カリングをするか

	{-1, false, false},		// [0]波紋
	{0, true, true},		// [1]衝撃波
};

//=============================================================================
// メッシュリングの初期化処理
//=============================================================================
void InitMeshRing(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	HRESULT hr;
	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameMeshRing / sizeof(c_apFilenameMeshRing[0]); nCntTexture++)
	{
		 hr = D3DXCreateTextureFromFile(pDevice, c_apFilenameMeshRing[nCntTexture], &g_apTextureMeshRing[nCntTexture]);
	}

	// メッシュリング情報の初期化
	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		g_aMeshRing[nCntMeshRing].pVtxBuff = NULL;
		g_aMeshRing[nCntMeshRing].pIdxBuff = NULL;
		g_aMeshRing[nCntMeshRing].type = MESHRINGTYPE_NONE;
		g_aMeshRing[nCntMeshRing].pos = FIRST_POS;
		g_aMeshRing[nCntMeshRing].rot = FIRST_POS;
		g_aMeshRing[nCntMeshRing].block = FIRST_SIZE;
		g_aMeshRing[nCntMeshRing].size = FIRST_SIZE;
		g_aMeshRing[nCntMeshRing].col = WHITE_VTX;
		g_aMeshRing[nCntMeshRing].bUse = false;
	}

	//SetMeshRing(MESHRINGTYPE_NONE, FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(100.0f, 50.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//SetMeshRing(MESHRINGTYPE_SHOCKWAVE, D3DXVECTOR3(0.0f, 500.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 3.14f), D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(10.0f, 5.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

//=============================================================================
// メッシュリングの終了処理
//=============================================================================
void UninitMeshRing(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_MESHRING; nCntTexture++)
	{
		if (g_apTextureMeshRing[nCntTexture] != NULL)
		{
			g_apTextureMeshRing[nCntTexture]->Release();
			g_apTextureMeshRing[nCntTexture] = NULL;
		}
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
	//VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	int nCnt = 0;

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse == true)
		{// 使用しているとき
			
			// 種類別の更新
			switch (g_aMeshRing[nCntMeshRing].type)
			{
			case MESHRINGTYPE_RIPPLES:

				// 段々輪を大きく、細く
				g_aMeshRing[nCntMeshRing].size.x += 0.8f;
				g_aMeshRing[nCntMeshRing].size.y -= 0.4f;

				if (g_aMeshRing[nCntMeshRing].size.y <= 0.0f)
				{// 輪が見えなくなる
					g_aMeshRing[nCntMeshRing].bUse = false;
				}

				break;

			case MESHRINGTYPE_SHOCKWAVE:

				g_aMeshRing[nCntMeshRing].size.x += 1.0f;
				g_aMeshRing[nCntMeshRing].size.y += 0.3f;

				if (g_aMeshRing[nCntMeshRing].size.x >= 70.0f
				 && g_aMeshRing[nCntMeshRing].size.y >= 2.0f)
				{
					g_aMeshRing[nCntMeshRing].col.a -= 0.1f;
				}

				if (g_aMeshRing[nCntMeshRing].col.a <= 0.0f)
				{
					g_aMeshRing[nCntMeshRing].bUse = false;
				}

				break;
			}

			// 更新後の判定
			if (g_aMeshRing[nCntMeshRing].bUse == false)
			{// 使用してない状態になった

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

				continue;	// 処理を抜ける
			}

			// 頂点の設定
			SetVtxMeshRing(nCntMeshRing);

			nCnt++;
		}
	}

	PrintDebugProc("NUM_MESHRING %d\n", nCnt);
}

//=============================================================================
// メッシュリングの描画処理
//=============================================================================
void DrawMeshRing(void)
{
	//return;

	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse == true)
		{// 使用しているとき

			if (g_aMeshRing[nCntMeshRing].type == -1)
			{
				continue;
			}

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
			if (g_aMeshRingTypeInfo[g_aMeshRing[nCntMeshRing].type].nIdxTexture != -1)
			{
				pDevice->SetTexture(0, g_apTextureMeshRing[g_aMeshRingTypeInfo[g_aMeshRing[nCntMeshRing].type].nIdxTexture]);
			}
			else
			{
				pDevice->SetTexture(0, NULL);
			}

			if (g_aMeshRingTypeInfo[g_aMeshRing[nCntMeshRing].type].bCulling == true)
			{// 両面カリングをする場合

				// カリングをOFFに設定
				pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			}

			if (g_aMeshRingTypeInfo[g_aMeshRing[nCntMeshRing].type].bAlphaBrend == true)
			{// アルファブレンドをする場合

				// 減算合成の設定
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// アルファブレンドの設定3
			}

			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, ((int)g_aMeshRing[nCntMeshRing].block.x + 1) * ((int)g_aMeshRing[nCntMeshRing].block.y + 1), 0,
				(((int)g_aMeshRing[nCntMeshRing].block.x) * ((int)g_aMeshRing[nCntMeshRing].block.y) * 2) + (((int)g_aMeshRing[nCntMeshRing].block.y - 1) * 4));

			if (g_aMeshRingTypeInfo[g_aMeshRing[nCntMeshRing].type].bCulling == true)
			{// 両面カリングをする場合

				// カリングを戻す
				pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			}

			if (g_aMeshRingTypeInfo[g_aMeshRing[nCntMeshRing].type].bAlphaBrend == true)
			{
				// ブレンディング(減算合成)を元に戻す 
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
			}
		}
	}
}

//=============================================================================
// メッシュリングの設定処理
//=============================================================================
void SetMeshRing(MESHRINGTYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size, D3DXCOLOR col)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	//VERTEX_3D* pVtx;					// 頂点情報へのポインタ
	WORD* pIdx;							// インデックス情報へのポインタ

	int nNumIdx;

	for (int nCntMeshRing = 0; nCntMeshRing < MAX_MESHRING; nCntMeshRing++)
	{
		if (g_aMeshRing[nCntMeshRing].bUse == false)
		{// 使用していない

			g_aMeshRing[nCntMeshRing].type = type;
			g_aMeshRing[nCntMeshRing].pos.x = pos.x;
			g_aMeshRing[nCntMeshRing].pos.y = pos.y;
			g_aMeshRing[nCntMeshRing].pos.z = pos.z;
			g_aMeshRing[nCntMeshRing].rot = rot;
			g_aMeshRing[nCntMeshRing].block = block;
			g_aMeshRing[nCntMeshRing].size = size;
			g_aMeshRing[nCntMeshRing].col = col;
			g_aMeshRing[nCntMeshRing].bUse = true;

			nNumIdx = (((int)g_aMeshRing[nCntMeshRing].block.x + 1) * ((int)g_aMeshRing[nCntMeshRing].block.y + 1));

				// 頂点バッファの生成
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nNumIdx,
										D3DUSAGE_WRITEONLY,
										FVF_VERTEX_3D,
										D3DPOOL_MANAGED,
										&g_aMeshRing[nCntMeshRing].pVtxBuff,
										NULL);

			// 頂点の設定
			SetVtxMeshRing(nCntMeshRing);

			// インデックスバッファの数
			nNumIdx = (((int)g_aMeshRing[nCntMeshRing].block.x) * ((int)g_aMeshRing[nCntMeshRing].block.y) * 2) + (((int)g_aMeshRing[nCntMeshRing].block.y - 1) * 4) + 2;

			// インデックスバッファの生成
			pDevice->CreateIndexBuffer(sizeof(WORD) * nNumIdx,
									   D3DUSAGE_WRITEONLY,
									   D3DFMT_INDEX16,
									   D3DPOOL_MANAGED,
									   &g_aMeshRing[nCntMeshRing].pIdxBuff,
									   NULL);

			// インデックスバッファをロックし、頂点番号データへのポインタを取得
			g_aMeshRing[nCntMeshRing].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

			//int nNum = 0;			// 縮退ポリゴン

			// 頂点番号データの設定
			for (int nCntMeshRing1 = 0; nCntMeshRing1 < nNumIdx; nCntMeshRing1++)
			{
				pIdx[0] = (WORD)nCntMeshRing1;
				pIdx++;
			}

			// インデックスバッファをアンロックする
			g_aMeshRing[nCntMeshRing].pIdxBuff->Unlock();

			break;
		}
	}
}

//=============================================================================
// メッシュリングの頂点設定処理
//=============================================================================
void SetVtxMeshRing(int nIdx)
{
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_aMeshRing[nIdx].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点情報の設定
	for (int nCntMeshRing1 = 0; nCntMeshRing1 < (int)g_aMeshRing[nIdx].block.y + 1; nCntMeshRing1++)
	{
		for (int nCntMeshRing2 = 0; nCntMeshRing2 < (int)g_aMeshRing[nIdx].block.x + 1; nCntMeshRing2++)
		{
			float fAngle = ((D3DX_PI * 2.0f) / g_aMeshRing[nIdx].block.x);
			float fSize = (g_aMeshRing[nIdx].size.x + (g_aMeshRing[nIdx].size.y * -((nCntMeshRing2 + nCntMeshRing1) % 2)));

			// 頂点座標の設定
			pVtx[0].pos.x = sinf(((nCntMeshRing2 + (nCntMeshRing1 * ((int)g_aMeshRing[nIdx].block.x + 1))) / 2) * -fAngle) * fSize;
			pVtx[0].pos.y = 0.0f;
			pVtx[0].pos.z = cosf(((nCntMeshRing2 + (nCntMeshRing1 * ((int)g_aMeshRing[nIdx].block.x + 1))) / 2) *  fAngle) * fSize;

			// 法線ベクトルの設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

			// 頂点カラーの設定
			pVtx[0].col = g_aMeshRing[nIdx].col;

			// テクスチャ座標の設定
			int nTmp = nCntMeshRing2 + ((nCntMeshRing1 != 0) ? nCntMeshRing1 * g_aMeshRing[nIdx].block.x + 1 : 0);

			pVtx[0].tex.x = (float)(nTmp != 0) ? nTmp / 2 : 0.0f;
			pVtx[0].tex.y = (float)((nCntMeshRing2 + nCntMeshRing1) % 2);

			pVtx++;
		}
	}

	// 頂点バッファをアンロックする
	g_aMeshRing[nIdx].pVtxBuff->Unlock();
}



//=============================================================================
// メッシュリングの衝撃波の角度を求める処理
//=============================================================================
D3DXVECTOR3 CalcShockWaveRot(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2)
{
	// 変数宣言 ======================================

	float fWidth  = (pos1.x - pos2.x),	// X方向の離れ具合を求める
		  fDipth  = (pos1.z - pos2.z),	// Z方向の離れ具合を求める
		  fHeight = (pos2.z - pos1.z);	// Y方向の離れ具合を求める

	float fDiaLengthXZ;					// (X,Z)の対角線の長さ

	float fAngleX, fAngleY;				// 設定するの角度

	// ===============================================

	// 対角線の長さを求める
	fDiaLengthXZ = sqrtf(fWidth * fWidth + fDipth * fDipth);

	// 設定する角度を求める
	fAngleY = atan2f(fWidth, fDipth);			// Yの向きを求める
	fAngleX = atan2f(fDiaLengthXZ, fHeight);	// Xの向きを求める

	// 設定する角度を返す
	return D3DXVECTOR3(fAngleX, fAngleY, 0.0f);
}