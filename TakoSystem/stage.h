//=============================================================================
// 
// ステージヘッダー [stage.h]
// Author : Mutsuki Uemura
// 
//=============================================================================
#ifndef _STAGE_H_		// このマクロ定義がされていなかったら
#define _STAGE_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define MAX_ARIA_STAGE		(18050.0f)		// ステージの最大の範囲(半径)

//*****************************************************************************
// ステージの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	// テクスチャへのポインタ
	LPD3DXMESH pMesh;							// マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						// メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								// マテリアルの数
	int nIdx;									// ステージのインデックス
	int nIdxStageParent;						// 親ステージのインデックス
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR3 posOff;							// 位置(オフセット)
	D3DXVECTOR3 rotOff;							// 向き(オフセット)
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
}Stage;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitStage(void);
void UninitStage(void);
void UpdateStage(void);
void DrawStage(void);
void CollisionStage(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fWidth, float fDepth);

#endif