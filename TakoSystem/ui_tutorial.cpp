//==================================================================================
// 
// チュートリアルのUIに関する処理[ui_tutorial.cpp]
// Author : Soki Inamura 
// 
//==================================================================================
#include "main.h"			// メインヘッダー

#include "debugproc.h"

#include "ui_tutorial.h"	// チュートリアルUIヘッダー

// マクロ定義 ==================================================

#define MAX_NUM_UITUTORIAL			(128)			// 設定出来るゲージアイコンの数
#define MAX_TEX_UITUTORIAL			(64)			// 用意出来るゲージアイコンのテクスチャ数

#define UITUTORIAL_GAUGE_SPEED		(6.0f)

#define UITUTORIAL_GAUGE_POSY		(670.0f)
#define UITUTORIAL_GAUGE_SIZEX		(233.0f)
#define UITUTORIAL_GAUGE_SIZEY		(26.0f)

// 構造体の定義 ================================================

// チュートリアルUIのポリゴン情報
typedef struct
{
	UI_TUTORIALTYPE type;		// ゲージアイコンの状態
	UI_TUTORIALPOS posPosion;	// POSの位置
	int nIdxTexture;			// テクスチャインデックス
	bool bAlphaBrend;			// アルファブレンドするか
	
	float fValue;				// 量

	D3DXVECTOR3 pos;			// 位置
	float fSizeWidth;			// 幅の大きさ
	float fSizeHeight;			// 高さの大きさ

	D3DXVECTOR2 texPos;			// テクスチャ座標
	D3DXVECTOR2 texSize;		// テクスチャサイズ
	
	D3DXCOLOR col;				// 色

	bool bDisp;					// 表示状態
	bool bUse;					// 使用状態

}UiTutorialPolygon;

// チュートリアルUIのポリゴン設定情報
typedef struct
{
	UI_TUTORIALTYPE type;		// ゲージアイコンの状態
	UI_TUTORIALPOS posPosion;	// POSの位置
	int nIdxTexture;			// テクスチャインデックス
	bool bAlphaBrend;			// アルファブレンドするか

	D3DXVECTOR3 pos;			// 位置
	float fSizeWidth;			// 大きさ(幅)
	float fSizeHeight;			// 大きさ(高さ)

	D3DXVECTOR2 texPos;			// テクスチャ座標
	D3DXVECTOR2 texSize;		// テクスチャサイズ

	D3DXCOLOR col;				// 色

}UiTutorialPolygon_info;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureUiTutorial[MAX_TEX_UITUTORIAL] = {};	// テクスチャへのポインタ
int g_aTextureInfo[MAX_TEX_UITUTORIAL] = {};						// テクスチャの場所の値

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUiTutorial = NULL;				// 頂点バッファ

UiTutorialPolygon g_aUiTutorialPolygon[MAX_NUM_UITUTORIAL];			// チュートリアルUIポリゴンの情報

bool g_bSkipTutorial;	// SKIPするかの状態

// テクスチャファイル情報
const char* c_apFilenameUiTutorial[] =
{
	"data/TEXTURE/SKIP_TUTORIAL.png",		// [0]スキップテキスト
};

// チュートリアルUIポリゴンの配置情報
UiTutorialPolygon_info g_aUiTutorialPolygonInfo[] =
{// {種類, POSの位置, テクスチャインデックス, アルファブレンドするか, 位置, 大きさ(幅), 大きさ(高さ), テクスチャ座標, テクスチャサイズ, 色}

	{UI_TUTORIALTYPE_SKIPGAUGE, UI_TUTORIALPOS_LEFT,   -1, true,  D3DXVECTOR3(SCREEN_WIDTH / 2.0f - UITUTORIAL_GAUGE_SIZEX, UITUTORIAL_GAUGE_POSY, 0.0f), UITUTORIAL_GAUGE_SIZEX * 2, UITUTORIAL_GAUGE_SIZEY, D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f)},
	{UI_TUTORIALTYPE_SKIPTEXT,  UI_TUTORIALPOS_CENTER,  0, false, D3DXVECTOR3(SCREEN_WIDTH / 2.0f						  , UITUTORIAL_GAUGE_POSY, 0.0f), UITUTORIAL_GAUGE_SIZEX	, UITUTORIAL_GAUGE_SIZEY, D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)},
};

