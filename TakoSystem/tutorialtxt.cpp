//=============================================================================
// 
// チュートリアルテキスト [tutorialtxt.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "tutorialtxt.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_TUTORIALTXT			(2)										// チュートリアルテキストの数
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量

//*****************************************************************************
// チュートリアルテキスト構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	bool bUse;					// 使用しているか
}TutorialTxt;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureTutorialTxt[NUM_TUTORIALTXT] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTutorialTxt = NULL;			// 頂点バッファへのポインタ
TutorialTxt g_aTutorialTxt[NUM_TUTORIALTXT];				// チュートリアルテキストの情報

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaTutorialTxt[NUM_TUTORIALTXT] =
{
	"data\\TEXTURE\\tutorialtxt000.png",
	"data\\TEXTURE\\tutorialtxt001.png",
};

//=============================================================================
// チュートリアルテキストの初期化処理
//=============================================================================
void InitTutorialTxt(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < NUM_TUTORIALTXT; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaTutorialTxt[nCntTexture], &g_apTextureTutorialTxt[nCntTexture]);
	}

	// チュートリアルテキスト情報の初期化
	for (int nCntTxt = 0; nCntTxt < NUM_TUTORIALTXT; nCntTxt++)
	{
		g_aTutorialTxt[nCntTxt].pos = FIRST_POS;
		g_aTutorialTxt[nCntTxt].bUse = false;
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * NUM_TUTORIALTXT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTutorialTxt,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTutorialTxt->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTxt = 0; nCntTxt < NUM_TUTORIALTXT; nCntTxt++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-200.0f, 100.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(200.0f, 100.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-200.0f, -100.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(200.0f, -100.0f, 0.0f);

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

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTutorialTxt->Unlock();

	SetTutorialTxt(D3DXVECTOR3(750.0f, 1000.0f, 0.0f));
	SetTutorialTxt(D3DXVECTOR3(-750.0f, 1000.0f, 0.0f));
}

//=============================================================================
// チュートリアルテキストの終了処理
//=============================================================================
void UninitTutorialTxt(void)
{
	// テクスチャの破棄
	for (int nCntTutorialTxt = 0; nCntTutorialTxt < NUM_TUTORIALTXT; nCntTutorialTxt++)
	{
		if (g_apTextureTutorialTxt[nCntTutorialTxt] != NULL)
		{
			g_apTextureTutorialTxt[nCntTutorialTxt]->Release();
			g_apTextureTutorialTxt[nCntTutorialTxt] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTutorialTxt != NULL)
	{
		g_pVtxBuffTutorialTxt->Release();
		g_pVtxBuffTutorialTxt = NULL;
	}
}

//=============================================================================
// チュートリアルテキストの更新処理
//=============================================================================
void UpdateTutorialTxt(void)
{
}

//=============================================================================
// チュートリアルテキストの描画処理
//=============================================================================
void DrawTutorialTxt(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxTrans;				// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス取得用

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (int nCntTxt = 0; nCntTxt < NUM_TUTORIALTXT; nCntTxt++)
	{
		if (g_aTutorialTxt[nCntTxt].bUse == false)
		{// 使用してないなら無視
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aTutorialTxt[nCntTxt].mtxWorld);

		// ビューマトリックスを取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		// ポリゴンをカメラに対して正面に向ける
		D3DXMatrixInverse(&g_aTutorialTxt[nCntTxt].mtxWorld, NULL, &mtxView);	// 逆行列を求める
		g_aTutorialTxt[nCntTxt].mtxWorld._41 = 0.0f;
		g_aTutorialTxt[nCntTxt].mtxWorld._42 = 0.0f;
		g_aTutorialTxt[nCntTxt].mtxWorld._43 = 0.0f;

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_aTutorialTxt[nCntTxt].pos.x, g_aTutorialTxt[nCntTxt].pos.y, g_aTutorialTxt[nCntTxt].pos.z);
		D3DXMatrixMultiply(&g_aTutorialTxt[nCntTxt].mtxWorld, &g_aTutorialTxt[nCntTxt].mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aTutorialTxt[nCntTxt].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffTutorialTxt, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureTutorialTxt[nCntTxt]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCntTxt * MAX_VERTEX,		// 描画する最初の頂点インデックス
			MAX_POLYGON);		// 描画するプリミティブ数
	}

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//=============================================================================
// チュートリアルテキストの設定
//=============================================================================
void SetTutorialTxt(D3DXVECTOR3 pos)
{
	for (int nCntTxt = 0; nCntTxt < NUM_TUTORIALTXT; nCntTxt++)
	{
		if (g_aTutorialTxt[nCntTxt].bUse == false)
		{// 使用してない
			g_aTutorialTxt[nCntTxt].pos = pos;
			g_aTutorialTxt[nCntTxt].bUse = true;

			break;
		}
	}
}