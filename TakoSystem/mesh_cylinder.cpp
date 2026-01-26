//========================================================================
// 
// メッシュシリンダー(円筒)に関する処理[mesh_clinder.cpp]
// Author : Soki Inamura
// 
//========================================================================
#include "main.h"			// メインヘッダー

// オブジェ
#include "mesh_cylinder.h"	// メッシュシリンダーヘッダー

// 入力
#include "input.h"			// 入力ヘッダー

// マクロ定義 ==================================================

#define MAX_SET_MESHCYLINDER			(4)								// メッシュシリンダーの最大値
	
#define FILENAME_TEX_MESHCYLINDER		"data\\TEXTURE\\ski000.jpg"		// メッシュシリンダーのテクスチャファイル名

#define CREATE_VERTEX(w,d)		((w + 1) * (d + 1))					// メッシュの頂点数((幅の分割時の頂点数) * (奥行きの分割時の頂点数))
#define CREATE_SETVERTEX(w,d)	(2 * (w + 1) * d + ((d - 1) * 2))	// メッシュの設定する頂点数((2頂点*幅の分割数) * 奥行きの分割数 + (ずらした時の縮退ポリゴンの頂点数))
#define CREATE_SETPOLYGON(w,d)	(w * d * 2 + (d - 1) * 4)			// メッシュの設定するポリゴン数((分割した時の三角ポリゴン数) + (縮退ポリゴンの数))


// 構造体の定義 ================================================

// メッシュシリンダーの情報
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;	// 頂点バッファのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;	// インデックスバッファのポインタ
	
	D3DXVECTOR3 pos;					// 位置
	D3DXVECTOR3 rot;					// 角度

	float fSizeRadius;					// 大きさ(半径)
	float fSizeHeight;					// 大きさ(高さ)

	WORD wBlockWidth;					// 分割数(幅)
	WORD wBlockHeight;					// 分割数(高さ)

	D3DXMATRIX mtxWorld;				// ワールドマトリックス

	bool bReverse;						// 面の向き(true:外向き, false:内向き)

	bool bUse;							// 使用状態

}Mesh_Cylinder;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_pTextureMesh_Cylinder = NULL;		// テクスチャへのポインタ

Mesh_Cylinder g_aMesh_Cylinder[MAX_SET_MESHCYLINDER];	// メッシュシリンダーの情報

//======================================================================== 
// メッシュシリンダーの初期化処理
//========================================================================
void InitMesh_Cylinder(void)
{
	// 変数宣言 ===========================================
	
	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntMeshCylinder;	// カウンタ

	// ====================================================

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,						// Direct3Dデバイスへのポインタ
							  FILENAME_TEX_MESHCYLINDER,	// 読み込むテクスチャ
							  &g_pTextureMesh_Cylinder);		// テクスチャへのポインタ

	// メッシュシリンダーの情報を初期化
	for (nCntMeshCylinder = 0; nCntMeshCylinder < MAX_SET_MESHCYLINDER; nCntMeshCylinder++)
	{
		g_aMesh_Cylinder[nCntMeshCylinder].pVtxBuff = NULL;						// 頂点バッファへのポインタを初期化
		g_aMesh_Cylinder[nCntMeshCylinder].pIdxBuff = NULL;						// インデックスバッファへのポインタを初期化

		g_aMesh_Cylinder[nCntMeshCylinder].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置を初期化
		g_aMesh_Cylinder[nCntMeshCylinder].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 角度を初期化

		g_aMesh_Cylinder[nCntMeshCylinder].fSizeRadius = 0.0f;					// 大きさ(半径)を初期化
		g_aMesh_Cylinder[nCntMeshCylinder].fSizeHeight = 0.0f;					// 大きさ(高さ)を初期化
																		   
		g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth = 0;						// 分割数(幅)を初期化 
		g_aMesh_Cylinder[nCntMeshCylinder].wBlockHeight = 0;					// 分割数(高さ)を初期化 
																		   
		g_aMesh_Cylinder[nCntMeshCylinder].bReverse = true;						// 面の向きを外側に設定
		g_aMesh_Cylinder[nCntMeshCylinder].bUse = false;						// 使用していない状態に設定
	}

	// メッシュシリンダーの設定
	//SetMesh_Cylinder(false, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 50.0f, 50.0f, 8, 8, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
}

