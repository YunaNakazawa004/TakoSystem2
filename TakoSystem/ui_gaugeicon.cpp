//==================================================================================
// 
// UIゲージアイコンに関する処理[ui_gaugeicon.cpp]
// Author : Soki Inamura 
// 
//==================================================================================
#include "main.h"			// メインヘッダー

#include "debugproc.h"

#include "camera.h"

#include "ui_gaugeicon.h"	// UIゲージアイコンヘッダー

// マクロ定義 ==================================================

#define MAX_SET_UIGAUGEICON			(64)			// 設定出来るゲージアイコンの数
#define MAX_TEX_UIGAUGEICON			(64)			// 用意出来るゲージアイコンのテクスチャ数

#define UIGAUGEICON_TIME_INKSPIT	(60 * 5)		// 墨吐きのリキャスト時間
#define UIGAUGEICON_TIME_TENTACLE	(60 * 1)		// 触手のリキャスト時間

#define UIGAUGEICON_ENDRECASTTIME	(60 * 1)		// リキャスト完了継続時間

#define UIGAUGEICON_CALC_SIZEARRAY(aArray)	(sizeof aArray / sizeof(aArray[0]))	// 配列の大きさを求める

// 構造体の定義 ================================================

// UIゲージアイコンの情報
typedef struct
{
	int nIdxTexture;			// テクスチャインデックス

	int nUser;					// 対象(コントローラー)
	JOYKEY input;				// 入力キー

	D3DXVECTOR3 pos;			// 位置
	float fSizeLength;			// 対角線の長さ
	float fSizeAngle;			// 対角線の角度

	D3DXCOLOR col;				// 色

	UIGAUGEICON_STATE state;	// ゲージアイコンの状態
	int nMaxCntState;			// 状態カウンタの最大値
	int nCounterState;			// 状態カウンタの値

	bool bDisp;					// 表示状態
	bool bUse;					// 使用状態

}UiGaugeIcon;

// UIゲージアイコンの設定情報
typedef struct
{
	int nIdxTexture;			// テクスチャインデックス

	int nUser;					// 対象(コントローラー)
	JOYKEY input;				// 入力キー

	D3DXVECTOR3 pos;			// 位置
	float fSizeWidth;			// 大きさ(幅)
	float fSizeHeight;			// 大きさ(高さ)

	D3DXCOLOR col;				// 色

	UIGAUGEICON_STATE state;	// ゲージアイコンの状態
	int nCounterState;			// 状態カウンタ

}UiGaugeIcon_info;

// グローバル宣言 ==============================================

LPDIRECT3DTEXTURE9 g_apTextureUiGaugeIcon[MAX_TEX_UIGAUGEICON] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUiGaugeIcon = NULL;					// 頂点バッファ

int g_aNumInfoTexture[MAX_TEX_UIGAUGEICON] = {};						// テクスチャの場所の値

UiGaugeIcon g_aUiGaugeIcon[MAX_SET_UIGAUGEICON];						// UIゲージアイコンの情報

// テクスチャファイル情報
const char* c_apFilenameUiGameIcon[] =
{
	"data/TEXTURE/Octopus_inkgage00.png",				// [0]墨吐きゲージ
	"data/TEXTURE/Octopus_tentaclegage00.png",		// [1]触手ゲージ
};

// UIゲージアイコンの配置情報
UiGaugeIcon_info g_aUiGaugeIconInfo[] =
{// {種類, 対象(使用者), 入力キー, 位置, 大きさ(幅), 大きさ(高さ), 色, 開始状態, 継続時間(リキャスト時間)}

	{0, 0, JOYKEY_LEFTTRIGGER,   D3DXVECTOR3(68.0f, 644.0f, 0.0f), 60.11f, 60.11f, D3DXCOLOR(0.5f,0.5f,0.5f,1.0f), UIGAUGEICON_STATE_RECAST, UIGAUGEICON_TIME_INKSPIT},
	{1, 0, JOYKEY_RIGHTTRIGGER,  D3DXVECTOR3(200.0f, 644.0f, 0.0f), 60.11f, 60.11f, D3DXCOLOR(0.5f,0.5f,0.5f,1.0f), UIGAUGEICON_STATE_RECAST, UIGAUGEICON_TIME_TENTACLE},

	{ 1, 1, JOYKEY_RIGHTTRIGGER, D3DXVECTOR3(1212.0f, 644.0f, 0.0f), 60.11f, 60.11f, D3DXCOLOR(0.5f,0.5f,0.5f,1.0f), UIGAUGEICON_STATE_RECAST, UIGAUGEICON_TIME_TENTACLE},
	{ 0, 1, JOYKEY_LEFTTRIGGER,  D3DXVECTOR3(1080.0f, 644.0f, 0.0f), 60.11f, 60.11f, D3DXCOLOR(0.5f,0.5f,0.5f,1.0f), UIGAUGEICON_STATE_RECAST, UIGAUGEICON_TIME_INKSPIT},
};

