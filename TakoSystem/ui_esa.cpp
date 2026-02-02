//==================================================================================
// 
// エサのUIに関する処理[ui_esa.cpp]
// Author : Soki Inamura 
// 
//==================================================================================

#include "main.h"		// メインヘッダー

#include "esa.h"		// エサヘッダー

#include "ui_esa.h"		// エサUIヘッダー

#include "input.h"

// マクロ定義 ==================================================

#define MAX_SET_UIESA		(128)		// エサUIを設定出来る数

#define UIESA_SIZE_WIDTH	(10.0f)		// 
#define UIESA_SIZE_HEIGHT	(10.0f)		// 

// 構造体の定義 ================================================

// エサUIの情報
typedef struct
{
	int nIdxTexture;	// テクスチャインデックス

	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 角度
	float fSizeLength;	// 対角線の長さ
	float fSizeAngle;	// 対角線の角度

	bool bDisp;			// 表示状態
	bool bUse;			// 使用状態

}UiEsa;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureUiEsa[MAX_MODEL_ESA] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUiEsa = NULL;				// 頂点バッファ

UiEsa g_aUiEsa[MAX_SET_UIESA];								// エサUIの情報

int g_nNumUiEsa;											// エサの総数

// テクスチャファイル情報
const char* c_apFilenameUiEsa[] =	// 最終的にMAX_MODEL_ESAと同じ値にする
{
	"data/TEXTURE/esa_tmp_texture/esa_tmp_crab.png",	// [0]カニ
	"data/TEXTURE/esa_tmp_texture/esa_tmp_shell.png",	// [1]貝
	"data/TEXTURE/esa_tmp_texture/esa_tmp_shrimp.png",	// [2]エビ
};

//========================================================================
// UIエサの初期化処理
//========================================================================
void InitUiEsa(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ====================================================

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameUiEsa / sizeof c_apFilenameUiEsa[0]; nCntTexture++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,							// Direct3Dデバイスへのポインタ
								  c_apFilenameUiEsa[nCntTexture],	// 読み込むテクスチャ
								  &g_apTextureUiEsa[nCntTexture]);	// テクスチャへのポインタ
	}

	// エサUIの情報を初期化
	for (int nCntUiEsa = 0; nCntUiEsa < MAX_SET_UIESA; nCntUiEsa++)
	{
		g_aUiEsa[nCntUiEsa].nIdxTexture = -1;;
		g_aUiEsa[nCntUiEsa].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aUiEsa[nCntUiEsa].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aUiEsa[nCntUiEsa].fSizeLength = 0.0f;
		g_aUiEsa[nCntUiEsa].fSizeAngle = 0.0f;
		g_aUiEsa[nCntUiEsa].bDisp = false;
		g_aUiEsa[nCntUiEsa].bUse = false;
	}

	g_nNumUiEsa = 0;		// エサの総数を初期化

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_SET_UIESA,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUiEsa,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffUiEsa->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntUiEsa = 0; nCntUiEsa < MAX_SET_UIESA; nCntUiEsa++)
	{
		// 頂点情報の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)			 
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)			 
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)			 
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)		 

		// rwhの設定
		pVtx[0].rhw = 1.0f;									// 1.0fで固定
		pVtx[1].rhw = 1.0f;									// 1.0fで固定
		pVtx[2].rhw = 1.0f;									// 1.0fで固定
		pVtx[3].rhw = 1.0f;									// 1.0fで固定

		// 頂点カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);				// (U, V) 
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);				// (U, V)
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);				// (U, V)
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);				// (U, V)

		pVtx += 4;
	}
	// ▲頂点バッファをアンロックする
	g_pVtxBuffUiEsa->Unlock();

	SetUiEsa(0,D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));
}

//========================================================================
// UIエサの終了処理  
//========================================================================
void UninitUiEsa(void)		
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_MODEL_ESA; nCntTexture++)
	{
		if (g_apTextureUiEsa[nCntTexture] != NULL)
		{
			g_apTextureUiEsa[nCntTexture]->Release();
			g_apTextureUiEsa[nCntTexture] = NULL;		// 中身を空にする
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffUiEsa != NULL)
	{
		g_pVtxBuffUiEsa->Release();
		g_pVtxBuffUiEsa = NULL;		// 中身を空にする
	}
}

