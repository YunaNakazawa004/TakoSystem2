//=============================================================================
// 
// リザルトヘッダー [result.h]
// Author : 井上 祐一
// 
//=============================================================================
#ifndef _RESULT2_H_		
#define _RESULT2_H_		// 2重インクルード防止のマクロを定義

#include "esa.h"

// 列挙型の定義
typedef enum
{
	RESULTSTATE_BEGIN = 0,	// [0]開始状態
	RESULTSTAE_WAIT,		// [1]待機状態

	RESULTSTAE_MAX			// 最大値

}RESULTSTATE;

typedef enum
{
	RESULTTYPE_RADIATION = 0,	// [0]放射線
	
	RESULTTYPE_ESA_START,
	RESULTTYPE_ESA_STOP,
	RESULTTYPE_ESA_END,

	RESULTTYPE_MAX
}RESULTTYPE;


// プロトタイプ宣言
void InitResult2(void);		// リザルトの初期化処理
void UninitResult2(void);	// リザルトの終了処理
void UpdateResult2(void);	// リザルトの更新処理
void DrawResult2(void);		// リザルトの描画処理

int SetResultPolygon2		// リザルトのポリゴンの設定処理
(int nIdxTexture, bool bAlphaBlend,			// テクスチャインデックス, aブレンドをするか
 float fSizeWidth, float fSizeHeight,		// 大きさ(幅, 高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize,	// テクスチャ座標, テクスチャサイズ
 D3DXCOLOR col);							// 色

int SetResult2
(bool bPolygon, int nIdx, int nDrowLevel,	// ポリゴンか, インデックス, 描画順位
 D3DXVECTOR3 pos, D3DXVECTOR3 rot);			// 位置, 角度

void ReceiveResult2			// リザルトの情報を貰う処理
(bool bGame,									// ゲームから情報を渡しているか
 int* nHaveEsa, int nMaxPlayer, int nMaxHave,	// 持っているエサ, プレイヤーの総数, 所持できる数
 EsaData *pEsaData, int nMaxEsaType				// エサの情報, エサの種類の総数
 );

#endif