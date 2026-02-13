//=============================================================================
// 
// プレイヤー [player.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "player.h"
#include "computer.h"
#include "ui_esa.h"
#include "meshcylinder.h"
#include "meshring.h"
#include "meshorbit.h"
#include "oceancurrents.h"
#include "object.h"
#include "particle_3d.h"
#include "crosshair.h"
#include "watersurf.h"
#include "pot.h"
#include "camera.h"
#include "input.h"
#include "time.h"
#include "sound.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MOVEMENT				(D3DXVECTOR3(0.3f, 0.3f, 0.3f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define SEA_GRAVITY				(-0.02f)								// 重力
#define INERTIA_MOVE			(0.05f)									// 移動の慣性
#define DASH_MOVE				(0.04f)									// 高速移動の速さ
#define DASH_RATE				(0.15f)									// 高速移動の速さ
#define DASH_REACH				(10.0f)									// 高速移動のリーチ
#define MAX_MOVE				(10.0f)									// 移動の制限
#define INERTIA_ANGLE			(0.1f)									// 角度の慣性
#define POS_ERROR				(50.0f)									// 位置の誤差
#define MOVE_ERROR				(5.0f)									// 移動量の誤差
#define FOGS_MIN				(500.0f)								// 開始フォグの最低
#define FOGS_MAX				(1200.0f)								// 開始フォグの最高
#define FOGE_MIN				(1000.0f)								// 終了フォグの最低
#define FOGE_MAX				(3000.0f)								// 終了フォグの最高
#define TENTACLE_RANGE			(1000.0f)								// 触手の長さ(見た目)
#define TENTACLE_REACH			(1000.0f)								// 触手のリーチ(実際)
#define TENTACLE_CT				(ONE_SECOND * 1 + ONE_SECOND)			// 触手のクールダウン
#define INK_CT					(ONE_SECOND * 5 + ONE_SECOND)			// 墨吐きのクールダウン
#define RIPPLE_COUNT			(20)									// 水面に波紋が出る間隔
#define PLAYER_TENTACLE			(8)										// プレイヤーの足の数
#define PLAYER_RADIUS			(25.0f)									// 半径
#define PLAYER_HEIGHT			(100.0f)								// 高さ
#define TENTACLE_RADIUS			(100.0f)								// 触手の当たり判定
#define PLAYER_FILE				"data\\motion_octo_1.txt"				// プレイヤーのデータファイル

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadPlayer(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Player g_aPlayer[MAX_PLAYER];							// プレイヤーの情報
char* g_apFilenamePlayer[MAX_NUMMODEL] = {};			// モデルファイルへのポインタ

//=============================================================================
// プレイヤーの初期化処理
//=============================================================================
void InitPlayer(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATERIAL* pMat;
	Player* pPlayer = GetPlayer();

	// プレイヤーの情報の初期化
	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++, pPlayer++)
	{
		pPlayer->nIdx = nCntPlayer;
		pPlayer->pos = FIRST_POS;
		pPlayer->posOld = FIRST_POS;
		pPlayer->move = FIRST_POS;
		pPlayer->rot = FIRST_POS;
		pPlayer->state = PLAYERSTATE_APPEAR;
		pPlayer->TentacleState = PLTENTACLESTATE_NORMAL;
		pPlayer->nCounterState = 0;
		pPlayer->fAngleX = 0.0f;
		pPlayer->fAngleY = 0.0f;
		pPlayer->fFogStart = (pPlayer->pos.y * 0.4f) + FOGS_MIN;
		pPlayer->fFogEnd = (pPlayer->pos.y * 1.1f) + FOGE_MIN;
		pPlayer->fRadius = PLAYER_RADIUS;
		pPlayer->fHeight = PLAYER_HEIGHT;
		pPlayer->bJump = false;
		pPlayer->bLand = true;
		pPlayer->bMove = false;
		pPlayer->bAct = false;
		pPlayer->bUse = false;
		pPlayer->bBlind = false;
		pPlayer->nBlindCounter = 0;
		pPlayer->nFood = 0;
		pPlayer->esaQueue.nTail = -1;
		memset(&pPlayer->esaQueue.nData, -1, sizeof(int[MAX_QUEUE]));
		pPlayer->Potstate = POTSTATE_NONE;
		pPlayer->nPotIdx = -1;
		pPlayer->nMaxFood = 0;
		pPlayer->nTentacleCooldown = 0;
		pPlayer->nInkCooldown = 0;
	}

	// ファイルからキャラクター情報をロード
	LoadPlayer();

	pPlayer = GetPlayer();

	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++, pPlayer++)
	{
		pPlayer->motionType = MOTIONTYPE_NEUTRAL;
		pPlayer->bLoopMotion = pPlayer->aMotionInfo[0].bLoop;
		pPlayer->nNumKey = pPlayer->aMotionInfo[0].nNumKey;
		pPlayer->nKey = 0;
		pPlayer->nCounterMotion = 0;
		pPlayer->bFinishMotion = false;
		pPlayer->bBlendMotion = false;
		pPlayer->motionTypeBlend = MOTIONTYPE_NEUTRAL;
		pPlayer->bLoopMotionBlend = pPlayer->aMotionInfo[0].bLoop;
		pPlayer->nNumKeyBlend = pPlayer->aMotionInfo[0].nNumKey;
		pPlayer->nKeyBlend = 0;
		pPlayer->nCounterMotionBlend = 0;
		pPlayer->nFrameBlend = 0;
		pPlayer->nCounterBlend = 0;

		for (int nCntModel = 0; nCntModel < pPlayer->nNumModel; nCntModel++)
		{
			// Xファイルの読み込み
			if (FAILED(D3DXLoadMeshFromX(g_apFilenamePlayer[pPlayer->aModel[nCntModel].nIdx],
				D3DXMESH_SYSTEMMEM,
				pDevice,
				NULL,
				&pPlayer->aModel[nCntModel].pBuffMat,
				NULL,
				&pPlayer->aModel[nCntModel].dwNumMat,
				&pPlayer->aModel[nCntModel].pMesh)))
			{
				return;
			}

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)pPlayer->aModel[nCntModel].pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)pPlayer->aModel[nCntModel].dwNumMat; nCntMat++)
			{
				if (pMat[nCntMat].pTextureFilename != NULL)
				{// テクスチャファイルが存在する
					D3DXCreateTextureFromFile(pDevice, pMat[nCntMat].pTextureFilename, &pPlayer->aModel[nCntModel].apTexture[nCntMat]);
				}
			}
		}
	}

	SetRandomPlayer(GetNumCamera());
}

//=============================================================================
// プレイヤーの終了処理
//=============================================================================
void UninitPlayer(void)
{
	Player* pPlayer = GetPlayer();

	for (int nCntModel = 0; nCntModel < pPlayer->nNumModel; nCntModel++)
	{
		// メッシュの破棄
		if (pPlayer->aModel[nCntModel].pMesh != NULL)
		{
			pPlayer->aModel[nCntModel].pMesh->Release();
			pPlayer->aModel[nCntModel].pMesh = NULL;
		}

		// マテリアルの破棄
		if (pPlayer->aModel[nCntModel].pBuffMat != NULL)
		{
			pPlayer->aModel[nCntModel].pBuffMat->Release();
			pPlayer->aModel[nCntModel].pBuffMat = NULL;
		}

		// テクスチャの破棄
		for (int nCntPlayer = 0; nCntPlayer < (int)pPlayer->aModel[nCntModel].dwNumMat; nCntPlayer++)
		{
			if (pPlayer->aModel[nCntModel].apTexture[nCntPlayer] != NULL)
			{
				pPlayer->aModel[nCntModel].apTexture[nCntPlayer]->Release();
				pPlayer->aModel[nCntModel].apTexture[nCntPlayer] = NULL;
			}
		}
	}
}

