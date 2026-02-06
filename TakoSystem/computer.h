//=============================================================================
//=============================================================================
// 
// CPUヘッダー [computer.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _COMPUTER_H_		// このマクロ定義がされていなかったら
#define _COMPUTER_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "player.h"
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define START_POS		(D3DXVECTOR3(0.0f, 0.0f, -200.0f))		// 開始位置

//*****************************************************************************
// CPUの状態
//*****************************************************************************4
typedef enum {
	CPUSTATE_APPEAR,				// 出現
	CPUSTATE_EXPLORE,				// 探索
	CPUSTATE_MOVE_TO_FOOD,			// エサを取りに行く
	CPUSTATE_ESCAPE,				// 回避
	CPUSTATE_ATTACK,				// 敵を追う
	CPUSTATE_INK_ATTACK,			// 墨吐き
	CPUSTATE_HIDE,					// 柱の裏に隠れる
	CPUSTATE_WAIT,					// 待機
	CPUSTATE_GO_TO_POT,				// タコつぼへ向かう
	CPUSTATE_HIDE_FOOD,				// エサを隠す
	CPUSTATE_STEAL_FOOD,			// 隠されたエサを奪う
	CPUSTATE_FINAL_COLLECT,		    // 終盤の回収行動
	CPUCPUSTATE_MAX
} CPUSTATE;

//*****************************************************************************
// 触手の状態
//*****************************************************************************
typedef enum
{
	CPUTENTACLESTATE_NORMAL = 0,		// 通常状態
	CPUTENTACLESTATE_TENTACLELONG,		// 触手伸ばし状態
	CPUTENTACLESTATE_TENTACLESHORT,		// 触手縮め状態
	CPUTENTACLESTATE_MAX
}CPUTENTACLESTATE;

//*****************************************************************************
// CPUの物理情報
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;				// 現在の位置
	D3DXVECTOR3 posOld;				// 前回の位置
	D3DXVECTOR3 move;				// 移動量
	D3DXVECTOR3 rot;				// 向き
	D3DXVECTOR3 dir;				// 向いている方向
	float fAngleY;					// Y向きの最終地点
	float fAngleX;					// X向きの最終地点
	float fRadius;					// 半径
	float fHeight;					// 高さ
	D3DXMATRIX mtxWorld;			// ワールドマトリックス
} Physics;

//*****************************************************************************
// CPUの構造体
//*****************************************************************************
typedef struct 
{
	int nIdx;					// タコのID
	CPUSTATE state;				// 現在の状態
	int nCounterState;			// 状態カウンター
	CPUTENTACLESTATE TentState;	// 触手の状態
	Physics phys;				// 物理情報
	bool bUse;					// 使用しているかどうか

	// ターゲット情報
	int nTargetFoodIdx;			// 狙っているエサ
	int nTargetEnemyIdx;		// 狙っている敵
	int nTargetPotIdx;			// 狙っているタコつぼ
	D3DXVECTOR3 targetWall;		// 触手で掴む壁ポイント
	D3DXVECTOR3 extarget;		// 探索中に目指す位置
	D3DXVECTOR3 posX;			// リーチの位置(クロスヘアの位置)
	D3DXVECTOR3 vecX;			// posVからposRへのベクトル(リーチへのベクトル)

	// スコアリング用
	float fFoodScore;			// エサの優先度
	float fAttackScore;			// 攻撃の優先度
	float fEscapeScore;			// 回避の優先度
	float fInkScore;			// 墨吐きの優先度
	float fPotScore;			// タコつぼの優先度

	// クールダウン
	int nTentacleCooldown;		// 触手のクールダウン
	int nInkCooldown;			// 墨吐きのクールダウン
	int nThinkCooldown;			// AIの思考間隔
	int nExploreCooldown;		// 探索位置更新間隔

	// 墨吐き
	bool bBlinded;				// 視界が悪化しているかどうか
	int nBlindCounter;			// 視界悪化カウンター

	// 所持エサ
	int nFoodCount;				// 所持エサの数
	EsaQueue esaQueue;			// エサのキュー情報
	POTSTATE Potstate;			// 状態
	int nMaxFood;				// 一本の足にもてるエサの最大数

	// ナビゲーション
	int nCurrentNode;			// 今いるノード
	int nNextNode;				// 次に向かうノード

	// 階層構造
	Model aModel[MAX_NUMMODEL];					// モデル(パーツ)
	int nNumModel;								// モデル(パーツ)の総数

	// モーション情報
	MOTION_INFO aMotionInfo[MAX_MOTION];		// モーション情報
	int nNumMotion;								// モーションの総数
	MOTIONTYPE motionType;						// 現在のモーションの種類
	bool bLoopMotion;							// 現在のループするかどうか
	int nNumKey;								// 現在のキーの総数
	int nKey;									// 現在の現在のキーNo.
	int nCounterMotion;							// 現在のモーションのカウンター
	bool bFinishMotion;							// 現在のモーションが終了しているかどうか

	// ブレンドモーション
	bool bBlendMotion;							// ブレンドモーションがあるかどうか
	MOTIONTYPE motionTypeBlend;					// ブレンドモーションの種類
	bool bLoopMotionBlend;						// ブレンドモーションがループするかどうか
	int nNumKeyBlend;							// ブレンドモーションのキーの総数
	int nKeyBlend;								// ブレンドモーションの現在のキーNo.
	int nCounterMotionBlend;					// ブレンドモーションのカウンター
	int nFrameBlend;							// ブレンドフレーム数
	int nCounterBlend;							// ブレンドカウンター
} Computer;
												
