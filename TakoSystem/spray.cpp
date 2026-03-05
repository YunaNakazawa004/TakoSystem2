//=============================================================================
// 
// 飛沫 [spray.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "spray.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SPRAY				(256)									// 飛沫の数
#define SIZE_SPRAY				(30.0f)									// 飛沫のサイズ半径
#define MAX_SIZE				(2.0f)									// 大きさ補正
#define START_ALPHA				(2)										// 透明度補正を始めるパターン数
#define MAX_ALPHA				(0.1f)									// 透明度補正
#define GRAVITY					(-0.05f)								// 重力

//*****************************************************************************
// 飛沫構造体の定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 move;				// 移動量
	D3DXCOLOR col;					// 色
	float fWidth;					// 幅
	float fDepth;					// 高さ
	SPRAYTYPE type;					// 種類
	bool bUse;						// 使用しているかどうか
}Spray;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureSpray[SPRAYTYPE_MAX] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSpray = NULL;			// 頂点バッファへのポインタ
Spray g_aSpray[MAX_SPRAY];								// 飛沫の情報
D3DXMATRIX g_mtxWorldSpray;								// ワールドマトリックス

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaSpray[SPRAYTYPE_MAX] =
{
	"data\\TEXTURE\\spray000.png",
	"data\\TEXTURE\\spray000.png",
};

//=============================================================================
// 飛沫の初期化処理
//=============================================================================
void InitSpray(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < SPRAYTYPE_MAX; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaSpray[nCntTexture], &g_apTextureSpray[nCntTexture]);
	}

	// 爆発情報の初期化
	for (int nCntSpray = 0; nCntSpray < MAX_SPRAY; nCntSpray++)
	{
		g_aSpray[nCntSpray].pos = FIRST_POS;
		g_aSpray[nCntSpray].move = FIRST_POS;
		g_aSpray[nCntSpray].col = WHITE_VTX;
		g_aSpray[nCntSpray].fWidth = SIZE_SPRAY;
		g_aSpray[nCntSpray].fDepth = SIZE_SPRAY;
		g_aSpray[nCntSpray].type = SPRAYTYPE_CIRCLE;
		g_aSpray[nCntSpray].bUse = false;		// 使用していない状態にする
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * MAX_SPRAY,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffSpray,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffSpray->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntSpray = 0; nCntSpray < MAX_SPRAY; nCntSpray++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-g_aSpray[nCntSpray].fWidth, g_aSpray[nCntSpray].fDepth, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aSpray[nCntSpray].fWidth, g_aSpray[nCntSpray].fDepth, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-g_aSpray[nCntSpray].fWidth, -g_aSpray[nCntSpray].fDepth, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aSpray[nCntSpray].fWidth, -g_aSpray[nCntSpray].fDepth, 0.0f);

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
	g_pVtxBuffSpray->Unlock();
}

