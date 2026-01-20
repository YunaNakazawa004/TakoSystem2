//=============================================================================
// 
// プレイヤーヘッダー [player.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _PLAYER_H_		// このマクロ定義がされていなかったら
#define _PLAYER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define START_POS		(D3DXVECTOR3(0.0f, 0.0f, -200.0f))		// 開始位置

//*****************************************************************************
// プレイヤーの状態
//*****************************************************************************4
typedef enum
{
	PLAYERSTATE_NORMAL = 0,			// 通常状態
	PLAYERSTATE_APPEAR,				// 出現状態
	PLAYERSTATE_WAIT,				// 待機状態
	PLAYERSTATE_MAX
}PLAYERSTATE;

//*****************************************************************************
// プレイヤーの構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;							// 現在の位置
	D3DXVECTOR3 posOld;							// 前回の位置
	D3DXVECTOR3 move;							// 移動量
	D3DXVECTOR3 rot;							// 向き
	float fAngle;								// 向きの最終地点
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	PLAYERSTATE state;							// 状態
	int nCounterState;							// 状態カウンター
	float fRadius;								// 半径
	float fHeight;								// 高さ
	bool bJump;									// ジャンプしているかどうか
	bool bLand;									// 着地しているかどうか
	bool bMove;									// 動いているかどうか
	bool bAct;									// アクション中かどうか
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
}Player;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void SetPlayer(int nIdx, D3DXVECTOR3 pos, D3DXVECTOR3 rot);
Player* GetPlayer(void);
void UpdateMotionPlayer(void);
void SetMotionPlayer(int nIdx, MOTIONTYPE motionType, bool bBlendMotion, int nFrameBlend);

#endif