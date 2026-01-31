//=============================================================================
// 
// メッシュフィールドヘッダー [meshfield.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _MESHFIELD_H_		// このマクロ定義がされていなかったら
#define _MESHFIELD_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// メッシュフィールドの種類
//*****************************************************************************
typedef enum
{
	MESHFIELDTYPE_ROAD = 0,					// 道
	MESHFIELDTYPE_SNOW,						// 雪
	MESHFIELDTYPE_ICE,						// 氷
	MESHFIELDTYPE_MAX
}MESHFIELDTYPE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitMeshField(void);
void UninitMeshField(void);
void UpdateMeshField(void);
void DrawMeshField(void);
void SetMeshField(MESHFIELDTYPE type, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size);
bool GetMeshFieldType(MESHFIELDTYPE type, D3DXVECTOR3 pos);

#endif