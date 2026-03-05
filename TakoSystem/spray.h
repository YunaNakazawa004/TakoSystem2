//=============================================================================
// 
// 飛沫ヘッダー [spray.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _SPRAY_H_		// このマクロ定義がされていなかったら
#define _SPRAY_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// 飛沫の種類
//*****************************************************************************
typedef enum
{
	SPRAYTYPE_0 = 0,			// タイプ0
	SPRAYTYPE_MAX
}SPRAYTYPE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitSpray(void);
void UninitSpray(void);
void UpdateSpray(void);
void DrawSpray(void);
void SetSpray(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float length, SPRAYTYPE type);
void SetSprayCircle(D3DXVECTOR3 pos, D3DXCOLOR col, SPRAYTYPE type);

#endif