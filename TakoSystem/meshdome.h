//=============================================================================
// 
// メッシュドームヘッダー [meshdome.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _MESHDOME_H_		// このマクロ定義がされていなかったら
#define _MESHDOME_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// メッシュドームの種類
//*****************************************************************************
typedef enum
{
	MESHDOMETYPE_SKY = 0,		// 空
	MESHDOMETYPE_ROCK,			// 岩
	MESHDOMETYPE_MAX
}MESHDOMETYPE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitMeshDome(void);
void UninitMeshDome(void);
void UpdateMeshDome(void);
void DrawMeshDome(void);
void SetMeshDome(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, float fRadius, bool bInside, MESHDOMETYPE type);

#endif