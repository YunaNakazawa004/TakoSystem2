//=============================================================================
// 
// スポットライト [spotlight.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "spotlight.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DLIGHT9 g_aSpotLight[1];		// スポットライトの情報

//=============================================================================
// スポットライトの初期化処理
//=============================================================================
void InitSpotLight(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	D3DXVECTOR3 vecDir;		// ライトの方向ベクトル

	// スポットライトの情報をクリアする
	ZeroMemory(&g_aSpotLight[0], sizeof(D3DLIGHT9));

	// スポットライトの種類を設定
	g_aSpotLight[0].Type = D3DLIGHT_SPOT;

	// スポットライトの拡散光を設定
	g_aSpotLight[0].Diffuse = D3DXCOLOR(0.7f, 0.9f, 1.0f, 0.3f);

	// スポットライトの減衰率を設定
	g_aSpotLight[0].Attenuation0 = 0.0f;
	g_aSpotLight[0].Attenuation1 = 0.01f; // 1.0f / (距離) で減衰
	g_aSpotLight[0].Attenuation2 = 0.0f;

	// スポットライトの照射範囲を設定
	g_aSpotLight[0].Range = 5000.0f;

	// スポットライトの位置を設定
	g_aSpotLight[0].Position = D3DXVECTOR3(0.0f, 2000.0f, 0.0f);

	// スポットライトの方向を設定
	vecDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	D3DXVec3Normalize(&vecDir, &vecDir);		// ベクトルを正規化する
	g_aSpotLight[0].Direction = vecDir;

	// スポットライトの内側の円錐（明るい部分）の角度 (ラジアン)
	g_aSpotLight[0].Theta = D3DXToRadian(120.0f);

	// スポットライトの外側の円錐（光が減衰して消える部分）の角度 (ラジアン)
	g_aSpotLight[0].Phi = D3DXToRadian(123.0f);

	// スポットライトの光の減衰強度（0.0fが滑らか、1.0fがシャープ）
	g_aSpotLight[0].Falloff = 1.0f;

	// スポットライトを設定する
	pDevice->SetLight(3, &g_aSpotLight[0]);

	// スポットライトを有効にする
	pDevice->LightEnable(3, TRUE);
}

//=============================================================================
// スポットライトの終了処理
//=============================================================================
void UninitSpotLight(void)
{
}

//=============================================================================
// スポットライトの更新処理
//=============================================================================
void UpdateSpotLight(void)
{
}

//=============================================================================
// スポットライトの色設定処理
//=============================================================================
void SetSpotLightColor(int nIdx, D3DXCOLOR col)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	g_aSpotLight[nIdx].Diffuse = col;

	// スポットライトを設定する
	pDevice->SetLight(nIdx, &g_aSpotLight[nIdx]);
}