//=============================================================================
// プレイヤーの更新処理
//=============================================================================
void UpdatePlayer(void)
{
	Camera* pCamera = GetCamera();
	Player* pPlayer = GetPlayer();

	for (int nCntPlayer = 0; nCntPlayer < GetNumCamera(); nCntPlayer++, pPlayer++, pCamera++)
	{
		if (pPlayer->bUse == true)
		{// 使用している
			static int nCounter = 0;		// 色々なものに使えるカウンター
			int nValueH, nValueV;
			int nValue;
			float fmoveAngle = 0.0f;
			float fAngle;

			pPlayer->posOld = pPlayer->pos;
			pPlayer->posX = pPlayer->pos + (pCamera->posR - pCamera->posV);

			switch (pPlayer->state)
			{
			case PLAYERSTATE_NORMAL:			// 通常状態
				PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_NORMAL ]\n");

				break;

			case PLAYERSTATE_MOVE:				// 移動状態
				PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_MOVE ]\n");

				break;

			case PLAYERSTATE_APPEAR:			// 出現状態
				PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_APPEAR ]\n");

				break;

			case PLAYERSTATE_WAIT:				// 待機状態
				PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_WAIT ]\n");

				break;

			case PLAYERSTATE_DASH:				// 高速移動状態
				PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_DASH ]\n");

				break;

			case PLAYERSTATE_INK:				// 墨吐き状態
				PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_INK ]\n");

				break;

			case PLAYERSTATE_BACKAREA:			// エリア戻り状態
				PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_BACKAREA ]\n");

				D3DXVECTOR3 correct = -pPlayer->pos;
				pPlayer->move += *D3DXVec3Normalize(&pPlayer->move, &correct);

				pPlayer->nCounterState--;
				
				if (pPlayer->nCounterState < 0)
				{// 戻り状態を終わる
					pPlayer->state = PLAYERSTATE_NORMAL;
					pPlayer->nCounterState = 0;
				}

				break;
			}

			PrintDebugProc("エサの数 %d / %d\n", pPlayer->nFood, pPlayer->nMaxFood * PLAYER_TENTACLE);

			if (pPlayer->state != PLAYERSTATE_APPEAR && pPlayer->state != PLAYERSTATE_DASH && pPlayer->state != PLAYERSTATE_BACKAREA)
			{// 出現状態のときは移動できない
				// パッド移動
				if (GetJoypadStick(nCntPlayer, JOYKEY_LEFTSTICK, &nValueH, &nValueV) == true)
				{// パッドの移動優先
					if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
					{// 触手を伸ばしているときは向きを変えない
						pPlayer->fAngleY = pCamera->rot.y + atan2f(-(float)nValueH, -D3DX_PI - (float)nValueV);
					}

					fAngle = atan2f((float)(nValueH), (float)(nValueV));

					pPlayer->move.x += sinf(fAngle + pCamera->rot.y) * MOVEMENT.x * sinf((D3DX_PI * 0.5f) + pCamera->fAngle);
					pPlayer->move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * (nValueV / 30300) * MOVEMENT.y;
					pPlayer->move.z += cosf(fAngle + pCamera->rot.y) * MOVEMENT.z * sinf((D3DX_PI * 0.5f) + pCamera->fAngle);

					pPlayer->bMove = true;
				}
				else if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nCntPlayer, JOYKEY_UP) == true)		// キーボード移動
				{// 奥に移動
					if (GetKeyboardPress(DIK_A) == true || GetJoypadPress(nCntPlayer, JOYKEY_LEFT) == true)
					{// 左奥に移動
						pPlayer[nCntPlayer].move.x += sinf(-D3DX_PI * 0.75f - pCamera->rot.y) * MOVEMENT.x;
						pPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * MOVEMENT.y;
						pPlayer[nCntPlayer].move.z += cosf(-D3DX_PI * 0.25f + pCamera->rot.y) * MOVEMENT.z;

						if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							pPlayer[nCntPlayer].fAngleY = pCamera->rot.y - (-D3DX_PI * 0.75f);
						}
					}
					else if (GetKeyboardPress(DIK_D) == true || GetJoypadPress(nCntPlayer, JOYKEY_RIGHT) == true)
					{// 右奥に移動
						pPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.75f - pCamera->rot.y) * MOVEMENT.x;
						pPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * MOVEMENT.y;
						pPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.25f + pCamera->rot.y) * MOVEMENT.z;

						if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							pPlayer[nCntPlayer].fAngleY = pCamera->rot.y - (D3DX_PI * 0.75f);
						}
					}
					else if (GetKeyboardPress(DIK_W) == true || GetJoypadPress(nCntPlayer, JOYKEY_UP) == true)
					{// 奥に移動
						pPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.0f + pCamera->rot.y) * MOVEMENT.x;
						pPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * MOVEMENT.y;
						pPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.0f + pCamera->rot.y) * MOVEMENT.z;

						if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							pPlayer[nCntPlayer].fAngleY = pCamera->rot.y - D3DX_PI;
						}
					}

					pPlayer->bMove = true;
				}
				else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nCntPlayer, JOYKEY_DOWN) == true)
				{// 手前に移動
					if (GetKeyboardPress(DIK_A) == true || GetJoypadPress(nCntPlayer, JOYKEY_LEFT) == true)
					{// 左手前に移動
						pPlayer[nCntPlayer].move.x += sinf(-D3DX_PI * 0.25f - pCamera->rot.y) * MOVEMENT.x;
						pPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * -MOVEMENT.y;
						pPlayer[nCntPlayer].move.z += cosf(-D3DX_PI * 0.75f + pCamera->rot.y) * MOVEMENT.z;

						if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							pPlayer[nCntPlayer].fAngleY = pCamera->rot.y + (D3DX_PI * 0.25f);
						}
					}
					else if (GetKeyboardPress(DIK_D) == true || GetJoypadPress(nCntPlayer, JOYKEY_RIGHT) == true)
					{// 右手前に移動
						pPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.25f - pCamera->rot.y) * MOVEMENT.x;
						pPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * -MOVEMENT.y;
						pPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.75f + pCamera->rot.y) * MOVEMENT.z;

						if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							pPlayer[nCntPlayer].fAngleY = pCamera->rot.y + (-D3DX_PI * 0.25f);
						}
					}
					else if (GetKeyboardPress(DIK_S) == true || GetJoypadPress(nCntPlayer, JOYKEY_DOWN) == true)
					{// 手前に移動
						pPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 1.0f + pCamera->rot.y) * MOVEMENT.x;
						pPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * -MOVEMENT.y;
						pPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 1.0f + pCamera->rot.y) * MOVEMENT.z;

						if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							pPlayer[nCntPlayer].fAngleY = pCamera->rot.y;
						}
					}

					pPlayer->bMove = true;
				}
				else if (GetKeyboardPress(DIK_A) == true || GetJoypadPress(nCntPlayer, JOYKEY_LEFT) == true)
				{// 左に移動
					pPlayer[nCntPlayer].move.x += sinf(-D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.x;
					pPlayer[nCntPlayer].move.z += cosf(-D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.z;

					if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
					{// 触手を伸ばしているときは向きを変えない
						pPlayer[nCntPlayer].fAngleY = pCamera->rot.y + (D3DX_PI * 0.5f);
					}

					pPlayer->bMove = true;
				}
				else if (GetKeyboardPress(DIK_D) == true || GetJoypadPress(nCntPlayer, JOYKEY_RIGHT) == true)
				{// 右に移動
					pPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.x;
					pPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.z;

					if (pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG)
					{// 触手を伸ばしているときは向きを変えない
						pPlayer[nCntPlayer].fAngleY = pCamera->rot.y - (D3DX_PI * 0.5f);
					}

					pPlayer->bMove = true;
				}
				else
				{// プレイヤーの入力がない
					pPlayer->bMove = false;
				}
			}

			switch (pPlayer->TentacleState)
			{
			case PLTENTACLESTATE_NORMAL:			// 通常状態
				pPlayer->aModel[2].scale.y = 1.0f;
				pPlayer->fAngleX = 0.0f;
				SetCameraViewAngle(nCntPlayer, 45.0f);

				break;

			case PLTENTACLESTATE_TENTACLELONG:		// 触手伸ばし状態
				if (pPlayer->motionType == MOTIONTYPE_INK || pPlayer->motionTypeBlend == MOTIONTYPE_INK)
				{// モーションキャンセル
					pPlayer->TentacleState = PLTENTACLESTATE_NORMAL;
				}
				else
				{// キャンセルしない
					if (pPlayer->bFinishMotion == true)
					{// 触手が伸ばし終わったら
						CrossHair* pCrossHair = GetCrossHair();
						pCrossHair = &pCrossHair[nCntPlayer];

						D3DXVECTOR3 tentaclePos = D3DXVECTOR3(pPlayer->aModel[4].mtxWorld._41, pPlayer->aModel[4].mtxWorld._42, pPlayer->aModel[4].mtxWorld._43);
						int nIdx = -1;

						if (CollisionEsa(&nIdx, false, &tentaclePos, TENTACLE_RADIUS) == true &&
							pPlayer->nFood < pPlayer->nMaxFood * PLAYER_TENTACLE &&
							pPlayer->motionType != MOTIONTYPE_OCEANCULLENT)
						{// エサと接触した
							Esa* pEsa = GetEsa();

							if (pEsa[nIdx].esaType != ESA_ACTTYPE_GOTO_POT)
							{// タコつぼに入れてる最中じゃない
								pEsa[nIdx].bUse = false;
								SetAddUiEsa(nCntPlayer, pEsa[nIdx].nIdxModel);

								pPlayer->nFood++;
								Enqueue(&pPlayer->esaQueue, pEsa[nIdx].nIdxModel);
							}
						}
						else if (CollisionPotArea(tentaclePos, TENTACLE_RADIUS * 0.5f, pPlayer, NULL, true) == true ||
							CollisionOcto(nCntPlayer, false, tentaclePos) == true)
						{// タコつぼからエサをとる
							pPlayer->TentacleState = PLTENTACLESTATE_TENTACLESHORT;

							SetMotionPlayer(nCntPlayer, MOTIONTYPE_TENTACLESHORT, true, 20);
						}
						else if (pCrossHair->state == CROSSHAIRSTATE_REACH &&
							(CollisionMeshCylinder(&tentaclePos, &pPlayer->pos, &pPlayer->move,
								TENTACLE_RADIUS, TENTACLE_RADIUS, true) == true ||
								tentaclePos.y < 0.0f) ||
							CollisionObject(&tentaclePos, &pPlayer->pos, &pPlayer->move,
								TENTACLE_RADIUS, TENTACLE_RADIUS, true) == true)
						{// 壁との当たり判定
							pPlayer->state = PLAYERSTATE_DASH;
							pPlayer->TentacleState = PLTENTACLESTATE_TENTACLESHORT;
							SetMotionPlayer(nCntPlayer, MOTIONTYPE_DASH, true, 20);

							SetCameraViewAngle(nCntPlayer, 15.0f);
						}
						else
						{// 触手を伸ばす
							if (pPlayer->aModel[2].scale.y < TENTACLE_RANGE * 0.1f)
							{// リーチより短い
								pPlayer->aModel[2].scale.y += 5.0f;
							}
							else
							{// リーチの長さになった
								pPlayer->TentacleState = PLTENTACLESTATE_TENTACLESHORT;

								SetMotionPlayer(nCntPlayer, MOTIONTYPE_TENTACLESHORT, true, 20);
							}
						}
					}
				}

				break;

			case PLTENTACLESTATE_TENTACLESHORT:		// 触手縮め状態
				if (pPlayer->aModel[2].scale.y > 1.0f)
				{// 触手を短くする
					pPlayer->aModel[2].scale.y += (1.0f - pPlayer->aModel[2].scale.y) * 0.5f;
				}
				else
				{// 元の長さに戻す
					pPlayer->aModel[2].scale.y = 1.0f;
					pPlayer->TentacleState = PLTENTACLESTATE_NORMAL;

					if (pPlayer->motionType != MOTIONTYPE_DASH)
					{// 高速移動していないとき
						pPlayer->fAngleX = 0.0f;
					}
				}

				break;
			}

			if (pPlayer->state == PLAYERSTATE_DASH)
			{// 高速移動
				pPlayer->pos += pPlayer->vecX;
				pPlayer->vecX.x += (0.0f - pPlayer->vecX.x) * DASH_MOVE;
				pPlayer->vecX.y += (0.0f - pPlayer->vecX.y) * DASH_MOVE;
				pPlayer->vecX.z += (0.0f - pPlayer->vecX.z) * DASH_MOVE;

				if (pPlayer->vecX.x < MOVE_ERROR && pPlayer->vecX.x > -MOVE_ERROR &&
					pPlayer->vecX.y < MOVE_ERROR && pPlayer->vecX.y > -MOVE_ERROR &&
					pPlayer->vecX.z < MOVE_ERROR && pPlayer->vecX.z > -MOVE_ERROR)
				{// 止まった
					pPlayer->vecX = FIRST_POS;

					SetCameraViewAngle(nCntPlayer, 45.0f);

					if (pPlayer->motionType == MOTIONTYPE_DASH)
					{// 高速移動モーションしていたら
						if (pPlayer->bMove == true)
						{// 移動してる
							pPlayer->state = PLAYERSTATE_MOVE;
							pPlayer->fAngleX = 0.0f;
							SetMotionPlayer(nCntPlayer, MOTIONTYPE_MOVE, true, 20);
						}
						else
						{// 移動してない
							pPlayer->state = PLAYERSTATE_WAIT;
							pPlayer->fAngleX = 0.0f;
							SetMotionPlayer(nCntPlayer, MOTIONTYPE_NEUTRAL, true, 20);
						}
					}
				}
			}

			if (GetOceanCurrents() != OCEANCURRENTSSTATE_WIRLPOOL ||
				(GetOceanCurrents() == OCEANCURRENTSSTATE_WIRLPOOL && CollisionObjectArea(pPlayer->pos) == true))
			{
				if (pPlayer->state != PLAYERSTATE_DASH && pPlayer->state != PLAYERSTATE_INK && pPlayer->state != PLAYERSTATE_BACKAREA
					&& pPlayer->bMove == true && pPlayer->TentacleState == PLTENTACLESTATE_NORMAL)
				{// 普通に移動しているとき
					pPlayer->state = PLAYERSTATE_MOVE;
					SetMotionPlayer(nCntPlayer, MOTIONTYPE_MOVE, true, 20);
				}
				else if (pPlayer->bMove == false && pPlayer->state != PLAYERSTATE_DASH &&
					pPlayer->state != PLAYERSTATE_INK && pPlayer->TentacleState == PLTENTACLESTATE_NORMAL)
				{// 待機モーション
					SetMotionPlayer(nCntPlayer, MOTIONTYPE_NEUTRAL, true, 20);
				}
			}

			PrintDebugProc("プレイヤーのpos ( %f %f %f )\n", pPlayer->pos.x, pPlayer->pos.y, pPlayer->pos.z);

#ifdef _DEBUG
			if (GetKeyboardTrigger(DIK_BACKSPACE) == true || GetJoypadTrigger(nCntPlayer, JOYKEY_LEFT_THUMB) == true)
			{// 位置回転リセット
				pPlayer->pos = START_POS;
				pPlayer->posOld = START_POS;
				pPlayer->move = FIRST_POS;
				pPlayer->rot = FIRST_POS;
				pPlayer->fAngleY = 0.0f;
				pPlayer->state = PLAYERSTATE_WAIT;
			}
#endif
			// クールダウン
			if (pPlayer->nTentacleCooldown > 0)
			{// 触手クールダウン
				pPlayer->nTentacleCooldown--;
			}

			if (pPlayer->nInkCooldown > 0)
			{// 墨吐きクールダウン
				pPlayer->nInkCooldown--;
			}

			if (pPlayer->nBlindCounter > 0)
			{// 視界悪化カウント
				pPlayer->nBlindCounter--;
			}
			else if (pPlayer->nBlindCounter == 0)
			{// 視界悪化が終わる
				pPlayer->bBlind = false;
				pPlayer->nBlindCounter = 0;
			}

			//PrintDebugProc("プレイヤーのmove ( %f %f %f )\n", pPlayer->move.x, pPlayer->move.y, pPlayer->move.z);

			// 重力
			pPlayer->move.y += SEA_GRAVITY;

			if (CollisionObjectArea(pPlayer->pos) == false)
			{// 安地外のときに渦潮
				MoveOceanCurrents(&pPlayer->pos);

				if (GetOceanCurrents() == OCEANCURRENTSSTATE_WIRLPOOL)
				{// 安地外で渦潮
					if (pPlayer->TentacleState == PLTENTACLESTATE_NORMAL && pPlayer->state != PLAYERSTATE_DASH && 
						pPlayer->state != PLAYERSTATE_INK && pPlayer->state != PLAYERSTATE_BACKAREA)
					{// 触手が通常状態のときだけ
						SetMotionPlayer(nCntPlayer, MOTIONTYPE_OCEANCULLENT, true, 20);
					}

					if (pPlayer->nFood > 0 && nCounter % 15 == 0)
					{// エサを持っている
						pPlayer->nFood--;
						int nIdx = Dequeue(&pPlayer->esaQueue);
						SetSubUiEsa(nCntPlayer);

						SetEsa(nIdx, ESA_ACTTYPE_SWIM, 0, pPlayer->pos, FIRST_POS);
					}
				}
			}

			if (GetOceanCurrents() == OCEANCURRENTSSTATE_WIRLPOOL &&
				CollisionObjectArea(pPlayer->pos) == true)
			{// 渦潮中に安地にいたら
				if (pPlayer->bMove == true)
				{// 移動してる
					pPlayer->state = PLAYERSTATE_MOVE;
					SetMotionPlayer(nCntPlayer, MOTIONTYPE_MOVE, true, 20);
				}
				else
				{// 移動してない
					pPlayer->state = PLAYERSTATE_WAIT;
					SetMotionPlayer(nCntPlayer, MOTIONTYPE_NEUTRAL, true, 20);
				}
			}

			if (pPlayer->state != PLAYERSTATE_APPEAR && pPlayer->state != PLAYERSTATE_DASH)
			{// 出現状態以外
				// 慣性
				pPlayer->pos += pPlayer->move;
				pPlayer->move.x += (0.0f - pPlayer->move.x) * INERTIA_MOVE;
				pPlayer->move.y += (0.0f - pPlayer->move.y) * INERTIA_MOVE;
				pPlayer->move.z += (0.0f - pPlayer->move.z) * INERTIA_MOVE;
			}

			// 軌跡
			for (int nCntTent = 0; nCntTent < PLAYER_TENTACLE; nCntTent++)
			{
				SetMeshOrbit(D3DXVECTOR3(pPlayer->aModel[(nCntTent + 1) * 4].posOff.x, pPlayer->aModel[(nCntTent + 1) * 4].posOff.y, pPlayer->aModel[(nCntTent + 1) * 4].posOff.z),
					D3DXVECTOR3(pPlayer->aModel[(nCntTent + 1) * 4].posOff.x, pPlayer->aModel[(nCntTent + 1) * 4].posOff.y + 5.5f, pPlayer->aModel[(nCntTent + 1) * 4].posOff.z),
					WHITE_VTX, CYAN_VTX, &pPlayer->aModel[(nCntTent + 1) * 4].mtxWorld);
			}

			D3DXVECTOR2 XZdist = D3DXVECTOR2(pPlayer->pos.x, pPlayer->pos.z);
			float fDist = D3DXVec2Length(&XZdist);

			if (fDist > OUTCYLINDER_RADIUS + 30.0f)
			{// 移動制限
				pPlayer->fAngleY = atan2f(pPlayer->pos.x, pPlayer->pos.z);
				pPlayer->state = PLAYERSTATE_BACKAREA;
				pPlayer->nCounterState = ONE_SECOND;
			}
			else if(fDist < INCYLINDER_RADIUS + 0.1f)
			{// 内側の岩
				D3DXVECTOR3 correct = -pPlayer->pos;
				pPlayer->move += *D3DXVec3Normalize(&pPlayer->move, &correct);
			}

			if (pPlayer->pos.y < 0.0f)
			{// 底
				pPlayer->pos.y = 0.0f;
			}

			if (pPlayer->pos.y > *GetWaterSurf_Height() - (PLAYER_HEIGHT * 0.5f))
			{// 上									  
				pPlayer->pos.y = *GetWaterSurf_Height() - (PLAYER_HEIGHT * 0.5f);

				if (nCounter % RIPPLE_COUNT == 0)
				{// 定期的に波紋
					SetMeshRing(D3DXVECTOR3(pPlayer->pos.x + (rand() % 6 - 3), *GetWaterSurf_Height(), pPlayer->pos.z + (rand() % 6 - 3)), FIRST_POS,
						D3DXVECTOR2(24.0f, 1.0f), D3DXVECTOR2(10.0f, 7.0f), D3DXCOLOR(WHITE_VTX.r, WHITE_VTX.g, WHITE_VTX.b, 0.5f));
				}
			}

			//PrintDebugProc("fAngle : %f", pCamera->fAngle);

			pPlayer->fFogStart = (pPlayer->pos.y * 0.4f + (-pCamera->fAngle * 0.2f)) + FOGS_MIN;

			if (pPlayer->fFogStart < FOGS_MIN)
			{// フォグの最低値
				pPlayer->fFogStart = FOGS_MIN;
			}
			else if (pPlayer->fFogStart > FOGS_MAX)
			{// フォグの最高値
				pPlayer->fFogStart = FOGS_MAX;
			}

			pPlayer->fFogEnd = (pPlayer->pos.y * 1.1f + (-pCamera->fAngle * 0.2f)) + FOGE_MIN;

			if (pPlayer->fFogEnd < FOGE_MIN)
			{// フォグの最低値
				pPlayer->fFogEnd = FOGE_MIN;
			}
			else if (pPlayer->fFogEnd > FOGE_MAX)
			{// フォグの最高値
				pPlayer->fFogEnd = FOGE_MAX;
			}

			if (pPlayer->bBlind == true)
			{// 視界悪化中
				pPlayer->fFogStart *= 0.5f;
				pPlayer->fFogEnd *= 0.5f;

				D3DXVECTOR3 headPos = D3DXVECTOR3(
					pPlayer->aModel[0].mtxWorld._41, 
					pPlayer->aModel[0].mtxWorld._42 + 10.0f, 
					pPlayer->aModel[0].mtxWorld._43);

				SetEffect3D(5, headPos, FIRST_POS, 0.0f, 15.0f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), EFFECTTYPE_OCTOINK);
			}

			fmoveAngle = pPlayer->fAngleY - pPlayer->rot.y;

			// 向きを調整
			CorrectAngle(&pPlayer->fAngleY, fmoveAngle);

			if (pPlayer->rot.y != pPlayer->fAngleY)
			{// 目標地点につくまで慣性で角度を足す
				pPlayer->rot.y += (pPlayer->fAngleY - pPlayer->rot.y) * INERTIA_ANGLE;

				// 向きを調整
				CorrectAngle(&pPlayer->rot.y, pPlayer->rot.y);
			}

			// 向きを調整
			CorrectAngle(&pPlayer->fAngleX, pPlayer->fAngleX - pPlayer->rot.x);

			if (pPlayer->rot.x != pPlayer->fAngleX)
			{// 目標地点につくまで慣性で角度を足す
				pPlayer->rot.x += (pPlayer->fAngleX - pPlayer->rot.x) * INERTIA_ANGLE;

				// 向きを調整
				CorrectAngle(&pPlayer->rot.x, pPlayer->rot.x);
			}

			if (nCounter % (ONE_SECOND * 30) == 0 && GetTime() != ONE_GAME)
			{// 持てるエサの最大値が増える
				pPlayer->nMaxFood++;
			}

			D3DXVECTOR3 dist;
			dist = pPlayer->posX - pPlayer->pos;
			D3DXVec3Normalize(&dist, &dist);
			dist *= TENTACLE_REACH;
			dist += pPlayer->pos;

			if (CollisionMeshCylinder(&dist, &pPlayer->pos, &pPlayer->move,
				0.0f, 0.0f, true) == true ||
				dist.y < 0.0f ||
				CollisionObject(&dist, &pPlayer->pos, &pPlayer->move, 0.0f, 0.0f, true) == true)
			{// 壁に当たった・オブジェクトに当たった・エサに当たった
				// クロスヘアの設定
				SetCrossHair(nCntPlayer, CROSSHAIRSTATE_REACH);
			}
			else
			{// 何にも当たっていない
				SetCrossHair(nCntPlayer, CROSSHAIRSTATE_NONE);
			}

			if (GetJoypadShoulder(nCntPlayer, JOYKEY_RIGHTTRIGGER, &nValue) == true
				&& pPlayer->TentacleState != PLTENTACLESTATE_TENTACLELONG && pPlayer->state != PLAYERSTATE_DASH &&
				pPlayer->nTentacleCooldown == 0)
			{// 触手伸ばしアクション
				pPlayer->TentacleState = PLTENTACLESTATE_TENTACLELONG;

				pPlayer->fAngleY = D3DX_PI + pCamera->rot.y;
				CorrectAngle(&pPlayer->fAngleY, pPlayer->fAngleY);

				D3DXVECTOR3 dir, rot;
				dir = pPlayer->posX - pPlayer->pos;
				D3DXVec3Normalize(&dir, &dir);

				//rot.y = D3DX_PI + atan2f(dir.x, dir.z);
				rot.x = (dir.y / 0.95f) * 1.2f;
				//rot.z = 0.0f;

				pPlayer->fAngleX = rot.x;
				//pPlayer->rot.y = rot.y;
				CorrectAngle(&pPlayer->fAngleX, pPlayer->fAngleX);
				//CorrectAngle(&pPlayer->rot.y, pPlayer->rot.y);

				pPlayer->vecX = (pCamera->posR - pCamera->posV) * DASH_RATE;

				SetMotionPlayer(nCntPlayer, MOTIONTYPE_TENTACLELONG, true, 20);

				// クールダウンを設定
				pPlayer->nTentacleCooldown = TENTACLE_CT;
			}

			if (GetJoypadShoulder(nCntPlayer, JOYKEY_LEFTTRIGGER, &nValue) == true
				&& pPlayer->state != PLAYERSTATE_INK && pPlayer->nInkCooldown == 0)
			{// 墨吐きアクション
				pPlayer->state = PLAYERSTATE_INK;

				SetMotionPlayer(nCntPlayer, MOTIONTYPE_INK, true, 20);

				SetParticle3D(14, 30, pPlayer->pos, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), D3DXVECTOR3(pPlayer->rot.x, pPlayer->rot.y - D3DX_PI, pPlayer->rot.z), 4.0f, 200, 8.0f, 0.06f, EFFECTTYPE_OCTOINK);
				SetParticle3D(14, 30, pPlayer->pos, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), D3DXVECTOR3(pPlayer->rot.x, pPlayer->rot.y - D3DX_PI, pPlayer->rot.z), 4.0f, 200, 8.0f, 0.06f, EFFECTTYPE_OCTOINK);
				SetParticle3D(14, 30, pPlayer->pos, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), D3DXVECTOR3(pPlayer->rot.x, pPlayer->rot.y - D3DX_PI, pPlayer->rot.z), 4.0f, 200, 8.0f, 0.06f, EFFECTTYPE_OCTOINK);

				CollisionInk(nCntPlayer, false, pPlayer->pos);

				// クールダウンを設定
				pPlayer->nInkCooldown = INK_CT;
			}

			if (pPlayer->state == PLAYERSTATE_INK && pPlayer->bFinishMotion == true)
			{// 墨吐きモーションが終わったら
				pPlayer->state = PLAYERSTATE_WAIT;
			}

			// 当たり判定
			CollisionPot(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move, pPlayer->fRadius, pPlayer->fHeight);
			CollisionObject(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move, pPlayer->fRadius, pPlayer->fHeight, false);
			CollisionMeshCylinder(&pPlayer->pos, &pPlayer->posOld, &pPlayer->move, pPlayer->fRadius, pPlayer->fHeight, false);

			int nIdx = -1;

			if (CollisionEsa(&nIdx, false, &pPlayer->pos, pPlayer->fRadius) == true &&
				pPlayer->nFood < pPlayer->nMaxFood * PLAYER_TENTACLE &&
				pPlayer->motionType != MOTIONTYPE_OCEANCULLENT)
			{// エサと接触した
				Esa* pEsa = GetEsa();

				if (pEsa[nIdx].esaType != ESA_ACTTYPE_GOTO_POT)
				{// タコつぼに入れてる最中じゃない
					pEsa[nIdx].bUse = false;
					SetAddUiEsa(nCntPlayer, pEsa[nIdx].nIdxModel);

					pPlayer->nFood++;
					Enqueue(&pPlayer->esaQueue, pEsa[nIdx].nIdxModel);
				}
			}

			if (pPlayer->nFood < 0)
			{// 最小値0
				pPlayer->nFood = 0;
			}

			CollisionPotArea(pPlayer->pos, pPlayer->fRadius, pPlayer, NULL, false);

			nCounter++;

			// モーションの更新処理
			UpdateMotionPlayer();
		}
	}
}

