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
	RESULTSTATE_BEGIN = 0,	// [0]開始状態(集計開始の表示)

	RESULTSTATE_SETING,		// [1]設定状態(獲得状況の設定)
	RESULTSTATE_WAIT,		// [2]待機状態(獲得状況の表示)
	RESULTSTATE_COMPLETE,	// [3]完了状態(全獲得状況表示終了状態)

	RESULTSTAE_MAX			// 最大値

}RESULTSTATE;

// リザルトの配置種類
typedef enum
{
	RESULTLAYTYPE_NULL = -1,		// [-1]無し
	RESULTLAYTYPE_INIT,				// [ 0]初期配置
	RESULTLAYTYPE_STARTTALLTING,	// [ 1]集計開始
	RESULTLAYTYPE_ESA,				// [ 2]エサ
	RESULTLAYTYPE_PLAYER,			// [ 3]プレイヤー

	RESULTLAYTYPE_MAX				// 最大値

}RESULTLAYTYPE;

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
	RESULT_TYPEPLAYER typePlayer;	// プレイヤーの種類
	
	int nTotalScore;				// プレイヤーのスコア

}GiveResultPlayer;

// プロトタイプ宣言 ============================================

void InitResult(void);		// リザルトの初期化処理
void UninitResult(void);	// リザルトの終了処理
void UpdateResult(void);	// リザルトの更新処理
void DrawResult(void);		// リザルトの描画処理

void DrawResultPolygon		// リザルトのポリゴン描画処理
(int nIdxPolygon);

void SetResult				// リザルトの設定処理
(RESULTLAYTYPE type, int nNum);

void DelResult				// リザルトの消去処理
(RESULTLAYTYPE type);

int SetResultPolygon		// リザルトのポリゴンの設定処理
(int nIdxTexture, bool bAlphaBlend, int nDrowLevel,			// テクスチャインデックス, aブレンドをするか
 D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 addRot,		// 位置, 角度, 加算角度
 float fSizeWidth, float fSizeHeight,						// 大きさ(幅, 高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 texSize,					// テクスチャ座標, テクスチャサイズ
 D3DXCOLOR col);											// 色

void ReceiveResult			// リザルトの情報を貰う処理
(RESULT_TYPEPLAYER* pTypePlayer, int pHaveEsa[], 			// プレイヤーの種類, エサの情報
 int nMaxPlayer, int nMaxHave);								// プレイヤーの種類の大きさ, プレイヤーの総数, エサの種類の総数 
 
int GetRankingForResult		// リザルトからランキングに情報を渡す処理
(GiveResultPlayer* pTypePlayer, int nNumPlayer);			// プレイヤーの種類情報(情報を入れる場所, その場所の大きさ)

void SetLoadResult			// リザルトの読み取り処理
(const char* pFilename);

int GetNowEsaTypeResult		// リザルトの集計中のエサの種類を返す処理
(void);	

#endif