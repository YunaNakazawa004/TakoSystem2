//=============================================================================
// 
// プレイヤー [player.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "player.h"
#include "computer.h"
#include "ui_esa.h"
#include "meshfield.h"
#include "meshcylinder.h"
#include "meshring.h"
#include "meshorbit.h"
#include "oceancurrents.h"
#include "object.h"
#include "particle_3d.h"
#include "crosshair.h"
#include "watersurf.h"
#include "pot.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "time.h"
#include "sound.h"
#include "seaweed.h"
#include "debugproc.h"
#include "readygo.h"
#include "spray.h"
#include "bubble.h"
#include "tutorialtxt.h"
#include "foodnum.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MOVEMENT				(D3DXVECTOR3(0.3f, 0.3f, 0.3f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define GRAVITY					(-0.45f)								// 重力
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
#define FLOW_COUNT				(10)									// 波が出る間隔
#define PLAYER_TENTACLE			(8)										// プレイヤーの足の数
#define PLAYER_RADIUS			(25.0f)									// 半径
#define PLAYER_HEIGHT			(50.0f)									// 高さ
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
	//LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ
	//D3DXMATERIAL* pMat;
	Player* pPlayer = GetPlayer();

	// プレイヤーの情報の初期化
	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++, pPlayer++)
	{
		g_aPlayer[nCntPlayer].nIdx = nCntPlayer;
		g_aPlayer[nCntPlayer].nCounter = 0;
		g_aPlayer[nCntPlayer].pos = FIRST_POS;
		g_aPlayer[nCntPlayer].posOld = FIRST_POS;
		g_aPlayer[nCntPlayer].move = FIRST_POS;
		g_aPlayer[nCntPlayer].rot = FIRST_POS;
		g_aPlayer[nCntPlayer].state = PLAYERSTATE_APPEAR;
		g_aPlayer[nCntPlayer].mode = PLAYERMODE_TUTORIAL;
		g_aPlayer[nCntPlayer].TentacleState = PLTENTACLESTATE_NORMAL;
		g_aPlayer[nCntPlayer].nCounterState = 0;
		g_aPlayer[nCntPlayer].fAutoY = 0.0f;
		g_aPlayer[nCntPlayer].fAngleX = 0.0f;
		g_aPlayer[nCntPlayer].fAngleY = 0.0f;
		g_aPlayer[nCntPlayer].fFogStart = (g_aPlayer[nCntPlayer].pos.y * 0.4f) + FOGS_MIN;
		g_aPlayer[nCntPlayer].fFogEnd = (g_aPlayer[nCntPlayer].pos.y * 1.1f) + FOGE_MIN;
		g_aPlayer[nCntPlayer].fRadius = PLAYER_RADIUS;
		g_aPlayer[nCntPlayer].fHeight = PLAYER_HEIGHT;
		g_aPlayer[nCntPlayer].bJump = false;
		g_aPlayer[nCntPlayer].bLand = true;
		g_aPlayer[nCntPlayer].bMove = false;
		g_aPlayer[nCntPlayer].bAct = false;
		g_aPlayer[nCntPlayer].bUse = false;
		g_aPlayer[nCntPlayer].bBlind = false;
		g_aPlayer[nCntPlayer].nBlindCounter = 0;
		memset(&g_aPlayer[nCntPlayer].nOrbitIdx, -1, sizeof(int[8]));
		g_aPlayer[nCntPlayer].nFood = 0;
		g_aPlayer[nCntPlayer].nFoodNumIdx = -1;
		g_aPlayer[nCntPlayer].esaQueue.nTail = -1;
		memset(&g_aPlayer[nCntPlayer].esaQueue.nData, -1, sizeof(int[MAX_QUEUE]));
		g_aPlayer[nCntPlayer].Potstate = POTSTATE_NONE;
		g_aPlayer[nCntPlayer].nPotIdx = -1;
		g_aPlayer[nCntPlayer].nMaxFood = 0;
		g_aPlayer[nCntPlayer].nTentacleCooldown = 0;
		g_aPlayer[nCntPlayer].nInkCooldown = 0;
	}

	// ファイルからキャラクター情報をロード
	LoadPlayer();

	pPlayer = GetPlayer();

	for (int nCntPlayer = 0; nCntPlayer < MAX_PLAYER; nCntPlayer++, pPlayer++)
	{
		g_aPlayer[nCntPlayer].motionType = MOTIONTYPE_NEUTRAL;
		g_aPlayer[nCntPlayer].bLoopMotion = g_aPlayer[nCntPlayer].aMotionInfo[0].bLoop;
		g_aPlayer[nCntPlayer].nNumKey = g_aPlayer[nCntPlayer].aMotionInfo[0].nNumKey;
		g_aPlayer[nCntPlayer].nKey = 0;
		g_aPlayer[nCntPlayer].nCounterMotion = 0;
		g_aPlayer[nCntPlayer].bFinishMotion = false;
		g_aPlayer[nCntPlayer].bBlendMotion = false;
		g_aPlayer[nCntPlayer].motionTypeBlend = MOTIONTYPE_NEUTRAL;
		g_aPlayer[nCntPlayer].bLoopMotionBlend = g_aPlayer[nCntPlayer].aMotionInfo[0].bLoop;
		g_aPlayer[nCntPlayer].nNumKeyBlend = g_aPlayer[nCntPlayer].aMotionInfo[0].nNumKey;
		g_aPlayer[nCntPlayer].nKeyBlend = 0;
		g_aPlayer[nCntPlayer].nCounterMotionBlend = 0;
		g_aPlayer[nCntPlayer].nFrameBlend = 0;
		g_aPlayer[nCntPlayer].nCounterBlend = 0;
	}
}

