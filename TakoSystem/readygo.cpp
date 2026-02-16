//===========================================================================
// 
// レディ処理[readygo.cpp]
// Author:Mutsuki Uemura
//
//===========================================================================
#include "readygo.h"
#include "input.h"
#include "fade.h"
#include "game.h"
#include "sound.h"
#include "time.h"

//========================================
// マクロ定義 
//========================================

#define MAX_READY				(3)										// レディの最大オブジェクト数
#define MAX_READY_TEX			(3)										// レディの最大テクスチャ数
#define SetTexReady(nIdx)		(pVtx[0].tex = {1 / 3.0f * nIdx, 0.0f}, pVtx[1].tex = {1 / 3.0f * (nIdx+1), 0.0f}, pVtx[2].tex ={1 / 3.0f * nIdx, 1.0f}, pVtx[3].tex = {1 / 3.0f * (nIdx+1), 1.0f})

//=======================================
// 構造体の定義
//=======================================

// レディの構造体 -----------------------

typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 size;		// サイズ
	bool bDisp;				// 表示するか
	int TexIdx;				// テクスチャ番号
	int Idx;				// 番号
	int nCnt;				// カウント用
	int nCnt2;				// カウント用
	bool bMove;				// 動くか

}Ready;

// レディのテクスチャ構造体 -------------
typedef struct
{
	char FileName[256];		// モデルのファイルネーム
	int TexIdx;				// テクスチャ番号

}ReadyTexInfo;

// レディの移動パターン構造体 -------------
typedef struct
{
	D3DXVECTOR3 StartPos;
	D3DXVECTOR3 EndPos;
	int Speed;
	bool bOnInertia;

}ReadyMove;

//=======================================
// グローバル宣言 
//=======================================

bool bGameStart = false;
LPDIRECT3DTEXTURE9 g_apTextureReady[MAX_READY] = {};
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffReady = NULL;

ReadyTexInfo g_aReadyTex[] =
{ // ファイル名,Idx

	{{"data\\TEXTURE\\readyfont.png"},
	0},

	{{"data\\TEXTURE\\go.png"},
	1},

	{{"data\\TEXTURE\\timeup00.png"},
	2},
};

Ready g_aReady[] =
{ // POS,SIZE,Disp,TexIdx,Idx,Cnt2つ,

	{{ 640.0f, 360.0f, 0.0f },
	{ 80.0f, 100.0f, 0.0f },
	false,
	0,
	0,
	0,
	0,
	true},

	{{ 640.0f, 360.0f, 0.0f },
	{ 500.0f, 100.0f, 0.0f },
	false,
	1,
	0,
	0,
	0,
	false},

	{{ 640.0f, 360.0f, 0.0f },
	{ 100.0f, 100.0f, 0.0f },
	false,
	2,
	0,
	0,
	0,
	false},
};

ReadyMove g_aRM[100] = {};