//======================================================================== 
// メッシュシリンダーの終了処理
//========================================================================
void UninitMesh_Cylinder(void)
{
	// テクスチャの破棄
	if (g_pTextureMesh_Cylinder != NULL)
	{
		g_pTextureMesh_Cylinder->Release();
		g_pTextureMesh_Cylinder = NULL;		// 中身を空にする
	}
	
	for (int nCntMeshWall = 0; nCntMeshWall < MAX_SET_MESHCYLINDER; nCntMeshWall++)
	{
		// 頂点バッファの破棄
		if (g_aMesh_Cylinder[nCntMeshWall].pVtxBuff != NULL)
		{
			g_aMesh_Cylinder[nCntMeshWall].pVtxBuff->Release();
			g_aMesh_Cylinder[nCntMeshWall].pVtxBuff = NULL;		// 中身を空にする
		}

		// インデックスバッファの破棄
		if (g_aMesh_Cylinder[nCntMeshWall].pIdxBuff != NULL)
		{
			g_aMesh_Cylinder[nCntMeshWall].pIdxBuff->Release();
			g_aMesh_Cylinder[nCntMeshWall].pIdxBuff = NULL;		// 中身を空にする
		}
	}
}

//======================================================================== 
// メッシュシリンダーの更新処理
//========================================================================
void UpdateMesh_Cylinder(void)
{
	
}

//======================================================================== 
// メッシュシリンダーの描画処理
//========================================================================
void DrawMesh_Cylinder(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXMATRIX mtxRot, mtxTrans;	// 計算用マトリックス

	// =========================================================

	for(int nCntMeshWall = 0; nCntMeshWall < MAX_SET_MESHCYLINDER; nCntMeshWall++)
	{ 
		if(g_aMesh_Cylinder[nCntMeshWall].bUse == true)
		{// 使用している状態の場合 

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aMesh_Cylinder[nCntMeshWall].mtxWorld);	// ワールドマトリックスの初期値を設定

			// 向きの反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot,
										  g_aMesh_Cylinder[nCntMeshWall].rot.y, g_aMesh_Cylinder[nCntMeshWall].rot.x, g_aMesh_Cylinder[nCntMeshWall].rot.z);

			D3DXMatrixMultiply(&g_aMesh_Cylinder[nCntMeshWall].mtxWorld, &g_aMesh_Cylinder[nCntMeshWall].mtxWorld, &mtxRot);

			// 位置を設定
			D3DXMatrixTranslation(&mtxTrans,
								  g_aMesh_Cylinder[nCntMeshWall].pos.x, g_aMesh_Cylinder[nCntMeshWall].pos.y, g_aMesh_Cylinder[nCntMeshWall].pos.z);

			D3DXMatrixMultiply(&g_aMesh_Cylinder[nCntMeshWall].mtxWorld, &g_aMesh_Cylinder[nCntMeshWall].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aMesh_Cylinder[nCntMeshWall].mtxWorld);

			// 頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0, g_aMesh_Cylinder[nCntMeshWall].pVtxBuff, 0, sizeof(VERTEX_3D));

			// インデックスバッファをデータストリームに設定
			pDevice->SetIndices(g_aMesh_Cylinder[nCntMeshWall].pIdxBuff);

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureMesh_Cylinder);

			// ポリゴンの描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
										  0,						
										  0,
										  CREATE_SETVERTEX(g_aMesh_Cylinder[nCntMeshWall].wBlockWidth, g_aMesh_Cylinder[nCntMeshWall].wBlockHeight),		// 頂点の数
										  0,
										  CREATE_SETPOLYGON(g_aMesh_Cylinder[nCntMeshWall].wBlockWidth, g_aMesh_Cylinder[nCntMeshWall].wBlockHeight));	// プリミティブ（ポリゴン）数
		}
	}
}

