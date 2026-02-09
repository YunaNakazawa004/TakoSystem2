//=============================================================================
// 
// カメラ [camera.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CAMERA_POS				(D3DXVECTOR3(0.0f, 100.0f, -200.0f))	// 初期座標
#define FIRST_ROT_X				(200.0f)								// 視点と注視点間の距離
#define HEIGHT					(100.0f)								// 視点の高さ
#define MOVEMENT				(D3DXVECTOR3(5.0f, 5.0f, 5.0f))			// 移動量
#define ROT						(D3DXVECTOR3(0.05f, 0.05f, 0.05f))		// 向き移動量
#define AUTO_ROT				(D3DXVECTOR3(0.005f, 0.005f, 0.005f))	// 自動回転移動量
#define DEFAULT_VIEW_ANGLE		(45.0f)									// 規定の視野角
#define INERTIA_VIEW_ANGLE		(0.05f)									// 視野角の慣性
#define INERTIA_POSR			(0.6f)									// 注視点の慣性
#define INERTIA_POSV			(0.6f)									// 視点の慣性
#define MAX_Y					(300.0f)								// 上の制限
#define MIN_Y					(-300.0f)								// 下の制限

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Camera g_aCamera[MAX_CAMERA];			// カメラの情報
int g_nNumCamera = 1;					// カメラの数