//========================================================================
// UIゲージアイコンの初期化処理
//========================================================================
void InitUiGaugeIcon(void)
{
	// 変数宣言 ===========================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ====================================================

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < UIGAUGEICON_CALC_SIZEARRAY(c_apFilenameUiGameIcon); nCntTexture++)
	{
		// テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,								// Direct3Dデバイスへのポインタ
								c_apFilenameUiGameIcon[nCntTexture],	// 読み込むテクスチャ
								&g_apTextureUiGaugeIcon[nCntTexture]);	// テクスチャへのポインタ
	}

	// UIゲージアイコンの情報を初期化
	for (int nCntUiGaugeIcon = 0; nCntUiGaugeIcon < MAX_SET_UIGAUGEICON; nCntUiGaugeIcon++)
	{
		g_aUiGaugeIcon[nCntUiGaugeIcon].nIdxTexture = -1;							// テクスチャインデックスを初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].nUser = -1;									// 対象を初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].input = JOYKEY_MAX;							// 入力キーを初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength = 0.0f;							// 対角線の長さを初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle = 0.0f;							// 対角線の角度を初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色を初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].state = UIGAUGEICON_STATE_FULL;				// ゲージ満タン状態に設定
		g_aUiGaugeIcon[nCntUiGaugeIcon].nMaxCntState = 0;							// 状態カウンタの最大値を初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].nCounterState = 0;							// 状態カウンタの値を初期化
		g_aUiGaugeIcon[nCntUiGaugeIcon].bDisp = false;								// 表示していない状態に設定
		g_aUiGaugeIcon[nCntUiGaugeIcon].bUse = false;								// 使用していない状態に設定
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_SET_UIGAUGEICON,
								D3DUSAGE_WRITEONLY,
								FVF_VERTEX_2D,
								D3DPOOL_MANAGED,
								&g_pVtxBuffUiGaugeIcon,
								NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// ▼頂点バッファをロックして頂点情報へのポインタを所得
	g_pVtxBuffUiGaugeIcon->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntUiGaugeIcon = 0; nCntUiGaugeIcon < MAX_SET_UIGAUGEICON; nCntUiGaugeIcon++)
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
	g_pVtxBuffUiGaugeIcon->Unlock();

	// 設定処理
	for (int nCntSetGaugeIcon = 0; nCntSetGaugeIcon < sizeof g_aUiGaugeIconInfo / sizeof g_aUiGaugeIconInfo[0]; nCntSetGaugeIcon++)
	{
		if (g_aUiGaugeIconInfo[nCntSetGaugeIcon].nUser < GetNumCamera())
		{// 対象がカメラの数より小さい場合

			// ゲージアイコンの設定
			SetUiGaugeIcon(g_aUiGaugeIconInfo[nCntSetGaugeIcon].nIdxTexture,													// インデックステクスチャ
						   g_aUiGaugeIconInfo[nCntSetGaugeIcon].nUser, g_aUiGaugeIconInfo[nCntSetGaugeIcon].input,				// 対象, 入力キー
						   g_aUiGaugeIconInfo[nCntSetGaugeIcon].pos, g_aUiGaugeIconInfo[nCntSetGaugeIcon].col,					// 位置, 色
						   g_aUiGaugeIconInfo[nCntSetGaugeIcon].fSizeWidth, g_aUiGaugeIconInfo[nCntSetGaugeIcon].fSizeHeight,	// 幅, 高さ
						   g_aUiGaugeIconInfo[nCntSetGaugeIcon].state, g_aUiGaugeIconInfo[nCntSetGaugeIcon].nCounterState);		// 開始状態, 持続時間
		}
	}
}

