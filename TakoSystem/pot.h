//=============================================================================
// 
// タコつぼヘッダー [pot.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _POT_H_		// このマクロ定義がされていなかったら
#define _POT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"
#include "player.h"
#include "computer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_POT				(128)									// タコつぼの最大数

//*****************************************************************************
// タコつぼの構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 向き
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	int nFood;					// 入っているエサの数
	EsaQueue esaQueue;			// エサのインデックス
	bool bUse;					// 使用しているかどうか
}Pot;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitPot(void);
void UninitPot(void);
void UpdatePot(void);
void DrawPot(void);
void SetPot(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
void SetRandomPot(int nAmount);
Pot* GetPot(void);

bool CollisionPot(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fRadius, float fHeight);
bool CollisionPotArea(D3DXVECTOR3 pos, float fRadius, Player* pPlayer, Computer* pComputer, bool bTentacle);

#endif