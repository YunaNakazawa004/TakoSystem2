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
// プロトタイプ宣言
//*****************************************************************************
void InitMeshDome(void);
void UninitMeshDome(void);
void UpdateMeshDome(void);
void DrawMeshDome(void);
void SetMeshDome(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, float fRadius);

#endif