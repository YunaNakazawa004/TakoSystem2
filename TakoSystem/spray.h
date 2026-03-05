//=============================================================================
// 
// 爆発ヘッダー [explosion.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _EXPLOSION_H_		// このマクロ定義がされていなかったら
#define _EXPLOSION_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// 爆発の種類
//*****************************************************************************
typedef enum
{
	EXPLOSIONTYPE_0 = 0,			// タイプ0
	EXPLOSIONTYPE_1,				// タイプ1
	EXPLOSIONTYPE_2,				// タイプ2
	EXPLOSIONTYPE_MAX
}EXPLOSIONTYPE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitExplosion(void);
void UninitExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);
void SetExplosion(D3DXVECTOR3 pos, D3DXVECTOR3 move, float length, EXPLOSIONTYPE type);

#endif