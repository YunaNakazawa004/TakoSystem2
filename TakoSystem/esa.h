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

// エサの種類（ファイル追加まで）
typedef enum
{
	ESATYPE_CRAB = 0,	// [0]カニ
	ESATYPE_SHELL,		// [1]貝
	ESATYPE_SHRIMP,		// [2]エビ
	ESATYPE_YA,		// [2]エビ

	ESATYPE_MAX			// 最大値

}ESATYPE;

// エサの挙動の種類
typedef enum
{
	ESA_ACTTYPE_NONE = 0,		// [0]何もしない状態

	ESA_ACTTYPE_LAND,			// [1]何かしらに接触している状態
	ESA_ACTTYPE_SWIM,			// [2]泳いでいる状態

	ESA_ACTTYPE_GOTO_PLAYER,	// [3]プレイヤーを追跡する状態
	ESA_ACTTYPE_GOTO_POT,		// [4]タコつぼに向かう状態

	ESA_ACTTYPE_MAX				// 最大値

}ESA_ACTTYPE;

// 構造体の定義 ================================================

// エサの設定情報
typedef struct
{
	int nidxType;			// エサの種類

	ESA_ACTTYPE esaType;	// エサの挙動
	int nBehavior;			// 挙動の値

	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 角度

}Esa_info;

// エサの情報
typedef struct
{
	int nIdxModel;			// モデルのインデックス

	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 角度

	D3DXMATRIX mtxWorld;	// ワールドマトリックス

	float fMoveAngle;		// 移動角度

	ESA_ACTTYPE esaType;	// エサの挙動
	int nNumBehavior;		// 挙動の値

	//bool bHave;				// 所持され状態(持っているか)
	bool bDisp;				// 表示状態
	bool bUse;				// 使用状態

}Esa;

// エサのモデルの設定情報
typedef struct
{
	char aFilename[128];	// モデルファイル名

	float fSpeed;			// 移動速度
	float fHitRadius;		// 当たり判定の大きさ

	int nScore;				// モデル(エサ)の獲得スコア

}EsaModel_info;

// エサのモデル情報
typedef struct
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_TEXTURE];	// テクスチャへのポインタ
	LPD3DXMESH pMesh;							// マテリアルへのポインタ
	LPD3DXBUFFER pBuffMat;						// メッシュ(頂点情報)へのポインタ
	DWORD dwNumMat;								// マテリアルの数

	int nScore;									// モデル(エサ)の獲得スコア
	
	float fSpeed;								// 移動速度
	float fHitRadius;							// 当たり判定の大きさ

	bool bUse;									// 使用状態

}EsaModel;

// プロトタイプ宣言 ============================================

// 基本処理

void InitEsa(void);			// エサの初期化処理
void UninitEsa(void);		// エサの終了処理
void UpdateEsa(void);		// エサの更新処理
void DrawEsa(void);			// エサの描画処理

// 設定処理

int SetModelEsa				// エサのモデル読み込み処理
(EsaModel_info infoEsaModel,			// エサモデルの設定情報 
 EsaModel *pEsaModel, int nMaxSizeNum);	// 読み込んだエサの格納場所, エサの格納場所の数

int SetEsa					// エサの設定処理
(int nEsaType,							// 設定するエサのタイプ, 
 ESA_ACTTYPE esaType, int nBehavior,	// エサの挙動, 挙動の値
 D3DXVECTOR3 pos, D3DXVECTOR3 rot);		// 位置, 角度

void ChangeEsaState			// エサの状態切り替え処理
(int nIdxEsa,							// エサのインテックス
 ESA_ACTTYPE changeState, int nValue);	// 切り替える挙動, 挙動の値

// 更新処理

void BehaviorEsa			// エサの挙動の処理
(Esa *pEsa);							// 処理するエサのポインタ

void MoveEsa				// エサの移動処理
(Esa *pEsa);							// 処理するエサのポインタ

// 判定処理

bool CollisionEsa			// エサの当たり判定処理
(int* pIdx,	bool bCollision,			// 判定したエサのインデックス, ぶつかりの判定をするか 
 D3DXVECTOR3 *pos, float fHitRadius);	// 位置, 大きさ 

// 値を返す処理

Esa *GetEsa(void);			// エサの情報を返す処理

EsaModel* GetEsaModel		// エサモデルの情報を返す処理
(int nIdx);								// エサモデル

int GetNumEsaType(void);	// エサの種類数を返す処理

#endif
