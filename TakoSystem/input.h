//=============================================================================
// 
// 入力ヘッダー [input.h]
// Author : 中澤優奈
// 
//=============================================================================
#ifndef _INPUT_H_		// このマクロ定義がされていなかったら
#define _INPUT_H_		// 2重インクルード防止のマクロを定義する

#include "main.h"

//*****************************************************************************
// ジョイパッドのキーの種類
//*****************************************************************************
typedef enum
{
	JOYKEY_UP = 0,				// [0]十字キー(上)
	JOYKEY_DOWN,				// [1]十字キー(下)
	JOYKEY_LEFT,				// [2]十字キー(左)
	JOYKEY_RIGHT,				// [3]十字キー(右)
	JOYKEY_START,				// [4]START
	JOYKEY_BACK,				// [5]BACK
	JOYKEY_LEFT_THUMB,			// [6]左スティック押し込み
	JOYKEY_RIGHT_THUMB,			// [7]右スティック押し込み
	JOYKEY_LEFT_SHOULDER,		// [8]左肩キー
	JOYKEY_RIGHT_SHOULDER,		// [9]右肩キー
	JOYKEY_A = 12,				// [12]Aキー
	JOYKEY_B,					// [13]Bキー
	JOYKEY_X,					// [14]Xキー
	JOYKEY_Y,					// [15]Yキー
	JOYKEY_LEFTSTICK,			// [16]左スティックの入力
	JOYKEY_LEFTSTICK_LEFT,		// [17]左スティックの左入力
	JOYKEY_LEFTSTICK_RIGHT,		// [18]左スティックの右入力
	JOYKEY_LEFTSTICK_UP,		// [19]左スティックの上入力
	JOYKEY_LEFTSTICK_DOWN,		// [20]左スティックの下入力
	JOYKEY_RIGHTSTICK,			// [21]右スティックの入力
	JOYKEY_RIGHTSTICK_LEFT,		// [22]右スティックの左入力
	JOYKEY_RIGHTSTICK_RIGHT,	// [23]右スティックの右入力
	JOYKEY_RIGHTSTICK_UP,		// [24]右スティックの上入力
	JOYKEY_RIGHTSTICK_DOWN,		// [25]右スティックの下入力
	JOYKEY_LEFTTRIGGER,			// [26]左のトリガー
	JOYKEY_RIGHTTRIGGER,		// [27]右のトリガー
	JOYKEY_MAX
}JOYKEY;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);
void UninitKeyboard(void);
void UpdateKeyboard(void);
bool GetKeyboardPress(int nKey);
bool GetKeyboardTrigger(int nKey);
bool GetKeyboardRelease(int nKey);
bool GetKeyboardRepeat(int nKey);
HRESULT InitJoypad(void);
void UninitJoypad(void);
void UpdateJoypad(void);
bool GetJoypadPress(int nIdx, JOYKEY key);
bool GetJoypadTrigger(int nIdx, JOYKEY key);
bool GetJoypadRelease(int nIdx, JOYKEY key);
bool GetJoypadRepeat(int nIdx, JOYKEY key);
bool GetJoypadStick(int nIdx, JOYKEY key, int* pValueH, int* pValueV);
bool GetJoypadShoulder(int nIdx, JOYKEY key, int* pValue);
bool GetJoypadStickSlow(int nIdx);
void SetVibration(int nIdx, WORD wLeftVibSpeed, WORD wRightVibSpeed, int nVibCount);
void UpdateVibration(void);
void UninitVibration(void);

#endif