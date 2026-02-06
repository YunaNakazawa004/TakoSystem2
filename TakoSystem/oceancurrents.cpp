//==================================================================================
// 
// 海流に関する処理[oceancurrents.cpp]
// Author : Soki Inamura 
// 
//==================================================================================

#include "main.h"			// メインヘッダー

#include "oceancurrents.h"	// 海流ヘッダー

#include "meshcylinder.h"

#include "input.h"
#include "debugproc.h"

// マクロ定義 ==================================================

#define MAX_SET_OCEANCURRENTS		(128)		// 設定出来る数
#define MAX_TEX_OCEANCURRENTS		(64)		// 設定出来るテクスチャ数

#define OCEANCURRECT_TIME_NOMAL		(60 * 20)	// 通常状態の継続時間
#define OCEANCURRECT_SPEED_NOMAL	(0.00005f)	// 通常時の海流の速さ	

#define OCEANCURRECT_TIME_WAIT		(60 * 5)	// 渦潮待機状態の継続時間
#define OCEANCURRECT_SPEED_WAIT		(0.0001f)	// 渦潮待機時の海流の速さ

#define OCEANCURRECT_TIME_WIRLPOOL	(60 * 7)	// 渦潮状態の継続時間
#define OCEANCURRECT_SPEED_WIRLPOOL	(0.001f)	// 渦潮時の海流の速さ


// 構造体の定義 ================================================

// 渦潮のポリゴン情報
typedef struct
{
	int nIdxTexture;
	int nIdxPos;

	D3DXVECTOR3 offPos;	
	D3DXVECTOR3 rot;
	float fSizeWidth;
	float fSizeHeight;

	D3DXCOLOR col;

	bool bDisp;
	bool bUse;

}OceanCurrents;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureOceanCurrents[MAX_SET_OCEANCURRENTS] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffOceanCurrents = NULL;						// 頂点バッファ

OceanCurrents g_aOceanCurrents[MAX_SET_OCEANCURRENTS];						// 渦潮のポリゴン情報

OCEANCURRENTSSTATE g_OceanCurrentsState;	// 海流の状態
int g_nCounterOceanCurrents;				// 海流カウンター
float g_fOceanCurrent;						// 海流の移動量

D3DXVECTOR3 g_aPosOceanCurrents[1];

// テクスチャファイル情報
const char* c_apFilenameOceanCurrents[] =
{
	"data/TEXTURE/whirlpool_warning.png",	
};

//========================================================================
// 海流の初期化処理
//========================================================================
void InitOceanCurrents(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntOceanCurrents;

	// ====================================================

	// テクスチャの読み込み
	for (nCntOceanCurrents = 0; nCntOceanCurrents < sizeof c_apFilenameOceanCurrents / sizeof c_apFilenameOceanCurrents[0]; nCntOceanCurrents++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,								// Direct3Dデバイスへのポインタ
								  c_apFilenameOceanCurrents[nCntOceanCurrents],	// 読み込むテクスチャ
								  &g_apTextureOceanCurrents[nCntOceanCurrents]);	// テクスチャへのポインタ
	}

	// 渦潮のポリゴン情報を初期化
	for (nCntOceanCurrents = 0; nCntOceanCurrents < MAX_SET_OCEANCURRENTS; nCntOceanCurrents++)
	{
		g_aOceanCurrents[nCntOceanCurrents].nIdxTexture = -1;							// テクスチャのインデックスを設定
		g_aOceanCurrents[nCntOceanCurrents].nIdxPos = -1;								// 中心点のインデックスを設定
		g_aOceanCurrents[nCntOceanCurrents].offPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// オフセットの位置を設定
		g_aOceanCurrents[nCntOceanCurrents].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 角度を設定
		g_aOceanCurrents[nCntOceanCurrents].fSizeWidth = 0.0f;							// 大きさ(幅)を設定
		g_aOceanCurrents[nCntOceanCurrents].fSizeHeight = 0.0f;							// 大きさ高さ)を設定
		g_aOceanCurrents[nCntOceanCurrents].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);	// 色を設定
		g_aOceanCurrents[nCntOceanCurrents].bDisp = false;								// 表示している状態に設定
		g_aOceanCurrents[nCntOceanCurrents].bUse = false;								// 使用している状態に設定
	}

	g_nCounterOceanCurrents = 0;						// カウンタの値を初期化
	g_fOceanCurrent = OCEANCURRECT_SPEED_NOMAL;		// 

	g_aPosOceanCurrents[0] = D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_SET_OCEANCURRENTS,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffOceanCurrents,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffOceanCurrents->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntOceanCurrents = 0; nCntOceanCurrents < MAX_SET_OCEANCURRENTS; nCntOceanCurrents++)
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
	g_pVtxBuffOceanCurrents->Unlock();

	//SetOceanCurrents(0,D3DXVECTOR3(0.0f, 0.0f, 0.0f),D3DXVECTOR3(0.0f,0.0f,0.0f), 20.0f,20.0f);
}

