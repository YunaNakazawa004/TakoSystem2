//=============================================================================
// 
// メッシュリングヘッダー [meshring.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _MESHRING_H_		// このマクロ定義がされていなかったら
#define _MESHRING_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// 列挙型の定義
//*****************************************************************************

// メッシュリングの種類
typedef enum
{
	MESHRINGTYPE_NONE = -1,		// [-1]無し
	MESHRINGTYPE_RIPPLES ,		// [0]波紋
	MESHRINGTYPE_SHOCKWAVE,		// [1]衝撃波

	MESHRINGTYPE_MAX			// 最大値

}MESHRINGTYPE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitMeshRing(void);
void UninitMeshRing(void);
void UpdateMeshRing(void);
void DrawMeshRing(void);
void SetMeshRing(MESHRINGTYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size, D3DXCOLOR col);


void SetVtxMeshRing(int nIdx);

D3DXVECTOR3 CalcShockWaveRot(D3DXVECTOR3 pos1, D3DXVECTOR3 pos2);	// 衝撃波の角度を求める

#endif