//========================================================================
// チュートリアルUIの初期化処理
//========================================================================
void InitUiTutorial(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	HRESULT hr;

	// ====================================================

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameUiTutorial / sizeof(c_apFilenameUiTutorial[0]); nCntTexture++)
	{
		// テクスチャの読み込み
		hr = D3DXCreateTextureFromFile(pDevice,								// Direct3Dデバイスへのポインタ
								  c_apFilenameUiTutorial[nCntTexture],	// 読み込むテクスチャ
								  &g_apTextureUiTutorial[nCntTexture]);	// テクスチャへのポインタ
	}

	// UIゲージアイコンの情報を初期化
	for (int nCntPolygon = 0; nCntPolygon < MAX_NUM_UITUTORIAL; nCntPolygon++)
	{
		g_aUiTutorialPolygon[nCntPolygon].type = UI_TUTORIALTYPE_NULL;				// 種類を初期化
		g_aUiTutorialPolygon[nCntPolygon].posPosion = UI_TUTORIALPOS_CENTER;		// POSの位置を初期化
		g_aUiTutorialPolygon[nCntPolygon].nIdxTexture = -1;							// テクスチャインデックスを初期化
		g_aUiTutorialPolygon[nCntPolygon].bAlphaBrend = false;						// アルファブレンドをしない状態に設定
		//g_aUiTutorialPolygon[nCntPolygon].fNum = 0.0f;								// 値を初期化
		g_aUiTutorialPolygon[nCntPolygon].fValue = 0.0f;							// 量を初期化
		g_aUiTutorialPolygon[nCntPolygon].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		g_aUiTutorialPolygon[nCntPolygon].fSizeWidth = 0.0f;						// 幅の大きさを初期化
		g_aUiTutorialPolygon[nCntPolygon].fSizeHeight = 0.0f;						// 高さの大きさを初期化
		g_aUiTutorialPolygon[nCntPolygon].texPos = D3DXVECTOR2(0.0f, 0.0f);			// テクスチャ座標を初期化
		g_aUiTutorialPolygon[nCntPolygon].texSize = D3DXVECTOR2(0.0f, 0.0f);		// テクスチャサイズを初期化
		g_aUiTutorialPolygon[nCntPolygon].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色を初期化
		g_aUiTutorialPolygon[nCntPolygon].bDisp = false;							// 表示していない状態に設定
		g_aUiTutorialPolygon[nCntPolygon].bUse = false;								// 使用していない状態に設定
	}

	g_bSkipTutorial = false;	// スキップしない状態に設定

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_NUM_UITUTORIAL,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUiTutorial,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffUiTutorial->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntUiTutorial = 0; nCntUiTutorial < MAX_NUM_UITUTORIAL; nCntUiTutorial++)
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
	g_pVtxBuffUiTutorial->Unlock();

	// チュートリアルUIポリゴンの設定(ゲージ)
	SetUiTutorialPolygon(g_aUiTutorialPolygonInfo[0].type, g_aUiTutorialPolygonInfo[0].posPosion, g_aUiTutorialPolygonInfo[0].nIdxTexture, g_aUiTutorialPolygonInfo[0].bAlphaBrend,
						 g_aUiTutorialPolygonInfo[0].pos, g_aUiTutorialPolygonInfo[0].fSizeWidth, g_aUiTutorialPolygonInfo[0].fSizeHeight,
						 g_aUiTutorialPolygonInfo[0].texPos, g_aUiTutorialPolygonInfo[0].texSize, g_aUiTutorialPolygonInfo[0].col);

	// チュートリアルUIポリゴンの設定(テキスト)
	SetUiTutorialPolygon(g_aUiTutorialPolygonInfo[1].type, g_aUiTutorialPolygonInfo[1].posPosion, g_aUiTutorialPolygonInfo[1].nIdxTexture, g_aUiTutorialPolygonInfo[1].bAlphaBrend,
						 g_aUiTutorialPolygonInfo[1].pos, g_aUiTutorialPolygonInfo[1].fSizeWidth, g_aUiTutorialPolygonInfo[1].fSizeHeight,
						 g_aUiTutorialPolygonInfo[1].texPos, g_aUiTutorialPolygonInfo[1].texSize, g_aUiTutorialPolygonInfo[1].col);
}

//========================================================================
// チュートリアルUIの終了処理
//========================================================================
void UninitUiTutorial(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_UITUTORIAL; nCntTexture++)
	{
		if (g_apTextureUiTutorial[nCntTexture] != NULL)
		{
			g_apTextureUiTutorial[nCntTexture]->Release();
			g_apTextureUiTutorial[nCntTexture] = NULL;		// 中身を空にする
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffUiTutorial != NULL)
	{
		g_pVtxBuffUiTutorial->Release();
		g_pVtxBuffUiTutorial = NULL;		// 中身を空にする
	}
}