//========================================================================
// メッシュシリンダーの設定処理
//========================================================================
void SetMesh_Cylinder(bool bReverse, D3DXVECTOR3 pos, D3DXVECTOR3 rot,
					  float fRadius, float fHeight, int nWidthBlock, int nHeightBlock,
					  D3DXCOLOR col)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	VERTEX_3D* pVtx;		// 頂点情報へのポインタを宣言

	WORD* pIdx;				// インデックス情報へのポインタ

	int nCntMeshCylinder,	// カウンタ
		nCntIdx,			// インデックスのカウンタ
		nCntMeshWall_x,		// x方向のカウンタ
		nCntMeshWall_y;		// y方向のカウンタ

	float fSetHeight = fHeight / nHeightBlock;	// 1ポリゴンの高さ

	float fVecX, fVecZ;

	// =========================================================

	for (nCntMeshCylinder = 0; nCntMeshCylinder < MAX_SET_MESHCYLINDER; nCntMeshCylinder++)
	{
		if (g_aMesh_Cylinder[nCntMeshCylinder].bUse == false)
		{// 使用していない場合

			g_aMesh_Cylinder[nCntMeshCylinder].pos = pos;														// 位置を設定
			g_aMesh_Cylinder[nCntMeshCylinder].rot = rot;														// 角度を設定

			g_aMesh_Cylinder[nCntMeshCylinder].fSizeRadius = fRadius;											// 大きさ（半径）を設定
			g_aMesh_Cylinder[nCntMeshCylinder].fSizeHeight = fHeight;											// 大きさ（高さ）を設定

			g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth = (WORD)((nWidthBlock >= 0) ? nWidthBlock : 0);		// 分割数を設定(値が0より大きい場合、値を代入)
			g_aMesh_Cylinder[nCntMeshCylinder].wBlockHeight = (WORD)((nHeightBlock >= 0) ? nHeightBlock : 0);	// 分割数を設定(値が0より大きい場合、値を代入)

			g_aMesh_Cylinder[nCntMeshCylinder].bReverse = bReverse;												// 面の向きを設定
			
			g_aMesh_Cylinder[nCntMeshCylinder].bUse = true;														// 使用している状態に設定

			// 頂点バッファの生成
			pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * CREATE_VERTEX(g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth, g_aMesh_Cylinder[nCntMeshCylinder].wBlockHeight),
										D3DUSAGE_WRITEONLY,
										FVF_VERTEX_3D,
										D3DPOOL_MANAGED,
										&g_aMesh_Cylinder[nCntMeshCylinder].pVtxBuff,
										NULL);
			
			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_aMesh_Cylinder[nCntMeshCylinder].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

			for (nCntMeshWall_y = 0; nCntMeshWall_y <= g_aMesh_Cylinder[nCntMeshCylinder].wBlockHeight; nCntMeshWall_y++)
			{// 高さの設定ポリゴン数が設定の値まで繰り返す

				for (nCntMeshWall_x = 0; nCntMeshWall_x <= g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth; nCntMeshWall_x++)
				{// 幅の設定ポリゴン数が設定の値まで繰り返す

					if (g_aMesh_Cylinder[nCntMeshCylinder].bReverse == true)
					{// 面の向きが外側の場合

						fVecX = cosf(D3DX_PI * 2 / g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth * nCntMeshWall_x);		// 開始位置(左上)に(1ポリゴンの幅 * 設定数)を足す(右にずらす)
						fVecZ = sinf(D3DX_PI * 2 / g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth * nCntMeshWall_x);		// 開始位置(左上)に(1ポリゴンの幅 * 設定数)を足す(右にずらす)
					}
					else
					{// 面の向きが内側の場合

						fVecX = sinf(D3DX_PI * 2 / g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth * nCntMeshWall_x);		// 開始位置(左上)に(1ポリゴンの幅 * 設定数)を足す(右にずらす)
						fVecZ = cosf(D3DX_PI * 2 / g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth * nCntMeshWall_x);		// 開始位置(左上)に(1ポリゴンの幅 * 設定数)を足す(右にずらす)
					}

					// 頂点座標の設定
					pVtx[0].pos.x = (0.0f - fVecX * fRadius);	
					pVtx[0].pos.y = (0.0f + g_aMesh_Cylinder[nCntMeshCylinder].fSizeHeight) - (fSetHeight * nCntMeshWall_y);	// 開始位置(左上)に(1ポリゴンの高さ * 設定数)を引く(下にずらす)				
					pVtx[0].pos.z = (0.0f - fVecZ * fRadius);	// 開始位置(左上)。

					// 法線ベクトルの設定
					pVtx[0].nor.x = fVecX;
					pVtx[0].nor.y = 0.0f;
					pVtx[0].nor.z = fVecZ;

					// 頂点カラー
					pVtx[0].col = D3DCOLOR_RGBA(255, 255, 255, 255);							// (R, G, B, A)

					// テクスチャ座標
					pVtx[0].tex = D3DXVECTOR2((float)nCntMeshWall_x, (float)nCntMeshWall_y);	// (U, V)

					pVtx++;	// 設定した分の頂点バッファを進める
				}
			}

			// ▲頂点バッファをアンロックする
			g_aMesh_Cylinder[nCntMeshCylinder].pVtxBuff->Unlock();

			// インデックスバッファの設定
			pDevice->CreateIndexBuffer(sizeof(WORD) * CREATE_SETVERTEX(g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth, g_aMesh_Cylinder[nCntMeshCylinder].wBlockHeight),		// 確保するバッファのサイズ
									   D3DUSAGE_WRITEONLY,
									   D3DFMT_INDEX16,
									   D3DPOOL_MANAGED,
									   &g_aMesh_Cylinder[nCntMeshCylinder].pIdxBuff,
									   NULL);
			
			// ▼インデックスバッファをロックして頂点情報へのポインタを所得
			g_aMesh_Cylinder[nCntMeshCylinder].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

			for (nCntIdx = 0, nCntMeshWall_x = 0, nCntMeshWall_y = 0; nCntIdx < CREATE_SETVERTEX(g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth, g_aMesh_Cylinder[nCntMeshCylinder].wBlockHeight); nCntIdx++)
			{// 頂点の数だけ設定

				// インデックスの位置を設定
				pIdx[nCntIdx] = nCntMeshWall_x + ((nCntIdx % 2 == 0)														// カウントが偶数の場合
											   ? g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth + 1 * (nCntMeshWall_y + 1)	// 真：幅の分割数 + 1に、縦のカウント + 1をかける
											   : nCntMeshWall_y);															// 偽：縦のカウント
				
				// 端かの判定
				if (pIdx[nCntIdx] == g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth + (g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth * nCntMeshWall_y) + (1 * nCntMeshWall_y)
					&& nCntMeshWall_y + 1 < g_aMesh_Cylinder[nCntMeshCylinder].wBlockHeight)
				{//
					
					// 折り返し前の2回目の値を設定
					pIdx[nCntIdx + 1] = (WORD)(nCntMeshWall_x + ((nCntIdx % 2 == 0)															// カウントが偶数の場合
															  ? g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth + 1 * (nCntMeshWall_y + 1)	// 真：幅の分割数 + 1に、縦のカウント + 1をかける
															  : nCntMeshWall_y));															// 偽：縦のカウント;	
					
					// 折り返し後の値を設定
					pIdx[nCntIdx + 2] = (WORD)((nCntMeshWall_x + 1 + nCntMeshWall_y) + (g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth + 1));	// 折り返し後の1回目の値を設定
					pIdx[nCntIdx + 3] = (WORD)((nCntMeshWall_x + 1 + nCntMeshWall_y) + (g_aMesh_Cylinder[nCntMeshCylinder].wBlockWidth + 1));	// 折り返し後の2回目の値を設定
					
					nCntMeshWall_y++;		// 高さの位置を加算 

					nCntIdx += 3;		// カウントを設定した数だけ進める
				}

				nCntMeshWall_x += nCntIdx % 2;	// カウンタが偶数の場合カウントを増やす
			}

			// ▲インデックスバッファァをアンロックする
			g_aMesh_Cylinder[nCntMeshCylinder].pIdxBuff->Unlock();
			
			break;	// for文を抜ける
		}
	}
}