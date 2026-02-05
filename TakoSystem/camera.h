//=============================================================================
// 
// カメラヘッダー [camera.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _CAMERA_H_		// このマクロ定義がされていなかったら
#define _CAMERA_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_CAMERA				(2)										// カメラの数
#define DISTANCE				(300.0f)								// 視点と注視点間の距離

//*****************************************************************************
// カメラの種類
//*****************************************************************************
typedef enum
{
	CAMERATYPE_PLAYER = 0,			// プレイヤー追従
	CAMERATYPE_POINT,				// 定点カメラ
	CAMERATYPE_MAX
}CAMERATYPE;

//*****************************************************************************
// カメラ構造体の定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 posV;				// 視点
	D3DXVECTOR3 posR;				// 注視点
	D3DXVECTOR3 posVDest;			// 目的の視点
	D3DXVECTOR3 posRDest;			// 目的の注視点
	D3DXVECTOR3 vecU;				// 上方向ベクトル
	D3DXVECTOR3 rot;				// 向き
	float fAngle;					// 上下移動用の角度
	float fDistance;				// 視点と注視点の距離
	float fRDistance;				// 注視点の差異
	D3DXMATRIX mtxProjection;		// プロジェクションマトリックス(モニターのサイズ)
	D3DXMATRIX mtxView;				// ビューマトリックス(カメラの画面のサイズ)
	D3DVIEWPORT9 viewport;			// ビューポート
	CAMERATYPE type;				// 種類
}Camera;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(int nIdx);
void SetNumCamera(int nNum);
void SetCameraPos(int nIdx, D3DXVECTOR3 posV, D3DXVECTOR3 posR, CAMERATYPE type);
Camera* GetCamera(void);
int GetNumCamera(void);

#endif