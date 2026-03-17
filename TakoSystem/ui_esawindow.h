//==================================================================================
// 
// エサUIのウィンドウに関する処理[ui_esawindow.h]
// Author : Soki Inamura
// 
//==================================================================================

#ifndef _UI_ESAWINDOW_H_
#define _UI_ESAWINDOW_H_

// 列挙型の定義 ================================================

// エサUIのウィンドウのPOSの位置
typedef enum
{
	UIESAWINDOWTYPEPOS_CENTER = 0,		// [0]中心
	UIESAWINDOWTYPEPOS_TOP,				// [1]上
	UIESAWINDOWTYPEPOS_BOT,				// [2]下
	UIESAWINDOWTYPEPOS_LEFT,			// [3]左
	UIESAWINDOWTYPEPOS_RIGHT,			// [4]右
	UIESAWINDOWTYPEPOS_TOPLEFT,			// [5]左上
	UIESAWINDOWTYPEPOS_TOPRIGHT,		// [6]右上
	UIESAWINDOWTYPEPOS_BOTLEFT,			// [7]左下
	UIESAWINDOWTYPEPOS_BOTRIGHT,		// [8]右下

	UIESAWINDOWTYPEPOS_MAX				// 最大値

}UIESAWINDOWTYPEPOS;

// エサUIのウィンドウの種類
typedef enum
{
	UIESAWINDOWTYPE_NONE = -1,		// [-1]無し
	
	UIESAWINDOWTYPE_LAY_WINDOW,		// [0]無し
	UIESAWINDOWTYPE_LAY_LINE,		// [1]
	UIESAWINDOWTYPE_LAY_ESATEX,		// [2]エサテクスチャのレイアウト
	
	UIESAWINDOWTYPE_ESATEX,			// [3]エサテクスチャ
	UIESAWINDOWTYPE_WINDOWLEG,		// [4]ウィンドウの足


	UIESAWINDOWTYPE_MAX				// 最大値

}UIESAWINDOWTYPE;

// プロトタイプ宣言 ============================================

void InitUiEsaWindow(void);		// エサUIのウィンドウの入力処理
void UninitUiEsaWindow(void);	// エサUIのウィンドウの終了処理
void UpdateUiEsaWindow(void);	// エサUIのウィンドウの更新処理
void DrawUiEsaWindow(void);		// エサUIのウィンドウの描画処理

void SetVtxUiEsaWindowPolygon	// エサUIのウィンドウポリゴンの頂点設定処理
(int nIdx, D3DXVECTOR3 pos, float fSizeScalar);

void SetUiEsaWindow
(int nIdx, D3DXVECTOR3 pos);

int SetUiEsaWindowGroup
(int nIdxParent, UIESAWINDOWTYPE layType, D3DXVECTOR3 pos, float fSizeScalar);

int SetUiEsaWindowPolygon		// エサUIのウィンドウポリゴンの設定処理
(UIESAWINDOWTYPEPOS posType, UIESAWINDOWTYPE type,
 int nIdxTexture, bool bAlphaBlend, 
 D3DXVECTOR3 pos, float fSizeWidth, float fSizeHeight,
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, D3DXCOLOR col);




#endif