//========================================================================
// UIゲージアイコンの終了処理
//========================================================================
void UninitUiGaugeIcon(void)
{
	// テクスチャの破棄
	for (int nCntTexture = 0; nCntTexture < MAX_TEX_UIGAUGEICON; nCntTexture++)
	{
		if (g_apTextureUiGaugeIcon[nCntTexture] != NULL)
		{
			g_apTextureUiGaugeIcon[nCntTexture]->Release();
			g_apTextureUiGaugeIcon[nCntTexture] = NULL;		// 中身を空にする
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffUiGaugeIcon != NULL)
	{
		g_pVtxBuffUiGaugeIcon->Release();
		g_pVtxBuffUiGaugeIcon = NULL;		// 中身を空にする
	}
}

//========================================================================
// UIゲージアイコンの更新処理
//========================================================================
void UpdateUiGaugeIcon(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

	// =========================================================

#if 0
	if (GetKeyboardPress(DIK_I)) g_aUiGaugeIcon[2].pos.y -= 1.0f;
	if (GetKeyboardPress(DIK_K)) g_aUiGaugeIcon[2].pos.y += 1.0f;
	if (GetKeyboardPress(DIK_J)) g_aUiGaugeIcon[2].pos.x -= 1.0f;
	if (GetKeyboardPress(DIK_L)) g_aUiGaugeIcon[2].pos.x += 1.0f;
												
	if (GetKeyboardPress(DIK_U)) g_aUiGaugeIcon[2].fSizeLength += 1.0f;
	if (GetKeyboardPress(DIK_M)) g_aUiGaugeIcon[2].fSizeLength -= 1.0f;
#endif

	for (int nCntUiGaugeIcon = 0; nCntUiGaugeIcon < MAX_SET_UIGAUGEICON; nCntUiGaugeIcon++)
	{
		if (g_aUiGaugeIcon[nCntUiGaugeIcon].bUse == true)
		{// 使用している場合
			
			// UIゲージアイコンの状態による更新処理
			UpdateStateUiGaugeIcon(nCntUiGaugeIcon);

			if (GetInputStateUiGaugeIcon(g_aUiGaugeIcon[nCntUiGaugeIcon].nUser, g_aUiGaugeIcon[nCntUiGaugeIcon].input))
			{// 入力キーが押された場合

				if (g_aUiGaugeIcon[nCntUiGaugeIcon].state == UIGAUGEICON_STATE_FULL)
				{// ゲージ満タンの状態の場合

					g_aUiGaugeIcon[nCntUiGaugeIcon].state = UIGAUGEICON_STATE_RECAST;								// 状態をリキャスト状態に設定

					g_aUiGaugeIcon[nCntUiGaugeIcon].nCounterState = g_aUiGaugeIcon[nCntUiGaugeIcon].nMaxCntState;	// リキャスト時間を設定

					g_aUiGaugeIcon[nCntUiGaugeIcon].col = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);						// ポリゴンの色を灰色に設定			
				}
			}

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffUiGaugeIcon->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntUiGaugeIcon * 4);

			// 頂点座標の設定
			pVtx[0].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x - sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[1].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x + sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[2].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x - sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[3].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x + sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;

			pVtx[0].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y - cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[1].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y - cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[2].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y + cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[3].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y + cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// 頂点カラー
			pVtx[0].col = g_aUiGaugeIcon[nCntUiGaugeIcon].col;	// (R, G, B, A)
			pVtx[1].col = g_aUiGaugeIcon[nCntUiGaugeIcon].col;	// (R, G, B, A)
			pVtx[2].col = g_aUiGaugeIcon[nCntUiGaugeIcon].col;	// (R, G, B, A)
			pVtx[3].col = g_aUiGaugeIcon[nCntUiGaugeIcon].col;	// (R, G, B, A)

			// ▲頂点バッファをアンロックする
			g_pVtxBuffUiGaugeIcon->Unlock();

#if 0
			PrintDebugProc("\nPOS[%d] %f %f %f"		, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x, g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y, g_aUiGaugeIcon[nCntUiGaugeIcon].pos.z);
			PrintDebugProc("\nSIZE[%d] %f"			, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength);
			PrintDebugProc("\nSTATE[%d] %d"			, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].state);
			PrintDebugProc("\nSTATE_COUNTER[%d] %d"	, nCntUiGaugeIcon, g_aUiGaugeIcon[nCntUiGaugeIcon].nCounterState);
#endif
		}
	}
}

