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
#include "esa.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define START_POS		(D3DXVECTOR3(0.0f, 0.0f, -200.0f))		// 開始位置
#define MAX_QUEUE		(256)									// キューに入るデータ数

//*****************************************************************************
// プレイヤーの状態
//*****************************************************************************
typedef enum
{
	PLAYERSTATE_NORMAL = 0,			// 通常状態
	PLAYERSTATE_MOVE,				// 移動状態
	PLAYERSTATE_APPEAR,				// 出現状態
	PLAYERSTATE_WAIT,				// 待機状態
	PLAYERSTATE_DASH,				// 高速移動状態
	PLAYERSTATE_INK,				// 墨吐き状態
	PLAYERSTATE_BACKAREA,			// エリア戻り状態
	PLAYERSTATE_MAX
}PLAYERSTATE;

//*****************************************************************************
// 触手の状態
//*****************************************************************************
typedef enum
{
	PLTENTACLESTATE_NORMAL = 0,			// 通常状態
	PLTENTACLESTATE_TENTACLELONG,		// 触手伸ばし状態
	PLTENTACLESTATE_TENTACLESHORT,		// 触手縮め状態
	PLTENTACLESTATE_MAX
}PLTENTACLESTATE;

//*****************************************************************************
// キュー構造体
//*****************************************************************************
typedef struct
{
	int nTail;					// データの最後尾
	int nData[MAX_QUEUE];		// エサのインデックスデータ
}EsaQueue;

//*****************************************************************************
// タコつぼとの関係
//*****************************************************************************
typedef enum
{
	POTSTATE_NONE = 0,			// 何もしてない
	POTSTATE_HIDE,				// 隠されている最中
	POTSTATE_STEAL,				// 盗られている最中
	POTSTATE_MAX
}POTSTATE;

//*****************************************************************************
// プレイヤーの構造体
//*****************************************************************************
typedef struct
{
	int nIdx;									// プレイヤーのインデックス
	D3DXVECTOR3 pos;							// 現在の位置
	D3DXVECTOR3 posOld;							// 前回の位置
	D3DXVECTOR3 move;							// 移動量
	D3DXVECTOR3 rot;							// 向き
	float fAngleX;								// X向きの最終地点
	float fAngleY;								// Y向きの最終地点
	float fRadius;								// 半径
	float fHeight;								// 高さ
	D3DXVECTOR3 posX;							// リーチの位置(クロスヘアの位置)
	D3DXVECTOR3 vecX;							// posVからposRへのベクトル(リーチへのベクトル)
	D3DXMATRIX mtxWorld;						// ワールドマトリックス

	PLAYERSTATE state;							// 状態
	PLTENTACLESTATE TentacleState;				// 触手の状態
	int nCounterState;							// 状態カウンター

	float fFogStart;							// フォグの開始位置
	float fFogEnd;								// フォグの終了位置

	bool bJump;									// ジャンプしているかどうか
	bool bLand;									// 着地しているかどうか
	bool bMove;									// 動いているかどうか
	bool bAct;									// アクション中かどうか
	bool bUse;									// 使用しているかどうか

	bool bBlind;								// 視界が悪いかどうか
	int nBlindCounter;							// 視界が悪いのを制御するカウンター

	int nFood;									// 持っているエサの数
	EsaQueue esaQueue;							// 持っているエサのキュー配列
	POTSTATE Potstate;							// 状態
	int nPotIdx;								// 近づいていたタコつぼのインデックス
	int nMaxFood;								// 一本の腕に一度に持てるエサの数

	int nTentacleCooldown;						// 触手のクールダウン
	int nInkCooldown;							// 墨吐きのクールダウン

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

// キュー
void Enqueue(EsaQueue* queue, int nIdx);
int Dequeue(EsaQueue* queue);

#endif