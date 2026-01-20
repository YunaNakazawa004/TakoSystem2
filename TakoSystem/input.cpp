//=============================================================================
// 
// 入力処理 [input.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_KEY_MAX			(256)		// キーの最大数
#define REPEAT_MAX			(40)		// リピートまでの最大フレーム数
#define REPEAT_COUNT		(10)		// リピート間隔

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECTINPUT8 g_pInput = NULL;						// DirectInputオブジェクトへのポインタ
LPDIRECTINPUTDEVICE8 g_pDevKeyboard = NULL;			// 入力デバイス(キーボード)へのポインタ
BYTE g_aKeyState[NUM_KEY_MAX];						// キーボードのプレス情報
BYTE g_aKeyStateTrigger[NUM_KEY_MAX];				// キーボードのトリガー情報
BYTE g_aKeyStateRelease[NUM_KEY_MAX];				// キーボードのリリース情報
BYTE g_aKeyStateRepeat[NUM_KEY_MAX];				// キーボードのリピート情報
int g_aRepeatCounter[NUM_KEY_MAX];					// 各キーごとのリピートカウンター
int g_RepeatCount[NUM_KEY_MAX];						// 各キーごとのリピート回数
XINPUT_STATE g_joykeyState[MAX_PLAYER];				// ジョイパッドのプレス情報
XINPUT_STATE g_joykeyStateTrigger[MAX_PLAYER];		// ジョイパッドのトリガー情報
XINPUT_STATE g_joykeyStateRelease[MAX_PLAYER];		// ジョイパッドのトリガー情報
XINPUT_STATE g_joykeyStateRepeat[MAX_PLAYER];		// ジョイパッドのトリガー情報
XINPUT_VIBRATION g_joykeyVibration[MAX_PLAYER];		// ジョイパッドの振動情報
int g_joykeyRepeatCounter[MAX_PLAYER] = {};			// リピートカウンター
int g_joykeyRepeatCount[MAX_PLAYER] = {};			// リピート間隔のカウント
int g_joyStickSlowCount[MAX_PLAYER] = {};			// スロウ間隔のカウント
int g_EndVib[MAX_PLAYER] = {};						// バイブレーションのカウンター
bool g_bUseVib[MAX_PLAYER] = {};					// バイブレーションの使用