//========================================================================
// UIゲージアイコンの描画処理
//========================================================================
void DrawUiGaugeIcon(void)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// =========================================================

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffUiGaugeIcon, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntUiGaugeIcon = 0; nCntUiGaugeIcon < MAX_SET_UIGAUGEICON; nCntUiGaugeIcon++)
	{// 用意した分繰り返す

		if (g_aUiGaugeIcon[nCntUiGaugeIcon].bUse == true && g_aUiGaugeIcon[nCntUiGaugeIcon].bDisp == true)
		{// 使用していて表示している状態の場合

			// テクスチャの設定
			if (g_aUiGaugeIcon[nCntUiGaugeIcon].nIdxTexture != -1)
			{// テクスチャが設定されている場合
				
				pDevice->SetTexture(0, g_apTextureUiGaugeIcon[g_aUiGaugeIcon[nCntUiGaugeIcon].nIdxTexture]);
			}
			else
			{// テクスチャが設定されていない場合

				pDevice->SetTexture(0, NULL);
			}

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,		// プリミティブの種類
								   nCntUiGaugeIcon * 4,		// インデックス
								   2);						// プリミティブ（ポリゴン）数
		}
	}
}

//========================================================================
// UIゲージアイコンの設定処理
//========================================================================
void SetUiGaugeIcon(int nIdxTexture, int nUser, JOYKEY input,						
					D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeWidth, float fSizeHeight,
					UIGAUGEICON_STATE startState, int nCounterState)
{
	// 変数宣言 ===========================================

	float fLength, fAngle;	// 対角線の値

	// ====================================================

	for (int nCntUiGaugeIcon = 0; nCntUiGaugeIcon < MAX_SET_UIGAUGEICON; nCntUiGaugeIcon++)
	{
		if (g_aUiGaugeIcon[nCntUiGaugeIcon].bUse == false)
		{// 使用していない場合

			fLength = sqrtf((fSizeWidth * 2) * (fSizeWidth * 2) + (fSizeHeight * 2) * (fSizeHeight * 2)) * 0.5f;

			fAngle = atan2f((fSizeWidth * 2), (fSizeHeight * 2));

			g_aUiGaugeIcon[nCntUiGaugeIcon].nIdxTexture = nIdxTexture;		// テクスチャのインデックスを設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].nUser = nUser;					// 対象を設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].input = input;					// 入力キーを設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].pos = pos;						// 位置を設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].col = col;						// 色を設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength = fLength;			// 対角線の長さを設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle = fAngle;			// 対角線の角度を設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].state = startState;				// 状態を設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].nMaxCntState = nCounterState;	// 継続時間を設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].nCounterState = 0;				// 状態カウンタを初期化
			g_aUiGaugeIcon[nCntUiGaugeIcon].bDisp = true;					// 表示している状態に設定
			g_aUiGaugeIcon[nCntUiGaugeIcon].bUse = true;					// 使用してない状態に設定

			VERTEX_2D* pVtx;	// 頂点情報へのポインタを宣言

			// ▼頂点バッファをロックして頂点情報へのポインタを所得
			g_pVtxBuffUiGaugeIcon->Lock(0, 0, (void**)&pVtx, 0);

			pVtx += (nCntUiGaugeIcon * 4);

			// 頂点座標の設定
			pVtx[0].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x - sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[1].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x + sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[2].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x - sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[3].pos.x = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.x + sinf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;

			pVtx[0].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y - cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[1].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y - cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[2].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y + cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;
			pVtx[3].pos.y = g_aUiGaugeIcon[nCntUiGaugeIcon].pos.y + cosf(g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeAngle) * g_aUiGaugeIcon[nCntUiGaugeIcon].fSizeLength;

			pVtx[0].pos.z = 0.0f;
			pVtx[1].pos.z = 0.0f;
			pVtx[2].pos.z = 0.0f;
			pVtx[3].pos.z = 0.0f;

			// ▲頂点バッファをアンロックする
			g_pVtxBuffUiGaugeIcon->Unlock();

			break;
		}
	}
}

