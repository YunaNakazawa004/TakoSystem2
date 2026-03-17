//==================================================================================
// 
// 泡に関する処理[bubble.cpp]
// Author : Soki Inamura
// 
//==================================================================================
#include "main.h"

#include "bubble.h"

#include "watersurf.h"

#include "input.h"
#include "debugproc.h"
// マクロ定義 ==================================================

#define MAX_NUM_BUBBLE			(1280)	// 泡の総数
#define MAX_SET_BUBBLE			(64)	// 泡の設定出来る数

#define BUBBLE_RISE_SPEED		(1.0f)	// 泡の上昇速度

#define FILENAME_BUBBLETEXTURE			"data/TEXTURE/tex_alpha_bubble000.jpg"

#define BUBBLE_BUOYANCY			(0.5f)	// 泡の浮力

// 構造体の定義 ================================================

// 泡ポリゴンの情報
typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	float fSizeRadius;		// 大きさ
	
	//D3DXVECTOR3 vecMove;	// 移動方向
	//float fSpeed;			// 移動速度
	
	int nLife;				// 寿命

	D3DXMATRIX mtxWorld;	// ワールドマトリックス

	bool bUse;				// 使用状態

}BubblePolygon;

// 泡パーティクルの情報
typedef struct
{
	D3DXVECTOR3* pPos;		// 位置
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 offPos;		// オフセット位置

	int nLife;				// 寿命
	int nValue;				// 1回に設定する量

	int nWaitTime;			// 生成待機時間
	int nWaitCounter;		// 生成待機カウンタ

	int nSetLife;			// 設定する泡の寿命
	float fSetSizeMax;		// 設定する泡の大きさの最大
	float fSetSizeMin;		// 設定する泡の大きさの最小

	D3DXMATRIX mtxWorld;	// ワールドマトリックス

	bool bUse;				// 使用状態

}BubblePaticle;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_pTextureBubble = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffBubble = NULL;	// 頂点バッファ

BubblePolygon g_aBubblePolygon[MAX_NUM_BUBBLE];		// 泡ポリゴンの情報
BubblePaticle g_aBubblePaticle[MAX_SET_BUBBLE];		// 泡パーティクルの設定情報

int g_nNumBubblePolygon;							// 泡の総数

D3DXVECTOR3 g_posBubblePaticle;						// 泡パーティクルの頂点

//========================================================================
// 泡の入力処理
//========================================================================
void InitBubble(void)
{
	// 変数宣言 ======================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntBubble = 0;

	// ===============================================

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice,					// Direct3Dデバイスへのポインタ
										 FILENAME_BUBBLETEXTURE,	// 読み込むテクスチャ
										 &g_pTextureBubble)))		// テクスチャへのポインタ
	{// テクスチャの読み込みに失敗

		// 警告ウィンドウの作成
		MessageBox(NULL,"テクスチャの読み込みに失敗\nby bubble.cpp","LoadError", MB_OK);
	}

	// 泡ポリゴンの初期化
	for (nCntBubble = 0; nCntBubble < MAX_NUM_BUBBLE; nCntBubble++)
	{
		g_aBubblePolygon[nCntBubble].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		//g_aBubblePolygon[nCntBubble].vecMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 移動方向を初期化
		g_aBubblePolygon[nCntBubble].fSizeRadius = 0.0f;						// 大きさの半径を初期化
		//g_aBubblePolygon[nCntBubble].fSpeed = 0.0f;								// 移動速度を初期化
		g_aBubblePolygon[nCntBubble].nLife = 0;									// 寿命を初期化
		g_aBubblePolygon[nCntBubble].bUse = false;								// 使用していない状態に設定
	}

	// 泡パーティクルの初期化
	for (nCntBubble = 0; nCntBubble < MAX_SET_BUBBLE; nCntBubble++)
	{
		g_aBubblePaticle[nCntBubble].pPos = NULL;								// 位置を初期化
		g_aBubblePaticle[nCntBubble].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		g_aBubblePaticle[nCntBubble].nLife = 0;									// 寿命を初期化
		g_aBubblePaticle[nCntBubble].nValue = 0;								// 生成量を初期化
		g_aBubblePaticle[nCntBubble].nWaitTime = 0;								// 生成待機時間を初期化
		g_aBubblePaticle[nCntBubble].nWaitCounter = 0;							// 生成待機カウンタを初期化
		g_aBubblePaticle[nCntBubble].nSetLife = 0;								// 生成物の寿命を初期化
		g_aBubblePaticle[nCntBubble].fSetSizeMax = 0;							// 生成サイズの最大を初期化
		g_aBubblePaticle[nCntBubble].fSetSizeMin = 0;							// 生成サイズの最小を初期化
		g_aBubblePaticle[nCntBubble].bUse = false;								// 使用していない状態に設定
	}

	g_posBubblePaticle = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化

	g_nNumBubblePolygon = 0;

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4 * MAX_NUM_BUBBLE,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_3D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffBubble,
								NULL);

	VERTEX_3D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffBubble->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntBubble = 0; nCntBubble < MAX_NUM_BUBBLE; nCntBubble++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z)
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z) 
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z) 
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// (x, y, z) 

		// 法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (x, y, z)
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (x, y, z)
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (x, y, z)
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);		// (x, y, z)

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
	g_pVtxBuffBubble->Unlock();

	// 泡パーティクルの設定
	//SetBubbleParticle(&g_posBubblePaticle, true, -1, 10, 1, 50, 3.0f, 1.0f);
}

