//=============================================================================
// 
// 配置物 [object.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "object.h"
#include "explosion.h"
#include "particle.h"
#include "player.h"
#include "shadow.h"
#include "houselight.h"
#include "meshcylinder.h"
#include "meshwall.h"
#include "meshfield.h"
#include "tree.h"
#include "goalarrow.h"
#include "debugproc.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FIRST_SCALE				(D3DXVECTOR3(1.0f, 1.0f, 1.0f))			// 初期拡大率
#define SMOKE_MOVE				(D3DXVECTOR3(0.0f, 1.5f, 0.0f))			// 煙の移動量
#define SMOKE_COUNT				(30)									// 煙の設定間隔
#define HOUSE_DISTANCE			(130.0f)								// 転んだ時に明かりがつく距離
#define HOUSE_AUTODISTANCE		(60.0f)									// 家の明かりが変わる距離
#define HOUSE_LIGHT_AUTOOFF		(300)									// 自動的に明かりが消えるカウント
#define HOUSE_LIGHT_AUTOON		(800)									// 自動的に明かりがつくランダム
#define HOUSE_CYLINDER_DISTANCE	(55.0f)									// 家の前の円柱の距離
#define HOUSE_CYLINDER_BLOCK	(D3DXVECTOR2(32.0f, 1.0f))				// 家の前の円柱の分割数
#define HOUSE_CYLINDER_SIZE		(D3DXVECTOR2(20.0f, 40.0f))				// 家の前の円柱のサイズ
#define HOUSE_SHADOW_SIZE		(1.7f)									// 家の影のサイズ(何倍にするか)
#define HOUSE_SHADOW_ALPHA		(0.9f)									// 家の影の透明度
#define HOUSE_SMALL_PRESENT		(15)									// 小さい家の規定のプレゼント量
#define HOUSE_MIDIUM_PRESENT	(30)									// 中くらい家の規定のプレゼント量
#define HOUSE_BIG_PRESENT		(50)									// 大きい家の規定のプレゼント量
#define OBS_CYLINDER_DISTANCE	(80.0f)									// 展望台の円柱の距離
#define OBS_CYLINDER_BLOCK		(D3DXVECTOR2(32.0f, 1.0f))				// 展望台の円柱の分割数
#define OBS_CYLINDER_SIZE		(D3DXVECTOR2(70.0f, 30.0f))				// 展望台の円柱のサイズ
#define OBS_SHADOW_SIZE			(70.0f)									// 展望台の影のサイズ
#define OBS_SHADOW_ALPHA		(0.5f)									// 展望台の影の透明度
#define TREE_SHADOW_SIZE		(60.0f)									// 木の影のサイズ
#define TREE_SHADOW_ALPHA		(0.7f)									// 木の影の透明度

//*****************************************************************************
// グローバル変数
//*****************************************************************************
ObjectModel g_aObjectModel[MAX_OBJECTMODEL];					// 配置物モデルの情報
Object g_aObject[MAX_OBJECT];									// 配置物の情報
char* g_apFilenameObject[MAX_OBJECTMODEL] = {};					// モデルファイルへのポインタ
int g_nNumObject = 0;											// オブジェクトの総数
int g_nGoalIdx = -1;											// ゴール番号
int g_nRandGoal = 0;											// ゴール番号のランダム
int g_nPresentIdx[MAX_OBJECT] = {};								// プレゼントを持っている配置物のインデックス
int g_nNumPresentHouse = 0;										// プレゼントをもらっている配置物の総数
int g_nNumPresent[MAX_OBJECT] = {};								// プレゼントをもらっている配置物のプレゼントの数
int g_nPresentNumClose[MAX_OBJECT] = {};						// 持っているプレゼントの数がどれだけ近いか
D3DXVECTOR3 g_posPresentHouse[MAX_OBJECT] = {};					// プレゼントを持っている配置物の場所

