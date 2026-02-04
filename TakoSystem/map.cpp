//=============================================================================
// 
// マップ [title.cpp]
// Author : Mutsuki Uemura
// 
//=============================================================================
#include "main.h"
#include "map.h"
#include "fade.h"
#include "camera.h"
#include "game.h"
#include "player.h"
#include "debugproc.h"
#include "title.h"

//=======================================
// マクロ定義 
//=======================================

#define	MAX_MAP			(4)						// マップで表示するテクスチャの最大数
#define	MAP_USE_NUM		(6)						// マップで使用するオブジェクト数
#define	PIN_USE_NUM		(2)						// マップで使用するピン数
#define MAP_SIZE		(100)					// マップサイズ
#define MAP_SIZE_INSIDE (INCYLINDER_RADIUS/OUTCYLINDER_RADIUS)
#define MAP_PIN_SIZE	(10)					// ピンサイズ
#define NUM_PIN_SIZE	(8)						// 番号サイズ
#define MAP_CALC_SIZEARRAY(aArray)	(sizeof aArray / sizeof(aArray[0]))	// サイズ比較

//=======================================
// グローバル宣言
//=======================================

LPDIRECT3DTEXTURE9 g_pTextureMap[MAX_MAP] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffMap = NULL;	// 頂点バッファへのポインタ

// テクスチャ情報 -----------------------

MapTextureInfo g_aMapTexInfo[] =
{ // [テクスチャの線の太さ分の調整,ファイルネーム,テクスチャ番号,ずらしたい距離]

	// map
	{10,"data\\TEXTURE\\maptest000.png",0,{0.0f,0.0f}},

	// pin
	{0,"data\\TEXTURE\\mappin000.png",1,{0.0f,0.0f}},

	// numpin
	{0,"data\\TEXTURE\\numpin000.png",2,{0.0f,-17.0f}},

	{0,"data\\TEXTURE\\numpin001.png",3,{0.0f,-17.0f}},
};

// マップの情報 -------------------------

Map g_aMap[] =
{ // [位置,サイズ,色,テクスチャ割り当て番号]

	{{SCREEN_WIDTH - MAP_SIZE - 50,MAP_SIZE + 50,0.0f},
	{MAP_SIZE,MAP_SIZE,0.0f},
	{{1.0f,1.0f,1.0f,1.0f}},
	g_aMapTexInfo[0].TexIdx},

	{{SCREEN_WIDTH - MAP_SIZE - 50,MAP_SIZE + 50,0.0f},
	{MAP_SIZE * MAP_SIZE_INSIDE,MAP_SIZE * MAP_SIZE_INSIDE,0.0f},
	{{1.0f,1.0f,1.0f,1.0f}},
	g_aMapTexInfo[0].TexIdx},

	{{SCREEN_WIDTH - MAP_SIZE - 50,MAP_SIZE + 50,0.0f},
	{MAP_PIN_SIZE,MAP_PIN_SIZE,0.0f},
	{{1.0f,0.0f,0.0f,1.0f}},
	g_aMapTexInfo[1].TexIdx},

	{{SCREEN_WIDTH - MAP_SIZE - 50,MAP_SIZE + 50,0.0f},
	{MAP_PIN_SIZE,MAP_PIN_SIZE,0.0f},
	{{1.0f,1.0f,0.0f,1.0f}},
	g_aMapTexInfo[1].TexIdx},

	{{SCREEN_WIDTH - NUM_PIN_SIZE - 50,NUM_PIN_SIZE + 50,0.0f},
	{NUM_PIN_SIZE,NUM_PIN_SIZE,0.0f},
	{{1.0f,0.0f,0.0f,1.0f}},
	g_aMapTexInfo[2].TexIdx},

	{{SCREEN_WIDTH - NUM_PIN_SIZE - 50,NUM_PIN_SIZE + 50,0.0f},
	{NUM_PIN_SIZE,NUM_PIN_SIZE,0.0f},
	{{0.0f,0.0f,1.0f,1.0f}},
	g_aMapTexInfo[3].TexIdx},
};


