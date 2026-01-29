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
// プロトタイプ宣言
//*****************************************************************************
void InitMeshRing(void);
void UninitMeshRing(void);
void UpdateMeshRing(void);
void DrawMeshRing(void);
void SetMeshRing(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size, D3DXCOLOR col);

#endif