//========================================================================
// チュートリアルUIの更新処理
//========================================================================
void UpdateUiTutorial(void)
{
	// 変数宣言 ================================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// =========================================================

#ifdef _DEBUG
#if 0
	static int nSelectIdx = 0;
	
	if (GetKeyboardTrigger(DIK_N)) nSelectIdx--;
	if (GetKeyboardTrigger(DIK_M)) nSelectIdx++;
	PrintDebugProc("\nUI_TUTORIAL_SELECTIDX %d\n", nSelectIdx);

	if (nSelectIdx >= 0)
	{
		if (GetKeyboardPress(DIK_LSHIFT) == false)
		{
			if (GetKeyboardPress(DIK_I)) g_aUiTutorialPolygon[nSelectIdx].pos.y -= 1.0f;
			if (GetKeyboardPress(DIK_K)) g_aUiTutorialPolygon[nSelectIdx].pos.y += 1.0f;
			if (GetKeyboardPress(DIK_J)) g_aUiTutorialPolygon[nSelectIdx].pos.x -= 1.0f;
			if (GetKeyboardPress(DIK_L)) g_aUiTutorialPolygon[nSelectIdx].pos.x += 1.0f;
		}
		else
		{
			if (GetKeyboardPress(DIK_I)) g_aUiTutorialPolygon[nSelectIdx].fSizeHeight -= 1.0f;
			if (GetKeyboardPress(DIK_K)) g_aUiTutorialPolygon[nSelectIdx].fSizeHeight += 1.0f;
			if (GetKeyboardPress(DIK_J)) g_aUiTutorialPolygon[nSelectIdx].fSizeWidth -= 1.0f;
			if (GetKeyboardPress(DIK_L)) g_aUiTutorialPolygon[nSelectIdx].fSizeWidth += 1.0f;
		}
	}
#endif
#endif

	for (int nCntUiTutorial = 0; nCntUiTutorial < MAX_NUM_UITUTORIAL; nCntUiTutorial++)
	{
		if (g_aUiTutorialPolygon[nCntUiTutorial].bUse == true)
		{// 使用している場合
			
			switch (g_aUiTutorialPolygon[nCntUiTutorial].type)
			{
			case UI_TUTORIALTYPE_NULL:			// [0]無し
				break;

			case UI_TUTORIALTYPE_SKIPTEXT:		// [1]開始テキスト
				
				if (GetJoypadPress(0, JOYKEY_START)
				 || GetJoypadPress(1, JOYKEY_START)
				 || GetKeyboardPress(DIK_RETURN))
				{// SKIPのボタンが押されている場合

					g_aUiTutorialPolygon[nCntUiTutorial].fValue = D3DX_PI * 0.5f;									// 角度を透明度が0になる角度に設定

					if (g_bSkipTutorial == false)
					{// SKIP状態でない場合
						
						g_aUiTutorialPolygon[nCntUiTutorial].bDisp = g_aUiTutorialPolygon[nCntUiTutorial].bDisp ^ true;	// 表示状態を点滅させる
					}
					else
					{// SKIP状態の場合

						g_aUiTutorialPolygon[nCntUiTutorial].bDisp = true;	// 表示している状態に設定
					}
				}
				else
				{// SKIPのボタンが押されていない場合
					
					if (g_bSkipTutorial == false)
					{
						g_aUiTutorialPolygon[nCntUiTutorial].fValue += 0.05f;	// 角度を更新
					}

					g_aUiTutorialPolygon[nCntUiTutorial].bDisp = true;		// 表示している状態に設定
				}

				// 値の補正
				if		(g_aUiTutorialPolygon[nCntUiTutorial].fValue < -D3DX_PI) g_aUiTutorialPolygon[nCntUiTutorial].fValue += D3DX_PI * 2.0f;
				else if (g_aUiTutorialPolygon[nCntUiTutorial].fValue >  D3DX_PI) g_aUiTutorialPolygon[nCntUiTutorial].fValue -= D3DX_PI * 2.0f;
				
				// 透明度を設定
				g_aUiTutorialPolygon[nCntUiTutorial].col.a = 0.5f + (sinf(g_aUiTutorialPolygon[nCntUiTutorial].fValue) * 0.5f);
				
				break;

			case UI_TUTORIALTYPE_SKIPGAUGE:		// [2]開始ゲージ
				
				// 初期設定
				if (g_aUiTutorialPolygon[nCntUiTutorial].fValue == 0.0f)
				{// 値がまだない場合

					g_aUiTutorialPolygon[nCntUiTutorial].fValue = g_aUiTutorialPolygon[nCntUiTutorial].fSizeWidth;	// 幅の長さを設定
				}

				if (g_bSkipTutorial == false)
				{// スキップ状態でない

					if (GetJoypadPress(0, JOYKEY_START)
					 || GetJoypadPress(1, JOYKEY_START)
					 || GetKeyboardPress(DIK_RETURN))
					{// SKIPのボタンが押されている場合

						if (g_aUiTutorialPolygon[nCntUiTutorial].fValue >= g_aUiTutorialPolygon[nCntUiTutorial].fSizeWidth)
						{// 幅の長さが、元の幅の長さより小さい場合

							g_aUiTutorialPolygon[nCntUiTutorial].fSizeWidth += 6.0f;	// 幅の長さを伸ばす
						}
						else
						{
							g_bSkipTutorial = true;		// スキップする状態に設定
						}
					}
					else
					{// SKIPのボタンが押されていない場合

						
						g_aUiTutorialPolygon[nCntUiTutorial].fSizeWidth = 0.0f;			// 幅の長さをリセット
					}
				}

				break;
			}
			
			// 頂点情報の更新
			SetVtxUiTutorial(nCntUiTutorial, g_aUiTutorialPolygon[nCntUiTutorial].posPosion,
							 g_aUiTutorialPolygon[nCntUiTutorial].pos,
							 g_aUiTutorialPolygon[nCntUiTutorial].fSizeWidth, g_aUiTutorialPolygon[nCntUiTutorial].fSizeHeight,
							 g_aUiTutorialPolygon[nCntUiTutorial].texPos, g_aUiTutorialPolygon[nCntUiTutorial].texSize, 
							 g_aUiTutorialPolygon[nCntUiTutorial].col);

#if 0
			PrintDebugProc("\nPOS[%d] %f %f %f"		, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x, g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y, g_aUiGaugeIcon[nCntUiGaugeIcon].pos.z);
			PrintDebugProc("\nSIZE[%d] %f"			, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeWidth);
			PrintDebugProc("\nSTATE[%d] %d"			, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].state);
			PrintDebugProc("\nSTATE_COUNTER[%d] %d"	, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].nCounterState);
#endif
		}
	}
}