//======================================================================== 
// マップの初期化処理
//======================================================================== 
void InitMap(void)
{
	// ローカル変数宣言 -----------------

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ
	VERTEX_2D* pVtx;			// 頂点情報へのポインタ
	Map* pMap = GetMap();
	int PlayerNum = GetPlayerSelect();
	MapTextureInfo* pMapInfo = &g_aMapTexInfo[0];

	// デバイスの取得 -------------------

	pDevice = GetDevice();

	// 構造体の初期化 -------------------

	if (PlayerNum == 1)
	{
		for (int nCntMap = 0; nCntMap < MAP_USE_NUM; nCntMap++)
		{
			pMap[nCntMap].pos =
			{ SCREEN_WIDTH - MAP_SIZE - 50,MAP_SIZE + 50,0.0f };
		}
	}
	else
	{
		for (int nCntMap = 0; nCntMap < MAP_USE_NUM; nCntMap++)
		{
			pMap[nCntMap].pos =
			{ SCREEN_WIDTH / 2,SCREEN_HEIGHT / 2 - 80,0.0f };
		}
	}

	// テクスチャの読み込み -------------

	for (int nCntMap = 0; nCntMap < MAX_MAP; nCntMap++)
	{
		D3DXCreateTextureFromFile(pDevice,
			g_aMapTexInfo[nCntMap].Model_FileName,
			&g_pTextureMap[nCntMap]);
	}

	// 頂点バッファの生成 ---------------

	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAP_USE_NUM,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffMap,
		NULL);

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffMap->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntMap = 0; nCntMap < MAP_USE_NUM; nCntMap++, pMap++)
	{
		pVtx[0].pos = D3DXVECTOR3(pMap->pos.x - (pMap->size.x + pMapInfo[nCntMap].Dif_Texture_Line), pMap->pos.y - (pMap->size.y + pMapInfo[nCntMap].Dif_Texture_Line), 0.0f);	// 右回りで設定する
		pVtx[1].pos = D3DXVECTOR3(pMap->pos.x + (pMap->size.x + pMapInfo[nCntMap].Dif_Texture_Line), pMap->pos.y - (pMap->size.y + pMapInfo[nCntMap].Dif_Texture_Line), 0.0f);	// 2Dの場合Zの値は0にする
		pVtx[2].pos = D3DXVECTOR3(pMap->pos.x - (pMap->size.x + pMapInfo[nCntMap].Dif_Texture_Line), pMap->pos.y + (pMap->size.y + pMapInfo[nCntMap].Dif_Texture_Line), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pMap->pos.x + (pMap->size.x + pMapInfo[nCntMap].Dif_Texture_Line), pMap->pos.y + (pMap->size.y + pMapInfo[nCntMap].Dif_Texture_Line), 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;	// 値は1.0fで固定
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = pMap->col;	// 0~255の値を設定
		pVtx[1].col = pMap->col;
		pVtx[2].col = pMap->col;
		pVtx[3].col = pMap->col;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffMap->Unlock();
}

//======================================================================== 
// マップの終了処理
//======================================================================== 
void UninitMap(void)
{
	// テクスチャの破棄 -----------------

	for (int nCntMap = 0; nCntMap < MAX_MAP; nCntMap++)
	{// マップの数だけ確認する

		if (g_pTextureMap[nCntMap] != NULL)
		{// テクスチャの破棄

			g_pTextureMap[nCntMap]->Release();
			g_pTextureMap[nCntMap] = NULL;
		}
	}

	// 頂点バッファの破棄 ---------------

	if (g_pVtxBuffMap != NULL)
	{
		g_pVtxBuffMap->Release();
		g_pVtxBuffMap = NULL;
	}
}

//======================================================================== 
// マップの更新処理
//======================================================================== 
void UpdateMap(void)
{
	// ローカル変数宣言 -----------------

	VERTEX_2D* pVtx;															// 頂点情報へのポインタ
	D3DXVECTOR3 MapMax = { OUTCYLINDER_RADIUS,0.0f,OUTCYLINDER_RADIUS };		// マップ座標の最大値
	D3DXVECTOR3 Mapmin = { -OUTCYLINDER_RADIUS ,0.0f,-OUTCYLINDER_RADIUS };		// マップ座標の最小値
	D3DXVECTOR3 posRate[MAX_PLAYER] = {};										// 中心地からのプレイヤーの位置の割合
	Player* pPlayer = GetPlayer();
	Map* pMap = GetMap();
	MapTextureInfo* pMapInfo = &g_aMapTexInfo[0];

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffMap->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += 8;

	for (int nCntMap = 2, nCntPlayer = 0; nCntMap < MAP_USE_NUM; nCntMap++, nCntPlayer++)
	{
		if (nCntPlayer >= PIN_USE_NUM)
		{
			nCntPlayer = 0;
		}

		posRate[nCntPlayer] = { pPlayer[nCntPlayer].pos.x / MapMax.x ,pPlayer[nCntPlayer].pos.y,pPlayer[nCntPlayer].pos.z / MapMax.z };

		PrintDebugProc("\nプレイヤーの位置 : [ %f : %f : %f ]\n", posRate[nCntPlayer].x, posRate[nCntPlayer].y, posRate[nCntPlayer].z);

		pVtx[0].pos = D3DXVECTOR3(pMap[nCntMap].pos.x - ((posRate[nCntPlayer].x * MapMax.x) * (MAP_SIZE / MapMax.z)) - pMap[nCntMap].size.x + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.x, pMap[nCntMap].pos.y + ((posRate[nCntPlayer].z * MapMax.z) * (MAP_SIZE / MapMax.z)) - pMap[nCntMap].size.y - (pMap[nCntMap].size.y / 2) + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.y, 0.0f);	// 右回りで設定する
		pVtx[1].pos = D3DXVECTOR3(pMap[nCntMap].pos.x - ((posRate[nCntPlayer].x * MapMax.x) * (MAP_SIZE / MapMax.x)) + pMap[nCntMap].size.x + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.x, pMap[nCntMap].pos.y + ((posRate[nCntPlayer].z * MapMax.z) * (MAP_SIZE / MapMax.z)) - pMap[nCntMap].size.y - (pMap[nCntMap].size.y / 2) + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.y, 0.0f);	// 2Dの場合Zの値は0にする
		pVtx[2].pos = D3DXVECTOR3(pMap[nCntMap].pos.x - ((posRate[nCntPlayer].x * MapMax.x) * (MAP_SIZE / MapMax.x)) - pMap[nCntMap].size.x + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.x, pMap[nCntMap].pos.y + ((posRate[nCntPlayer].z * MapMax.z) * (MAP_SIZE / MapMax.z)) + pMap[nCntMap].size.y - (pMap[nCntMap].size.y / 2) + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pMap[nCntMap].pos.x - ((posRate[nCntPlayer].x * MapMax.x) * (MAP_SIZE / MapMax.x)) + pMap[nCntMap].size.x + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.x, pMap[nCntMap].pos.y + ((posRate[nCntPlayer].z * MapMax.z) * (MAP_SIZE / MapMax.z)) + pMap[nCntMap].size.y - (pMap[nCntMap].size.y / 2) + g_aMapTexInfo[pMap[nCntMap].TexIdx].shiftpos.y, 0.0f);

		pVtx += 4;		// 頂点データのポインタを4つ分進める
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffMap->Unlock();
}

//======================================================================== 
// マップの描画処理
//======================================================================== 
void DrawMap(void)
{
	// ローカル変数宣言 -----------------

	LPDIRECT3DDEVICE9 pDevice;	// デバイスへのポインタ
	Map* pMap = GetMap();


	// デバイスの取得
	pDevice = GetDevice();

	// 減算合成の設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);		// アルファブレンドの設定1
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);	// アルファブレンドの設定2
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);		// アルファブレンドの設定3



		// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffMap, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	for (int nCntMap = 0; nCntMap < MAP_USE_NUM; nCntMap++)
	{// 敵の最大数まで繰り返す

		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureMap[pMap[nCntMap].TexIdx]);

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntMap * 4, 2);

	}
	// ブレンディング(減算合成)を元に戻す 
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// アルファブレンドの設定1
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定2
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// アルファブレンドの設定3
}
//=============================================================================
// マップの取得処理
//=============================================================================
Map* GetMap(void)
{
	return &g_aMap[0];
}