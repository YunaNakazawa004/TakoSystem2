//=============================================================================
// 
// ライト [light.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "light.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DLIGHT9 g_aLight[3];		// ライトの情報

//=============================================================================
// ライトの初期化処理
//=============================================================================
void InitLight(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXVECTOR3 vecDir;		// ライトの方向ベクトル

	// ライトの情報をクリアする
	ZeroMemory(&g_aLight[0], sizeof(D3DLIGHT9));
	ZeroMemory(&g_aLight[1], sizeof(D3DLIGHT9));
	ZeroMemory(&g_aLight[2], sizeof(D3DLIGHT9));

	// ライトの種類を設定
	g_aLight[0].Type = D3DLIGHT_DIRECTIONAL;
	g_aLight[1].Type = D3DLIGHT_DIRECTIONAL;
	g_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライトの拡散光を設定
	g_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	g_aLight[1].Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
	g_aLight[2].Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.3f);

	// ライトの方向を設定
	vecDir = D3DXVECTOR3(0.2f, -0.8f, -0.4f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	g_aLight[0].Direction = vecDir;

	vecDir = D3DXVECTOR3(-0.2f, 0.6f, 0.3f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	g_aLight[1].Direction = vecDir;

	vecDir = D3DXVECTOR3(0.0f, 0.8f, -0.2f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	g_aLight[2].Direction = vecDir;

	// ライトを設定する
	pDevice->SetLight(0, &g_aLight[0]);
	pDevice->SetLight(1, &g_aLight[1]);
	pDevice->SetLight(2, &g_aLight[2]);

	// ライトを有効にする
	pDevice->LightEnable(0, TRUE);
	pDevice->LightEnable(1, TRUE);
	pDevice->LightEnable(2, TRUE);
}

//=============================================================================
// ライトの終了処理
//=============================================================================
void UninitLight(void)
{
}

//=============================================================================
// ライトの更新処理
//=============================================================================
void UpdateLight(void)
{
}

//=============================================================================
// ライトの色設定処理
//=============================================================================
void SetLightColor(int nIdx, D3DXCOLOR col)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	g_aLight[nIdx].Diffuse = col;

	// ライトを設定する
	pDevice->SetLight(nIdx, &g_aLight[nIdx]);
}