//*****************************************************************************
// ノードの構造体
//*****************************************************************************
typedef struct 
{
	int nIdx;					// ノードのID
	D3DXVECTOR3 pos;			// 位置
	bool bUse;					// 使用しているかどうか
} Node;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitComputer(void);
void UninitComputer(void);
void UpdateComputer(void);
void DrawComputer(void);

// 行動管理
void MoveToFood(Computer* pComputer);
void AttackEnemy(Computer* pComputer);
void Escape(Computer* pComputer);
void InkAttack(Computer* pComputer);
void Explore(Computer* pComputer);
void GoToPot(Computer* pComputer);
void HideFood(Computer* pComputer);
void StealFood(Computer* pComputer);
void FinalCollect(Computer* pComputer);

// 行動管理の補助関数
D3DXVECTOR3 GetFoodPosition(Computer* pComputer);
D3DXVECTOR3 GetEnemyPosition(Computer* pComputer);
D3DXVECTOR3 GetNearestEnemy(Computer* pComputer);

// スコア計算
void CalcFoodScore(Computer* pComputer);
void CalcAttackScore(Computer* pComputer);
void CalcEscapeScore(Computer* pComputer);
void CalcInkScore(Computer* pComputer);
void CalcPotScore(Computer* pComputer);

// 補助関数
bool IsEnemyNear(int nIdx, D3DXVECTOR3 pos, float fRange);
bool IsBehindPillar(D3DXVECTOR3 self, D3DXVECTOR3 target);
bool IsNearWall(D3DXVECTOR3 pos);
int CountEnemiesNear(int nIdx, D3DXVECTOR3 pos, float range);
bool IsNearPillar(D3DXVECTOR3 pos);

// 触手移動レイキャスト
void FindTentacleTarget(Computer* pComputer);
bool RaycastToOuterWall(D3DXVECTOR3 origin, D3DXVECTOR3 dir, float* hitDist);
bool RaycastToInnerPillar(D3DXVECTOR3 origin, D3DXVECTOR3 dir, float* hitDist);
bool ShouldUseTentacle(Computer* pComputer);
void UseTentacle(Computer* pComputer);

// ノード
void CreateOuterNodes3D(void);
void CreateInnerNodes3D(void);
D3DXVECTOR3 GetRandomExplorePoint(void);

void SetComputer(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
void SetRandomComputer(int nAmount);
Computer* GetComputer(void);
void CollisionInk(int nIdx, bool bCPU, D3DXVECTOR3 pos);
bool CollisionOcto(int nIdx, bool bCPU, D3DXVECTOR3 pos);

// モーション
void UpdateMotionComputer(int nIdx);
void SetMotionComputer(int nIdx, MOTIONTYPE motionType, bool bBlendMotion, int nFrameBlend);

#endif