//=============================================================================
// プレイヤーの描画処理
//=============================================================================
void DrawPlayer(void)
{
	// ローカル変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans, mtxView;		// 計算用マトリックス
	D3DMATERIAL9 matDef;				// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;					// マテリアルデータへのポインタ
	Player* pPlayer = GetPlayer();
	Camera* pCamera = GetCamera();

	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++, pPlayer++, pCamera++)
	{
		if (pPlayer->bUse == true)
		{// 使用しているとき
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pPlayer->mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pPlayer->rot.y, pPlayer->rot.x, pPlayer->rot.z);
			D3DXMatrixMultiply(&pPlayer->mtxWorld, &pPlayer->mtxWorld, &mtxRot);

			//D3DXVECTOR3 rotX, rotY, rotZ;
			//rotZ = pPlayer->posX - pPlayer->pos;
			//D3DXVec3Normalize(&rotZ, &rotZ);

			//rotX = D3DXVECTOR3((rotZ.y * pCamera->vecU.z) - (rotZ.z * pCamera->vecU.y),
			//	(rotZ.z * pCamera->vecU.x) - (rotZ.x * pCamera->vecU.z),
			//	(rotZ.x * pCamera->vecU.y) - (rotZ.y * pCamera->vecU.x));
			//D3DXVec3Normalize(&rotX, &rotX);

			//rotY = D3DXVECTOR3((rotX.y * rotZ.z) - (rotX.z * rotZ.y),
			//	(rotX.z * rotZ.x) - (rotX.x * rotZ.z),
			//	(rotX.x * rotZ.y) - (rotX.y * rotZ.x));
			//D3DXVec3Normalize(&rotY, &rotY);

			//// ビューマトリックスを取得
			//pDevice->GetTransform(D3DTS_VIEW, &mtxView);
			//pPlayer->mtxWorld._11 = rotX.x;
			//pPlayer->mtxWorld._12 = rotY.x;
			//pPlayer->mtxWorld._13 = rotZ.x;
			//pPlayer->mtxWorld._14 = pPlayer->pos.x;
			//pPlayer->mtxWorld._21 = rotX.y;
			//pPlayer->mtxWorld._22 = rotY.y;
			//pPlayer->mtxWorld._23 = rotZ.y;
			//pPlayer->mtxWorld._24 = pPlayer->pos.y;
			//pPlayer->mtxWorld._31 = rotX.z;
			//pPlayer->mtxWorld._32 = rotY.z;
			//pPlayer->mtxWorld._33 = rotZ.z;
			//pPlayer->mtxWorld._34 = pPlayer->pos.z;
			//pPlayer->mtxWorld._41 = 0.0f;
			//pPlayer->mtxWorld._42 = 0.0f;
			//pPlayer->mtxWorld._43 = 0.0f;
			//pPlayer->mtxWorld._44 = 1.0f;

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pPlayer->pos.x, pPlayer->pos.y, pPlayer->pos.z);
			D3DXMatrixMultiply(&pPlayer->mtxWorld, &pPlayer->mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pPlayer->mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// 全モデル(パーツ)の描画
			for (int nCntModel = 0; nCntModel < pPlayer->nNumModel; nCntModel++)
			{
				D3DXMATRIX mtxRotModel, mtxTransModel, mtxScaleModel;		// 計算用マトリックス
				D3DXMATRIX mtxParent;						// 親のマトリックス

				// パーツのワールドマトリックスの初期化
				D3DXMatrixIdentity(&pPlayer->aModel[nCntModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotModel, pPlayer->aModel[nCntModel].rot.y, pPlayer->aModel[nCntModel].rot.x, pPlayer->aModel[nCntModel].rot.z);
				D3DXMatrixMultiply(&pPlayer->aModel[nCntModel].mtxWorld, &pPlayer->aModel[nCntModel].mtxWorld, &mtxRotModel);

				// 拡大率を反映
				D3DXMatrixScaling(&mtxScaleModel, pPlayer->aModel[nCntModel].scale.x, pPlayer->aModel[nCntModel].scale.y, pPlayer->aModel[nCntModel].scale.z);
				D3DXMatrixMultiply(&pPlayer->aModel[nCntModel].mtxWorld, &pPlayer->aModel[nCntModel].mtxWorld, &mtxScaleModel);

				// パーツの位置を反映
				D3DXMatrixTranslation(&mtxTransModel, pPlayer->aModel[nCntModel].pos.x, pPlayer->aModel[nCntModel].pos.y, pPlayer->aModel[nCntModel].pos.z);
				D3DXMatrixMultiply(&pPlayer->aModel[nCntModel].mtxWorld, &pPlayer->aModel[nCntModel].mtxWorld, &mtxTransModel);

				// パーツの「親のマトリックス」を設定
				if (pPlayer->aModel[nCntModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = pPlayer->aModel[pPlayer->aModel[nCntModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = pPlayer->mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
				D3DXMatrixMultiply(&pPlayer->aModel[nCntModel].mtxWorld,
					&pPlayer->aModel[nCntModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &pPlayer->aModel[nCntModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pPlayer->aModel[nCntModel].pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pPlayer->aModel[nCntModel].dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pPlayer->aModel[nCntModel].apTexture[nCntMat]);

					// モデルパーツの描画
					pPlayer->aModel[nCntModel].pMesh->DrawSubset(nCntMat);
				}
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
// プレイヤーの設定処理
//=============================================================================
void SetPlayer(int nIdx, D3DXVECTOR3 pos, D3DXVECTOR3 rot, MOTIONTYPE MotionType)
{
	Player* pPlayer = GetPlayer();

	pPlayer[nIdx].nIdx = nIdx;
	pPlayer[nIdx].pos = pos;
	pPlayer[nIdx].posOld = pos;
	pPlayer[nIdx].rot = rot;
	pPlayer[nIdx].state = PLAYERSTATE_NORMAL;
	pPlayer[nIdx].TentacleState = PLTENTACLESTATE_NORMAL;
	pPlayer[nIdx].nCounterState = 0;
	pPlayer[nIdx].fFogStart = (pPlayer[nIdx].pos.y * 0.4f) + FOGS_MIN;
	pPlayer[nIdx].fFogEnd = (pPlayer[nIdx].pos.y * 1.1f) + FOGE_MIN;
	pPlayer[nIdx].bJump = true;
	pPlayer[nIdx].bLand = false;
	pPlayer[nIdx].bAct = false;
	pPlayer[nIdx].bMove = false;
	pPlayer[nIdx].bUse = true;
	pPlayer[nIdx].bBlind = false;
	pPlayer[nIdx].nBlindCounter = 0;
	pPlayer[nIdx].nFood = 0;
	memset(&pPlayer[nIdx].esaQueue.nData, -1, sizeof(int[MAX_QUEUE]));
	pPlayer[nIdx].Potstate = POTSTATE_NONE;
	pPlayer[nIdx].nPotIdx = -1;
	pPlayer[nIdx].nMaxFood = 1;
	pPlayer[nIdx].nTentacleCooldown = 0;
	pPlayer[nIdx].nInkCooldown = 0;

	pPlayer[nIdx].motionType = MOTIONTYPE_NEUTRAL;
	pPlayer[nIdx].bLoopMotion = pPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].bLoop;
	pPlayer[nIdx].nNumKey = pPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].nNumKey;
	pPlayer[nIdx].nKey = 0;
	pPlayer[nIdx].nCounterMotion = 0;
	pPlayer[nIdx].bFinishMotion = false;
	pPlayer[nIdx].bBlendMotion = false;
	pPlayer[nIdx].motionTypeBlend = MOTIONTYPE_NEUTRAL;
	pPlayer[nIdx].bLoopMotionBlend = pPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].bLoop;
	pPlayer[nIdx].nNumKeyBlend = pPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].nNumKey;
	pPlayer[nIdx].nKeyBlend = 0;
	pPlayer[nIdx].nCounterMotionBlend = 0;
	pPlayer[nIdx].nFrameBlend = 0;
	pPlayer[nIdx].nCounterBlend = 0;

	SetMotionPlayer(nIdx, MotionType, false, 0);
}

//=============================================================================
// プレイヤーのランダム設定処理
//=============================================================================
void SetRandomPlayer(int nAmount)
{
	for (int nCntPlayer = 0; nCntPlayer < nAmount; nCntPlayer++)
	{
		D3DXVECTOR3 pos;
		float fAngle = (D3DX_PI * 2.0f) * ((float)((nCntPlayer + 1) * (360.0f / nAmount)) / 360.0f);
		float fsin = sinf(fAngle);

		pos.x = sinf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));
		pos.y = (float)(rand() % (int)(CYLINDER_HEIGHT * 0.6f)) + (CYLINDER_HEIGHT * 0.2f);
		pos.z = cosf(fAngle) * (INCYLINDER_RADIUS + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) + 1))));

		SetPlayer(nCntPlayer, pos, FIRST_POS, MOTIONTYPE_NEUTRAL);
	}
}

