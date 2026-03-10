//==================================================================================
// 
// チュートリアルのUIに関する処理[ui_tutorial.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _UI_TUTORIAL_H_
#define _UI_TUTORIAL_H_

#include "input.h"

// 列挙型の定義 ================================================

// チュートリアルUIの種類
typedef enum
{
	UI_TUTORIALTYPE_NULL = 0,		// [0]無し
	UI_TUTORIALTYPE_SKIPTEXT,		// [1]開始テキスト
	UI_TUTORIALTYPE_SKIPGAUGE,		// [2]開始ゲージ

	UI_TUTORIALTYPE_MAX				// 最大値

}UI_TUTORIALTYPE;

// チュートリアルUIのPOSの位置
typedef enum
{
	UI_TUTORIALPOS_CENTER = 0,		// [0]中心
	UI_TUTORIALPOS_TOP,				// [1]上
	UI_TUTORIALPOS_BOT,				// [2]下
	UI_TUTORIALPOS_LEFT,			// [3]左
	UI_TUTORIALPOS_RIGHT,			// [4]右
	UI_TUTORIALPOS_TOPLEFT,			// [5]左上
	UI_TUTORIALPOS_TOPRIGHT,		// [6]右上
	UI_TUTORIALPOS_BOTLEFT,			// [7]左下
	UI_TUTORIALPOS_BOTRIGHT,		// [8]右下

	UI_TUTORIALPOS_MAX				// 最大値

}UI_TUTORIALPOS;

// プロトタイプ宣言 ============================================

void InitUiTutorial(void);		// チュートリアルUIの初期化処理
void UninitUiTutorial(void);	// チュートリアルUIの終了処理
void UpdateUiTutorial(void);	// チュートリアルUIの更新処理
void DrawUiTutorial(void);		// チュートリアルUIの描画処理

void SetUiTutorialPolygon		// チュートリアルUIのポリゴン設定処理
(UI_TUTORIALTYPE type, UI_TUTORIALPOS posPosion,	// 種類, POSの位置
 int nIdxTexture, bool bAlphaBrend,					// テクスチャインデックス, アルファブレンドするか
 D3DXVECTOR3 pos,									// 位置,
 float fSizeWidth, float fSizeHeight,				// 大きさ(幅), 大きさ(高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize,			// テクスチャ座標, テクスチャサイズ
 D3DXCOLOR col);									// 色

void SetVtxUiTutorial			// チュートリアルUIの頂点設定処理
(int nStartVtx,										// 描画開始頂点
 UI_TUTORIALPOS posPsion,							// POSの位置
 D3DXVECTOR3 pos,									// 位置
 float fSizeWidth, float fSizeHeight,				// 大きさ(幅), 大きさ(高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize, 			// テクスチャ座標, テクスチャサイズ
 D3DXCOLOR col);									// 色

bool GetSkipTutorial(void);		// チュートリアルUIのスキップするかを返す処理

#endif
