//==================================================================================
// 
// 海流に関する処理[oceancurrents.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _OCEANCURRENTS_H_
#define _OCEANCURRENTS_H_

// マクロ定義 ================================================== 

#define MAX_IDX_OCPOLYGON			(32)		// 設定出来るインデックス数

// 列挙型の定義 ================================================

// 海流の状態
typedef enum
{
	OCEANCURRENTSSTATE_NOMAL = 0,	// [0]通常状態
	OCEANCURRENTSSTATE_WAIT,		// [1]渦潮待機状態
	OCEANCURRENTSSTATE_WIRLPOOL,	// [2]渦潮状態
	
	OCEANCURRENTSSTATE_MAX			// 最大値

}OCEANCURRENTSSTATE;

// 海流UIの種類
typedef enum
{
	OCUITYPE_NULL = -1,			// [-1]無し

	OCUITYPE_LAY_TOPWANING,		// [0]警告
	OCUITYPE_LAY_BOTWANING,		// [0]警告
	OCUITYPE_LAY_LEFTWANING,	// [0]警告
	OCUITYPE_LAY_RIGHTWANING,	// [0]警告
	
	OCUITYPE_TIMER,			// [1]タイマー
	
	OCUITYPE_MAX			// 最大値

}OCUITYPE;

// 構造体の定義 ================================================

// 海流のUI情報
typedef struct
{
	OCUITYPE type;							// 種類
	float fValue;
	int aIdxOCPolygon[MAX_IDX_OCPOLYGON];	// ポリゴンのインデックス
	int nNumIdx;							// インデックスの総数

	D3DXVECTOR3 initPos;					// Uiの位置
	D3DXVECTOR3 pos;						// Uiの位置

	float fColA;							// 透明度

	bool bDisp;								// 表示状態
	bool bUse;								// 使用状態

}OceanCurrentsLayout;

// 海流ポリゴンの設定情報
typedef struct
{
	int type;				// 種類
	int nIdxTexture;			// テクスチャインデックス
	bool bAlphaBlend;			// アルファブレンドするか

	D3DXVECTOR3 offPos;			// オフセット
	float fSizeWidth;			// 幅の長さ
	float fSizeHeight;			// 高さの長さ
	D3DXVECTOR2 texPos;			// テクスチャ座標
	D3DXVECTOR2 addTexPos;		// 座標加算量
	D3DXVECTOR2 texSize;		// テクスチャサイズ

	D3DXCOLOR col;				// 色

}OCUI_info;

// プロトタイプ宣言 ============================================

void InitOceanCurrents(void);				// 海流の初期化処理
void UninitOceanCurrents(void);				// 海流の終了処理
void UpdateOceanCurrents(void);				// 海流の更新処理
void DrawOceanCurrents(void);				// 海流の描画処理

void SetVtxOceanCurrents					// 海流の頂点設定処理
(OceanCurrentsLayout* pOCLayout);				// 

int SetOceanCurrentsLayOut					// 海流のUI設定処理
(OCUITYPE type, D3DXVECTOR3 pos,				// 
 OCUI_info *pOCUiInfo, int nSetNum);			// 位置, 倍率

int SetOceanCurrentsPolygon					// 海流のポリゴン設定処理		
(OCUITYPE type, int nIdxTex, bool bAlphaBlend,						// 種類, aブレンドをするか
 D3DXVECTOR3 pos, float fWidth, float fHeight,						// 位置, 大きさ(幅, 高さ)
 D3DXVECTOR2 texPos, D3DXVECTOR2 addTexPos, D3DXVECTOR2 texSize,	// テクスチャ(座標, 座標加算量, サイズ)
 D3DXCOLOR col);													// 色					

void UpdateOceanCurrentsState(void);					// 海流の状態更新処理

void MoveOceanCurrents						// 中心を軸にした海流の移動処理
(D3DXVECTOR3* pPos);

OCEANCURRENTSSTATE GetOceanCurrents(void);	// 現在の海流の状態を返す

void SetLoadOceanCurrents(const char* pFilename);

#endif