//===========================================================================
// レディの初期化処理
//===========================================================================
void InitReady(void)
{
	// ローカル変数宣言 -----------------

	VERTEX_2D* pVtx;					//頂点情報へのポインタ
	LPDIRECT3DDEVICE9 pDevice;			//デバイスへのポインタ

	// デバイスの取得 -------------------
	pDevice = GetDevice();

	bGameStart = false;

	for (int nCntReady = 0; nCntReady < MAX_READY; nCntReady++)
	{
		//　テクスチャの読み込み
		D3DXCreateTextureFromFile(pDevice,
			&g_aReadyTex[g_aReady[nCntReady].TexIdx].FileName[0],
			&g_apTextureReady[nCntReady]);
	}

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * MAX_VERTEX * MAX_READY,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffReady,
		NULL);

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffReady->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntReady = 0; nCntReady < MAX_READY; nCntReady++)
	{
		pVtx[0].pos = D3DXVECTOR3(-500, -500, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(-500, -500, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-500, -500, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(-500, -500, 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		if (g_aReady[nCntReady].TexIdx == 0)
		{
		}
		else
		{
			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		}

		pVtx += MAX_VERTEX;
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffReady->Unlock();
	
	ResetReady();
	SetReadyMove(0, { 500.0f, 360.0f, 0.0f }, { 640.0f, 360.0f, 0.0f }, 18, false);
	SetReady(0, 4); 
}

//===========================================================================
// レディの終了処理
//===========================================================================
void UninitReady(void)
{
	for (int nCntTex = 0; nCntTex < MAX_READY_TEX; nCntTex++)
	{
		if (g_apTextureReady[nCntTex] != NULL)
		{
			// テクスチャの破棄
			g_apTextureReady[nCntTex]->Release();
			g_apTextureReady[nCntTex] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffReady != NULL)
	{
		g_pVtxBuffReady->Release();
		g_pVtxBuffReady = NULL;
	}
}

//===========================================================================
// レディの更新処理
//===========================================================================
void UpdateReady(void)
{
	// ローカル変数宣言 -----------------

	VERTEX_2D* pVtx;					//頂点情報へのポインタ
	int nTime = GetTime();
	static bool bfrag[100] = {};

	if (GetKeyboardTrigger(DIK_I) == true)
	{
		g_aReady[0].Idx = 4;
		bfrag[0] = false; 
		g_aReady[0].bDisp = true;
		g_aReady[0].nCnt = 0;
	}

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffReady->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntReady = 0; nCntReady < MAX_READY; nCntReady++)
	{
		if (g_aReady[nCntReady].Idx > -1)
		{ // 状態カウンター

			g_aReady[nCntReady].nCnt++;
		}

		if (g_aReady[nCntReady].nCnt == 60 && g_aReady[nCntReady].Idx > -1)
		{ // 数字をカウントダウン

			g_aReady[nCntReady].Idx--;
			g_aReady[nCntReady].nCnt = 0;
			bfrag[nCntReady] = false;
		}

		if (g_aReady[nCntReady].TexIdx == 0 && g_aReady[nCntReady].Idx < 3)
		{ // 数字テクスチャなら

			// テクスチャ座標の設定
			SetTexReady(g_aReady[nCntReady].Idx);

			if (g_aReady[nCntReady].Idx == -1 && g_aReady[nCntReady].bDisp == true)
			{ // 非表示にしてGOを表示

				g_aReady[nCntReady].bDisp = false;
				SetReady(1, 0);

			}

		}
		if (g_aReady[nCntReady].TexIdx == 1)
		{ // GOテクスチャなら

			if (g_aReady[nCntReady].Idx == -1 && g_aReady[nCntReady].bDisp == true)
			{ // 非表示にしてゲームスタート

				g_aReady[nCntReady].bDisp = false;

				bGameStart = true;
			}
		}
		if (g_aReady[nCntReady].TexIdx == 2)
		{ // TimeUpテクスチャなら

			if (nTime < 0 && g_aReady[nCntReady].bDisp == false)
			{
				SetReady(2, 1);

				bGameStart = false;
			}

			// 時間切れ
			if (nTime < 0 && bGameStart == false && g_aReady[nCntReady].Idx == -1)
			{
				SetFade(MODE_RESULT);
				PlaySound(SOUND_SE_TIMEUP);	// カウントダウン
			}

		}

		if (bfrag[nCntReady] == false && g_aReady[nCntReady].bMove == true)
		{  // 動くOBJなら

			if (MoveReady(0, 0, true) == true)
			{ // 最終地点についたら

				bfrag[nCntReady] = true;
			}
		}
	}


	for (int nCntReady = 0; nCntReady < MAX_READY; nCntReady++)
	{ // 動かす

		pVtx[0].pos = D3DXVECTOR3(g_aReady[nCntReady].pos.x - g_aReady[nCntReady].size.x, g_aReady[nCntReady].pos.y - g_aReady[nCntReady].size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(g_aReady[nCntReady].pos.x + g_aReady[nCntReady].size.x, g_aReady[nCntReady].pos.y - g_aReady[nCntReady].size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(g_aReady[nCntReady].pos.x - g_aReady[nCntReady].size.x, g_aReady[nCntReady].pos.y + g_aReady[nCntReady].size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(g_aReady[nCntReady].pos.x + g_aReady[nCntReady].size.x, g_aReady[nCntReady].pos.y + g_aReady[nCntReady].size.y, 0.0f);

		pVtx += MAX_VERTEX;
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffReady->Unlock();

}

//===========================================================================
// レディの描画処理
//===========================================================================
void DrawReady(void)
{
	// ローカル変数宣言 -----------------

	LPDIRECT3DDEVICE9 pDevice;			// デバイスへのポインタ

	// デバイスの取得 -------------------

	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffReady, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntReady = 0; nCntReady < MAX_READY; nCntReady++)
	{
		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureReady[g_aReady[nCntReady].TexIdx]);

		if (g_aReady[nCntReady].bDisp == true)
		{
			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCntReady * MAX_VERTEX,		// 描画する最初の頂点インデックス
				MAX_POLYGON);				// 描画するプリミティブ数
		}
	}
}

//=============================================================================
// レディの設定
//=============================================================================
void SetReady(int aIdx,int Idx)
{
	if (aIdx < MAX_READY)
	{
		g_aReady[aIdx].Idx = Idx;
		g_aReady[aIdx].bDisp = true;
	}
}

//=============================================================================
// レディのリセット
//=============================================================================
void ResetReady(void)
{

	g_aReady[0] =
	{  // POS,SIZE,Disp,TexIdx,Idx,

		{ 640.0f, 360.0f, 0.0f },
		{ 80.0f, 100.0f, 0.0f },
		false,
		0,
		0,
		0,
		0,
		true
	};

	g_aReady[1] =
	{  // POS,SIZE,Disp,TexIdx,Idx,

		{ 640.0f, 360.0f, 0.0f },
		{ 500.0f, 100.0f, 0.0f },
		false,
		1,
		0,
		0,
		0,
		false
	};

	g_aReady[2] =
	{  // POS,SIZE,Disp,TexIdx,Idx,

		{ 640.0f, 360.0f, 0.0f },
		{ 100.0f, 100.0f, 0.0f },
		false,
		2,
		0,
		0,
		0,
		false
	};
}

//=============================================================================
// 移動パターンの設定
//=============================================================================
void SetReadyMove(int aIdx, D3DXVECTOR3 startPos, D3DXVECTOR3 EndPos, int Speed, bool bOnInertia)
{
	g_aRM[aIdx].StartPos = startPos;
	g_aRM[aIdx].EndPos = EndPos;
	g_aRM[aIdx].Speed = Speed;
	g_aRM[aIdx].bOnInertia = bOnInertia;
}

//=============================================================================
// 移動処理
//=============================================================================
bool MoveReady(int aIdxObj, int aIdxMove, bool bUse)
{
	// ローカル変数宣言 -----------------

	VERTEX_2D* pVtx;					//頂点情報へのポインタ
	static bool bMove[MAX_READY];
	D3DXVECTOR3 move = { 0.0f,0.0f,0.0f };


	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffReady->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += aIdxObj * 4;

	if (bUse == true)
	{
		if (bMove[aIdxObj] == false)
		{
			g_aReady[aIdxObj].pos = { g_aRM[aIdxMove].StartPos.x,g_aRM[aIdxMove].StartPos.y,0.0f };

			bMove[aIdxObj] = true;
			g_aReady[aIdxObj].nCnt2 = 100;
		}
		move = g_aRM[aIdxMove].EndPos - g_aRM[aIdxMove].StartPos;

		g_aReady[aIdxObj].pos += { move.x * (g_aRM[aIdxMove].Speed / 100.0f), move.y * (g_aRM[aIdxMove].Speed / 100.0f), 0.0f };

		g_aReady[aIdxObj].nCnt2 -= (int)g_aRM[aIdxMove].Speed;
	}
	if (g_aReady[aIdxObj].nCnt2 < 0)
	{
		g_aReady[aIdxObj].pos = g_aRM[aIdxMove].EndPos;
		bMove[aIdxObj] = false;
		g_aReady[aIdxObj].nCnt2 = 0;

		return true;
	}
	else
	{
		return false;
	}
}

//=============================================================================
// ゲームの状態を取得
//=============================================================================
bool GetGameStart(void)
{
	return bGameStart;
}
