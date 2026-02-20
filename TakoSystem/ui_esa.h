//==================================================================================
// 
// エサのUIに関する処理[ui_esa.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _UI_ESA_H_
#define _UI_ESA_H_

// 構造体の定義 ================================================

// エサUIの状態
typedef enum
{
	UIESASTATE_NONE = 0,	// [0]何もない状態
	UIESASTATE_ADD,			// [1]アイテム欄に加わる状態
	UIESASTATE_SUB,			// [2]アイテム欄から消える状態

	UIESASUTATE_MAX			// 最大

}UIESASTATE;

// 構造体の定義 ================================================

// エサUI(親)の情報
typedef struct
{
	D3DXVECTOR3 pos;			// 位置(左上)
	
	int nNumNewLine;			// 改行するまでの数
	D3DXVECTOR2 space;			// 余白の大きさ
	float fEsaWidth;			// エサの大きさ(幅)
	float fEsaHeight;			// エサの大きさ(高さ)

	int nNumEsa;				// 親が持っているエサの総数
	int aIdxEsa[MAX_SET_ESA];	// 持っているエサのインデックス

	bool bUse;					// 使用状態

}UiEsaParent;

// プロトタイプ宣言 ============================================

void InitUiEsa(void);		// エサUIの初期化処理
void UninitUiEsa(void);		// エサUIの終了処理
void UpdateUiEsa(void);		// エサUIの更新処理
void DrawUiEsa(void);		// エサUIの描画処理

int SetUiEsaParent			// エサUIの親の設定処理
(D3DXVECTOR3 pos,						// 位置(左上端),
 int nNumNewLine, D3DXVECTOR2 space,	// 改行するまでの数, 余白の大きさ
 float fEsaWidth, float fEsaHeight);	// エサの大きさ(幅, 高さ)

int SetUiEsaChild			// エサUIの子の設定処理
(int nIdxType,							// 設定するエサの種類
 float fSizeWidth, float fSizeHeight);	// 大きさ(幅, 高さ)
 
void SetAddUiEsa			// エサUIのエサの追加処理
(int nIdxParent,						// 親のインデックス(プレイヤー)
 int nSetType);							// エサの種類

int SetSubUiEsa				// エサUIのエサの除外処理
(int nIdxParent);						// 親のインデックス(プレイヤー)

void FillSort				// 空白を詰めるソート処理
(int *pArray, int nArraySize);			// 並び替える配列, 配列の大きさ 

#endif