//========================================================================
// UIゲージアイコンの状態更新処理
//========================================================================
void UpdateStateUiGaugeIcon(int nIdxUiGaugeIcon)
{
	// 変数宣言 ================================================

	// デバイスの所得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// =========================================================

	if (g_aUiGaugeIcon[nIdxUiGaugeIcon].bUse == true)
	{// 使用している場合

		if (g_aUiGaugeIcon[nIdxUiGaugeIcon].nCounterState > 0)
		{// 状態カウンタが0より大きい場合

			g_aUiGaugeIcon[nIdxUiGaugeIcon].nCounterState--;	// カウンタを減らす
		}

		// ゲージの状態による処理
		switch (g_aUiGaugeIcon[nIdxUiGaugeIcon].state)
		{
		case UIGAUGEICON_STATE_FULL:		// ゲージ満タン状態



			break;

		case UIGAUGEICON_STATE_RECAST:		// リキャスト状態

			if (g_aUiGaugeIcon[nIdxUiGaugeIcon].nCounterState <= 0)
			{// カウンターが0になった

				g_aUiGaugeIcon[nIdxUiGaugeIcon].state = UIGAUGEICON_STATE_ENDRECAST;		// リキャスト終了状態に設定

				g_aUiGaugeIcon[nIdxUiGaugeIcon].nCounterState = UIGAUGEICON_ENDRECASTTIME;	// 継続時間を設定

				g_aUiGaugeIcon[nIdxUiGaugeIcon].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// ポリゴンの色を白色に設定
			}

			break;

		case UIGAUGEICON_STATE_ENDRECAST:	// リキャスト終了状態

			if (g_aUiGaugeIcon[nIdxUiGaugeIcon].nCounterState <= 0)
			{// カウンターが0になった

				g_aUiGaugeIcon[nIdxUiGaugeIcon].state = UIGAUGEICON_STATE_FULL;		// ゲージ満タン状態に設定

				g_aUiGaugeIcon[nIdxUiGaugeIcon].nCounterState = 0;					// 状態カウンタをリセット

				g_aUiGaugeIcon[nIdxUiGaugeIcon].bDisp = true;						// 表示している状態にする
			}
			else
			{// カウンターが0になってない

				g_aUiGaugeIcon[nIdxUiGaugeIcon].bDisp = (g_aUiGaugeIcon[nIdxUiGaugeIcon].bDisp == true) ? false : true;	// 表示状態を切り替える
			}

			break;
		}
	}
}

//========================================================================
// UIゲージアイコンの入力判定処理
//========================================================================
bool GetInputStateUiGaugeIcon(int nUser, JOYKEY Input)
{
	if (Input == JOYKEY_RIGHTTRIGGER || Input == JOYKEY_LEFTTRIGGER)
	{// 肩ボタンの入力の場合

		return GetJoypadShoulder(nUser, Input, NULL);
	}
	else if (Input == JOYKEY_LEFTSTICK		 || Input == JOYKEY_RIGHTSTICK
		  || Input == JOYKEY_LEFTSTICK_UP	 || Input == JOYKEY_LEFTSTICK_DOWN 
		  || Input == JOYKEY_LEFTSTICK_LEFT	 || Input == JOYKEY_LEFTSTICK_RIGHT
		  || Input == JOYKEY_RIGHTSTICK_UP	 || Input == JOYKEY_RIGHTSTICK_DOWN 
		  || Input == JOYKEY_RIGHTSTICK_LEFT || Input == JOYKEY_RIGHTSTICK_RIGHT)
	{// スティックの入力の場合

		return GetJoypadStick(nUser,Input, NULL, NULL);
	}
	else
	{// それ以外(ボタン)

		return GetJoypadTrigger(nUser, Input);
	}
}