//=============================================================================
// プレイヤーの取得処理
//=============================================================================
Player* GetPlayer(void)
{
	return &g_aPlayer[0];
}

//=============================================================================
// プレイヤーのロード処理
//=============================================================================
void LoadPlayer(void)
{
	// ローカル変数宣言
	FILE* pFile;
	Player* pPlayer = GetPlayer();
	char aString[512] = {};				// ファイルのテキスト読み込み
	char aTrash[512] = {};				// ごみ箱
	char aModelName[128][512] = {};		// モデルの名前

	// テクスチャ読み込み用の変数
	int nNumTexture = 0;
	char aTextureName[64][256] = {};		// テクスチャの名前

	// キャラクターセット用の変数
	int nNumParts = 0;		// 読み込むパーツ数
	float fRadius = 0.0f;	// キャラクターの半径
	float fHeight = 0.0f;	// キャラクターの高さ
	float fMove = 0.0f;		// キャラクターの移動量
	float fJump = 0.0f;		// キャラクターのジャンプ量
	D3DXVECTOR2 Blowoff = D3DXVECTOR2(0.0f, 0.0f);		// 吹っ飛び量
	D3DXVECTOR2 Somersault = D3DXVECTOR2(0.0f, 0.0f);	// バク宙量

	// パーツセット用の変数
	int nIdx = -1;			// モデル番号
	int nIdxParent = -1;	// 親のモデル番号
	D3DXVECTOR3 pos = FIRST_POS;		// 位置
	D3DXVECTOR3 rot = FIRST_POS;		// 向き

	// モーションセット用の変数
	int nLoop = 0;			// ループするかどうか
	int nNumKey = 0;		// キーの総数
	int nFrame = 0;			// 再生フレーム数
	int nCntMotion = 0;		// モーション番号
	KEY key = {};			// キー要素

	pFile = fopen(PLAYER_FILE, "r");

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
				fscanf(pFile, " = %d", &pPlayer->nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < pPlayer->nNumModel; nCntModel++)
				{
					if (nCntModel > 0)
					{// 2回目以降のMODEL_FILENAMEを読み込んでおく
						if (strcmp(&aString[0], "MODEL_FILENAME") != 0)
						{// コメントの代わりに読み込んだ場合はなし
							fscanf(pFile, "%s", &aTrash[0]);
						}
					}

					fscanf(pFile, " = %s", &aModelName[nCntModel][0]);		// モデルのパス
					g_apFilenamePlayer[nCntModel] = &aModelName[nCntModel][0];

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);
					}
				}

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

			if (strcmp(&aString[0], "CHARACTERSET") == 0)
			{// キャラクターの設定
				fscanf(pFile, "%s", &aString[0]);

				if (strcmp(&aString[0], "NUM_PARTS") == 0)
				{// 読み込むパーツ数
					fscanf(pFile, " = %d", &nNumParts);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "RADIUS") == 0)
				{// キャラクターの半径
					fscanf(pFile, " = %f", &pPlayer->fRadius);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "HEIGHT") == 0)
				{// キャラクターの高さ
					fscanf(pFile, " = %f", &pPlayer->fHeight);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "MOVE") == 0)
				{// キャラクターの移動量
					fscanf(pFile, " = %f", &fMove);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "JUMP") == 0)
				{// キャラクターのジャンプ量
					fscanf(pFile, " = %f", &fJump);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "BLOWOFF") == 0)
				{// キャラクターの吹っ飛び量
					fscanf(pFile, " = %f %f", &Blowoff.x, &Blowoff.y);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "SOMERSAULT") == 0)
				{// キャラクターのバク宙量
					fscanf(pFile, " = %f %f", &Somersault.x, &Somersault.y);

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				// パーツ読み込み
				for (int nCntParts = 0; nCntParts < nNumParts; nCntParts++)
				{
					if (strcmp(&aString[0], "PARTSSET") == 0)
					{// PARTSSET
						fscanf(pFile, "%s", &aString[0]);
					}

					if (strcmp(&aString[0], "INDEX") == 0)
					{// モデル番号
						fscanf(pFile, " = %d", &nIdx);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "PARENT") == 0)
					{// 親のモデル番号
						fscanf(pFile, " = %d", &nIdxParent);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "POS") == 0)
					{// パーツの位置(オフセット)
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
					{// パーツの向き
						fscanf(pFile, " = %f %f %f", &rot.x, &rot.y, &rot.z);

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "END_PARTSSET") == 0)
					{// END_PARTSSET
						fscanf(pFile, "%s", &aString[0]);
					}

					// プレイヤーのモデル情報に設定
					pPlayer->aModel[nCntParts].nIdx = nIdx;
					pPlayer->aModel[nCntParts].nIdxModelParent = nIdxParent;
					pPlayer->aModel[nCntParts].pos = pos;
					pPlayer->aModel[nCntParts].rot = rot;
					pPlayer->aModel[nCntParts].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
					pPlayer->aModel[nCntParts].posOff = pos;
					pPlayer->aModel[nCntParts].rotOff = rot;
				}

				continue;
			}

			if (strcmp(&aString[0], "MOTIONSET") == 0)
			{// モーションの設定
				fscanf(pFile, "%s", &aString[0]);

				if (aString[0] == '#')
				{// コメント無視
					fgets(&aTrash[0], ONE_LINE, pFile);

					// 次の文字列を読み込む
					fscanf(pFile, "%s", &aString[0]);
				}

				if (strcmp(&aString[0], "LOOP") == 0)
				{// ループするかどうか
					fscanf(pFile, " = %d", &nLoop);

					// ループの真偽を代入
					if (nLoop == 0)
					{// false
						pPlayer->aMotionInfo[nCntMotion].bLoop = false;
					}
					else if (nLoop == 1)
					{// true
						pPlayer->aMotionInfo[nCntMotion].bLoop = true;
					}

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				if (strcmp(&aString[0], "NUM_KEY") == 0)
				{// キーの総数
					fscanf(pFile, " = %d", &nNumKey);

					// キーの総数を代入
					pPlayer->aMotionInfo[nCntMotion].nNumKey = nNumKey;

					fscanf(pFile, "%s", &aString[0]);

					if (aString[0] == '#')
					{// コメント無視
						fgets(&aTrash[0], ONE_LINE, pFile);

						// 次の文字列を読み込む
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				// キー読み込み
				for (int nCntKey = 0; nCntKey < nNumKey; nCntKey++)
				{
					if (strcmp(&aString[0], "KEYSET") == 0)
					{// KEYSET
						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					if (strcmp(&aString[0], "FRAME") == 0)
					{// 再生フレーム数
						fscanf(pFile, " = %d", &nFrame);

						pPlayer->aMotionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = nFrame;

						fscanf(pFile, "%s", &aString[0]);

						if (aString[0] == '#')
						{// コメント無視
							fgets(&aTrash[0], ONE_LINE, pFile);

							// 次の文字列を読み込む
							fscanf(pFile, "%s", &aString[0]);
						}
					}

					// パーツごとに読み込み
					for (int nCntPartsKey = 0; nCntPartsKey < nNumParts; nCntPartsKey++)
					{
						if (strcmp(&aString[0], "KEY") == 0)
						{// KEY
							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "POS") == 0)
						{// パーツの位置(オフセット)
							fscanf(pFile, " = %f %f %f", &key.fPosX, &key.fPosY, &key.fPosZ);

							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "ROT") == 0)
						{// パーツの向き
							fscanf(pFile, " = %f %f %f", &key.fRotX, &key.fRotY, &key.fRotZ);

							fscanf(pFile, "%s", &aString[0]);

							if (aString[0] == '#')
							{// コメント無視
								fgets(&aTrash[0], ONE_LINE, pFile);

								// 次の文字列を読み込む
								fscanf(pFile, "%s", &aString[0]);
							}
						}

						if (strcmp(&aString[0], "END_KEY") == 0)
						{// END_KEY
							fscanf(pFile, "%s", &aString[0]);
						}

						// モーション情報に代入
						pPlayer->aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPartsKey] = key;
					}

					if (strcmp(&aString[0], "END_KEYSET") == 0)
					{// END_KEYSET
						fscanf(pFile, "%s", &aString[0]);
					}
				}

				nCntMotion++;

				continue;
			}

			if (strcmp(&aString[0], "END_SCRIPT") == 0)
			{// ファイルの読み込みが完了
				break;
			}
		}

		// モーションの総数を代入
		pPlayer->nNumMotion = nCntMotion + 1;

		if (MAX_PLAYER > 1)
		{// 1人プレイじゃない場合
			pPlayer[1] = pPlayer[0];
		}

		fclose(pFile);
	}
	else
	{// ファイルが開けなかった場合
		printf("ファイルを開けませんでした");
	}
}

