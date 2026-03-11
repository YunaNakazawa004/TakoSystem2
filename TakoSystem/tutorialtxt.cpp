//=============================================================================
// 
// チュートリアルテキスト [tutorialtxt.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "tutorialtxt.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TUTTXT				(30)									// 最大
#define MOVEMENT				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define CLEAR_COUNT				(30)									// クリア時のカウント
#define FADE_MINUS				(0.03f)									// フェード時の透明度

//*****************************************************************************
// チュートリアルテキスト構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXCOLOR col;				// 色
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	TUTTXTTYPE type;			// 種類
	TUTTXTSTATE state;			// 状態
	int nCounterState;			// 状態カウンター
	bool bClear;				// クリアしているか
	bool bUse;					// 使用しているか
}TutorialTxt;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureTutorialTxt[TUTTXTTYPE_MAX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTutorialTxt = NULL;			// 頂点バッファへのポインタ
TutorialTxt g_aTutorialTxt[MAX_TUTTXT];				// チュートリアルテキストの情報

//*****************************************************************************
// テクスチャファイル名
//*****************************************************************************
const char* c_apFilernamaTutorialTxt[TUTTXTTYPE_MAX] =
{
	"data\\TEXTURE\\tutorialtxt000.png",
	"data\\TEXTURE\\tutorialtxt001.png",
	"data\\TEXTURE\\tutorialtxt002.png",
	"data\\TEXTURE\\tutorialtxt003.png",
	"data\\TEXTURE\\tutorialtxt004.png",
	"data\\TEXTURE\\tutorialtxt005.png",
	"data\\TEXTURE\\tutorialtxt006.png",
	"data\\TEXTURE\\tutorialtxt007.png",
	"data\\TEXTURE\\tutorialtxt008.png",
	"data\\TEXTURE\\tutorialtxt009.png",
};

//=============================================================================
// チュートリアルテキストの初期化処理
//=============================================================================
void InitTutorialTxt(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < TUTTXTTYPE_MAX; nCntTexture++)
	{
		D3DXCreateTextureFromFile(pDevice, c_apFilernamaTutorialTxt[nCntTexture], &g_apTextureTutorialTxt[nCntTexture]);
	}

	// チュートリアルテキスト情報の初期化
	for (int nCntTxt = 0; nCntTxt < MAX_TUTTXT; nCntTxt++)
	{
		g_aTutorialTxt[nCntTxt].pos = FIRST_POS;
		g_aTutorialTxt[nCntTxt].col = WHITE_VTX;
		g_aTutorialTxt[nCntTxt].type = TUTTXTTYPE_CAMERA;
		g_aTutorialTxt[nCntTxt].state = TUTTXTSTATE_NONE;
		g_aTutorialTxt[nCntTxt].nCounterState = 0;
		g_aTutorialTxt[nCntTxt].bClear = false;
		g_aTutorialTxt[nCntTxt].bUse = false;
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MAX_VERTEX * MAX_TUTTXT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTutorialTxt,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTutorialTxt->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTxt = 0; nCntTxt < MAX_TUTTXT; nCntTxt++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-200.0f, 100.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(200.0f, 100.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-200.0f, -100.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(200.0f, -100.0f, 0.0f);

		// rhwの設定					 
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 頂点カラーの設定
		pVtx[0].col = g_aTutorialTxt[nCntTxt].col;
		pVtx[1].col = g_aTutorialTxt[nCntTxt].col;
		pVtx[2].col = g_aTutorialTxt[nCntTxt].col;
		pVtx[3].col = g_aTutorialTxt[nCntTxt].col;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTutorialTxt->Unlock();

	SetTutorialTxt(TUTTXTTYPE_POOL, TUTTXTSTATE_NONE, D3DXVECTOR3(0.0f, 400.0f, -700.0f));		// [8]渦潮について
	SetTutorialTxt(TUTTXTTYPE_RULE, TUTTXTSTATE_NONE, D3DXVECTOR3(0.0f, 200.0f, -2200.0f));		// [9]ルール
	SetTutorialTxt(TUTTXTTYPE_POT, TUTTXTSTATE_NONE, D3DXVECTOR3(0.0f, 300.0f, -2700.0f));		// [7]タコつぼ
	SetTutorialTxt(TUTTXTTYPE_ESA, TUTTXTSTATE_NONE, D3DXVECTOR3(0.0f, 200.0f, -3500.0f));		// [6]エサの取り方

	SetTutorialTxt(TUTTXTTYPE_INK, TUTTXTSTATE_NONE, D3DXVECTOR3(100.0f, 300.0f, -5000.0f));		// [5]墨吐き
	SetTutorialTxt(TUTTXTTYPE_TENT, TUTTXTSTATE_NONE, D3DXVECTOR3(-100.0f, 100.0f, -5000.0f));	// [4]触手伸ばし
	SetTutorialTxt(TUTTXTTYPE_UP, TUTTXTSTATE_NONE, D3DXVECTOR3(0.0f, 300.0f, -5500.0f));			// [3]上昇
	SetTutorialTxt(TUTTXTTYPE_DOWN, TUTTXTSTATE_NONE, D3DXVECTOR3(0.0f, 100.0f, -6200.0f));		// [2]下降
	SetTutorialTxt(TUTTXTTYPE_CAMERA, TUTTXTSTATE_DISP, D3DXVECTOR3(100.0f, 100.0f, -7000.0f));	// [1]カメラ
	SetTutorialTxt(TUTTXTTYPE_MOVE, TUTTXTSTATE_DISP, D3DXVECTOR3(-100.0f, 300.0f, -7000.0f));	// [0]移動
}

//=============================================================================
// チュートリアルテキストの終了処理
//=============================================================================
void UninitTutorialTxt(void)
{
	// テクスチャの破棄
	for (int nCntTutorialTxt = 0; nCntTutorialTxt < TUTTXTTYPE_MAX; nCntTutorialTxt++)
	{
		if (g_apTextureTutorialTxt[nCntTutorialTxt] != NULL)
		{
			g_apTextureTutorialTxt[nCntTutorialTxt]->Release();
			g_apTextureTutorialTxt[nCntTutorialTxt] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTutorialTxt != NULL)
	{
		g_pVtxBuffTutorialTxt->Release();
		g_pVtxBuffTutorialTxt = NULL;
	}
}

//=============================================================================
// チュートリアルテキストの更新処理
//=============================================================================
void UpdateTutorialTxt(void)
{
	VERTEX_3D* pVtx;					// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffTutorialTxt->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntTxt = 0; nCntTxt < MAX_TUTTXT; nCntTxt++)
	{
		if (g_aTutorialTxt[nCntTxt].bUse == true)
		{// 使用している
			switch (g_aTutorialTxt[nCntTxt].state)
			{
			case TUTTXTSTATE_NONE:
				g_aTutorialTxt[nCntTxt].col.a = 0.0f;

				break;

			case TUTTXTSTATE_DISP:
				g_aTutorialTxt[nCntTxt].col.a = 1.0f;

				break;

			case TUTTXTSTATE_CLEAR:
				if (g_aTutorialTxt[nCntTxt].nCounterState % 5 == 0)
				{// 一定間隔で点滅
					g_aTutorialTxt[nCntTxt].col = (g_aTutorialTxt[nCntTxt].col == WHITE_VTX) ? YELLOW_VTX : WHITE_VTX;
				}

				g_aTutorialTxt[nCntTxt].nCounterState--;

				if (g_aTutorialTxt[nCntTxt].nCounterState < 0)
				{// カウントがなくなったらフェードにする
					//g_aTutorialTxt[nCntTxt].state = TUTTXTSTATE_FADE;
					//g_aTutorialTxt[nCntTxt].nCounterState = 0;
					g_aTutorialTxt[nCntTxt].col = WHITE_VTX;
					g_aTutorialTxt[nCntTxt].col.a = 0.5f;
				}

				break;

			case TUTTXTSTATE_FADE:
				g_aTutorialTxt[nCntTxt].col.a -= FADE_MINUS;

				if (g_aTutorialTxt[nCntTxt].col.a <= 0.0f)
				{// 透明になった
					g_aTutorialTxt[nCntTxt].col.a = 0.0f;
					g_aTutorialTxt[nCntTxt].state = TUTTXTSTATE_NONE;
				}

				break;
			}

			// 頂点座標の設定
			if (g_aTutorialTxt[nCntTxt].type == TUTTXTTYPE_RULE)
			{// ルール
				pVtx[0].pos = D3DXVECTOR3(-90.0f, 160.0f, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(90.0f, 160.0f, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-90.0f, -160.0f, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(90.0f, -160.0f, 0.0f);
			}
			else
			{// 以外
				pVtx[0].pos = D3DXVECTOR3(-200.0f, 100.0f, 0.0f);
				pVtx[1].pos = D3DXVECTOR3(200.0f, 100.0f, 0.0f);
				pVtx[2].pos = D3DXVECTOR3(-200.0f, -100.0f, 0.0f);
				pVtx[3].pos = D3DXVECTOR3(200.0f, -100.0f, 0.0f);
			}

			// 頂点カラーの設定
			pVtx[0].col = g_aTutorialTxt[nCntTxt].col;
			pVtx[1].col = g_aTutorialTxt[nCntTxt].col;
			pVtx[2].col = g_aTutorialTxt[nCntTxt].col;
			pVtx[3].col = g_aTutorialTxt[nCntTxt].col;
		}

		pVtx += MAX_VERTEX;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTutorialTxt->Unlock();
}

//=============================================================================
// チュートリアルテキストの描画処理
//=============================================================================
void DrawTutorialTxt(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxTrans;				// 計算用マトリックス
	D3DXMATRIX mtxView;				// ビューマトリックス取得用

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Zテストを無効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	for (int nCntTxt = 0; nCntTxt < MAX_TUTTXT; nCntTxt++)
	{
		if (g_aTutorialTxt[nCntTxt].bUse == false)
		{// 使用してないなら無視
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aTutorialTxt[nCntTxt].mtxWorld);

		// ビューマトリックスを取得
		pDevice->GetTransform(D3DTS_VIEW, &mtxView);

		// ポリゴンをカメラに対して正面に向ける
		D3DXMatrixInverse(&g_aTutorialTxt[nCntTxt].mtxWorld, NULL, &mtxView);	// 逆行列を求める
		g_aTutorialTxt[nCntTxt].mtxWorld._41 = 0.0f;
		g_aTutorialTxt[nCntTxt].mtxWorld._42 = 0.0f;
		g_aTutorialTxt[nCntTxt].mtxWorld._43 = 0.0f;

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_aTutorialTxt[nCntTxt].pos.x, g_aTutorialTxt[nCntTxt].pos.y, g_aTutorialTxt[nCntTxt].pos.z);
		D3DXMatrixMultiply(&g_aTutorialTxt[nCntTxt].mtxWorld, &g_aTutorialTxt[nCntTxt].mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aTutorialTxt[nCntTxt].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_pVtxBuffTutorialTxt, 0, sizeof(VERTEX_3D));

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureTutorialTxt[g_aTutorialTxt[nCntTxt].type]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCntTxt * MAX_VERTEX,		// 描画する最初の頂点インデックス
			MAX_POLYGON);		// 描画するプリミティブ数
	}

	// Zテストを有効にする
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}

//=============================================================================
// チュートリアルテキストの設定
//=============================================================================
void SetTutorialTxt(TUTTXTTYPE type, TUTTXTSTATE state, D3DXVECTOR3 pos)
{
	for (int nCntTxt = 0; nCntTxt < MAX_TUTTXT; nCntTxt++)
	{
		if (g_aTutorialTxt[nCntTxt].bUse == false)
		{// 使用してない
			g_aTutorialTxt[nCntTxt].pos = pos;
			g_aTutorialTxt[nCntTxt].col = WHITE_VTX;
			g_aTutorialTxt[nCntTxt].type = type;
			g_aTutorialTxt[nCntTxt].state = state;

			g_aTutorialTxt[nCntTxt].nCounterState = 0;
			g_aTutorialTxt[nCntTxt].bUse = true;

			break;
		}
	}
}

//=============================================================================
// チュートリアルテキストの状態設定
//=============================================================================
void SetTutorialTxtState(TUTTXTTYPE type, TUTTXTSTATE state)
{
	int nIdx = GetTutorialTxtFromType(type);

	if (nIdx == -1)
	{// 範囲外
		return;
	}

	if (g_aTutorialTxt[nIdx].state == state || g_aTutorialTxt[nIdx].bClear == true)
	{// 同じ状態は入れない
		return;
	}

	if (state == TUTTXTSTATE_CLEAR)
	{// クリア条件
		if (g_aTutorialTxt[nIdx].state == TUTTXTSTATE_DISP)
		{// 見えてる状態じゃないとクリア判定にならない
			g_aTutorialTxt[nIdx].state = state;

			PlaySound(SOUND_SE_CLEAR);
		}
		else
		{// 見えてない場合
			return;
		}
	}
	else
	{// 以外
		g_aTutorialTxt[nIdx].state = state;
	}

	if (state == TUTTXTSTATE_CLEAR)
	{// クリア状態に設定されたら
		g_aTutorialTxt[nIdx].nCounterState = CLEAR_COUNT;

		switch (type)
		{
		case TUTTXTTYPE_MOVE:
			g_aTutorialTxt[nIdx].bClear = true;

			if (g_aTutorialTxt[GetTutorialTxtFromType(TUTTXTTYPE_CAMERA)].bClear == true)
			{// カメラもクリアしてたら
				SetTutorialTxtState(TUTTXTTYPE_DOWN, TUTTXTSTATE_DISP);
			}

			break;

		case TUTTXTTYPE_CAMERA:
			g_aTutorialTxt[nIdx].bClear = true;

			if (g_aTutorialTxt[GetTutorialTxtFromType(TUTTXTTYPE_MOVE)].bClear == true)
			{// カメラもクリアしてたら
				SetTutorialTxtState(TUTTXTTYPE_DOWN, TUTTXTSTATE_DISP);
			}

			break;

		case TUTTXTTYPE_DOWN:
			g_aTutorialTxt[nIdx].bClear = true;

			SetTutorialTxtState(TUTTXTTYPE_UP, TUTTXTSTATE_DISP);

			break;

		case TUTTXTTYPE_UP:
			g_aTutorialTxt[nIdx].bClear = true;

			SetTutorialTxtState(TUTTXTTYPE_TENT, TUTTXTSTATE_DISP);
			SetTutorialTxtState(TUTTXTTYPE_INK, TUTTXTSTATE_DISP);

			break;

		case TUTTXTTYPE_TENT:
			g_aTutorialTxt[nIdx].bClear = true;

			if (g_aTutorialTxt[GetTutorialTxtFromType(TUTTXTTYPE_INK)].bClear == true)
			{// カメラもクリアしてたら
				SetTutorialTxtState(TUTTXTTYPE_ESA, TUTTXTSTATE_DISP);
			}

			break;

		case TUTTXTTYPE_INK:
			g_aTutorialTxt[nIdx].bClear = true;

			if (g_aTutorialTxt[GetTutorialTxtFromType(TUTTXTTYPE_TENT)].bClear == true)
			{// カメラもクリアしてたら
				SetTutorialTxtState(TUTTXTTYPE_ESA, TUTTXTSTATE_DISP);
			}

			break;

		case TUTTXTTYPE_ESA:
			g_aTutorialTxt[nIdx].bClear = true;

			SetTutorialTxtState(TUTTXTTYPE_POT, TUTTXTSTATE_DISP);

			break;

		case TUTTXTTYPE_POT:
			g_aTutorialTxt[nIdx].bClear = true;

			SetTutorialTxtState(TUTTXTTYPE_POOL, TUTTXTSTATE_DISP);
			SetTutorialTxtState(TUTTXTTYPE_RULE, TUTTXTSTATE_DISP);

			break;
		}
	}
}

//=============================================================================
// チュートリアルテキストの種類からインデックスを返す
//=============================================================================
int GetTutorialTxtFromType(TUTTXTTYPE type)
{
	int nIdx = -1;

	for (int nCntTxt = 0; nCntTxt < MAX_TUTTXT; nCntTxt++)
	{
		if (g_aTutorialTxt[nCntTxt].type == type && g_aTutorialTxt[nCntTxt].bUse == true)
		{// 一致
			nIdx = nCntTxt;

			break;
		}
	}

	return nIdx;
}