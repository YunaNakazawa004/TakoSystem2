//=============================================================================
// 
// 3Dエフェクトに関する処理[effect_3d.cpp]
// Author:稲村 蒼稀
// 
//=============================================================================
#include "main.h"

// エフェクト
#include "effect_3d.h"	// エフェクトヘッダー

#include "input.h"

// マクロ定義 ==================================================

#define MAX_SET_EFFECT3D		(20000)							// エフェクトの最大数

#define EFFECT3D_FILENAME		"data\\TEXTURE\\shadow000.png"	// 使用するエフェクトのファイル名

#define EFFECT_INERTIA			(0.017f)						// 慣性

// 構造体の定義 ================================================

// 3Dエフェクトの情報
typedef struct
{
	// 頂点情報
	D3DXVECTOR3 pos;		// 位置

	D3DXVECTOR3 vecMove;	// 移動方向(ベクトル)
	float fSpeed;			// 移動速度

	float fRadius;			// 半径
	float faddRadius;		// 半径の加算値

	// マトリックス
	D3DXMATRIX mtxWorld;	// ワールドマトリックス

	// ステータス
	int nLife;				// 寿命

	// 状態
	bool bUse;				// 使用状況

	EFFECTTYPE effecttype;	// エフェクトの用途

}Effect3D;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_pTextureEffect3D = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffEffect3D = NULL;		// 頂点バッファ

Effect3D g_aEffect3D[MAX_SET_EFFECT3D];					// エフェクトの情報

bool g_bDispEffect3D = true;							// 3Dエフェクトの表示状態

//======================================================================== 
// 3Dエフェクトの初期化処理
//========================================================================
void InitEffect3D(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntEffect;	// カウンタ

	// ====================================================

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// Direct3Dデバイスへのポインタ
		EFFECT3D_FILENAME,		// 読み込むテクスチャ
		&g_pTextureEffect3D);		// テクスチャへのポインタ

// エフェクト情報の初期化
	for (nCntEffect = 0; nCntEffect < MAX_SET_EFFECT3D; nCntEffect++)
	{
		g_aEffect3D[nCntEffect].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化

		g_aEffect3D[nCntEffect].vecMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動方向を初期化
		g_aEffect3D[nCntEffect].fSpeed = 0.0f;								// 移動速度を初期化

		g_aEffect3D[nCntEffect].fRadius = 0.0f;								// 半径を初期化
		g_aEffect3D[nCntEffect].faddRadius = 0.0f;							// 半径の加算値を初期化

		g_aEffect3D[nCntEffect].nLife = 0;									// 寿命を初期化

		g_aEffect3D[nCntEffect].bUse = false;								// 使用していない状態に設定

		g_aEffect3D[nCntEffect].effecttype = EFFECTTYPE_NORMAL;				// エフェクトタイプを初期化
	}

	g_bDispEffect3D = true;	// 表示状態に設定

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_SET_EFFECT3D,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffEffect3D,
		NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffEffect3D->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntEffect = 0; nCntEffect < MAX_SET_EFFECT3D; nCntEffect++)
	{// 用意した数、繰り返し設定

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (X,Y,Z)	
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (X,Y,Z)	
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (X,Y,Z)	
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (X,Y,Z)	

		// 法線ベクトル(向き)の設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (X,Y,Z)										
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (X,Y,Z)									
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (X,Y,Z)									
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (X,Y,Z)									

		// 頂点カラーの設定
		pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);	// (R, G, B, A)
		pVtx[1].col = D3DCOLOR_RGBA(255, 255, 255, 255);	// (R, G, B, A)
		pVtx[2].col = D3DCOLOR_RGBA(255, 255, 255, 255);	// (R, G, B, A)
		pVtx[3].col = D3DCOLOR_RGBA(255, 255, 255, 255);	// (R, G, B, A)

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);				// (U, V)
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);				// (U, V)
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);				// (U, V)
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);				// (U, V)

		pVtx += 4;	// 頂点バッファを進める
	}

	// ▲頂点バッファをアンロックする
	g_pVtxBuffEffect3D->Unlock();
}

