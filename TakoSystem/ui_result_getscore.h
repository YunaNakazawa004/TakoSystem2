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
	UIRESULTGS_STATE_BIGIN = 0,	// [0]開始数集計状態(全てのスコアUIの背景が一定の長さになったか)
	UIRESULTGS_STATE_TALLY,		// [1]集計数集計状態(全てのスコアUIのスコアの集計が終わったか)
	UIRESULTGS_STATE_TALLY_END,	// [2]集計終了数集計状態(全てのスコアUIのスコアの集計が終わって一定時間たったか)
	UIRESULTGS_STATE_END,		// [3]終了数集計状態(全てのスコアUIの背景が一定の長さになったか)

	UIRESULTGS_STATE_MAX		// 最大値

}UIRESULTGS_STATE;

typedef enum
{
	UI_RESULTGSPOLYGONTYPE_NONE = 0,	// [0]特に無し
	UI_RESULTGSPOLYGONTYPE_BG,			// [1]背景
	UI_RESULTGSPOLYGONTYPE_NUM_SCORE,	// [2]数字(獲得スコア)
	UI_RESULTGSPOLYGONTYPE_NUM_GET,		// [3]数字(所持数)

	UI_RESULTGSPOLYGONTYPE_MAX			// 最大値

}UI_RESULTGSPOLYGONTYPE;

// プロトタイプ宣言 ============================================

void InitUiResultGetScore(void);		// リザルトの獲得スコアのUIの初期化処理
void UninitUiResultGetScore(void);		// リザルトの獲得スコアのUIの終了処理
void UpdateUiResultGetScore(void);		// リザルトの獲得スコアのUIの更新処理
void DrawUiResultGetScore(void);		// リザルトの獲得スコアのUIの描画処理

void UpdateVtxUiResultGetScore			// リザルトの獲得スコアのUIの頂点更新処理 
(int nIdxP, int nIdxC);

void SetDrawUiResultGetScore
(int nIdx);

int SetUiResultGetScorePolygon			// リザルトの獲得スコアのUIのポリゴンの設定処理
(int nIdxTexture, bool bAlphaBlend, UI_RESULTGSPOLYGONTYPE type,
 D3DXVECTOR3 pos, D3DXVECTOR3 rot,
 float fSizeWidth, float fSizeHeight,
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize,
 D3DXCOLOR col);

int SetUiResultGetScore					// リザルトの獲得スコアのUIの設定処理
(D3DXVECTOR3 pos, D3DXCOLOR col, int nPoint, int nNumHave);

void DelUiResultGetScore
(int nIdx);

int CalcNumDigit						// 桁数を求める処理
(int nNum);

void CalcDigit							// 桁の値を求める処理
(int nNum, int nDigit, int* pTexU, int nSizeTexU);

bool GetCompletGetScore					// 完了したかを返す処理
(UIRESULTGS_STATE state);			

#endif

