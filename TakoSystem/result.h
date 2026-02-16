//=============================================================================
// 
// リザルトヘッダー [result.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _RESULT_H_		
#define _RESULT_H_		// 2重インクルード防止のマクロを定義

#include "esa.h"

// 列挙型の定義
typedef enum
{
	RESULTSTATE_BEGIN = 0,	// [0]開始状態
	
	RESULTSTATE_SETING,		// [1]設定状態
	RESULTSTATE_WAIT,		// [2]待機状態
	RESULTSTATE_COMPLETE,	// [3]完了状態
	
	RESULTSTAE_MAX			// 最大値

}RESULTSTATE;

typedef enum
{
	RESULTTYPE_RADIATION = 0,	// [0]放射線
	RESULTTYPE_PLAYER,

	RESULTTYPE_MAX
}RESULTTYPE;


// プロトタイプ宣言
void InitResult(void);		// リザルトの初期化処理
void UninitResult(void);	// リザルトの終了処理
void UpdateResult(void);	// リザルトの更新処理
void DrawResult(void);		// リザルトの描画処理

void SetResult
(RESULTTYPE type, D3DXVECTOR3 pos);

int SetResultPolygon		// リザルトのポリゴンの設定処理
(int nIdxTexture, bool bAlphaBlend, int nDrowLevel,		// テクスチャインデックス, aブレンドをするか
 D3DXVECTOR3 pos, D3DXVECTOR3 rot,						// 位置, 角度
 float fSizeWidth, float fSizeHeight,					// 大きさ(幅, 高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize,				// テクスチャ座標, テクスチャサイズ
 D3DXCOLOR col);										// 色

void ReceiveResult			// リザルトの情報を貰う処理
(int pHaveEsa[], int nMaxPlayer, int nMaxHave			// エサの情報, エサの種類の総数
 );

#endif