//=============================================================================
// プレイヤーモーションの更新処理
//=============================================================================
void UpdateMotionPlayer(void)
{
	int nNextKey;
	Player* pPlayer = GetPlayer();

	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++, pPlayer++)
	{
		// 全モデル(パーツ)の更新
		for (int nCntModel = 0; nCntModel < pPlayer->nNumModel; nCntModel++)
		{
			// ローカル変数宣言
			KEY* pKey;
			KEY* pKeyNext;
			Model* pModel;
			D3DXVECTOR3 posCurrent, rotCurrent;
			D3DXVECTOR3 posDest, rotDest;
			float fDiffKey, fRateKey;

			// 1フレームあたりに動く割合
			fRateKey = ((float)pPlayer->nCounterMotion / (float)pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey].nFrame);

			// 次のキー
			if (pPlayer->bLoopMotion == true)
			{// ループモーション
				nNextKey = (pPlayer->nKey + 1) % (pPlayer->aMotionInfo[pPlayer->motionType].nNumKey);
			}
			else
			{// ループしないモーション
				nNextKey = pPlayer->nKey + 1;

				if (nNextKey > pPlayer->aMotionInfo[pPlayer->motionType].nNumKey - 1)
				{// キーの総数を超えた
					nNextKey = pPlayer->aMotionInfo[pPlayer->motionType].nNumKey - 1;
				}
			}

			// 現在のキーのポインタ
			pKey = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey].aKey[nCntModel];

			// 次のキーのポインタ
			pKeyNext = &pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[nNextKey].aKey[nCntModel];

			// モデルのポインタ
			pModel = &pPlayer->aModel[nCntModel];

			if (pPlayer->bBlendMotion == true)
			{// ブレンドあり
				// ローカル変数宣言
				KEY* pKeyBlend;
				KEY* pKeyNextBlend;
				D3DXVECTOR3 posBlend, rotBlend;
				float fDiffKeyBlend, fDiffBlend, fRateKeyBlend, fRateBlend;
				int nNextKeyBlend;

				// 1フレームあたりに動く割合
				fRateKeyBlend = ((float)pPlayer->nCounterMotionBlend / (float)pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[pPlayer->nKeyBlend].nFrame);

				// ブレンドの相対値
				fRateBlend = (float)pPlayer->nCounterBlend / (float)pPlayer->nFrameBlend;

				// 次のキー
				if (pPlayer->bLoopMotionBlend == true)
				{// ループモーション
					nNextKeyBlend = (pPlayer->nKeyBlend + 1) % (pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					nNextKeyBlend = pPlayer->nKeyBlend + 1;

					if (nNextKeyBlend > pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						nNextKeyBlend = pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey - 1;
					}
				}

				// 現在のキーのポインタ
				pKeyBlend = &pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[pPlayer->nKeyBlend].aKey[nCntModel];

				// 次のキーのポインタ
				pKeyNextBlend = &pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[nNextKeyBlend].aKey[nCntModel];

				// キー情報から位置・向きを算出
				// 位置X
				fDiffKey = pKeyNext->fPosX - pKey->fPosX;					// 現在のモーション
				posCurrent.x = pKey->fPosX + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosX - pKeyBlend->fPosX;	// ブレンドモーション
				posBlend.x = pKeyBlend->fPosX + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.x - posCurrent.x;						// 差分
				posDest.x = posCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

				// 位置Y
				fDiffKey = pKeyNext->fPosY - pKey->fPosY;					// 現在のモーション
				posCurrent.y = pKey->fPosY + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosY - pKeyBlend->fPosY;	// ブレンドモーション
				posBlend.y = pKeyBlend->fPosY + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.y - posCurrent.y;						// 差分
				posDest.y = posCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

				// 位置Z
				fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;					// 現在のモーション
				posCurrent.z = pKey->fPosZ + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fPosZ - pKeyBlend->fPosZ;	// ブレンドモーション
				posBlend.z = pKeyBlend->fPosZ + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = posBlend.z - posCurrent.z;						// 差分
				posDest.z = posCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

				// 向きX
				fDiffKey = pKeyNext->fRotX - pKey->fRotX;					// 現在のモーション
				rotCurrent.x = pKey->fRotX + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotX - pKeyBlend->fRotX;	// ブレンドモーション
				rotBlend.x = pKeyBlend->fRotX + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.x - rotCurrent.x;						// 差分
				rotDest.x = rotCurrent.x + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.x > D3DX_PI)
				{
					rotDest.x -= D3DX_PI * 2;
				}
				else if (rotDest.x < -D3DX_PI)
				{
					rotDest.x += D3DX_PI * 2;
				}

				// 向きY
				fDiffKey = pKeyNext->fRotY - pKey->fRotY;					// 現在のモーション
				rotCurrent.y = pKey->fRotY + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotY - pKeyBlend->fRotY;	// ブレンドモーション
				rotBlend.y = pKeyBlend->fRotY + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.y - rotCurrent.y;						// 差分
				rotDest.y = rotCurrent.y + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.y > D3DX_PI)
				{
					rotDest.y -= D3DX_PI * 2;
				}
				else if (rotDest.y < -D3DX_PI)
				{
					rotDest.y += D3DX_PI * 2;
				}

				// 向きZ
				fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;					// 現在のモーション
				rotCurrent.z = pKey->fRotZ + (fDiffKey * fRateKey);
				fDiffKeyBlend = pKeyNextBlend->fRotZ - pKeyBlend->fRotZ;	// ブレンドモーション
				rotBlend.z = pKeyBlend->fRotZ + (fDiffKeyBlend * fRateKeyBlend);
				fDiffBlend = rotBlend.z - rotCurrent.z;						// 差分
				rotDest.z = rotCurrent.z + (fDiffBlend * fRateBlend);		// 求める値

				// 向きを調整
				if (rotDest.z > D3DX_PI)
				{
					rotDest.z -= D3DX_PI * 2;
				}
				else if (rotDest.z < -D3DX_PI)
				{
					rotDest.z += D3DX_PI * 2;
				}
			}
			else
			{// ブレンドなし
				// キー情報から位置・向きを算出
				// 位置X
				fDiffKey = pKeyNext->fPosX - pKey->fPosX;
				posDest.x = pKey->fPosX + (fDiffKey * fRateKey);

				// 位置Y
				fDiffKey = pKeyNext->fPosY - pKey->fPosY;
				posDest.y = pKey->fPosY + (fDiffKey * fRateKey);

				// 位置Z
				fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
				posDest.z = pKey->fPosZ + (fDiffKey * fRateKey);

				// 向きX
				fDiffKey = pKeyNext->fRotX - pKey->fRotX;
				rotDest.x = pKey->fRotX + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.x > D3DX_PI)
				{
					rotDest.x -= D3DX_PI * 2;
				}
				else if (rotDest.x < -D3DX_PI)
				{
					rotDest.x += D3DX_PI * 2;
				}

				// 向きY
				fDiffKey = pKeyNext->fRotY - pKey->fRotY;
				rotDest.y = pKey->fRotY + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.y > D3DX_PI)
				{
					rotDest.y -= D3DX_PI * 2;
				}
				else if (rotDest.y < -D3DX_PI)
				{
					rotDest.y += D3DX_PI * 2;
				}

				// 向きZ
				fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;
				rotDest.z = pKey->fRotZ + (fDiffKey * fRateKey);

				// 向きを調整
				if (rotDest.z > D3DX_PI)
				{
					rotDest.z -= D3DX_PI * 2;
				}
				else if (rotDest.z < -D3DX_PI)
				{
					rotDest.z += D3DX_PI * 2;
				}
			}

			// パーツの位置・向きを設定
			pModel->pos = posDest + pPlayer->aModel[nCntModel].posOff;
			pModel->rot = rotDest + pPlayer->aModel[nCntModel].rotOff;
		}

		if (pPlayer->bBlendMotion == true)
		{// ブレンドあり
			pPlayer->nCounterMotion++;

			if (pPlayer->nCounterMotion >= pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pPlayer->nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (pPlayer->bLoopMotion == true)
				{// ループモーション
					pPlayer->nKey = (pPlayer->nKey + 1) % (pPlayer->aMotionInfo[pPlayer->motionType].nNumKey);
				}
				else
				{// ループしないモーション
					pPlayer->nKey++;

					if (pPlayer->nKey >= pPlayer->aMotionInfo[pPlayer->motionType].nNumKey - 1)
					{// キーの総数を超えた
						pPlayer->nKey = pPlayer->aMotionInfo[pPlayer->motionType].nNumKey - 1;

						pPlayer->bFinishMotion = true;
					}
				}
			}

			pPlayer->nCounterMotionBlend++;

			if (pPlayer->nCounterMotionBlend >= pPlayer->aMotionInfo[pPlayer->motionTypeBlend].aKeyInfo[pPlayer->nKeyBlend].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pPlayer->nCounterMotionBlend = 0;	// カウンターを戻す

				// ループかどうか
				if (pPlayer->bLoopMotionBlend == true)
				{// ループモーション
					pPlayer->nKeyBlend = (pPlayer->nKeyBlend + 1) % (pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					pPlayer->nKeyBlend++;

					if (pPlayer->nKeyBlend >= pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						pPlayer->nKeyBlend = pPlayer->aMotionInfo[pPlayer->motionTypeBlend].nNumKey - 1;

						pPlayer->bFinishMotion = true;
					}
				}
			}

			pPlayer->nCounterBlend++;

			if (pPlayer->nCounterBlend >= pPlayer->nFrameBlend)
			{// ブレンドフレームに到達
				// ブレンドモーションを現在のモーションに設定
				pPlayer->motionType = pPlayer->motionTypeBlend;
				pPlayer->bLoopMotion = pPlayer->bLoopMotionBlend;
				pPlayer->nNumKey = pPlayer->nNumKeyBlend;
				pPlayer->nKey = pPlayer->nKeyBlend;
				pPlayer->nCounterMotion = pPlayer->nCounterMotionBlend;
				//pPlayer->bFinishMotion = true;

				pPlayer->nKeyBlend = 0;
				pPlayer->nCounterMotionBlend = 0;
				pPlayer->bBlendMotion = false;
				pPlayer->nCounterBlend = 0;
				pPlayer->nFrameBlend = 0;
			}
		}
		else
		{// ブレンドなし
			pPlayer->nCounterMotion++;

			if (pPlayer->nCounterMotion >= pPlayer->aMotionInfo[pPlayer->motionType].aKeyInfo[pPlayer->nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				pPlayer->nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (pPlayer->bLoopMotion == true)
				{// ループモーション
					pPlayer->nKey = (pPlayer->nKey + 1) % (pPlayer->aMotionInfo[pPlayer->motionType].nNumKey);
				}
				else
				{// ループしないモーション
					pPlayer->nKey++;

					if (pPlayer->nKey >= pPlayer->aMotionInfo[pPlayer->motionType].nNumKey - 1)
					{// キーの総数を超えた
						pPlayer->nKey = pPlayer->aMotionInfo[pPlayer->motionType].nNumKey - 1;

						pPlayer->bFinishMotion = true;
					}
				}
			}
		}
	}
}

