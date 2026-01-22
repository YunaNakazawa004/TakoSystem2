//==================================================================================
// 
// エサに関する処理[esa.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _ESA_H_
#define _ESA_H_

// 構造体の定義 ================================================

// エサのモデル情報
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	// テクスチャへのポインタ
	LPD3DXMESH pMesh;							// マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						// メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								// マテリアルの数

	bool bUse;									// 使用状態

}EsaModel;

// プロトタイプ宣言 ============================================

void InitEsa(void);			// エサの初期化処理
void UninitEsa(void);		// エサの終了処理
void UpdateEsa(void);		// エサの更新処理
void DrawEsa(void);			// エサの描画処理

void SetEsa					// エサの設定処理
(int nType, D3DXVECTOR3 pos, D3DXVECTOR3 rot);				

int SetModelEsa			// エサのモデル読み込み処理
(const char* pFilenameModel, EsaModel *pModel, int nMaxSizeNum);

#endif