//========================================================================
// 海流の終了処理　
//========================================================================
void UninitOceanCurrents(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_OCEANCURRENTS; nCntTexture++)
	{
		if (g_apTextureOceanCurrents[nCntTexture] != NULL)
		{
			g_apTextureOceanCurrents[nCntTexture]->Release();
			g_apTextureOceanCurrents[nCntTexture] = NULL;		// 中身を空にする
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffOceanCurrents != NULL)
	{
		g_pVtxBuffOceanCurrents->Release();
		g_pVtxBuffOceanCurrents = NULL;		// 中身を空にする
	}
}

//========================================================================
// 海流の更新処理　
//========================================================================
void UpdateOceanCurrents(void)
{
	// 変数宣言 ================================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	D3DXVECTOR3 SetPos;
	float fAngle, fLength;

	// =========================================================

#if 0
	if (GetKeyboardPress(DIK_I)) g_aPosOceanCurrents[0].y -= 1.0f;
	if (GetKeyboardPress(DIK_K)) g_aPosOceanCurrents[0].y += 1.0f;
	if (GetKeyboardPress(DIK_J)) g_aPosOceanCurrents[0].x -= 1.0f;
	if (GetKeyboardPress(DIK_L)) g_aPosOceanCurrents[0].x += 1.0f;

	if (GetKeyboardPress(DIK_U)) {g_aOceanCurrents[0].fSizeWidth += 1.0f; g_aOceanCurrents[0].fSizeHeight += 1.0f;}
	if (GetKeyboardPress(DIK_M)) {g_aOceanCurrents[0].fSizeWidth -= 1.0f; g_aOceanCurrents[0].fSizeHeight -= 1.0f;}
#endif

	g_nCounterOceanCurrents++;	// カウンタを加算
	
	// 海流の回転速度の処理
	switch (g_OceanCurrentsState)
	{
	case OCEANCURRENTSSTATE_NOMAL:		// 通常時
		
		// 回転量を通常時の移動量に修正
		g_fOceanCurrent += OCEANCURRECT_SPEED_NOMAL - g_fOceanCurrent * 0.03f;	// 慣性

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_NOMAL)
		{// 通常時の継続時間を過ぎた

			g_OceanCurrentsState = OCEANCURRENTSSTATE_WAIT;			// 渦潮待機状態に設定

			g_nCounterOceanCurrents = 0;							// カウンタを初期化
		}

		break;

	case OCEANCURRENTSSTATE_WAIT:		// 渦潮待機時

		// 回転量を渦潮待機時の移動量に修正
		g_fOceanCurrent += OCEANCURRECT_SPEED_WAIT - g_fOceanCurrent * 0.05f;	// 慣性

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_WAIT)
		{// 渦潮待機時の継続時間を過ぎた

			g_OceanCurrentsState = OCEANCURRENTSSTATE_WIRLPOOL;		// 渦潮状態に設定

			g_nCounterOceanCurrents = 0;							// カウンタを初期化
		}

		break;

	case OCEANCURRENTSSTATE_WIRLPOOL:	// 渦潮時

		// 回転量を渦潮時の移動量に修正
		g_fOceanCurrent += OCEANCURRECT_SPEED_WIRLPOOL - g_fOceanCurrent * 0.05f;	// 慣性

		if (g_nCounterOceanCurrents >= OCEANCURRECT_TIME_WIRLPOOL)
		{// 渦潮時の継続時間を過ぎた

			g_OceanCurrentsState = OCEANCURRENTSSTATE_NOMAL;		// 通常状態に設定

			g_nCounterOceanCurrents = 0;							// カウンタを初期化
		}

		break;
	}

	// 海流のポリゴンの更新
	for (int nCntOceanCurrents = 0; nCntOceanCurrents < MAX_SET_OCEANCURRENTS; nCntOceanCurrents++)
	{
		if (g_aOceanCurrents[nCntOceanCurrents].bUse == false)
		{// 使用されていない場合

			continue;	// 処理の始めに戻る
		}

		// 設定に必要な各値を求める
		SetPos = g_aPosOceanCurrents[0] + g_aOceanCurrents[nCntOceanCurrents].offPos;														// 設定する位置(pos)を求める
		
		fLength = sqrtf((g_aOceanCurrents[nCntOceanCurrents].fSizeWidth  * 2) * (g_aOceanCurrents[nCntOceanCurrents].fSizeWidth  * 2)		// 対角線の長さを求める
					  + (g_aOceanCurrents[nCntOceanCurrents].fSizeHeight * 2) * (g_aOceanCurrents[nCntOceanCurrents].fSizeHeight * 2))
					  * 0.5f;
		
		fAngle = atan2f(g_aOceanCurrents[nCntOceanCurrents].fSizeWidth * 2, g_aOceanCurrents[nCntOceanCurrents].fSizeHeight * 2);			// 対角線の角度を求める
		
		// ▼頂点バッファをロックして頂点情報へのポインタを所得
		g_pVtxBuffOceanCurrents->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nCntOceanCurrents * 4);	// 頂点をインデックス分進める

		// 頂点情報の設定
		pVtx[0].pos.x = SetPos.x - sinf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;					 
		pVtx[1].pos.x = SetPos.x + sinf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;
		pVtx[2].pos.x = SetPos.x - sinf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;
		pVtx[3].pos.x = SetPos.x + sinf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;
		
		pVtx[0].pos.y = SetPos.y - cosf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;					 
		pVtx[1].pos.y = SetPos.y - cosf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;
		pVtx[2].pos.y = SetPos.y + cosf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;
		pVtx[3].pos.y = SetPos.y + cosf(fAngle + g_aOceanCurrents[nCntOceanCurrents].rot.y) * fLength;

		pVtx[0].pos.z = 0.0f;
		pVtx[1].pos.z = 0.0f;
		pVtx[2].pos.z = 0.0f;
		pVtx[3].pos.z = 0.0f;

		// 頂点カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// (R, G, B, A)

		// ▲頂点バッファをアンロックする
		g_pVtxBuffOceanCurrents->Unlock();

		//PrintDebugProc("\nOC[%d]_POS	  %f %f %f", 0, g_aPosOceanCurrents[0].x, g_aPosOceanCurrents[0].y, g_aPosOceanCurrents[0].z);
		//PrintDebugProc("\nOC[%d]_OFFPOS %f %f %f", nCntOceanCurrents, g_aOceanCurrents[nCntOceanCurrents].offPos.x, g_aOceanCurrents[nCntOceanCurrents].offPos.y, g_aOceanCurrents[nCntOceanCurrents].offPos.z);
		//PrintDebugProc("\nOC[%d]_SIZE	  %f %f", nCntOceanCurrents, g_aOceanCurrents[nCntOceanCurrents].fSizeWidth, g_aOceanCurrents[nCntOceanCurrents].fSizeHeight);
	}

	PrintDebugProc("\n\nOC_CNT %d",g_nCounterOceanCurrents);
	PrintDebugProc("\nOC_STATE %d", g_OceanCurrentsState);
	PrintDebugProc("\nOC_SPEED %f", g_fOceanCurrent);

}

