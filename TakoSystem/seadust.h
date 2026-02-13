//========================================================================
// 
// 塵ヘッダー [seadust.h]
// Author : 中澤優奈
// 
//========================================================================
#ifndef _SEADUST_H_	
#define _SEADUST_H_	

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitSeaDust(void);		
void UninitSeaDust(void);	
void UpdateSeaDust(void);	
void DrawSeaDust(void);		
void SetSeaDust(D3DXVECTOR3 pos);
void SetRandomSeaDust(int nAmount);

#endif