//=============================================================================
// 配置物の初期化処理
//=============================================================================
void InitObject(const char* pStr)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	int nNumVtx;			// 頂点数
	DWORD dwSizeFVF;		// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;			// 頂点バッファへのポインタ

	// 配置物の情報の初期化
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		g_aObject[nCntObject].pos = FIRST_POS;
		g_aObject[nCntObject].posOff = FIRST_POS;
		g_aObject[nCntObject].move = FIRST_POS;
		g_aObject[nCntObject].rot = FIRST_POS;
		g_aObject[nCntObject].scale = FIRST_SCALE;
		g_aObject[nCntObject].state = OBJECTSTATE_NONE;
		g_aObject[nCntObject].nCounterState = 0;
		g_aObject[nCntObject].nType = 0;
		g_aObject[nCntObject].nCounterSmoke = 0;
		g_aObject[nCntObject].nCounterLight = 0;
		g_aObject[nCntObject].nIdxHouseLight[0] = -1;
		g_aObject[nCntObject].nIdxHouseLight[1] = -1;
		g_aObject[nCntObject].nIdxMeshCylinder = -1;
		g_aObject[nCntObject].nIdxGoal = -1;
		g_aObject[nCntObject].nIdxShadow = -1;
		g_aObject[nCntObject].nDefPresent = 0;
		g_aObject[nCntObject].nNumPresent = 0;
		g_aObject[nCntObject].nNumPresentOld = 0;
		g_aObject[nCntObject].bUse = false;
		g_aObject[nCntObject].bCollision = true;

		g_nPresentIdx[nCntObject] = -1;
	}

	g_nNumObject = 0;
	g_nGoalIdx = -1;
	g_nRandGoal = 0;
	g_nNumPresentHouse = 0;

	// モデルの読み込み
	LoadStage(pStr);

	// 配置物読み込み
	LoadObject(pStr);

	// 配置物モデルの情報の初期化
	for (int nCntObject = 0; nCntObject < MAX_OBJECTMODEL; nCntObject++)
	{
		// 最大値最小値の初期化
		g_aObjectModel[nCntObject].VtxMax = D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f);
		g_aObjectModel[nCntObject].VtxMin = D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f);

		// Xファイルの読み込み
		if (FAILED(D3DXLoadMeshFromX(g_apFilenameObject[nCntObject],
			D3DXMESH_SYSTEMMEM,
			pDevice,
			NULL,
			&g_aObjectModel[nCntObject].pBuffMat,
			NULL,
			&g_aObjectModel[nCntObject].dwNumMat,
			&g_aObjectModel[nCntObject].pMesh)))
		{
			return;
		}

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)g_aObjectModel[nCntObject].pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)g_aObjectModel[nCntObject].dwNumMat; nCntMat++)
		{
			if (pMat[nCntMat].pTextureFilename != NULL)
			{// テクスチャファイルが存在する
				D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &g_aObjectModel[nCntObject].apTexture[nCntMat]);
			}
		}

		// 頂点数を取得
		nNumVtx = g_aObjectModel[nCntObject].pMesh->GetNumVertices();

		// 頂点フォーマットのサイズを取得
		dwSizeFVF = D3DXGetFVFVertexSize(g_aObjectModel[nCntObject].pMesh->GetFVF());

		// 頂点バッファをロック
		g_aObjectModel[nCntObject].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

		for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++)
		{
			D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;		// 頂点座標の代入

			if (vtx.x > g_aObjectModel[nCntObject].VtxMax.x)
			{// Xの最大値
				g_aObjectModel[nCntObject].VtxMax.x = vtx.x;
			}

			if (vtx.x < g_aObjectModel[nCntObject].VtxMin.x)
			{// Xの最小値
				g_aObjectModel[nCntObject].VtxMin.x = vtx.x;
			}

			if (vtx.y > g_aObjectModel[nCntObject].VtxMax.y)
			{// Yの最大値
				g_aObjectModel[nCntObject].VtxMax.y = vtx.y;
			}

			if (vtx.y < g_aObjectModel[nCntObject].VtxMin.y)
			{// Yの最小値
				g_aObjectModel[nCntObject].VtxMin.y = vtx.y;
			}

			if (vtx.z > g_aObjectModel[nCntObject].VtxMax.z)
			{// Zの最大値
				g_aObjectModel[nCntObject].VtxMax.z = vtx.z;
			}

			if (vtx.z < g_aObjectModel[nCntObject].VtxMin.z)
			{// Zの最小値
				g_aObjectModel[nCntObject].VtxMin.z = vtx.z;
			}

			pVtxBuff += dwSizeFVF;		// 頂点バッファのサイズ分ポインタを進める
		}

		// 頂点バッファをアンロック
		g_aObjectModel[nCntObject].pMesh->UnlockVertexBuffer();
	}
}

