//=============================================================================
// 
// リザルトヘッダー [result.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _RESULT_H_		
#define _RESULT_H_		// 2重インクルード防止のマクロを定義

#include "esa.h"

// 列挙型の定義 ================================================

// リザルトの状態
typedef enum
{
	RESULTSTATE_BEGIN = 0,	// [0]開始状態
	
	RESULTSTATE_SETING,		// [1]設定状態
	RESULTSTATE_WAIT,		// [2]待機状態
	RESULTSTATE_COMPLETE,	// [3]完了状態
	
	RESULTSTAE_MAX			// 最大値

}RESULTSTATE;

// リザルトの設定種類
typedef enum
{
	RESULTTYPE_PLAYER = 0,	// 

	RESULTTYPE_MAX

}RESULTTYPE;

// ランキングに渡すプレイヤーの種類
typedef enum
{
	RESULT_PLAYER_NONE = -1,	// [-1]無し
	RESULT_PLAYER_PLAYER,		// [ 0]プレイヤー
	RESULT_PLAYER_COMPUTER,		// [ 1]コンピューター

	RESULT_PLAYER_MAX			// 最大値

}RESULT_TYPEPLAYER;

// 構造体の定義 ================================================


typedef struct
{
	RESULT_TYPEPLAYER typePlayer;
	
	int nTotalScore;

}GiveResultPlayer;


// プロトタイプ宣言 ============================================

void InitResult(void);		// リザルトの初期化処理
void UninitResult(void);	// リザルトの終了処理
void UpdateResult(void);	// リザルトの更新処理
void DrawResult(void);		// リザルトの描画処理

void SetResult
(RESULTTYPE type, D3DXVECTOR3 pos);

int SetResultPolygon						// リザルトのポリゴンの設定処理
(int nIdxTexture, bool bAlphaBlend, int nDrowLevel,			// テクスチャインデックス, aブレンドをするか
 D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 addRot,		// 位置, 角度
 float fSizeWidth, float fSizeHeight,						// 大きさ(幅, 高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize,					// テクスチャ座標, テクスチャサイズ
 D3DXCOLOR col);											// 色

void ReceiveResult							// リザルトの情報を貰う処理
(RESULT_TYPEPLAYER* pTypePlayer, int pHaveEsa[], 			// プレイヤーの種類, エサの情報
 int nMaxPlayer, int nMaxHave);								// プレイヤーの種類の大きさ, プレイヤーの総数, エサの種類の総数 
 
GiveResultPlayer* GetRankingForResult	// リザルトからランキングに情報を渡す処理
(RESULT_TYPEPLAYER *pTypePlayer,int nDataSizeTypePlayer,	// プレイヤーの種類情報(情報を入れる場所, その場所の大きさ)
 int *pTotalScore, int nDataSizeTotalScore);				// スコアのトータル情報(情報を入れる場所, その場所の大きさ)

#endif