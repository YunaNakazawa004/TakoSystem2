//=============================================================================
// 
// マップヘッダー[map.h]
// Author : Mutsuki Uemura
// 
//=============================================================================
#ifndef _MAP_H_		
#define _MAP_H_		// 2重インクルード防止のマクロを定義

//=======================================
// 構造体の定義
//=======================================

// マップの構造体 -------------------

typedef struct
{
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 size;				// サイズ
	int TexIdx;						//テクスチャ番号

}Map;

// マップのテクスチャ情報の構造体 -------------------

typedef struct
{
	float Dif_Texture_Line;			// テクスチャの線の太さ分の差異
	char Model_FileName[256];		// モデルのファイルネーム
	int TexIdx;						//テクスチャ番号

}MapTextureInfo;

//=======================================
// プロトタイプ宣言
//=======================================
void InitMap(void);
void UninitMap(void);
void UpdateMap(void);
void DrawMap(void);
Map* GetMap(void);

#endif