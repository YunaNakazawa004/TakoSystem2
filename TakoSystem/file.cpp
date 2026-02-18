//==================================================================================
// 
// ファイルに関する処理[file.cpp]
// Author : Soki Inamura 
// 
//==================================================================================
#include "main.h"
#include "file.h"

#include "input.h"

#define FILENAME_PASSLOG	"data/FILE/Debug_PassLog/log"
#define MAX_NUMSAVELOG		(100000)	// 1ファイルに記録出来る回数

// 
char g_aPassLogFileName[256] = {};		// ログに記録するファイルの名前
int g_nFileNumber = 0;					// 何番目のファイルか
unsigned int g_unNumFileSave = 0;		// 記録した回数
unsigned long g_ulTimeOld = 0;			// 前の時間

//========================================================================
// ファイルから文字だけの読み取り処理
//========================================================================
bool FileExtractText(FILE* pFile, char* pReadText)
{// <処理> 空欄、コメントを省いた文字を読み取る

	// 変数宣言 ===========================================

	char aReadText[256] = {};	// 読み取った文字
	char aBlank[256] = {};		// 空の読み取り

	int nCntPass = 0;			// 処理の通過(繰り返し)回数

	bool bSCRead;				// 読み取れたかの状態

	// ====================================================

	// ファイルから文字だけを読み取る
	while (1)
	{
		// 文字の読み取り
		fscanf(pFile, "%[^ \n\t]", &aReadText[0]);	// 文字でなくなる所まで読み取る

		if (aReadText[0] == '#')
		{// 最初の文字が「#」の場合

			fscanf(pFile, "%[^\n]", &aBlank[0]);	// 改行まで読み取る
		}
		else
		{// 最初の文字が「#」ではない

			if (aReadText[0] != '\0')
			{// 読み取った文字の一文字目が「\0」ではない場合

				if (pReadText)
				{
					// 読み取った文字を入れる
					strcpy(&pReadText[0], &aReadText[0]);
				}

				bSCRead = true;	// 読み取りに成功

				break;			// while文を抜ける
			}
		}

		// 空欄の読み取り
		fscanf(pFile, "%[ \t\n]", &aBlank[0]);		// 空欄でなくなる所まで読み取る

		nCntPass++;	// 通過回数をカウント

		if (nCntPass >= 50)
		{// 正常に読み取れていなさそうな場合

			bSCRead = false;	// 文字の読み取りに失敗

			break;				// while文を抜ける
		}
	}

	return bSCRead;
}

//========================================================================
// posの移動+保存処理
//========================================================================
void FileMovePosion(const char* pSaveFilename, D3DXVECTOR3* pPos, float fSpeed,		
					int nRightKey, int nLeftKey, int nUpKey, int nDownKey, int nForKey, int nBackKey)
{
	// キーが押された時にその方向に移動
	if (GetKeyboardPress(nRightKey)) pPos->x += fSpeed;		// 設定された「右キー」が押された時「+X」方向に移動　
	if (GetKeyboardPress(nLeftKey))  pPos->x -= fSpeed;		// 設定された「左キー」が押された時「-X」方向に移動
	if (GetKeyboardPress(nUpKey))	 pPos->y += fSpeed;		// 設定された「上キー」が押された時「+Y」方向に移動　
	if (GetKeyboardPress(nDownKey))	 pPos->y -= fSpeed;		// 設定された「下キー」が押された時「-Y」方向に移動　
	if (GetKeyboardPress(nForKey))	 pPos->z += fSpeed;		// 設定された「前キー」が押された時「+Z」方向に移動　
	if (GetKeyboardPress(nBackKey))	 pPos->z -= fSpeed;		// 設定された「後キー」が押された時「-Z」方向に移動

	if (GetKeyboardTrigger(DIK_F9))
	{// [F9]がおされた

		FILE* pFile;	// ファイルポインタ

		// ▼ファイルを開く
		pFile = fopen(pSaveFilename, "a");	// 追記用

		if (pFile == NULL)
		{// ファイルが開けなかった場合
			
			return;	// 処理を抜ける
		}

		// 位置の書き込み
		fprintf(pFile, "POS = %0.1f %0.1f %0.1f", pPos->x, pPos->y, pPos->z);

		// ▲ファイルを閉じる
		fclose(pFile);
	}
}

//========================================================================
// 通過ログの書き込み処理
//========================================================================
void FileLogPass(const char* pPassPointName)
{
	// 変数宣言 ===========================================

	FILE* pFile;									// ファイルポインタ

	unsigned long ulCurrentTime = timeGetTime();	// 現在の時刻

	// ====================================================

#if 1	// 記録するかしないか(1:しない, 2:する)

	return;	// ファイルにログを書き込まない
#endif

	// 記録上限
	if (g_unNumFileSave == 0)
	{// 記録出来る回数が0になった

		g_nFileNumber++;	// 書き込むファイル番号を増やす

		// ファイル番号を開くファイルにつなげる
		sprintf(&g_aPassLogFileName[0], "%s_%d.txt", FILENAME_PASSLOG, g_nFileNumber);

		g_unNumFileSave = MAX_NUMSAVELOG;	// 書き込める回数を設定
	}

	// ▼ファイルを開く
	pFile = fopen(g_aPassLogFileName, "a");	// 追記モード

	if (pFile == NULL)
	{// ファイルが開けなかった場合

		return;
	}

	// 情報を書き込む
	fprintf(pFile, "[%10s] %d\n", &pPassPointName[0], ulCurrentTime - g_ulTimeOld);

	g_unNumFileSave--;		// 書き込める回数を減らす

	// ▲ファイルを閉じる
	fclose(pFile);

	// 前の時刻を現在の時刻に設定
	g_ulTimeOld = ulCurrentTime;
}