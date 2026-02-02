//=============================================================================
// 
// 3D [main.cpp]
// Author : 中澤優奈
// 
//=============================================================================
#include "main.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "debugproc.h"
#include "crosshair.h"
#include "sound.h"
#include "fade.h"
#include "title.h"
#include "tutorial.h"
#include "game.h"
#include "result.h"
#include "ranking.h"
#include "logo.h"
#include "meshcylinder.h"
#include "player.h"
#include "object.h"
#include "stage.h"
#include "esa.h"		// エサ
#include "time.h"
#include "fishes.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME			"WindowClass"			// ウィンドウクラスの名前
#define WINDOW_NAME			"TakoSystem"			// ウィンドウの名前

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3D9 g_pD3D = NULL;							// Direct3Dオブジェクトへのポインタ
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;				// Direct3Dデバイスへのポインタ
MODE g_mode = MODE_TITLE;							// 現在のモード
int g_nCountFPS = 0;								// FPSカウンタ
bool g_bWindowSize = TRUE;							// ウィンドウサイズ(TRUE : ウィンドウ FALSE : フルスクリーン)

//=============================================================================
// メイン関数
//=============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmdLine, int nCmdShow)
{
	// ローカル変数宣言
	DWORD dwCurrentTime;		// 現在時刻
	DWORD dwExecLastTime;		// 最後に処理した時刻
	DWORD dwFrameCount;			// フレームカウント
	DWORD dwFPSLastTime;		// 最後にFPSを計測した時刻

	RECT rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };		// 画面サイズの構造体

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),					// WNDCLASSEXのメモリサイズ
		CS_CLASSDC,							// ウィンドウのスタイル
		WindowProc,							// ウィンドウプロシージャ
		0,									// 0にする(通常は使用しない)
		0,
		hInstance,							// インスタンスハンドル
		LoadIcon(NULL, IDI_APPLICATION),	// タスクバーのアイコン
		LoadCursor(NULL, IDC_ARROW),		// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),			// クライアント領域の背景色
		NULL,								// メニューバー
		CLASS_NAME,							// ウィンドウクラスの名前
		LoadIcon(NULL, IDI_APPLICATION)		// ファイルのアイコン
	};

	HWND hWnd;			// ウィンドウハンドル(識別子)
	MSG msg;			// メッセージを格納する変数

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウィンドウを生成
	hWnd = CreateWindowEx(0,
		CLASS_NAME,				// ウィンドウクラスの名前
		WINDOW_NAME,				// ウィンドウの名前
		WS_OVERLAPPEDWINDOW,		// ウィンドウスタイル
		CW_USEDEFAULT,			// ウィンドウの左上X座標
		CW_USEDEFAULT,			// ウィンドウの左上Y座標
		(rect.right - rect.left),	// ウィンドウの幅
		(rect.bottom - rect.top),	// ウィンドウの高さ
		NULL,						// 親ウィンドウのハンドル
		NULL,						// メニューハンドルまたは子ウィンドウID
		hInstance,				// インスタンスハンドル
		NULL);					// ウィンドウ作成データ

	// 初期化処理
	if (FAILED(Init(hInstance, hWnd, g_bWindowSize)))
	{// 初期化処理が失敗した場合
		return -1;
	}

	// 分解能を設定
	timeBeginPeriod(1);
	dwCurrentTime = 0;					// 初期化する
	dwExecLastTime = timeGetTime();		// 現在時刻を取得(保存)

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	// ウィンドウの表示
	ShowWindow(hWnd, nCmdShow);			// ウィンドウの表示状態を設定
	UpdateWindow(hWnd);					// クライアント領域を更新

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{// Windowsの処理
			if (msg.message == WM_QUIT)
			{// WM_QUITメッセージを受け取ったらメッセージループを抜ける
				break;
			}

			else
			{// メッセージの設定
				TranslateMessage(&msg);		// 仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);		// ウィンドウプロシージャへメッセージを送出
			}
		}

		else
		{// DirectXの処理
			dwCurrentTime = timeGetTime();			// 現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5秒経過
				// FPSを計測
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
				dwFPSLastTime = dwCurrentTime;			// FPSを測定した時刻を保存
				dwFrameCount = 0;						// フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{// 60分の1秒経過
				dwExecLastTime = dwCurrentTime;		// 処理開始の時刻を保存

				// 更新処理
				Update();

				// 描画処理
				Draw();

				dwFrameCount++;
			}
		}
	}

	// 終了処理
	Uninit();

	// 分解能を戻す
	timeEndPeriod(1);

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