//=============================================================================
// プレイヤーの終了処理
//=============================================================================
void UninitPlayer(void)
{
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
		if (g_aPlayer[nCntPlayer].bUse == true)
		{// 使用している
			int nValueH, nValueV;
			int nValue;
			float fmoveAngle = 0.0f;
			float fAngle;

			g_aPlayer[nCntPlayer].posOld = g_aPlayer[nCntPlayer].pos;
			g_aPlayer[nCntPlayer].posX = g_aPlayer[nCntPlayer].pos + (pCamera->posR - pCamera->posV);

			if (GetGameStart() == true && GetMode() == MODE_GAME && g_aPlayer[nCntPlayer].state == PLAYERSTATE_APPEAR)
			{// GOで操作可能にする
				g_aPlayer[nCntPlayer].state = PLAYERSTATE_NORMAL;
			}

			switch (g_aPlayer[nCntPlayer].state)
			{
			case PLAYERSTATE_NORMAL:			// 通常状態
				//PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_NORMAL ]\n");

				break;

			case PLAYERSTATE_MOVE:				// 移動状態
				//PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_MOVE ]\n");

				break;

			case PLAYERSTATE_APPEAR:			// 出現状態
				//PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_APPEAR ]\n");

				break;

			case PLAYERSTATE_WAIT:				// 待機状態
				//PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_WAIT ]\n");

				break;

			case PLAYERSTATE_DASH:				// 高速移動状態
				//PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_DASH ]\n");

				break;

			case PLAYERSTATE_INK:				// 墨吐き状態
				//PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_INK ]\n");

				break;

			case PLAYERSTATE_BACKAREA:			// エリア戻り状態
				//PrintDebugProc("プレイヤーの状態 : [ PLAYERSTATE_BACKAREA ]\n");

				D3DXVECTOR3 correct = -g_aPlayer[nCntPlayer].pos;
				g_aPlayer[nCntPlayer].move += *D3DXVec3Normalize(&g_aPlayer[nCntPlayer].move, &correct);

				g_aPlayer[nCntPlayer].nCounterState--;

				if (g_aPlayer[nCntPlayer].nCounterState < 0)
				{// 戻り状態を終わる
					g_aPlayer[nCntPlayer].state = PLAYERSTATE_NORMAL;
					g_aPlayer[nCntPlayer].nCounterState = 0;
				}

				break;
			}

			//PrintDebugProc("エサの数 %d / %d\n", g_aPlayer[nCntPlayer].nFood, g_aPlayer[nCntPlayer].nMaxFood * PLAYER_TENTACLE);

			if (g_aPlayer[nCntPlayer].state != PLAYERSTATE_APPEAR && g_aPlayer[nCntPlayer].state != PLAYERSTATE_DASH && g_aPlayer[nCntPlayer].state != PLAYERSTATE_BACKAREA)
			{// 出現状態のときは移動できない
				// パッド移動
				if (GetJoypadStick(nCntPlayer, JOYKEY_LEFTSTICK, &nValueH, &nValueV) == true)
				{// パッドの移動優先
					if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
					{// 触手を伸ばしているときは向きを変えない
						g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y + atan2f(-(float)nValueH, -D3DX_PI - (float)nValueV);
					}

					fAngle = atan2f((float)(nValueH), (float)(nValueV));

					g_aPlayer[nCntPlayer].move.x += sinf(fAngle + pCamera->rot.y) * MOVEMENT.x/* * sinf((D3DX_PI * 0.5f) + pCamera->fAngle)*/;
					//g_aPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * (nValueV / 30300) * MOVEMENT.y;
					g_aPlayer[nCntPlayer].move.z += cosf(fAngle + pCamera->rot.y) * MOVEMENT.z /** sinf((D3DX_PI * 0.5f) + pCamera->fAngle)*/;

					g_aPlayer[nCntPlayer].bMove = true;
				}
				else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_W) == true : GetKeyboardPress(DIK_UP) == true) || GetJoypadPress(nCntPlayer, JOYKEY_UP) == true)		// キーボード移動
				{// 奥に移動
					if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_A) == true : GetKeyboardPress(DIK_LEFT) == true) || GetJoypadPress(nCntPlayer, JOYKEY_LEFT) == true)
					{// 左奥に移動
						g_aPlayer[nCntPlayer].move.x += sinf(-D3DX_PI * 0.75f - pCamera->rot.y) * MOVEMENT.x;
						//g_aPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * MOVEMENT.y;
						g_aPlayer[nCntPlayer].move.z += cosf(-D3DX_PI * 0.25f + pCamera->rot.y) * MOVEMENT.z;

						if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y - (-D3DX_PI * 0.75f);
						}
					}
					else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_D) == true : GetKeyboardPress(DIK_RIGHT) == true) || GetJoypadPress(nCntPlayer, JOYKEY_RIGHT) == true)
					{// 右奥に移動
						g_aPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.75f - pCamera->rot.y) * MOVEMENT.x;
						//g_aPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * MOVEMENT.y;
						g_aPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.25f + pCamera->rot.y) * MOVEMENT.z;

						if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y - (D3DX_PI * 0.75f);
						}
					}
					else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_W) == true : GetKeyboardPress(DIK_UP) == true) || GetJoypadPress(nCntPlayer, JOYKEY_UP) == true)
					{// 奥に移動
						g_aPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.0f + pCamera->rot.y) * MOVEMENT.x;
						//g_aPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * MOVEMENT.y;
						g_aPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.0f + pCamera->rot.y) * MOVEMENT.z;

						if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y - D3DX_PI;
						}
					}

					g_aPlayer[nCntPlayer].bMove = true;
				}
				else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_S) == true : GetKeyboardPress(DIK_DOWN) == true) || GetJoypadPress(nCntPlayer, JOYKEY_DOWN) == true)
				{// 手前に移動
					if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_A) == true : GetKeyboardPress(DIK_LEFT) == true) || GetJoypadPress(nCntPlayer, JOYKEY_LEFT) == true)
					{// 左手前に移動
						g_aPlayer[nCntPlayer].move.x += sinf(-D3DX_PI * 0.25f - pCamera->rot.y) * MOVEMENT.x;
						//g_aPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * -MOVEMENT.y;
						g_aPlayer[nCntPlayer].move.z += cosf(-D3DX_PI * 0.75f + pCamera->rot.y) * MOVEMENT.z;

						if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y + (D3DX_PI * 0.25f);
						}
					}
					else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_D) == true : GetKeyboardPress(DIK_RIGHT) == true) || GetJoypadPress(nCntPlayer, JOYKEY_RIGHT) == true)
					{// 右手前に移動
						g_aPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.25f - pCamera->rot.y) * MOVEMENT.x;
						//g_aPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * -MOVEMENT.y;
						g_aPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.75f + pCamera->rot.y) * MOVEMENT.z;

						if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y + (-D3DX_PI * 0.25f);
						}
					}
					else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_S) == true : GetKeyboardPress(DIK_DOWN) == true) || GetJoypadPress(nCntPlayer, JOYKEY_DOWN) == true)
					{// 手前に移動
						g_aPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 1.0f + pCamera->rot.y) * MOVEMENT.x;
						//g_aPlayer[nCntPlayer].move.y += cosf(((D3DX_PI * 0.5f) + pCamera->fAngle)) * -MOVEMENT.y;
						g_aPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 1.0f + pCamera->rot.y) * MOVEMENT.z;

						if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
						{// 触手を伸ばしているときは向きを変えない
							g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y;
						}
					}

					g_aPlayer[nCntPlayer].bMove = true;
				}
				else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_A) == true : GetKeyboardPress(DIK_LEFT) == true) || GetJoypadPress(nCntPlayer, JOYKEY_LEFT) == true)
				{// 左に移動
					g_aPlayer[nCntPlayer].move.x += sinf(-D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.x;
					g_aPlayer[nCntPlayer].move.z += cosf(-D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.z;

					if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
					{// 触手を伸ばしているときは向きを変えない
						g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y + (D3DX_PI * 0.5f);
					}

					g_aPlayer[nCntPlayer].bMove = true;
				}
				else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_D) == true : GetKeyboardPress(DIK_RIGHT) == true) || GetJoypadPress(nCntPlayer, JOYKEY_RIGHT) == true)
				{// 右に移動
					g_aPlayer[nCntPlayer].move.x += sinf(D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.x;
					g_aPlayer[nCntPlayer].move.z += cosf(D3DX_PI * 0.5f + pCamera->rot.y) * MOVEMENT.z;

					if (g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG)
					{// 触手を伸ばしているときは向きを変えない
						g_aPlayer[nCntPlayer].fAngleY = pCamera->rot.y - (D3DX_PI * 0.5f);
					}

					g_aPlayer[nCntPlayer].bMove = true;
				}
				else
				{// プレイヤーの入力がない
					g_aPlayer[nCntPlayer].bMove = false;
				}

				if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_LCONTROL) == true : GetKeyboardPress(DIK_NUMPAD0) == true) ||
					/*GetJoypadPress(nCntPlayer, JOYKEY_LEFT_SHOULDER) == true*/
					GetJoypadShoulder(nCntPlayer, JOYKEY_LEFTTRIGGER, &nValue) == true)
				{// 下降
					g_aPlayer[nCntPlayer].move.y += -MOVEMENT.y;

					g_aPlayer[nCntPlayer].bMove = true;

					if (g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
					{// チュートリアルモード
						SetTutorialTxtState(TUTTXTTYPE_DOWN, TUTTXTSTATE_CLEAR);
					}
				}
				else if ((nCntPlayer == 0 ? GetKeyboardPress(DIK_LSHIFT) == true : GetKeyboardPress(DIK_NUMPAD2) == true) ||
					GetJoypadPress(nCntPlayer, JOYKEY_LEFT_SHOULDER) == true)
				{// 上昇
					g_aPlayer[nCntPlayer].move.y += MOVEMENT.y;

					g_aPlayer[nCntPlayer].bMove = true;

					if (g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
					{// チュートリアルモード
						SetTutorialTxtState(TUTTXTTYPE_UP, TUTTXTSTATE_CLEAR);
					}
				}
			}

			switch (g_aPlayer[nCntPlayer].TentacleState)
			{
			case PLTENTACLESTATE_NORMAL:			// 通常状態
				g_aPlayer[nCntPlayer].aModel[2].scale.y = 1.0f;
				g_aPlayer[nCntPlayer].fAngleX = 0.0f;
				SetCameraViewAngle(nCntPlayer, 45.0f);

				break;

			case PLTENTACLESTATE_TENTACLELONG:		// 触手伸ばし状態
				if (g_aPlayer[nCntPlayer].motionType == MOTIONTYPE_INK || g_aPlayer[nCntPlayer].motionTypeBlend == MOTIONTYPE_INK)
				{// モーションキャンセル
					g_aPlayer[nCntPlayer].TentacleState = PLTENTACLESTATE_NORMAL;
				}
				else
				{// キャンセルしない
					if (g_aPlayer[nCntPlayer].bFinishMotion == true)
					{// 触手が伸ばし終わったら
						CrossHair* pCrossHair = GetCrossHair();
						pCrossHair = &pCrossHair[nCntPlayer];

						D3DXVECTOR3 tentaclePos = D3DXVECTOR3(g_aPlayer[nCntPlayer].aModel[4].mtxWorld._41, g_aPlayer[nCntPlayer].aModel[4].mtxWorld._42, g_aPlayer[nCntPlayer].aModel[4].mtxWorld._43);
						int nIdx = -1;

						if (CollisionEsa(&nIdx, false, &tentaclePos, TENTACLE_RADIUS) == true &&
							g_aPlayer[nCntPlayer].nFood < g_aPlayer[nCntPlayer].nMaxFood * PLAYER_TENTACLE &&
							g_aPlayer[nCntPlayer].motionType != MOTIONTYPE_OCEANCULLENT)
						{// エサと接触した
							Esa* pEsa = GetEsa();

							if (pEsa[nIdx].esaType != ESA_ACTTYPE_GOTO_POT)
							{// タコつぼに入れてる最中じゃない

								// エサの削除処理
								int nIdxEsaType = DelEsa(nIdx, true, nCntPlayer);	// 削除したエサの種類を獲得

								if (nIdxEsaType != -1)
								{
									g_aPlayer[nCntPlayer].nFood++;
									Enqueue(&g_aPlayer[nCntPlayer].esaQueue, nIdxEsaType);
									EsaPlaySE(nIdxEsaType);
								}
							}
						}
						else if (CollisionPotArea(tentaclePos, TENTACLE_RADIUS * 0.5f, pPlayer, NULL, true) == true ||
							CollisionOcto(nCntPlayer, false, tentaclePos) == true)
						{// エサをとる
							g_aPlayer[nCntPlayer].TentacleState = PLTENTACLESTATE_TENTACLESHORT;

							SetMotionPlayer(nCntPlayer, MOTIONTYPE_TENTACLESHORT, true, 20);
						}
						else if (pCrossHair->state == CROSSHAIRSTATE_REACH &&
							(CollisionMeshCylinder(&tentaclePos, &g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].move,
								TENTACLE_RADIUS, TENTACLE_RADIUS, true) == true ||
								tentaclePos.y < 0.0f) ||
							CollisionObject(&tentaclePos, &g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].move,
								TENTACLE_RADIUS, TENTACLE_RADIUS, true) == true)
						{// 壁との当たり判定
							g_aPlayer[nCntPlayer].state = PLAYERSTATE_DASH;
							g_aPlayer[nCntPlayer].TentacleState = PLTENTACLESTATE_TENTACLESHORT;
							SetMotionPlayer(nCntPlayer, MOTIONTYPE_DASH, true, 20);
							PlaySound(SOUND_SE_HIGHSPEED);

							SetCameraViewAngle(nCntPlayer, 15.0f);

							// 衝撃波エフェクトの生成
							SetMeshRing(MESHRINGTYPE_SHOCKWAVE, tentaclePos, CalcShockWaveRot(tentaclePos, g_aPlayer[nCntPlayer].pos), D3DXVECTOR2(16.0f, 1.0f), D3DXVECTOR2(10.0f, 7.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
						}
						else
						{// 触手を伸ばす
							if (g_aPlayer[nCntPlayer].aModel[2].scale.y < TENTACLE_RANGE * 0.1f)
							{// リーチより短い
								g_aPlayer[nCntPlayer].aModel[2].scale.y += 5.0f;
							}
							else
							{// リーチの長さになった
								g_aPlayer[nCntPlayer].TentacleState = PLTENTACLESTATE_TENTACLESHORT;

								SetMotionPlayer(nCntPlayer, MOTIONTYPE_TENTACLESHORT, true, 20);
							}
						}
					}
				}

				break;

			case PLTENTACLESTATE_TENTACLESHORT:		// 触手縮め状態
				if (g_aPlayer[nCntPlayer].aModel[2].scale.y > 1.0f)
				{// 触手を短くする
					g_aPlayer[nCntPlayer].aModel[2].scale.y += (1.0f - g_aPlayer[nCntPlayer].aModel[2].scale.y) * 0.5f;

					PlaySound(SOUND_SE_TENTACLE_RETRACT);
				}
				else
				{// 元の長さに戻す
					g_aPlayer[nCntPlayer].aModel[2].scale.y = 1.0f;
					g_aPlayer[nCntPlayer].TentacleState = PLTENTACLESTATE_NORMAL;

					if (g_aPlayer[nCntPlayer].motionType != MOTIONTYPE_DASH)
					{// 高速移動していないとき
						g_aPlayer[nCntPlayer].fAngleX = 0.0f;
					}
				}

				break;
			}

			if (g_aPlayer[nCntPlayer].state == PLAYERSTATE_DASH)
			{// 高速移動
				g_aPlayer[nCntPlayer].pos += g_aPlayer[nCntPlayer].vecX;
				g_aPlayer[nCntPlayer].vecX.x += (0.0f - g_aPlayer[nCntPlayer].vecX.x) * DASH_MOVE;
				g_aPlayer[nCntPlayer].vecX.y += (0.0f - g_aPlayer[nCntPlayer].vecX.y) * DASH_MOVE;
				g_aPlayer[nCntPlayer].vecX.z += (0.0f - g_aPlayer[nCntPlayer].vecX.z) * DASH_MOVE;

				SetVibration(nCntPlayer, 1000, 1300, 10);

				if (g_aPlayer[nCntPlayer].vecX.x < MOVE_ERROR && g_aPlayer[nCntPlayer].vecX.x > -MOVE_ERROR &&
					g_aPlayer[nCntPlayer].vecX.y < MOVE_ERROR && g_aPlayer[nCntPlayer].vecX.y > -MOVE_ERROR &&
					g_aPlayer[nCntPlayer].vecX.z < MOVE_ERROR && g_aPlayer[nCntPlayer].vecX.z > -MOVE_ERROR)
				{// 止まった
					g_aPlayer[nCntPlayer].vecX = FIRST_POS;

					SetCameraViewAngle(nCntPlayer, 45.0f);

					if (g_aPlayer[nCntPlayer].motionType == MOTIONTYPE_DASH)
					{// 高速移動モーションしていたら
						if (g_aPlayer[nCntPlayer].bMove == true)
						{// 移動してる
							g_aPlayer[nCntPlayer].state = PLAYERSTATE_MOVE;
							g_aPlayer[nCntPlayer].fAngleX = 0.0f;
							SetMotionPlayer(nCntPlayer, MOTIONTYPE_MOVE, true, 20);
						}
						else
						{// 移動してない
							g_aPlayer[nCntPlayer].state = PLAYERSTATE_WAIT;
							g_aPlayer[nCntPlayer].fAngleX = 0.0f;
							SetMotionPlayer(nCntPlayer, MOTIONTYPE_NEUTRAL, true, 20);
						}
					}
				}
			}

			if (g_aPlayer[nCntPlayer].bMove == false)
			{// 動いてないとき
				g_aPlayer[nCntPlayer].fAutoY += 0.05f;
				g_aPlayer[nCntPlayer].move.y += cosf(g_aPlayer[nCntPlayer].fAutoY) * 0.03f;
			}
			else if (g_aPlayer[nCntPlayer].bMove == true && g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
			{// 動いているとき
				SetTutorialTxtState(TUTTXTTYPE_MOVE, TUTTXTSTATE_CLEAR);
			}

			if (g_aPlayer[nCntPlayer].nCounter % (ONE_SECOND * 10) == 0)
			{// 泳いでいる音
				PlaySound(SOUND_SE_SWIM);
			}

			if (GetOceanCurrents() != OCEANCURRENTSSTATE_WIRLPOOL ||
				(GetOceanCurrents() == OCEANCURRENTSSTATE_WIRLPOOL && CollisionObjectArea(g_aPlayer[nCntPlayer].pos) == true))
			{
				if (g_aPlayer[nCntPlayer].state != PLAYERSTATE_DASH && g_aPlayer[nCntPlayer].state != PLAYERSTATE_INK && g_aPlayer[nCntPlayer].state != PLAYERSTATE_BACKAREA
					&& g_aPlayer[nCntPlayer].bMove == true && g_aPlayer[nCntPlayer].TentacleState == PLTENTACLESTATE_NORMAL)
				{// 普通に移動しているとき
					g_aPlayer[nCntPlayer].state = PLAYERSTATE_MOVE;
					SetMotionPlayer(nCntPlayer, MOTIONTYPE_MOVE, true, 20);
				}
				else if (g_aPlayer[nCntPlayer].bMove == false && g_aPlayer[nCntPlayer].state != PLAYERSTATE_DASH &&
					g_aPlayer[nCntPlayer].state != PLAYERSTATE_INK && g_aPlayer[nCntPlayer].TentacleState == PLTENTACLESTATE_NORMAL)
				{// 待機モーション
					SetMotionPlayer(nCntPlayer, MOTIONTYPE_NEUTRAL, true, 20);
				}
			}

			//PrintDebugProc("プレイヤーのpos ( %f %f %f )\n", g_aPlayer[nCntPlayer].pos.x, g_aPlayer[nCntPlayer].pos.y, g_aPlayer[nCntPlayer].pos.z);

#ifdef _DEBUG
			if (GetKeyboardTrigger(DIK_BACKSPACE) == true || GetJoypadTrigger(nCntPlayer, JOYKEY_LEFT_THUMB) == true)
			{// 位置回転リセット
				g_aPlayer[nCntPlayer].pos = START_POS;
				g_aPlayer[nCntPlayer].posOld = START_POS;
				g_aPlayer[nCntPlayer].move = FIRST_POS;
				g_aPlayer[nCntPlayer].rot = FIRST_POS;
				g_aPlayer[nCntPlayer].fAngleY = 0.0f;
				g_aPlayer[nCntPlayer].state = PLAYERSTATE_WAIT;
			}
#endif
			// クールダウン
			if (g_aPlayer[nCntPlayer].nTentacleCooldown > 0)
			{// 触手クールダウン
				g_aPlayer[nCntPlayer].nTentacleCooldown--;
			}

			if (g_aPlayer[nCntPlayer].nInkCooldown > 0)
			{// 墨吐きクールダウン
				g_aPlayer[nCntPlayer].nInkCooldown--;
			}

			if (g_aPlayer[nCntPlayer].nBlindCounter > 0)
			{// 視界悪化カウント
				g_aPlayer[nCntPlayer].nBlindCounter--;
			}
			else if (g_aPlayer[nCntPlayer].nBlindCounter == 0)
			{// 視界悪化が終わる
				g_aPlayer[nCntPlayer].bBlind = false;
				g_aPlayer[nCntPlayer].nBlindCounter = 0;
			}

			//PrintDebugProc("プレイヤーのmove ( %f %f %f )\n", g_aPlayer[nCntPlayer].move.x, g_aPlayer[nCntPlayer].move.y, g_aPlayer[nCntPlayer].move.z);

			if (CollisionObjectArea(g_aPlayer[nCntPlayer].pos) == false && g_aPlayer[nCntPlayer].pos.y < *GetWaterSurf_Height() + (PLAYER_HEIGHT * 0.5f))
			{// 安地外のときに渦潮

				if (g_aPlayer[nCntPlayer].mode != PLAYERMODE_TUTORIAL)
				{// チュートリアル中以外
					MoveOceanCurrents(&g_aPlayer[nCntPlayer].pos);

					if (GetOceanCurrents() == OCEANCURRENTSSTATE_WIRLPOOL)
					{// 安地外で渦潮
						if (g_aPlayer[nCntPlayer].TentacleState == PLTENTACLESTATE_NORMAL && g_aPlayer[nCntPlayer].state != PLAYERSTATE_DASH &&
							g_aPlayer[nCntPlayer].state != PLAYERSTATE_INK && g_aPlayer[nCntPlayer].state != PLAYERSTATE_BACKAREA)
						{// 触手が通常状態のときだけ
							SetMotionPlayer(nCntPlayer, MOTIONTYPE_OCEANCULLENT, true, 20);

							SetVibration(nCntPlayer, 10000, 10000, 1);	// 渦潮に巻き込まれている時の振動
						}

						if (g_aPlayer[nCntPlayer].nFood > 0 && g_aPlayer[nCntPlayer].nCounter % 15 == 0)
						{// エサを持っている
							g_aPlayer[nCntPlayer].nFood--;
							int nIdx = Dequeue(&g_aPlayer[nCntPlayer].esaQueue);
							SetSubUiEsa(nCntPlayer);

							SetEsa(nIdx, true, ESA_ACTTYPE_SWIM, 0, g_aPlayer[nCntPlayer].pos, FIRST_POS);
						}
					}
				}
			}

			if (g_aPlayer[nCntPlayer].state != PLAYERSTATE_DASH)
			{// 出現状態以外
				// 慣性
				g_aPlayer[nCntPlayer].pos += g_aPlayer[nCntPlayer].move;
				g_aPlayer[nCntPlayer].move.x += (0.0f - g_aPlayer[nCntPlayer].move.x) * INERTIA_MOVE;
				g_aPlayer[nCntPlayer].move.y += (0.0f - g_aPlayer[nCntPlayer].move.y) * INERTIA_MOVE;
				g_aPlayer[nCntPlayer].move.z += (0.0f - g_aPlayer[nCntPlayer].move.z) * INERTIA_MOVE;
			}

			D3DXVECTOR2 XZdist = D3DXVECTOR2(g_aPlayer[nCntPlayer].pos.x, g_aPlayer[nCntPlayer].pos.z);
			float fDist = D3DXVec2Length(&XZdist);

			if (fDist > OUTCYLINDER_RADIUS + ((OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) / 2) &&
				g_aPlayer[nCntPlayer].mode != PLAYERMODE_TUTORIAL)
			{// 移動制限
				g_aPlayer[nCntPlayer].fAngleY = atan2f(g_aPlayer[nCntPlayer].pos.x, g_aPlayer[nCntPlayer].pos.z);
				g_aPlayer[nCntPlayer].state = PLAYERSTATE_BACKAREA;
				g_aPlayer[nCntPlayer].nCounterState = ONE_SECOND;
			}
			else if (fDist <= OUTCYLINDER_RADIUS + ((OUTCYLINDER_RADIUS - INCYLINDER_RADIUS) / 2) &&
				g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
			{// エリア内に入ったらチュートリアルモード解除
				if (g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
				{// チュートリアルモード
					SetTutorialTxtState(TUTTXTTYPE_RULE, TUTTXTSTATE_FADE);

					SetTutorialTxt(TUTTXTTYPE_POT, TUTTXTSTATE_DISP, D3DXVECTOR3(0.0f, 700.0f, 900.0f));
					SetTutorialTxt(TUTTXTTYPE_ESA, TUTTXTSTATE_DISP, D3DXVECTOR3(700.0f, 400.0f, 700.0f));

					SetTutorialTxt(TUTTXTTYPE_INK, TUTTXTSTATE_DISP, D3DXVECTOR3(-700.0f, 500.0f, 700.0f));
					SetTutorialTxt(TUTTXTTYPE_TENT, TUTTXTSTATE_DISP, D3DXVECTOR3(-700.0f, 300.0f, 700.0f));
					SetTutorialTxt(TUTTXTTYPE_UP, TUTTXTSTATE_DISP, D3DXVECTOR3(700.0f, 700.0f, -700.0f));
					SetTutorialTxt(TUTTXTTYPE_DOWN, TUTTXTSTATE_DISP, D3DXVECTOR3(700.0f, 500.0f, -700.0f));
					SetTutorialTxt(TUTTXTTYPE_CAMERA, TUTTXTSTATE_DISP, D3DXVECTOR3(-700.0f, 500.0f, -700.0f));
					SetTutorialTxt(TUTTXTTYPE_MOVE, TUTTXTSTATE_DISP, D3DXVECTOR3(-700.0f, 700.0f, -700.0f));
				}

				g_aPlayer[nCntPlayer].mode = PLAYERMODE_GAME;

				//// ランダムな位置に設定	このままじゃ2Pのとき2回目入るから一旦放置
				//SetRandomComputer(ALL_OCTO - GetNumCamera());
			}

			if (g_aPlayer[nCntPlayer].pos.y < 0.0f)
			{// 底
				g_aPlayer[nCntPlayer].pos.y = 0.0f;

				if (g_aPlayer[nCntPlayer].bLand == false)
				{// ついてなかった場合
					SetSprayCircle(D3DXVECTOR3(g_aPlayer[nCntPlayer].pos.x, g_aPlayer[nCntPlayer].pos.y + 30.0f, g_aPlayer[nCntPlayer].pos.z),
						D3DXCOLOR(0.9f, 0.9f, 0.7f, 1.0f), SPRAYTYPE_CIRCLE);

					SetVibration(nCntPlayer, 10000, 10000, 30);	// 地面に着地した時の振動
				}

				g_aPlayer[nCntPlayer].bLand = true;
			}
			else
			{// ついていないとき
				g_aPlayer[nCntPlayer].bLand = false;
			}

			if (g_aPlayer[nCntPlayer].pos.y < 10.0f && g_aPlayer[nCntPlayer].nCounter % FLOW_COUNT == 0 && g_aPlayer[nCntPlayer].bMove == true)
			{// 地面に近かったら
				SetSprayFlow(D3DXVECTOR3(g_aPlayer[nCntPlayer].pos.x, g_aPlayer[nCntPlayer].pos.y + 20.0f, g_aPlayer[nCntPlayer].pos.z), g_aPlayer[nCntPlayer].rot,
					D3DXCOLOR(0.9f, 0.9f, 0.7f, 1.0f), SPRAYTYPE_FLOW);

				SetVibration(nCntPlayer, 256, 0, 1);	// 地面を移動している時の振動
				PlaySound(SOUND_SE_LANDING);
			}

			if (g_aPlayer[nCntPlayer].pos.y > *GetWaterSurf_Height() - (PLAYER_HEIGHT * 0.5f))
			{// 水上
				// 重力
				g_aPlayer[nCntPlayer].move.y += GRAVITY;
			}
			else if (g_aPlayer[nCntPlayer].pos.y > 340.0f && g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
			{// チュートリアルモード中の上方向制限
				g_aPlayer[nCntPlayer].pos.y = 340.0f;
			}

			if (g_aPlayer[nCntPlayer].pos.y > *GetWaterSurf_Height() - (PLAYER_HEIGHT * 0.5f) &&
				g_aPlayer[nCntPlayer].pos.y < *GetWaterSurf_Height() + (PLAYER_HEIGHT * 0.5f))
			{// 水面付近				
				if (g_aPlayer[nCntPlayer].bJump == false)
				{// 水の中から
					PlaySound(SOUND_SE_FLOW);

					g_aPlayer[nCntPlayer].bJump = true;
				}

				if (g_aPlayer[nCntPlayer].nCounter % RIPPLE_COUNT == 0)
				{// 定期的に波紋
					SetMeshRing(MESHRINGTYPE_RIPPLES, D3DXVECTOR3(g_aPlayer[nCntPlayer].pos.x + (rand() % 6 - 3), *GetWaterSurf_Height(), g_aPlayer[nCntPlayer].pos.z + (rand() % 6 - 3)), FIRST_POS,
						D3DXVECTOR2(24.0f, 1.0f), D3DXVECTOR2(10.0f, 7.0f), D3DXCOLOR(WHITE_VTX.r, WHITE_VTX.g, WHITE_VTX.b, 0.5f));

					SetSprayCircle(D3DXVECTOR3(g_aPlayer[nCntPlayer].pos.x, *GetWaterSurf_Height(), g_aPlayer[nCntPlayer].pos.z),
						WHITE_VTX, SPRAYTYPE_CIRCLE);
				}

				if (g_aPlayer[nCntPlayer].nCounter % FLOW_COUNT == 0 && g_aPlayer[nCntPlayer].bMove == true)
				{// 波
					SetSprayFlow(D3DXVECTOR3(g_aPlayer[nCntPlayer].pos.x, *GetWaterSurf_Height(), g_aPlayer[nCntPlayer].pos.z), g_aPlayer[nCntPlayer].rot,
						WHITE_VTX, SPRAYTYPE_FLOW);

					SetVibration(nCntPlayer, 1000, 3500, 2);	// 水面から出た時の振動
				}
			}
			else
			{// 水の中
				g_aPlayer[nCntPlayer].bJump = false;
			}

			//PrintDebugProc("fAngle : %f", pCamera->fAngle);

			// 墨の当たり判定
			CollisionInk(g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].bBlind, &g_aPlayer[nCntPlayer].nBlindCounter, g_aPlayer[nCntPlayer].nIdx + 100);

			g_aPlayer[nCntPlayer].fFogStart = (g_aPlayer[nCntPlayer].pos.y * 0.4f + (-pCamera->fAngle * 0.2f)) + FOGS_MIN;

			if (g_aPlayer[nCntPlayer].fFogStart < FOGS_MIN)
			{// フォグの最低値
				g_aPlayer[nCntPlayer].fFogStart = FOGS_MIN;
			}
			else if (g_aPlayer[nCntPlayer].fFogStart > FOGS_MAX)
			{// フォグの最高値
				g_aPlayer[nCntPlayer].fFogStart = FOGS_MAX;
			}

			g_aPlayer[nCntPlayer].fFogEnd = (g_aPlayer[nCntPlayer].pos.y * 1.1f + (-pCamera->fAngle * 0.2f)) + FOGE_MIN;

			if (g_aPlayer[nCntPlayer].fFogEnd < FOGE_MIN)
			{// フォグの最低値
				g_aPlayer[nCntPlayer].fFogEnd = FOGE_MIN;
			}
			else if (g_aPlayer[nCntPlayer].fFogEnd > FOGE_MAX)
			{// フォグの最高値
				g_aPlayer[nCntPlayer].fFogEnd = FOGE_MAX;
			}

			if (g_aPlayer[nCntPlayer].bBlind == true)
			{// 視界悪化中
				g_aPlayer[nCntPlayer].fFogStart *= 0.5f;
				g_aPlayer[nCntPlayer].fFogEnd *= 0.5f;

				D3DXVECTOR3 headPos = D3DXVECTOR3(
					g_aPlayer[nCntPlayer].aModel[0].mtxWorld._41,
					g_aPlayer[nCntPlayer].aModel[0].mtxWorld._42 + 10.0f,
					g_aPlayer[nCntPlayer].aModel[0].mtxWorld._43);

				SetEffect3D(1, headPos, FIRST_POS, 0.0f, 15.0f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), EFFECTTYPE_OCTOINK);
			}

			fmoveAngle = g_aPlayer[nCntPlayer].fAngleY - g_aPlayer[nCntPlayer].rot.y;

			// 向きを調整
			CorrectAngle(&g_aPlayer[nCntPlayer].fAngleY, fmoveAngle);

			if (g_aPlayer[nCntPlayer].rot.y != g_aPlayer[nCntPlayer].fAngleY)
			{// 目標地点につくまで慣性で角度を足す
				g_aPlayer[nCntPlayer].rot.y += (g_aPlayer[nCntPlayer].fAngleY - g_aPlayer[nCntPlayer].rot.y) * INERTIA_ANGLE;

				// 向きを調整
				CorrectAngle(&g_aPlayer[nCntPlayer].rot.y, g_aPlayer[nCntPlayer].rot.y);
			}

			// 向きを調整
			CorrectAngle(&g_aPlayer[nCntPlayer].fAngleX, g_aPlayer[nCntPlayer].fAngleX - g_aPlayer[nCntPlayer].rot.x);

			if (g_aPlayer[nCntPlayer].rot.x != g_aPlayer[nCntPlayer].fAngleX)
			{// 目標地点につくまで慣性で角度を足す
				g_aPlayer[nCntPlayer].rot.x += (g_aPlayer[nCntPlayer].fAngleX - g_aPlayer[nCntPlayer].rot.x) * INERTIA_ANGLE;

				// 向きを調整
				CorrectAngle(&g_aPlayer[nCntPlayer].rot.x, g_aPlayer[nCntPlayer].rot.x);
			}

			if ((g_aPlayer[nCntPlayer].mode != PLAYERMODE_TUTORIAL || g_aPlayer[nCntPlayer].state != PLAYERSTATE_APPEAR) &&
				GetGameState() != GAMESTATE_LITTLETIME)
			{// チュートリアルモードじゃないとき
				if (g_aPlayer[nCntPlayer].nCounter % (ONE_SECOND * 50) == 0 && GetTime() != ONE_GAME && g_aPlayer[nCntPlayer].nCounter != 0)
				{// 持てるエサの最大値が増える
					g_aPlayer[nCntPlayer].nMaxFood++;
					PlaySound(SOUND_SE_FOODNUMUP);

					if (GetNumCamera() == 1 && nCntPlayer == 0)
					{// 1人プレイの1P
						SetFoodNum(FOODNUMTYPE_UP, FOODNUMSTATE_MOVE,
							D3DXVECTOR3(640.0f, 800.0f, 0.0f), 100.0f, 50.0f);
					}
					else if (GetNumCamera() == 2 && nCntPlayer == 0)
					{// 2人プレイの1P
						SetFoodNum(FOODNUMTYPE_UP, FOODNUMSTATE_MOVE,
							D3DXVECTOR3(320.0f, 800.0f, 0.0f), 100.0f, 50.0f);
					}
					else if (GetNumCamera() == 2 && nCntPlayer == 1)
					{// 2人プレイの2P
						SetFoodNum(FOODNUMTYPE_UP, FOODNUMSTATE_MOVE,
							D3DXVECTOR3(960.0f, 800.0f, 0.0f), 100.0f, 50.0f);
					}
				}
			}

			if (g_aPlayer[nCntPlayer].nFood >= g_aPlayer[nCntPlayer].nMaxFood * PLAYER_TENTACLE)
			{// 持てる数が満杯だったら
				if (g_aPlayer[nCntPlayer].nFoodNumIdx == -1)
				{// 設定してないとき
					if (GetNumCamera() == 1 && nCntPlayer == 0)
					{// 1人プレイの1P
						g_aPlayer[nCntPlayer].nFoodNumIdx = SetFoodNum(FOODNUMTYPE_FULL, FOODNUMSTATE_DISP,
							D3DXVECTOR3(640.0f, 620.0f, 0.0f), 100.0f, 50.0f);
					}
					else if (GetNumCamera() == 2 && nCntPlayer == 0)
					{// 2人プレイの1P
						g_aPlayer[nCntPlayer].nFoodNumIdx = SetFoodNum(FOODNUMTYPE_FULL, FOODNUMSTATE_DISP,
							D3DXVECTOR3(320.0f, 620.0f, 0.0f), 100.0f, 50.0f);
					}
					else if (GetNumCamera() == 2 && nCntPlayer == 1)
					{// 2人プレイの2P
						g_aPlayer[nCntPlayer].nFoodNumIdx = SetFoodNum(FOODNUMTYPE_FULL, FOODNUMSTATE_DISP,
							D3DXVECTOR3(960.0f, 620.0f, 0.0f), 100.0f, 50.0f);
					}
				}
			}
			else
			{// まだ持てる場合
				if (g_aPlayer[nCntPlayer].nFoodNumIdx != -1)
				{// インデックスが存在する
					SetFoodNumState(g_aPlayer[nCntPlayer].nFoodNumIdx, FOODNUMSTATE_NONE);
					g_aPlayer[nCntPlayer].nFoodNumIdx = -1;
				}
			}

			D3DXVECTOR3 dist;
			dist = g_aPlayer[nCntPlayer].posX - g_aPlayer[nCntPlayer].pos;
			D3DXVec3Normalize(&dist, &dist);
			dist *= TENTACLE_REACH;
			dist += g_aPlayer[nCntPlayer].pos;

			if (CollisionMeshCylinder(&dist, &g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].move, 0.0f, 0.0f, true) == true ||
				dist.y < 0.0f ||
				CollisionObject(&dist, &g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].move, 0.0f, 0.0f, true) == true)
			{// 壁に当たった・オブジェクトに当たった・エサに当たった
				// クロスヘアの設定
				SetCrossHair(nCntPlayer, CROSSHAIRSTATE_REACH);
			}
			else
			{// 何にも当たっていない
				SetCrossHair(nCntPlayer, CROSSHAIRSTATE_NONE);
			}

			if (((nCntPlayer == 0 ? GetKeyboardPress(DIK_E) == true : (GetKeyboardPress(DIK_END) == true || GetKeyboardPress(DIK_NUMPAD1) == true)) ||
				GetJoypadShoulder(nCntPlayer, JOYKEY_RIGHTTRIGGER, &nValue) == true)
				&& g_aPlayer[nCntPlayer].TentacleState != PLTENTACLESTATE_TENTACLELONG && g_aPlayer[nCntPlayer].state != PLAYERSTATE_DASH &&
				g_aPlayer[nCntPlayer].state != PLAYERSTATE_APPEAR &&
				g_aPlayer[nCntPlayer].nTentacleCooldown == 0 && g_aPlayer[nCntPlayer].pos.y < *GetWaterSurf_Height())
			{// 触手伸ばしアクション
				g_aPlayer[nCntPlayer].TentacleState = PLTENTACLESTATE_TENTACLELONG;

				g_aPlayer[nCntPlayer].fAngleY = D3DX_PI + pCamera->rot.y;
				CorrectAngle(&g_aPlayer[nCntPlayer].fAngleY, g_aPlayer[nCntPlayer].fAngleY);

				D3DXVECTOR3 dir, rot;
				dir = g_aPlayer[nCntPlayer].posX - g_aPlayer[nCntPlayer].pos;
				D3DXVec3Normalize(&dir, &dir);

				//rot.y = D3DX_PI + atan2f(dir.x, dir.z);
				rot.x = (dir.y / 0.95f) * 1.2f;
				//rot.z = 0.0f;

				g_aPlayer[nCntPlayer].fAngleX = rot.x;
				//g_aPlayer[nCntPlayer].rot.y = rot.y;
				CorrectAngle(&g_aPlayer[nCntPlayer].fAngleX, g_aPlayer[nCntPlayer].fAngleX);
				//CorrectAngle(&g_aPlayer[nCntPlayer].rot.y, g_aPlayer[nCntPlayer].rot.y);

				g_aPlayer[nCntPlayer].vecX = (pCamera->posR - pCamera->posV) * DASH_RATE;

				SetMotionPlayer(nCntPlayer, MOTIONTYPE_TENTACLELONG, true, 20);

				// クールダウンを設定
				g_aPlayer[nCntPlayer].nTentacleCooldown = TENTACLE_CT;

				PlaySound(SOUND_SE_TENTACLE_STRETCH);

				if (g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
				{// チュートリアルモード
					SetTutorialTxtState(TUTTXTTYPE_TENT, TUTTXTSTATE_CLEAR);
				}
			}

			if (((nCntPlayer == 0 ? GetKeyboardPress(DIK_Q) == true : GetKeyboardPress(DIK_RSHIFT) == true) ||
				/*GetJoypadShoulder(nCntPlayer, JOYKEY_LEFTTRIGGER, &nValue) == true*/
				GetJoypadPress(nCntPlayer, JOYKEY_RIGHT_SHOULDER) == true) &&
				g_aPlayer[nCntPlayer].state != PLAYERSTATE_INK && g_aPlayer[nCntPlayer].state != PLAYERSTATE_APPEAR && 
				g_aPlayer[nCntPlayer].nInkCooldown == 0 &&
				g_aPlayer[nCntPlayer].pos.y < *GetWaterSurf_Height())
			{// 墨吐きアクション
				g_aPlayer[nCntPlayer].state = PLAYERSTATE_INK;

				SetMotionPlayer(nCntPlayer, MOTIONTYPE_INK, true, 20);

				SetParticle3D(14, 30, g_aPlayer[nCntPlayer].pos, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), D3DXVECTOR3(g_aPlayer[nCntPlayer].rot.x, g_aPlayer[nCntPlayer].rot.y - D3DX_PI, g_aPlayer[nCntPlayer].rot.z), 4.0f, 420, 8.0f, 0.06f, EFFECTTYPE_OCTOINK, g_aPlayer[nCntPlayer].nIdx + 100);
				SetParticle3D(14, 30, g_aPlayer[nCntPlayer].pos, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), D3DXVECTOR3(g_aPlayer[nCntPlayer].rot.x, g_aPlayer[nCntPlayer].rot.y - D3DX_PI, g_aPlayer[nCntPlayer].rot.z), 4.0f, 420, 8.0f, 0.06f, EFFECTTYPE_OCTOINK, g_aPlayer[nCntPlayer].nIdx + 100);
				SetParticle3D(14, 30, g_aPlayer[nCntPlayer].pos, D3DXCOLOR(0.0f, 0.0f, 0.1f, 1.0f), D3DXVECTOR3(g_aPlayer[nCntPlayer].rot.x, g_aPlayer[nCntPlayer].rot.y - D3DX_PI, g_aPlayer[nCntPlayer].rot.z), 4.0f, 420, 8.0f, 0.06f, EFFECTTYPE_OCTOINK, g_aPlayer[nCntPlayer].nIdx + 100);

				// クールダウンを設定
				g_aPlayer[nCntPlayer].nInkCooldown = INK_CT;

				PlaySound(SOUND_SE_MUD);

				if (g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL)
				{// チュートリアルモード
					SetTutorialTxtState(TUTTXTTYPE_INK, TUTTXTSTATE_CLEAR);
				}
			}

			if (g_aPlayer[nCntPlayer].state == PLAYERSTATE_INK && g_aPlayer[nCntPlayer].bFinishMotion == true)
			{// 墨吐きモーションが終わったら
				g_aPlayer[nCntPlayer].state = PLAYERSTATE_WAIT;
			}

			// 当たり判定
			CollisionMeshField(g_aPlayer[nCntPlayer].pos, g_aPlayer[nCntPlayer].rot, g_aPlayer[nCntPlayer].fRadius, g_aPlayer[nCntPlayer].fHeight);
			CollisionSeaweed(g_aPlayer[nCntPlayer].pos);
			CollisionPot(&g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].posOld, &g_aPlayer[nCntPlayer].move, g_aPlayer[nCntPlayer].fRadius, g_aPlayer[nCntPlayer].fHeight);
			CollisionObject(&g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].posOld, &g_aPlayer[nCntPlayer].move, g_aPlayer[nCntPlayer].fRadius, g_aPlayer[nCntPlayer].fHeight, false);
			CollisionMeshCylinder(&g_aPlayer[nCntPlayer].pos, &g_aPlayer[nCntPlayer].posOld, &g_aPlayer[nCntPlayer].move, g_aPlayer[nCntPlayer].fRadius, g_aPlayer[nCntPlayer].fHeight, false);

			int nIdx = -1;

			if (CollisionEsa(&nIdx, false, &g_aPlayer[nCntPlayer].pos, g_aPlayer[nCntPlayer].fRadius) == true &&
				g_aPlayer[nCntPlayer].nFood < g_aPlayer[nCntPlayer].nMaxFood * PLAYER_TENTACLE &&
				(g_aPlayer[nCntPlayer].motionType != MOTIONTYPE_OCEANCULLENT || g_aPlayer[nCntPlayer].motionType != MOTIONTYPE_OCEANCULLENT) &&
				GetOceanCurrents() != OCEANCURRENTSSTATE_WIRLPOOL && 
				g_aPlayer[nCntPlayer].state != PLAYERSTATE_APPEAR)
			{// エサと接触した
				Esa* pEsa = GetEsa();

				if (pEsa[nIdx].esaType != ESA_ACTTYPE_GOTO_POT)
				{// タコつぼに入れてる最中じゃない

					// エサの削除処理
					int nIdxEsaType = DelEsa(nIdx, true, nCntPlayer);	// 削除したエサの種類を獲得

					if (nIdxEsaType != -1)
					{
						g_aPlayer[nCntPlayer].nFood++;
						Enqueue(&g_aPlayer[nCntPlayer].esaQueue, nIdxEsaType);
						EsaPlaySE(nIdxEsaType);
					}
				}
			}

			if (g_aPlayer[nCntPlayer].mode == PLAYERMODE_TUTORIAL && g_aPlayer[nCntPlayer].nFood > 0)
			{// チュートリアルモード
				SetTutorialTxtState(TUTTXTTYPE_ESA, TUTTXTSTATE_CLEAR);
			}

			if (g_aPlayer[nCntPlayer].nFood < 0)
			{// 最小値0
				g_aPlayer[nCntPlayer].nFood = 0;
			}

			CollisionPotArea(g_aPlayer[nCntPlayer].pos, g_aPlayer[nCntPlayer].fRadius, pPlayer, NULL, false);

			g_aPlayer[nCntPlayer].nCounter++;

		}
	}

	// モーションの更新処理
	UpdateMotionPlayer();
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
	Model_Info* pTakoModel = GetTakoModel();

	for (int nCntPlayer = 0; nCntPlayer < GetNumCamera(); nCntPlayer++, pPlayer++)
	{
		if (g_aPlayer[nCntPlayer].bUse == true)
		{// 使用しているとき

			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aPlayer[nCntPlayer].mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aPlayer[nCntPlayer].rot.y, g_aPlayer[nCntPlayer].rot.x, g_aPlayer[nCntPlayer].rot.z);
			D3DXMatrixMultiply(&g_aPlayer[nCntPlayer].mtxWorld, &g_aPlayer[nCntPlayer].mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aPlayer[nCntPlayer].pos.x, g_aPlayer[nCntPlayer].pos.y, g_aPlayer[nCntPlayer].pos.z);
			D3DXMatrixMultiply(&g_aPlayer[nCntPlayer].mtxWorld, &g_aPlayer[nCntPlayer].mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aPlayer[nCntPlayer].mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// 全モデル(パーツ)の描画
			for (int nCntModel = 0; nCntModel < g_aPlayer[nCntPlayer].nNumModel; nCntModel++)
			{
				D3DXMATRIX mtxRotModel, mtxTransModel, mtxScaleModel;		// 計算用マトリックス
				D3DXMATRIX mtxParent;						// 親のマトリックス

				// パーツのワールドマトリックスの初期化
				D3DXMatrixIdentity(&g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotModel, g_aPlayer[nCntPlayer].aModel[nCntModel].rot.y, g_aPlayer[nCntPlayer].aModel[nCntModel].rot.x, g_aPlayer[nCntPlayer].aModel[nCntModel].rot.z);
				D3DXMatrixMultiply(&g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld, &g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld, &mtxRotModel);

				// 拡大率を反映
				D3DXMatrixScaling(&mtxScaleModel, g_aPlayer[nCntPlayer].aModel[nCntModel].scale.x, g_aPlayer[nCntPlayer].aModel[nCntModel].scale.y, g_aPlayer[nCntPlayer].aModel[nCntModel].scale.z);
				D3DXMatrixMultiply(&g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld, &g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld, &mtxScaleModel);

				// パーツの位置を反映
				D3DXMatrixTranslation(&mtxTransModel, g_aPlayer[nCntPlayer].aModel[nCntModel].pos.x, g_aPlayer[nCntPlayer].aModel[nCntModel].pos.y, g_aPlayer[nCntPlayer].aModel[nCntModel].pos.z);
				D3DXMatrixMultiply(&g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld, &g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld, &mtxTransModel);

				// パーツの「親のマトリックス」を設定
				if (g_aPlayer[nCntPlayer].aModel[nCntModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = g_aPlayer[nCntPlayer].aModel[g_aPlayer[nCntPlayer].aModel[nCntModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = g_aPlayer[nCntPlayer].mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」をかけ合わせる
				D3DXMatrixMultiply(&g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld,
					&g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &g_aPlayer[nCntPlayer].aModel[nCntModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pTakoModel[nCntModel].pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pTakoModel[nCntModel].dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pTakoModel[nCntModel].apTexture[nCntMat]);

					// モデルパーツの描画
					pTakoModel[nCntModel].pMesh->DrawSubset(nCntMat);
				}
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);

			for (int nCntTent = 0; nCntTent < PLAYER_TENTACLE; nCntTent++)
			{
				if (g_aPlayer[nCntPlayer].nOrbitIdx[nCntTent] == -1)
				{// 初回
					g_aPlayer[nCntPlayer].nOrbitIdx[nCntTent] = SetMeshOrbit(D3DXVECTOR3(g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.x, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.y, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.z),
						D3DXVECTOR3(g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.x, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.y + 5.5f, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.z),
						D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f), &g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].mtxWorld);
				}

				SetMeshOrbitPos(g_aPlayer[nCntPlayer].nOrbitIdx[nCntTent], D3DXVECTOR3(g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.x, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.y, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.z),
					D3DXVECTOR3(g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.x, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.y + 5.5f, g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].posOff.z),
					D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f), D3DXCOLOR(0.0f, 1.0f, 1.0f, 0.5f), &g_aPlayer[nCntPlayer].aModel[(nCntTent + 1) * 4].mtxWorld);
			}
		}
	}
}

