//=============================================================================
// 
// 生き物ヘッダー [fishes.h]
// Author : 中澤優奈
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
	D3DXVECTOR3 pos;							// 現在の位置
	D3DXVECTOR3 posOld;							// 前回の位置
	D3DXVECTOR3 move;							// 移動量
	D3DXVECTOR3 rot;							// 向き
	float fAngle;								// 向きの最終地点
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	FISHESSTATE state;							// 状態
	int nCounterState;							// 状態カウンター
	float fRadius;								// 半径
	float fHeight;								// 高さ
	bool bMove;									// 動いているかどうか
	bool bUse;									// 使用しているかどうか
	Model aModel[MAX_NUMMODEL];					// モデル(パーツ)
	int nNumModel;								// モデル(パーツ)の総数
	MOTION_INFO aMotionInfo[MAX_MOTION];		// モーション情報
	int nNumMotion;								// モーションの総数
	MOTIONTYPE motionType;						// 現在のモーションの種類
	bool bLoopMotion;							// 現在のループするかどうか
	int nNumKey;								// 現在のキーの総数
	int nKey;									// 現在の現在のキーNo.
	int nCounterMotion;							// 現在のモーションのカウンター
	bool bFinishMotion;							// 現在のモーションが終了しているかどうか
	bool bBlendMotion;							// ブレンドモーションがあるかどうか
	MOTIONTYPE motionTypeBlend;					// ブレンドモーションの種類
	bool bLoopMotionBlend;						// ブレンドモーションがループするかどうか
	int nNumKeyBlend;							// ブレンドモーションのキーの総数
	int nKeyBlend;								// ブレンドモーションの現在のキーNo.
	int nCounterMotionBlend;					// ブレンドモーションのカウンター
	int nFrameBlend;							// ブレンドフレーム数
	int nCounterBlend;							// ブレンドカウンター
}Fishes;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitFishes(void);
void UninitFishes(void);
void UpdateFishes(void);
void DrawFishes(void);
void CollisionFishes(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fWidth, float fDepth);
Fishes* GetFishes(void);

#endif