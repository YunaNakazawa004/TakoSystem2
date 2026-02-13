//==================================================================================
// 
// リザルトの獲得スコアのUIに関する処理[ui_result_getscore.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _UI_RESULT_GETSCORE_H_
#define _UI_RESULT_GETSCORE_H_

// 列挙型の定義 ================================================

typedef enum
{
	UI_RESULTGSPOLYGONTYPE_NONE = 0,	// [0]特に無し
	UI_RESULTGSPOLYGONTYPE_BG,			// [1]背景
	UI_RESULTGSPOLYGONTYPE_NUM,			// [2]数字

	UI_RESULTGSPOLYGONTYPE_MAX			// 最大値

}UI_RESULTGSPOLYGONTYPE;

// プロトタイプ宣言 ============================================

void InitUiResultGetScore(void);		// リザルトの獲得スコアのUIの初期化処理
void UninUiResultGetScore(void);		// リザルトの獲得スコアのUIの終了処理
void UpdateUiResultGetScore(void);		// リザルトの獲得スコアのUIの更新処理
void DrawUiResultGetScore(void);		// リザルトの獲得スコアのUIの描画処理

int SetUiResultGetScorePolygon			// リザルトの獲得スコアのUIのポリゴンの設定処理
(int nIdxTexture, bool bAlphaBlend, UI_RESULTGSPOLYGONTYPE type,
 D3DXVECTOR3 pos, D3DXVECTOR3 rot,
 float fSizeWidth, float fSizeHeight,
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize,
 D3DXCOLOR col);

int SetUiResultGetScore
(D3DXVECTOR3 pos, int nPoint, int nNumHave);

#endif

