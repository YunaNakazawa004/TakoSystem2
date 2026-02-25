//=============================================================================
// 
// ロゴ [logo.cpp]
// Author : 井上 祐一
// 
//=============================================================================
#include "main.h"
#include "title.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "camera.h"

// マクロ定義
#define	MAX_LOGO			(2)							// ロゴで表示するテクスチャの最大数
#define	LOGO_SIZEUP_SPEED	(1.0f)						// ロゴが大きくなるサイズ
#define	LOGO_SIZE_MAX		(15.0f)					// ロゴの最大サイズ
#define	TITLE_TRANS			(LOGO_SIZE_MAX + 50.0f)		// タイトルまでの時間

// 列挙型の定義
typedef enum
{
	LOGOTYPE_NULL = -1, // [-1]何もない
	LOGOTYPE_BG,        // [0]背景
	LOGOTYPE_LOGO,		// [1]ロゴ

	LOGOTYPE_MAX		// 最大値

}LOGOTYPE;

// 構造体の定義
typedef struct
{
	LOGOTYPE type;		// 種類

	D3DXVECTOR3 pos;	// 位置
	float fSizeWidth;	// 大きさ
	float fSizeHeight;	// 大きさ

	D3DXCOLOR col;		// 色

	bool bUse;			// 使用状況

}LogoPolygon;

// プロトタイプ宣言
void SetLogoPolygon(LOGOTYPE type, D3DXVECTOR3 pos, float fSizeWidth, float fSizeHeight, D3DXCOLOR col);

// グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureLogo[MAX_LOGO] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffLogo = NULL;		// 頂点バッファへのポインタ

LogoPolygon g_aLogoPolygon[MAX_LOGO];				// ロゴポリゴンの情報

float g_fLogoDeley;		// ロゴの表示時間
float g_fLogoSizeMove;	// ロゴのサイズ変化
float g_fLogoOut;		// ロゴの透明化

// ファイル名
const char* c_apFilenameLogo[] =
{
	"data/TEXTURE/In_the_sea.png",
	"data/TEXTURE/LOGO.png",
};

//===================================================================
// ロゴの初期化処理
//===================================================================
void InitLogo(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ

	// テクスチャの読み込み
	for (int nCntTexture = 0; nCntTexture < sizeof c_apFilenameLogo / sizeof(c_apFilenameLogo[0]); nCntTexture++)
	{
		if (FAILED(D3DXCreateTextureFromFile(pDevice,
			c_apFilenameLogo[nCntTexture],
			&g_pTextureLogo[nCntTexture])))
		{// 読み込みに失敗

			continue;	// 処理のはじめに戻る
		}
	}

	// ロゴポリゴンの初期化
	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{
		g_aLogoPolygon[nCntLogo].type = LOGOTYPE_NULL;						// 種類を初期化
		g_aLogoPolygon[nCntLogo].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		// 位置を初期化
		g_aLogoPolygon[nCntLogo].fSizeWidth = 0.0f;							// 大きさを初期化
		g_aLogoPolygon[nCntLogo].fSizeHeight = 0.0f;						// 大きさを初期化
		g_aLogoPolygon[nCntLogo].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 色を初期化
		g_aLogoPolygon[nCntLogo].bUse = false;								// 使用していない状況に設定
	}

	g_fLogoDeley = 0.0f;	// ロゴの値を初期化
	g_fLogoSizeMove = 0.0f;	// ロゴの大きさを初期化
	g_fLogoOut = 0.0f;		// ロゴの透明度を初期化

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_LOGO,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffLogo,
		NULL);

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffLogo->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 右回りで設定する
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 2Dの場合Zの値は0にする
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// 0~255の値を設定
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;	// 値は1.0fで固定
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// テクスチャ
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffLogo->Unlock();

	// ロゴポリゴンの設定(背景)
	SetLogoPolygon(LOGOTYPE_BG, D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f),
				   SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f));

	// ロゴポリゴンの設定(ロゴ)
	SetLogoPolygon(LOGOTYPE_LOGO, D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f),
				   100.0f, 100.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	PlaySound(SOUND_SE_BOOT);	// 起動音
}