//=============================================================================
// 配置物の終了処理
//=============================================================================
void UninitObject(void)
{
	for (int nCntObjectModel = 0; nCntObjectModel < MAX_OBJECTMODEL; nCntObjectModel++)
	{
		// メッシュの破棄
		if (g_aObjectModel[nCntObjectModel].pMesh != NULL)
		{
			g_aObjectModel[nCntObjectModel].pMesh->Release();
			g_aObjectModel[nCntObjectModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (g_aObjectModel[nCntObjectModel].pBuffMat != NULL)
		{
			g_aObjectModel[nCntObjectModel].pBuffMat->Release();
			g_aObjectModel[nCntObjectModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntObject = 0; nCntObject < (int)g_aObjectModel[nCntObjectModel].dwNumMat; nCntObject++)
		{
			if (g_aObjectModel[nCntObjectModel].apTexture[nCntObject] != NULL)
			{
				g_aObjectModel[nCntObjectModel].apTexture[nCntObject]->Release();
				g_aObjectModel[nCntObjectModel].apTexture[nCntObject] = NULL;
			}
		}
	}
}

//=============================================================================
// 配置物の更新処理
//=============================================================================
void UpdateObject(void)
{
	PrintDebugProc("配置物の数 : %d\n", g_nNumObject);

	Player* pPlayer = GetPlayer();
	MeshCylinder* pMeshCylinder = GetMeshCylinder();

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == true)
		{// 使われているとき
			g_aObject[nCntObject].nNumPresentOld = g_aObject[nCntObject].nNumPresent;

			switch (g_aObject[nCntObject].state)
			{
			case OBJECTSTATE_NONE:			// 動かない
				g_aObject[nCntObject].pos = g_aObject[nCntObject].posOff;
				g_aObject[nCntObject].move = FIRST_POS;
				g_aObject[nCntObject].scale = FIRST_SCALE;

				break;

			case OBJECTSTATE_SCALE:			// スケール
				// 拡大
				g_aObject[nCntObject].move.x += 0.75f;
				g_aObject[nCntObject].move.y += 0.75f;
				g_aObject[nCntObject].move.z += 0.75f;

				g_aObject[nCntObject].scale.x += sinf(g_aObject[nCntObject].move.x) * 0.01f;
				g_aObject[nCntObject].scale.y += sinf(g_aObject[nCntObject].move.y) * 0.01f;
				g_aObject[nCntObject].scale.z += sinf(g_aObject[nCntObject].move.z) * 0.01f;

				g_aObject[nCntObject].nCounterState--;

				if (g_aObject[nCntObject].nCounterState < 0)
				{// カウントがなくなった
					g_aObject[nCntObject].state = OBJECTSTATE_NONE;
					g_aObject[nCntObject].nCounterState = 0;

					g_aObject[nCntObject].move = FIRST_POS;
					g_aObject[nCntObject].scale = FIRST_SCALE;
				}

				break;
			}

			float fWidth, fDepth;

			if (g_aObject[nCntObject].rot.y == 0.0f || g_aObject[nCntObject].rot.y == D3DX_PI)
			{// そのままの向き
				fWidth = (g_aObjectModel[g_aObject[nCntObject].nType].VtxMax.x -
					g_aObjectModel[g_aObject[nCntObject].nType].VtxMin.x) * 0.5f;
				fDepth = (g_aObjectModel[g_aObject[nCntObject].nType].VtxMax.z -
					g_aObjectModel[g_aObject[nCntObject].nType].VtxMin.z) * 0.5f;
			}
			else
			{// 角度が変わっている
				fWidth = (g_aObjectModel[g_aObject[nCntObject].nType].VtxMax.z -
					g_aObjectModel[g_aObject[nCntObject].nType].VtxMin.z) * 0.5f;
				fDepth = (g_aObjectModel[g_aObject[nCntObject].nType].VtxMax.x -
					g_aObjectModel[g_aObject[nCntObject].nType].VtxMin.x) * 0.5f;
			}

			switch (g_aObject[nCntObject].nType)
			{
			case 0:
				// 影
				SetPositionShadow(g_aObject[nCntObject].nIdxShadow, g_aObject[nCntObject].pos,
					fWidth * HOUSE_SHADOW_SIZE * g_aObject[nCntObject].scale.x, fDepth * HOUSE_SHADOW_SIZE * g_aObject[nCntObject].scale.z, HOUSE_SHADOW_ALPHA);

				break;

			case 1:
				// 影
				SetPositionShadow(g_aObject[nCntObject].nIdxShadow, g_aObject[nCntObject].pos,
					fWidth * HOUSE_SHADOW_SIZE * g_aObject[nCntObject].scale.x, fDepth * HOUSE_SHADOW_SIZE * g_aObject[nCntObject].scale.z, HOUSE_SHADOW_ALPHA);

				break;

			case 2:		
				// 煙突の煙
				g_aObject[nCntObject].nCounterSmoke++;

				if (g_aObject[nCntObject].nCounterSmoke % SMOKE_COUNT == 0)
				{// カウントが間隔にあった
					float fPosX = sinf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 30.0f;
					float fPosZ = cosf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 30.0f;

					SetExplosion(D3DXVECTOR3(g_aObject[nCntObject].pos.x + fPosX, 120.0f, g_aObject[nCntObject].pos.z + fPosZ), SMOKE_MOVE, 10.0f, EXPLOSIONTYPE_0);
				}

				// 影
				SetPositionShadow(g_aObject[nCntObject].nIdxShadow, g_aObject[nCntObject].pos,
					fWidth * HOUSE_SHADOW_SIZE * g_aObject[nCntObject].scale.x, fDepth * HOUSE_SHADOW_SIZE * g_aObject[nCntObject].scale.z, HOUSE_SHADOW_ALPHA);

				break;

			case 7:
				SetPositionShadow(g_aObject[nCntObject].nIdxShadow, g_aObject[nCntObject].pos,
					TREE_SHADOW_SIZE, TREE_SHADOW_SIZE, TREE_SHADOW_ALPHA);

				break;

			case 9:
				// 影
				SetPositionShadow(g_aObject[nCntObject].nIdxShadow, g_aObject[nCntObject].pos,
					OBS_SHADOW_SIZE, OBS_SHADOW_SIZE, OBS_SHADOW_ALPHA);

				break;

			case 10:
				// 影
				SetPositionShadow(g_aObject[nCntObject].nIdxShadow, g_aObject[nCntObject].pos,
					fWidth * 1.5f, fDepth * 2.0f, HOUSE_SHADOW_ALPHA);

				break;
			}

			if ((g_aObject[nCntObject].nType == 0 || g_aObject[nCntObject].nType == 1 || g_aObject[nCntObject].nType == 2))
			{// 家
				float fLength = (pPlayer->pos.x - g_aObject[nCntObject].pos.x) * (pPlayer->pos.x - g_aObject[nCntObject].pos.x) +
					(pPlayer->pos.z - g_aObject[nCntObject].pos.z) * (pPlayer->pos.z - g_aObject[nCntObject].pos.z);

				float fHouseRadius = (g_aObjectModel[g_aObject[nCntObject].nType].VtxMax.x + (abs)((int)g_aObjectModel[g_aObject[nCntObject].nType].VtxMin.x)) / 2;

				if (fLength <= ((pPlayer->fRadius + fHouseRadius + HOUSE_DISTANCE) * (pPlayer->fRadius + fHouseRadius + HOUSE_DISTANCE)))
				{// 家の範囲内
					if (pPlayer->motionType == MOTIONTYPE_LANDINGICE || pPlayer->motionTypeBlend == MOTIONTYPE_LANDINGICE)
					{// 氷の上に着地

						SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[0], true);
						SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[1], true);

						g_aObject[nCntObject].nCounterLight = HOUSE_LIGHT_AUTOOFF;
					}

					if (pPlayer->bShot == true)
					{// 連打しすぎてたら周りの家が起きる
						SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[0], true);
						SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[1], true);

						g_aObject[nCntObject].nCounterLight = HOUSE_LIGHT_AUTOOFF;
					}
				}

				if (rand() % HOUSE_LIGHT_AUTOON == 0 && GetHouseLightONOFF(nCntObject) == false)
				{// ランダムなタイミング
					SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[0], true);
					SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[1], true);

					g_aObject[nCntObject].nCounterLight = rand() % (HOUSE_LIGHT_AUTOOFF / 2) + HOUSE_LIGHT_AUTOOFF;
				}

				g_aObject[nCntObject].nCounterLight--;

				if (g_aObject[nCntObject].nCounterLight <= 0)
				{// カウントがなくなったらライトをOFF
					SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[0], false);
					SetHouseLightONOFF(g_aObject[nCntObject].nIdxHouseLight[1], false);

					g_aObject[nCntObject].nCounterLight = 0;
				}
			}

			if (pPlayer->nMaxPresent == -1)
			{// プレゼントがなくなっていたら
				if (g_aObject[nCntObject].nIdxGoal == g_nGoalIdx)
				{// ゴール
					GoalArrowONOFF(true);
				}
			}
		}
	}

	PrintDebugProc("ゴール番号 : [ %d ]\n", g_nGoalIdx);
}