//========================================================================
// UIエサの更新処理  
//========================================================================
void UpdateUiEsa(void)
{
	// 変数宣言 ================================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// =========================================================

#if 1
	if (GetKeyboardPress(DIK_I)) g_aUiEsa[0].pos.y -= 1.0f;
	if (GetKeyboardPress(DIK_K)) g_aUiEsa[0].pos.y += 1.0f;
	if (GetKeyboardPress(DIK_J)) g_aUiEsa[0].pos.x -= 1.0f;
	if (GetKeyboardPress(DIK_L)) g_aUiEsa[0].pos.x += 1.0f;

	if (GetKeyboardPress(DIK_U)) g_aUiEsa[0].fSizeLength += 1.0f;
	if (GetKeyboardPress(DIK_M)) g_aUiEsa[0].fSizeLength -= 1.0f;
#endif

	for (int nCntUiEsa = 0; nCntUiEsa < MAX_SET_UIESA; nCntUiEsa++)
	{
		// ▼頂点バッファをロックして頂点情報へのポインタを所得
		g_pVtxBuffUiEsa->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nCntUiEsa * 4);

		// 頂点座標の設定
		pVtx[0].pos.x = g_aUiEsa[nCntUiEsa].pos.x - sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
		pVtx[1].pos.x = g_aUiEsa[nCntUiEsa].pos.x + sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
		pVtx[2].pos.x = g_aUiEsa[nCntUiEsa].pos.x - sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
		pVtx[3].pos.x = g_aUiEsa[nCntUiEsa].pos.x + sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;

		pVtx[0].pos.y = g_aUiEsa[nCntUiEsa].pos.y - cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
		pVtx[1].pos.y = g_aUiEsa[nCntUiEsa].pos.y - cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
		pVtx[2].pos.y = g_aUiEsa[nCntUiEsa].pos.y + cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
		pVtx[3].pos.y = g_aUiEsa[nCntUiEsa].pos.y + cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;

		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.z = 0.0f;

		// ▲頂点バッファをアンロックする
		g_pVtxBuffUiEsa->Unlock();
	}
}

//========================================================================
// UIエサの描画処理  
//========================================================================
void DrawUiEsa(void)		
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// =========================================================

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffUiEsa, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntUiEsa = 0; nCntUiEsa < MAX_SET_UIESA; nCntUiEsa++)
	{// 用意した分繰り返す

		if (g_aUiEsa[nCntUiEsa].bUse == true && g_aUiEsa[nCntUiEsa].bDisp == true)
		{// 使用していて表示している状態の場合

			// テクスチャの設定
			if (g_aUiEsa[nCntUiEsa].nIdxTexture != -1)
			{// テクスチャが設定されている場合

				pDevice->SetTexture(0, g_apTextureUiEsa[g_aUiEsa[nCntUiEsa].nIdxTexture]);
			}
			else
			{// テクスチャが設定されていない場合

				pDevice->SetTexture(0, NULL);
			}

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
								   nCntUiEsa * 4,			// インデックス
								   2);						// プリミティブ（ポリゴン）数
		}
	}
}

//========================================================================
// UIエサの設定処理  
//========================================================================
void SetUiEsa(int nIdxType, D3DXVECTOR3 pos)
{
	// 変数宣言 ================================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	float fLength, fAngle;

	// =========================================================
	for (int nCntUiEsa = 0; nCntUiEsa < MAX_SET_UIESA; nCntUiEsa++)
	{
		if (g_aUiEsa[nCntUiEsa].bUse == false)
		{// 使用していない場合

			fLength = sqrtf((UIESA_SIZE_WIDTH * 2) * (UIESA_SIZE_WIDTH * 2) + (UIESA_SIZE_HEIGHT * 2) * (UIESA_SIZE_HEIGHT * 2)) * 0.5f;

			fAngle = atan2f((UIESA_SIZE_WIDTH * 2), (UIESA_SIZE_HEIGHT * 2));
			
			g_aUiEsa[nCntUiEsa].nIdxTexture = nIdxType;					// テクスチャインデックスを設定
			g_aUiEsa[nCntUiEsa].pos = pos;								// 位置を設定
			g_aUiEsa[nCntUiEsa].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 角度を設定
			g_aUiEsa[nCntUiEsa].fSizeLength = fLength;					// 対角線の長さを設定
			g_aUiEsa[nCntUiEsa].fSizeAngle = fAngle;					// 対角線の角度を設定
			g_aUiEsa[nCntUiEsa].bDisp = true;							// 表示している状態に設定
			g_aUiEsa[nCntUiEsa].bUse = true;							// 使用している状態に設定

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffUiEsa->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntUiEsa * 4);

			// 頂点座標の設定
			pVtx[0].pos.x = g_aUiEsa[nCntUiEsa].pos.x - sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
			pVtx[1].pos.x = g_aUiEsa[nCntUiEsa].pos.x + sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
			pVtx[2].pos.x = g_aUiEsa[nCntUiEsa].pos.x - sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
			pVtx[3].pos.x = g_aUiEsa[nCntUiEsa].pos.x + sinf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;

			pVtx[0].pos.y = g_aUiEsa[nCntUiEsa].pos.y - cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
			pVtx[1].pos.y = g_aUiEsa[nCntUiEsa].pos.y - cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
			pVtx[2].pos.y = g_aUiEsa[nCntUiEsa].pos.y + cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;
			pVtx[3].pos.y = g_aUiEsa[nCntUiEsa].pos.y + cosf(g_aUiEsa[nCntUiEsa].fSizeAngle) * g_aUiEsa[nCntUiEsa].fSizeLength;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// ▲頂点バッファをアンロックする
			g_pVtxBuffUiEsa->Unlock();

			g_nNumUiEsa++;

			break;
		}
	}
}