//=============================================================================
// プレイヤーモーションの設定処理
//=============================================================================
void SetMotionPlayer(int nIdx, MOTIONTYPE motionType, bool bBlendMotion, int nFrameBlend)
{
	Player* pPlayer = GetPlayer();
	pPlayer = &pPlayer[nIdx];

	if (pPlayer->motionTypeBlend != motionType)
	{// 違うモーションが設定されたときだけ
		if (bBlendMotion == true)
		{// ブレンドあり
			pPlayer->motionTypeBlend = motionType;
			pPlayer->bLoopMotionBlend = pPlayer->aMotionInfo[motionType].bLoop;
			pPlayer->nNumKeyBlend = pPlayer->aMotionInfo[motionType].nNumKey;
			pPlayer->nKeyBlend = 0;
			pPlayer->nCounterMotionBlend = 0;
			pPlayer->bFinishMotion = false;

			pPlayer->bBlendMotion = bBlendMotion;
			pPlayer->nFrameBlend = nFrameBlend;
			pPlayer->nCounterBlend = 0;
		}
		else
		{// ブレンドなし
			pPlayer->motionType = motionType;
			pPlayer->bLoopMotion = pPlayer->aMotionInfo[motionType].bLoop;
			pPlayer->nNumKey = pPlayer->aMotionInfo[motionType].nNumKey;
			pPlayer->nKey = 0;
			pPlayer->nCounterMotion = 0;
			pPlayer->bFinishMotion = false;

			pPlayer->bBlendMotion = bBlendMotion;

			// 全モデル(パーツ)の初期設定
			for (int nCntModel = 0; nCntModel < pPlayer->nNumModel; nCntModel++)
			{
				pPlayer->aModel[nCntModel].pos.x = pPlayer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosX;
				pPlayer->aModel[nCntModel].pos.y = pPlayer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosY;
				pPlayer->aModel[nCntModel].pos.z = pPlayer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosZ;

				pPlayer->aModel[nCntModel].rot.x = pPlayer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotX;
				pPlayer->aModel[nCntModel].rot.y = pPlayer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotY;
				pPlayer->aModel[nCntModel].rot.z = pPlayer->aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotZ;
			}
		}
	}
}

//=============================================================================
// エサキューにエンキュー
//=============================================================================
void Enqueue(EsaQueue* queue, int nIdx)
{
	if (queue->nTail == MAX_QUEUE - 1)
	{// キューが満杯なら何もせず関数終了 
		return;
	}

	// データをデータの最後尾の１つ後ろに格納
	queue->nData[queue->nTail + 1] = nIdx;

	// データの最後尾を１つ後ろに移動 
	queue->nTail = queue->nTail + 1;
}

//=============================================================================
// エサキューにデキュー
//=============================================================================
int Dequeue(EsaQueue* queue)
{
	int nIdx = -1;

	if (queue->nTail == -1)
	{// キューが空なら何もせずに関数終了
		return -1;
	}

	// データの先頭からデータを取得
	nIdx = queue->nData[0];

	// データの先頭を１つ後ろにずらす
	for (int nCnt = 0; nCnt < queue->nTail; nCnt++)
	{
		queue->nData[nCnt] = queue->nData[nCnt + 1];
	}

	queue->nData[queue->nTail] = -1;

	// データの最後尾も１つ前にずらす
	queue->nTail = queue->nTail - 1;

	// 取得したデータを返す
	return nIdx;
}