//=============================================================================
// 配置物の描画処理
//=============================================================================
void DrawObject(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == true)
		{// 使用されているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aObject[nCntObject].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aObject[nCntObject].rot.y, g_aObject[nCntObject].rot.x, g_aObject[nCntObject].rot.z);
			D3DXMatrixMultiply(&g_aObject[nCntObject].mtxWorld, &g_aObject[nCntObject].mtxWorld, &mtxRot);

			// 拡大率を反映
			D3DXMatrixScaling(&mtxScale, g_aObject[nCntObject].scale.x, g_aObject[nCntObject].scale.y, g_aObject[nCntObject].scale.z);
			D3DXMatrixMultiply(&g_aObject[nCntObject].mtxWorld, &g_aObject[nCntObject].mtxWorld, &mtxScale);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aObject[nCntObject].pos.x, g_aObject[nCntObject].pos.y, g_aObject[nCntObject].pos.z);
			D3DXMatrixMultiply(&g_aObject[nCntObject].mtxWorld, &g_aObject[nCntObject].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aObject[nCntObject].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_aObjectModel[g_aObject[nCntObject].nType].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_aObjectModel[g_aObject[nCntObject].nType].dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_aObjectModel[g_aObject[nCntObject].nType].apTexture[nCntMat]);

				// モデルパーツの描画
				g_aObjectModel[g_aObject[nCntObject].nType].pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// 配置物の設定処理