//========================================================================
// 海流の描画処理　
//========================================================================
void DrawOceanCurrents(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// =========================================================

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffOceanCurrents, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntOceanCurrents = 0; nCntOceanCurrents < MAX_SET_OCEANCURRENTS; nCntOceanCurrents++)
	{
		if (g_aOceanCurrents[nCntOceanCurrents].bUse == false || g_aOceanCurrents[nCntOceanCurrents].bDisp == false)
		{// 使用されていない,場合

			continue;	// 処理の始めに戻る
		}

		// テクスチャの設定
		if (g_aOceanCurrents[nCntOceanCurrents].nIdxTexture != -1)
		{// テクスチャが設定されている場合

			pDevice->SetTexture(0, g_apTextureOceanCurrents[g_aOceanCurrents[nCntOceanCurrents].nIdxTexture]);
		}
		else
		{// テクスチャが設定されていない場合

			pDevice->SetTexture(0, NULL);
		}

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
							   nCntOceanCurrents * 4,	// インデックス
							   2);						// プリミティブ（ポリゴン）数
	}
}

//========================================================================
// 海流の設定処理
//========================================================================
int SetOceanCurrents(int nIdxTex, D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight)
{
	for (int nCntOceanCurrents = 0; nCntOceanCurrents < MAX_SET_OCEANCURRENTS; nCntOceanCurrents++)
	{
		if (g_aOceanCurrents[nCntOceanCurrents].bUse == true)
		{// 使用されている場合

			continue;	// 処理の始めに戻る
		}

		// 海流のポリゴン情報を設定
		g_aOceanCurrents[nCntOceanCurrents].nIdxTexture = nIdxTex;	// テクスチャのインデックスを設定
		g_aOceanCurrents[nCntOceanCurrents].offPos = pos;			// オフセットの位置を設定
		g_aOceanCurrents[nCntOceanCurrents].rot = rot;				// 角度を設定
		g_aOceanCurrents[nCntOceanCurrents].fSizeWidth = fWidth;	// 大きさ(幅)を設定
		g_aOceanCurrents[nCntOceanCurrents].fSizeHeight = fHeight;	// 大きさ高さ)を設定
		g_aOceanCurrents[nCntOceanCurrents].bDisp = true;			// 表示している状態に設定
		g_aOceanCurrents[nCntOceanCurrents].bUse = true;			// 使用している状態に設定



		return nCntOceanCurrents;	// 設定した場所のインデックスを返す
	}
	 
	return -1;						// 設定していない値を返す
}