//=============================================================================
// カメラの初期化処理
//=============================================================================
void InitCamera(void)
{
	// カメラ情報の初期化
	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++)
	{
		g_aCamera[nCntCamera].posV = CAMERA_POS;
		g_aCamera[nCntCamera].posR = FIRST_POS;
		g_aCamera[nCntCamera].posVDest = CAMERA_POS;
		g_aCamera[nCntCamera].posRDest = FIRST_POS;
		g_aCamera[nCntCamera].vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		g_aCamera[nCntCamera].rot = FIRST_POS;
		g_aCamera[nCntCamera].fAngle = 0.0f;
		g_aCamera[nCntCamera].fDistance = 300.0f;
		g_aCamera[nCntCamera].fRDistance = 0.0f;
		g_aCamera[nCntCamera].fViewAngle = DEFAULT_VIEW_ANGLE;
		g_aCamera[nCntCamera].fMoveVA = 0.0f;
		g_aCamera[nCntCamera].type = CAMERATYPE_POINT;
	}

	g_aCamera[0].viewport.X = 0;
	g_aCamera[0].viewport.Y = 0;
	g_aCamera[0].viewport.Width = SCREEN_WIDTH / 2;
	g_aCamera[0].viewport.Height = SCREEN_HEIGHT;
	g_aCamera[0].viewport.MinZ = 0.0f;
	g_aCamera[0].viewport.MaxZ = 1.0f;

	g_aCamera[1].viewport.X = SCREEN_WIDTH / 2;
	g_aCamera[1].viewport.Y = 0;
	g_aCamera[1].viewport.Width = SCREEN_WIDTH / 2;
	g_aCamera[1].viewport.Height = SCREEN_HEIGHT;
	g_aCamera[1].viewport.MinZ = 0.0f;
	g_aCamera[1].viewport.MaxZ = 1.0f;
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{
}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	Camera* pCamera = &g_aCamera[0];
	Player* pPlayer = GetPlayer();

	// それぞれのカメラの処理
	for (int nCntCamera = 0; nCntCamera < g_nNumCamera; nCntCamera++, pCamera++, pPlayer++)
	{
		switch (pCamera->type)
		{
		case CAMERATYPE_PLAYER:
			int nValueH, nValueV;

			// プレイヤー向きを調整
			CorrectAngle(&pPlayer->rot.y, ((pPlayer->rot.y - D3DX_PI) - pCamera->rot.y));

			// 目的の注視点を設定
			pCamera->posRDest.x = pPlayer->pos.x + sinf(D3DX_PI + pPlayer->fAngleY) * pCamera->fRDistance;
			pCamera->posRDest.y = pPlayer->pos.y + HEIGHT;
			pCamera->posRDest.z = pPlayer->pos.z + cosf(D3DX_PI + pPlayer->fAngleY) * pCamera->fRDistance;

			// 目的の視点を設定
			pCamera->posVDest.x = pCamera->posRDest.x + sinf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
			pCamera->posVDest.y = pCamera->posRDest.y + cosf((D3DX_PI * 0.5f) - pCamera->fAngle) * pCamera->fDistance;
			pCamera->posVDest.z = pCamera->posRDest.z + cosf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);

			if (GetKeyboardPress(DIK_Q) == true || GetJoypadStick(nCntCamera, JOYKEY_RIGHTSTICK_LEFT, &nValueH, &nValueV) == true)
			{// 右に旋回
				pCamera->rot.y += -ROT.y;

				pCamera->posVDest.x = pCamera->posRDest.x + sinf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
				pCamera->posVDest.y = pCamera->posRDest.y + cosf((D3DX_PI * 0.5f) - pCamera->fAngle) * pCamera->fDistance;
				pCamera->posVDest.z = pCamera->posRDest.z + cosf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
			}
			else if (GetKeyboardPress(DIK_E) == true || GetJoypadStick(nCntCamera, JOYKEY_RIGHTSTICK_RIGHT, &nValueH, &nValueV) == true)
			{// 左に旋回
				pCamera->rot.y += ROT.y;

				pCamera->posVDest.x = pCamera->posRDest.x + sinf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
				pCamera->posVDest.y = pCamera->posRDest.y + cosf((D3DX_PI * 0.5f) - pCamera->fAngle) * pCamera->fDistance;
				pCamera->posVDest.z = pCamera->posRDest.z + cosf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
			}

			// 視点移動
			if (GetKeyboardPress(DIK_B) == true || GetJoypadStick(nCntCamera, JOYKEY_RIGHTSTICK_DOWN, &nValueH, &nValueV) == true)
			{// 上に移動
				pCamera->fAngle += ROT.y;

				if (pCamera->fAngle > D3DX_PI * 0.4f)
				{// 上の制限
					pCamera->fAngle = D3DX_PI * 0.4f;
				}

				CorrectAngle(&pCamera->fAngle, pCamera->fAngle);

				pCamera->posVDest.x = pCamera->posRDest.x + sinf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
				pCamera->posVDest.y = pCamera->posRDest.y + cosf((D3DX_PI * 0.5f) - pCamera->fAngle) * pCamera->fDistance;
				pCamera->posVDest.z = pCamera->posRDest.z + cosf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
			}
			else if (GetKeyboardPress(DIK_T) == true || GetJoypadStick(nCntCamera, JOYKEY_RIGHTSTICK_UP, &nValueH, &nValueV) == true)
			{// 下に移動
				pCamera->fAngle += -ROT.y;

				if (pCamera->fAngle < -D3DX_PI * 0.4f)
				{// 下の制限
					pCamera->fAngle = -D3DX_PI * 0.4f;
				}

				CorrectAngle(&pCamera->fAngle, pCamera->fAngle);

				pCamera->posVDest.x = pCamera->posRDest.x + sinf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
				pCamera->posVDest.y = pCamera->posRDest.y + cosf((D3DX_PI * 0.5f) - pCamera->fAngle) * pCamera->fDistance;
				pCamera->posVDest.z = pCamera->posRDest.z + cosf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance * sinf((D3DX_PI * 0.5f) - pCamera->fAngle);
			}

			// カメラ向きを調整
			CorrectAngle(&pCamera->rot.y, pCamera->rot.y);

			// 注視点の補正
			pCamera->posR += (pCamera->posRDest - pCamera->posR) * INERTIA_POSR;

			// 視点の補正
			pCamera->posV += (pCamera->posVDest - pCamera->posV) * INERTIA_POSV;


#if 0
			// 並行移動
			if (GetKeyboardPress(DIK_W) == true)
			{// 奥に移動
				g_aCamera[0].posV.x += sinf(D3DX_PI * 0.0f + g_aCamera[0].rot.y) * MOVEMENT.x;
				g_aCamera[0].posV.z += cosf(D3DX_PI * 0.0f + g_aCamera[0].rot.y) * MOVEMENT.z;

				g_aCamera[0].posR.x = g_aCamera[0].posV.x + sinf(g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posR.z = g_aCamera[0].posV.z + cosf(g_aCamera[0].rot.y) * DISTANCE;
			}

			if (GetKeyboardPress(DIK_S) == true)
			{// 手前に移動
				g_aCamera[0].posV.x += sinf(D3DX_PI * 1.0f + g_aCamera[0].rot.y) * MOVEMENT.x;
				g_aCamera[0].posV.z += cosf(D3DX_PI * 1.0f + g_aCamera[0].rot.y) * MOVEMENT.z;

				g_aCamera[0].posR.x = g_aCamera[0].posV.x + sinf(g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posR.z = g_aCamera[0].posV.z + cosf(g_aCamera[0].rot.y) * DISTANCE;
			}

			if (GetKeyboardPress(DIK_A) == true)
			{// 左に移動
				g_aCamera[0].posV.x += sinf(-D3DX_PI * 0.5f + g_aCamera[0].rot.y) * MOVEMENT.x;
				g_aCamera[0].posV.z += cosf(-D3DX_PI * 0.5f + g_aCamera[0].rot.y) * MOVEMENT.z;

				g_aCamera[0].posR.x = g_aCamera[0].posV.x + sinf(g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posR.z = g_aCamera[0].posV.z + cosf(g_aCamera[0].rot.y) * DISTANCE;
			}

			if (GetKeyboardPress(DIK_D) == true)
			{// 右に移動
				g_aCamera[0].posV.x += sinf(D3DX_PI * 0.5f + g_aCamera[0].rot.y) * MOVEMENT.x;
				g_aCamera[0].posV.z += cosf(D3DX_PI * 0.5f + g_aCamera[0].rot.y) * MOVEMENT.z;

				g_aCamera[0].posR.x = g_aCamera[0].posV.x + sinf(g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posR.z = g_aCamera[0].posV.z + cosf(g_aCamera[0].rot.y) * DISTANCE;
			}

			// 注視点移動
			if (GetKeyboardPress(DIK_T) == true)
			{// 上に移動
				g_aCamera[0].posR.y += MOVEMENT.y;
			}
			else if (GetKeyboardPress(DIK_B) == true)
			{// 下に移動
				g_aCamera[0].posR.y += -MOVEMENT.y;
			}

			if (GetKeyboardPress(DIK_Q) == true)
			{// 左に旋回
				g_aCamera[0].rot.y += -ROT.y;

				g_aCamera[0].posR.x = g_aCamera[0].posV.x + sinf(g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posR.z = g_aCamera[0].posV.z + cosf(g_aCamera[0].rot.y) * DISTANCE;
			}
			else if (GetKeyboardPress(DIK_E) == true)
			{// 右に旋回
				g_aCamera[0].rot.y += ROT.y;

				g_aCamera[0].posR.x = g_aCamera[0].posV.x + sinf(g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posR.z = g_aCamera[0].posV.z + cosf(g_aCamera[0].rot.y) * DISTANCE;
			}

			// 視点移動
			if (GetKeyboardPress(DIK_Y) == true)
			{// 上に移動
				g_aCamera[0].posV.y += MOVEMENT.y;
			}
			else if (GetKeyboardPress(DIK_N) == true)
			{// 下に移動
				g_aCamera[0].posV.y += -MOVEMENT.y;
			}

			if (GetKeyboardPress(DIK_Z) == true)
			{// 左に旋回
				g_aCamera[0].rot.y += ROT.y;

				g_aCamera[0].posV.x = g_aCamera[0].posR.x + sinf(D3DX_PI + g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posV.z = g_aCamera[0].posR.z + cosf(D3DX_PI + g_aCamera[0].rot.y) * DISTANCE;
			}
			else if (GetKeyboardPress(DIK_C) == true)
			{// 右に旋回
				g_aCamera[0].rot.y += -ROT.y;

				g_aCamera[0].posV.x = g_aCamera[0].posR.x + sinf(D3DX_PI + g_aCamera[0].rot.y) * DISTANCE;
				g_aCamera[0].posV.z = g_aCamera[0].posR.z + cosf(D3DX_PI + g_aCamera[0].rot.y) * DISTANCE;
			}

			// 向きを調整
			if (g_aCamera[0].rot.y > D3DX_PI)
			{
				g_aCamera[0].rot.y -= D3DX_PI * 2;
			}
			else if (g_aCamera[0].rot.y < -D3DX_PI)
			{
				g_aCamera[0].rot.y += D3DX_PI * 2;
			}

			// リセット
			if (GetKeyboardPress(DIK_RETURN) == true)
			{
				g_aCamera[0].posV = FIRST_POS;
				g_aCamera[0].posR = CENTER;
				g_aCamera[0].posVDest = FIRST_POS;
				g_aCamera[0].posRDest = CENTER;
				g_aCamera[0].rot = CENTER;
			}

			Model* pModel = GetModel();

			// プレイヤー向きを調整
			CorrectAngle(&pModel->rot.y, ((pModel->rot.y - D3DX_PI) - g_aCamera[1].rot.y));

			// 目的の注視点を設定
			g_aCamera[1].posRDest.x = pModel->pos.x + sinf(D3DX_PI + pModel->rot.y) * g_aCamera[1].fRDistance;
			g_aCamera[1].posRDest.y = pModel->pos.y;
			g_aCamera[1].posRDest.z = pModel->pos.z + cosf(D3DX_PI + pModel->rot.y) * g_aCamera[1].fRDistance;

			// 目的の視点を設定
			g_aCamera[1].posVDest.x = pModel->pos.x + sinf(D3DX_PI + g_aCamera[1].rot.y) * g_aCamera[1].fDistance;
			g_aCamera[1].posVDest.z = pModel->pos.z + cosf(D3DX_PI + g_aCamera[1].rot.y) * g_aCamera[1].fDistance;

			// カメラ向きを調整
			CorrectAngle(&g_aCamera[1].rot.y, g_aCamera[1].rot.y);

			// 注視点の補正
			g_aCamera[1].posR += (g_aCamera[1].posRDest - g_aCamera[1].posR) * INERTIA_POSR;

			// 視点の補正
			g_aCamera[1].posV.x += (g_aCamera[1].posVDest.x - g_aCamera[1].posV.x) * INERTIA_POSV;
			g_aCamera[1].posV.y += (g_aCamera[1].posVDest.y - g_aCamera[1].posV.y) * INERTIA_POSV;
			g_aCamera[1].posV.z += (g_aCamera[1].posVDest.z - g_aCamera[1].posV.z) * INERTIA_POSV;
#endif
			break;

		case CAMERATYPE_POINT:
			pCamera->rot.y += -AUTO_ROT.y;

			// カメラ向きを調整
			CorrectAngle(&pCamera->rot.y, pCamera->rot.y);

			pCamera->posV.x = pCamera->posR.x + sinf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance;
			pCamera->posV.z = pCamera->posR.z + cosf(D3DX_PI + pCamera->rot.y) * pCamera->fDistance;

			break;
		}

		if (pCamera->fMoveVA != 0.0f)
		{// 視野角
			pCamera->fViewAngle += (pCamera->fMoveVA - pCamera->fViewAngle) * INERTIA_VIEW_ANGLE;
		}
	}
}


//=============================================================================
// カメラの設定処理
//=============================================================================
void SetCamera(int nIdx)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ビューポートの設定
	pDevice->SetViewport(&g_aCamera[nIdx].viewport);

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_aCamera[nIdx].mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&g_aCamera[nIdx].mtxProjection,
		D3DXToRadian(g_aCamera[nIdx].fViewAngle),								// 視野角
		(float)g_aCamera[nIdx].viewport.Width / (float)g_aCamera[nIdx].viewport.Height,			// 画面のアスペクト比
		10.0f,												// カメラから一番近い描画距離
		80000.0f);											// 最大描画距離

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &g_aCamera[nIdx].mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_aCamera[nIdx].mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(&g_aCamera[nIdx].mtxView,
		&g_aCamera[nIdx].posV,			// 視点
		&g_aCamera[nIdx].posR,			// 注視点
		&g_aCamera[nIdx].vecU);		// 上方向ベクトル

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &g_aCamera[nIdx].mtxView);
}

//=============================================================================
// カメラの数設定処理
//=============================================================================
void SetNumCamera(int nNum)
{
	g_nNumCamera = nNum;

	if (nNum == 1)
	{// 1人プレイ
		g_aCamera[0].viewport.X = 0;
		g_aCamera[0].viewport.Y = 0;
		g_aCamera[0].viewport.Width = SCREEN_WIDTH;
		g_aCamera[0].viewport.Height = SCREEN_HEIGHT;
		g_aCamera[0].viewport.MinZ = 0.0f;
		g_aCamera[0].viewport.MaxZ = 1.0f;
	}
	else if (nNum == 2)
	{// 2人プレイ
		g_aCamera[0].viewport.X = 0;
		g_aCamera[0].viewport.Y = 0;
		g_aCamera[0].viewport.Width = SCREEN_WIDTH / 2;
		g_aCamera[0].viewport.Height = SCREEN_HEIGHT;
		g_aCamera[0].viewport.MinZ = 0.0f;
		g_aCamera[0].viewport.MaxZ = 1.0f;

		g_aCamera[1].viewport.X = SCREEN_WIDTH / 2;
		g_aCamera[1].viewport.Y = 0;
		g_aCamera[1].viewport.Width = SCREEN_WIDTH / 2;
		g_aCamera[1].viewport.Height = SCREEN_HEIGHT;
		g_aCamera[1].viewport.MinZ = 0.0f;
		g_aCamera[1].viewport.MaxZ = 1.0f;
	}
}

//=============================================================================
// カメラの位置設定
//=============================================================================
void SetCameraPos(int nIdx, D3DXVECTOR3 posV, D3DXVECTOR3 posR, CAMERATYPE type)
{
	Camera* pCamera = GetCamera();

	pCamera[nIdx].posV = posV;
	pCamera[nIdx].posVDest = posV;
	pCamera[nIdx].posR = posR;
	pCamera[nIdx].posRDest = posR;
	pCamera[nIdx].type = type;
}

//=============================================================================
// カメラの視野角設定処理
//=============================================================================
void SetCameraViewAngle(int nIdx, float fViewAngle)
{
	Camera* pCamera = GetCamera();

	pCamera[nIdx].fMoveVA = fViewAngle;
}

//=============================================================================
// カメラの取得処理
//=============================================================================
Camera* GetCamera(void)
{
	return &g_aCamera[0];
}

//=============================================================================
// カメラの数取得処理
//=============================================================================
int GetNumCamera(void)
{
	return g_nNumCamera;
}