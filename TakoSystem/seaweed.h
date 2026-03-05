//=============================================================================
// 
// 海藻ヘッダー [seaweed.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _SEAWEED_H_		// このマクロ定義がされていなかったら
#define _SEAWEED_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "model.h"

//*****************************************************************************
// 海藻の種類
//*****************************************************************************4
typedef enum 
{
	SEAWEEDTYPE_BOTTOM,					// 茎
	SEAWEEDTYPE_TOP,					// 一番上
	SEAWEEDTYPE_MAX
} SEAWEEDTYPE;

//*****************************************************************************
// 海藻のモデルの構造体
//*****************************************************************************
typedef struct
{
	int nIdx;									// モデルのインデックス
	int nIdxModelParent;						// 親モデルのインデックス
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR3 rotDest;						// 目的の向き
	float fAngle;								// 
	D3DXVECTOR3 posOff;							// 位置(オフセット)
	D3DXVECTOR3 rotOff;							// 向き(オフセット)
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	SEAWEEDTYPE type;							// 種類
}Seaweed_Model;

//*****************************************************************************
// 海藻の構造体
//*****************************************************************************
typedef struct 
{
	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 向き
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	int nIdxSafe;				// 安地インデックス
	bool bUse;					// 使用しているかどうか

	Seaweed_Model aModel[MAX_NUMMODEL];	// モデル(パーツ)
	int nNumModel;						// モデル(パーツ)の総数
}Seaweed;
											
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitSeaweed(void);
void UninitSeaweed(void);
void UpdateSeaweed(void);
void DrawSeaweed(void);
void SetSeaweed(D3DXVECTOR3 pos, int nLength);
void SetRandomSeaweed(int nAmount);
Seaweed* GetSeaweed(void);

#endif