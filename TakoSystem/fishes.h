//=============================================================================
// 
// 生き物ヘッダー [fishes.h]
// Author : Mutsuki Uemura
// 
//=============================================================================
#ifndef _FISHES_H_		// このマクロ定義がされていなかったら
#define _FISHES_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "model.h"

//*****************************************************************************
// 生き物の状態
//*****************************************************************************4
typedef enum
{
	FISHESSTATE_MOVE = 0,			// 移動状態
	FISHESSTATE_STOP,				// 停止状態
	FISHESSTATE_MAX
}FISHESSTATE;

//*****************************************************************************
// 生き物の構造体
//*****************************************************************************
typedef struct
{
	int nUseNum;								// 使用数
	D3DXVECTOR3 pos;							// 現在の位置
	D3DXVECTOR3 posOld;							// 前回の位置
	D3DXVECTOR3 move;							// 移動量
	D3DXVECTOR3 rot;							// 向き
	float fAngle;								// 向きの最終地点
	int MoveTime;								// 移動時間
	int StopTime;								// 停止時間
	int nMoveCnt;								// 移動カウント
	int	nStopCnt;								// 停止カウント
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	FISHESSTATE state;							// 状態
	int nCounterState;							// 状態カウンター
	float fRadius;								// 半径
	float fHeight;								// 高さ
	bool bMove;									// 動いているかどうか
	bool bMoving;								// 動くかどうか
	bool bUse;									// 使用しているかどうか
	int nModelIdx;								// 使用するモデル
	int nNumModel;								// モデル(パーツ)の総数
}Fishes;

//*****************************************************************************
// 生き物のモデル構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	// テクスチャへのポインタ
	LPD3DXMESH pMesh;							// マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						// メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								// マテリアルの数

	

	bool bUse;									// 使用状態

}Fishes_Model;
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitFishes(void);
void UninitFishes(void);
void UpdateFishes(void);
void DrawFishes(void);
void CollisionFishes(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fWidth, float fDepth);
Fishes* GetFishes(void);
void SetFishes(int ModelIdx, int nNumSet);

#endif