//========================================================================
// チュートリアルUIの描画処理
//========================================================================
void DrawUiTutorial(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// =========================================================

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffUiTutorial, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntUiGaugeIcon = 0; nCntUiGaugeIcon < MAX_NUM_UITUTORIAL; nCntUiGaugeIcon++)
	{// 用意した分繰り返す

		if (g_aUiTutorialPolygon[nCntUiGaugeIcon].bUse == true && g_aUiTutorialPolygon[nCntUiGaugeIcon].bDisp == true)
		{// 使用していて表示している状態の場合

			// テクスチャの設定
			if (g_aUiTutorialPolygon[nCntUiGaugeIcon].nIdxTexture != -1)
			{// テクスチャが設定されている場合
				
				pDevice->SetTexture(0, g_apTextureUiTutorial[g_aUiTutorialPolygon[nCntUiGaugeIcon].nIdxTexture]);
			}
			else
			{// テクスチャが設定されていない場合

				pDevice->SetTexture(0, NULL);
			}

			if (g_aUiTutorialPolygon[nCntUiGaugeIcon].bAlphaBrend == true)
			{// アルファブレンドする場合
				
				// 減算合成の設定
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3
			}

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
								   nCntUiGaugeIcon * 4,		// インデックス
								   2);						// プリミティブ（ポリゴン）数

			if (g_aUiTutorialPolygon[nCntUiGaugeIcon].bAlphaBrend == true)
			{// アルファブレンドする場合

				// 減算合成を元に戻す 
				pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
				pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
				pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
			}
		}
	}
}

