//==================================================================================
// 
// メッシュシリンダー(円筒)に関する処理[mesh_clinder.h]
// Author : Soki Inamura
// 
//==================================================================================

#ifndef _MESH_CYLINDER_H_	
#define _MESH_CYLINDER_H_	

// プロトタイプ宣言 ============================================

void InitMesh_Cylinder(void);			// メッシュシリンダーの初期化処理
void UninitMesh_Cylinder(void);			// メッシュシリンダーの終了処理
void UpdateMesh_Cylinder(void);			// メッシュシリンダーの更新処理
void DrawMesh_Cylinder(void);			// メッシュシリンダーの描画処理

void SetMesh_Cylinder					// メッシュシリンダーの設定処理
(bool bReverse,								// 面の向き(true:外向き, false:内向き)
 D3DXVECTOR3 pos, D3DXVECTOR3 rot,			// 位置, 角度, 
 float fRadius, float fHeightSize,			// 大きさ(半径,高さ)
 int nWidthBlock, int nDepthBlock,			// 分割数(幅,奥行き)
 D3DXCOLOR col);							// 色

#endif