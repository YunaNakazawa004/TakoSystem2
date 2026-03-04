//=============================================================================
// 
// メッシュオービットヘッダー [meshorbit.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _MESHORBIT_H_		// このマクロ定義がされていなかったら
#define _MESHORBIT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ORBIT_VTX		(100)			// オービットの頂点数

//*****************************************************************************
// メッシュオービットの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;		// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;		// インデックスバッファへのポインタ
	D3DMATRIX pMtxParent;					// 親のマトリックス
	D3DXVECTOR3 aOffset[2];					// 両端のオフセット
	D3DXCOLOR aCol[2];						// 両端の基準の色
	D3DXVECTOR3 aPosPoint[MAX_ORBIT_VTX];	// 計算後の各頂点座標
	D3DXCOLOR aColPoint[MAX_ORBIT_VTX];		// 各頂点カラー
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	bool bUse;								// 使用しているかどうか
}MeshOrbit;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitMeshOrbit(void);
void UninitMeshOrbit(void);
void UpdateMeshOrbit(void);
void DrawMeshOrbit(void);
void SetMeshOrbitPos(int nIdx, D3DXVECTOR3 Offset0, D3DXVECTOR3 Offset1, D3DXCOLOR col0, D3DXCOLOR col1, D3DMATRIX* pMtxParent);
int SetMeshOrbit(D3DXVECTOR3 Offset0, D3DXVECTOR3 Offset1, D3DXCOLOR col0, D3DXCOLOR col1, D3DMATRIX* pMtxParent);
MeshOrbit* GetMeshOrbit(void);

#endif