//=============================================================================
void SetObject(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdx, bool bCollision)
{
	float fPosX = 0.0f;
	float fPosZ = 0.0f;

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == false)
		{// 配置物が使用されていない
			g_aObject[nCntObject].pos = pos;
			g_aObject[nCntObject].posOff = pos;
			g_aObject[nCntObject].move = FIRST_POS;
			g_aObject[nCntObject].rot = rot;
			g_aObject[nCntObject].scale = FIRST_SCALE;
			g_aObject[nCntObject].state = OBJECTSTATE_NONE;
			g_aObject[nCntObject].nCounterState = 0;
			g_aObject[nCntObject].nType = nIdx;
			g_aObject[nCntObject].nCounterSmoke = 0;
			g_aObject[nCntObject].nCounterLight = 0;
			g_aObject[nCntObject].nIdxHouseLight[0] = -1;
			g_aObject[nCntObject].nIdxHouseLight[1] = -1;
			g_aObject[nCntObject].nIdxShadow = -1;
			g_aObject[nCntObject].nNumPresent = 0;
			g_aObject[nCntObject].nNumPresentOld = 0;
			g_aObject[nCntObject].bCollision = bCollision;
			g_aObject[nCntObject].bUse = true;		// 使用している状態にする

			switch (nIdx)
			{
			case 0:
				// 家の明かり設定
				fPosX = sinf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 51.0f;
				fPosZ = cosf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 51.0f;

				g_aObject[nCntObject].nIdxHouseLight[0] =
					SetHouseLight(D3DXVECTOR3(pos.x + fPosX, 25.0f, pos.z + fPosZ), D3DXVECTOR3(0.0f, rot.y + (D3DX_PI * 0.5f), 0.0f), 30.0f, 23.0f);

				fPosX = sinf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 112.0f;
				fPosZ = cosf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 112.0f;

				g_aObject[nCntObject].nIdxHouseLight[1] =
					SetHouseLight(D3DXVECTOR3(pos.x + fPosX, 25.0f, pos.z + fPosZ), D3DXVECTOR3(0.0f, rot.y - (D3DX_PI * 0.5f), 0.0f), 30.0f, 23.0f);

				g_aObject[nCntObject].nDefPresent = HOUSE_BIG_PRESENT;

				g_aObject[nCntObject].nIdxShadow = SetShadow(SHADOWTYPE_SQUARE);

				break;

			case 1:
				// 家の明かり設定
				fPosX = sinf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 91.5f;
				fPosZ = cosf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 91.5f;

				g_aObject[nCntObject].nIdxHouseLight[0] =
					SetHouseLight(D3DXVECTOR3(pos.x + fPosX, 27.0f, pos.z + fPosZ), D3DXVECTOR3(0.0f, rot.y + (D3DX_PI * 0.5f), 0.0f), 32.0f, 19.0f);

				fPosX = sinf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 91.5f;
				fPosZ = cosf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 91.5f;

				g_aObject[nCntObject].nIdxHouseLight[1] =
					SetHouseLight(D3DXVECTOR3(pos.x + fPosX, 27.0f, pos.z + fPosZ), D3DXVECTOR3(0.0f, rot.y - (D3DX_PI * 0.5f), 0.0f), 32.0f, 19.0f);

				g_aObject[nCntObject].nDefPresent = HOUSE_MIDIUM_PRESENT;

				g_aObject[nCntObject].nIdxShadow = SetShadow(SHADOWTYPE_SQUARE);

				break;

			case 2:
				// 家の明かり設定
				fPosX = sinf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 41.5f;
				fPosZ = cosf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 41.5f;

				g_aObject[nCntObject].nIdxHouseLight[0] =
					SetHouseLight(D3DXVECTOR3(pos.x + fPosX, 25.0f, pos.z + fPosZ), D3DXVECTOR3(0.0f, rot.y + (D3DX_PI * 0.5f), 0.0f), 36.0f, 16.0f);

				fPosX = sinf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 41.5f;
				fPosZ = cosf((D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y) * 41.5f;

				g_aObject[nCntObject].nIdxHouseLight[1] =
					SetHouseLight(D3DXVECTOR3(pos.x + fPosX, 25.0f, pos.z + fPosZ), D3DXVECTOR3(0.0f, rot.y - (D3DX_PI * 0.5f), 0.0f), 36.0f, 16.0f);

				g_aObject[nCntObject].nDefPresent = HOUSE_SMALL_PRESENT;

				g_aObject[nCntObject].nIdxShadow = SetShadow(SHADOWTYPE_SQUARE);

				break;

			case 7:
				g_aObject[nCntObject].nIdxShadow = SetShadow(SHADOWTYPE_CIRCLE);

				break;

			case 9:
				// 展望台設定
				g_aObject[nCntObject].nIdxGoal = g_nRandGoal;
				g_nRandGoal++;

				g_nGoalIdx = rand() % g_nRandGoal;

				// シリンダー設置
				fPosX = sinf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y - 0.25f) * OBS_CYLINDER_DISTANCE;
				fPosZ = cosf((-D3DX_PI * 0.5f) + g_aObject[nCntObject].rot.y - 0.25f) * OBS_CYLINDER_DISTANCE;

				g_aObject[nCntObject].nIdxMeshCylinder =
					SetMeshCylinder(D3DXVECTOR3(pos.x, 0.0f, pos.z), FIRST_POS, OBS_CYLINDER_BLOCK, OBS_CYLINDER_SIZE, YELLOW_VTX);

				if (g_aObject[nCntObject].nIdxGoal == g_nGoalIdx)
				{// ゴールの位置を設定
					MeshCylinder* pMeshCylinder = GetMeshCylinder();

					SetGoalArrow(pMeshCylinder[g_aObject[nCntObject].nIdxMeshCylinder].pos);
				}

				g_aObject[nCntObject].nIdxShadow = SetShadow(SHADOWTYPE_CIRCLE);

				break;

			case 10:
				g_aObject[nCntObject].nIdxShadow = SetShadow(SHADOWTYPE_SQUARE);

				break;
			}

			g_nNumObject++;

			break;
		}
	}
}

//=============================================================================
// 配置物のランダム設定処理
//=============================================================================
void SetObjectRandom(int nType, D3DXVECTOR3 posMin, D3DXVECTOR3 posMax, int nAmount)
{
	D3DXVECTOR3 pos, rot;

	for (int nCntObject = 0; nCntObject < nAmount; nCntObject++)
	{
		pos.x = rand() % (int)(posMax.x - posMin.x) + posMin.x;
		pos.y = 0.0f;
		pos.z = rand() % (int)(posMax.z - posMin.z) + posMin.z;

		rot.x = 0.0f;
		rot.y = (float)(rand() % 360) / 360.0f;
		rot.z = 0.0f;

		SetObject(pos, rot, nType, false);

		if (nType == 7)
		{// 木だったらもう片方も同じ場所に生成
			SetObject(pos, rot, 8, false);
		}
		else if (nType == 8)
		{// 上の逆
			SetObject(pos, rot, 7, false);
		}
	}

}