//========================================================================
// チュートリアルUIの設定処理
//========================================================================
void SetUiTutorialPolygon(UI_TUTORIALTYPE type, UI_TUTORIALPOS posPosion, int nIdxTexture, bool bAlphaBrend,
						  D3DXVECTOR3 pos, float fSizeWidth, float fSizeHeight, D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{

	for (int nCntUiGaugeIcon = 0; nCntUiGaugeIcon < MAX_NUM_UITUTORIAL; nCntUiGaugeIcon++)
	{
		if (g_aUiTutorialPolygon[nCntUiGaugeIcon].bUse == false)
		{// 使用していない場合

			g_aUiTutorialPolygon[nCntUiGaugeIcon].type = type;					// 種類を設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].posPosion = posPosion;		// POSの位置を設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].nIdxTexture = nIdxTexture;	// テクスチャのインデックスを設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].bAlphaBrend = bAlphaBrend;	// アルファブレンドをするかを設定
			
			g_aUiTutorialPolygon[nCntUiGaugeIcon].fValue = 0.0f;				// 量を設定
			
			g_aUiTutorialPolygon[nCntUiGaugeIcon].pos = pos;					// 位置を設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].fSizeWidth = fSizeWidth;		// 大きさ(幅)を設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].fSizeHeight = fSizeHeight;	// 大きさ(高さ)を設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].texPos = texPos;				// テクスチャ座標を設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].texSize = texSize;			// テクスチャサイズを設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].col = col;					// 色を設定
	
			g_aUiTutorialPolygon[nCntUiGaugeIcon].bDisp = true;					// 表示している状態に設定
			g_aUiTutorialPolygon[nCntUiGaugeIcon].bUse = true;					// 使用してない状態に設定

			// 頂点情報の更新
			SetVtxUiTutorial(nCntUiGaugeIcon, g_aUiTutorialPolygon[nCntUiGaugeIcon].posPosion,
							 g_aUiTutorialPolygon[nCntUiGaugeIcon].pos,
							 g_aUiTutorialPolygon[nCntUiGaugeIcon].fSizeWidth, g_aUiTutorialPolygon[nCntUiGaugeIcon].fSizeHeight,
							 g_aUiTutorialPolygon[nCntUiGaugeIcon].texPos, g_aUiTutorialPolygon[nCntUiGaugeIcon].texSize,
							 g_aUiTutorialPolygon[nCntUiGaugeIcon].col);

			break;	// for文を抜ける
		}
	}
}

//========================================================================
// チュートリアルUIの頂点設定処理
//========================================================================
void SetVtxUiTutorial(int nStartVtx, UI_TUTORIALPOS posPosion, D3DXVECTOR3 pos, float fSizeWidth, float fSizeHeight,
					  D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col)
{
	// 変数宣言 ======================================

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ===============================================

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffUiTutorial->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += (nStartVtx * 4);	// 開始位置まで進める

	// 頂点座標の設定
	switch (posPosion)
	{
	case UI_TUTORIALPOS_CENTER:
		
		pVtx[0].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y + fSizeHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y + fSizeHeight, 0.0f);

		break;

	case UI_TUTORIALPOS_TOP:

		pVtx[0].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y				 , 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y				 , 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y + fSizeHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y + fSizeHeight, 0.0f);

		break;

	case UI_TUTORIALPOS_BOT:

		pVtx[0].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y				 , 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y				 , 0.0f);

		break;

	case UI_TUTORIALPOS_LEFT:

		pVtx[0].pos = D3DXVECTOR3(pos.x				, pos.y - fSizeHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x				, pos.y + fSizeHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y + fSizeHeight, 0.0f);

		break;

	case UI_TUTORIALPOS_RIGHT:

		pVtx[0].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x				, pos.y - fSizeHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y + fSizeHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x				, pos.y + fSizeHeight, 0.0f);

		break;

	case UI_TUTORIALPOS_TOPLEFT:

		pVtx[0].pos = D3DXVECTOR3(pos.x				, pos.y				 , 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y				 , 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x				, pos.y + fSizeHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y + fSizeHeight, 0.0f);

		break;

	case UI_TUTORIALPOS_TOPRIGHT:

		pVtx[0].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y				 , 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x				, pos.y				 , 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y + fSizeHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x				, pos.y + fSizeHeight, 0.0f);

		break;

	case UI_TUTORIALPOS_BOTLEFT:

		pVtx[0].pos = D3DXVECTOR3(pos.x				, pos.y - fSizeHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x				, pos.y				 , 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x + fSizeWidth, pos.y				 , 0.0f);

		break;

	case UI_TUTORIALPOS_BOTRIGHT:

		pVtx[0].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y - fSizeHeight, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pos.x				, pos.y - fSizeHeight, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pos.x - fSizeWidth, pos.y + fSizeHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pos.x				, pos.y + fSizeHeight, 0.0f);

		break;
	}

	// 頂点カラー
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(texPos.x			  , texPos.y			); 
	pVtx[1].tex = D3DXVECTOR2(texPos.x + texSize.x, texPos.y			);
	pVtx[2].tex = D3DXVECTOR2(texPos.x			  , texPos.y + texSize.y);
	pVtx[3].tex = D3DXVECTOR2(texPos.x + texSize.x, texPos.y + texSize.y);

	// ▲頂点バッファをアンロックする
	g_pVtxBuffUiTutorial->Unlock();
}

//========================================================================
// チュートリアルUIのスキップするかを返す処理
//========================================================================
bool GetSkipTutorial(void)
{
	return g_bSkipTutorial;
}