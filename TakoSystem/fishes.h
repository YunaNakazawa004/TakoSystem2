//=============================================================================
// 
// 生き物ヘッダー [model.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _MODEL_H_		// このマクロ定義がされていなかったら
#define _MODEL_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// モーションの種類
//*****************************************************************************
typedef enum
{
	MOTIONTYPE_NEUTRAL = 0,		// 待機
	MOTIONTYPE_MOVE,			// 移動
	MOTIONTYPE_ACTION,			// アクション
	MOTIONTYPE_MAX
}MOTIONTYPE;

//*****************************************************************************
// キー要素の構造体
//*****************************************************************************
typedef struct
{
	float fPosX;		// 位置X
	float fPosY;		// 位置Y
	float fPosZ;		// 位置Z
	float fRotX;		// 向きX
	float fRotY;		// 向きY
	float fRotZ;		// 向きZ
}KEY;

//*****************************************************************************
// キー情報の構造体
//*****************************************************************************
typedef struct
{
	int nFrame;					// 再生フレーム
	KEY aKey[MAX_NUMMODEL];		// 各パーツのキー要素	
}KEY_INFO;

//*****************************************************************************
// モーション情報の構造体
//*****************************************************************************
typedef struct
{
	bool bLoop;						// ループするかどうか
	int nNumKey;					// キーの総数
	KEY_INFO aKeyInfo[MAX_KEY];		// キー情報
}MOTION_INFO;

//*****************************************************************************
// 生き物の構造体
//*****************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	// テクスチャへのポインタ
	LPD3DXMESH pMesh;							// マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						// メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								// マテリアルの数
	int nIdx;									// 生き物のインデックス
	int nIdxModelParent;						// 親生き物のインデックス
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR3 posOff;							// 位置(オフセット)
	D3DXVECTOR3 rotOff;							// 向き(オフセット)
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
}Model;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitModel(void);
void UninitModel(void);
void UpdateModel(void);
void DrawModel(void);
void CollisionModel(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fWidth, float fDepth);

#endif