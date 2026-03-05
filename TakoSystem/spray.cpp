//=============================================================================
// 
// 爆発 [explosion.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "explosion.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_EXPLOSION			(256)									// 爆発の数
#define SIZE_EXPLOSION			(5.0f)									// 爆発のサイズ半径
#define TEXTURE_WIDTH			(0.125f)								// テクスチャパターンの幅
#define TEXTURE_NUM				(8)										// テクスチャパターンの列数
#define MAX_TIME				(4)										// テクスチャ切り替えのカウント
#define MAX_SIZE				(2.0f)									// 大きさ補正
#define START_ALPHA				(2)										// 透明度補正を始めるパターン数
#define MAX_ALPHA				(0.1f)									// 透明度補正

//*****************************************************************************
// 爆発構造体の定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 move;				// 移動量
	D3DXCOLOR col;					// 色
	int nCounterAnim;				// アニメーションカウンター
	int nPatternAnim;				// アニメーションパターンNO.
	float fWidth;					// 幅
	float fDepth;					// 高さ
	EXPLOSIONTYPE type;				// 種類
	bool bUse;						// 使用しているかどうか
}Explosion;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureExplosion[EXPLOSIONTYPE_MAX] = {};		// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffExplosion = NULL;			// 頂点バッファへのポインタ
Explosion g_aExplosion[MAX_EXPLOSION];								// 爆発の情報
D3DXMATRIX g_mtxWorldExplosion;								// ワールドマトリックス

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaExplosion[EXPLOSIONTYPE_MAX] =
{
	"data\\TEXTURE\\explosion000.png",
	"data\\TEXTURE\\explosion001.png",
	"data\\TEXTURE\\explosion002.png",
};

//=============================================================================
// 爆発の初期化処理
//=============================================================================
void InitExplosion(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < EXPLOSIONTYPE_MAX; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaExplosion[nCntTexture], &g_apTextureExplosion[nCntTexture]);
	}

	// 爆発情報の初期化
	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		g_aExplosion[nCntExplosion].pos = FIRST_POS;
		g_aExplosion[nCntExplosion].move = FIRST_POS;
		g_aExplosion[nCntExplosion].col = WHITE_VTX;
		g_aExplosion[nCntExplosion].nCounterAnim = 0;
		g_aExplosion[nCntExplosion].nPatternAnim = 0;
		g_aExplosion[nCntExplosion].fWidth = SIZE_EXPLOSION;
		g_aExplosion[nCntExplosion].fDepth = SIZE_EXPLOSION;
		g_aExplosion[nCntExplosion].type = EXPLOSIONTYPE_0;
		g_aExplosion[nCntExplosion].bUse = false;		// 使用していない状態にする
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * MAX_EXPLOSION,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffExplosion,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-g_aExplosion[nCntExplosion].fWidth, g_aExplosion[nCntExplosion].fDepth, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aExplosion[nCntExplosion].fWidth, g_aExplosion[nCntExplosion].fDepth, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-g_aExplosion[nCntExplosion].fWidth, -g_aExplosion[nCntExplosion].fDepth, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aExplosion[nCntExplosion].fWidth, -g_aExplosion[nCntExplosion].fDepth, 0.0f);

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
		pVtx[0].tex = D3DXVECTOR2(g_aExplosion[nCntExplosion].nPatternAnim * TEXTURE_WIDTH, 0.0f);
		pVtx[1].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * TEXTURE_WIDTH, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(g_aExplosion[nCntExplosion].nPatternAnim * TEXTURE_WIDTH, 1.0f);
		pVtx[3].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * TEXTURE_WIDTH, 1.0f);

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffExplosion->Unlock();
}