//=============================================================================
// 配置物の当たり判定
//=============================================================================
bool CollisionObject(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, float fRadius, float fHeight)
{
	bool bLand = false;		// 着地しているか
	Object* pObject = GetObjectAll();

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{
		if (pObject->bUse == true && pObject->bCollision == true)
		{// 使用されているとき
			float fLengthX = 0.0f;
			float fLengthZ = 0.0f;

			if (pObject->rot.y == 0.0f || pObject->rot.y == D3DX_PI)
			{// そのままの向き
				fLengthX = (g_aObjectModel[pObject->nType].VtxMax.x - g_aObjectModel[pObject->nType].VtxMin.x) / 2.0f;
				fLengthZ = (g_aObjectModel[pObject->nType].VtxMax.z - g_aObjectModel[pObject->nType].VtxMin.z) / 2.0f;
			}
			else
			{// 角度が変わっている
				fLengthX = (g_aObjectModel[pObject->nType].VtxMax.z - g_aObjectModel[pObject->nType].VtxMin.z) / 2.0f;
				fLengthZ = (g_aObjectModel[pObject->nType].VtxMax.x - g_aObjectModel[pObject->nType].VtxMin.x) / 2.0f;
			}

			if ((pPos->x + fRadius > pObject->posOff.x - fLengthX) &&
				(pPos->x - fRadius < pObject->posOff.x + fLengthX) &&
				(pObject->posOff.z - fLengthZ - fRadius <= pPos->z) &&
				(pObject->posOff.z + fLengthZ + fRadius >= pPos->z))
#if 0
				&&
				(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight <= pPos->y) &&
				(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y >= pPos->y))
#endif
			{// 現在の位置が配置物の範囲内
				if (((pObject->posOff.x - fLengthX - fRadius == pPosOld->x) &&
					(pObject->posOff.x + fLengthX + fRadius > pPosOld->x)) ||
					((pObject->posOff.x + fLengthX + fRadius == pPosOld->x) &&
						(pObject->posOff.x - fLengthX - fRadius < pPosOld->x)))
				{// 前回の位置が配置物の位置なら奥行判定しない

				}
				else if (((pObject->posOff.z + fLengthZ + fRadius <= pPosOld->z) &&
					(pObject->posOff.z + fLengthZ + fRadius >= pPos->z)))
				{// 奥からの当たり判定
					pPos->z = pObject->posOff.z + fLengthZ + fRadius;
					pMove->z = 0.0f;
				}
				else if (((pObject->posOff.z - fLengthZ - fRadius >= pPosOld->z) &&
					(pObject->posOff.z - fLengthZ - fRadius <= pPos->z)))
				{// 手前からの当たり判定
					pPos->z = pObject->posOff.z - fLengthZ - fRadius;
					pMove->z = 0.0f;
				}

				if (((pObject->posOff.z - fLengthZ - fRadius == pPosOld->z) &&
					(pObject->posOff.z + fLengthZ + fRadius > pPosOld->z)) ||
					((pObject->posOff.z + fLengthZ + fRadius == pPosOld->z) &&
						(pObject->posOff.z - fLengthZ - fRadius < pPosOld->z)))
				{// 前回の位置が配置物の位置なら左右判定しない

				}
				else if ((pObject->posOff.x - fLengthX - fRadius >= pPosOld->x) &&
					(pObject->posOff.x - fLengthX - fRadius <= pPos->x))
				{// 左からの当たり判定
					pPos->x = pObject->posOff.x - fLengthX - fRadius;
					pMove->x = 0.0f;
				}
				else if ((pObject->posOff.x + fLengthX + fRadius <= pPosOld->x) &&
					(pObject->posOff.x + fLengthX + fRadius >= pPos->x))
				{// 右からの当たり判定
					pPos->x = pObject->posOff.x + fLengthX + fRadius;
					pMove->x = 0.0f;
				}
#if 0		// 今回はなし
				else if ((pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight >= pPosOld->y) &&
					(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight <= pPos->y))
				{// 下からの当たり判定
					pPos->y = pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMin.y - fHeight;
					pMove->y = -0.5f;							// 移動量を0にする
				}
				else if ((pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y <= pPosOld->y) &&
					(pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y >= pPos->y))
				{// 上からの当たり判定
					pPos->y = pObject->posOff.y + g_aObjectModel[pObject->nType].VtxMax.y;
					pMove->y = 0.0f;							// 移動量を0にする

					bLand = true;
				}
#endif
			}
		}
	}

	return bLand;
}