//========================================================================
// 泡の終了処理
//========================================================================
void UninitBubble(void)
{
	// テクスチャの破棄
	if (g_pTextureBubble != NULL)
	{
		g_pTextureBubble->Release();
		g_pTextureBubble = NULL;		// 中身を空にする
	}
	
	// 頂点バッファの破棄
	if (g_pVtxBuffBubble != NULL)
	{
		g_pVtxBuffBubble->Release();
		g_pVtxBuffBubble = NULL;		// 中身を空にする
	}
}

//========================================================================
// 泡の更新処理
//========================================================================
void UpdateBubble(void)
{
	// 変数宣言 ======================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();					// デバイスのポインタ

	VERTEX_3D* pVtx;											// 頂点情報へのポインタを宣言

	D3DXVECTOR3 tmpPosOld = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 過去の位置
	float fDistLength = 0.0f;									// 距離

	D3DXVECTOR3 setOffPos  = D3DXVECTOR3(0.0f, 0.0f, 0.0f),		// 始点からの離れ具合
				setVecMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 移動方向

	// 確率
	float fRandDistance = 0.0f;									// 中心から離す距離
	float fRandRadius = 0.0f;									// 半径
	float fRandRot = 0.0f;
	
	// ===============================================

	// デバッグテキスト
	PrintDebugProc("\nBUBBLE_POLYGON_NUM %d", g_nNumBubblePolygon);
	

	if(GetKeyboardPress(DIK_UP))	g_posBubblePaticle.z += 1.0f;
	if(GetKeyboardPress(DIK_DOWN))	g_posBubblePaticle.z -= 1.0f;
	if(GetKeyboardPress(DIK_LEFT))	g_posBubblePaticle.x -= 1.0f;
	if(GetKeyboardPress(DIK_RIGHT)) g_posBubblePaticle.x += 1.0f;

	if (GetKeyboardTrigger(DIK_7))g_aBubblePaticle[0].nValue++;
	if (GetKeyboardTrigger(DIK_6))g_aBubblePaticle[0].nValue--;

	// 泡パーティクルの更新
	for (int nCntParticle = 0; nCntParticle < MAX_SET_BUBBLE; nCntParticle++)
	{
		if (g_aBubblePaticle[nCntParticle].bUse == false)
		{// 使用していない場合

			continue;	// 処理の始めに戻る
		}

		// 前回の位置を保存
		tmpPosOld = g_aBubblePaticle[nCntParticle].pos;
		
		// パーティクルの位置を更新
		if (g_aBubblePaticle[nCntParticle].pPos)
		{// ポインタの値がある場合
			
			g_aBubblePaticle[nCntParticle].pos = *g_aBubblePaticle[nCntParticle].pPos;	// パーティクルの位置をポインタの位置に更新
		} 

		// 設定待機時間を減らす
		g_aBubblePaticle[nCntParticle].nWaitCounter--;

		if(g_aBubblePaticle[nCntParticle].nWaitCounter <= 0)
		{// 設定待機時間が0になった場合
	
			// エフェクトの設定
			for (int nCntSet = 0; nCntSet < g_aBubblePaticle[nCntParticle].nValue; nCntSet++)
			{// 生成数だけ繰り返す

				// 設定する各値の乱数を求める
				fRandRot		  = (float)(rand() % 629 - 314) / 100.0f;										// 中心から離す角度を求める
				fRandDistance = (float)(rand() % (int)(1.0 * 100.0f) + (int)(0.1f * 100.0f)) / 100.0f;		// 中心から離す距離を求める 
				
				fRandRadius   = (float)(rand() % (int)(g_aBubblePaticle[nCntParticle].fSetSizeMax * 10.0f)	// 泡の大きさを求める
											   + (int)(g_aBubblePaticle[nCntParticle].fSetSizeMin * 10.0f)) 
											   / 10.0f;
				
				//fDistLength = sqrtf((g_aBubblePaticle[nCntParticle].pos.x - tmpPosOld.x) * g_aBubblePaticle[nCntParticle].pos.z - tmpPosOld.z);

				// 位置の設定
				setOffPos.x = sinf(fRandRot) * fRandDistance;
				setOffPos.z = cosf(fRandRot) * fRandDistance;
				
				// 泡ポリゴンの設定
				SetBubblePolygon(g_aBubblePaticle[nCntParticle].pos + setOffPos,	// 位置
								 fRandRadius,										// 半径
								 g_aBubblePaticle[nCntParticle].nSetLife);			// 寿命
			}
						 
			// 待機時間を再設定
			g_aBubblePaticle[nCntParticle].nWaitCounter = g_aBubblePaticle[nCntParticle].nWaitTime;
		}

		// パーティクル寿命の更新
		if (g_aBubblePaticle[nCntParticle].nLife > 0)
		{// 寿命が0より大きい

			g_aBubblePaticle[nCntParticle].nLife--;	// 寿命をデクリメント

			if (g_aBubblePaticle[nCntParticle].nLife == 0)
			{// 寿命が0になった

				g_aBubblePaticle[nCntParticle].bUse = false;	// 使用していない状態に設定
			}
		}
	}

	// 泡ポリゴンの更新
	for (int nCntPolygon = 0; nCntPolygon < MAX_NUM_BUBBLE; nCntPolygon++)
	{
		if (g_aBubblePolygon[nCntPolygon].bUse == false)
		{// 使用していない場合

			continue;	// 処理の始めに戻る
		}

		// 位置を更新
		//g_aBubblePolygon[nCntPolygon].pos += g_aBubblePolygon[nCntPolygon].vecMove * g_aBubblePolygon[nCntPolygon].fSpeed;	// 移動方向に角度

		// 浮力を加える
		g_aBubblePolygon[nCntPolygon].pos.y += BUBBLE_BUOYANCY + g_aBubblePolygon[nCntPolygon].fSizeRadius * 0.3f;

		if (g_aBubblePolygon[nCntPolygon].pos.y > *GetWaterSurf_Height())
		{// 水面より上に出た

			g_aBubblePolygon[nCntPolygon].bUse = false;	// 使用していない状態に設定

			g_nNumBubblePolygon--;						// 泡ポリゴンの総数をデクリメント
		}
		else
		{// 水面より上に出てない

			// エフェクト寿命の更新
			if (g_aBubblePolygon[nCntPolygon].nLife > 0)
			{// 寿命が0より大きい

				g_aBubblePolygon[nCntPolygon].nLife--;	// 寿命をデクリメント

				if (g_aBubblePolygon[nCntPolygon].nLife == 0)
				{// 寿命が0になった

					g_aBubblePolygon[nCntPolygon].bUse = false;	// 使用していない状態に設定

					g_nNumBubblePolygon--;						// 泡ポリゴンの総数をデクリメント
				}
			}

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffBubble->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntPolygon * 4);	// 頂点バッファのポインタをインデックスの先頭まで進める

			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-g_aBubblePolygon[nCntPolygon].fSizeRadius, +g_aBubblePolygon[nCntPolygon].fSizeRadius, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(+g_aBubblePolygon[nCntPolygon].fSizeRadius, +g_aBubblePolygon[nCntPolygon].fSizeRadius, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aBubblePolygon[nCntPolygon].fSizeRadius, -g_aBubblePolygon[nCntPolygon].fSizeRadius, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(+g_aBubblePolygon[nCntPolygon].fSizeRadius, -g_aBubblePolygon[nCntPolygon].fSizeRadius, 0.0f);

			// ▲頂点バッファをアンロックする
			g_pVtxBuffBubble->Unlock();
		}
	}
}

