//=========================================
// 
// 地面のヘッダー
// Author : Mutsuki Uemura
// 
//=========================================
#ifndef _WATERSURF_H_
#define _WATERSURF_H_

#define MAX_WATERSURF	(256)

//地面の構造体
typedef struct
{
	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 rot;			// 角度
	D3DXCOLOR col;				// 色
	D3DXMATRIX mtxWorld;
	D3DXVECTOR3 size;			// サイズ
	bool bUse;					// 使用しているか
	bool bUp;					// 上昇か
}WaterSurf;

//プロトタイプ宣言
void InitWaterSurf(void);
void UninitWaterSurf(void);
void UpdateWaterSurf(void);
void DrawWaterSurf(void);
void SetWaterSurf(D3DXVECTOR3 pos, D3DXVECTOR3 rot);

#endif