//=============================================================================
// 配置物モデル読み込み
//=============================================================================
void LoadObject(const char* pStr)
{
	// ローカル変数宣言
	FILE* pFile;
	char aString[256] = {};				// ファイルのテキスト読み込み
	char aTrash[256] = {};				// ゴミ箱
	int nNumModel = 0;					// モデルの読み込み数
	char aModelName[64][256] = {};		// モデルの名前

	// セット用の変数
	int nIdx = 0;			// テクスチャやモデルの種類
	int nCollision = 0;		// 当たり判定するかどうか
	bool bCollision = true;	// 当たり判定するかどうか
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き

	pFile = fopen(pStr, "r");

	if (pFile != NULL)
	{// ファイルが開けた場合
		fscanf(pFile, "%[^SCRIPT]", &aTrash[0]);

		while (1)
		{
			fscanf(pFile, "%s", &aString[0]);

			if (aString[0] == '#')
			{// コメントは無視
				fgets(&aTrash[0], ONE_LINE, pFile);

				continue;
			}

			if (strcmp(&aString[0], "NUM_MODEL") == 0)
			{// モデル数の読み込み
				fscanf(pFile, " = %d", &nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
				{
					if (nCntModel > 0)
					{// 2回目以降のMODEL_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "MODEL_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aModelName[nCntModel][0]);		// モデルのパス
					g_apFilenameObject[nCntModel] = &aModelName[nCntModel][0];

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				fscanf(pFile, "%[^MODELSET]", &aTrash[0]);

				continue;
			}

			if (strcmp(&aString[0], "MODELSET") == 0)
			{// 配置物の設置
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "TYPE") == 0)
				{// テクスチャやモデルの種類
					fscanf(pFile, " = %d", &nIdx);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "POS") == 0)
				{// 位置
					fscanf(pFile, " = %f %f %f", &pos.x, &pos.y, &pos.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "ROT") == 0)
				{// 向き
					fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "COLLISION") == 0)
				{// ループするかどうか
					fscanf(pFile, " = %d", &nCollision);

					// ループの真偽を代入
					if (nCollision == 0)
					{// false
						bCollision = false;
					}
					else if (nCollision == 1)
					{// true
						bCollision = true;
					}

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				SetObject(pos, D3DXVECTOR3(D3DX_PI * rot.x / 180.0f, D3DX_PI * rot.y / 180.0f, D3DX_PI * rot.z / 180.0f), nIdx, bCollision);
			}

			if (strcmp(&aString[0], "END_SCRIPT") == 0)
			{// ファイルの読み込みが完了
				break;
			}
		}

		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合
		printf("ファイルを開けませんでした");
	}
}

