//==================================================================================
// 
// エサのUIに関する処理[ui_esa.cpp]
// Author : Soki Inamura 
// 
//==================================================================================

#include "main.h"		// メインヘッダー

#include "esa.h"		// エサヘッダー

#include "ui_esa.h"		// エサUIヘッダー

#include "camera.h"

#include "input.h"
#include "debugproc.h"

// マクロ定義 ==================================================

#define UIESA_SIZE_WIDTH	(10.0f)		// エサテクスチャの幅の大きさ
#define UIESA_SIZE_HEIGHT	(10.0f)		// エサテクスチャの高さの大きさ 

// 構造体の定義 ================================================

// エサUI(子)の情報
typedef struct
{
	int nIdxTexture;	// テクスチャインデックス

	D3DXVECTOR3 pos;	// 位置
	D3DXVECTOR3 rot;	// 角度
	float fSizeLength;	// 対角線の長さ
	float fSizeAngle;	// 対角線の角度

	bool bDisp;			// 表示状態
	bool bUse;			// 使用状態

}UiEsaChild;

// エサUI(親)の設定情報
typedef struct
{
	int nNumPlay;				// プレイ人数(nNumPlayの値の時に設定される)

	D3DXVECTOR3 pos;			// 位置(左上)

	int nNumNewLine;			// 改行するまでの数
	D3DXVECTOR2 space;			// 余白の大きさ
	float fEsaWidth;			// エサの大きさ(幅)
	float fEsaHeight;			// エサの大きさ(高さ)

}UiEsaParent_info;

// グローバル宣言 ==============================================

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUiEsa = NULL;				// 頂点バッファ

UiEsaParent g_aUiEsaParent[MAX_PLAYER];						// エサUIの親の情報
UiEsaChild g_aUiEsaChild[MAX_SET_ESA];						// エサUIの子の情報(総ポリゴン)

// エサUIの設定情報
UiEsaParent_info g_aUiEsaParentInfo[] =
{// {プレイ人数, 位置, 改行数, 余白, エサの大きさ(幅), エサの大きさ(高さ)}

	{1,D3DXVECTOR3(975.0f,291.0f,0.0f), 5, D3DXVECTOR2(10.0f, 0.0f), 25.0f, 25.0f},	// 1人プレイ時：1P設定情報

	{2,D3DXVECTOR3(60.0f,18.0f,0.0f), 7, D3DXVECTOR2(10.0f, 0.0f), 25.0f, 30.0f},	// 2人プレイ時：1P設定情報	
	{2,D3DXVECTOR3(820.0f,18.0f,0.0f), 7, D3DXVECTOR2(10.0f, 0.0f), 25.0f, 30.0f},	// 2人プレイ時：2P設定情報	
};

//========================================================================
// エサUIの初期化処理
//========================================================================
void InitUiEsa(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nCntUiEsa;

	// ====================================================

	// エサUIの親の情報を初期化
	for (nCntUiEsa = 0; nCntUiEsa < MAX_PLAYER; nCntUiEsa++)
	{
		g_aUiEsaParent[nCntUiEsa].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置を初期化
		g_aUiEsaParent[nCntUiEsa].space = D3DXVECTOR2(0.0f, 0.0f);		// 余白を初期化
		g_aUiEsaParent[nCntUiEsa].fEsaWidth = 0.0f;						// エサの大きさを初期化
		g_aUiEsaParent[nCntUiEsa].fEsaHeight = 0.0f;					// エサの大きさを初期化
		g_aUiEsaParent[nCntUiEsa].nNumEsa = 0;							// 所持してるエサの総数を初期化
		g_aUiEsaParent[nCntUiEsa].nNumNewLine = 0;						// 改行するまでの数を初期化
		g_aUiEsaParent[nCntUiEsa].bUse = false;							// 使用していない状態に設定

		memset(&g_aUiEsaParent[nCntUiEsa].aIdxEsa[0],					// 所持しているエサのインデックスを初期化
			   -1,  sizeof g_aUiEsaParent[nCntUiEsa].aIdxEsa);	
	}

	// エサUIの子の情報を初期化
	for (nCntUiEsa = 0; nCntUiEsa < MAX_SET_ESA; nCntUiEsa++)
	{
		g_aUiEsaChild[nCntUiEsa].nIdxTexture = -1;						// テクスチャインデックスを初期化
		g_aUiEsaChild[nCntUiEsa].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 位置を初期化
		g_aUiEsaChild[nCntUiEsa].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 角度を初期化
		g_aUiEsaChild[nCntUiEsa].fSizeLength = 0.0f;					// 対角線の長さを初期化
		g_aUiEsaChild[nCntUiEsa].fSizeAngle = 0.0f;						// 対角線の角度を初期化
		g_aUiEsaChild[nCntUiEsa].bDisp = false;							// 表示していない状態に設定
		g_aUiEsaChild[nCntUiEsa].bUse = false;							// 使用していない状態に設定
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_SET_ESA,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUiEsa,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffUiEsa->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntUiEsa = 0; nCntUiEsa < MAX_SET_ESA; nCntUiEsa++)
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

	// エサの親の設定
	for (int nCntUiEsa = 0; nCntUiEsa < sizeof g_aUiEsaParentInfo / sizeof g_aUiEsaParentInfo[0]; nCntUiEsa++)
	{
		if (g_aUiEsaParentInfo[nCntUiEsa].nNumPlay == GetNumCamera())
		{// プレイ人数が同じ場合

			SetUiEsaParent(g_aUiEsaParentInfo[nCntUiEsa].pos,													// 位置,
						   g_aUiEsaParentInfo[nCntUiEsa].nNumNewLine, g_aUiEsaParentInfo[nCntUiEsa].space,		// 改行数, 余白, 
						   g_aUiEsaParentInfo[nCntUiEsa].fEsaWidth, g_aUiEsaParentInfo[nCntUiEsa].fEsaHeight);	// エサの大きさ(幅, 高さ)
		}
	}
}