//=============================================================================
// ウィンドウプロシージャ
//=============================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID;		// 返り値を格納

	switch (uMsg)
	{
	case WM_DESTROY:		// ウィンドウ破棄のメッセージ
		// WM_QUITメッセージを送る
		PostQuitMessage(0);

		break;

	case WM_KEYDOWN:		// キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE:		// [ESC]キーが押された
			nID = MessageBox(hWnd, "終了しますか？", "終了確認", MB_YESNO | MB_ICONQUESTION);

			if (nID == IDYES)
			{// 終了する場合
				// ウィンドウ破棄する(WM_DESTROYメッセージを送る)
				DestroyWindow(hWnd);
			}

			break;
		}

		break;

	case WM_CLOSE:			// 閉じるボタン押下のメッセージ
		nID = MessageBox(hWnd, "終了しますか？", "終了確認", MB_YESNO | MB_ICONQUESTION);

		if (nID == IDYES)
		{// 終了する場合
			// ウィンドウ破棄する(WM_DESTROYメッセージを送る)
			DestroyWindow(hWnd);
		}

		else
		{// 終了しない場合
			return 0;		// 0を返さないと終了してしまう
		}

		break;

	case VK_F11:			// F11キーが押された

		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);		// 既定の処理を返す
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// Direct3Dオブジェクトの生成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));								// パラメータのゼロクリア
	d3dpp.BackBufferWidth = SCREEN_WIDTH;							// ゲーム画面サイズ(幅)
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;							// ゲーム画面サイズ(高さ)
	d3dpp.BackBufferFormat = d3ddm.Format;							// バックバッファの形式
	d3dpp.BackBufferCount = 1;										// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;						// ダブルバッファの切り替え(映像信号に動機)
	d3dpp.EnableAutoDepthStencil = TRUE;							// デプスバッファとステンシルバッファを作成
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;						// デプスバッファとして16bitを使う
	d3dpp.Windowed = bWindow;										// ウィンドウモード
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル

	// Direct3Dデバイスの生成
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pD3DDevice)))
	{
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&g_pD3DDevice)))
		{
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&g_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// サンプラーステートの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);			// テクスチャの拡縮補間の設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);			// テクスチャの繰り返しの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// キーボードの初期化処理
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// ジョイパッドの初期化処理
	if (FAILED(InitJoypad()))
	{
		return E_FAIL;
	}

	srand((unsigned int)time(NULL));

	// カメラの初期化処理
	InitCamera();
	SetNumCamera(MAX_PLAYER);

	// ライトの初期化処理
	InitLight();

	// デバッグ表示の初期化処理
	InitDebugProc();

	//// プレイヤーの初期化処理
	//InitPlayer();
	//SetPlayer(0, D3DXVECTOR3(0.0f, 10000.0f, 15000.0f), FIRST_POS);
	//SetPlayer(1, D3DXVECTOR3(0.0f, 15000.0f, -15000.0f), FIRST_POS);

	//// ステージの初期化処理
	//InitStage();

	//// 配置物の初期化処理
	//InitObject("objpos.txt");

	//// メッシュシリンダーの初期化処理
	//InitMeshCylinder();
	//SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(2000.0f, 17500.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), false);
	//SetMeshCylinder(FIRST_POS, FIRST_POS, D3DXVECTOR2(8.0f, 1.0f), D3DXVECTOR2(18050.0f, 17500.0f), D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f), true);

	//// 生き物の初期化処理
	//InitFishes();

	//// エサの初期化処理
	//InitEsa();

	//// クロスヘアの初期化処理
	//InitCrossHair();

	//// 時間の初期化処理
	//InitTime();

	//// 時間の初期設定
	//SetTime(DEFAULT_TIME);

	// サウンドの初期化
	InitSound(hWnd);

	// フェードの初期化
	InitFade(g_mode);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
#if 1
	// タイトル画面の終了処理
	UninitTitle();

	// チュートリアル画面の終了処理
	UninitTutorial();

	// ゲーム画面の終了処理
	UninitGame();

	// リザルト画面の終了処理
	UninitResult();

	// ランキング画面の終了処理
	UninitRanking();
#endif

	// キーボードの終了処理
	UninitKeyboard();

	// ジョイパッドの終了処理
	UninitJoypad();
	UninitVibration();

	// デバッグ表示の終了処理
	UninitDebugProc();

	//// プレイヤーの終了処理
	//UninitPlayer();

	//// ステージの終了処理
	//UninitStage();

	//// 配置物の終了処理
	//UninitObject();

	//// メッシュシリンダーの終了処理
	//UninitMeshCylinder();

	//// 生き物の終了処理
	//UninitFishes();

	//// エサの終了処理
	//UninitEsa();

	//// クロスヘアの終了処理
	//UninitCrossHair();

	//// 時間の終了処理
	//UninitTime();

	// サウンドの終了処理
	StopSound();
	UninitSound();

	// フェードの終了処理
	UninitFade();

	// カメラの終了処理
	UninitCamera();

	// ライトの終了処理
	UninitLight();

	// Direct3Dデバイスの破棄
	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	// Direct3Dオブジェクトの破棄
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// カメラの更新処理
	UpdateCamera();

	// ライトの更新処理
	UpdateLight();

	// デバッグ表示の更新処理
	UpdateDebugProc();
	PrintDebugProc("FPS : %d\n", g_nCountFPS);

	// キーボードの更新処理
	UpdateKeyboard();

	// ジョイパッドの更新処理
	UpdateJoypad();
	UpdateVibration();

	//// プレイヤーの更新処理
	//UpdatePlayer();

	//// ステージの更新処理
	//UpdateStage();

	//// 配置物の更新処理
	//UpdateObject();

	//// メッシュシリンダーの更新処理
	//UpdateMeshCylinder();

	//// 生き物の更新処理
	//UpdateFishes();

	//// エサの更新処理
	//UpdateEsa();

	//// クロスヘアの更新処理
	//UpdateCrossHair();

	//// 時間の更新処理
	//UpdateTime();

#if 1
	switch (g_mode)
	{
	case MODE_TITLE:			// タイトル画面
		UpdateTitle();
		break;

	case MODE_TUTORIAL:			// チュートリアル画面
		UpdateTutorial();
		break;

	case MODE_GAME:				// ゲーム画面
		UpdateGame();
		break;

	case MODE_RESULT:			// リザルト画面
		UpdateResult();
		break;

	case MODE_RANKING:			// ランキング画面
		UpdateRanking();
		break;

	case MODE_LOGO:				// ロゴ画面
		UpdateLogo();
		break;
	}

	// フェードの更新処理
	UpdateFade();
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	D3DVIEWPORT9 viewportDef;

	// 画面クリア(バックバッファとZバッファのクリア)
	g_pD3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// 描画開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{// 描画開始が成功した場合
		// 現在のビューポートを取得
		g_pD3DDevice->GetViewport(&viewportDef);

		for (int nCntCamera = 0; nCntCamera < GetNumCamera(); nCntCamera++)
		{
			// カメラの描画処理
			SetCamera(nCntCamera);

			//// プレイヤーの描画処理
			//DrawPlayer();

			//// ステージの描画処理
			//DrawStage();

			//// 配置物の描画処理
			//DrawObject();

			//// メッシュシリンダーの描画処理
			//DrawMeshCylinder();

			//// 生き物の描画処理
			//DrawFishes();

			//// エサの描画処理
			//DrawEsa();

			//// クロスヘアの描画処理
			//DrawCrossHair();

			//// 時間の描画処理
			//DrawTime();

			switch (g_mode)
			{
			case MODE_TITLE:			// タイトル画面
				DrawTitle();
				break;

			case MODE_TUTORIAL:			// チュートリアル画面
				DrawTutorial();
				break;

			case MODE_GAME:				// ゲーム画面
				DrawGame();
				break;

			case MODE_RESULT:			// リザルト画面
				DrawResult();
				break;

			case MODE_RANKING:			// ランキング画面
				DrawRanking();
				break;

			case MODE_LOGO:				// ロゴ画面
				DrawLogo();
				break;
			}
			
			// フェードの描画処理
			DrawFade();
		}

	

		// デバッグ表示の描画処理
		DrawDebugProc();

		// ビューポートを元に戻す
		g_pD3DDevice->SetViewport(&viewportDef);

		// 描画終了
		g_pD3DDevice->EndScene();
		}

	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
	}