//=============================================================================
// ステージ読み込み
//=============================================================================
void LoadStage(const char* pStr)
{
	// ローカル変数宣言
	FILE* pFile;
	char aString[256] = {};				// ファイルのテキスト読み込み
	char aTrash[256] = {};				// ゴミ箱
	int nNumTexture = 0;				// テクスチャの読み込み数
	char aTextureName[64][256] = {};	// テクスチャの名前
	int nNumModel = 0;					// モデルの読み込み数
	char aModelName[64][256] = {};		// モデルの名前

	// セット用の変数
	int nIdx = 0;			// テクスチャやモデルの種類
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 向き
	int nBlockX = 0;		// 分割数(X)
	int nBlockZ = 0;		// 分割数(Z)
	float fWidth = 0.0f;	// 大きさ(X)(幅)
	float fDepth = 0.0f;	// 大きさ(Z)(奥行)

	pFile = fopen(pStr, "r");

	if (pFile != NULL)
	{// ファイルが開けた場合
		fscanf(pFile, "%[^SCRIPT]", &aTrash[0]);

		while (1)
		{
			fscanf(pFile, "%s", &aString[0]);

			if (aString[0] == '#')
			{// コメントは無視
				fgets(&aTrash[0], ONE_LINE, pFile);

				continue;
			}

			if (strcmp(&aString[0], "NUM_TEXTURE") == 0)
			{// テクスチャ数の読み込み
				fscanf(pFile, " = %d", &nNumTexture);

				continue;
			}

			if (strcmp(&aString[0], "TEXTURE_FILENAME") == 0)
			{// テクスチャの名前読み込み
				for (int nCntTexture = 0; nCntTexture < nNumTexture; nCntTexture++)
				{
					if (nCntTexture > 0)
					{// 2回目以降のTEXTURE_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "TEXTURE_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aTextureName[nCntTexture][0]);		// テクスチャのパス

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				continue;
			}

			if (strcmp(&aString[0], "NUM_MODEL") == 0)
			{// モデル数の読み込み
				fscanf(pFile, " = %d", &nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
				{
					if (nCntModel > 0)
					{// 2回目以降のMODEL_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "MODEL_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aModelName[nCntModel][0]);		// モデルのパス

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

				continue;
			}

			if (strcmp(&aString[0], "FIELDSET") == 0)
			{// 地面の設置
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "TEXTYPE") == 0)
				{// テクスチャやモデルの種類
					fscanf(pFile, " = %d", &nIdx);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "POS") == 0)
				{// 位置
					fscanf(pFile, " = %f %f %f", &pos.x, &pos.y, &pos.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "ROT") == 0)
				{// 向き
					fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOCK") == 0)
				{// 分割数
					fscanf(pFile, " = %d %d", &nBlockX, &nBlockZ);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SIZE") == 0)
				{// 大きさ
					fscanf(pFile, " = %f %f", &fWidth, &fDepth);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				SetMeshField((MESHFIELDTYPE)nIdx, pos, D3DXVECTOR3(0.0f, D3DX_PI * rot.y / 180.0f, 0.0f), D3DXVECTOR2((float)nBlockX, (float)nBlockZ), D3DXVECTOR2(fWidth / (float)nBlockX, fDepth / (float)nBlockZ));

				continue;
			}

			if (strcmp(&aString[0], "WALLSET") == 0)
			{// 壁の設置
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "TEXTYPE") == 0)
				{// テクスチャやモデルの種類
					fscanf(pFile, " = %d", &nIdx);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "POS") == 0)
				{// 位置
					fscanf(pFile, " = %f %f %f", &pos.x, &pos.y, &pos.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "ROT") == 0)
				{// 向き
					fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOCK") == 0)
				{// 分割数
					fscanf(pFile, " = %d %d", &nBlockX, &nBlockZ);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SIZE") == 0)
				{// 大きさ
					fscanf(pFile, " = %f %f", &fWidth, &fDepth);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				SetMeshWall(pos, D3DXVECTOR3(0.0f, D3DX_PI * rot.y / 180.0f, 0.0f), D3DXVECTOR2((float)nBlockX, (float)nBlockZ), D3DXVECTOR2(fWidth / (float)nBlockX, fDepth / (float)nBlockZ));

				continue;
			}

			if (strcmp(&aString[0], "TREESET") == 0)
			{// 木の設置
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "TEXTYPE") == 0)
				{// テクスチャやモデルの種類
					fscanf(pFile, " = %d", &nIdx);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "POS") == 0)
				{// 位置
					fscanf(pFile, " = %f %f %f", &pos.x, &pos.y, &pos.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "ROT") == 0)
				{// 向き
					fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOCK") == 0)
				{// 分割数
					fscanf(pFile, " = %d %d", &nBlockX, &nBlockZ);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SIZE") == 0)
				{// 大きさ
					fscanf(pFile, " = %f %f", &fWidth, &fDepth);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				SetTree(pos, (TREETYPE)(nIdx - 4));

				continue;
			}

			if (strcmp(&aString[0], "END_SCRIPT") == 0)
			{// ファイルの読み込みが完了
				break;
			}
		}

		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合
		printf("ファイルを開けませんでした");
	}
}

//=============================================================================
// 配置物の取得(メッシュシリンダーのインデックスより)
//=============================================================================
Object GetObjectFromMCylinder(int nIdxMeshCylinder)
{
	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == true)
		{// 使用されているとき
			if (g_aObject[nCntObject].nIdxMeshCylinder == nIdxMeshCylinder)
			{// 引数のインデックスと配置物のインデックスが一致
				return g_aObject[nCntObject];
			}
		}
	}

	return g_aObject[0];
}

//=============================================================================
// 配置物の取得
//=============================================================================
Object* GetObjectAll(void)
{
	return &g_aObject[0];
}

//=============================================================================
// 配置物モデルの取得
//=============================================================================
ObjectModel* GetObjectModel(void)
{
	return &g_aObjectModel[0];
}

//=============================================================================
// ゴール番号を取得
//=============================================================================
int GetGoalIdx(void)
{
	return g_nGoalIdx;
}

//=============================================================================
// プレゼントをもらった家の番号を取得
//=============================================================================
int* GetObjectPresent(int nType)
{
	g_nNumPresentHouse = 0;

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == true)
		{// 使用されているとき
			if (g_aObject[nCntObject].nNumPresent != 0)
			{// もらっている
				g_nPresentIdx[g_nNumPresentHouse] = g_aObject[nCntObject].nType;
				g_nNumPresent[g_nNumPresentHouse] = g_aObject[nCntObject].nNumPresent;
				g_posPresentHouse[g_nNumPresentHouse] = g_aObject[nCntObject].pos;

				if (g_aObject[nCntObject].nNumPresent == g_aObject[nCntObject].nDefPresent)
				{// ぴったり
					g_nPresentNumClose[g_nNumPresentHouse] = (int)PRESENTNUM_PERFECT;
				}
				else if (g_aObject[nCntObject].nNumPresent <= g_aObject[nCntObject].nDefPresent + CLOSE_NUMPRESENT &&
					g_aObject[nCntObject].nNumPresent >= g_aObject[nCntObject].nDefPresent - CLOSE_NUMPRESENT)
				{// ±1の範囲内
					g_nPresentNumClose[g_nNumPresentHouse] = (int)PRESENTNUM_CLOSE;
				}
				else if (g_aObject[nCntObject].nNumPresent <= g_aObject[nCntObject].nDefPresent + NEAR_NUMPRESENT &&
					g_aObject[nCntObject].nNumPresent >= g_aObject[nCntObject].nDefPresent - NEAR_NUMPRESENT)
				{// ±3の範囲内
					g_nPresentNumClose[g_nNumPresentHouse] = (int)PRESENTNUM_NEAR;
				}
				else
				{// 遠い
					g_nPresentNumClose[g_nNumPresentHouse] = (int)PRESENTNUM_NONE;
				}

				g_nNumPresentHouse++;
			}
		}
	}

	switch (nType)
	{
	case 0:			// プレゼントをもらった家の数
		return &g_nNumPresentHouse;

		break;

	case 1:			// プレゼントをもらった家のインデックス
		return &g_nPresentIdx[0];

		break;

	case 2:			// もらったプレゼントの数
		return &g_nNumPresent[0];

		break;

	case 3:			// もらったプレゼントの数がどれくらい規定の数に近いか
		return &g_nPresentNumClose[0];

		break;

	default:
		return &g_nNumPresentHouse;

		break;
	}
}

//=============================================================================
// プレゼントをもらった家の場所
//=============================================================================
D3DXVECTOR3* GetObjectPresentPos(void)
{
	int nPresentHouseCount = 0;

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++)
	{
		if (g_aObject[nCntObject].bUse == true)
		{// 使用されているとき
			if (g_aObject[nCntObject].nNumPresent != 0)
			{// もらっている
				g_posPresentHouse[nPresentHouseCount] = g_aObject[nCntObject].pos;

				nPresentHouseCount++;
			}
		}
	}

	return &g_posPresentHouse[0];
}