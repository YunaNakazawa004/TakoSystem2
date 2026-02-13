//=============================================================================
// 
// ビルボード [billboard.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "billboard.h"
#include "input.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_VERTEX				(4)										// 最大頂点数
#define MAX_POLYGON				(2)										// 最大ポリゴン数
#define NUM_BILLBOARD			(1)										// ビルボードの数
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define CENTER					(D3DXVECTOR3(640.0f, 360.0f, 0.0f))		// 中心座標
#define FIRST_POS				(D3DXVECTOR3(0.0f, 0.0f, 0.0f))			// 初期座標
#define WHITE_VTX				(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))		// 頂点カラーが白

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureBillboard[NUM_BILLBOARD] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBillboard = NULL;			// 頂点バッファへのポインタ
D3DXVECTOR3 g_posBillboard;									// ビルボードの位置情報
D3DXMATRIX g_mtxWorldBillboard;								// ワールドマトリックス

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaBillboard[NUM_BILLBOARD] =
{
	"data\\TEXTURE\\ski000.jpg",
};

//=============================================================================
// ビルボードの初期化処理
//=============================================================================
void InitBillboard(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < NUM_BILLBOARD; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaBillboard[nCntTexture], &g_apTextureBillboard[nCntTexture]);
	}

	// ビルボード情報の初期化
	g_posBillboard = FIRST_POS;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffBillboard,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffBillboard->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(-200.0f, 200.0f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(200.0f, 200.0f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-200.0f, -200.0f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(200.0f,-200.0f, 0.0f);

	// rhwの設定					 
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

	// 頂点バッファをアンロックする
	g_pVtxBuffBillboard->Unlock();
}

//=============================================================================
// ビルボードの終了処理
//=============================================================================
void UninitBillboard(void)
{
	// テクスチャの破棄
	for (int nCntBillboard = 0; nCntBillboard < NUM_BILLBOARD; nCntBillboard++)
	{
		if (g_apTextureBillboard[nCntBillboard] != NULL)
		{
			g_apTextureBillboard[nCntBillboard]->Release();
			g_apTextureBillboard[nCntBillboard] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffBillboard != NULL)
	{
		g_pVtxBuffBillboard->Release();
		g_pVtxBuffBillboard = NULL;
	}
}

//=============================================================================
// ビルボードの更新処理
//=============================================================================
void UpdateBillboard(void)
{
}

//=============================================================================
// ビルボードの描画処理
//=============================================================================
void DrawBillboard(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxTrans;				// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス取得用

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldBillboard);

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// ポリゴンをカメラに対して正面に向ける
	D3DXMatrixInverse(&g_mtxWorldBillboard, NULL, &mtxView);	// 逆行列を求める
	g_mtxWorldBillboard._41 = 0.0f;
	g_mtxWorldBillboard._42 = 0.0f;
	g_mtxWorldBillboard._43 = 0.0f;

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, g_posBillboard.x, g_posBillboard.y, g_posBillboard.z);
	D3DXMatrixMultiply(&g_mtxWorldBillboard, &g_mtxWorldBillboard, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldBillboard);

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffBillboard, 0, sizeof(VERTEX_3D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_apTextureBillboard[0]);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,		// 描画する最初の頂点インデックス
		MAX_POLYGON);		// 描画するプリミティブ数

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
}