//======================================================================== 
// 3Dエフェクトの終了処理
//========================================================================
void UninitEffect3D(void)
{
	// テクスチャの破棄
	if (g_pTextureEffect3D != NULL)
	{
		g_pTextureEffect3D->Release();
		g_pTextureEffect3D = NULL;		// 中身を空にする
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffEffect3D != NULL)
	{
		g_pVtxBuffEffect3D->Release();
		g_pVtxBuffEffect3D = NULL;		// 中身を空にする
	}
}

//======================================================================== 
// 3Dエフェクトの更新処理
//========================================================================
void UpdateEffect3D(void)
{
	// 変数宣言 ================================================

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言

	int nCntEffect;		// カウンタ

	// =========================================================

	// 3Dエフェクトの表示切り替え
	if (GetKeyboardTrigger(DIK_F2) || GetJoypadTrigger(0,JOYKEY_LEFT_SHOULDER))
	{
		g_bDispEffect3D = (g_bDispEffect3D == true) ? false : true;
	}

	for (nCntEffect = 0; nCntEffect < MAX_SET_EFFECT3D; nCntEffect++)
	{
		if (g_aEffect3D[nCntEffect].bUse == true)
		{// 使用されている場合

			// 位置の更新
			g_aEffect3D[nCntEffect].pos += g_aEffect3D[nCntEffect].vecMove * g_aEffect3D[nCntEffect].fSpeed;

			// 慣性
			g_aEffect3D[nCntEffect].fSpeed += (0.0f - g_aEffect3D[nCntEffect].fSpeed) * EFFECT_INERTIA;

			// 半径の大きさを更新
			g_aEffect3D[nCntEffect].fRadius += g_aEffect3D[nCntEffect].faddRadius;

			// 寿命を更新
			g_aEffect3D[nCntEffect].nLife--;

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffEffect3D->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntEffect * 4);

			pVtx[0].pos.x = -g_aEffect3D[nCntEffect].fRadius;
			pVtx[1].pos.x = g_aEffect3D[nCntEffect].fRadius;
			pVtx[2].pos.x = -g_aEffect3D[nCntEffect].fRadius;
			pVtx[3].pos.x = g_aEffect3D[nCntEffect].fRadius;

			pVtx[0].pos.y = g_aEffect3D[nCntEffect].fRadius;
			pVtx[1].pos.y = g_aEffect3D[nCntEffect].fRadius;
			pVtx[2].pos.y = -g_aEffect3D[nCntEffect].fRadius;
			pVtx[3].pos.y = -g_aEffect3D[nCntEffect].fRadius;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// ▲頂点バッファをアンロックする
			g_pVtxBuffEffect3D->Unlock();

			if (g_aEffect3D[nCntEffect].nLife < 0)
			{// 寿命が尽きた場合

				g_aEffect3D[nCntEffect].bUse = false;	// 使用してない状態に設定
			}
		}
	}
}