//=============================================================================
// プレイヤーの設定処理
//=============================================================================
void SetPlayer(int nIdx, D3DXVECTOR3 pos, D3DXVECTOR3 rot, MOTIONTYPE MotionType, PLAYERMODE mode, PLAYERSTATE state)
{
	g_aPlayer[nIdx].nIdx = nIdx;
	g_aPlayer[nIdx].nCounter = 0;

	if (pos == FIRST_POS)
	{// 初期座標の場合はランダムで決める
		D3DXVECTOR3 posRand;
		float fAngle = (D3DX_PI * 2.0f) * ((float)((nIdx) * (360.0f / GetNumCamera())) / 360.0f);

		posRand.x = sinf(fAngle) * ((INCYLINDER_RADIUS * 1.5f) + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - (INCYLINDER_RADIUS * 1.5f)) + 1))));
		posRand.y = (float)(rand() % (int)(CYLINDER_HEIGHT * 0.6f)) + (CYLINDER_HEIGHT * 0.2f);
		posRand.z = cosf(fAngle) * ((INCYLINDER_RADIUS * 1.5f) + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - (INCYLINDER_RADIUS * 1.5f)) + 1))));

		g_aPlayer[nIdx].pos = posRand;
		g_aPlayer[nIdx].posOld = posRand;
		g_aPlayer[nIdx].rot = D3DXVECTOR3(0.0f, fAngle, 0.0f);
		g_aPlayer[nIdx].fAngleY = fAngle;

		SetCameraPos(nIdx, FIRST_POS, FIRST_POS, D3DXVECTOR3(0.0f, fAngle + D3DX_PI, 0.0f), CAMERATYPE_PLAYER);
	}
	else
	{// マップ内
		g_aPlayer[nIdx].pos = pos;
		g_aPlayer[nIdx].posOld = pos;
		g_aPlayer[nIdx].rot = rot;
		g_aPlayer[nIdx].fAngleY = rot.y;
	}

	g_aPlayer[nIdx].state = state;
	g_aPlayer[nIdx].mode = mode;
	g_aPlayer[nIdx].TentacleState = PLTENTACLESTATE_NORMAL;
	g_aPlayer[nIdx].nCounterState = 0;
	g_aPlayer[nIdx].fFogStart = (g_aPlayer[nIdx].pos.y * 0.4f) + FOGS_MIN;
	g_aPlayer[nIdx].fFogEnd = (g_aPlayer[nIdx].pos.y * 1.1f) + FOGE_MIN;
	g_aPlayer[nIdx].bJump = true;
	g_aPlayer[nIdx].bLand = false;
	g_aPlayer[nIdx].bAct = false;
	g_aPlayer[nIdx].bMove = false;
	g_aPlayer[nIdx].bUse = true;
	g_aPlayer[nIdx].bBlind = false;
	g_aPlayer[nIdx].nBlindCounter = 0;
	g_aPlayer[nIdx].nFoodNumIdx = -1;
	g_aPlayer[nIdx].nFood = 0;
	memset(&g_aPlayer[nIdx].esaQueue.nData, -1, sizeof(int[MAX_QUEUE]));
	g_aPlayer[nIdx].Potstate = POTSTATE_NONE;
	g_aPlayer[nIdx].nPotIdx = -1;
	g_aPlayer[nIdx].nMaxFood = 1;
	g_aPlayer[nIdx].nTentacleCooldown = 0;
	g_aPlayer[nIdx].nInkCooldown = 0;


	g_aPlayer[nIdx].motionType = MOTIONTYPE_NEUTRAL;
	g_aPlayer[nIdx].bLoopMotion = g_aPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].bLoop;
	g_aPlayer[nIdx].nNumKey = g_aPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].nNumKey;
	g_aPlayer[nIdx].nKey = 0;
	g_aPlayer[nIdx].nCounterMotion = 0;
	g_aPlayer[nIdx].bFinishMotion = false;
	g_aPlayer[nIdx].bBlendMotion = false;
	g_aPlayer[nIdx].motionTypeBlend = MOTIONTYPE_NEUTRAL;
	g_aPlayer[nIdx].bLoopMotionBlend = g_aPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].bLoop;
	g_aPlayer[nIdx].nNumKeyBlend = g_aPlayer[nIdx].aMotionInfo[MOTIONTYPE_NEUTRAL].nNumKey;
	g_aPlayer[nIdx].nKeyBlend = 0;
	g_aPlayer[nIdx].nCounterMotionBlend = 0;
	g_aPlayer[nIdx].nFrameBlend = 0;
	g_aPlayer[nIdx].nCounterBlend = 0;

	SetMotionPlayer(nIdx, MotionType, false, 0);

	// 泡の設定
	SetBubbleParticle(&g_aPlayer[nIdx].pos, true, -1, 30, 1, 30, 5.0f, 3.0f);
}

