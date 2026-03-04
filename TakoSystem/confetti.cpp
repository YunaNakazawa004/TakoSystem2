//========================================================================
// 
// 紙吹雪処理[confetti.cpp]
// Author: Mutsuki Uemura
// 
//========================================================================
#include "main.h"

#include "confetti.h"	// 紙吹雪ヘッダー

#include "input.h"

// マクロ定義 ==================================================

#define MAX_SET_CONFETTI		(10000)							// エフェクトの最大数

#define CONFETTI_FILENAME		"data\\TEXTURE\\shadow000.png"	// 使用するエフェクトのファイル名

#define EFFECT_INERTIA			(0.017f)						// 慣性

// 構造体の定義 ================================================

// 紙吹雪の情報
typedef struct
{
	// 頂点情報
	D3DXVECTOR3 pos;		// 位置

	D3DXVECTOR3 rot;		// 角度
	D3DXVECTOR3 rotMove;	// 角度

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

}Confetti;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_pTextureConfetti = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffConfetti = NULL;		// 頂点バッファ

Confetti g_aConfetti[MAX_SET_CONFETTI];					// エフェクトの情報

bool g_bDispConfetti = true;							// 紙吹雪の表示状態

//======================================================================== 
// 紙吹雪の初期化処理
//========================================================================
void InitConfetti(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntConfetti;	// カウンタ

	// ====================================================

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,					// Direct3Dデバイスへのポインタ
		CONFETTI_FILENAME,		// 読み込むテクスチャ
		&g_pTextureConfetti);		// テクスチャへのポインタ

// エフェクト情報の初期化
	for (nCntConfetti = 0; nCntConfetti < MAX_SET_CONFETTI; nCntConfetti++)
	{
		g_aConfetti[nCntConfetti].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化

		g_aConfetti[nCntConfetti].rot = D3DXVECTOR3(0.0f, 1.0f, 0.0f);		// 角度を初期化

		g_aConfetti[nCntConfetti].vecMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動方向を初期化
		g_aConfetti[nCntConfetti].fSpeed = 0.0f;								// 移動速度を初期化

		g_aConfetti[nCntConfetti].fRadius = 0.0f;								// 半径を初期化
		g_aConfetti[nCntConfetti].faddRadius = 0.0f;							// 半径の加算値を初期化

		g_aConfetti[nCntConfetti].nLife = 0;									// 寿命を初期化

		g_aConfetti[nCntConfetti].bUse = false;								// 使用していない状態に設定
	}

	g_bDispConfetti = true;	// 表示状態に設定

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_SET_CONFETTI,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffConfetti,
		NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffConfetti->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntConfetti = 0; nCntConfetti < MAX_SET_CONFETTI; nCntConfetti++)
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
	g_pVtxBuffConfetti->Unlock();
}

//======================================================================== 
// 紙吹雪の終了処理
//========================================================================
void UninitConfetti(void)
{
	// テクスチャの破棄
	if (g_pTextureConfetti != NULL)
	{
		g_pTextureConfetti->Release();
		g_pTextureConfetti = NULL;		// 中身を空にする
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffConfetti != NULL)
	{
		g_pVtxBuffConfetti->Release();
		g_pVtxBuffConfetti = NULL;		// 中身を空にする
	}
}

//======================================================================== 
// 紙吹雪の更新処理
//========================================================================
void UpdateConfetti(void)
{
	// 変数宣言 ================================================

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言

	int nCntConfetti;		// カウンタ

	// =========================================================

	for (nCntConfetti = 0; nCntConfetti < MAX_SET_CONFETTI; nCntConfetti++)
	{
		if (g_aConfetti[nCntConfetti].bUse == true)
		{// 使用されている場合

			// 位置の更新
			g_aConfetti[nCntConfetti].pos += g_aConfetti[nCntConfetti].vecMove * g_aConfetti[nCntConfetti].fSpeed;

			g_aConfetti[nCntConfetti].rot += g_aConfetti[nCntConfetti].rotMove * 0.03f;

			// 慣性
			g_aConfetti[nCntConfetti].fSpeed ;

			// 半径の大きさを更新
			g_aConfetti[nCntConfetti].fRadius += g_aConfetti[nCntConfetti].faddRadius;

			// 寿命を更新
			g_aConfetti[nCntConfetti].nLife--;

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffConfetti->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntConfetti * 4);

			pVtx[0].pos.x = -g_aConfetti[nCntConfetti].fRadius;
			pVtx[1].pos.x = g_aConfetti[nCntConfetti].fRadius;
			pVtx[2].pos.x = -g_aConfetti[nCntConfetti].fRadius;
			pVtx[3].pos.x = g_aConfetti[nCntConfetti].fRadius;

			pVtx[0].pos.y = g_aConfetti[nCntConfetti].fRadius;
			pVtx[1].pos.y = g_aConfetti[nCntConfetti].fRadius;
			pVtx[2].pos.y = -g_aConfetti[nCntConfetti].fRadius;
			pVtx[3].pos.y = -g_aConfetti[nCntConfetti].fRadius;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// ▲頂点バッファをアンロックする
			g_pVtxBuffConfetti->Unlock();

			if (g_aConfetti[nCntConfetti].nLife < 0)
			{// 寿命が尽きた場合

				g_aConfetti[nCntConfetti].bUse = false;	// 使用してない状態に設定
			}
		}
	}
}