//=============================================================================
// キーボードの初期化処理
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd)
{
	// DirectInputオブジェクトの生成
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pInput, NULL)))
	{
		return E_FAIL;
	}

	// 入力デバイス(キーボード)の生成
	if (FAILED(g_pInput->CreateDevice(GUID_SysKeyboard, &g_pDevKeyboard, NULL)))
	{
		return E_FAIL;
	}

	// データフォーマットを設定
	if (FAILED(g_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// 協調モードを設定
	if (FAILED(g_pDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	// キーボードへのアクセス権を獲得
	g_pDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void UninitKeyboard(void)
{
	// 入力デバイス(キーボード)の破棄
	if (g_pDevKeyboard != NULL)
	{
		g_pDevKeyboard->Unacquire();			// キーボードへのアクセス権を破棄
		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}

	// DirectInputオブジェクトの破棄
	if (g_pInput != NULL)
	{
		g_pInput->Release();
		g_pInput = NULL;
	}
}

//=============================================================================
// キーボードの更新処理
//=============================================================================
void UpdateKeyboard(void)
{
	// ローカル変数宣言
	BYTE aKeyState[NUM_KEY_MAX];		// キーボードの入力情報
	int nCntKey;

	// 入力デバイスからデータを取得(ブレークポイントが効かない！！！)
	if (SUCCEEDED(g_pDevKeyboard->GetDeviceState(sizeof(aKeyState), &aKeyState[0])))
	{
		for (nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			g_aKeyStateTrigger[nCntKey] = ~g_aKeyState[nCntKey] & aKeyState[nCntKey];			// キーボードのトリガー情報を保存
			g_aKeyStateRelease[nCntKey] = g_aKeyState[nCntKey] & ~aKeyState[nCntKey];			// キーボードのリリース情報を保存
			g_aKeyState[nCntKey] = aKeyState[nCntKey];			// キーボードのプレス情報を保存

			if (GetKeyboardRelease(nCntKey))
			{
				g_aRepeatCounter[nCntKey] = 0;
				g_RepeatCount[nCntKey] = 0;
				g_aKeyStateRepeat[nCntKey] = 0;
			}
			else if (GetKeyboardPress(nCntKey))
			{
				if (g_aKeyStateTrigger[nCntKey])
				{
					g_aKeyStateRepeat[nCntKey] = 0x80; 
					g_aRepeatCounter[nCntKey] = 0;
					g_RepeatCount[nCntKey] = 0;
				}
				else
				{
					g_aRepeatCounter[nCntKey]++;

					if (g_aRepeatCounter[nCntKey] > REPEAT_MAX)
					{
						g_RepeatCount[nCntKey]++;

						if (g_RepeatCount[nCntKey] > REPEAT_COUNT)
						{
							g_RepeatCount[nCntKey] = 0;
							g_aKeyStateRepeat[nCntKey] = 0x80; 
						}
						else
						{
							g_aKeyStateRepeat[nCntKey] = 0;
						}
					}
					else
					{
						g_aKeyStateRepeat[nCntKey] = 0;
					}
				}
			}
			else
			{
				g_aKeyStateRepeat[nCntKey] = 0;
			}
		}
	}
	else
	{
		g_pDevKeyboard->Acquire();					// キーボードへのアクセス権を獲得
	}
}

//=============================================================================
// キーボードのプレス情報を取得
//=============================================================================
bool GetKeyboardPress(int nKey)
{
	return (g_aKeyState[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのトリガー情報を取得
//=============================================================================
bool GetKeyboardTrigger(int nKey)
{
	return (g_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリリース情報を取得
//=============================================================================
bool GetKeyboardRelease(int nKey)
{
	return (g_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリピート情報を取得
//=============================================================================
bool GetKeyboardRepeat(int nKey)
{
	return (g_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}

//=============================================================================
// ジョイパッドの初期化処理
//=============================================================================
HRESULT InitJoypad(void)
{
	// メモリのクリア
	memset(&g_joykeyState, 0, sizeof(XINPUT_STATE));
	memset(&g_joykeyStateTrigger, 0, sizeof(XINPUT_STATE));
	memset(&g_joykeyVibration, 0, sizeof(XINPUT_VIBRATION));

	// Xinputのステートを設定(有効にする)
	XInputEnable(true);

	return S_OK;
}

//=============================================================================
// ジョイパッドの終了処理
//=============================================================================
void UninitJoypad(void)
{
	// Xinputのステートを設定(無効にする)
	XInputEnable(false);
}

//=============================================================================
// ジョイパッドの更新処理
//=============================================================================
void UpdateJoypad(void)
{
	// ローカル変数宣言
	XINPUT_STATE joykeyState;		// ジョイパッドの入力情報

	for (int nCntJoypad = 0; nCntJoypad < MAX_PLAYER; nCntJoypad++)
	{
		// ジョイパッドの状態を取得
		if (XInputGetState(nCntJoypad, &joykeyState) == ERROR_SUCCESS)
		{
			g_joykeyStateTrigger[nCntJoypad].Gamepad.wButtons =
				(g_joykeyState[nCntJoypad].Gamepad.wButtons ^ joykeyState.Gamepad.wButtons)
				& joykeyState.Gamepad.wButtons;			// ジョイパッドのトリガー情報を保存

			g_joykeyStateRelease[nCntJoypad].Gamepad.wButtons =
				g_joykeyState[nCntJoypad].Gamepad.wButtons & ~joykeyState.Gamepad.wButtons;			// ジョイパッドのリリース情報を保存

			g_joykeyState[nCntJoypad] = joykeyState;									// ジョイパッドのプレス情報を保存

			if (g_joykeyState[nCntJoypad].Gamepad.wButtons != 0)
			{// ボタンが押されているとき
				g_joykeyStateRepeat[nCntJoypad].Gamepad.wButtons = g_joykeyStateTrigger[nCntJoypad].Gamepad.wButtons;

				g_joykeyRepeatCounter[nCntJoypad]++;
				g_joykeyRepeatCount[nCntJoypad]++;
			}

			if (g_joykeyRepeatCounter[nCntJoypad] > REPEAT_MAX)
			{// カウンターが一定の値を超えた
				if (g_joykeyRepeatCount[nCntJoypad] % REPEAT_COUNT == 0)
				{
					g_joykeyStateRepeat[nCntJoypad].Gamepad.wButtons = joykeyState.Gamepad.wButtons;			// ジョイパッドのリピート情報を保存
				}

				if (g_joykeyStateRelease[nCntJoypad].Gamepad.wButtons != 0)
				{
					g_joykeyRepeatCounter[nCntJoypad] = 0;
				}
			}

			g_joyStickSlowCount[nCntJoypad]++;
		}
	}
}

//=============================================================================
// ジョイパッドのプレス情報を取得
//=============================================================================
bool GetJoypadPress(int nIdx, JOYKEY key)
{
	return (g_joykeyState[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドのトリガー情報を取得
//=============================================================================
bool GetJoypadTrigger(int nIdx, JOYKEY key)
{
	return (g_joykeyStateTrigger[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドのリリース情報を取得
//=============================================================================
bool GetJoypadRelease(int nIdx, JOYKEY key)
{
	return (g_joykeyStateRelease[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドのリピート情報を取得
//=============================================================================
bool GetJoypadRepeat(int nIdx, JOYKEY key)
{
	return (g_joykeyStateRepeat[nIdx].Gamepad.wButtons & (0x01 << key)) ? true : false;
}

//=============================================================================
// ジョイパッドのスティックプレス情報を取得(true : 動く	false : 動かない)
//=============================================================================
bool GetJoypadStick(int nIdx, JOYKEY key, int *pValueH, int *pValueV)
{
	if (key == JOYKEY_LEFTSTICK)
	{// 左スティック
		if (pValueH != NULL)
		{// 水平方向
			*pValueH = g_joykeyState[nIdx].Gamepad.sThumbLX;
		}

		if (pValueV != NULL)
		{// 垂直方向
			*pValueV = g_joykeyState[nIdx].Gamepad.sThumbLY;
		}

		if (g_joykeyState[nIdx].Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			g_joykeyState[nIdx].Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			g_joykeyState[nIdx].Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ||
			g_joykeyState[nIdx].Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	if (key == JOYKEY_RIGHTSTICK)
	{// 右スティック
		if (pValueH != NULL)
		{// 水平方向
			*pValueH = g_joykeyState[nIdx].Gamepad.sThumbLX;
		}

		if (pValueV != NULL)
		{// 垂直方向
			*pValueV = g_joykeyState[nIdx].Gamepad.sThumbLY;
		}

		if (g_joykeyState[nIdx].Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			g_joykeyState[nIdx].Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			g_joykeyState[nIdx].Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE ||
			g_joykeyState[nIdx].Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	// ローカル変数宣言
	float fLX = g_joykeyState[nIdx].Gamepad.sThumbLX;
	float fLY = g_joykeyState[nIdx].Gamepad.sThumbLY;
	float fRX = g_joykeyState[nIdx].Gamepad.sThumbRX;
	float fRY = g_joykeyState[nIdx].Gamepad.sThumbRY;

	if (key == JOYKEY_LEFTSTICK_LEFT)
	{// 左スティックの左入力
		return (fLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LEFTSTICK_RIGHT)
	{// 左スティックの右入力
		return (fLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LEFTSTICK_UP)
	{// 左スティックの上入力
		return (fLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_LEFTSTICK_DOWN)
	{// 左スティックの下入力
		return (fLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) ? true : false;
	}

#if 1
	if (key == JOYKEY_RIGHTSTICK_LEFT)
	{// 右スティックの左入力
		return (fRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RIGHTSTICK_RIGHT)
	{// 右スティックの右入力
		return (fRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RIGHTSTICK_UP)
	{// 右スティックの上入力
		return (fRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
	if (key == JOYKEY_RIGHTSTICK_DOWN)
	{// 右スティックの下入力
		return (fRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) ? true : false;
	}
#endif

	return false;
}

//=============================================================================
// ジョイパッドの肩トリガー情報を取得(true : 動く	false : 動かない)
//=============================================================================
bool GetJoypadShoulder(int nIdx, JOYKEY key, int* pValue)
{
	if (key == JOYKEY_LEFTTRIGGER)
	{// 左トリガー
		if (pValue != NULL)
		{// 水平方向
			*pValue = g_joykeyState[nIdx].Gamepad.bLeftTrigger;
		}

		if (g_joykeyState[nIdx].Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	if (key == JOYKEY_RIGHTTRIGGER)
	{// 右トリガー
		if (pValue != NULL)
		{// 水平方向
			*pValue = g_joykeyState[nIdx].Gamepad.bRightTrigger;
		}

		if (g_joykeyState[nIdx].Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{// 入力がある
			return true;
		}
		else
		{// 入力がない
			return false;
		}
	}

	return false;
}

//=============================================================================
// ジョイパッドのスティックスロウ情報を取得(true : 動く	false : 動かない)
//=============================================================================
bool GetJoypadStickSlow(int nIdx)
{
	return ((g_joyStickSlowCount[nIdx] % REPEAT_COUNT) == 0) ? true : false;
}

//=============================================================================
// ジョイパッドの振動設定処理
//=============================================================================
void SetVibration(int nIdx, WORD wLeftVibSpeed, WORD wRightVibSpeed, int nVibCount)
{
	g_joykeyVibration[nIdx].wLeftMotorSpeed = wLeftVibSpeed;
	g_joykeyVibration[nIdx].wRightMotorSpeed = wRightVibSpeed;

	// 振動時間の設定
	g_EndVib[nIdx] = nVibCount;
	g_bUseVib[nIdx] = true;

	// 振動
	XInputSetState(0, &g_joykeyVibration[nIdx]);
}

//=============================================================================
// ジョイパッドの振動更新処理
//=============================================================================
void UpdateVibration(void)
{
	for (int nCntJoypad = 0; nCntJoypad < MAX_PLAYER; nCntJoypad++)
	{
		g_EndVib[nCntJoypad]--;

		if (g_bUseVib[nCntJoypad] == true && g_EndVib[nCntJoypad] < 0)
		{// 振動を終わる時になった
			// 振動を止める
			g_joykeyVibration[nCntJoypad] = {};
			XInputSetState(0, &g_joykeyVibration[nCntJoypad]);
			g_bUseVib[nCntJoypad] = false;
		}
	}
}

//=============================================================================
// ジョイパッドの振動終了処理
//=============================================================================
void UninitVibration(void)
{
	for (int nCntJoypad = 0; nCntJoypad < MAX_PLAYER; nCntJoypad++)
	{
		g_joykeyVibration[nCntJoypad] = {};
	}
}