//=============================================================================
// 中心を軸にした海流の移動処理
//=============================================================================
void MoveOceanCurrents(D3DXVECTOR3* pPos)
{
	// 変数宣言 ===========================================

	float fDistRadius;	// 中心からの距離(半径)
	float fNomRadius;	// 正規化した距離(半径)
	float fNowAngle;	// 現在の角度

	MeshCylinder *pMeshCylinder = GetMeshCylinder();

	// ====================================================

	fDistRadius = sqrtf(pPos->x * pPos->x + pPos->z * pPos->z);		// 中心からの距離を求める
	fNomRadius = fDistRadius / pMeshCylinder[1].size.x;				// posが中心からどれだけ離れているかを求める
	fNowAngle = (float)atan2(pPos->x, pPos->z);						// 中心からの角度を求める

	// 角度を更新
	fNowAngle += g_fOceanCurrent / fNomRadius;		// 移動量(角度)を正規化した距離の長さにする

	// 角度が超えた場合、角度を範囲内に修正
	if		(fNowAngle < -D3DX_PI) fNowAngle = fNowAngle + D3DX_PI * 2;
	else if (fNowAngle >  D3DX_PI) fNowAngle = fNowAngle - D3DX_PI * 2;

	// 位置を設定
	pPos->x = sinf(fNowAngle) * fDistRadius;
	pPos->z = cosf(fNowAngle) * fDistRadius;
}

//========================================================================
// 海流の警告処理
//========================================================================
void SetWarningOceanCurrents(D3DXVECTOR3 pos, int nWaitTime)
{

}

//========================================================================
// 現在の海流の状態を返す
//========================================================================
OCEANCURRENTSSTATE GetOceanCurrents(void)
{
	return g_OceanCurrentsState;
}