//=============================================================================
// 爆発の終了処理
//=============================================================================
void UninitExplosion(void)
{
	// テクスチャの破棄
	for (int nCntExplosion = 0; nCntExplosion < EXPLOSIONTYPE_MAX; nCntExplosion++)
	{
		if (g_apTextureExplosion[nCntExplosion] != NULL)
		{
			g_apTextureExplosion[nCntExplosion]->Release();
			g_apTextureExplosion[nCntExplosion] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffExplosion != NULL)
	{
		g_pVtxBuffExplosion->Release();
		g_pVtxBuffExplosion = NULL;
	}
}

//=============================================================================
// 爆発の更新処理
//=============================================================================
void UpdateExplosion(void)
{
	// ローカル変数宣言
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		if (g_aExplosion[nCntExplosion].bUse == true)
		{// 爆発が使用されている
			g_aExplosion[nCntExplosion].pos += g_aExplosion[nCntExplosion].move;

			g_aExplosion[nCntExplosion].nCounterAnim++;		// カウンターを加算

			if (g_aExplosion[nCntExplosion].nCounterAnim % MAX_TIME == 0)
			{// 一定時間経過した
				g_aExplosion[nCntExplosion].fWidth += MAX_SIZE;
				g_aExplosion[nCntExplosion].fDepth += MAX_SIZE;

				if (g_aExplosion[nCntExplosion].nPatternAnim > START_ALPHA)
				{
					g_aExplosion[nCntExplosion].col.a -= MAX_ALPHA;
				}

				g_aExplosion[nCntExplosion].nPatternAnim++;	// パターンNO.を更新

				// 頂点座標の設定
				pVtx[0].pos = D3DXVECTOR3(-g_aExplosion[nCntExplosion].fWidth, g_aExplosion[nCntExplosion].fDepth, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(g_aExplosion[nCntExplosion].fWidth, g_aExplosion[nCntExplosion].fDepth, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-g_aExplosion[nCntExplosion].fWidth, -g_aExplosion[nCntExplosion].fDepth, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(g_aExplosion[nCntExplosion].fWidth, -g_aExplosion[nCntExplosion].fDepth, 0.0f);

				// 頂点カラーの設定
				pVtx[0].col = g_aExplosion[nCntExplosion].col;
				pVtx[1].col = g_aExplosion[nCntExplosion].col;
				pVtx[2].col = g_aExplosion[nCntExplosion].col;
				pVtx[3].col = g_aExplosion[nCntExplosion].col;

				// テクスチャ座標の更新
				pVtx[0].tex = D3DXVECTOR2(g_aExplosion[nCntExplosion].nPatternAnim * TEXTURE_WIDTH, 0.0f);
				pVtx[1].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * TEXTURE_WIDTH, 0.0f);
				pVtx[2].tex = D3DXVECTOR2(g_aExplosion[nCntExplosion].nPatternAnim * TEXTURE_WIDTH, 1.0f);
				pVtx[3].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * TEXTURE_WIDTH, 1.0f);
			}

			if (g_aExplosion[nCntExplosion].nPatternAnim >= TEXTURE_NUM)
			{// 総パターン数を超えた
				g_aExplosion[nCntExplosion].bUse = false;		// 使用していない状態にする
			}
		}

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffExplosion->Unlock();
}

//=============================================================================
// 爆発の描画処理
//=============================================================================
void DrawExplosion(void)
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

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		if (g_aExplosion[nCntExplosion].bUse == true)
		{// 爆発が使用されている
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldExplosion);

			// ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_mtxWorldExplosion, NULL, &mtxView);	// 逆行列を求める
			g_mtxWorldExplosion._41 = 0.0f;
			g_mtxWorldExplosion._42 = 0.0f;
			g_mtxWorldExplosion._43 = 0.0f;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aExplosion[nCntExplosion].pos.x, g_aExplosion[nCntExplosion].pos.y, g_aExplosion[nCntExplosion].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldExplosion, &g_mtxWorldExplosion, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldExplosion);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffExplosion, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureExplosion[g_aExplosion[nCntExplosion].type]);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCntExplosion * MAX_VERTEX,		// 描画する最初の頂点インデックス
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
// 爆発の設定処理
//=============================================================================
void SetExplosion(D3DXVECTOR3 pos, D3DXVECTOR3 move, float length, EXPLOSIONTYPE type)
{
	// ローカル変数宣言
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffExplosion->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		if (g_aExplosion[nCntExplosion].bUse == false)
		{// 爆発が使用されていない
			g_aExplosion[nCntExplosion].pos = pos;
			g_aExplosion[nCntExplosion].move = move;
			g_aExplosion[nCntExplosion].col = WHITE_VTX;
			g_aExplosion[nCntExplosion].fWidth = length;
			g_aExplosion[nCntExplosion].fDepth = length;
			g_aExplosion[nCntExplosion].type = type;
			g_aExplosion[nCntExplosion].nCounterAnim = 0;
			g_aExplosion[nCntExplosion].nPatternAnim = 0;

			// 頂点座標の更新
			pVtx[0].pos = D3DXVECTOR3(-g_aExplosion[nCntExplosion].fWidth, g_aExplosion[nCntExplosion].fDepth, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(g_aExplosion[nCntExplosion].fWidth, g_aExplosion[nCntExplosion].fDepth, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aExplosion[nCntExplosion].fWidth, -g_aExplosion[nCntExplosion].fDepth, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(g_aExplosion[nCntExplosion].fWidth, -g_aExplosion[nCntExplosion].fDepth, 0.0f);

			// 頂点カラーの更新
			pVtx[0].col = g_aExplosion[nCntExplosion].col;
			pVtx[1].col = g_aExplosion[nCntExplosion].col;
			pVtx[2].col = g_aExplosion[nCntExplosion].col;
			pVtx[3].col = g_aExplosion[nCntExplosion].col;

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(g_aExplosion[nCntExplosion].nPatternAnim * TEXTURE_WIDTH, 0.0f);
			pVtx[1].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * TEXTURE_WIDTH, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(g_aExplosion[nCntExplosion].nPatternAnim * TEXTURE_WIDTH, 1.0f);
			pVtx[3].tex = D3DXVECTOR2((g_aExplosion[nCntExplosion].nPatternAnim + 1) * TEXTURE_WIDTH, 1.0f);

			g_aExplosion[nCntExplosion].bUse = true;			// 使用している状態にする

			break;
		}

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffExplosion->Unlock();
}