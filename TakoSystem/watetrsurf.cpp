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
	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度
	D3DXCOLOR col;				// 色
	D3DXVECTOR2 block;			// 分割数
	D3DXMATRIX mtxWorld;
	D3DXVECTOR3 size;			// サイズ
	bool bUse;					// 使用しているか
	bool bUp;					// 上昇か
}WaterSurf;

//=======================================
// グローバル宣言
//=======================================

LPDIRECT3DTEXTURE9 g_pTextureWaterSurf = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffWaterSurf = NULL;		// 頂点バッファへのポインタ
WaterSurf g_awatersurf[MAX_WATERSURF];

//=============================================================================
// 水面の初期化処理
//=============================================================================
void InitWaterSurf(void)
{
	VERTEX_3D* pVtx;				// 頂点情報へのポインタ
	LPDIRECT3DDEVICE9 pDevice;		// デバイスへのポインタ
	float pRadius = 10000.0f;

	// デバイスの取得
	pDevice = GetDevice();

	// randのランダム化
	srand((unsigned int)time(NULL));

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(
		pDevice,
		"data/TEXTURE/sea3.jpg",
		&g_pTextureWaterSurf);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(
		sizeof(VERTEX_3D) * 4 * MAX_WATERSURF,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffWaterSurf,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWaterSurf->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntWaterSurf = 0; nCntWaterSurf < MAX_WATERSURF; nCntWaterSurf++)
	{
		g_awatersurf[nCntWaterSurf].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_awatersurf[nCntWaterSurf].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_awatersurf[nCntWaterSurf].col = D3DXCOLOR(1.0f, 1.0f, 1.0f,0.4f);
		g_awatersurf[nCntWaterSurf].size = D3DXVECTOR3(pRadius * 2, 0.0f, pRadius * 2);
		g_awatersurf[nCntWaterSurf].bUse = false;

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(g_awatersurf[nCntWaterSurf].pos.x + (g_awatersurf[nCntWaterSurf].size.x / 2.0f), g_awatersurf[nCntWaterSurf].pos.y, g_awatersurf[nCntWaterSurf].pos.z - (g_awatersurf[nCntWaterSurf].size.z / 2.0f));
		pVtx[1].pos = D3DXVECTOR3(g_awatersurf[nCntWaterSurf].pos.x - (g_awatersurf[nCntWaterSurf].size.x / 2.0f), g_awatersurf[nCntWaterSurf].pos.y, g_awatersurf[nCntWaterSurf].pos.z - (g_awatersurf[nCntWaterSurf].size.z / 2.0f));
		pVtx[2].pos = D3DXVECTOR3(g_awatersurf[nCntWaterSurf].pos.x + (g_awatersurf[nCntWaterSurf].size.x / 2.0f), g_awatersurf[nCntWaterSurf].pos.y, g_awatersurf[nCntWaterSurf].pos.z + (g_awatersurf[nCntWaterSurf].size.z / 2.0f));
		pVtx[3].pos = D3DXVECTOR3(g_awatersurf[nCntWaterSurf].pos.x - (g_awatersurf[nCntWaterSurf].size.x / 2.0f), g_awatersurf[nCntWaterSurf].pos.y, g_awatersurf[nCntWaterSurf].pos.z + (g_awatersurf[nCntWaterSurf].size.z / 2.0f));

		// 法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		// 頂点カラーの設定
		pVtx[0].col = g_awatersurf[nCntWaterSurf].col;
		pVtx[1].col = g_awatersurf[nCntWaterSurf].col;
		pVtx[2].col = g_awatersurf[nCntWaterSurf].col;
		pVtx[3].col = g_awatersurf[nCntWaterSurf].col;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}
	// 頂点バッファをアンロックします
	g_pVtxBuffWaterSurf->Unlock();

	SetWaterSurf({ 0.0f,CYLINDER_HEIGHT,0.0f }, {0.0f,0.0f,0.0f});
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
	if (g_pVtxBuffWaterSurf != NULL)
	{
		g_pVtxBuffWaterSurf->Release();
		g_pVtxBuffWaterSurf = NULL;
	}
}
//=========================================
// 水面の更新処理
//=========================================
void UpdateWaterSurf(void)
{
	VERTEX_3D* pVtx;							// 頂点情報へのポインタ
	static float speedX = 0.0002f;				// 速度
	static float speedY = 0.0002f;;				// 速度
	static D3DXVECTOR2 move = { 0.0f,0.0f };	// 移動量

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWaterSurf->Lock(0, 0, (void**)&pVtx, 0);

	move += {speedX, speedY};
	
	for (int nCntWaterSurf = 0; nCntWaterSurf < MAX_WATERSURF; nCntWaterSurf++)
	{
		if (g_awatersurf[nCntWaterSurf].bUse == true)
		{
			if (g_awatersurf[nCntWaterSurf].pos.y < CYLINDER_HEIGHT / 2)
			{
				g_awatersurf[nCntWaterSurf].bUp = true;
			}
			else if(g_awatersurf[nCntWaterSurf].pos.y > CYLINDER_HEIGHT)
			{
				g_awatersurf[nCntWaterSurf].bUp = false;
			}

			if (g_awatersurf[nCntWaterSurf].bUp == true)
			{ // 水面上昇
				g_awatersurf[nCntWaterSurf].pos.y += CYLINDER_HEIGHT / 10000;
			}
			else
			{ // 水面下降
				g_awatersurf[nCntWaterSurf].pos.y -= CYLINDER_HEIGHT / 10000;
			}

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f + move.x, 0.0f + move.y);
			pVtx[1].tex = D3DXVECTOR2(1.0f + move.x, 0.0f + move.y);
			pVtx[2].tex = D3DXVECTOR2(0.0f + move.x, 1.0f + move.y);
			pVtx[3].tex = D3DXVECTOR2(1.0f + move.x, 1.0f + move.y);
		}
	}
	// 頂点バッファをアンロックします
	g_pVtxBuffWaterSurf->Unlock();
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
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_awatersurf[nCntWaterSurf].mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot,
			g_awatersurf[nCntWaterSurf].rot.y * D3DX_PI, g_awatersurf[nCntWaterSurf].rot.x * D3DX_PI, g_awatersurf[nCntWaterSurf].rot.z * D3DX_PI);

		D3DXMatrixMultiply(&g_awatersurf[nCntWaterSurf].mtxWorld, &g_awatersurf[nCntWaterSurf].mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans,
			g_awatersurf[nCntWaterSurf].pos.x, g_awatersurf[nCntWaterSurf].pos.y, g_awatersurf[nCntWaterSurf].pos.z);

		D3DXMatrixMultiply(&g_awatersurf[nCntWaterSurf].mtxWorld, &g_awatersurf[nCntWaterSurf].mtxWorld, &mtxTrans);

		// 裏面も描画したいポリゴン
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_awatersurf[nCntWaterSurf].mtxWorld);

		// 選択バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffWaterSurf, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureWaterSurf);

		if (g_awatersurf[nCntWaterSurf].bUse == true)
		{
			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 * nCntWaterSurf, 2);
		}
		// 元に戻す（通常は裏面カリング）
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	}
}

//=========================================
// 水面の設定処理
//=========================================
void SetWaterSurf(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	VERTEX_3D* pVtx;				// 頂点情報へのポインタ
	int nCntWaterSurf = 0;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffWaterSurf->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntWaterSurf = 0; nCntWaterSurf < MAX_WATERSURF; nCntWaterSurf++)
	{
		if (g_awatersurf[nCntWaterSurf].bUse == false)
		{
			g_awatersurf[nCntWaterSurf].pos = pos;
			g_awatersurf[nCntWaterSurf].rot = rot;
			g_awatersurf[nCntWaterSurf].bUse = true;
			break;
		}
	}

	// 頂点バッファをアンロックします
	g_pVtxBuffWaterSurf->Unlock();
}
//=========================================
// 水面の取得
//=========================================
float* GetWaterSurf_Height(void)
{
	return &g_awatersurf->pos.y;
}