//=============================================================================
// プレイヤーのランダム設定処理
//=============================================================================
void SetRandomPlayer(int nAmount)
{
	for (int nCntPlayer = 0; nCntPlayer < nAmount; nCntPlayer++)
	{
		D3DXVECTOR3 pos;
		float fAngle = (D3DX_PI * 2.0f) * ((float)((nCntPlayer) * (360.0f / nAmount)) / 360.0f);
		//float fsin = sinf(fAngle);

		pos.x = sinf(fAngle) * ((INCYLINDER_RADIUS * 1.5f) + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - (INCYLINDER_RADIUS * 1.5f)) + 1))));
		pos.y = (float)(rand() % (int)(CYLINDER_HEIGHT * 0.6f)) + (CYLINDER_HEIGHT * 0.2f);
		pos.z = cosf(fAngle) * ((INCYLINDER_RADIUS * 1.5f) + (((float)(rand() % (int)(OUTCYLINDER_RADIUS - (INCYLINDER_RADIUS * 1.5f)) + 1))));

		SetPlayer(nCntPlayer, pos, D3DXVECTOR3(0.0f, fAngle, 0.0f), MOTIONTYPE_NEUTRAL, PLAYERMODE_GAME, PLAYERSTATE_APPEAR);
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
	char aString[512] = {};				// ファイルのテキスト読み込み
	char aTrash[512] = {};				// ごみ箱
	char aModelName[128][512] = {};		// モデルの名前

	// テクスチャ読み込み用の変数
	int nNumTexture = 0;
	char aTextureName[64][256] = {};		// テクスチャの名前

	// キャラクターセット用の変数
	int nNumParts = 0;		// 読み込むパーツ数
	//float fRadius = 0.0f;	// キャラクターの半径
	//float fHeight = 0.0f;	// キャラクターの高さ
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
				fscanf(pFile, " = %d", &g_aPlayer[0].nNumModel);

				continue;
			}

			if (strcmp(&aString[0], "MODEL_FILENAME") == 0)
			{// モデルの名前読み込み
				for (int nCntModel = 0; nCntModel < g_aPlayer[0].nNumModel; nCntModel++)
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
					fscanf(pFile, " = %f", &g_aPlayer[0].fRadius);

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
					fscanf(pFile, " = %f", &g_aPlayer[0].fHeight);

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
					g_aPlayer[0].aModel[nCntParts].nIdx = nIdx;
					g_aPlayer[0].aModel[nCntParts].nIdxModelParent = nIdxParent;
					g_aPlayer[0].aModel[nCntParts].pos = pos;
					g_aPlayer[0].aModel[nCntParts].rot = rot;
					g_aPlayer[0].aModel[nCntParts].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
					g_aPlayer[0].aModel[nCntParts].posOff = pos;
					g_aPlayer[0].aModel[nCntParts].rotOff = rot;
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
						g_aPlayer[0].aMotionInfo[nCntMotion].bLoop = false;
					}
					else if (nLoop == 1)
					{// true
						g_aPlayer[0].aMotionInfo[nCntMotion].bLoop = true;
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
					g_aPlayer[0].aMotionInfo[nCntMotion].nNumKey = nNumKey;

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

						g_aPlayer[0].aMotionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame = nFrame;

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
						g_aPlayer[0].aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPartsKey] = key;
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
		g_aPlayer[0].nNumMotion = nCntMotion + 1;

		if (GetNumCamera() > 1)
		{// 1人プレイじゃない場合
			g_aPlayer[1] = g_aPlayer[0];
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

	for (int nCntPlayer = 0; nCntPlayer < GetNumCamera(); nCntPlayer++, pPlayer++)
	{
		// 全モデル(パーツ)の更新
		for (int nCntModel = 0; nCntModel < g_aPlayer[nCntPlayer].nNumModel; nCntModel++)
		{
			// ローカル変数宣言
			KEY* pKey;
			KEY* pKeyNext;
			Model* pModel;
			D3DXVECTOR3 posCurrent, rotCurrent;
			D3DXVECTOR3 posDest, rotDest;
			float fDiffKey, fRateKey;

			// 1フレームあたりに動く割合
			fRateKey = ((float)g_aPlayer[nCntPlayer].nCounterMotion / (float)g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].aKeyInfo[g_aPlayer[nCntPlayer].nKey].nFrame);

			// 次のキー
			if (g_aPlayer[nCntPlayer].bLoopMotion == true)
			{// ループモーション
				nNextKey = (g_aPlayer[nCntPlayer].nKey + 1) % (g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey);
			}
			else
			{// ループしないモーション
				nNextKey = g_aPlayer[nCntPlayer].nKey + 1;

				if (nNextKey > g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey - 1)
				{// キーの総数を超えた
					nNextKey = g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey - 1;
				}
			}

			// 現在のキーのポインタ
			pKey = &g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].aKeyInfo[g_aPlayer[nCntPlayer].nKey].aKey[nCntModel];

			// 次のキーのポインタ
			pKeyNext = &g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].aKeyInfo[nNextKey].aKey[nCntModel];

			// モデルのポインタ
			pModel = &g_aPlayer[nCntPlayer].aModel[nCntModel];

			if (g_aPlayer[nCntPlayer].bBlendMotion == true)
			{// ブレンドあり
				// ローカル変数宣言
				KEY* pKeyBlend;
				KEY* pKeyNextBlend;
				D3DXVECTOR3 posBlend, rotBlend;
				float fDiffKeyBlend, fDiffBlend, fRateKeyBlend, fRateBlend;
				int nNextKeyBlend;

				// 1フレームあたりに動く割合
				fRateKeyBlend = ((float)g_aPlayer[nCntPlayer].nCounterMotionBlend / (float)g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].aKeyInfo[g_aPlayer[nCntPlayer].nKeyBlend].nFrame);

				// ブレンドの相対値
				fRateBlend = (float)g_aPlayer[nCntPlayer].nCounterBlend / (float)g_aPlayer[nCntPlayer].nFrameBlend;

				// 次のキー
				if (g_aPlayer[nCntPlayer].bLoopMotionBlend == true)
				{// ループモーション
					nNextKeyBlend = (g_aPlayer[nCntPlayer].nKeyBlend + 1) % (g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					nNextKeyBlend = g_aPlayer[nCntPlayer].nKeyBlend + 1;

					if (nNextKeyBlend > g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						nNextKeyBlend = g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].nNumKey - 1;
					}
				}

				// 現在のキーのポインタ
				pKeyBlend = &g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].aKeyInfo[g_aPlayer[nCntPlayer].nKeyBlend].aKey[nCntModel];

				// 次のキーのポインタ
				pKeyNextBlend = &g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].aKeyInfo[nNextKeyBlend].aKey[nCntModel];

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
			pModel->pos = posDest + g_aPlayer[nCntPlayer].aModel[nCntModel].posOff;
			pModel->rot = rotDest + g_aPlayer[nCntPlayer].aModel[nCntModel].rotOff;
		}

		if (g_aPlayer[nCntPlayer].bBlendMotion == true)
		{// ブレンドあり
			g_aPlayer[nCntPlayer].nCounterMotion++;

			if (g_aPlayer[nCntPlayer].nCounterMotion >= g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].aKeyInfo[g_aPlayer[nCntPlayer].nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				g_aPlayer[nCntPlayer].nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (g_aPlayer[nCntPlayer].bLoopMotion == true)
				{// ループモーション
					g_aPlayer[nCntPlayer].nKey = (g_aPlayer[nCntPlayer].nKey + 1) % (g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey);
				}
				else
				{// ループしないモーション
					g_aPlayer[nCntPlayer].nKey++;

					if (g_aPlayer[nCntPlayer].nKey >= g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey - 1)
					{// キーの総数を超えた
						g_aPlayer[nCntPlayer].nKey = g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey - 1;

						g_aPlayer[nCntPlayer].bFinishMotion = true;
					}
				}
			}

			g_aPlayer[nCntPlayer].nCounterMotionBlend++;

			if (g_aPlayer[nCntPlayer].nCounterMotionBlend >= g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].aKeyInfo[g_aPlayer[nCntPlayer].nKeyBlend].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				g_aPlayer[nCntPlayer].nCounterMotionBlend = 0;	// カウンターを戻す

				// ループかどうか
				if (g_aPlayer[nCntPlayer].bLoopMotionBlend == true)
				{// ループモーション
					g_aPlayer[nCntPlayer].nKeyBlend = (g_aPlayer[nCntPlayer].nKeyBlend + 1) % (g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].nNumKey);
				}
				else
				{// ループしないモーション
					g_aPlayer[nCntPlayer].nKeyBlend++;

					if (g_aPlayer[nCntPlayer].nKeyBlend >= g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].nNumKey - 1)
					{// キーの総数を超えた
						g_aPlayer[nCntPlayer].nKeyBlend = g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionTypeBlend].nNumKey - 1;

						g_aPlayer[nCntPlayer].bFinishMotion = true;
					}
				}
			}

			g_aPlayer[nCntPlayer].nCounterBlend++;

			if (g_aPlayer[nCntPlayer].nCounterBlend >= g_aPlayer[nCntPlayer].nFrameBlend)
			{// ブレンドフレームに到達
				// ブレンドモーションを現在のモーションに設定
				g_aPlayer[nCntPlayer].motionType = g_aPlayer[nCntPlayer].motionTypeBlend;
				g_aPlayer[nCntPlayer].bLoopMotion = g_aPlayer[nCntPlayer].bLoopMotionBlend;
				g_aPlayer[nCntPlayer].nNumKey = g_aPlayer[nCntPlayer].nNumKeyBlend;
				g_aPlayer[nCntPlayer].nKey = g_aPlayer[nCntPlayer].nKeyBlend;
				g_aPlayer[nCntPlayer].nCounterMotion = g_aPlayer[nCntPlayer].nCounterMotionBlend;
				//g_aPlayer[nCntPlayer].bFinishMotion = true;

				g_aPlayer[nCntPlayer].nKeyBlend = 0;
				g_aPlayer[nCntPlayer].nCounterMotionBlend = 0;
				g_aPlayer[nCntPlayer].bBlendMotion = false;
				g_aPlayer[nCntPlayer].nCounterBlend = 0;
				g_aPlayer[nCntPlayer].nFrameBlend = 0;
			}
		}
		else
		{// ブレンドなし
			g_aPlayer[nCntPlayer].nCounterMotion++;

			if (g_aPlayer[nCntPlayer].nCounterMotion >= g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].aKeyInfo[g_aPlayer[nCntPlayer].nKey].nFrame)
			{// 再生フレーム数に達したら現在のキーを1つ進める
				g_aPlayer[nCntPlayer].nCounterMotion = 0;	// カウンターを戻す

				// ループかどうか
				if (g_aPlayer[nCntPlayer].bLoopMotion == true)
				{// ループモーション
					g_aPlayer[nCntPlayer].nKey = (g_aPlayer[nCntPlayer].nKey + 1) % (g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey);
				}
				else
				{// ループしないモーション
					g_aPlayer[nCntPlayer].nKey++;

					if (g_aPlayer[nCntPlayer].nKey >= g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey - 1)
					{// キーの総数を超えた
						g_aPlayer[nCntPlayer].nKey = g_aPlayer[nCntPlayer].aMotionInfo[g_aPlayer[nCntPlayer].motionType].nNumKey - 1;

						g_aPlayer[nCntPlayer].bFinishMotion = true;
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
	if (g_aPlayer[nIdx].motionTypeBlend != motionType)
	{// 違うモーションが設定されたときだけ
		if (bBlendMotion == true)
		{// ブレンドあり
			g_aPlayer[nIdx].motionTypeBlend = motionType;
			g_aPlayer[nIdx].bLoopMotionBlend = g_aPlayer[nIdx].aMotionInfo[motionType].bLoop;
			g_aPlayer[nIdx].nNumKeyBlend = g_aPlayer[nIdx].aMotionInfo[motionType].nNumKey;
			g_aPlayer[nIdx].nKeyBlend = 0;
			g_aPlayer[nIdx].nCounterMotionBlend = 0;
			g_aPlayer[nIdx].bFinishMotion = false;

			g_aPlayer[nIdx].bBlendMotion = bBlendMotion;
			g_aPlayer[nIdx].nFrameBlend = nFrameBlend;
			g_aPlayer[nIdx].nCounterBlend = 0;
		}
		else
		{// ブレンドなし
			g_aPlayer[nIdx].motionType = motionType;
			g_aPlayer[nIdx].bLoopMotion = g_aPlayer[nIdx].aMotionInfo[motionType].bLoop;
			g_aPlayer[nIdx].nNumKey = g_aPlayer[nIdx].aMotionInfo[motionType].nNumKey;
			g_aPlayer[nIdx].nKey = 0;
			g_aPlayer[nIdx].nCounterMotion = 0;
			g_aPlayer[nIdx].bFinishMotion = false;

			g_aPlayer[nIdx].bBlendMotion = bBlendMotion;

			// 全モデル(パーツ)の初期設定
			for (int nCntModel = 0; nCntModel < g_aPlayer[nIdx].nNumModel; nCntModel++)
			{
				g_aPlayer[nIdx].aModel[nCntModel].pos.x = g_aPlayer[nIdx].aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosX;
				g_aPlayer[nIdx].aModel[nCntModel].pos.y = g_aPlayer[nIdx].aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosY;
				g_aPlayer[nIdx].aModel[nCntModel].pos.z = g_aPlayer[nIdx].aMotionInfo[motionType].aKeyInfo[0].aKey[0].fPosZ;

				g_aPlayer[nIdx].aModel[nCntModel].rot.x = g_aPlayer[nIdx].aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotX;
				g_aPlayer[nIdx].aModel[nCntModel].rot.y = g_aPlayer[nIdx].aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotY;
				g_aPlayer[nIdx].aModel[nCntModel].rot.z = g_aPlayer[nIdx].aMotionInfo[motionType].aKeyInfo[0].aKey[0].fRotZ;
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