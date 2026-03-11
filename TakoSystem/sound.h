//=============================================================================
// サウンド処理 [sound.h]
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"
#include "esa.h"

//*****************************************************************************
// サウンド一覧
//*****************************************************************************
typedef enum
{
	SOUND_BGM_TITLE = 0,				//タイトルBGM
	SOUND_BGM_TUTORIAL,					//チュートリアルBGM
	SOUND_BGM_GAME,						//ゲームBGM
	SOUND_BGM_RESULT,					//リザルトBGM
	SOUND_BGM_RANKING,					//ランキングBGM
	SOUND_SE_SWIM,						//泳ぐ音
	SOUND_SE_TENTACLE_STRETCH,			//触手を伸ばす音
	SOUND_SE_TENTACLE_RETRACT,			//触手を引っ込める音
	SOUND_SE_INBAIT,					//エサを入れる音
	SOUND_SE_OUTBAIT,					//エサを取り出す音
	SOUND_SE_HIGHSPEED,					//高速移動音
	SOUND_SE_LANDING,					//着地音	
	SOUND_SE_WIN,						//勝利音
	SOUND_SE_COUNT,						//カウント音(残り時間10秒などに使用)
	SOUND_SE_COUNTDOWN,					//カウントダウン音(3.2.1.Goで始まるやつ)
	SOUND_SE_TIMEUP,					//タイムアップ音
	SOUND_SE_BOOT,						//起動音
	SOUND_SE_CURSORMOVE,				//カーソル移動音(ポーズメニューや人数選択時など)
	SOUND_SE_DECISION,					//決定音
	SOUND_SE_WARNING,					//警告音(渦潮発生時など)
	SOUND_SE_MUD,						//水音(べちゃっ)
	SOUND_SE_DRUMROLL,					//ドラムロール
	SOUND_SE_JAN,						//ジャン
	SOUND_SE_ESALOW,					//エサ低い
	SOUND_SE_ESANORMAL,					//エサ普通
	SOUND_SE_ESAHIGH,					//エサ高い
	SOUND_SE_FLOW,						//波
	SOUND_SE_POOL,						//渦潮
	SOUND_SE_STOLEN,					//盗まれた
	SOUND_SE_INKED,						//視界悪化
	SOUND_SE_CLEAR,						//クリア(チュートリアル)
	SOUND_MAX,
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);

void EsaPlaySE(int nType);

#endif