//=============================================================================
// デバイス取得
//=============================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

//=============================================================================
// モードの設定
//=============================================================================
void SetMode(MODE mode)
{
#if 1
	// 現在の画面(モード)の終了処理
	switch (g_mode)
	{
	case MODE_TITLE:			// タイトル画面
		UninitTitle();

		break;

	case MODE_TUTORIAL:			// チュートリアル画面
		UninitTutorial();

		break;

	case MODE_GAME:				// ゲーム画面
		UninitGame();

		break;

	case MODE_RESULT:			// リザルト画面
		UninitResult();

		break;

	case MODE_RANKING:			// ランキング画面
		UninitRanking();

		break;

	case MODE_LOGO:				// ロゴ画面
		UninitLogo();

		break;
	}

	// 新しい画面(モード)の初期化処理
	switch (mode)
	{
	case MODE_TITLE:			// タイトル画面
		InitTitle();

		break;

	case MODE_TUTORIAL:			// チュートリアル画面
		InitTutorial();

		break;

	case MODE_GAME:				// ゲーム画面
		InitGame();

		break;

	case MODE_RESULT:			// リザルト画面
		InitResult();

		break;

	case MODE_RANKING:			// ランキング画面
		InitRanking();

		break;

	case MODE_LOGO:			// ロゴ画面
		InitLogo();

		break;
	}

	g_mode = mode;			// モードを保存する
#endif
}

