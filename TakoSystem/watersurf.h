//=========================================
// 
// 水面のヘッダー
// Author : Mutsuki Uemura
// 
//=========================================
#ifndef _WATERSURF_H_
#define _WATERSURF_H_

#define MAX_WATERSURF	(256)

//=======================================
// プロトタイプ宣言
//=======================================

void InitWaterSurf(void);
void UninitWaterSurf(void);
void UpdateWaterSurf(void);
void DrawWaterSurf(void);
void SetWaterSurf(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
float* GetWaterSurf_Height(void);

#endif
