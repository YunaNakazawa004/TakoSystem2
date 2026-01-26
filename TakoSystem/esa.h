//==================================================================================
// 
// エサに関する処理[esa.h]
// Author : Soki Inamura 
// 
//==================================================================================

#ifndef _ESA_H_
#define _ESA_H_

// マクロ定義 ==================================================

#define MAX_MODEL_ESA	(64)	// 用意出来るモデルの最大値
#define MAX_SET_ESA		(128)	// 設定出来るエサの総数

// 列挙型の定義 ================================================

// エサの挙動の種類
typedef enum
{
	ESATYPE_LAND = 0,	// [0]何かしらに接触している状態
	ESATYPE_SWIM,		// [1]泳いでいる状態

	ESATYPE_MAX			// 最大値

}ESATYPE;

// 構造体の定義 ================================================

// エサの情報
typedef struct
{
	int nIdxModel;			// モデルのインデックス

	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 角度

	D3DXMATRIX mtxWorld;	// ワールドマトリックス

	bool bDisp;				// 表示状態
	bool bUse;				// 使用状態

}Esa;

// エサのモデルの設定情報
typedef struct
{
	char aFilename[128];	// モデルファイル名

	float fHitRadius;		// 当たり判定の大きさ

}EsaModel_info;

// エサのモデル情報
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	// テクスチャへのポインタ
	LPD3DXMESH pMesh;							// マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						// メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								// マテリアルの数

	float fHitRadius;							// 当たり判定の大きさ

	bool bUse;									// 使用状態

}EsaModel;

// プロトタイプ宣言 ============================================

void InitEsa(void);			// エサの初期化処理
void UninitEsa(void);		// エサの終了処理
void UpdateEsa(void);		// エサの更新処理
void DrawEsa(void);			// エサの描画処理

int SetModelEsa				// エサのモデル読み込み処理
(EsaModel_info infoEsaModel,			// エサモデルの設定情報 
 EsaModel *pEsaModel, int nMaxSizeNum);	// 読み込んだエサの格納場所, エサの格納場所の数

void SetEsa					// エサの設定処理
(int nEsaType, ESATYPE esaType,			// 設定するエサのタイプ, エサの挙動
 D3DXVECTOR3 pos, D3DXVECTOR3 rot);		// 位置, 角度			
 						 
bool CollisionEsa			// エサの当たり判定処理
(int* pIdx,	bool bCollision,			// 判定したエサのインデックス, ぶつかりの判定をするか 
 D3DXVECTOR3 *pos, float fHitRadius);	// 位置, 大きさ 

Esa *GetEsa(void);			// エサの情報を返す処理

#endif