//======================================================================== 
// 紙吹雪の描画処理
//========================================================================
void DrawConfetti(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ===============================================

	int nCntConfetti;

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス所得用

	// =========================================================

	if (g_bDispConfetti == false)
	{// 表示しない状態の場合

		return;	// 描画を行わない
	}

	// 
	// ライティングを無効化（影の影響を受けない）
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for (nCntConfetti = 0; nCntConfetti < MAX_SET_CONFETTI; nCntConfetti++)
	{
		if (g_aConfetti[nCntConfetti].bUse == true)
		{// 使用されている場合

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aConfetti[nCntConfetti].mtxWorld);			// ワールドマトリックスの初期値を設定

			// ビューマトリックスの所得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			// 角度を設定
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
				g_aConfetti[nCntConfetti].rot.x, g_aConfetti[nCntConfetti].rot.y, g_aConfetti[nCntConfetti].rot.z);

			// 位置を設定
			D3DXMatrixTranslation(&mtxTrans,
				g_aConfetti[nCntConfetti].pos.x, g_aConfetti[nCntConfetti].pos.y, g_aConfetti[nCntConfetti].pos.z);

			D3DXMatrixMultiply(&g_aConfetti[nCntConfetti].mtxWorld, &mtxRot, &mtxTrans);


			// Zテストを無効にする
			pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Zバッファ更新の無効の設定

			//アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファブレンドを有効に設定
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ( , 比較方法(より大きい))
			pDevice->SetRenderState(D3DRS_ALPHAREF, 100);				// ( , 基準値)

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aConfetti[nCntConfetti].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_pVtxBuffConfetti, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureConfetti);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	// プリミティブの種類
				nCntConfetti * 4,		// インデックス数
				2);					// プリミティブ（ポリゴン）数

			// Zテストを有効にする
			pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ更新の有効の設定

			// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);			// アルファブレンドを無効に設定
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);		// ( , 比較方法(すべて))
			pDevice->SetRenderState(D3DRS_ALPHAREF, 0);						// ( , 基準値)

		}
	}
	// 元に戻す
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

//======================================================================== 
// 紙吹雪の設定処理
//========================================================================
void SetConfetti(int nLife,				// 寿命
	D3DXVECTOR3 pos,					// 表示位置
	D3DXVECTOR3 rot,					// 角度
	D3DXVECTOR3 vecMove, float fSpeed,	// 移動方向, 移動速度
	float fRadius, float faddRadius,	// 表示サイズ, 加算サイズ
	D3DXCOLOR col)						// 色
{
	// 変数宣言 ===========================================

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言
	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// ====================================================

	for (int nCntConfetti = 0; nCntConfetti < MAX_SET_CONFETTI; nCntConfetti++)
	{
		if (g_aConfetti[nCntConfetti].bUse == false)
		{// 使用されていない場合

			g_aConfetti[nCntConfetti].pos = pos;				// 位置を設定

			g_aConfetti[nCntConfetti].rot = { 0.0f,0.0f,0.0f };

			g_aConfetti[nCntConfetti].rotMove = rot;			// 角度を設定

			g_aConfetti[nCntConfetti].vecMove = vecMove;		// 移動方向を設定
			g_aConfetti[nCntConfetti].fSpeed = fSpeed;			// 移動速度を設定

			g_aConfetti[nCntConfetti].fRadius = fRadius;			// 半径の大きさを設定
			g_aConfetti[nCntConfetti].faddRadius = faddRadius;	// 半径の加算値を設定

			g_aConfetti[nCntConfetti].nLife = nLife;				// 寿命を設定

			g_aConfetti[nCntConfetti].bUse = true;				// 使用している状態に設定

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffConfetti->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntConfetti * 4);	// 頂点バッファを進める

			// 頂点座標の設定
			pVtx[0].pos.x = -g_aConfetti[nCntConfetti].fRadius;
			pVtx[1].pos.x = +g_aConfetti[nCntConfetti].fRadius;
			pVtx[2].pos.x = -g_aConfetti[nCntConfetti].fRadius;
			pVtx[3].pos.x = +g_aConfetti[nCntConfetti].fRadius;

			pVtx[0].pos.y = +g_aConfetti[nCntConfetti].fRadius;
			pVtx[1].pos.y = +g_aConfetti[nCntConfetti].fRadius;
			pVtx[2].pos.y = -g_aConfetti[nCntConfetti].fRadius;
			pVtx[3].pos.y = -g_aConfetti[nCntConfetti].fRadius;

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
			g_pVtxBuffConfetti->Unlock();

			break;	// for文を抜ける
		}
	}
}