//=============================================================================
// 
// デバッグ表示 [debugproc.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "debugproc.h"
#include "input.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPD3DXFONT g_pFont = NULL;				// フォントへのポインタ
char g_aStrDebug[4096];					// 文字列(デバッグ情報)を格納するバッファ
bool g_bDebug = true;					// デバッグ表示のON/OFF

//=============================================================================
// デバッグ表示の初期化処理
//=============================================================================
void InitDebugProc(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタ

	// デバッグ表示用フォントの生成
	D3DXCreateFont(pDevice, 18, 0, 0, 0, FALSE,
		SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH,
		"Terminal", &g_pFont);

	// デバッグ情報バッファのクリア
	memset(&g_aStrDebug[0], 0, sizeof g_aStrDebug);

	// デバッグ表示ON/OFFを初期化
#ifdef _DEBUG
	g_bDebug = true;
#else
	g_bDebug = false;
#endif
}

//=============================================================================
// デバッグ表示の終了処理
//=============================================================================
void UninitDebugProc(void)
{
	// デバッグ表示用フォントの破棄
	if (g_pFont != NULL)
	{
		g_pFont->Release();
		g_pFont = NULL;
	}
}

//=============================================================================
// デバッグ表示の更新処理
//=============================================================================
void UpdateDebugProc(void)
{
	if (GetKeyboardTrigger(DIK_F1) == true)
	{// デバッグ表示切り替え
		(g_bDebug == false) ? (g_bDebug = true) : (g_bDebug = false);
	}
}

//=============================================================================
// デバッグ表示の描画処理
//=============================================================================
void DrawDebugProc(void)
{
	RECT rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };

	if (g_bDebug == true)
	{// 表示するときだけ
		// テキストの描画
		g_pFont->DrawText(NULL, &g_aStrDebug[0], -1, &rect, DT_LEFT, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	// デバッグ情報バッファのクリア
	memset(&g_aStrDebug[0], 0, sizeof g_aStrDebug);
}

//=============================================================================
// デバッグ情報を登録
//=============================================================================
void PrintDebugProc(const char* fmt, ...)
{
	va_list ap;
	char aString[2048] = "\0";

	va_start(ap, fmt);

	for (const char* pChara = fmt; *pChara != '\0'; ++pChara)
	{
		if (*pChara == '%')
		{// 型変換
			pChara++;

			switch (*pChara)
			{
			case 'd':		// int
				sprintf(&aString[0], "%d", va_arg(ap, int));

				break;

			case 'f':		// double
				sprintf(&aString[0], "%.2f", va_arg(ap, double));

				break;

			case 'c':		// char
				sprintf(&aString[0], "%c", va_arg(ap, char));

				break;

			case 's':		// char*
				sprintf(&aString[0], "%s", va_arg(ap, const char*));

				break;
			}
		}
		else
		{// 文字列
			sprintf(&aString[0], "%c", *pChara);
		}

		strcat(&g_aStrDebug[0], &aString[0]);
	}

	va_end(ap);
}