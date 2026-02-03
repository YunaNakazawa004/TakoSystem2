//=============================================================================
// 
// メッシュシリンダーヘッダー [meshcylinder.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _MESHCYLINDER_H_		// このマクロ定義がされていなかったら
#define _MESHCYLINDER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// メッシュシリンダーの種類
//*****************************************************************************
typedef enum
{
	MESHCYLINDERTYPE_SEA = 0,			// 海
	MESHCYLINDERTYPE_ROCK,				// 岩
	MESHCYLINDERTYPE_MAX			
}MESHCYLINDERTYPE;

//*****************************************************************************
// メッシュシリンダーの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;		// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;						// メッシュシリンダーの位置情報
	D3DXCOLOR col;							// メッシュシリンダーの色
	D3DXVECTOR3 rot;						// メッシュシリンダーの向き情報
	D3DXVECTOR2 block;						// 分割数
	D3DXVECTOR2 size;						// サイズ
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	MESHCYLINDERTYPE type;					// 種類
	bool bInside;							// 内側からの当たり判定かどうか
	bool bUse;								// 使用しているかどうか
}MeshCylinder;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitMeshCylinder(void);
void UninitMeshCylinder(void);
void UpdateMeshCylinder(void);
void DrawMeshCylinder(void);
int SetMeshCylinder(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR2 block, D3DXVECTOR2 size, D3DXCOLOR col, bool bInside, MESHCYLINDERTYPE type);
bool CollisionMeshCylinder(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fRadius, float fHeight, bool binsec);
MeshCylinder* GetMeshCylinder(void);

#endif