//=============================================================================
// 飛沫の終了処理
//=============================================================================
void UninitSpray(void)
{
	// テクスチャの破棄
	for (int nCntSpray = 0; nCntSpray < SPRAYTYPE_MAX; nCntSpray++)
	{
		if (g_apTextureSpray[nCntSpray] != NULL)
		{
			g_apTextureSpray[nCntSpray]->Release();
			g_apTextureSpray[nCntSpray] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffSpray != NULL)
	{
		g_pVtxBuffSpray->Release();
		g_pVtxBuffSpray = NULL;
	}
}

//=============================================================================
// 飛沫の更新処理
//=============================================================================
void UpdateSpray(void)
{
	// ローカル変数宣言
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffSpray->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntSpray = 0; nCntSpray < MAX_SPRAY; nCntSpray++)
	{
		if (g_aSpray[nCntSpray].bUse == true)
		{// 飛沫が使用されている
			switch (g_aSpray[nCntSpray].type)
			{
			case SPRAYTYPE_CIRCLE:
				g_aSpray[nCntSpray].col.a -= 0.05f;

				if (g_aSpray[nCntSpray].col.a < 0.0f)
				{// 透明になった
					g_aSpray[nCntSpray].bUse = false;
				}

				g_aSpray[nCntSpray].move.y += GRAVITY;

				break;

			case SPRAYTYPE_FLOW:
				g_aSpray[nCntSpray].col.a -= 0.03f;

				if (g_aSpray[nCntSpray].col.a < 0.0f)
				{// 透明になった
					g_aSpray[nCntSpray].bUse = false;
				}

				g_aSpray[nCntSpray].move.x += (0.0f - g_aSpray[nCntSpray].move.x) * 0.01f;
				g_aSpray[nCntSpray].move.z += (0.0f - g_aSpray[nCntSpray].move.z) * 0.01f;

				break;
			}

			g_aSpray[nCntSpray].pos += g_aSpray[nCntSpray].move;

			// 頂点カラーの設定
			pVtx[0].col = g_aSpray[nCntSpray].col;
			pVtx[1].col = g_aSpray[nCntSpray].col;
			pVtx[2].col = g_aSpray[nCntSpray].col;
			pVtx[3].col = g_aSpray[nCntSpray].col;
		}

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffSpray->Unlock();
}

//=============================================================================
// 飛沫の描画処理
//=============================================================================
void DrawSpray(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxTrans;				// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス取得用

	// ライティングオフ
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//// アルファテストを有効にする
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	for (int nCntSpray = 0; nCntSpray < MAX_SPRAY; nCntSpray++)
	{
		if (g_aSpray[nCntSpray].bUse == true)
		{// 飛沫が使用されている
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldSpray);

			// ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_mtxWorldSpray, NULL, &mtxView);	// 逆行列を求める
			g_mtxWorldSpray._41 = 0.0f;
			g_mtxWorldSpray._42 = 0.0f;
			g_mtxWorldSpray._43 = 0.0f;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aSpray[nCntSpray].pos.x, g_aSpray[nCntSpray].pos.y, g_aSpray[nCntSpray].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldSpray, &g_mtxWorldSpray, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldSpray);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffSpray, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureSpray[g_aSpray[nCntSpray].type]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCntSpray * MAX_VERTEX,		// 描画する最初の頂点インデックス
				MAX_POLYGON);		// 描画するプリミティブ数
		}
	}

	//// アルファテストを無効にする
	//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// ライティングオン
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//=============================================================================
// 飛沫の設定処理
//=============================================================================
void SetSpray(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float length, SPRAYTYPE type)
{
	// ローカル変数宣言
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffSpray->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntSpray = 0; nCntSpray < MAX_SPRAY; nCntSpray++)
	{
		if (g_aSpray[nCntSpray].bUse == false)
		{// 飛沫が使用されていない
			g_aSpray[nCntSpray].pos = pos;
			g_aSpray[nCntSpray].move = move;
			g_aSpray[nCntSpray].col = col;
			g_aSpray[nCntSpray].fWidth = length;
			g_aSpray[nCntSpray].fDepth = length;
			g_aSpray[nCntSpray].type = type;

			// 頂点座標の更新
			pVtx[0].pos = D3DXVECTOR3(-g_aSpray[nCntSpray].fWidth, g_aSpray[nCntSpray].fDepth, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aSpray[nCntSpray].fWidth, g_aSpray[nCntSpray].fDepth, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aSpray[nCntSpray].fWidth, -g_aSpray[nCntSpray].fDepth, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aSpray[nCntSpray].fWidth, -g_aSpray[nCntSpray].fDepth, 0.0f);

			// 頂点カラーの更新
			pVtx[0].col = g_aSpray[nCntSpray].col;
			pVtx[1].col = g_aSpray[nCntSpray].col;
			pVtx[2].col = g_aSpray[nCntSpray].col;
			pVtx[3].col = g_aSpray[nCntSpray].col;

			g_aSpray[nCntSpray].bUse = true;			// 使用している状態にする

			break;
		}

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffSpray->Unlock();
}

//=============================================================================
// 飛沫の円形設定処理
//=============================================================================
void SetSprayCircle(D3DXVECTOR3 pos, D3DXCOLOR col, SPRAYTYPE type)
{
	for (int nCntSpray = 0; nCntSpray < 8; nCntSpray++)
	{
		SetSpray(pos, D3DXVECTOR3(sinf(D3DX_PI * (nCntSpray * 0.25f)) * 4.0f, 0.6f, cosf(D3DX_PI * (nCntSpray * 0.25f)) * 4.0f),
			col, SIZE_SPRAY, type);
	}
}

//=============================================================================
// 飛沫の波形設定処理
//=============================================================================
void SetSprayFlow(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXCOLOR col, SPRAYTYPE type)
{
	SetSpray(pos, D3DXVECTOR3(sinf(rot.y + (D3DX_PI * 0.5f)) * 2.0f, 0.0f, cosf(rot.y + (D3DX_PI * 0.5f)) * 2.0f),
		col, SIZE_SPRAY, type);

	SetSpray(pos, D3DXVECTOR3(sinf(rot.y - (D3DX_PI * 0.5f)) * 2.0f, 0.0f, cosf(rot.y - (D3DX_PI * 0.5f)) * 2.0f),
		col, SIZE_SPRAY, type);
}