//===================================================================
// ロゴの終了処理
//===================================================================
void UninitLogo(void)
{
	// テクスチャの破棄
	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{// ロゴの数だけ確認する

		if (g_pTextureLogo[nCntLogo] != NULL)
		{// テクスチャの破棄

			g_pTextureLogo[nCntLogo]->Release();
			g_pTextureLogo[nCntLogo] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffLogo != NULL)
	{
		g_pVtxBuffLogo->Release();
		g_pVtxBuffLogo = NULL;
	}
}

//===================================================================
// ロゴの更新処理
//===================================================================
void UpdateLogo(void)
{
	// フェード情報の取得
	FADE pFade = GetFade();

	D3DXVECTOR3 setPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 設定位置
	float fSetWidth = 0.0f, fSetHeight = 0.0f;			// 設定サイズ
	float fSetColAlpha = 0.0f;							// 設定アルファ値

	if (g_fLogoDeley < LOGO_SIZE_MAX)
	{// ロゴの大きさが規定値より小さい場合

		g_fLogoSizeMove += LOGO_SIZEUP_SPEED;	// ロゴの大きさを大きくする
	}
	else
	{// ロゴをフェードアウト

		g_fLogoOut += LOGO_SIZEUP_SPEED * 0.02f;
		g_fLogoSizeMove += LOGO_SIZEUP_SPEED;
	}

	g_fLogoDeley += LOGO_SIZEUP_SPEED;

	VERTEX_2D* pVtx;	// 頂点情報へのポインタ

	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{
		if (g_aLogoPolygon[nCntLogo].bUse == false)
		{// 使用されていない場合

			continue;	// 処理のはじめに戻る
		}

		// 種類別の各値の設定
		switch (g_aLogoPolygon[nCntLogo].type)
		{
		case LOGOTYPE_BG:		// 背景

			setPos = g_aLogoPolygon[nCntLogo].pos;									// 位置を設定
			fSetWidth = g_aLogoPolygon[nCntLogo].fSizeWidth;						// 大きさを設定
			fSetHeight = g_aLogoPolygon[nCntLogo].fSizeHeight;						// 大きさを設定

			fSetColAlpha = 1.0f;													// アルファ値を設定

			break;

		case LOGOTYPE_LOGO:		// ロゴ

			setPos = g_aLogoPolygon[nCntLogo].pos;									// 位置を設定
			fSetWidth = g_aLogoPolygon[nCntLogo].fSizeWidth + g_fLogoSizeMove;		// 大きさを設定
			fSetHeight = g_aLogoPolygon[nCntLogo].fSizeHeight + g_fLogoSizeMove;	// 大きさを設定

			fSetColAlpha = 1.0f - g_fLogoOut;										// アルファ値を設定

			break;
		}

		// 頂点バッファをロックし、頂点情報へのポインタを取得
		g_pVtxBuffLogo->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nCntLogo * 4);	// 頂点ポインタをインデックスの先頭まで進める

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(setPos.x - fSetWidth, setPos.y - fSetHeight, 0.0f);		// 右回りで設定する
		pVtx[1].pos = D3DXVECTOR3(setPos.x + fSetWidth, setPos.y - fSetHeight, 0.0f);		// 2Dの場合Zの値は0にする
		pVtx[2].pos = D3DXVECTOR3(setPos.x - fSetWidth, setPos.y + fSetHeight, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(setPos.x + fSetWidth, setPos.y + fSetHeight, 0.0f);

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(g_aLogoPolygon[nCntLogo].col.r, g_aLogoPolygon[nCntLogo].col.g, g_aLogoPolygon[nCntLogo].col.b, fSetColAlpha);	// 0~255の値を設定
		pVtx[1].col = D3DXCOLOR(g_aLogoPolygon[nCntLogo].col.r, g_aLogoPolygon[nCntLogo].col.g, g_aLogoPolygon[nCntLogo].col.b, fSetColAlpha);
		pVtx[2].col = D3DXCOLOR(g_aLogoPolygon[nCntLogo].col.r, g_aLogoPolygon[nCntLogo].col.g, g_aLogoPolygon[nCntLogo].col.b, fSetColAlpha);
		pVtx[3].col = D3DXCOLOR(g_aLogoPolygon[nCntLogo].col.r, g_aLogoPolygon[nCntLogo].col.g, g_aLogoPolygon[nCntLogo].col.b, fSetColAlpha);

		// 頂点バッファをアンロックする
		g_pVtxBuffLogo->Unlock();
	}

#if 1
	if ((GetKeyboardTrigger(DIK_RETURN) == true 		// キーボードの[ENTER]が押された
		|| GetJoypadTrigger(0, JOYKEY_START) == true 	// ジョイパッドの[START]が押された
		|| GetJoypadTrigger(0, JOYKEY_A) == true))		// ジョイパッドの[A]が押された
	{// 特定のキーを押すと指定のモードに画面転移

		// フェードの設定
		SetFade(MODE_TITLE);
	}
#endif

	if (pFade == FADE_NONE && g_fLogoDeley > TITLE_TRANS)
	{// フェードしてない && 規定の大きさを超えた

		// フェードの設定
		SetFade(MODE_TITLE);
	}
}

//===================================================================
// ロゴの描画処理
//===================================================================
void DrawLogo(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();;	// デバイスへのポインタ

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffLogo, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{// 敵の最大数まで繰り返す

		if (g_aLogoPolygon[nCntLogo].bUse == false)
		{// 使用されていない場合

			continue;	// 処理のはじめに戻る
		}

		// テクスチャの設定
		if (g_aLogoPolygon[nCntLogo].type > LOGOTYPE_NULL
			&& g_aLogoPolygon[nCntLogo].type < LOGOTYPE_MAX)
		{// 設定するテクスチャがある場合

			pDevice->SetTexture(0, g_pTextureLogo[g_aLogoPolygon[nCntLogo].type]);
		}
		else
		{// 設定するテクスチャがない場合

			pDevice->SetTexture(0, NULL);
		}

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,     // 形式
			nCntLogo * 4,            // インデックス
			2);						// プリミティブ数
	}
}

//===================================================================
// ロゴの設定処理
//===================================================================
void SetLogoPolygon(LOGOTYPE type, D3DXVECTOR3 pos, float fSizeWidth, float fSizeHeight, D3DXCOLOR col)
{
	for (int nCntLogo = 0; nCntLogo < MAX_LOGO; nCntLogo++)
	{
		if (g_aLogoPolygon[nCntLogo].bUse == true)
		{// 使用している場合

			continue;	// 処理のはじめに戻る
		}

		// ロゴの各情報を設定
		g_aLogoPolygon[nCntLogo].type = type;                  	// 種類を設定
		g_aLogoPolygon[nCntLogo].pos = pos;    					// 位置を設定
		g_aLogoPolygon[nCntLogo].fSizeWidth = fSizeWidth;       // 大きさを設定
		g_aLogoPolygon[nCntLogo].fSizeHeight = fSizeHeight;     // 大きさを設定
		g_aLogoPolygon[nCntLogo].col = col;  					// 色を設定
		g_aLogoPolygon[nCntLogo].bUse = true;					// 使用している状況に設定

		break;	// for文を抜ける
	}
}