//========================================================================
// 泡の描画処理
//========================================================================
void DrawBubble(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxTrans;	// 計算用マトリックス
	D3DXMATRIX mtxView;		// ビューマトリックス所得用
	
	// =========================================================

#if 1
	// ライティングの設定
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);				// ライティングをオフに設定

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);			// Zバッファ更新の無効の設定

	//アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファブレンドを有効に設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// ( , 比較方法(より大きい))
	pDevice->SetRenderState(D3DRS_ALPHAREF, 100);				// ( , 基準値)
	
	// 減算合成の設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3
#endif

	for (int nCntBubble = 0; nCntBubble < MAX_NUM_BUBBLE; nCntBubble++)
	{// 泡ポリゴンの数だけ繰り返す

		if (g_aBubblePolygon[nCntBubble].bUse == false)
		{// 使用していない場合

			continue;	// 処理の始めに戻る
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aBubblePolygon[nCntBubble].mtxWorld);	// ワールドマトリックスの初期値を設定

		// ビューマトリックスの所得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		// ポリゴンの向きをカメラに対して正面に向ける
		D3DXMatrixInverse(&g_aBubblePolygon[nCntBubble].mtxWorld, NULL, &mtxView);

		// マトリックスの位置を修正
		g_aBubblePolygon[nCntBubble].mtxWorld._41 = 0.0f;
		g_aBubblePolygon[nCntBubble].mtxWorld._42 = 0.0f;
		g_aBubblePolygon[nCntBubble].mtxWorld._43 = 0.0f;

		// 位置を設定
		D3DXMatrixTranslation(&mtxTrans,
							  g_aBubblePolygon[nCntBubble].pos.x, g_aBubblePolygon[nCntBubble].pos.y, g_aBubblePolygon[nCntBubble].pos.z);

		D3DXMatrixMultiply(&g_aBubblePolygon[nCntBubble].mtxWorld, &g_aBubblePolygon[nCntBubble].mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aBubblePolygon[nCntBubble].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffBubble, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureBubble);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,	// プリミティブの種類
							   nCntBubble * 4,		// インデックス数
							   2);					// プリミティブ（ポリゴン）数
	}

