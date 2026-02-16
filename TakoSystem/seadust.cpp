//=============================================================================
// 
// 塵 [seadust.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "main.h"
#include "seadust.h"
#include "oceancurrents.h"
#include "meshorbit.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SEADUST				(5000)							// 塵の最大数
#define SEADUST_SIZE			(1.0f)							// サイズ
#define SEADUST_FILENAME		"data\\TEXTURE\\shadow000.png"	// 使用する塵のファイル名

//*****************************************************************************
// 塵の構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	D3DXMATRIX mtxWorld;	// ワールドマトリックス
	bool bUse;				// 使用しているか
}SeaDust;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureSeaDust = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSeaDust = NULL;		// 頂点バッファ
SeaDust g_aSeaDust[MAX_SEADUST];						// 塵の情報

//======================================================================== 
// 塵の初期化処理
//========================================================================
void InitSeaDust(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice, SEADUST_FILENAME, &g_pTextureSeaDust);

	// 塵情報の初期化
	for (int nCntDust = 0; nCntDust < MAX_SEADUST; nCntDust++)
	{
		g_aSeaDust[nCntDust].pos = FIRST_POS;
		g_aSeaDust[nCntDust].bUse = false;
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * MAX_SEADUST,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffSeaDust,
		NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffSeaDust->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntDust = 0; nCntDust < MAX_SEADUST; nCntDust++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-SEADUST_SIZE, SEADUST_SIZE, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(SEADUST_SIZE, SEADUST_SIZE, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-SEADUST_SIZE, -SEADUST_SIZE, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(SEADUST_SIZE, -SEADUST_SIZE, 0.0f);

		// 法線ベクトル(向き)の設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 頂点カラーの設定
		pVtx[0].col = WHITE_VTX;
		pVtx[1].col = WHITE_VTX;
		pVtx[2].col = WHITE_VTX;
		pVtx[3].col = WHITE_VTX;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += MAX_VERTEX;
	}

	// ▲頂点バッファをアンロックする
	g_pVtxBuffSeaDust->Unlock();

	SetRandomSeaDust(5000);
}

//======================================================================== 
// 塵の終了処理
//========================================================================
void UninitSeaDust(void)
{
	// テクスチャの破棄
	if (g_pTextureSeaDust != NULL)
	{
		g_pTextureSeaDust->Release();
		g_pTextureSeaDust = NULL;		// 中身を空にする
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffSeaDust != NULL)
	{
		g_pVtxBuffSeaDust->Release();
		g_pVtxBuffSeaDust = NULL;		// 中身を空にする
	}
}

//======================================================================== 
// 塵の更新処理
//========================================================================
void UpdateSeaDust(void)
{
	for (int nCntDust = 0; nCntDust < MAX_SEADUST; nCntDust++)
	{
		if (g_aSeaDust[nCntDust].bUse == false)
		{// 使用されてない
			continue;
		}

		//float fAngleMove = (rand() % 100 + 1) / 100000.0f;

		//static float fAngle = 0.0f;
		//fAngle += fAngleMove * 0.01f;
		//CorrectAngle(&fAngle, fAngle);

		//g_aSeaDust[nCntDust].pos.y += cosf(fAngle);
	}
}

//======================================================================== 
// 塵の描画処理
//========================================================================
void DrawSeaDust(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス所得用

	for (int nCntDust = 0; nCntDust < MAX_SEADUST; nCntDust++)
	{
		if (g_aSeaDust[nCntDust].bUse == true)
		{// 使用されている場合
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aSeaDust[nCntDust].mtxWorld);			// ワールドマトリックスの初期値を設定

			// ビューマトリックスの所得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンの向きをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aSeaDust[nCntDust].mtxWorld, NULL, &mtxView);

			g_aSeaDust[nCntDust].mtxWorld._41 = 0.0f;
			g_aSeaDust[nCntDust].mtxWorld._42 = 0.0f;
			g_aSeaDust[nCntDust].mtxWorld._43 = 0.0f;

			// 位置を設定
			D3DXMatrixTranslation(&mtxTrans,
				g_aSeaDust[nCntDust].pos.x, g_aSeaDust[nCntDust].pos.y, g_aSeaDust[nCntDust].pos.z);

			D3DXMatrixMultiply(&g_aSeaDust[nCntDust].mtxWorld, &g_aSeaDust[nCntDust].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aSeaDust[nCntDust].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffSeaDust, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureSeaDust);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	// プリミティブの種類
				nCntDust * MAX_VERTEX,		// インデックス数
				MAX_POLYGON);					// ポリゴン数
		}
	}
}

//======================================================================== 
// 塵の設定
//========================================================================
void SetSeaDust(D3DXVECTOR3 pos)
{
	for (int nCntDust = 0; nCntDust < MAX_SEADUST; nCntDust++)
	{
		if (g_aSeaDust[nCntDust].bUse == false)
		{// 使用されてない
			g_aSeaDust[nCntDust].pos = pos;
			g_aSeaDust[nCntDust].bUse = true;

			break;
		}
	}
}

//======================================================================== 
// 塵のランダム設定
//========================================================================
void SetRandomSeaDust(int nAmount)
{
	for (int nCntDust = 0; nCntDust < nAmount; nCntDust++)
	{
		D3DXVECTOR3 pos;
		float fAngle = (D3DX_PI * 2.0f) * ((float)((nCntDust + 1) * (360.0f / nAmount)) / 360.0f);
		float fsin = sinf(fAngle);

		pos.x = sinf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)((OUTCYLINDER_RADIUS * 2) - INCYLINDER_RADIUS) + 1))));
		pos.y = (float)(rand() % (int)(CYLINDER_HEIGHT));
		pos.z = cosf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)((OUTCYLINDER_RADIUS * 2) - INCYLINDER_RADIUS) + 1))));

		SetSeaDust(pos);
	}
}