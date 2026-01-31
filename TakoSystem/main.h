//=============================================================================
// 
// メインヘッダー [main.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _MAIN_H_		// このマクロ定義がされていなかったら
#define _MAIN_H_		// 2重インクルード防止のマクロを定義する

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "d3dx9.h"		// 描画処理に必要
#define DIRECTINPUT_VERSION	(0x0800)		// ビルド時の警告対処用マクロ
#include "dinput.h"		// 入力処理に必要
#include "xaudio2.h"	// サウンド処理に必要
#include "xinput.h"		// ジョイパッド処理に必要

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment(lib,"d3d9.lib")		// 描画処理に必要
#pragma comment(lib,"d3dx9.lib")	// [d3d9.lib]の拡張ライブラリ
#pragma comment(lib,"dxguid.lib")	// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib,"winmm.lib")	// システム時刻取得に必要
#pragma comment(lib,"dinput8.lib")	// 入力処理に必要
#pragma comment(lib,"xinput.lib")	// ジョイパッド処理に必要

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCREEN_WIDTH		(1280)															// ウィンドウの幅
#define SCREEN_HEIGHT		(720)															// ウィンドウの高さ
#define FVF_VERTEX_2D		(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)					// 頂点フォーマット[2D]
#define FVF_VERTEX_3D		(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)		// 頂点フォーマット[3D]
#define FVF_VERTEX_3D_MALTI	(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2)		// 3Dフォーマット(マルチテクスチャver)
#define MAX_PLAYER			(2)																// プレイ人数
#define MAX_COMPUTER		(10)															// CPUの数
#define ONE_GAME			(120)															// 1ゲームの制限時間
#define ONE_SECOND			(60)															// 1秒
#define WHITE_VTX			(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f))								// 頂点カラーが白
#define RED_VTX				(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f))								// 頂点カラーが赤
#define GREEN_VTX			(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f))								// 頂点カラーが緑
#define BLUE_VTX			(D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f))								// 頂点カラーが青
#define YELLOW_VTX			(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f))								// 頂点カラーが黄色
#define PURPLE_VTX			(D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f))								// 頂点カラーが紫
#define CYAN_VTX			(D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.0f))								// 頂点カラーが水色
#define BLACK_VTX			(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f))								// 頂点カラーが黒
#define CENTER				(D3DXVECTOR3(640.0f, 360.0f, 0.0f))								// 中心座標
#define FIRST_POS			(D3DXVECTOR3(0.0f, 0.0f, 0.0f))									// 初期座標
#define MAX_VERTEX			(4)																// 最大頂点数
#define MAX_POLYGON			(2)																// 最大ポリゴン数
#define DEFAULT_RHW			(1.0f)															// rhwの規定値
#define ONE_LINE			(100)															// 一行
#define MAX_TEXTURE			(50)															// 最大テクスチャ数
#define MAX_NUMMODEL		(70)															// モデルの最大数
#define MAX_KEY				(100)															// キーの最大数
#define MAX_MOTION			(20)															// モーションの最大数
#define ALLOW_X				(10000000)														// X方向の移動制限
#define ALLOW_Z				(10000000)														// Z方向の移動制限
#define OUTCYLINDER_RADIUS	(9000.0f)														// 外側の円柱の半径
#define INCYLINDER_RADIUS	(2000.0f)														// 内側の円柱の半径
#define CYLINDER_HEIGHT		(17500.0f)														// 円柱の高さ

//*****************************************************************************
// 画面(モード)の種類
//*****************************************************************************
typedef enum
{
	MODE_TITLE = 0,				// タイトル画面
	MODE_TUTORIAL,				// チュートリアル画面
	MODE_GAME,					// ゲーム画面
	MODE_RESULT,				// リザルト画面
	MODE_RANKING,				// ランキング画面
	MODE_MAX
}MODE;

//*****************************************************************************
// 頂点情報[2D]の構造体を定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 頂点座標(x.y.z)
	float rhw;					// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;				// 頂点カラー(r,g,b,a)
	D3DXVECTOR2 tex;			// テクスチャ座標(x,y)
}VERTEX_2D;

//*****************************************************************************
// 頂点情報[3D]の構造体を定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;			// 頂点座標(x.y.z)
	D3DXVECTOR3 nor;			// 法線ベクトル
	D3DCOLOR col;				// 頂点カラー(r,g,b,a)
	D3DXVECTOR2 tex;			// テクスチャ座標(x,y)
}VERTEX_3D;

//*****************************************************************************
// 頂点情報[3D_MALTI]の構造体を定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;	// 頂点情報(X,Y,Z)
	D3DXVECTOR3 nor;	// 法線ベクトル
	D3DCOLOR col;		// 頂点カラー（r,g,b,a）

	D3DXVECTOR2 tex;	// テクスチャの座標0(X,Y)
	D3DXVECTOR2 texM;	// テクスチャの座標1(X,Y)

}VERTEX_3D_MALTI;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LPDIRECT3DDEVICE9 GetDevice(void);
void SetMode(MODE mode);
MODE GetMode(void);
void onWireFrame(void);
void offWireFrame(void);
void CorrectAngle(float* fAngle, float fAngleCmp);
void SetFog(D3DXCOLOR col, float fFogStart, float fFogEnd);

#endif