#if 1
	// ライティングの設定
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);					// ライティングをオンに設定

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファ更新の有効の設定

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);			// アルファブレンドを無効に設定
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);		// ( , 比較方法(すべて))
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);						// ( , 基準値)
	
	// 減算合成を元に戻す 
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
#endif
}

//========================================================================
// 泡パーティクルの設定処理
//========================================================================
int SetBubbleParticle(D3DXVECTOR3* pPos, bool bRefPointer, int nLife, int nWait, int nValue, int nSetLife, float fSetSizeMax, float fSetSizeMin)
{
	if (pPos == NULL)
	{// 位置ポインタがない場合

		return -1;	// 設定する値がない為設定しない
	}

	for (int nCntParticle = 0; nCntParticle < MAX_NUM_BUBBLE; nCntParticle++)
	{
		if (g_aBubblePaticle[nCntParticle].bUse == true)
		{// 使用している場合

			continue;	// 処理の始めに戻る
		}

		// 泡パーティクルの情報を設定
		g_aBubblePaticle[nCntParticle].pPos = (bRefPointer == true) ? pPos : NULL;	// 位置ポインタを参照すら場合、ポインタを設定
		g_aBubblePaticle[nCntParticle].pos = *pPos;									// 位置を設定
		g_aBubblePaticle[nCntParticle].nLife = nLife;								// 寿命を設定
		g_aBubblePaticle[nCntParticle].nValue = nValue;								// 1回の生成数を設定
		g_aBubblePaticle[nCntParticle].nWaitTime = nWait;							// 生成待機時間を設定
		g_aBubblePaticle[nCntParticle].nWaitCounter = 0;							// 生成待機カウンタを初期化
		g_aBubblePaticle[nCntParticle].nSetLife = nSetLife;							// 生成ポリゴンの寿命を設定
		g_aBubblePaticle[nCntParticle].fSetSizeMax = fSetSizeMax;					// 生成サイズの最大を設定
		g_aBubblePaticle[nCntParticle].fSetSizeMin = fSetSizeMin;					// 生成サイズの最小を設定
		g_aBubblePaticle[nCntParticle].bUse = true;									// 使用している状態に設定

		return nCntParticle;	// 設定したインデックスを返す
	}

	return -1;					// 設定したインデックスが無い事を返す
}

//========================================================================
// 泡ポリゴンの設定処理
//========================================================================
void SetBubblePolygon(D3DXVECTOR3 pos, float fSizeRadius, int nLife)
{
	for (int nCntPolygon = 0; nCntPolygon < MAX_NUM_BUBBLE; nCntPolygon++)
	{
		if (g_aBubblePolygon[nCntPolygon].bUse == true)
		{// 使用している場合

			continue;	// 処理の始めに戻る
		}

		// 泡ポリゴンの情報を設定
		g_aBubblePolygon[nCntPolygon].pos = pos;					// 位置を設定
		g_aBubblePolygon[nCntPolygon].fSizeRadius = fSizeRadius;	// 大きさを設定
		//g_aBubblePolygon[nCntPolygon].vecMove = move;				// 
		//g_aBubblePolygon[nCntPolygon].fSpeed = fSpeed;			// 
		g_aBubblePolygon[nCntPolygon].nLife = nLife;				// 寿命を設定
		g_aBubblePolygon[nCntPolygon].bUse = true;					// 使用している状態に設定

		g_nNumBubblePolygon++;	// 泡ポリゴンの総数をインクリメント

		break;	// for文を抜ける
	}
}

//========================================================================
// 泡パーティクルの削除処理
//========================================================================
void DelBubbleParticle(int nIdxParticle)
{
	if (g_aBubblePaticle[nIdxParticle].bUse == true)
	{// 使用している場合

		g_aBubblePaticle[nIdxParticle].bUse = false;	// 使用していない状態に設定
	}
}