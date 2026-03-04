//=============================================================================
// 
// モデルヘッダー [model.h]
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
	MOTIONTYPE_INK,				// 墨吐きアクション
	MOTIONTYPE_TENTACLELONG,	// 触手伸ばしアクション
	MOTIONTYPE_TENTACLESHORT,	// 触手戻しアクション
	MOTIONTYPE_DASH,			// 高速移動
	MOTIONTYPE_OCEANCULLENT,	// 渦潮
	MOTIONTYPE_WIN1,			// 喜び
	MOTIONTYPE_WIN2,			// ガッツポーズ
	MOTIONTYPE_CLAP1,			// 拍手1
	MOTIONTYPE_CLAP2,			// 拍手2(不満げ)
	MOTIONTYPE_DEATH,			// 絶命
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
// モデルの情報
//*****************************************************************************
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	// テクスチャへのポインタ
	LPD3DXMESH pMesh;							// マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						// メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								// マテリアルの数
}Model_Info;

//*****************************************************************************
// モデルの構造体
//*****************************************************************************
typedef struct
{
	int nIdx;									// モデルのインデックス
	int nIdxModelParent;						// 親モデルのインデックス
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR3 scale;							// サイズ
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