//======================================================================== 
// 3Dエフェクトの描画処理
//========================================================================
void DrawEffect3D(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ===============================================

	int nCntEffect;

	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス所得用

	// =========================================================

	if (g_bDispEffect3D == false)
	{// 表示しない状態の場合

		return;	// 描画を行わない
	}

	// ライティングを無効化（影の影響を受けない）
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	for (nCntEffect = 0; nCntEffect < MAX_SET_EFFECT3D; nCntEffect++)
	{
		if (g_aEffect3D[nCntEffect].bUse == true)
		{// 使用されている場合

			if (g_aEffect3D[nCntEffect].effecttype != EFFECTTYPE_OCTOINK)
			{
				// Zテストを無効にする
				pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
				pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Zバッファ更新の無効の設定

				//アルファテストを有効にする
				pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファブレンドを有効に設定
				pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ( , 比較方法(より大きい))
				pDevice->SetRenderState(D3DRS_ALPHAREF, 100);				// ( , 基準値)

				// 減算合成の設定
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3
			}
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aEffect3D[nCntEffect].mtxWorld);			// ワールドマトリックスの初期値を設定

			// ビューマトリックスの所得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// ポリゴンの向きをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aEffect3D[nCntEffect].mtxWorld, NULL, &mtxView);

			g_aEffect3D[nCntEffect].mtxWorld._41 = 0.0f;
			g_aEffect3D[nCntEffect].mtxWorld._42 = 0.0f;
			g_aEffect3D[nCntEffect].mtxWorld._43 = 0.0f;

			// 位置を設定
			D3DXMatrixTranslation(&mtxTrans,
				g_aEffect3D[nCntEffect].pos.x, g_aEffect3D[nCntEffect].pos.y, g_aEffect3D[nCntEffect].pos.z);

			D3DXMatrixMultiply(&g_aEffect3D[nCntEffect].mtxWorld, &g_aEffect3D[nCntEffect].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aEffect3D[nCntEffect].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffEffect3D, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureEffect3D);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	// プリミティブの種類
				nCntEffect * 4,		// インデックス数
				2);					// プリミティブ（ポリゴン）数


			// Zテストを有効にする
			pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ更新の有効の設定

			// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);			// アルファブレンドを無効に設定
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);		// ( , 比較方法(すべて))
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);						// ( , 基準値)

			// ブレンディング(減算合成)を元に戻す 
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
		}
	}
	// ライティングを元に戻す
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ更新の有効の設定

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);			// アルファブレンドを無効に設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);		// ( , 比較方法(すべて))
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);						// ( , 基準値)

	// ブレンディング(減算合成)を元に戻す 
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
}

//======================================================================== 
// 3Dエフェクトの設定処理
//========================================================================
void SetEffect3D(int nLife,							// 寿命
	D3DXVECTOR3 pos,						// 表示位置
	D3DXVECTOR3 vecMove, float fSpeed,	// 移動方向, 移動速度
	float fRadius, float faddRadius,		// 表示サイズ, 加算サイズ
	D3DXCOLOR col,						// 色
	EFFECTTYPE type)						// エフェクトのタイプ
{
	// 変数宣言 ===========================================

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言

	// ====================================================

	for (int nCntEffect = 0; nCntEffect < MAX_SET_EFFECT3D; nCntEffect++)
	{
		if (g_aEffect3D[nCntEffect].bUse == false)
		{// 使用されていない場合

			g_aEffect3D[nCntEffect].pos = pos;					// 位置を設定

			g_aEffect3D[nCntEffect].vecMove = vecMove;			// 移動方向を設定
			g_aEffect3D[nCntEffect].fSpeed = fSpeed;			// 移動速度を設定

			g_aEffect3D[nCntEffect].fRadius = fRadius;			// 半径の大きさを設定
			g_aEffect3D[nCntEffect].faddRadius = faddRadius;	// 半径の加算値を設定

			g_aEffect3D[nCntEffect].nLife = nLife;				// 寿命を設定

			g_aEffect3D[nCntEffect].bUse = true;				// 使用している状態に設定

			g_aEffect3D[nCntEffect].effecttype = type;			// エフェクトタイプを設定

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffEffect3D->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntEffect * 4);	// 頂点バッファを進める

			// 頂点座標の設定
			pVtx[0].pos.x = -g_aEffect3D[nCntEffect].fRadius;
			pVtx[1].pos.x = +g_aEffect3D[nCntEffect].fRadius;
			pVtx[2].pos.x = -g_aEffect3D[nCntEffect].fRadius;
			pVtx[3].pos.x = +g_aEffect3D[nCntEffect].fRadius;

			pVtx[0].pos.y = +g_aEffect3D[nCntEffect].fRadius;
			pVtx[1].pos.y = +g_aEffect3D[nCntEffect].fRadius;
			pVtx[2].pos.y = -g_aEffect3D[nCntEffect].fRadius;
			pVtx[3].pos.y = -g_aEffect3D[nCntEffect].fRadius;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// 頂点カラーの設定
			pVtx[0].col = col;
			pVtx[1].col = col;
			pVtx[2].col = col;
			pVtx[3].col = col;

			// ▲頂点バッファをアンロックする
			g_pVtxBuffEffect3D->Unlock();

			break;	// for文を抜ける
		}
	}
}