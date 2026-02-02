//==================================================================================
// 
// エサのUIに関する処理[ui_esa.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _UI_ESA_H_
#define _UI_ESA_H_

// プロトタイプ宣言 ============================================

void InitUiEsa(void);		// UIエサの初期化処理
void UninitUiEsa(void);		// UIエサの終了処理
void UpdateUiEsa(void);		// UIエサの更新処理
void DrawUiEsa(void);		// UIエサの描画処理

void SetUiEsa				// UIエサの設定処理
(int nIdxType, D3DXVECTOR3 pos);

#endif