//========================================================================
// エサUIの終了処理  
//========================================================================
void UninitUiEsa(void)		
{
	// 頂点バッファの破棄
	if (g_pVtxBuffUiEsa != NULL)
	{
		g_pVtxBuffUiEsa->Release();
		g_pVtxBuffUiEsa = NULL;		// 中身を空にする
	}
}

//========================================================================
// エサUIの更新処理  
//========================================================================
void UpdateUiEsa(void)
{
	// 変数宣言 ================================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	D3DXVECTOR3 setPos;	// 設定する位置の値

	int nSetRand = rand() % ESATYPE_MAX;

	// =========================================================

#if 0	// デバッグ

	if (GetKeyboardTrigger(DIK_RSHIFT))
	{
		SetAddUiEsa(0, nSetRand);
		SetAddUiEsa(1, nSetRand);
	}
	if (GetKeyboardTrigger(DIK_LSHIFT))
	{
		SetSubUiEsa(0);
		SetSubUiEsa(1);
	}

		if (GetKeyboardPress(DIK_I)) g_aUiEsaParent[0].pos.y -= 1.0f;
		if (GetKeyboardPress(DIK_K)) g_aUiEsaParent[0].pos.y += 1.0f;
		if (GetKeyboardPress(DIK_J)) g_aUiEsaParent[0].pos.x -= 1.0f;
		if (GetKeyboardPress(DIK_L)) g_aUiEsaParent[0].pos.x += 1.0f;


		PrintDebugProc("\nUIESA[0]_POS %f %f %f", g_aUiEsaParent[0].pos.x, g_aUiEsaParent[0].pos.y, g_aUiEsaParent[0].pos.z);
	
		//PrintDebugProc("\nUIESA(PALENT)[%d]_TOTAL %d", nCntUiEsa, g_aUiEsaParent[nCntUiEsa].nNumEsa);

#endif

	for (int nCntParent = 0; nCntParent < GetNumCamera(); nCntParent++)
	{
		if (g_aUiEsaParent[nCntParent].bUse == false)
		{// 使用していない場合

			continue;		// 処理の始めに戻る
		}

		for (int nCntChild = 0; nCntChild < MAX_SET_ESA; nCntChild++)
		{
			// インデックスの値を取り出す
			int nIdxNum = g_aUiEsaParent[nCntParent].aIdxEsa[nCntChild];

			if (nIdxNum == -1)
			{// インデックスが設定されていない場合

				continue;	// 処理の始めに戻る
			}

			// 位置を求める
			setPos.x = g_aUiEsaParent[nCntParent].pos.x + g_aUiEsaParent[nCntParent].fEsaWidth			// 基準の位置
					 + (g_aUiEsaParent[nCntParent].fEsaWidth * 2 + g_aUiEsaParent[nCntParent].space.x)	// + 基準のずらす量
					 * (nCntChild % g_aUiEsaParent[nCntParent].nNumNewLine);							// * ずらす量

			setPos.y = g_aUiEsaParent[nCntParent].pos.y + g_aUiEsaParent[nCntParent].fEsaHeight			// 基準の位置
					 + (g_aUiEsaParent[nCntParent].fEsaHeight * 2 + g_aUiEsaParent[nCntParent].space.y)	// + 基準のずらす量
					 * (nCntChild / g_aUiEsaParent[nCntParent].nNumNewLine);							// * ずらす量

			setPos.z = 0.0f;																			// 無し

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffUiEsa->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nIdxNum * 4);	// 頂点バッファを設定する場所の先頭まで進める

			// 頂点座標の設定
			pVtx[0].pos.x = setPos.x - sinf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;
			pVtx[1].pos.x = setPos.x + sinf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;
			pVtx[2].pos.x = setPos.x - sinf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;
			pVtx[3].pos.x = setPos.x + sinf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;

			pVtx[0].pos.y = setPos.y - cosf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;
			pVtx[1].pos.y = setPos.y - cosf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;
			pVtx[2].pos.y = setPos.y + cosf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;
			pVtx[3].pos.y = setPos.y + cosf(g_aUiEsaChild[nIdxNum].fSizeAngle) * g_aUiEsaChild[nIdxNum].fSizeLength;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// ▲頂点バッファをアンロックする
			g_pVtxBuffUiEsa->Unlock();
		}
	}
}

