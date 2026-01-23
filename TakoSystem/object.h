//=============================================================================
// 
// 配置物ヘッダー [object.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _OBJECT_H_		// このマクロ定義がされていなかったら
#define _OBJECT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_OBJECTMODEL			(30)									// 配置物モデルの最大数
#define MAX_OBJECT				(2048)									// 配置物の最大数

//*****************************************************************************
// 配置物の状態
//*****************************************************************************
typedef enum
{
	OBJECTSTATE_NONE = 0,		// その場で動かない
	OBJECTSTATE_MAX
}OBJECTSTATE;

//*****************************************************************************
// 配置物モデルの構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	  // テクスチャへのポインタ
	LPD3DXMESH pMesh;							  // マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						  // メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								  // マテリアルの数
	D3DXVECTOR3 VtxMax, VtxMin;					  // 最大値・最小値
}ObjectModel;

//*****************************************************************************
// 配置物の構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;						// 位置
	D3DXVECTOR3 posOff;						// 元の位置
	D3DXVECTOR3 move;						// 移動量
	D3DXVECTOR3 scale;						// 拡大率
	D3DXVECTOR3 rot;						// 向き
	D3DXMATRIX mtxWorld;					// ワールドマトリックス
	OBJECTSTATE state;						// 状態
	int nCounterState;						// 状態カウンター
	int nType;								// 種類
	int nIdxShadow;							// 影
	bool bUse;								// 使用しているかどうか
	bool bCollision;						// 当たり判定するかどうか
}Object;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitObject(const char* pStr);
void UninitObject(void);
void UpdateObject(void);
void DrawObject(void);
void SetObject(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdx, bool bCollision);
void SetObjectRandom(int nType, D3DXVECTOR3 posMin, D3DXVECTOR3 posMax, int nAmount);
Object* GetObjectAll(void);

bool CollisionObject(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fRadius, float fHeight);

void LoadObject(const char* pStr);
void LoadStage(const char* pStr);

#endif