//=============================================================================
// モードの取得
//=============================================================================
MODE GetMode(void)
{
	return g_mode;
}

//=============================================================================
// ワイヤーフレームON
//=============================================================================
void onWireFrame(void)
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
}
//=============================================================================
// ワイヤーフレームOFF
//=============================================================================
void offWireFrame(void)
{
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

//=============================================================================
// 角度を調整
//=============================================================================
void CorrectAngle(float* fAngle, float fAngleCmp)
{
	if (fAngleCmp > D3DX_PI)
	{
		*fAngle -= D3DX_PI * 2;
	}
	else if (fAngleCmp < -D3DX_PI)
	{
		*fAngle += D3DX_PI * 2;
	}
}

//=============================================================================
// フォグの設定
//=============================================================================
void SetFog(D3DXCOLOR col, float fFogStart, float fFogEnd)
{
	// フォグを有効にする
	g_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);

	// フォグカラー設定
	g_pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, col);

	// バーテックスフォグ(線形公式)を使用
	g_pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);		// D3DRS_FOGTABLEMODE (ピクセルフォグ) / D3DRS_FOGVERTEXMODE (バーテックスフォグ)

	// フォグ範囲設定
	g_pD3DDevice->SetRenderState(D3DRS_FOGSTART, *((LPDWORD)(&fFogStart)));
	g_pD3DDevice->SetRenderState(D3DRS_FOGEND, *((LPDWORD)(&fFogEnd)));

	// 範囲ベースのフォグを使用
	g_pD3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);
}