//========================================================================
// エサUIの描画処理  
//========================================================================
void DrawUiEsa(void)		
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	int nIdx;	// インデックスの値

	EsaData* pEsaData = GetEsaData(0);

	// =========================================================

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffUiEsa, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntParent = 0; nCntParent < GetNumCamera(); nCntParent++)
	{
		if (g_aUiEsaParent[nCntParent].bUse == false)
		{// 親が使用されていない場合

			continue;	// 処理の始めに戻る
		}

		for (int nCntChild = 0; nCntChild < MAX_SET_ESA; nCntChild++)
		{
			// エサ(子)のインデックスの値を取り出す
			nIdx = g_aUiEsaParent[nCntParent].aIdxEsa[nCntChild];

			if (nIdx != -1)
			{// エサのインデックスが設定されている場合

				// テクスチャの設定
				if (g_aUiEsaChild[nIdx].nIdxTexture != -1)
				{// テクスチャが設定されている場合

					pDevice->SetTexture(0, pEsaData[g_aUiEsaChild[nIdx].nIdxTexture].pTexture);
				}
				else
				{// テクスチャが設定されていない場合

					pDevice->SetTexture(0, NULL);
				}

				// ポリゴンの描画
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
									   nIdx * 4,				// インデックス
									   2);						// プリミティブ（ポリゴン）数
			}
		}
	}
}

//========================================================================
// エサUIの親の設定処理  
//========================================================================
int SetUiEsaParent(D3DXVECTOR3 pos, int nNumNewLine, D3DXVECTOR2 space, float fEsaWidth, float fEsaHeight)
{
	for (int nCntParent = 0; nCntParent < GetNumCamera(); nCntParent++)
	{
		if (g_aUiEsaParent[nCntParent].bUse == false)
		{// 使用していない場合

			// エサUI(親)の各情報を設定
			g_aUiEsaParent[nCntParent].pos = pos;					// 位置を設定

			g_aUiEsaParent[nCntParent].nNumEsa = 0;					// 所持してるエサの総数を設定
			g_aUiEsaParent[nCntParent].nNumNewLine = nNumNewLine;	// 改行するまでの数を設定
			g_aUiEsaParent[nCntParent].space = space;				// 余白を設定
			
			g_aUiEsaParent[nCntParent].fEsaWidth = fEsaWidth;		// エサの大きさを設定
			g_aUiEsaParent[nCntParent].fEsaHeight = fEsaHeight;		// エサの大きさを設定

			g_aUiEsaParent[nCntParent].bUse = true;					// 使用している状態に設定

			return nCntParent;		// 設定した場所のインデックスを返す
		}
	}

	return -1;						// 設定していない事を返す
}

//========================================================================
// エサUIの子の設定処理  
//========================================================================
int SetUiEsaChild(int nIdxType, float fSizeWidth, float fSizeHeight)
{
	// 変数宣言 ================================================
	
	float fLength, fAngle;	// 対角線の値

	// =========================================================

	for (int nCntChild = 0; nCntChild < MAX_SET_ESA; nCntChild++)
	{
		if (g_aUiEsaChild[nCntChild].bUse == false)
		{// 使用していない場合

			// 対角線の値を求める
			fLength = sqrtf((fSizeWidth * 2) * (fSizeWidth * 2) + (fSizeHeight * 2) * (fSizeHeight * 2)) * 0.5f;

			fAngle = atan2f((fSizeWidth * 2), (fSizeHeight * 2));

			// エサUI(子)の各情報を設定
			g_aUiEsaChild[nCntChild].nIdxTexture = nIdxType;				// テクスチャインデックスを設定
			//g_aUiEsaChild[nCntChild].pos = pos;							// 位置を設定
			g_aUiEsaChild[nCntChild].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 角度を初期化
			g_aUiEsaChild[nCntChild].fSizeLength = fLength;					// 対角線の長さを設定
			g_aUiEsaChild[nCntChild].fSizeAngle = fAngle;					// 対角線の角度を設定
			g_aUiEsaChild[nCntChild].bDisp = true;							// 表示している状態に設定
			g_aUiEsaChild[nCntChild].bUse = true;							// 使用している状態に設定

			return nCntChild;	// 設定した場所のインデックスを返す
		}
	}

	return -1;					// 設定していないことを返す
}

//========================================================================
// エサUIのエサの追加処理  
//========================================================================
void SetAddUiEsa(int nIdxParent, int nSetType)
{
	// 変数宣言 ================================================

	D3DXVECTOR3 setPos;									// 設定する位置の値
	
	UiEsaParent* pUiEsaP = &g_aUiEsaParent[nIdxParent];	// 追加する親の情報

	// =========================================================

	if (pUiEsaP->bUse == false)
	{// 設定されていない場合

		return;	// エサを追加しない
	}

	// エサUI(子)の設定(設定した子のインデックスを設定)
	pUiEsaP->aIdxEsa[pUiEsaP->nNumEsa] = SetUiEsaChild(nSetType,								// 種類
													   g_aUiEsaParent[nIdxParent].fEsaWidth,	// 大きさ(幅)
													   g_aUiEsaParent[nIdxParent].fEsaHeight);	// 大きさ(高さ)

	pUiEsaP->nNumEsa++;	// 持っているエサの総数を増やす	
}

//========================================================================
// エサUIのエサの除外処理
//========================================================================
int SetSubUiEsa(int nIdxParent)
{
	// 変数宣言 ================================================

	D3DXVECTOR3 setPos;													// 設定する位置の値

	UiEsaParent* pUiEsaP = &g_aUiEsaParent[nIdxParent];					// 除外する親の情報

	int nIdxNum;														// インテックスの値

	int nSize = sizeof pUiEsaP->aIdxEsa / sizeof pUiEsaP->aIdxEsa[0];	// 所持しているエサの大きさを求める

	// =========================================================

	if (pUiEsaP->bUse == false)
	{// 親が設定されていない場合

		return -1;	// エサの除外しない
	}

	if (pUiEsaP->aIdxEsa[0] == -1)
	{// 手持ちのエサがない場合

		return -1;	// エサの除外しない 
	}

	// 最初の値を除外
	nIdxNum = pUiEsaP->aIdxEsa[0];			// インテックスを取り出す
	pUiEsaP->aIdxEsa[0] = -1;				// インテックスを初期化

	g_aUiEsaChild[nIdxNum].bDisp = false;	// 表示していない状態に設定
	g_aUiEsaChild[nIdxNum].bUse = false;	// 使用していない状態に設定

	// 並び替える(整える)処理
	FillSort(&pUiEsaP->aIdxEsa[0], nSize);

	pUiEsaP->nNumEsa--;	// 持っているエサの総数を減らす

	return g_aUiEsaChild[nIdxNum].nIdxTexture;	// 除外したエサの種類を返す
}

//========================================================================
// 空白を詰めるソート処理
//========================================================================
void FillSort(int* pArray, int nArraySize)
{
	for(int nCntArray = 0; nCntArray < nArraySize; nCntArray++)
	{// 全ての値で調べる

		if (pArray[nCntArray] == -1 && nCntArray < nArraySize -1)
		{// 値が無い && 次の場所がある場合

			// 次の値を今の配列に詰める処理
			for (int nCnt = nCntArray; nCnt < nArraySize; nCnt++)
			{
				if (nCnt == nArraySize - 1)
				{// 配列の最後の場合

					pArray[nCnt] = -1;					// 無いという値を代入
				}
				else
				{// 配列の最後でない場合

					pArray[nCnt] = pArray[nCnt + 1];	// 次の配列の値を代入
				}
			}
		}
	}
}